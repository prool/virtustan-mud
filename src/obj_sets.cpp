// Copyright (c) 2014 Krodo
// Part of Bylins http://www.mud.ru

#include "conf.h"
#include <string>
#include <vector>
#include <map>
#include <array>
#include <algorithm>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
// GCC 4.4
#ifndef CYGWIN // ���� ���� ����������� � ��������� ������� boost, ����. � ������ 1.66.0. � ���� ��������� �� ���� ������ ��� ������ � ����� ������ � �������������� ���� ���� ������ ��� ����, � ��� ����� �����. /prool
#include <boost/tr1/unordered_map.hpp>
#endif

#include "obj_sets.hpp"
#include "obj_sets_stuff.hpp"
#include "structs.h"
#include "obj.hpp"
#include "db.h"
#include "pugixml.hpp"
#include "parse.hpp"
#include "constants.h"
#include "handler.h"
#include "char_player.hpp"
#include "skills.h"
#include "screen.h"
#include "modify.h"
#include "spells.h"
#include "help.hpp"

extern obj_rnum top_of_objt;

namespace obj_sets
{

int set_node::uid_cnt = 0;

const char *OBJ_SETS_FILE = LIB_MISC"obj_sets.xml";
/// ���/���� ���-�� ����������� ��� ��������� ����
const unsigned MIN_ACTIVE_SIZE = 2;
/// ������-�� ��������� ���-�� ���������� ��� ����� ������
const unsigned MAX_ACTIVE_SIZE = 10;
/// ������� ��������� ����� ���� � ����
const unsigned MAX_OBJ_LIST = 20;
/// �������� ������ �����
std::vector<boost::shared_ptr<set_node>> sets_list;
/// ��������� ��������� ���� �����, ������ � init_global_msg()
msg_node global_msg;

/// �������������� ������ � ���������� �� ������ �� ������� \param len,
/// ������ ����������� ������� ������� �� ����������� \param sep
/// \param base_offset = 0 - �������� ������ ������
std::string line_split_str(const std::string &str, const std::string &sep,
	size_t len, size_t base_offset)
{
	std::string out;
	out.reserve(str.size());
	const size_t sep_len = sep.length();
	size_t idx = 0, cur_line = base_offset, prev_idx = 0;

	while ((idx = str.find(sep, idx)) != std::string::npos)
	{
		const size_t tok_len = idx + sep_len - prev_idx;
		if (cur_line + tok_len > len)
		{
			out += "\r\n";
			cur_line = 0;
		}
		out += str.substr(prev_idx, tok_len);
		cur_line += tok_len;
		idx += sep_len;
		prev_idx = idx;
	}
	out += str.substr(prev_idx);

	return out;
}

/// \return ������ ���� ����� ���� ������ ��� ��������
size_t setidx_by_objvnum(int vnum)
{
	for (size_t i = 0; i < sets_list.size(); ++i)
	{
		if (sets_list.at(i)->obj_list.find(vnum) !=
			sets_list.at(i)->obj_list.end())
		{
			return i;
		}
	}
	return -1;
}

/// \return ������ ���� ����� ��� ��� (���)
size_t setidx_by_uid(int uid)
{
	for (size_t i = 0; i < sets_list.size(); ++i)
	{
		if (sets_list.at(i)->uid == uid)
		{
			return i;
		}
	}
	return -1;
}

/// �������� �������� �� ������� � ������ �����
/// \param set_uid - ����� �� ������� ���� �� ��� �� �����
bool is_duplicate(int set_uid, int vnum)
{
	for (size_t i = 0; i < sets_list.size(); ++i)
	{
		if (sets_list.at(i)->uid != set_uid
			&& sets_list.at(i)->obj_list.find(vnum)
				!= sets_list.at(i)->obj_list.end())
		{
			return true;
		}
	}
	return false;
}

void update_char_sets()
{
	for (CHAR_DATA* ch = character_list; ch; ch = ch->next)
	{
		if (!IS_NPC(ch) || IS_CHARMICE(ch))
		{
			ch->obj_bonus().update(ch);
		}
	}
}

/// ������ �������� ����� � ������ �������� ��������� (�� � OBJ_DATA)
/// ����� �� ����������� ������� �� ����������� �����
void init_obj_index()
{
	// obj vnum, obj_index idx
	// GCC 4.4
	//std::unordered_map<int, int> tmp;
	boost::unordered_map<int, int> tmp;
	tmp.reserve(top_of_objt + 1);

	for (int i = 0; i <= top_of_objt; ++i)
	{
		obj_index[i].set_idx = -1;
		tmp.emplace(obj_index[i].vnum, i);
	}

	for (size_t i = 0; i < sets_list.size(); ++i)
	{
		for (auto k = sets_list.at(i)->obj_list.begin();
			k != sets_list.at(i)->obj_list.end(); ++k)
		{
			auto m = tmp.find(k->first);
			if (m != tmp.end())
			{
				obj_index[m->second].set_idx = i;
			}
		}
	}
	HelpSystem::reload(HelpSystem::STATIC);
	update_char_sets();
}

/// ���� �� �������� ��: ������, ��������, ��������, ����
bool verify_wear_flag(OBJ_DATA *obj)
{
	if (CAN_WEAR(obj, ITEM_WEAR_FINGER)
		|| CAN_WEAR(obj, ITEM_WEAR_NECK)
		|| CAN_WEAR(obj, ITEM_WEAR_WRIST)
		|| GET_OBJ_TYPE(obj) == ITEM_LIGHT)
	{
		return false;
	}
	return true;
}

/// �������� ���� ����� ��� ��� ����� �� �������, ��� �������������
/// �������� ��� ��� ���������� (��� ��� ��� ������ ��� ���� �� ���������)
void verify_set(set_node &set)
{
	const size_t num = setidx_by_uid(set.uid) + 1;

	if (set.obj_list.size() < 2 || set.activ_list.size() < 1)
	{
		err_log("��� #%zu: incomplete (objs=%zu, activs=%zu)",
			num, set.obj_list.size(), set.activ_list.size());
		set.enabled = false;
	}
	if (set.obj_list.size() > MAX_OBJ_LIST)
	{
		err_log("��� #%zu: too many objects (size=%zu)",
			num, set.obj_list.size());
		set.enabled = false;
	}

	for (auto i = set.obj_list.begin(); i != set.obj_list.end(); ++i)
	{
		const int rnum = real_object(i->first);
		if (rnum < 0)
		{
			err_log("��� #%zu: empty obj proto (vnum=%d)", num, i->first);
			set.enabled = false;
		}
		else if (OBJ_FLAGGED(obj_proto[rnum], ITEM_SETSTUFF))
		{
			err_log("��� #%zu: obj have ITEM_SETSTUFF flag (vnum=%d)",
				num, i->first);
			set.enabled = false;
		}
		else if (!verify_wear_flag(obj_proto[rnum]))
		{
			err_log("��� #%zu: obj have invalid wear flag (vnum=%d)",
				num, i->first);
			set.enabled = false;
		}
		if (is_duplicate(set.uid, i->first))
		{
			err_log("��� #%zu: dublicate obj (vnum=%d)", num, i->first);
			set.enabled = false;
		}
	}

	std::bitset<NUM_CLASSES> prof_bits;
	bool prof_restrict = false;
	for (auto i = set.activ_list.begin(); i != set.activ_list.end(); ++i)
	{
		if (i->first < MIN_ACTIVE_SIZE || i->first > MAX_ACTIVE_SIZE)
		{
			err_log("��� #%zu: ������������ ������ ���������� (activ=%d)",
				num, i->first);
			set.enabled = false;
		}
		if (i->first > set.obj_list.size())
		{
			err_log("��� #%zu: ��������� ������ ������ ��������� (activ=%d)",
				num, i->first);
			set.enabled = false;
		}
		for (auto k = i->second.apply.begin(); k != i->second.apply.end(); ++k)
		{
			if (k->location < 0 || k->location >= NUM_APPLIES)
			{
				err_log(
					"��� #%zu: ������������ ����� apply ������� (loc=%d, mod=%d, activ=%d)",
					num, k->location, k->modifier, i->first);
				set.enabled = false;
			}
		}
		// ����� ��������� ����� � �����
		if (i->second.skill.first > MAX_SKILL_NUM
			|| i->second.skill.first < 0
			|| i->second.skill.second > 200
			|| i->second.skill.second < -200)
		{
			err_log(
				"��� #%zu: ������������ ����� ��� �������� ������ (num=%d, val=%d, activ=%d)",
				num, i->second.skill.first, i->first);
			set.enabled = false;
		}
		if (i->second.prof.none())
		{
			err_log("��� #%zu: ������ ������ ��������� ���������� (activ=%d)",
				num, i->first);
			set.enabled = false;
		}
		if (i->second.empty())
		{
			err_log("��� #%zu: ������ ��������� (activ=%d)", num, i->first);
			set.enabled = false;
		}
		if (i->second.prof.count() != i->second.prof.size())
		{
			if (!prof_restrict)
			{
				prof_bits = i->second.prof;
				prof_restrict = true;
			}
			else if (i->second.prof != prof_bits)
			{
				err_log(
					"��� #%zu: ������������ ������������� ������ ��������� ���������� (activ=%d)",
					num, i->first);
				set.enabled = false;
			}
		}
		if (i->second.bonus.phys_dmg < 0 || i->second.bonus.phys_dmg > 1000)
		{
			err_log("��� #%zu: ������������ ����� ���. ����� (activ=%d)",
				num, i->first);
			set.enabled = false;
		}
		if (i->second.bonus.mage_dmg < 0 || i->second.bonus.mage_dmg > 1000)
		{
			err_log("��� #%zu: ������������ ����� ���. ����� (activ=%d)",
				num, i->first);
			set.enabled = false;
		}

		if (i->second.enchant.first < 0)
		{
			err_log("��� #%zu: ������������ vnum �������� ��� ������� (vnum=%d, activ=%d)",
				num, i->second.enchant.first, i->first);
			set.enabled = false;
		}
		if (i->second.enchant.first > 0 &&
			set.obj_list.find(i->second.enchant.first) == set.obj_list.end())
		{
			err_log("��� #%zu: ������ ��� ��������, �� ����������� ������ ������ (vnum=%d, activ=%d)",
				num, i->second.enchant.first, i->first);
			set.enabled = false;
		}
		if (i->second.enchant.first > 0 && i->second.enchant.second.empty())
		{
			err_log("��� #%zu: ������ ������ ��� �������� (vnum=%d, activ=%d)",
				num, i->second.enchant.first, i->first);
			set.enabled = false;
		}
		if (i->second.enchant.second.weight < -100
			|| i->second.enchant.second.weight > 100)
		{
			err_log("��� #%zu: ������������ ��� ��� ������� (weight=%d, activ=%d)",
				num, i->second.enchant.second.weight, i->first);
			set.enabled = false;
		}
		if (i->second.enchant.second.ndice < -100
			|| i->second.enchant.second.ndice > 100
			|| i->second.enchant.second.sdice > 100
			|| i->second.enchant.second.sdice > 100)
		{
			err_log("��� #%zu: ������������ ������ ��� ������� (%dD%d, activ=%d)",
				num, i->second.enchant.second.ndice,
				i->second.enchant.second.sdice, i->first);
			set.enabled = false;
		}
	}
}

/// ������ ������ ���������� ��������� �� ������� - ���� ������� ���������
void init_global_msg()
{
	if (global_msg.char_on_msg.empty())
	{
		global_msg.char_on_msg = "&W$o0 ��������$U ������ �������.&n";
	}
	if (global_msg.char_off_msg.empty())
	{
		global_msg.char_off_msg = "&W������ $o1 �������� ������.&n";
	}
	if (global_msg.room_on_msg.empty())
	{
		global_msg.room_on_msg = "&W$o0 $n1 ��������$U ������ �������.&n";
	}
	if (global_msg.room_off_msg.empty())
	{
		global_msg.room_off_msg = "&W������ $o1 $n1 �������� ������.&n";
	}
}

/// ���� ��������� ��������� �� �������
/// ���������� �����-�� �� ����� (����) �� �������� �������
void init_msg_node(msg_node &node, const pugi::xml_node &xml_msg)
{
	node.char_on_msg = xml_msg.child_value("char_on_msg");
	node.char_off_msg = xml_msg.child_value("char_off_msg");
	node.room_on_msg = xml_msg.child_value("room_on_msg");
	node.room_off_msg = xml_msg.child_value("room_off_msg");
}

/// ���� ��� ������ ����, ������ ����� 'reload obj_sets.xml'
void load()
{
	log("Loadind %s: start", OBJ_SETS_FILE);
	sets_list.clear();
	init_global_msg();

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(OBJ_SETS_FILE);
	if (!result)
	{
		err_log("%s", result.description());
		return;
	}
    pugi::xml_node xml_obj_sets = doc.child("obj_sets");
    if (!xml_obj_sets)
    {
		err_log("<obj_sets> read fail");
		return;
    }
	// <messages>
	pugi::xml_node xml_msg = xml_obj_sets.child("messages");
	if (xml_msg)
	{
		init_msg_node(global_msg, xml_msg);
	}
	// <set>
	for (pugi::xml_node xml_set = xml_obj_sets.child("set"); xml_set;
		xml_set = xml_set.next_sibling("set"))
	{
		boost::shared_ptr<set_node> tmp_set = boost::make_shared<set_node>();
		// ���, ����� � ������ �� �����������
		tmp_set->name = xml_set.attribute("name").value();
		tmp_set->alias = xml_set.attribute("alias").value();
		tmp_set->comment = xml_set.attribute("comment").value();
		// enabled �� ����������, �� ������� ��� �������
		tmp_set->enabled =
			(xml_set.attribute("enabled").as_int(1) == 1 ? true : false);
		// <obj>
		for (pugi::xml_node xml_obj = xml_set.child("obj"); xml_obj;
			xml_obj = xml_obj.next_sibling("obj"))
		{
			// <messages>
			struct msg_node tmp_msg;
			pugi::xml_node xml_msg = xml_obj.child("messages");
			if (xml_msg)
			{
				init_msg_node(tmp_msg, xml_msg);
			}
			// GCC 4.4
			//tmp_set->obj_list.emplace(Parse::attr_int(xml_obj, "vnum"), tmp_msg);
			tmp_set->obj_list.insert(std::make_pair(Parse::attr_int(xml_obj, "vnum"), tmp_msg));
		}
		// <activ>
		for (pugi::xml_node xml_activ = xml_set.child("activ"); xml_activ;
			xml_activ = xml_activ.next_sibling("activ"))
		{
			activ_node tmp_activ;
			// <affects>
			asciiflag_conv(xml_activ.child_value("affects"), &tmp_activ.affects);
			// <apply>
			for (pugi::xml_node xml_apply = xml_activ.child("apply"); xml_apply;
				xml_apply = xml_apply.next_sibling("apply"))
			{
				// ����������� ������ ������ MAX_OBJ_AFFECT
				for (auto i = tmp_activ.apply.begin();
					i != tmp_activ.apply.end(); ++i)
				{
					if (i->location <= 0)
					{
						i->location = Parse::attr_int(xml_apply, "loc");
						i->modifier = Parse::attr_int(xml_apply, "mod");
						break;
					}
				}
			}
			// <skill>
			pugi::xml_node xml_cur = xml_activ.child("skill");
			if (xml_cur)
			{
				tmp_activ.skill.first = Parse::attr_int(xml_cur, "num");
				tmp_activ.skill.second = Parse::attr_int(xml_cur, "val");
			}
			// <enchant>
			xml_cur = xml_activ.child("enchant");
			if (xml_cur)
			{
				tmp_activ.enchant.first = Parse::attr_int(xml_cur, "vnum");
				tmp_activ.enchant.second.weight =
					xml_cur.attribute("weight").as_int(0);
				tmp_activ.enchant.second.ndice =
					xml_cur.attribute("ndice").as_int(0);
				tmp_activ.enchant.second.sdice =
					xml_cur.attribute("sdice").as_int(0);
			}
			// <phys_dmg>
			xml_cur = xml_activ.child("phys_dmg");
			if (xml_cur)
			{
				tmp_activ.bonus.phys_dmg = Parse::attr_int(xml_cur, "pct");
			}
			// <mage_dmg>
			xml_cur = xml_activ.child("mage_dmg");
			if (xml_cur)
			{
				tmp_activ.bonus.mage_dmg = Parse::attr_int(xml_cur, "pct");
			}
			// ���� ��� �������� prof - ������ ����� �� ��� �����
			pugi::xml_attribute xml_prof = xml_activ.attribute("prof");
			if (xml_prof)
			{
				std::bitset<NUM_CLASSES> tmp_p(std::string(xml_prof.value()));
				tmp_activ.prof = tmp_p;
			}
			tmp_set->activ_list[Parse::attr_int(xml_activ, "size")] = tmp_activ;
		}
		// <messages>
		pugi::xml_node xml_msg = xml_set.child("messages");
		if (xml_msg)
		{
			init_msg_node(tmp_set->messages, xml_msg);
		}
		sets_list.push_back(tmp_set);
		verify_set(*tmp_set);
	}

	init_obj_index();
	log("Loadind %s: done", OBJ_SETS_FILE);
	save();
}

/// ���������� ��������� ��������� � ������
void save_messages(pugi::xml_node &xml, msg_node &msg)
{
	if (!msg.char_on_msg.empty()
		|| !msg.char_off_msg.empty()
		|| !msg.room_on_msg.empty()
		|| !msg.room_off_msg.empty())
	{
		xml.append_child("messages");
	}
	else
	{
		return;
	}

	pugi::xml_node xml_messages = xml.last_child();
	if (!msg.char_on_msg.empty())
	{
		pugi::xml_node xml_msg = xml_messages.append_child("char_on_msg");
		xml_msg.append_child(pugi::node_pcdata).set_value(msg.char_on_msg.c_str());
	}
	if (!msg.char_off_msg.empty())
	{
		pugi::xml_node xml_msg = xml_messages.append_child("char_off_msg");
		xml_msg.append_child(pugi::node_pcdata).set_value(msg.char_off_msg.c_str());
	}
	if (!msg.room_on_msg.empty())
	{
		pugi::xml_node xml_msg = xml_messages.append_child("room_on_msg");
		xml_msg.append_child(pugi::node_pcdata).set_value(msg.room_on_msg.c_str());
	}
	if (!msg.room_off_msg.empty())
	{
		pugi::xml_node xml_msg = xml_messages.append_child("room_off_msg");
		xml_msg.append_child(pugi::node_pcdata).set_value(msg.room_off_msg.c_str());
	}
}

/// ���������� �������, ������� � �����������, � ������ ����
void save()
{
	log("Saving %s: start", OBJ_SETS_FILE);
	char buf_[256];
	pugi::xml_document doc;
	pugi::xml_node xml_obj_sets = doc.append_child("obj_sets");
	// obj_sets/messages
	save_messages(xml_obj_sets, global_msg);

	for (auto i = sets_list.begin(); i != sets_list.end(); ++i)
	{
		// obj_sets/set
		pugi::xml_node xml_set = xml_obj_sets.append_child("set");
		if (!(*i)->name.empty())
		{
			xml_set.append_attribute("name") = (*i)->name.c_str();
		}
		if (!(*i)->alias.empty())
		{
			xml_set.append_attribute("alias") = (*i)->alias.c_str();
		}
		if (!(*i)->comment.empty())
		{
			xml_set.append_attribute("comment") = (*i)->comment.c_str();
		}
		if (!(*i)->enabled)
		{
			xml_set.append_attribute("enabled") = 0;
		}
		// set/messages
		save_messages(xml_set, (*i)->messages);
		// set/obj
		for (auto o = (*i)->obj_list.begin(); o != (*i)->obj_list.end(); ++o)
		{
			pugi::xml_node xml_obj = xml_set.append_child("obj");
			xml_obj.append_attribute("vnum") = o->first;
			save_messages(xml_obj, o->second);
		}
		// set/activ
		for (auto k = (*i)->activ_list.begin(); k != (*i)->activ_list.end(); ++k)
		{
			pugi::xml_node xml_activ = xml_set.append_child("activ");
			xml_activ.append_attribute("size") = k->first;
			if (k->second.prof.count() != k->second.prof.size()) // !all()
			{
				xml_activ.append_attribute("prof")
					= k->second.prof.to_string().c_str();
			}
			// set/activ/affects
			if (!k->second.affects.empty())
			{
				pugi::xml_node xml_affects = xml_activ.append_child("affects");
				*buf_ = '\0';
				tascii(&GET_FLAG(k->second.affects, 0), 4, buf_);
				xml_affects.append_child(pugi::node_pcdata).set_value(buf_);
			}
			// set/activ/apply
			for (auto m = k->second.apply.begin(); m != k->second.apply.end(); ++m)
			{
				if (m->location > 0 && m->location < NUM_APPLIES && m->modifier)
				{
					pugi::xml_node xml_apply = xml_activ.append_child("apply");
					xml_apply.append_attribute("loc") = m->location;
					xml_apply.append_attribute("mod") = m->modifier;
				}
			}
			// set/activ/skill
			if (k->second.skill.first > 0)
			{
				pugi::xml_node xml_skill = xml_activ.append_child("skill");
				xml_skill.append_attribute("num") = k->second.skill.first;
				xml_skill.append_attribute("val") = k->second.skill.second;
			}
			// set/activ/enchant
			if (k->second.enchant.first > 0)
			{
				pugi::xml_node xml_enchant = xml_activ.append_child("enchant");
				xml_enchant.append_attribute("vnum") = k->second.enchant.first;
				if (k->second.enchant.second.weight > 0)
				{
					xml_enchant.append_attribute("weight") =
						k->second.enchant.second.weight;
				}
				if (k->second.enchant.second.ndice > 0)
				{
					xml_enchant.append_attribute("ndice") =
						k->second.enchant.second.ndice;
				}
				if (k->second.enchant.second.sdice)
				{
					xml_enchant.append_attribute("sdice") =
						k->second.enchant.second.sdice;
				}
			}
			// set/activ/phys_dmg
			if (k->second.bonus.phys_dmg > 0)
			{
				pugi::xml_node xml_bonus = xml_activ.append_child("phys_dmg");
				xml_bonus.append_attribute("pct") = k->second.bonus.phys_dmg;
			}
			// set/activ/mage_dmg
			if (k->second.bonus.mage_dmg > 0)
			{
				pugi::xml_node xml_bonus = xml_activ.append_child("mage_dmg");
				xml_bonus.append_attribute("pct") = k->second.bonus.mage_dmg;
			}
		}
	}

	pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "koi8-r";
	doc.save_file(OBJ_SETS_FILE);
	log("Saving %s: done", OBJ_SETS_FILE);
}

/// ���������� ��������� ���� � � �������
/// \param activated - �������� ��������� ��� �����������
/// ��������� ������� �� ������� ���������� � �������:
/// ������� -> ����� -> ���������� ���������
void print_msg(CHAR_DATA *ch, OBJ_DATA *obj, size_t set_idx, bool activated)
{
	if (set_idx >= sets_list.size()) return;

	const char *char_on_msg = 0;
	const char *room_on_msg = 0;
	const char *char_off_msg = 0;
	const char *room_off_msg = 0;

	boost::shared_ptr<set_node> &curr_set = sets_list.at(set_idx);
	auto i = curr_set->obj_list.find(GET_OBJ_VNUM(obj));
	if (i != curr_set->obj_list.end())
	{
		// ��������� � ��������
		if (!i->second.char_on_msg.empty())
			char_on_msg = i->second.char_on_msg.c_str();
		if (!i->second.room_on_msg.empty())
			room_on_msg = i->second.room_on_msg.c_str();
		if (!i->second.char_off_msg.empty())
			char_off_msg = i->second.char_off_msg.c_str();
		if (!i->second.room_off_msg.empty())
			room_off_msg = i->second.room_off_msg.c_str();
	}
	// ��������� � ����
	if (!char_on_msg && !curr_set->messages.char_on_msg.empty())
		char_on_msg = curr_set->messages.char_on_msg.c_str();
	if (!room_on_msg && !curr_set->messages.room_on_msg.empty())
		room_on_msg = curr_set->messages.room_on_msg.c_str();
	if (!char_off_msg && !curr_set->messages.char_off_msg.empty())
		char_off_msg = curr_set->messages.char_off_msg.c_str();
	if (!room_off_msg && !curr_set->messages.room_off_msg.empty())
		room_off_msg = curr_set->messages.room_off_msg.c_str();
	// ���������� ���������
	if (!char_on_msg)
		char_on_msg = global_msg.char_on_msg.c_str();
	if (!room_on_msg)
		room_on_msg = global_msg.room_on_msg.c_str();
	if (!char_off_msg)
		char_off_msg = global_msg.char_off_msg.c_str();
	if (!room_off_msg)
		room_off_msg = global_msg.room_off_msg.c_str();
	// ���-�� � ����� ������ �����������
	if (activated)
	{
		act(char_on_msg, FALSE, ch, obj, 0, TO_CHAR);
		act(room_on_msg, TRUE, ch, obj, 0, TO_ROOM);
	}
	else
	{
		act(char_off_msg, FALSE, ch, obj, 0, TO_CHAR);
		act(room_off_msg, TRUE, ch, obj, 0, TO_ROOM);
	}
}

/// ��������� ����������� ��������
void print_off_msg(CHAR_DATA *ch, OBJ_DATA *obj)
{
	const int set_idx = GET_OBJ_RNUM(obj) >= 0
		? obj_index[GET_OBJ_RNUM(obj)].set_idx : -1;
	if (set_idx >= 0)
	{
		obj_sets::print_msg(ch, obj, set_idx, false);
	}
}

/// ������� ����� ���� �� �����������, �� �� ���� ����� ���������
/// ��� ���������� �� ������ ��������� ������� ����, �.�. �������� ������
/// 5 ���������, � �������� ������� ��������� �� 4 ���������, ���� ��������
/// ��� �����, ����� � ������ ������ ������ �������� �� ������� �������
/// ������������ ��� ��������, � ������ ���� ���������� ��������� ���������
/// �.�. ����� ������� � affect_total() � ���� ������� ������ � get_activator()
/// ���������� �� ������ ���� � ���, � ������ ��� ��� ���, �� � ������
/// ������������� ����������� � ������ ������ ���������� � �� ����
void check_activated(CHAR_DATA *ch, int activ, idx_node &node)
{
	int need_msg = activ - node.activated_cnt;
	for (auto i = node.obj_list.begin(); i != node.obj_list.end(); ++i)
	{
		OBJ_DATA *obj = *i;
		if (need_msg > 0 && !obj->get_activator().first)
		{
			if (obj->get_activator().second < activ)
			{
				print_msg(ch, obj, node.set_idx, true);
			}
			obj->set_activator(true, activ);
			++node.activated_cnt;
			--need_msg;
		}
		else
		{
			obj->set_activator(obj->get_activator().first,
				std::max(activ, obj->get_activator().second));
		}
	}
}

/// ��������� ����� �������� ����������� ����, ����� ������ ������, �������
/// ��������� �������� � ����������� �� ��������� � �����������
void check_deactivated(CHAR_DATA *ch, int max_activ, idx_node &node)
{
	int need_msg = node.activated_cnt - max_activ;
	for (auto i = node.obj_list.begin(); i != node.obj_list.end(); ++i)
	{
		OBJ_DATA *obj = *i;
		if (need_msg > 0 && obj->get_activator().first)
		{
			print_msg(ch, obj, node.set_idx, false);
			obj->set_activator(false, max_activ);
			--node.activated_cnt;
			--need_msg;
		}
		else
		{
			obj->set_activator(obj->get_activator().first, max_activ);
		}
	}
}

/// ���������� ��������� ������ � ��� ������� �� ����� �������������� �
/// ������ �� ���, ��� �������� ����� �� ������ ��� ���������
std::string print_obj_list(const set_node &set)
{
	char buf_[128];
	std::string out;
	std::vector<int> rnum_list;
	size_t r_max_name = 0, l_max_name = 0;
	bool left = true;

	for (auto i = set.obj_list.begin(); i != set.obj_list.end(); ++i)
	{
		const int rnum = real_object(i->first);
		if (rnum < 0 || !obj_proto[rnum]->short_description) continue;

		const size_t curr_name =
			strlen_no_colors(obj_proto[rnum]->short_description);
		if (left)
		{
			l_max_name = std::max(l_max_name, curr_name);
		}
		else
		{
			r_max_name = std::max(r_max_name, curr_name);
		}
		rnum_list.push_back(rnum);
		left = !left;
	}

	left = true;
	for (auto i = rnum_list.begin(); i != rnum_list.end(); ++i)
	{
		snprintf(buf_, sizeof(buf_), "   %s",
			colored_name(obj_proto[*i]->short_description,
			left ? l_max_name : r_max_name, true));
		out += buf_;
		if (!left)
		{
			out += "\r\n";
		}
		left = !left;
	}

	boost::trim_right(out);
	return out + "\r\n";
}

/// ��������� �������� ��������
void print_identify(CHAR_DATA *ch, const OBJ_DATA *obj)
{
	const size_t set_idx = GET_OBJ_RNUM(obj) >= 0
		? obj_index[GET_OBJ_RNUM(obj)].set_idx : sets_list.size();
	if (set_idx < sets_list.size())
	{
		const set_node &cur_set = *(sets_list.at(set_idx));
		if (!cur_set.enabled) return;

		std::string out;
		char buf_[256], buf_2[128];

		snprintf(buf_, sizeof(buf_), "%s����� ������ ���������: %s%s%s\r\n",
			CCNRM(ch, C_NRM), CCWHT(ch, C_NRM),
			cur_set.name.c_str(), CCNRM(ch, C_NRM));
		out += buf_;
		out += print_obj_list(cur_set);

		auto i = obj->get_activator();
		if (i.second > 0)
		{
			snprintf(buf_2, sizeof(buf_2), " (������� %d %s)",
				i.second, desc_count(i.second, WHAT_OBJECT));
		}

		snprintf(buf_, sizeof(buf_),
			"�������� ������%s: %s������� %s%s\r\n",
			(i.second > 0 ? buf_2 : ""),
			CCWHT(ch, C_NRM), cur_set.help.c_str(), CCNRM(ch, C_NRM));
		out += buf_;

		send_to_char(out, ch);
	}
}

/// ������ ����� ���� � ����������� �������, �� ������� ��������� sed � �������
void do_slist(CHAR_DATA *ch)
{
	std::string out("������������������ ������ ���������:\r\n");
	char buf_[256];

	int idx = 1;
	for (auto i = sets_list.begin(); i != sets_list.end(); ++i)
	{
		char comment[128];
		snprintf(comment, sizeof(comment), " (%s)", (*i)->comment.c_str());
		char status[64];
		snprintf(status, sizeof(status), "������: %s��������%s, ",
			CCICYN(ch, C_NRM), CCNRM(ch, C_NRM));

		snprintf(buf_, sizeof(buf_),
			"%3d) %s%s\r\n"
			"     %s��������: ", idx++,
			((*i)->name.empty() ? "���������� �����" : (*i)->name.c_str()),
			((*i)->comment.empty() ? "" : comment),
			((*i)->enabled ? "" : status));
		out += buf_;
		for (auto o = (*i)->obj_list.begin(); o !=  (*i)->obj_list.end(); ++o)
		{
			out += boost::lexical_cast<std::string>(o->first) + " ";
		}
		out += "\r\n";
	}
	page_string(ch->desc, out);
}

/// ���������� �������� ���������� ��� ������� � �������������� �� 80 ��������
std::string print_activ_affects(const FLAG_DATA &aff)
{
	char buf_[2048];
	if (sprintbits(aff, weapon_affects, buf_, ","))
	{
		// ���� ���� ������, ����� ������� ������� � ��������� �� ������
		// �� 80 �������� (�� �������� �����), ��� ���� ��������� �������
		// ������ ������ " + " � �������� ���� ������� ������
		std::string aff_str(" + ������� :\r\n");
		aff_str += line_split_str(buf_, ",", 74, 0);
		boost::trim_right(aff_str);
		char filler[64];
		snprintf(filler, sizeof(filler), "\n%s +    %s", KNRM, KCYN);
		boost::replace_all(aff_str, "\n", filler);
		aff_str += KNRM;
		aff_str += "\r\n";

		return aff_str;
	}
	return "";
}

/// ��� ���������� apply ��������, ������� ����� ���� ��� � std::array, ��� �
/// � std::vector, ���� ������ ��� ��������� ���� � ����
template <class T>
std::string print_activ_apply(const T &list)
{
	std::string out;
	for (auto i = list.begin(); i != list.end(); ++i)
	{
		if (i->location > 0)
		{
			out += " +    " + print_obj_affects(*i);
		}
	}
	return out;
}

/// ���������� ���������� ������� ������� � ���������� ��� ����� �����������
std::string print_activ_bonus(const bonus_type &bonus)
{
	std::string out;
	char buf_[128];

	if (bonus.phys_dmg > 0)
	{
		snprintf(buf_, sizeof(buf_),
			" +    %s����������� ���. ���� �� %d%%%s\r\n",
			KCYN, bonus.phys_dmg, KNRM);
		out += buf_;
	}
	if (bonus.mage_dmg > 0)
	{
		snprintf(buf_, sizeof(buf_),
			" +    %s����������� ���. ���� �� %d%%%s\r\n",
			KCYN, bonus.mage_dmg, KNRM);
		out += buf_;
	}

	return out;
}

std::string print_activ_enchant(const std::pair<int, ench_type> &ench)
{
	std::string out;
	char buf_[128];

	if (ench.first > 0)
	{
		int rnum = real_object(ench.first);
		if (rnum < 0) return "";

		if (ench.second.weight != 0)
		{
			snprintf(buf_, sizeof(buf_),
				" +    %s%s ��� %s �� %d%s\r\n",
				KCYN, ench.second.weight > 0 ? "�����������" : "���������",
				GET_OBJ_PNAME(obj_proto[rnum], 1),
				abs(ench.second.weight), KNRM);
			out += buf_;
		}
		if (ench.second.ndice != 0 || ench.second.sdice != 0)
		{
			if (ench.second.ndice >= 0 && ench.second.sdice >= 0)
			{
				snprintf(buf_, sizeof(buf_),
					" +    %s����������� ���� %s �� %dD%d%s\r\n",
					KCYN, GET_OBJ_PNAME(obj_proto[rnum], 1),
					abs(ench.second.ndice), abs(ench.second.sdice), KNRM);
			}
			else if (ench.second.ndice <= 0 && ench.second.sdice <= 0)
			{
				snprintf(buf_, sizeof(buf_),
					" +    %s��������� ���� %s �� %dD%d%s\r\n",
					KCYN, GET_OBJ_PNAME(obj_proto[rnum], 1),
					abs(ench.second.ndice), abs(ench.second.sdice), KNRM);
			}
			else
			{
				snprintf(buf_, sizeof(buf_),
					" +    %s�������� ���� %s �� %+dD%+d%s\r\n",
					KCYN, GET_OBJ_PNAME(obj_proto[rnum], 1),
					ench.second.ndice, ench.second.sdice, KNRM);
			}
			out += buf_;
		}
	}
	return out;
}

std::string print_activ_enchants(const std::map<int, ench_type> &enchants)
{
	std::string out;
	for (auto i = enchants.begin(); i != enchants.end(); ++i)
	{
		out += print_activ_enchant(*i);
	}
	return out;
}

/// ���������� �����, ��� ��������� ��� ������ '��������' � ����������
std::string print_activ_properties(const activ_node &activ)
{
	std::string out;

	// apply
	out += print_activ_apply(activ.apply);
	// skill
	out += PrintActivators::print_skill(activ.skill, true);
	// bonus
	out += print_activ_bonus(activ.bonus);
	//enchant
	out += print_activ_enchant(activ.enchant);

	if (!out.empty())
	{
		return " + �������� :\r\n" + out;
	}
	return out;
}

/// ���������� ������� �� ���������� � ������������� ��������, ���� �����������
/// ������ ������ - ����� ����� ��������
std::string print_activ_help(const set_node &set)
{
	std::string out, prof_list;
	char buf_[2048];

	snprintf(buf_, sizeof(buf_),
		"--------------------------------------------------------------------------------\r\n"
		"%s����� ���������: %s%s%s%s\r\n",
		KNRM, KWHT, set.name.c_str(), KNRM,
		set.enabled ? "" : " (� ������ ������ ��������)");
	out += buf_ + print_obj_list(set) +
		"--------------------------------------------------------------------------------\r\n";

	for (auto i = set.activ_list.begin(); i != set.activ_list.end(); ++i)
	{
		if (i->second.prof.count() != i->second.prof.size())
		{
			// ��������� �� ������������ ������ ���� (���������� �������������
			// �� ��, ��� � �������� ����� ������ ���� ������ ���� ����������)
			if (prof_list.empty())
			{
				print_bitset(i->second.prof, pc_class_name, ",", prof_list);
			}
			snprintf(buf_, sizeof(buf_), "%d %s (%s)\r\n",
				i->first, desc_count(i->first, WHAT_OBJECT), prof_list.c_str());
		}
		else
		{
			snprintf(buf_, sizeof(buf_), "%d %s\r\n",
				i->first, desc_count(i->first, WHAT_OBJECT));
		}
		out += buf_;
		// �������
		out += print_activ_affects(i->second.affects);
		// ��������
		out += print_activ_properties(i->second);
	}

	if (set.activ_list.size() > 1)
	{
		out += print_total_activ(set);
	}

	return out;
}

/// ������� print_activ_help ������ ��� ����� ����������� (���)
std::string print_total_activ(const set_node &set)
{
	std::string out, prof_list, properties;

	activ_sum summ, prof_summ;
	for (auto i = set.activ_list.begin(); i != set.activ_list.end(); ++i)
	{
		if (i->second.prof.count() != i->second.prof.size())
		{
			// ��������� �� ������������ ������ ���� (���������� �������������
			// �� ��, ��� � �������� ����� ������ ���� ������ ���� ����������)
			if (prof_list.empty())
			{
				print_bitset(i->second.prof, pc_class_name, ",", prof_list);
			}
			prof_summ += &(i->second);
		}
		else
		{
			summ += &(i->second);
		}
	}

	out += "--------------------------------------------------------------------------------\r\n";
	out += "��������� �����:\r\n";
	out += print_activ_affects(summ.affects);
	properties += print_activ_apply(summ.apply);
	properties += PrintActivators::print_skills(summ.skills, true, false);
	properties += print_activ_bonus(summ.bonus);
	properties += print_activ_enchants(summ.enchants);
	if (!properties.empty())
	{
		out += " + �������� :\r\n" + properties;
	}

	if (!prof_list.empty())
	{
		properties.clear();
		out += "���������: " + prof_list + "\r\n";
		out += print_activ_affects(prof_summ.affects);
		properties += print_activ_apply(prof_summ.apply);
		properties += PrintActivators::print_skills(prof_summ.skills, true, false);
		properties += print_activ_bonus(prof_summ.bonus);
		properties += print_activ_enchants(prof_summ.enchants);
		if (!properties.empty())
		{
			out += " + �������� :\r\n" + properties;
		}
	}
	out += "--------------------------------------------------------------------------------\r\n";

	return out;
}

/// ��������� ������� �� ����������� ����� ������� �����, ������� ����� �����
/// ����� ��������� ����� ������
void init_xhelp()
{
	char buf_[128];
	for (size_t i = 0; i < sets_list.size(); ++i)
	{
		const int lvl = (sets_list.at(i)->enabled ? 0 : LVL_IMMORT);
		if (sets_list.at(i)->alias.empty())
		{
			snprintf(buf_, sizeof(buf_), "�����%02d", i + 1);
			HelpSystem::add_static(buf_,
				print_activ_help(*(sets_list.at(i))), lvl, true);
			sets_list.at(i)->help = buf_;
		}
		else
		{
			bool first = true;
			std::string name = "�����";
			std::vector<std::string> str_list;
			boost::split(str_list, sets_list.at(i)->alias,
				boost::is_any_of(", "), boost::token_compress_on);
			for (auto k = str_list.begin(); k != str_list.end(); ++k)
			{
				if (first)
				{
					sets_list.at(i)->help = name + *k;
					first = false;
				}
				HelpSystem::add_static(name + *k,
					print_activ_help(*(sets_list.at(i))), lvl, true);
			}
		}
	}
}

/// ��������� ����� ��� ������-����� ����� - ��� ���� ����� ������
std::string get_name(size_t idx)
{
	if (idx < sets_list.size())
	{
		return sets_list.at(idx)->name;
	}
	return "";
}

/// ������� ������ ����� �� ���� ����� ��������� �����������
void WornSets::clear()
{
	for (auto i = idx_list_.begin(); i != idx_list_.end(); ++i)
	{
		i->set_idx = -1;
		i->obj_list.clear();
		i->activated_cnt = 0;
	}
}

/// ���������� ������ (��� ������� � ��������) �� ����������� ���������
/// ������������ ����� �� ��������� ���-�� �������������� ������ � ������ ����
void WornSets::add(OBJ_DATA *obj)
{
	if (obj && is_set_item(obj))
	{
		const size_t cur_idx = obj_index[GET_OBJ_RNUM(obj)].set_idx;
		for (auto i = idx_list_.begin(); i != idx_list_.end(); ++i)
		{
			if (i->set_idx == static_cast<size_t>(-1))
			{
				i->set_idx = cur_idx;
			}
			if (i->set_idx == cur_idx)
			{
				i->obj_list.push_back(obj);
				if (obj->get_activator().first)
				{
					i->activated_cnt += 1;
				}
				return;
			}
		}
	}
}

/// �������� ������ ����� �� ���� � ���������� �� �����������
void WornSets::check(CHAR_DATA *ch)
{
    for (auto i = idx_list_.begin(); i != idx_list_.end(); ++i)
	{
		if (i->set_idx >= sets_list.size()) return;

		boost::shared_ptr<set_node> &cur_set = sets_list.at(i->set_idx);

		int max_activ = 0;
		if (cur_set->enabled)
		{
			for (auto k = cur_set->activ_list.cbegin();
				k != cur_set->activ_list.cend(); ++k)
			{
				const size_t prof_bit = GET_CLASS(ch);
				// k->first - ���-�� ��� ���������,
				// i->obj_list.size() - ����� �� ����
				if (k->first > i->obj_list.size())
				{
						continue;
				}
				else if (!IS_NPC(ch)
					&& prof_bit < k->second.prof.size()
					&& !k->second.prof.test(prof_bit))
				{
						continue;
				}
				else if (IS_NPC(ch)
					&& k->second.prof.count() != k->second.prof.size())
				{
					continue;
				}
				// ��������� ��� �� ����, ����� ���� ���� - ���� ������
				ch->obj_bonus() += &(k->second);
				max_activ = k->first;
				check_activated(ch, k->first, *i);
			}
		}
		// �� ����������� ��������� ���� ���� ����������� ����
		check_deactivated(ch, max_activ, *i);
	}
}

bool bonus_type::operator!=(const bonus_type &r) const
{
	if (phys_dmg != r.phys_dmg || mage_dmg != r.mage_dmg)
	{
		return true;
	}
	return false;
}

bool bonus_type::operator==(const bonus_type &r) const
{
	return !(*this != r);
}

bonus_type& bonus_type::operator+=(const bonus_type &r)
{
	phys_dmg += r.phys_dmg;
	mage_dmg += r.mage_dmg;
	return *this;
}

bool bonus_type::empty() const
{
	if (phys_dmg != 0 || mage_dmg != 0)
	{
		return false;
	}
	return true;
}

bool ench_type::operator!=(const ench_type &r) const
{
	if (weight != r.weight
		|| ndice != r.ndice
		|| sdice != r.sdice)
	{
		return true;
	}
	return false;
}

bool ench_type::operator==(const ench_type &r) const
{
	return !(*this != r);
}

ench_type& ench_type::operator+=(const ench_type &r)
{
	weight += r.weight;
	ndice += r.ndice;
	sdice += r.sdice;
	return *this;
}

bool ench_type::empty() const
{
	if (weight != 0 || ndice != 0 || sdice != 0)
	{
		return false;
	}
	return true;
}

activ_sum& activ_sum::operator+=(const activ_node *r)
{
	affects += r->affects;
	PrintActivators::sum_apply(apply, r->apply);
	PrintActivators::add_pair(skills, r->skill);
	bonus += r->bonus;
	PrintActivators::add_pair(enchants, r->enchant);

	return *this;
}

bool activ_sum::operator!=(const activ_sum &r) const
{
	if (affects != r.affects
		|| apply != r.apply
		|| skills != r.skills
		|| bonus != r.bonus
		|| enchants != r.enchants)
	{
		return true;
	}
	return false;
}

bool activ_sum::operator==(const activ_sum &r) const
{
	return !(*this != r);
}

bool activ_sum::empty() const
{
	if (!affects.empty()
		|| !apply.empty()
		|| !skills.empty()
		|| !bonus.empty()
		|| !enchants.empty())
	{
		return false;
	}
	return true;
}

void activ_sum::clear()
{
	affects = clear_flags;
	apply.clear();
	skills.clear();
	bonus.phys_dmg = 0;
	bonus.mage_dmg = 0;
	enchants.clear();
}

WornSets worn_sets;

void check_enchants(CHAR_DATA *ch)
{
	OBJ_DATA *obj;
	for (int i = 0; i < NUM_WEARS; i++)
	{
		obj = GET_EQ(ch, i);
		if (obj)
		{
			auto i = ch->obj_bonus().enchants.find(GET_OBJ_VNUM(obj));
			if (i != ch->obj_bonus().enchants.end())
			{
				obj->enchants.update_set_bonus(obj, &(i->second));
			}
			else
			{
				obj->enchants.remove_set_bonus(obj);
			}
		}
	}
}

void activ_sum::update(CHAR_DATA *ch)
{
	if (IS_NPC(ch))
	{
		return;
	}

	this->clear();
	worn_sets.clear();
	for (int i = 0; i < NUM_WEARS; i++)
	{
		worn_sets.add(GET_EQ(ch, i));
	}
	worn_sets.check(ch);
	check_enchants(ch);
}

void activ_sum::apply_affects(CHAR_DATA *ch) const
{
	for (int j = 0; weapon_affect[j].aff_bitvector >= 0; j++)
	{
		if (weapon_affect[j].aff_bitvector != 0
			&& IS_SET(GET_FLAG(affects, weapon_affect[j].aff_pos), weapon_affect[j].aff_pos))
		{
			affect_modify(ch, APPLY_NONE, 0, weapon_affect[j].aff_bitvector, TRUE);
		}
	}
	for (auto i = apply.begin(); i != apply.end(); ++i)
	{
		affect_modify(ch, i->location, i->modifier, 0, TRUE);
	}
}

int activ_sum::calc_phys_dmg(int dam) const
{
	return dam * bonus.phys_dmg / 100;
}

int activ_sum::calc_mage_dmg(int dam) const
{
	return dam * bonus.mage_dmg / 100;
}

int activ_sum::get_skill(int num) const
{
	auto i = skills.find(num);
	if (i != skills.end())
	{
		return i->second;
	}
	return 0;
}

bool is_set_item(OBJ_DATA *obj)
{
	if (GET_OBJ_RNUM(obj) >= 0
		&& obj_index[GET_OBJ_RNUM(obj)].set_idx != static_cast<size_t>(-1))
	{
		return true;
	}
	return false;
}

} // namespace obj_sets

/// ������� slist
ACMD(do_slist)
{
	if (IS_NPC(ch))
	{
		return;
	}
	obj_sets::do_slist(ch);
}
