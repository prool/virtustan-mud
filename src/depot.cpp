// $RCSfile$     $Date$     $Revision$
// Copyright (c) 2007 Krodo
// Part of Bylins http://www.mud.ru

#include <map>
#include <list>
#include <sstream>
#include <cmath>
#include <bitset>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include "depot.hpp"
#include "db.h"
#include "handler.h"
#include "utils.h"
#include "comm.h"
#include "auction.h"
#include "exchange.h"
#include "interpreter.h"
#include "screen.h"
#include "char.hpp"
#include "name_list.hpp"
#include "char_player.hpp"
#include "modify.h"
#include "objsave.h"
#include "room.hpp"
#include "features.hpp"
#include "house.h"
#include "obj.hpp"

extern SPECIAL(bank);
extern int can_take_obj(CHAR_DATA * ch, OBJ_DATA * obj);
extern OBJ_DATA *read_one_object_new(char **data, int *error);
extern void olc_update_object(int robj_num, OBJ_DATA *obj, OBJ_DATA *olc_proto);

namespace Depot
{

// ������������ ���-�� ������ � ������������ ��������� (������� * 2)
const unsigned int MAX_PERS_SLOTS = 25;
const unsigned int MAX_PERS_INGR_SLOTS = 50;

// * ��� ������������ ������ ������ � ���������.
class OfflineNode
{
public:
	OfflineNode() : vnum(0), timer(0), rent_cost(0), uid(0) {};
	int vnum; // ����
	int timer; // ������
	int rent_cost; // ���� ����� � ����
	int uid; // ���������� ���
};

typedef std::list<OBJ_DATA *> ObjListType; // ���, ������
typedef std::list<OfflineNode> TimerListType; // ������ �����, ������������ � ��������

class CharNode
{
public:
	CharNode() : ch(0), money(0), money_spend(0), buffer_cost(0), need_save(0), cost_per_day(0) {};

// ������
	// ������ � ������������ ��������� ������
	ObjListType pers_online;
	// ��� (������ ����� � ���� ������� �������)
	CHAR_DATA *ch;
// �������
	// ������ � ���������� �������
	TimerListType offline_list;
	// ����� ����+���� ������� (���� ��� ����� ������ ��� ����)
	long money;
	// ������� ���� ��������� �� ����� ���� � �����
	long money_spend;
// ����� ����
	// ������ ��� ������� ������ �����
	double buffer_cost;
	// ��� ����, ����� �� ������ ���� ����� ���� ��� ���������
	std::string name;
	// � ��������� ���������� ������ � ��� ����� ��������� (������� �������� �� � ����)
	// �����/�������� ������, �������� �� �������� �������, ���� ���� � ����
	bool need_save;
	// ����� ��������� ����� ����� � ���� (�������� ������������ ��������� ������),
	// ����� �� ����� ������� � ����������� � ������� �����������
	int cost_per_day;

	void save_online_objs();
	void update_online_item();
	void update_offline_item(long uid);
	void reset();
	bool removal_period_cost();

	void take_item(CHAR_DATA *vict, char *arg, int howmany);
	void remove_item(ObjListType::iterator &obj_it, ObjListType &cont, CHAR_DATA *vict);
	bool obj_from_obj_list(char *name, CHAR_DATA *vict);
	void load_online_objs(int file_type, bool reload = 0);
	void online_to_offline(ObjListType &cont);

