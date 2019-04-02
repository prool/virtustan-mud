// Copyright (c) 2012 Krodo
// Part of Bylins http://www.mud.ru

#include "conf.h"
#include <sstream>
#include <iostream>
#include <set>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "sets_drop.hpp"
#include "db.h"
#include "utils.h"
#include "char.hpp"
#include "comm.h"
#include "handler.h"
#include "dg_scripts.h"
#include "im.h"
#include "room.hpp"
#include "pugixml.hpp"
#include "modify.h"
#include "house.h"
#include "screen.h"
#include "help.hpp"
#include "parse.hpp"
#include "mob_stat.hpp"
#include "obj_sets.hpp"

namespace SetsDrop
{

// ������ ����� �� ����
const char *CONFIG_FILE = LIB_MISC"full_set_drop.xml";
// ����������� ������� ���� ��� ������� � ����-������ �����
const int MIN_GROUP_MOB_LVL = 32;
// ���/���� ������ ����� ��� ������� ����-�����
const int MIN_SOLO_MOB_LVL = 25;
const int MAX_SOLO_MOB_LVL = 31;
// ����. ���-�� ���������� � ������ ����������� � ����������
const int MAX_GROUP_SIZE = 12;
const char *DROP_TABLE_FILE = LIB_PLRSTUFF"sets_drop.xml";
// ����� ������� ����� ������ � �����
const int RESET_TIMER = 35;
// ������� ���� ����� ���� ����� *10
const int DEFAULT_SOLO_CHANCE = 30;
// ���������� ���� ��� ����-����� ������������ ���-�����
const double MINI_SET_MULT = 1.5;
// ��������� ������� ��� ������ ������� �����
const char *RESET_MESSAGE =
	"�������� � ���� ��������� ���-�� ����������.";

enum { SOLO_MOB, GROUP_MOB, SOLO_ZONE, GROUP_ZONE };

// ����� ���������� ������ �������
time_t next_reset_time = 0;
// ���� ��� ����� ������� ����� �� �������
bool need_save_drop_table = false;

// ������ ����-����� �� ���� (vnum)
std::list<int> group_obj_list;
// ������ ����-����� �� ���� (vnum)
std::list<int> solo_obj_list;

struct MobNode
{
	MobNode() : vnum(-1), rnum(-1), miw(-1), type(-1)
	{
		kill_stat.fill(0);
	};

	int vnum;
	int rnum;
	// ����.�.����
	int miw;
	// ��� ���� (��� ����������)
	std::string name;
	// ����/���� ���
	int type;
	// ���������� ������ �� ������� ������
	mob_stat::KillStatType kill_stat;
};

struct ZoneNode
{
	ZoneNode() : zone(-1), type(-1) {};

	// ���� ����
	int zone;
	// ��� ���� (����/���������), ����������� �� ���������� ������� �����
	int type;
	// ������ ����� � ����
	std::list<MobNode> mobs;
};

// ��������� ������ ��� ������ ������������ ���� ������ ����� ����
std::list<ZoneNode> mob_name_list;
// ��������� ������ ����� �� ���� ����-�����
std::list<ZoneNode> group_mob_list;
// ��������� ������ ����� �� ���� ����-�����
std::list<ZoneNode> solo_mob_list;

struct HelpNode
{
	// ������ �������
	std::string alias_list;
	// ������ ��� ����
	std::string title;
	// ������ ��������� �� ����� �����
	std::set<int> solo_list_1;
	std::set<int> solo_list_2;
	std::set<int> group_list;
};
// ������ ����� ��� �������
std::vector<HelpNode> help_list;

class Linked
{
public:
	Linked() {};
	int list_size() const { return mob_list.size(); };
	int drop_count() const;
	bool need_reset() const;
	void reset();
	void add(int mob_rnum) { mob_list.insert(mob_rnum); };
private:
	// ������ ����� ������������� ����� �������
	std::set<int /* mob rnum */> mob_list;
};

struct DropNode
{
	DropNode() : obj_rnum(0), obj_vnum(0), chance(0), solo(false), can_drop(false), is_big_set(false) {};
	// ���� ������
	int obj_rnum;
	// ���� ������
	int obj_vnum;
	// ���� ����� (�������� * 10), chance �� 1000
	int chance;
	// �� ���� ��� ���� ��� ����� ��� ������ (��� ��������� �������)
	bool solo;
	// false = �������� � ������, ����� ������ = ���� � ����
	// true = �������� � ������, ����� ������ ����� ����������
	bool can_drop;
	// ��� ����-����� ��� ����������� ������ ����� (������ ������ ��� solo �����)
	bool is_big_set;
	// ����������� ����� ������� ����-����
	Linked linked_mobs;

	void reset_chance() { chance = is_big_set ? DEFAULT_SOLO_CHANCE : DEFAULT_SOLO_CHANCE * MINI_SET_MULT; };
};

// ��������� ������ ����� �� ����� (mob_rnum)
std::map<int, DropNode> drop_list;

// ����������� ���� � ���� ������ � ���� ����������� �����, ��� �������������
// ������������ ���� ����� � ����������� ���� ������������� ������ ������ ��� ���� ������
bool Linked::need_reset() const
{
	bool flag = false;
	for (std::set<int>::const_iterator i = mob_list.begin(),
		iend = mob_list.end(); i != iend; ++i)
	{
		std::map<int, DropNode>::iterator k = drop_list.find(*i);
		if (k != drop_list.end())
		{
			const int num = obj_index[k->second.obj_rnum].stored + obj_index[k->second.obj_rnum].number;
			if (num < GET_OBJ_MIW(obj_proto[k->second.obj_rnum]) && !k->second.can_drop)
			{
				flag = true;
				k->second.can_drop = true;
			}
			if (num >= GET_OBJ_MIW(obj_proto[k->second.obj_rnum]) && k->second.can_drop)
			{
				k->second.can_drop = false;
			}
		}
	}
	return flag;
}

void Linked::reset()
{
	for (std::set<int>::const_iterator i = mob_list.begin(),
		iend = mob_list.end(); i != iend; ++i)
	{
		std::map<int, DropNode>::iterator k = drop_list.find(*i);
		if (k != drop_list.end())
		{
			k->second.reset_chance();
		}
	}
}

int Linked::drop_count() const
{
	int count = 0;

	for (std::set<int>::const_iterator i = mob_list.begin(),
		iend = mob_list.end(); i != iend; ++i)
	{
		std::map<int, DropNode>::const_iterator k = drop_list.find(*i);
		if (k != drop_list.end() && k->second.can_drop)
		{
			++count;
		}
	}

	return count;
}

// * ������������� ������ ����� �� ����.
void init_obj_list()
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(CONFIG_FILE);
	if (!result)
	{
		snprintf(buf, MAX_STRING_LENGTH, "...%s", result.description());
		mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
		return;
	}
    pugi::xml_node node_list = doc.child("set_list");
    if (!node_list)
    {
		mudlog("...<set_list> read fail", CMP, LVL_IMMORT, SYSLOG, TRUE);
		return;
    }
	for (pugi::xml_node set_node = node_list.child("set");
		set_node; set_node = set_node.next_sibling("set"))
	{
		HelpNode node;

		node.alias_list = Parse::attr_str(set_node, "help_alias");
		if (node.alias_list.empty())
		{
			mudlog("...bad set attributes (empty help_alias)",
				CMP, LVL_IMMORT, SYSLOG, TRUE);
			continue;
		}

		std::string type = Parse::attr_str(set_node, "type");
		if (type.empty() || (type != "auto" && type != "manual"))
		{
			snprintf(buf, sizeof(buf),
				"...bad set attributes (type=%s)", type.c_str());
			mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
			continue;
		}

		std::set<int> tmp_solo_list;

		if (type == "manual")
		{
			for (pugi::xml_node obj_node = set_node.child("obj");
				obj_node; obj_node = obj_node.next_sibling("obj"))
			{
				const int obj_vnum = Parse::attr_int(obj_node, "vnum");
				const int obj_rnum = real_object(obj_vnum);
				if (obj_rnum < 0)
				{
					snprintf(buf, sizeof(buf),
						"...bad obj_node attributes (vnum=%d)", obj_vnum);
					mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
					continue;
				}

				std::string list_type = Parse::attr_str(obj_node, "list");
				if (list_type.empty()
					|| (list_type != "solo" && list_type != "group"))
				{
					snprintf(buf, sizeof(buf),
						"...bad manual obj attributes (list=%s, obj_vnum=%d)",
						list_type.c_str(), obj_vnum);
					mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
					continue;
				}

				if (list_type == "solo")
				{
					solo_obj_list.push_back(obj_vnum);
					// ��������������� ���� ������, �������
					// ������ ����� ���� ������� ��� �� ���
					tmp_solo_list.insert(obj_vnum);
				}
				else
				{
					group_obj_list.push_back(obj_vnum);
					// ���� ������ ������ �����
					node.group_list.insert(obj_vnum);
				}
				// ��� ����
				if (node.title.empty())
				{
					if (obj_index[obj_rnum].set_idx != static_cast<size_t>(-1))
					{
						node.title =
							obj_sets::get_name(obj_index[obj_rnum].set_idx);
					}
					else
					{
						for (auto it = obj_data::set_table.begin(),
							iend = obj_data::set_table.end(); it != iend; ++it)
						{
							auto k = it->second.find(obj_vnum);
							if (k != it->second.end())
							{
								node.title = it->second.get_name();
							}
						}
					}
				}
			}
		}
		else
		{
			// ������ ���� ������������� �� ����.�����������
			std::multimap<int, int> set_sort_list;

			for (pugi::xml_node obj_node = set_node.child("obj");
				obj_node; obj_node = obj_node.next_sibling("obj"))
			{
				const int obj_vnum = Parse::attr_int(obj_node, "vnum");
				if (real_object(obj_vnum) < 0)
				{
					snprintf(buf, sizeof(buf),
						"...bad obj_node attributes (vnum=%d)", obj_vnum);
					mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
					continue;
				}
				// ���������� ������ �����������
				for (id_to_set_info_map::const_iterator it = obj_data::set_table.begin(),
					iend = obj_data::set_table.end(); it != iend; ++it)
				{
					set_info::const_iterator k = it->second.find(obj_vnum);
					if (k != it->second.end() && !k->second.empty())
					{
						// ������� ��������� (������������) � ������ ���������
						set_sort_list.insert(
							std::make_pair(k->second.rbegin()->first, obj_vnum));
						// ��� ����
						if (node.title.empty())
						{
							node.title = it->second.get_name();
						}
					}
				}
			}
			// ������ �������� ����������� � ����-����, ������ � �����
			int num = 0, total_num = set_sort_list.size();
			for (std::multimap<int, int>::const_iterator i = set_sort_list.begin(),
				iend = set_sort_list.end(); i != iend; ++i, ++num)
			{
				if (num < total_num / 2)
				{
					solo_obj_list.push_back(i->second);
					tmp_solo_list.insert(i->second);
				}
				else
				{
					group_obj_list.push_back(i->second);
					node.group_list.insert(i->second);
				}
			}
		}
		// ������� ���� ������ �� ���, ���� ���������
		if (tmp_solo_list.size() > 4)
		{
			int num = 0, total_num = tmp_solo_list.size();
			for (std::set<int>::const_iterator i = tmp_solo_list.begin(),
				iend = tmp_solo_list.end(); i != iend; ++i, ++num)
			{
				if (num < total_num / 2)
				{
					node.solo_list_1.insert(*i);
				}
				else
				{
					node.solo_list_2.insert(*i);
				}
			}
		}
		else
		{
			node.solo_list_1 = tmp_solo_list;
		}

		// ������ ������� � ����� ��� �������
		help_list.push_back(node);
	}
}