	int get_cost_per_day()
	{
		return cost_per_day;
	};
	void reset_cost_per_day()
	{
		cost_per_day = 0;
	} ;
	void add_cost_per_day(OBJ_DATA *obj);
	void add_cost_per_day(int amount);
};

typedef std::map<long, CharNode> DepotListType; // ��� ����, ����
DepotListType depot_list; // ������ ������ ��������

// * ���������� ����������� ������ ������� ��� ��������.
void depot_log(const char *format, ...)
{
	return;
	const char *filename = "../log/depot.log";
	static FILE *file = 0;
	if (!file)
	{
		file = fopen(filename, "a");
		if (!file)
		{
			log("SYSERR: can't open %s!", filename);
			return;
		}
		opened_files.push_back(file);
	}
	else if (!format)
		format = "SYSERR: // depot_log received a NULL format.";

	write_time(file);
	va_list args;
	va_start(args, format);
	vfprintf(file, format, args);
	va_end(args);
	fprintf(file, "\n");
	fflush(file);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// ������ �������, � ������� ���� ��-���� �������� ���� ��� ���� �������.
// �������� ������ ��� ������ � ��� ���������� �������� ��� ���� �� �������� � �����.
// ���� ������ ������ ������� ��� ����, ����� �� ������� ������� ������ (������ ������ �� �����).
typedef std::map < long/*��� ������*/, time_t /*����� ������ ������ � �����*/ > PurgedListType;
PurgedListType purged_list;

bool need_save_purged_list = false;

void save_purged_list()
{
	if (!need_save_purged_list) return;

	const char *filename = LIB_DEPOT"purge.db";
	std::ofstream file(filename);
	if (!file.is_open())
	{
		log("���������: error open file: %s! (%s %s %d)", filename, __FILE__, __func__, __LINE__);
		return;
	}
	for (PurgedListType::const_iterator it = purged_list.begin(); it != purged_list.end(); ++it)
		file << it->first << " " << it->second << "\n";

	need_save_purged_list = false;
}

void load_purge_list()
{
	const char *filename = LIB_DEPOT"purge.db";
	std::ifstream file(filename);
	if (!file.is_open())
	{
		log("���������: error open file: %s! (%s %s %d)", filename, __FILE__, __func__, __LINE__);
		return;
	}

	long uid;
	time_t date;

	while (file >> uid >> date)
		purged_list[uid] = date;
}

std::string generate_purged_filename(long uid)
{
	std::string name = GetNameByUnique(uid);
	if (name.empty())
		return "";

	char filename[MAX_STRING_LENGTH];
	if (!get_filename(name.c_str(), filename, PURGE_DEPOT_FILE))
		return "";

	return filename;
}

std::string generate_purged_text(long uid, int obj_vnum, unsigned int obj_uid)
{
	std::ostringstream out;
	out << "������ ��� �������� ��������: vnum " << obj_vnum << ", uid " << obj_uid << "\r\n";

	std::string name = GetNameByUnique(uid);
	if (name.empty())
		return out.str();

	Player t_ch;
	Player *ch = &t_ch;
	if (load_char(name.c_str(), ch) < 0)
		return out.str();

	char filename[MAX_STRING_LENGTH];
	if (!get_filename(name.c_str(), filename, PERS_DEPOT_FILE))
	{
		log("���������: �� ������� ������������� ��� ����� (name: %s, filename: %s) (%s %s %d).",
			name.c_str(), filename, __FILE__, __func__, __LINE__);
		return out.str();
	}

	FILE *fl = fopen(filename, "r+b");
	if (!fl) return out.str();

	fseek(fl, 0L, SEEK_END);
	int fsize = ftell(fl);
	if (!fsize)
	{
		fclose(fl);
		log("���������: ������ ���� ��������� (%s).", filename);
		return out.str();
	}
	char *databuf = new char [fsize + 1];

	fseek(fl, 0L, SEEK_SET);
	if (!fread(databuf, fsize, 1, fl) || ferror(fl) || !databuf)
	{
		fclose(fl);
		log("���������: ������ ������ ����� ��������� (%s).", filename);
		return out.str();
	}
	fclose(fl);

	char *data = databuf;
	*(data + fsize) = '\0';
	int error = 0;
	OBJ_DATA *obj;

	for (fsize = 0; *data && *data != '$'; fsize++)
	{
		if (!(obj = read_one_object_new(&data, &error)))
		{
			if (error)
				log("���������: ������ ������ �������� (%s, error: %d).", filename, error);
			continue;
		}
		if (GET_OBJ_UID(obj) == obj_uid && GET_OBJ_VNUM(obj) == obj_vnum)
		{
			std::ostringstream text;
			text << "[������������ ���������]: " << CCIRED(ch, C_NRM) << "'"
			<< obj->short_description << char_get_custom_label(obj, ch)
			<< " ��������" << GET_OBJ_SUF_2(obj)
			<<  " � ����'" << CCNRM(ch, C_NRM) << "\r\n";
			extract_obj(obj);
			return text.str();
		}
		extract_obj(obj);
	}
	delete [] databuf;
	return out.str();
}

void add_purged_message(long uid, int obj_vnum, unsigned int obj_uid)
{
	std::string name = generate_purged_filename(uid);
	if (name.empty())
	{
		log("���������: add_purge_message ������ ��� ����� %ld.", uid);
		return;
	}

	if (purged_list.find(uid) == purged_list.end())
	{
		purged_list[uid] = time(0);
		need_save_purged_list = true;
	}

	std::ofstream file(name.c_str(), std::ios_base::app);
	if (!file.is_open())
	{
		log("���������: error open file: %s! (%s %s %d)", name.c_str(), __FILE__, __func__, __LINE__);
		return;
	}
	file << generate_purged_text(uid, obj_vnum, obj_uid);
}

void delete_purged_entry(long uid)
{
	PurgedListType::iterator it = purged_list.find(uid);
	if (it != purged_list.end())
	{
		std::string name = generate_purged_filename(uid);
		if (name.empty())
		{
			log("���������: delete_purge_entry ������ ��� ����� %ld.", uid);
			return;
		}
		remove(name.c_str());
		purged_list.erase(it);
		need_save_purged_list = true;
	}
}

void show_purged_message(CHAR_DATA *ch)
{
	PurgedListType::iterator it = purged_list.find(GET_UNIQUE(ch));
	if (it != purged_list.end())
	{
		// ��� � ��� ����� � ��� ����, �� �� ��� ��� �����������
		std::string name = generate_purged_filename(GET_UNIQUE(ch));
		if (name.empty())
		{
			log("���������: show_purged_message ������ ��� ����� %d.", GET_UNIQUE(ch));
			return;
		}
		std::ifstream file(name.c_str(), std::ios::binary);
		if (!file.is_open())
		{
			log("���������: error open file: %s! (%s %s %d)", name.c_str(), __FILE__, __func__, __LINE__);
			return;
		}
		std::ostringstream out;
		out << "\r\n" << file.rdbuf();
		send_to_char(out.str(), ch);
		remove(name.c_str());
		purged_list.erase(it);
		need_save_purged_list = true;
	}
}

void clear_old_purged_entry()
{
	time_t today = time(0);
	for (PurgedListType::iterator it = purged_list.begin(); it != purged_list.end(); /* ����� */)
	{
		time_t diff = today - it->second;
		// ����� � ��������
		if (diff >= 60 * 60 * 24 * 31)
		{
			std::string name = generate_purged_filename(it->first);
			if (!name.empty())
				remove(name.c_str());
			purged_list.erase(it++);
			need_save_purged_list = true;
		}
		else
			++it;
	}
}

void init_purged_list()
{
	load_purge_list();
	clear_old_purged_entry();
	need_save_purged_list = true;
	save_purged_list();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// * �������� ����� ������������� ��������� (������ ���������� ����).
void remove_pers_file(const std::string &name)
{
	if (name.empty()) return;

	char filename[MAX_STRING_LENGTH];
	if (get_filename(name.c_str(), filename, PERS_DEPOT_FILE))
		remove(filename);
}

/**
* �������� ������ � ������ ������, ���� ��� ����.
* �������� ������� ������ � ���� ����� �� �����.
*/
void remove_char_entry(long uid, CharNode &node)
{
	depot_log("remove_char_entry: %ld", uid);
	// ���� ��� ��� ���-�� ������, ���� ���������� � ���� ��� �����
	if (!node.name.empty() && (node.money_spend || node.buffer_cost))
	{
		Player t_victim;
		Player *victim = &t_victim;
		if (load_char(node.name.c_str(), victim) > -1 && GET_UNIQUE(victim) == uid)
		{
			int total_pay = node.money_spend + static_cast<int>(node.buffer_cost);
			victim->remove_both_gold(total_pay);
			victim->save_char();
		}
	}
	node.reset();
	remove_pers_file(node.name);
}

// * ������������� ������ ��������. ���� ����� � ������� ����������� �� ���������.
void init_depot()
{
	depot_log("init_depot start");

	init_purged_list();

	const char *depot_file = LIB_DEPOT"depot.db";
	std::ifstream file(depot_file);
	if (!file.is_open())
	{
		log("���������: error open file: %s! (%s %s %d)", depot_file, __FILE__, __func__, __LINE__);
		return;
	}

	std::string buffer;
	while (file >> buffer)
	{
		if (buffer != "<Node>")
		{
			log("���������: ������ ������ <Node>");
			break;
		}
		CharNode tmp_node;
		long uid;
		// ����� ����
		if (!(file >> uid >> tmp_node.money >> tmp_node.money_spend >> tmp_node.buffer_cost))
		{
			log("���������: ������ ������ uid(%ld), money(%ld), money_spend(%ld), buffer_cost(%f).",
				uid, tmp_node.money, tmp_node.money_spend, tmp_node.buffer_cost);
			break;
		}
		// ������ ���������
		file >> buffer;
		if (buffer != "<Objects>")
		{
			log("���������: ������ ������ <Objects>.");
			break;
		}
		while (file >> buffer)
		{
			if (buffer == "</Objects>") break;

			OfflineNode tmp_obj;
			try
			{
				tmp_obj.vnum = boost::lexical_cast<int>(buffer);
			}
			catch (boost::bad_lexical_cast &)
			{
				log("���������: ������ ������ vnum (%s)", buffer.c_str());
				break;
			}
			if (!(file >> tmp_obj.timer >> tmp_obj.rent_cost >> tmp_obj.uid))
			{
				log("���������: ������ ������ timer(%d) rent_cost(%d) uid(%d) (obj vnum: %d).",
					tmp_obj.timer, tmp_obj.rent_cost, tmp_obj.uid, tmp_obj.vnum);
				break;
			}
			// ��������� ������������� ��������� �������� � ���� ��� � ���� � ���� �� ������
			int rnum = real_object(tmp_obj.vnum);
			if (rnum >= 0)
			{
				obj_index[rnum].stored++;
				tmp_node.add_cost_per_day(tmp_obj.rent_cost);
				tmp_node.offline_list.push_back(tmp_obj);
			}
		}
		if (buffer != "</Objects>")
		{
			log("���������: ������ ������ </Objects>.");
			break;
		}
		// �������� ���������� ������������
		file >> buffer;
		if (buffer != "</Node>")
		{
			log("���������: ������ ������ </Node>.");
			break;
		}

		// �������� ������ �� ������ �������� �������� ������ ����� � ��� ����� �� �������� �����,
		// ������ ����� �� ��������, ��� ���� ��� ����� ������ ��� ��� ������ � ������ ���� � ����
		// � �������� ������ �� ����� remove_char_entry - ��� ����� �����

		// ��������� ���� �� ��� ����� ��� ������
		tmp_node.name = GetNameByUnique(uid);
		if (tmp_node.name.empty())
		{
			log("���������: UID %ld - ��������� �� ����������.", uid);
			remove_char_entry(uid, tmp_node);
			continue;
		}
		// ������ ��������� ��������� ������ �����
		if (tmp_node.offline_list.empty() && tmp_node.pers_online.empty())
		{
			log("���������: UID %ld - ������ ���������.", uid);
			remove_char_entry(uid, tmp_node);
			continue;
		}

		name_convert(tmp_node.name);
		depot_list[uid] = tmp_node;
	}

	for (DepotListType::iterator it = depot_list.begin(); it != depot_list.end(); ++it)
	{
		depot_log("char: %ld", it->first);
		for (TimerListType::iterator obj_it = it->second.offline_list.begin(); obj_it != it->second.offline_list.end(); ++obj_it)
			depot_log("%d %d %d %d", obj_it->vnum, obj_it->uid, obj_it->timer, obj_it->rent_cost);
	}
	depot_log("init_depot end");
}

// * �������� ����� �������� � ����� �������� ����� ����� �������� � ������ ���, ������ ��� ����� ����.
void load_chests()
{
	for (CHAR_DATA *ch = character_list; ch; ch = ch->next)
	{
		if (ch->nr > 0 && ch->nr <= top_of_mobt && mob_index[ch->nr].func == bank)
		{
			OBJ_DATA *pers_chest = read_object(system_obj::PERS_CHEST_RNUM, REAL);
			if (!pers_chest) return;
			obj_to_room(pers_chest, ch->in_room);
		}
	}
}

// * ���������� ���� ����� � ��������� �� ���������� � ������������.
void CharNode::add_cost_per_day(int amount)
{
	if (amount < 0 || amount > 50000)
	{
		log("���������: ���������� ��������� ����� %i", amount);
		return;
	}
	int over = std::numeric_limits<int>::max() - cost_per_day;
	if (amount > over)
		cost_per_day = std::numeric_limits<int>::max();
	else
		cost_per_day += amount;
}

// * ������ add_cost_per_day(int amount) ��� �������� *OBJ_DATA.
void CharNode::add_cost_per_day(OBJ_DATA *obj)
{
	add_cost_per_day(get_object_low_rent(obj));
}

// * ��� �������� � save_timedata(), ������ ������ � ����������� ������.
void write_objlist_timedata(const ObjListType &cont, std::stringstream &out)
{
	for (ObjListType::const_iterator obj_it = cont.begin(); obj_it != cont.end(); ++obj_it)
	{
		out << GET_OBJ_VNUM(*obj_it) << " " << (*obj_it)->get_timer() << " "
			<< get_object_low_rent(*obj_it) << " " << GET_OBJ_UID(*obj_it) << "\n";
	}
}

// * ���������� ���� ������� � ����� ���� � ����-����� ������ (� ��� ����� �������� �� ������ ������).
void save_timedata()
{
	// �� ������ ������� ���� �� ����� ������� �� ������
	std::stringstream out;

	for (DepotListType::const_iterator it = depot_list.begin(); it != depot_list.end(); ++it)
	{
		out << "<Node>\n" << it->first << " ";
		// ��� ������ - ����� ����� � ��� �����, ����� ����� �� ����������� ����� ����
		if (it->second.ch)
			out << it->second.ch->get_total_gold() << " 0 ";
		else
			out << it->second.money << " " << it->second.money_spend << " ";
		out << it->second.buffer_cost << "\n";

		// ������ ����� ������ �� ������ - �� � �����, ����� �� ���������
		out << "<Objects>\n";
		write_objlist_timedata(it->second.pers_online, out);
		for (TimerListType::const_iterator obj_it = it->second.offline_list.begin();
				obj_it != it->second.offline_list.end(); ++obj_it)
		{
			out << obj_it->vnum << " " << obj_it->timer << " " << obj_it->rent_cost
				<< " " << obj_it->uid << "\n";
		}
		out << "</Objects>\n</Node>\n";
	}

	// ����� ��� ����� ���� �� ��������� � �� ��� ��� �������, �� ��� �������
	const char *depot_file = LIB_DEPOT"depot.db";
	std::ofstream file(depot_file);
	if (!file.is_open())
	{
		log("���������: error open file: %s! (%s %s %d)", depot_file, __FILE__, __func__, __LINE__);
		return;
	}
	file << out.rdbuf();
	file.close();
}

// * ������ ����� �� ������ (������ ������ ������).
void write_obj_file(const std::string &name, int file_type, const ObjListType &cont)
{
	// ������� ��� �����
	depot_log("write_obj_file: %s", name.c_str());
	char filename[MAX_STRING_LENGTH];
	if (!get_filename(name.c_str(), filename, file_type))
	{
		log("���������: �� ������� ������������� ��� ����� (name: %s, filename: %s) (%s %s %d).",
			name.c_str(), filename, __FILE__, __func__, __LINE__);
		return;
	}

	// ��� ������ ������ ������ ������� ����, ����� �� ������� �������� � ����
	if (cont.empty())
	{
		depot_log("empty cont");
		remove(filename);
		return;
	}

	// ����� � ����� ��� �������� �� ����� �����
	std::stringstream out;
	out << "* Items file\n";
	for (ObjListType::const_iterator obj_it = cont.begin(); obj_it != cont.end(); ++obj_it)
	{
		depot_log("save: %s %d %d", (*obj_it)->short_description, GET_OBJ_UID(*obj_it), GET_OBJ_VNUM(*obj_it));
		write_one_object(out, *obj_it, 0);
	}
	out << "\n$\n$\n";

	// ��������� � ����
	std::ofstream file(filename);
	if (!file.is_open())
	{
		log("���������: error open file: %s! (%s %s %d)", filename, __FILE__, __func__, __LINE__);
		return;
	}
	file << out.rdbuf();
	file.close();
}

// * ���������� ������������� ��������� ����������� ������ � ����. ��������� ����� �����.
void CharNode::save_online_objs()
{
	if (need_save)
	{
		log("Save obj: %s depot", ch->get_name());
		ObjSaveSync::check(ch->get_uid(), ObjSaveSync::PERS_CHEST_SAVE);

		write_obj_file(name, PERS_DEPOT_FILE, pers_online);
		need_save = false;
	}
}

// * ���������� ���� ������ ����� ���������� ������� ��������� � �����.
void save_all_online_objs()
{
	for (DepotListType::iterator it = depot_list.begin(); it != depot_list.end(); ++it)
		it->second.save_online_objs();
}

// * ���������� ����� ��������� �� ������� �������������.
void save_char_by_uid(int uid)
{
	DepotListType::iterator it = depot_list.find(uid);
	if (it != depot_list.end())
	{
		it->second.save_online_objs();
	}
}

// * ������ �������� � ������ ������� � ����������� � �����, ���� ��� ������ � �������� ����� �����.
void CharNode::update_online_item()
{
	for (ObjListType::iterator obj_it = pers_online.begin(); obj_it != pers_online.end();)
	{
		(*obj_it)->dec_timer();
		if ((*obj_it)->get_timer() <= 0)
		{
			if (ch)
			{
				// ���� ��� � �� ��� ��� ���� - ����� �������� � ������ ��� ��� ��� ����
				// ����� � ����, ����� �� ����� ������
				if (ch->desc && STATE(ch->desc) == CON_PLAYING)
				{
					send_to_char(ch, "[������������ ���������]: %s'%s%s ��������%s � ����'%s\r\n",
								 CCIRED(ch, C_NRM), (*obj_it)->short_description,
								 char_get_custom_label(*obj_it, ch).c_str(),
								 GET_OBJ_SUF_2((*obj_it)), CCNRM(ch, C_NRM));
				}
				else
					add_purged_message(GET_UNIQUE(ch), GET_OBJ_VNUM(*obj_it), GET_OBJ_UID(*obj_it));
			}

			// �������� ����� �� cost_per_day ����� �� ����, ������ ��� ��� ��� ��������
			depot_log("zero timer, online extract: %s %d %d", (*obj_it)->short_description, GET_OBJ_UID(*obj_it), GET_OBJ_VNUM(*obj_it));
			extract_obj(*obj_it);
			pers_online.erase(obj_it++);
			need_save = true;
		}
		else
			++obj_it;
	}
	save_purged_list();
}

/**
* ������ ����� �� ������ � �������� � ��������� ����� ��� �������� �����.
* \return true - ����� ������� ������ � ������ ��������
*/
bool CharNode::removal_period_cost()
{
	double i;
	buffer_cost += static_cast<double>(cost_per_day) / SECS_PER_MUD_DAY;
	std::modf(buffer_cost, &i);
	if (i >= 1.0f)
	{
		unsigned diff = static_cast<unsigned>(i);
		money -= diff;
		money_spend += diff;
		buffer_cost -= i;
	}
	return (money < 0) ? true : false;
}

// * ������ �������� �� ���� �������, �������� ��� ������� �������� (� �����������).
void update_timers()
{
	for (DepotListType::iterator it = depot_list.begin(); it != depot_list.end();)
	{
		class CharNode & node = it->second;
		// �������� ����� � ������������� �� �� ���� ���������� ������� ��������
		node.reset_cost_per_day();
		// ����������/����������� ������
		if (node.ch)
			node.update_online_item();
		else
			node.update_offline_item(it->first);
		// ������ ����� � ���� �����, ���� ����� ��� �� �������
		if (node.get_cost_per_day() && node.removal_period_cost())
		{
			depot_log("update_timers: no money %ld %s", it->first, node.name.c_str());
			log("���������: UID %ld (%s) - ������ ������� ��-�� �������� ����� �� �����.",
				it->first, node.name.c_str());
			remove_char_entry(it->first, it->second);
			depot_list.erase(it++);
		}
		else
			++it;
	}
}

// * ������ �������� � ������� ������� � �������� ����� �����.
void CharNode::update_offline_item(long uid)
{
	for (TimerListType::iterator obj_it = offline_list.begin(); obj_it != offline_list.end();)
	{
		--(obj_it->timer);
		if (obj_it->timer <= 0)
		{
			depot_log("update_offline_item %s: zero timer %d %d", name.c_str(), obj_it->vnum, obj_it->uid);
			add_purged_message(uid, obj_it->vnum, obj_it->uid);
			// ������ ������ � ����
			int rnum = real_object(obj_it->vnum);
			if (rnum >= 0)
				obj_index[rnum].stored--;
			// �������� ����� �� cost_per_day ����� �� ����, ������ ��� ��� ��� ��������
			offline_list.erase(obj_it++);
		}
		else
		{
			add_cost_per_day(obj_it->rent_cost);
			++obj_it;
		}
	}
	save_purged_list();
}

/**
* �������� ���� ������� ����� � ����� ����� � �������. ���� ������ ��������� �� ���������.
* ���� ��� ���������� ����� ������ ����� �� ����������, ������ ��� �� ������ ����� (������).
*/
void CharNode::reset()
{
	for (ObjListType::iterator obj_it = pers_online.begin(); obj_it != pers_online.end(); ++obj_it)
	{
		depot_log("reset %s: online extract %s %d %d", name.c_str(), (*obj_it)->short_description, GET_OBJ_UID(*obj_it), GET_OBJ_VNUM(*obj_it));
		extract_obj(*obj_it);
	}
	pers_online.clear();

	// ��� ����� ������� ���������� ��������� ���� � ���� � ����� ��� �������� ��� �����
	for (TimerListType::iterator obj = offline_list.begin(); obj != offline_list.end(); ++obj)
	{
		depot_log("reset_%s: offline erase %d %d", name.c_str(), obj->vnum, obj->uid);
		int rnum = real_object(obj->vnum);
		if (rnum >= 0)
			obj_index[rnum].stored--;
	}
	offline_list.clear();

	reset_cost_per_day();
	buffer_cost = 0;
	money = 0;
	money_spend = 0;
}

/**
* ��������, �������� �� obj ������������ ����������.
* \return 0 - �� ��������, 1- ��������.
*/
bool is_depot(OBJ_DATA *obj)
{
	if (system_obj::PERS_CHEST_RNUM < 0
		|| obj->item_number != system_obj::PERS_CHEST_RNUM)
	{
		return false;
	}
	else
	{
		return true;
	}
}

// * ���������� ���������� �������� ��� ������� ���������.
void print_obj(std::stringstream &i_out, std::stringstream &s_out,
	OBJ_DATA *obj, int count, CHAR_DATA *ch)
{
	std::stringstream &out = (GET_OBJ_TYPE(obj) == ITEM_MING
		|| GET_OBJ_TYPE(obj) == ITEM_MATERIAL) ? i_out : s_out;

	out << obj->short_description;
	out << char_get_custom_label(obj, ch);
	if (count > 1)
		out << " [" << count << "]";
	out << " [" << get_object_low_rent(obj) << " "
	<< desc_count(get_object_low_rent(obj), WHAT_MONEYa) << "]\r\n";
}

// * ������ ���-�� ������ ��� ������ � ������������ ��������� � ������ ����� ����.
unsigned int get_max_pers_slots(CHAR_DATA *ch)
{
	if (can_use_feat(ch, THRIFTY_FEAT))
		return MAX_PERS_SLOTS * 2;
	else
		return MAX_PERS_SLOTS;
}

/**
* ��� ���������� show_depot - ����� ������ ��������� ���������.
* ���������� �� ������� � ����������� ���������� ���������.
*/
std::string print_obj_list(CHAR_DATA *ch, ObjListType &cont)
{
	cont.sort(boost::bind(std::less<char *>(),
		boost::bind(&obj_data::aliases, _1),
		boost::bind(&obj_data::aliases, _2)));

	// ����� ������� ������� ������, � ����� �����
	std::stringstream s_out, i_out;
	int rent_per_day = 0;
	int count = 0, s_cnt = 0, i_cnt = 0;
	bool found = 0;

	auto prev_obj_it = cont.cend();
	for (auto obj_it = cont.cbegin(); obj_it != cont.cend(); ++obj_it)
	{
		if (GET_OBJ_TYPE(*obj_it) == ITEM_MING
			|| GET_OBJ_TYPE(*obj_it) == ITEM_MATERIAL)
		{
			++i_cnt;
		}
		else
		{
			++s_cnt;
		}

		if (prev_obj_it == cont.cend())
		{
			prev_obj_it = obj_it;
			count = 1;
		}
		else if (!equal_obj(*obj_it, *prev_obj_it))
		{
			print_obj(i_out, s_out, *prev_obj_it, count, ch);
			prev_obj_it = obj_it;
			count = 1;
		}
		else
		{
			count++;
		}
		rent_per_day += get_object_low_rent(*obj_it);
		found = true;
	}
	if (prev_obj_it != cont.cend() && count)
	{
		print_obj(i_out, s_out, *prev_obj_it, count, ch);
	}

	const long money = ch->get_gold() + ch->get_bank();
	const long expired = rent_per_day ? (money / rent_per_day) : 0;

	std::stringstream head;
	head << CCWHT(ch, C_NRM)
		<< "���� ������������ ���������. ����� � ����: "
		<< rent_per_day << " " << desc_count(rent_per_day, WHAT_MONEYa);
	if (rent_per_day)
	{
		head << ", ����� ������ �� " << expired
			<< " " << desc_count(expired, WHAT_DAY);
	}
	head << ".\r\n"
		<< "������������� ��������� ��� �����: "
		<< s_cnt << "/" << get_max_pers_slots(ch)
		<< ", ��������� ��� ������������: "
		<< i_cnt << "/" << MAX_PERS_INGR_SLOTS
		<< CCNRM(ch, C_NRM) << "\r\n\r\n";

	if (!found)
	{
		head << "� ������ ������ ��������� ��������� �����.\r\n";
	}
	std::string s_str = s_out.str();
	std::string i_str = i_out.str();
	if (!s_str.empty() && !i_str.empty())
	{
		s_str += "-----------------------------------\r\n";
	}

	return (head.str() + s_str + i_str);
}

/**
* ����� ��������� ��������� ��� �������� ������, ����� �� ��������.
* ������� ��������� �� ������� ��������, ��� ���� � �����-������� �� �� ����� (��� �������).
* \param ch - ����������� � ����������� �� ������� ���� ������, ����� ����� �������.
*/
DepotListType::iterator create_depot(long uid, CHAR_DATA *ch = 0)
{
	DepotListType::iterator it = depot_list.find(uid);
	if (it == depot_list.end())
	{
		CharNode tmp_node;
		// � ������ ���������� ���� (���� ������ ��� �������) ch ��������� �������,
		// � ��� ��������� ����� ���
		tmp_node.ch = ch;
		tmp_node.name = GetNameByUnique(uid);
		if (!tmp_node.name.empty())
		{
			depot_list[uid] = tmp_node;
			it = depot_list.find(uid);
		}
	}
	return it;
}

// * ������� ������������ ��������� ������ ��������� ����������.
void show_depot(CHAR_DATA *ch)
{
	if (IS_NPC(ch)) return;

#if 1 /* TEST_BUILD */ // by prool
	if (IS_IMMORTAL(ch))
	{
		send_to_char("� ��� ��������� ����������...\r\n" , ch);
		return;
	}
#endif

	if (RENTABLE(ch))
	{
		send_to_char(ch, "%s��������� ���������� � ����� � ������� ����������.%s\r\n",
					 CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
		return;
	}

	DepotListType::iterator it = create_depot(GET_UNIQUE(ch), ch);
	if (it == depot_list.end())
	{
		send_to_char("��������, ������� �����...\r\n" , ch);
		log("���������: UID %d, name: %s - ��������� ������������ ��� ���������.", GET_UNIQUE(ch), GET_NAME(ch));
		return;
	}

	std::string out;
	out = print_obj_list(ch, it->second.pers_online);
	page_string(ch->desc, out);
}

/**
* � ������ ������������ ����� �� ����� - ������ ������� �����, ��������� ���������� ���� �� ����.
* �� ����� ��� �������� ������������� ��� ������, �.�. �������� ���� � ���� ����� ���.
*/
void put_gold_chest(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (GET_OBJ_TYPE(obj) != ITEM_MONEY)
	{
		return;
	}
	long gold = GET_OBJ_VAL(obj, 0);
	ch->add_bank(gold);
	obj_from_char(obj);
	extract_obj(obj);
	send_to_char(ch, "�� ������� %ld %s.\r\n", gold, desc_count(gold, WHAT_MONEYu));
}

/**
* �������� ����������� �������� ������ � ���������.
* FIXME � ������� ��������.
*/
bool can_put_chest(CHAR_DATA *ch, OBJ_DATA *obj)
{
	// depot_log("can_put_chest: %s, %s", GET_NAME(ch), GET_OBJ_PNAME(obj, 0));
	if (OBJ_FLAGGED(obj, ITEM_ZONEDECAY)
			|| OBJ_FLAGGED(obj, ITEM_REPOP_DECAY)
			|| OBJ_FLAGGED(obj, ITEM_NOSELL)
			|| OBJ_FLAGGED(obj, ITEM_DECAY)
			|| OBJ_FLAGGED(obj, ITEM_NORENT)
			|| GET_OBJ_TYPE(obj) == ITEM_KEY
			|| GET_OBJ_RENT(obj) < 0
			|| GET_OBJ_RNUM(obj) <= NOTHING
			|| OBJ_FLAGGED(obj, ITEM_NAMED))//added by WorM ������� ���� ������ �������� � ����
	{
		send_to_char(ch, "��������� ���� �������� �������� %s � ���������.\r\n", OBJ_PAD(obj, 3));
		return 0;
	}
	else if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER && obj->contains)
	{
		send_to_char(ch, "� %s ���-�� �����.\r\n", OBJ_PAD(obj, 5));
		return 0;
	}
	else if (SetSystem::is_norent_set(ch, obj))
	{
		snprintf(buf, MAX_STRING_LENGTH,
				"%s - ��������� ��� � ����� ���� �� ������.\r\n",
				OBJ_PAD(obj, 0));
		send_to_char(CAP(buf), ch);
		return 0;
	}
	return 1;
}

unsigned count_inrg(const ObjListType &cont)
{
	unsigned ingr_cnt = 0;
	for (auto obj_it = cont.cbegin(); obj_it != cont.cend(); ++obj_it)
	{
		if (GET_OBJ_TYPE(*obj_it) == ITEM_MING
			|| GET_OBJ_TYPE(*obj_it) == ITEM_MATERIAL)
		{
			++ingr_cnt;
		}
	}
	return ingr_cnt;
}

// * ������ ������ � ��������� (����� �������� �����), ������ ��������� �� ���� � �����.
bool put_depot(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (IS_NPC(ch)) return 0;

#if 1 /* TEST_BUILD */ // by prool
	if (IS_IMMORTAL(ch))
	{
		send_to_char("� ��� ��������� ����������...\r\n" , ch);
		return 0;
	}
#endif

	if (RENTABLE(ch))
	{
		send_to_char(ch,
			"%s��������� ���������� � ����� � ������� ����������.%s\r\n",
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
		return 0;
	}

	if (GET_OBJ_TYPE(obj) == ITEM_MONEY)
	{
		put_gold_chest(ch, obj);
		return 1;
	}
	// � ������ ������ ���� �� �� ����� ������ �����-��
	// ���������� ������ - ��� �� ������ �������
	if (!can_put_chest(ch, obj)) return 1;

	DepotListType::iterator it = create_depot(GET_UNIQUE(ch), ch);
	if (it == depot_list.end())
	{
		send_to_char("��������, ������� �����...\r\n" , ch);
		log("���������: UID %d, name: %s - ��������� ������������ ��� ���������.",
			GET_UNIQUE(ch), GET_NAME(ch));
		return 0;
	}

	const unsigned ingr_cnt = count_inrg(it->second.pers_online);
	const unsigned staff_cnt = it->second.pers_online.size() - ingr_cnt;
	const bool is_ingr = (GET_OBJ_TYPE(obj) == ITEM_MING
		|| GET_OBJ_TYPE(obj) == ITEM_MATERIAL) ? true : false;

	if (is_ingr && ingr_cnt >= MAX_PERS_INGR_SLOTS)
	{
		send_to_char(
			"� ����� ��������� ������ �� �������� ����� ��� ������������ :(.\r\n", ch);
		return 0;
	}
	else if (!is_ingr && staff_cnt >= get_max_pers_slots(ch))
	{
		send_to_char(
			"� ����� ��������� ������ �� �������� ����� ��� ����� :(.\r\n", ch);
		return 0;
	}

	if (0/*!ch->get_bank() && !ch->get_gold()*/) // prool: ���������
	{
		send_to_char(ch,
			"� ��� ���� ������ ��� �����, ��� �� ����������� �������������� �� �������� �����?\r\n",
			OBJ_PAD(obj, 5));
		return 0;
	}

	depot_log("put_depot %s %ld: %s %d %d",
		GET_NAME(ch), GET_UNIQUE(ch), obj->short_description,
		GET_OBJ_UID(obj), GET_OBJ_VNUM(obj));
	it->second.pers_online.push_front(obj);
	it->second.need_save = true;

	act("�� �������� $o3 � ������������ ���������.", FALSE, ch, obj, 0, TO_CHAR);
	act("$n �������$g $o3 � ������������ ���������.", TRUE, ch, obj, 0, TO_ROOM);

	obj_from_char(obj);
	check_auction(NULL, obj);
	OBJ_DATA *temp;
	REMOVE_FROM_LIST(obj, object_list, next);
//	ObjectAlias::remove(obj);
	ObjSaveSync::add(ch->get_uid(), ch->get_uid(), ObjSaveSync::PERS_CHEST_SAVE);

	return 1;
}

// * ������ ����-�� �� ������������� ���������.
void take_depot(CHAR_DATA *vict, char *arg, int howmany)
{
	if (IS_NPC(vict)) return;

#if 1 /* TEST_BUILD */ // by prool
	if (IS_IMMORTAL(vict))
	{
		send_to_char("� ��� ��������� ����������...\r\n" , vict);
		return;
	}
#endif

	if (RENTABLE(vict))
	{
		send_to_char(vict, "%s��������� ���������� � ����� � ������� ����������.%s\r\n",
					 CCIRED(vict, C_NRM), CCNRM(vict, C_NRM));
		return;
	}

	DepotListType::iterator it = depot_list.find(GET_UNIQUE(vict));
	if (it == depot_list.end())
	{
		send_to_char("� ������ ������ ���� ��������� ��������� �����.\r\n", vict);
		return;
	}

	it->second.take_item(vict, arg, howmany);
}

// * ����� ������ �� ���������.
void CharNode::remove_item(ObjListType::iterator &obj_it, ObjListType &cont, CHAR_DATA *vict)
{
	depot_log("remove_item %s: %s %d %d", name.c_str(), (*obj_it)->short_description, GET_OBJ_UID(*obj_it), GET_OBJ_VNUM(*obj_it));
	(*obj_it)->next = object_list;
	object_list = *obj_it;
//	ObjectAlias::add(*obj_it);
	obj_to_char(*obj_it, vict);
	act("�� ����� $o3 �� ������������� ���������.", FALSE, vict, *obj_it, 0, TO_CHAR);
	act("$n ����$g $o3 �� ������������� ���������.", TRUE, vict, *obj_it, 0, TO_ROOM);
	cont.erase(obj_it++);
	need_save = true;
	ObjSaveSync::add(ch->get_uid(), ch->get_uid(), ObjSaveSync::PERS_CHEST_SAVE);
}

// * ����� ������ � ���������� (�� ������� �������), ������� �� ��� ��.
bool CharNode::obj_from_obj_list(char *name, CHAR_DATA *vict)
{
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;
	strcpy(tmp, name);

	ObjListType &cont = pers_online;

	int j = 0, number;
	if (!(number = get_number(&tmp))) return false;

	for (ObjListType::iterator obj_it = cont.begin(); obj_it != cont.end() && (j <= number); ++obj_it)
	{
		if ((isname(tmp, (*obj_it)->aliases) || CHECK_CUSTOM_LABEL(tmp, *obj_it, vict)) && ++j == number)
		{
			remove_item(obj_it, cont, vict);
			return true;
		}
	}
	return false;
}

// * ����� ������ � ��������� � �� ������.
void CharNode::take_item(CHAR_DATA *vict, char *arg, int howmany)
{
	ObjListType &cont = pers_online;

	int obj_dotmode = find_all_dots(arg);
	if (obj_dotmode == FIND_INDIV)
	{
		bool result = obj_from_obj_list(arg, vict);
		if (!result)
		{
			send_to_char(vict, "�� �� ������ '%s' � ���������.\r\n", arg);
			return;
		}
		while (result && --howmany)
			result = obj_from_obj_list(arg, vict);
	}
	else
	{
		if (obj_dotmode == FIND_ALLDOT && !*arg)
		{
			send_to_char("����� ��� \"���\"?\r\n", vict);
			return;
		}
		bool found = 0;
		for (ObjListType::iterator obj_list_it = cont.begin(); obj_list_it != cont.end();)
		{
			if (obj_dotmode == FIND_ALL || isname(arg, (*obj_list_it)->aliases) || CHECK_CUSTOM_LABEL(arg, *obj_list_it, vict))
			{
				// ����� ������ ���� ����� ������� �� �������� ����.��� ������
				if (!can_take_obj(vict, *obj_list_it)) return;
				found = 1;
				remove_item(obj_list_it, cont, vict);
			}
			else
				++obj_list_it;
		}

		if (!found)
		{
			send_to_char(vict, "�� �� ������ ������ �������� �� '%s' � ���������.\r\n", arg);
			return;
		}
	}
}

/**
* ����� �� ���� � ����, �� � ������ ������������� ��������� ������, ��� ������ ����� ������.
* ��� ������� ������� �� ����, ��� ���������� ������������� ����� � ������� ������.
*/
int get_total_cost_per_day(CHAR_DATA *ch)
{
	DepotListType::iterator it = depot_list.find(GET_UNIQUE(ch));
	if (it != depot_list.end())
	{
		int cost = 0;
		if (!it->second.pers_online.empty())
			for (ObjListType::const_iterator obj_it = it->second.pers_online.begin();
					obj_it != it->second.pers_online.end(); ++obj_it)
			{
				cost += get_object_low_rent(*obj_it);
			}
		cost += it->second.get_cost_per_day();
		return cost;
	}
	return 0;
}


/**
* ����� ���� � show stats.
* TODO: �������� ����� ��������, ����� ��� ������� ������������� ������������ ��������.
*/
void show_stats(CHAR_DATA *ch)
{
	std::stringstream out;
	int pers_count = 0, offline_count = 0;
	for (DepotListType::iterator it = depot_list.begin(); it != depot_list.end(); ++it)
	{
		pers_count += it->second.pers_online.size();
		offline_count += it->second.offline_list.size();
	}

	out << "  ��������: " << depot_list.size() << ", "
	<< "� ������������: " << pers_count << ", "
	<< "� ��������: " << offline_count << "\r\n";
	send_to_char(out.str().c_str(), ch);
}

/**
* ���� ������ ������ � ������ ������. FIXME: ����-���� ������ ��������� � ���� ����.
* \param reload - ��� ������� ������ ����, �� ��������� ����
* (���������� ��������� �� � ������� ������� ��� ����� ��� ���).
*/
void CharNode::load_online_objs(int file_type, bool reload)
{
	depot_log("load_online_objs %s start", name.c_str());
	if (!reload && offline_list.empty())
	{
		depot_log("empty offline_list and !reload");
		return;
	}

	char filename[MAX_STRING_LENGTH];
	if (!get_filename(name.c_str(), filename, file_type))
	{
		log("���������: �� ������� ������������� ��� ����� (name: %s, filename: %s) (%s %s %d).",
			name.c_str(), filename, __FILE__, __func__, __LINE__);
		return;
	}

	FILE *fl = fopen(filename, "r+b");
	if (!fl) return;

	fseek(fl, 0L, SEEK_END);
	int fsize = ftell(fl);
	if (!fsize)
	{
		fclose(fl);
		log("���������: ������ ���� ��������� (%s).", filename);
		return;
	}
	char *databuf = new char [fsize + 1];

	fseek(fl, 0L, SEEK_SET);
	if (!fread(databuf, fsize, 1, fl) || ferror(fl) || !databuf)
	{
		fclose(fl);
		log("���������: ������ ������ ����� ��������� (%s).", filename);
		return;
	}
	fclose(fl);

	depot_log("file ok");

	char *data = databuf;
	*(data + fsize) = '\0';
	int error = 0;
	OBJ_DATA *obj;

	for (fsize = 0; *data && *data != '$'; fsize++)
	{
		if (!(obj = read_one_object_new(&data, &error)))
		{
			depot_log("reading object error %d", error);
			if (error)
				log("���������: ������ ������ �������� (%s, error: %d).", filename, error);
			continue;
		}
		if (!reload)
		{
			// ������ �������� �� ������� �������� � ��������� ������������ ������
			TimerListType::iterator obj_it = std::find_if(offline_list.begin(), offline_list.end(),
											 boost::bind(std::equal_to<long>(),
														 boost::bind(&OfflineNode::uid, _1), GET_OBJ_UID(obj)));
			if (obj_it != offline_list.end() && obj_it->vnum == GET_OBJ_VNUM(obj))
			{
				depot_log("load object %s %d %d", obj->short_description, GET_OBJ_UID(obj), GET_OBJ_VNUM(obj));
				obj->set_timer(obj_it->timer);
				// ���� ��������� ���� � ���� �� ������, ���� � ���� ������ � ����
				// ������������� � read_one_object_new ����� read_object
				int rnum = real_object(GET_OBJ_VNUM(obj));
				if (rnum >= 0)
					obj_index[rnum].stored--;
			}
			else
			{
				depot_log("extract object %s %d %d", obj->short_description, GET_OBJ_UID(obj), GET_OBJ_VNUM(obj));
				extract_obj(obj);
				continue;
			}
		}
		// ��� ������� �� ������ �� �������, � ������ ���, ��� ����,
		// ���� � ���� � ��� ������������� ��� ������ ������, � �� ������ �� � �� ����

		pers_online.push_front(obj);
		// ������� �� �� ����������� �����, � ������� ��� ���������� ��� �� ������ ������ �� �����
		OBJ_DATA *temp;
		REMOVE_FROM_LIST(obj, object_list, next);
//		ObjectAlias::remove(obj);
	}
	delete [] databuf;
	offline_list.clear();
	reset_cost_per_day();
}

// * ���� ���� � ���� - ������ �� ���������, ���� ������, ���������� � ���-�� ������ �����, ������� ������� � ������.
void enter_char(CHAR_DATA *ch)
{
	DepotListType::iterator it = depot_list.find(GET_UNIQUE(ch));
	if (it != depot_list.end())
	{
		// ������� �����, ���� ���-�� ���� ��������� �� �����
		if (it->second.money_spend > 0)
		{
			send_to_char(ch, "%s���������: �� ����� ������ ���������� �������� %d %s.%s\r\n\r\n",
					CCWHT(ch, C_NRM), it->second.money_spend,
					desc_count(it->second.money_spend, WHAT_MONEYa), CCNRM(ch, C_NRM));

			long rest = ch->remove_both_gold(it->second.money_spend);
			if (rest > 0)
			{
				// ���� �������, ��� ����� �� ������, ������ ��� ������ �������� ��� ������ ��
				// ������ � ���������, � ��������� ��� � �� ��� �������� ��� ���-�� �����
				// ������� �� ������ ������, ���� ���, ��� �������� �������� ��� �����
				depot_log("no money %s %ld: reset depot", GET_NAME(ch), GET_UNIQUE(ch));
				it->second.reset();
				// ���� ������� ����� ��� ������ �� ������ �����,
				// ���� ���� �� ����� ������� ����������� �� ���� ����
				send_to_char(ch, "%s���������: � ��� ��������� ����� �� ������.%s\r\n\r\n",
						CCWHT(ch, C_NRM), CCNRM(ch, C_NRM));
			}
		}
		// ������ ���������, ��������� ��� ��� ����� ��� ������ ���
		it->second.load_online_objs(PERS_DEPOT_FILE);
		// ��������� ����������� ����� � ������������ ch ��� ������ ����� ������
		it->second.money = 0;
		it->second.money_spend = 0;
		it->second.offline_list.clear();
		it->second.ch = ch;
	}
}

// * ������� ������ ������ � �������.
void CharNode::online_to_offline(ObjListType &cont)
{
	for (ObjListType::const_iterator obj_it = cont.begin(); obj_it != cont.end(); ++obj_it)
	{
		depot_log("online_to_offline %s: %s %d %d", name.c_str(), (*obj_it)->short_description, GET_OBJ_UID(*obj_it), GET_OBJ_VNUM(*obj_it));
		OfflineNode tmp_obj;
		tmp_obj.vnum = GET_OBJ_VNUM(*obj_it);
		tmp_obj.timer = (*obj_it)->get_timer();
		tmp_obj.rent_cost = get_object_low_rent(*obj_it);
		tmp_obj.uid = GET_OBJ_UID(*obj_it);
		offline_list.push_back(tmp_obj);
		extract_obj(*obj_it);
		// ������� ������������ ��������� � ����� �����
		add_cost_per_day(tmp_obj.rent_cost);
		// �� ����.� ���� � ���� ��� ������ � �����
		int rnum = real_object(tmp_obj.vnum);
		if (rnum >= 0)
			obj_index[rnum].stored++;
	}
	cont.clear();
}

// * �������� ������ ������ � ���������� � ������ ���������� ����� ����� ���.
void renumber_obj_rnum(int rnum)
{
	depot_log("renumber_obj_rnum");
	for (DepotListType::iterator it = depot_list.begin(); it != depot_list.end(); ++it)
		for (ObjListType::iterator obj_it = it->second.pers_online.begin(); obj_it != it->second.pers_online.end(); ++obj_it)
			if (GET_OBJ_RNUM(*obj_it) >= rnum)
				GET_OBJ_RNUM(*obj_it)++;
}

/**
* ����� ���� �� ���� - ������� �������� � ������� (���� � �����/��/������ � ����������� ����� ����� ����,
* ����-����� ��� ��, '�����'). TODO: ����� ���� ������� ��� �� ����� ���� - ���� ���������, ��� ���� ��
* �� ����� �� ���� (��� �����/������) - � ������ ������ �� ������� ����� ���� ��� ��� ������.
* TODO: ��� �� �� ������ ������� ��� ������� ��� ������ �����, � ������� ��� �������� �������.
*/
void exit_char(CHAR_DATA *ch)
{
	DepotListType::iterator it = depot_list.find(GET_UNIQUE(ch));
	if (it != depot_list.end())
	{
		// ��� ����� ������� ���� ������ �� ����, �.�. ���� ������� ����� � ��� �� �����
		// � ����� � ������� ����� ������ ������ ����� ������ � ��� ���������
		it->second.save_online_objs();

		it->second.online_to_offline(it->second.pers_online);
		it->second.ch = 0;
		it->second.money = ch->get_bank() + ch->get_gold();
		it->second.money_spend = 0;
	}
}

// * ������ ������ ���� �� ������ ������.
void reload_char(long uid, CHAR_DATA *ch)
{
	DepotListType::iterator it = create_depot(uid);
	if (it == depot_list.end())
	{
		log("���������: UID %ld - ��������� ������������ ��� ���������.", uid);
		return;
	}
	// ������� �������� ���, ��� ����
	it->second.reset();

	// ����� ���� ���� �������� ��� �����, ����� ��� ����� ��� ��������� (����� ��������� � exit_char)
	CHAR_DATA *vict = 0, *t_vict = 0;
	DESCRIPTOR_DATA *d = DescByUID(uid);
	if (d)
		vict = d->character; // ��� ������
	else
	{
		// ��� �������������� �������
		t_vict = new Player; // TODO: ���������� �� ����
		if (load_char(it->second.name.c_str(), t_vict) < 0)
		{
			// ������ �� ���������� �������� ����� �������� � do_reboot
			send_to_char(ch, "������������ ��� ��������� (%s).\r\n", it->second.name.c_str());
			delete t_vict;
			t_vict = 0;
		}
		vict = t_vict;
	}
	if (vict)
	{
		// ������ ��� ����� �����: ������ ���. ��������� ��� ������� ������� ���� ��� ����� ����,
		// ������� �� ������ ������ ��� ������� ������ ������, ����� ���� ���������� ��� � �������
		it->second.load_online_objs(PERS_DEPOT_FILE, 1);
		// ���� ���� ������� � ��������
		if (!d) exit_char(vict);
	}
	if (t_vict) delete t_vict;

	snprintf(buf, MAX_STRING_LENGTH, "Depot: %s reload items for %s.", GET_NAME(ch), it->second.name.c_str());
	mudlog(buf, DEF, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), SYSLOG, TRUE);
	imm_log(buf);
}

/**
* ���� ������ � ������������ ����������.
* \param count - ���������� ���-�� ��������� ������������ ������ ����� ������� �� ��������� ���-������.
*/
int print_spell_locate_object(CHAR_DATA *ch, int count, std::string name)
{
	for (DepotListType::iterator it = depot_list.begin(); it != depot_list.end(); ++it)
	{
		for (ObjListType::iterator obj_it = it->second.pers_online.begin(); obj_it != it->second.pers_online.end(); ++obj_it)
		{
			if (number(1, 100) > (40 + MAX((GET_REAL_INT(ch) - 25) * 2, 0)))
				continue;
			if (!isname(name.c_str(), (*obj_it)->aliases))
				continue;

			snprintf(buf, MAX_STRING_LENGTH, "%s �����%s�� � ����-�� � ������������ ���������.\r\n",
					(*obj_it)->short_description, GET_OBJ_POLY_1(ch, (*obj_it)));
			CAP(buf);
			send_to_char(buf, ch);

			if (--count <= 0)
				return count;
		}
	}
	return count;
}

int print_imm_where_obj(CHAR_DATA *ch, char *arg, int num)
{
	for (DepotListType::iterator it = depot_list.begin(); it != depot_list.end(); ++it)
	{
		for (ObjListType::iterator obj_it = it->second.pers_online.begin(); obj_it != it->second.pers_online.end(); ++obj_it)
		{
			if (isname(arg, (*obj_it)->aliases))
			{
				send_to_char(ch, "O%3d. %-25s - �����%s�� � ������������ ��������� (%s).\r\n",
						num++, (*obj_it)->short_description, GET_OBJ_POLY_1(ch, (*obj_it)), it->second.name.c_str());
			}
		}
	}
	return num;
}

// * ���������� ����� �������� ��� ��������� �� ��������� ����� ���.
void olc_update_from_proto(int robj_num, OBJ_DATA *olc_proto)
{
	for (DepotListType::iterator it = depot_list.begin(); it != depot_list.end(); ++it)
	{
		for (ObjListType::iterator obj_it = it->second.pers_online.begin(); obj_it != it->second.pers_online.end(); ++obj_it)
		{
			if (GET_OBJ_RNUM(*obj_it) == robj_num)
				olc_update_object(robj_num, *obj_it, olc_proto);
		}
	}
}

// * ��������� ������� ���������.
void rename_char(CHAR_DATA *ch)
{
	DepotListType::iterator it = depot_list.find(GET_UNIQUE(ch));
	if (it != depot_list.end())
	{
		it->second.name = GET_NAME(ch);
	}
}

// * ����� ���� ��� ����� �������.
OBJ_DATA * locate_object(const char *str)
{
	for (DepotListType::const_iterator i = depot_list.begin(); i != depot_list.end(); ++i)
	{
		for (ObjListType::const_iterator k = i->second.pers_online.begin(); k != i->second.pers_online.end(); ++k)
		{
			if (isname(str, (*k)->aliases))
			{
				return *k;
			}
		}
	}
	return 0;
}

// * ���������� ����� ����, ������������ ������� ��� �������� ��� (���� ��������� �������).
void add_offline_money(long uid, int money)
{
	DepotListType::iterator it = depot_list.find(uid);
	if (it != depot_list.end())
	{
		it->second.money += money;
	}
}

// * ����� � ��������� ����� �� ������ vnum_list.
bool find_set_item(CHAR_DATA *ch, const std::set<int> &vnum_list)
{
	DepotListType::iterator it = depot_list.find(GET_UNIQUE(ch));
	if (it != depot_list.end())
	{
		for (ObjListType::iterator obj_it = it->second.pers_online.begin(),
			obj_it_end = it->second.pers_online.end(); obj_it != obj_it_end; ++obj_it)
		{
			if (vnum_list.find(GET_OBJ_VNUM(*obj_it)) != vnum_list.end())
			{
				return true;
			}
		}
	}
	return false;
}

// * ������ �� ����� ��� ������� ������������ ������ � ��������� � ���������� �� ����-������.
int report_unrentables(CHAR_DATA *ch, CHAR_DATA *recep)
{
	DepotListType::iterator it = depot_list.find(GET_UNIQUE(ch));
	if (it != depot_list.end())
	{
		for (ObjListType::iterator obj_it = it->second.pers_online.begin(),
			obj_it_end = it->second.pers_online.end(); obj_it != obj_it_end; ++obj_it)
		{
			if (SetSystem::is_norent_set(ch, *obj_it))
			{
				snprintf(buf, MAX_STRING_LENGTH,
					"$n ������$g ��� : \"� �� ����� �� ������ %s - ��������� ��� � ����� ���� �� ������.\"",
					OBJN(*obj_it, ch, 3));
				act(buf, FALSE, recep, 0, ch, TO_VICT);
				return 1;
			}
		}
	}
	return 0;
}

// * �������� ��� ������ �� �������� �������� ����-������.
void check_rented(int uid)
{
	DepotListType::iterator it = depot_list.find(uid);
	if (it != depot_list.end())
	{
		for (TimerListType::iterator k = it->second.offline_list.begin(),
			kend = it->second.offline_list.end(); k != kend; ++k)
		{
			SetSystem::check_item(k->vnum);
		}
	}
}

/**
 * ��������� ������� ��������� ������ � ������� ��������� (��� ������).
 * ����.�.���� ������� ��� ����������� ���������� �������� ���������.
 */
void delete_set_item(int uid, int vnum)
{
	DepotListType::iterator i = depot_list.find(uid);
	if (i != depot_list.end())
	{
		TimerListType::iterator k = std::find_if(
			i->second.offline_list.begin(),
			i->second.offline_list.end(),
			boost::bind(std::equal_to<long>(),
				boost::bind(&OfflineNode::vnum, _1), vnum));
		if (k != i->second.offline_list.end())
		{
			log("[TO] Player %s depot : set-item %d deleted",
				i->second.name.c_str(), k->vnum);
			k->timer = -1;
		}
	}
}

} // namespace Depot