void add_to_zone_list(std::list<ZoneNode> &cont, MobNode &node)
{
	int zone = node.vnum/100;
	std::list<ZoneNode>::iterator k = std::find_if(cont.begin(), cont.end(),
		boost::bind(std::equal_to<int>(),
		boost::bind(&ZoneNode::zone, _1), zone));

	if (k != cont.end())
	{
		k->mobs.push_back(node);
	}
	else
	{
		ZoneNode tmp_node;
		tmp_node.zone = zone;
		tmp_node.mobs.push_back(node);
		cont.push_back(tmp_node);
	}
}

/**
 * ��������� ���������������� ������ ������ ����� ��� ������������
 * ������ ������������ �� ������ ������ ����� ����.
 * ������ � ����: ����, ���, ���
 * ����������: ���� ��� ���������
 *             ����� ����: ����-�����, ������ � ������, ������ � ������
 */
void init_mob_name_list()
{
	std::set<int> bad_zones;

	// ����-�����
	for (ClanListType::const_iterator clan = Clan::ClanList.begin();
		clan != Clan::ClanList.end(); ++clan)
	{
		bad_zones.insert((*clan)->GetRent()/100);
	}

	// ������
	int curr_zone = 0;
	bool rent = false, mail = false, banker = false;
	for (std::vector<ROOM_DATA *>::const_iterator i = world.begin(),
		iend = world.end(); i != iend; ++i)
	{
		if (curr_zone != zone_table[(*i)->zone].number)
		{
			if (rent && mail && banker)
			{
				bad_zones.insert(curr_zone);
			}
			rent = false;
			mail = false;
			banker = false;
			curr_zone = zone_table[(*i)->zone].number;
		}
		for (CHAR_DATA *ch = (*i)->people; ch; ch = ch->next_in_room)
		{
			if (IS_RENTKEEPER(ch))
			{
				rent = true;
			}
			else if (IS_POSTKEEPER(ch))
			{
				mail = true;
			}
			else if (IS_BANKKEEPER(ch))
			{
				banker = true;
			}
		}
	}

	// �������� ����
	for (int nr = 0; nr <= top_of_zone_table; nr++)
	{
		if (zone_table[nr].under_construction)
		{
			bad_zones.insert(zone_table[nr].number);
		}
	}

	for (auto i = mob_stat::mob_list.cbegin(),
		iend = mob_stat::mob_list.cend(); i != iend; ++i)
	{
		const int rnum = real_mobile(i->first);
		const int zone = i->first/100;
		std::set<int>::const_iterator k = bad_zones.find(zone);

		if (rnum < 0
			|| zone < 100
			|| k != bad_zones.end())
		{
			continue;
		}

		MobNode node;
		node.vnum = i->first;
		node.rnum = rnum;
		node.name = mob_proto[rnum].get_name();
		auto stat = mob_stat::sum_stat(i->second, 4);
		node.kill_stat = stat.kills;

		add_to_zone_list(mob_name_list, node);
	}
}

// * ������ � ����: ���
void init_zone_type()
{
	for (std::list<ZoneNode>::iterator i = mob_name_list.begin(),
		iend = mob_name_list.end(); i != iend; ++i)
	{
		int killed_solo = 0;
		for (std::list<MobNode>::iterator k = i->mobs.begin(),
			kend = i->mobs.end(); k != kend; ++k)
		{
			int group_cnt = 0;
			for (int cnt = 2; cnt <= MAX_GROUP_SIZE; ++cnt)
			{
				group_cnt += k->kill_stat.at(cnt);
			}
			if (k->kill_stat.at(1) > group_cnt)
			{
				++killed_solo;
			}
		}
		if (killed_solo >= i->mobs.size() * 0.8)
		{
			i->type = SOLO_ZONE;
		}
		else
		{
			i->type = GROUP_ZONE;
		}
	}
}

// * ������ � ����: ���
void init_mob_type()
{
	for (std::list<ZoneNode>::iterator i = mob_name_list.begin(),
		iend = mob_name_list.end(); i != iend; ++i)
	{
		for (std::list<MobNode>::iterator k = i->mobs.begin(),
			kend = i->mobs.end(); k != kend; ++k)
		{
			int group_cnt = 0;
			for (int cnt = 2; cnt <= MAX_GROUP_SIZE; ++cnt)
			{
				group_cnt += k->kill_stat.at(cnt);
			}
			if (i->type == SOLO_ZONE && k->kill_stat.at(1) > group_cnt)
			{
				k->type = SOLO_MOB;
			}
			else if (i->type == GROUP_ZONE && k->kill_stat.at(1) < group_cnt)
			{
				k->type = GROUP_MOB;
			}
		}
	}
}

/**
 * ������ ����.�.���� �� ������ ���� ������ ����.
 * ���� ���� ����� ��������� - ������� ������������ ��������.
 */
int calc_max_in_world(int mob_rnum)
{
	int max_in_world = 0;
	for (int i = 0; i <= top_of_zone_table; ++i)
	{
		for (int cmd_no = 0; zone_table[i].cmd[cmd_no].command != 'S'; ++cmd_no)
		{
			if (zone_table[i].cmd[cmd_no].command == 'M'
				&& zone_table[i].cmd[cmd_no].arg1 == mob_rnum)
			{
				max_in_world = MAX(max_in_world, zone_table[i].cmd[cmd_no].arg2);
			}
		}
	}
	return max_in_world;
}

/**
 * � ����� ������� �������� ���������� ��������� ����
 * ����������: ���� � ������������ ������ ������ ����� ����,
 *             ���� ������ 1 ����.�.����
 *             ���� ���� ���������� ������ ��� ������� ����
 *             ���� � ������� ������ ����������/������� ����
 * ������ � ����: ����.�.����
 */
void filter_dupe_names()
{
	for (std::list<ZoneNode>::iterator it = mob_name_list.begin(),
		iend = mob_name_list.end(); it != iend; ++it)
	{
		std::list<MobNode> tmp_list;
		// ��������� (������� ��������) ���������� ����� � ������� �������
		for (std::list<MobNode>::iterator k = it->mobs.begin(),
			kend = it->mobs.end(); k != kend; ++k)
		{
			// ���������� ����� � �������� ����
			bool good = true;
			for (std::list<MobNode>::iterator l = it->mobs.begin(),
				lend = it->mobs.end(); l != lend; ++l)
			{
				if (k->vnum != l->vnum && k->name == l->name)
				{
					good = false;
					break;
				}
			}
			if (!good || k->type == -1)
			{
				continue;
			}
			// �������� �� ����� ����
			if (k->type == SOLO_MOB
				&& (mob_proto[k->rnum].get_level() < MIN_SOLO_MOB_LVL
					|| mob_proto[k->rnum].get_level() > MAX_SOLO_MOB_LVL))
			{
				continue;
			}
			if (k->type == GROUP_MOB
				&& mob_proto[k->rnum].get_level() < MIN_GROUP_MOB_LVL)
			{
				continue;
			}
			// ����� ������������ ����, ���� ��� �����
			const CHAR_DATA *mob = &mob_proto[k->rnum];
			if (MOB_FLAGGED(mob, MOB_LIKE_FULLMOON)
				|| MOB_FLAGGED(mob, MOB_LIKE_WINTER)
				|| MOB_FLAGGED(mob, MOB_LIKE_SPRING)
				|| MOB_FLAGGED(mob, MOB_LIKE_SUMMER)
				|| MOB_FLAGGED(mob, MOB_LIKE_AUTUMN)
				|| mob->get_exp() <= 0)
			{
				continue;
			}
			// ���� ������ ���������� ����
			k->miw = calc_max_in_world(k->rnum);
			if (k->miw != 1)
			{
				continue;
			}

			tmp_list.push_back(*k);
		}
		it->mobs = tmp_list;
	}
}

/**
 * ����������: ������ ���� � ����� � ����� ������� ������� �����
 * ��� ����� ������������ ������������� �� ������ �����
 */
void filter_extra_mobs(int total, int type)
{
	std::list<ZoneNode> &cont = (type == GROUP_MOB) ? group_mob_list : solo_mob_list;
	const int obj_total = (type == GROUP_MOB) ? group_obj_list.size() : solo_obj_list.size();
	// ��������� ������ ����� � ����� ����������� �����
	int num_del = total - obj_total;
	while (num_del > 0)
	{
		unsigned max_num = 0;
		for (std::list<ZoneNode>::iterator it = cont.begin(),
			iend = cont.end(); it != iend; ++it)
		{
			if (it->mobs.size() > max_num)
			{
				max_num = it->mobs.size();
			}
		}
		for (std::list<ZoneNode>::iterator it = cont.begin(),
			iend = cont.end(); it != iend; ++it)
		{
			if (it->mobs.size() >= max_num)
			{
				std::list<MobNode>::iterator l = it->mobs.begin();
				std::advance(l, number(0, it->mobs.size() - 1));
				it->mobs.erase(l);
				if (it->mobs.empty())
				{
					cont.erase(it);
				}
				--num_del;
				break;
			}
		}
	}
}

// * ���������� ������ ������ ����� �� �����- � ����- ������.
void split_mob_name_list()
{
	int total_group_mobs = 0, total_solo_mobs = 0;

	for (std::list<ZoneNode>::iterator it = mob_name_list.begin(),
		iend = mob_name_list.end(); it != iend; ++it)
	{
		for (std::list<MobNode>::iterator k = it->mobs.begin(),
			kend = it->mobs.end(); k != kend; ++k)
		{
			if (k->type == GROUP_MOB)
			{
				add_to_zone_list(group_mob_list, *k);
				++total_group_mobs;
			}
			else if (k->type == SOLO_MOB)
			{
				add_to_zone_list(solo_mob_list, *k);
				++total_solo_mobs;
			}
		}
	}
	mob_name_list.clear();

	filter_extra_mobs(total_group_mobs, GROUP_MOB);
	filter_extra_mobs(total_solo_mobs, SOLO_MOB);
}

int calc_drop_chance(std::list<MobNode>::iterator &mob, int obj_rnum)
{
	int chance = 0;

	if (mob->type == GROUP_MOB)
	{
		// ��� ������� ������������ �� ���-�� �������
		int max_kill = 0;
		int num1 = 2;
		// � ��� ����� ���-�� ���������
		for (int i = 2; i <= MAX_GROUP_SIZE; ++i)
		{
			if (mob->kill_stat.at(i) > max_kill)
			{
				max_kill = mob->kill_stat.at(i);
				num1 = i;
			}
		}
		int max_kill2 = 0;
		int num2 = 2;
		for (int i = 2; i <= MAX_GROUP_SIZE; ++i)
		{
			if (i != num1
				&& mob->kill_stat.at(i) > max_kill2)
			{
				max_kill2 = mob->kill_stat.at(i);
				num2 = i;
			}
		}
		// � ������� ����� ����
		double num_mod = (num1 + num2) / 2.0;
		// 5.8% .. 14.8%
		chance = (40 + num_mod * 9) / mob->miw;
	}
	else
	{
		// 2.6% .. 3.4% / 38 ... 28
/*		int mob_lvl = mob_proto[mob->rnum].get_level();
		int lvl_mod = MIN(MAX(0, mob_lvl - MIN_SOLO_MOB_LVL), 6);
		chance = (26 + lvl_mod * 1.45) / mob->miw;
*/
		chance = DEFAULT_SOLO_CHANCE;
	}

	// ���� ���� ����������� ���� �� ����
	const OBJ_DATA *obj = obj_proto[obj_rnum];
	if (!SetSystem::is_big_set(obj))
	{
		chance *= MINI_SET_MULT;
	}

	return chance;
}

// * ��������� ��������� ������� ����� � �����.
void init_drop_table(int type)
{
	std::list<ZoneNode> &mob_list = (type == GROUP_MOB) ? group_mob_list : solo_mob_list;
	std::list<int> &obj_list = (type == GROUP_MOB) ? group_obj_list : solo_obj_list;

	while(!obj_list.empty() && !mob_list.empty())
	{
		std::list<int>::iterator it = obj_list.begin();
		std::advance(it, number(0, obj_list.size() - 1));
		const int obj_rnum = real_object(*it);

		std::list<ZoneNode>::iterator k = mob_list.begin();
		std::advance(k, number(0, mob_list.size() - 1));

		// �� ���� ������� ���� ����� ����� �������� ����������
		// ��������, �� ��� ����� ���������� ��������, ��� �� �����
		// ���������� ��� ������ ������ ����� ������ ����������
		if (k->mobs.empty())
		{
			mob_list.erase(k);
			continue;
		}

		std::list<MobNode>::iterator l = k->mobs.begin();
		std::advance(l, number(0, k->mobs.size() - 1));

		DropNode tmp_node;
		tmp_node.obj_rnum = obj_rnum;
		tmp_node.obj_vnum = *it;
		tmp_node.chance = calc_drop_chance(l, obj_rnum);
		tmp_node.solo = (type == GROUP_MOB) ? false : true;

		drop_list.insert(std::make_pair(l->rnum, tmp_node));

		obj_list.erase(it);
		k->mobs.erase(l);
	}

	obj_list.clear();
	mob_list.clear();
}

void init_linked_mobs(const std::set<int> &node)
{
	// ���� ������ ������������� �����
	Linked tmp_linked_mobs;
	for (std::set<int>::const_iterator l = node.begin(),
		lend = node.end(); l != lend; ++l)
	{
		for (std::map<int, DropNode>::iterator k = drop_list.begin(),
				kend = drop_list.end(); k != kend; ++k)
		{
			if (k->second.obj_vnum == *l)
			{
				tmp_linked_mobs.add(k->first);
			}
		}
	}
	// ���� �������������� ������ ������� ���� �� ����� �� ������,
	// �� ��� � ������� DropNode
	for (std::set<int>::const_iterator l = node.begin(),
		lend = node.end(); l != lend; ++l)
	{
		for (std::map<int, DropNode>::iterator k = drop_list.begin(),
				kend = drop_list.end(); k != kend; ++k)
		{
			if (k->second.obj_vnum == *l)
			{
				k->second.linked_mobs = tmp_linked_mobs;
			}
		}
	}
}

void init_link_system()
{
	// ���� DropNode::linked_mobs
	for (std::vector<HelpNode>::const_iterator i = help_list.begin(),
		iend = help_list.end(); i != iend; ++i)
	{
		init_linked_mobs(i->solo_list_1);
		init_linked_mobs(i->solo_list_2);
	}
	// ���� DropNode::is_big_set
	for (std::map<int, DropNode>::iterator k = drop_list.begin(),
		kend = drop_list.end(); k != kend; ++k)
	{
		const OBJ_DATA *obj = obj_proto[k->second.obj_rnum];
		k->second.is_big_set = SetSystem::is_big_set(obj);
	}
}

std::string print_solo_list(const std::set<int> &node)
{
	if (node.empty()) return "";

	std::stringstream out;
	std::stringstream solo_obj_names;
	std::vector<std::string> solo_mob_list;
	int new_line = -1;

	for (std::set<int>::const_iterator l = node.begin(),
		lend = node.end(); l != lend; ++l)
	{
		for (std::map<int, DropNode>::iterator k = drop_list.begin(),
				kend = drop_list.end(); k != kend; ++k)
		{
			if (k->second.obj_vnum == *l)
			{
				if (new_line == -1)
				{
					solo_obj_names << "   ";
					new_line = 0;
				}
				else if (new_line == 0)
				{
					solo_obj_names << ", ";
					new_line = 1;
				}
				else if (new_line == 1)
				{
					solo_obj_names << "\r\n   ";
					new_line = 0;
				}
				solo_obj_names << GET_OBJ_PNAME(obj_proto[k->second.obj_rnum], 0);
				std::stringstream solo_out;
				solo_out.precision(1);
				solo_out << "    - " << mob_proto[k->first].get_name()
					<< " (" << zone_table[mob_index[k->first].zone].name << ")"
					<< " - " << std::fixed << k->second.chance / 10.0 << "%\r\n";
				solo_mob_list.push_back(solo_out.str());
			}
		}
	}

	std::srand(std::time(0));
	std::random_shuffle(solo_mob_list.begin(), solo_mob_list.end());
	out << solo_obj_names.str() << "\r\n";
	for (std::vector<std::string>::const_iterator i = solo_mob_list.begin(),
		iend = solo_mob_list.end(); i != iend; ++i)
	{
		out << *i;
	}

	return out.str();
}

/**
 * ���������� ����������� ���� �� ������ ����� ��������������� HelpNode.
 * � �������� ������ �� ���� � ���� ������.
 */
std::string print_current_set(const HelpNode &node)
{
	std::stringstream out;
	out << node.title << "\r\n";
	out.precision(1);

	out << print_solo_list(node.solo_list_1);
	out << print_solo_list(node.solo_list_2);

	for (std::set<int>::const_iterator l = node.group_list.begin(),
		lend = node.group_list.end(); l != lend; ++l)
	{
		for (std::map<int, DropNode>::iterator k = drop_list.begin(),
				kend = drop_list.end(); k != kend; ++k)
		{
			if (obj_index[k->second.obj_rnum].vnum == *l && k->second.chance > 0)
			{
				out << "   " << GET_OBJ_PNAME(obj_proto[k->second.obj_rnum], 0)
					<< " - " << mob_proto[k->first].get_name()
					<< " (" << zone_table[mob_index[k->first].zone].name << ")"
					<< " - " << std::fixed << k->second.chance / 10.0 << "%\r\n";
				break;
			}
		}
	}

	return out.str();
}

/**
 * ��������� '������� ����'.
 */
void init_xhelp()
{
	std::stringstream out;
	out << "������ ���������, ����������� � ������� ��������������� ���������:\r\n";

	for (std::vector<HelpNode>::const_iterator i = help_list.begin(),
		iend = help_list.end(); i != iend; ++i)
	{
		out << "\r\n" << print_current_set(*i);
	}

	HelpSystem::add_sets_drop("����", out.str());
	HelpSystem::add_sets_drop("����", out.str());
	HelpSystem::add_sets_drop("���������������", out.str());
}

/**
 * ��������� ������� �� ������� ������ ��������.
 */
void init_xhelp_full()
{
	for (std::vector<HelpNode>::const_iterator i = help_list.begin(),
		iend = help_list.end(); i != iend; ++i)
	{
		std::string text = print_current_set(*i);
		std::vector<std::string> str_list;
		boost::split(str_list, i->alias_list, boost::is_any_of(", "), boost::token_compress_on);
		for (std::vector<std::string>::const_iterator k = str_list.begin(),
			kend = str_list.end(); k != kend; ++k)
		{
			HelpSystem::add_sets_drop(*k, text);
		}
	}
}

bool load_drop_table()
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(DROP_TABLE_FILE);
	if (!result)
	{
		snprintf(buf, MAX_STRING_LENGTH, "...%s", result.description());
		mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
		return false;
	}

    pugi::xml_node node_list = doc.child("drop_list");
    if (!node_list)
    {
		snprintf(buf, MAX_STRING_LENGTH, "...<drop_list> read fail");
		mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
		return false;
    }

	pugi::xml_node time_node = node_list.child("time");
	std::string timer = Parse::attr_str(time_node, "reset");
	if (!timer.empty())
	{
		try
		{
			next_reset_time = boost::lexical_cast<time_t>(timer);
		}
		catch(...)
		{
			snprintf(buf, MAX_STRING_LENGTH,
				"...timer (%s) lexical_cast fail", timer.c_str());
			mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
			return false;
		}
	}
	else
	{
		mudlog("...empty reset time", CMP, LVL_IMMORT, SYSLOG, TRUE);
		return false;
	}

	for (pugi::xml_node item_node = node_list.child("item"); item_node;
		item_node = item_node.next_sibling("item"))
	{
		const int obj_vnum = Parse::attr_int(item_node, "vnum");
		const int obj_rnum = real_object(obj_vnum);
		if (obj_vnum <= 0 || obj_rnum < 0)
		{
			snprintf(buf, sizeof(buf),
				"...bad item attributes (vnum=%d)", obj_vnum);
			mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
			return false;
		}

		const int mob_vnum = Parse::attr_int(item_node, "mob");
		const int mob_rnum = real_mobile(mob_vnum);
		if (mob_vnum <= 0 || mob_rnum < 0)
		{
			snprintf(buf, sizeof(buf),
				"...bad item attributes (mob=%d)", mob_vnum);
			mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
			return false;
		}

		const int chance = Parse::attr_int(item_node, "chance");
		if (chance < 0)
		{
			snprintf(buf, sizeof(buf),
				"...bad item attributes (chance=%d)", chance);
			mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
			return false;
		}

		std::string solo = Parse::attr_str(item_node, "solo");
		if (solo.empty())
		{
			snprintf(buf, sizeof(buf), "...bad item attributes (solo=empty)");
			mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
			return false;
		}

		std::string can_drop = Parse::attr_str(item_node, "can_drop");
		if (can_drop.empty())
		{
			snprintf(buf, sizeof(buf), "...bad item attributes (can_drop=empty)");
			mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
			return false;
		}

		DropNode tmp_node;
		tmp_node.obj_rnum = obj_rnum;
		tmp_node.obj_vnum = obj_vnum;
		tmp_node.chance = chance;
		tmp_node.solo = solo == "true" ? true : false;
		tmp_node.can_drop = can_drop == "true" ? true : false;

		drop_list.insert(std::make_pair(mob_rnum, tmp_node));
	}
	return true;
}

void save_drop_table()
{
	if (!need_save_drop_table) return;

	pugi::xml_document doc;
	doc.append_child().set_name("drop_list");
	pugi::xml_node node_list = doc.child("drop_list");

	pugi::xml_node time_node = node_list.append_child();
	time_node.set_name("time");
	time_node.append_attribute("reset") = boost::lexical_cast<std::string>(next_reset_time).c_str();

	for (std::map<int, DropNode>::iterator i = drop_list.begin(),
		iend = drop_list.end(); i != iend; ++i)
	{
		pugi::xml_node mob_node = node_list.append_child();
		mob_node.set_name("item");
		mob_node.append_attribute("vnum") = obj_index[i->second.obj_rnum].vnum;
		mob_node.append_attribute("mob") = mob_index[i->first].vnum;
		mob_node.append_attribute("chance") = i->second.chance;
		mob_node.append_attribute("solo") = i->second.solo ? "true" : "false";
		mob_node.append_attribute("can_drop") = i->second.can_drop ? "true" : "false";
	}

	doc.save_file(DROP_TABLE_FILE);

	need_save_drop_table = false;
}

void reload_by_timer()
{
	if (next_reset_time <= time(0))
	{
		reload();
	}
}

void message_for_players()
{
	for (DESCRIPTOR_DATA *i = descriptor_list; i; i = i->next)
	{
		if (STATE(i) == CON_PLAYING && i->character)
		{
			send_to_char(i->character, "%s%s%s\r\n",
				CCICYN(i->character, C_NRM), RESET_MESSAGE,
				CCNRM(i->character, C_NRM));
		}
	}
}

void init_drop_system()
{
	init_mob_name_list();
	init_zone_type();
	init_mob_type();
	filter_dupe_names();
	split_mob_name_list();

	init_drop_table(SOLO_MOB);
	init_drop_table(GROUP_MOB);

	next_reset_time = time(0) + 60 * 60 * RESET_TIMER;
	need_save_drop_table = true;
	save_drop_table();
}

/**
 * ������ ������� �����, ��� ������� ���������� �� ��������� �����.
 * \param zone_vnum - ���� �� �������, ������� �� ���������� �����
 * ��� ��������� ���� � �������� ������ ������ ��� ��� ���
 */
void reload(int zone_vnum)
{
	group_obj_list.clear();
	solo_obj_list.clear();
	mob_name_list.clear();
	group_mob_list.clear();
	solo_mob_list.clear();
	drop_list.clear();
	help_list.clear();

	if (zone_vnum > 0)
	{
		mob_stat::clear_zone(zone_vnum);
	}

	init_obj_list();
	init_drop_system();
	init_link_system();

	HelpSystem::reload(HelpSystem::DYNAMIC);

	message_for_players();
}

/**
 * ������������� ������� ����� ����� ��� ������ ����.
 * ��� ������� �������� ������� �� ����� - ���������������� ������ ����������
 * �� �����, ������� ����� ����������� ����� �� �����.
 * init_obj_list() ��������� � ����� ������ ��-�� ������� ����� ��� �������,
 * ���� ���� ��� ����� �����-�� �������������� � ������ ����� - �������� �����
 * �������� '������� ����', �������� �� ����� ������� �� �������.
 */
void init()
{
	init_obj_list();

	if (!load_drop_table())
	{
		init_drop_system();
	}

	init_link_system();
}

// * \return ���� ������ ��� -1 ���� ������� ������
int check_mob(int mob_rnum)
{
	int rnum = -1;

	std::map<int, DropNode>::iterator it = drop_list.find(mob_rnum);
	if (it != drop_list.end() && it->second.chance > 0)
	{
		const int num = obj_index[it->second.obj_rnum].stored + obj_index[it->second.obj_rnum].number;
		// ���� ������ �� ������ �������
		if (!it->second.solo)
		{
			if (num < GET_OBJ_MIW(obj_proto[it->second.obj_rnum])
				&& number(0, 1000) <= it->second.chance)
			{
				rnum = it->second.obj_rnum;
			}
			return rnum;
		}
log("->sd: %d", it->second.obj_vnum);
		// ���� ������ - �� ������������� ������ ����������� ��� ������
		if (it->second.linked_mobs.need_reset())
		{
log("reset");
			it->second.linked_mobs.reset();
		}
		// +0% ���� ��� 4 ������ � �����
		// +0.1% ���� 3 �� 4 ������ � �����
		// +0.2% ���� 2 �� 4 ������ � �����
		// +0.3% ���� 1 �� 4 ������ � �����
		// ���� �������, ���� ������ � ����� ������ 4
		const int drop_mod = it->second.linked_mobs.list_size() - it->second.linked_mobs.drop_count();
log("list_size=%d, drop_count=%d, drop_mod=%d",
	it->second.linked_mobs.list_size(),
	it->second.linked_mobs.drop_count(), drop_mod);
log("num=%d, miw=%d", num, GET_OBJ_MIW(obj_proto[it->second.obj_rnum]));
		if (num < GET_OBJ_MIW(obj_proto[it->second.obj_rnum]))
		{
log("chance1=%d", it->second.chance);
			it->second.chance += MAX(0, drop_mod);
log("chance2=%d", it->second.chance);
			// ���������� �������� �� ����
			if (it->second.chance >= 120 || number(0, 1000) <= it->second.chance)
			{
log("drop");
				// ���� ������ ��� � ����� � ���� - ������ ������ �� �������
				it->second.reset_chance();
				rnum = it->second.obj_rnum;
			}
		}
		else
		{
log("chance3=%d", it->second.chance);
			// ������ �� � �����, ����������� ����� ��� �� ����� � ��������� ������������
			it->second.chance += MAX(0, drop_mod);
log("chance4=%d", it->second.chance);
			if (it->second.chance > 1000)
			{
log("reset");
				it->second.reset_chance();
			}
		}
log("<-sd");

		need_save_drop_table = true;
		HelpSystem::reload(HelpSystem::DYNAMIC);
	}

	return rnum;
}

void renumber_obj_rnum(const int rnum, const int mob_rnum)
{
	if(rnum < 0 && mob_rnum < 0)
	{
		snprintf(buf, MAX_STRING_LENGTH,
			"SetsDrop: renumber_obj_rnum wrong parameters...");
		mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
		return;
	}
	if (rnum > -1)
	{
		for (std::map<int, DropNode>::iterator it = drop_list.begin(),
			iend = drop_list.end(); it != iend; ++it)
		{
			if (it->second.obj_rnum >= rnum)
			{
				it->second.obj_rnum += 1;
			}
		}
	}
	if (mob_rnum > -1)
	{
		std::map<int, DropNode> tmp_list;
		for (std::map<int, DropNode>::iterator it = drop_list.begin(),
			iend = drop_list.end(); it != iend; ++it)
		{
			if (it->first >= mob_rnum)
			{
				tmp_list.insert(std::make_pair(it->first + 1, it->second));
			}
			else
			{
				tmp_list.insert(std::make_pair(it->first, it->second));
			}
		}
		drop_list = tmp_list;
	}
}

void print_timer_str(CHAR_DATA *ch)
{
	char time_buf[17];
	strftime(time_buf, sizeof(time_buf), "%H:%M %d-%m-%Y", localtime(&next_reset_time));

	std::stringstream out;
	out << "��������� ���������� �������: " << time_buf;

	const int minutes = (next_reset_time - time(0)) / 60;
	if (minutes >= 60)
	{
		out << " (" << minutes / 60 << "�)";
	}
	else if (minutes >= 1)
	{
		out << " (" << minutes << "�)";
	}
	else
	{
		out << " (������ ������)";
	}
	out << "\r\n";

	send_to_char(out.str().c_str(), ch);
}

} // namespace SetsDrop
