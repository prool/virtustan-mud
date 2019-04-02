// $RCSfile$     $Date$     $Revision$
// Copyright (c) 2009 Krodo
// Part of Bylins http://www.mud.ru

#include <fstream>
#include <string>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "house_exp.hpp"
#include "structs.h"
#include "utils.h"
#include "house.h"
#include "comm.h"
#include "room.hpp"
#include "modify.h"

namespace
{

// ���������� ����� � ������ ����� (������� �� CLAN_EXP_UPDATE_PERIOD)
const unsigned int MAX_LIST_NODES = 720;
// ������� ����� ������ ������� ���� �� � �������, ����� �� ���� ���������
const int MIN_EXP_HISTORY = 50000000;
// ����. ���-�� ������� � ������ ��������
const unsigned MAX_PK_LOG = 15;
// ����. ���-�� ������� � ���� ����-�����
const unsigned MAX_CHEST_LOG = 5000;

} // namespace

// * ���������� ����� �� ��������� ������.
void ClanExp::add_temp(int exp)
{
	buffer_exp_ += exp;
}

// * ���������� ���� � ������ ����� � ������������ ������ ������.
void ClanExp::add_chunk()
{
	list_.push_back(buffer_exp_);
	if (list_.size() > MAX_LIST_NODES)
	{
		list_.erase(list_.begin());
	}
	buffer_exp_ = 0;
	update_total_exp();
}

// * ����� ���-�� ����� � ������.
long long ClanExp::get_exp() const
{
	return total_exp_;
}

// * ���������� ������ � ������� � ��������� ���� ����� (�� ������������).
void ClanExp::save(const std::string &abbrev) const
{
	std::string filename = LIB_HOUSE + abbrev + "/" + abbrev + ".exp";
	std::ofstream file(filename.c_str());
	if (!file.is_open())
	{
		log("Error open file: %s! (%s %s %d)", filename.c_str(), __FILE__, __func__, __LINE__);
		return;
	}
	std::stringstream out;
	out << buffer_exp_ << "\n";
	for (ExpListType::const_iterator it = list_.begin(); it != list_.end(); ++it)
	{
		out << *it << "\n";
	}
	file << out.rdbuf();
}

// * �������� ������ ����� � ������� ����������� ����� (�� ������������).
void ClanExp::load(const std::string &abbrev)
{
	std::string filename = LIB_HOUSE + abbrev + "/" + abbrev + ".exp";
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		log("Error open file: %s! (%s %s %d)", filename.c_str(), __FILE__, __func__, __LINE__);
		return;
	}
	if (!(file >> buffer_exp_))
	{
		log("Error read buffer_exp_: %s! (%s %s %d)", filename.c_str(), __FILE__, __func__, __LINE__);
		return;
	}
	long long tmp_exp;
	while (file >> tmp_exp && list_.size() < MAX_LIST_NODES)
	{
		list_.push_back(tmp_exp);
	}
	update_total_exp();
}

void ClanExp::update_total_exp()
{
	total_exp_ = 0;
	for (ExpListType::const_iterator it = list_.begin(); it != list_.end(); ++it)
	{
		total_exp_ += *it;
	}
}

// * ���������� ���� (��� � ���).
void update_clan_exp()
{
	for (ClanListType::const_iterator clan = Clan::ClanList.begin(); clan != Clan::ClanList.end(); ++clan)
	{
		(*clan)->last_exp.add_chunk();
	}
}

// * ���������� ������� (��� � ��� � �� ������).
void save_clan_exp()
{
	for (ClanListType::const_iterator clan = Clan::ClanList.begin(); clan != Clan::ClanList.end(); ++clan)
	{
		(*clan)->last_exp.save((*clan)->get_file_abbrev());
		(*clan)->exp_history.save((*clan)->get_file_abbrev());
	}
}

////////////////////////////////////////////////////////////////////////////////
// ClanPkLog

void ClanPkLog::add(const std::string &text)
{
	pk_log.push_back(text);
	if (pk_log.size() > MAX_PK_LOG)
	{
		pk_log.pop_front();
	}
	need_save = true;
}

void ClanPkLog::print(CHAR_DATA *ch) const
{
	std::string text;
	for (std::list<std::string>::const_iterator i = pk_log.begin(); i != pk_log.end(); ++i)
	{
		text += *i;
	}

	if (!text.empty())
	{
		send_to_char(ch, "��������� ��-������� � �������� ������ �������:\r\n%s", text.c_str());
	}
	else
	{
		send_to_char("�����.\r\n", ch);
	}
}

void ClanPkLog::save(const std::string &abbrev)
{
	if (!need_save)
	{
		return;
	}

	std::string filename = LIB_HOUSE + abbrev + "/" + abbrev + ".war";
	if (pk_log.empty())
	{
		remove(filename.c_str());
		return;
	}

	std::ofstream file(filename.c_str());
	if (!file.is_open())
	{
		log("Error open file: %s! (%s %s %d)", filename.c_str(), __FILE__, __func__, __LINE__);
		return;
	}

	for (std::list<std::string>::const_iterator i = pk_log.begin(); i != pk_log.end(); ++i)
	{
		file << *i;
	}

	file.close();
	need_save = false;
}

void ClanPkLog::load(const std::string &abbrev)
{
	std::string filename = LIB_HOUSE + abbrev + "/" + abbrev + ".war";

	std::ifstream file(filename.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		log("Error open file: %s! (%s %s %d)", filename.c_str(), __FILE__, __func__, __LINE__);
		return;
	}

	std::string buffer;
	while(std::getline(file, buffer))
	{
		boost::trim(buffer);
		buffer += "\r\n";
		pk_log.push_back(buffer);
	}
	file.close();
}

void ClanPkLog::check(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (!ch || !victim || ch->purged() || victim->purged()
		|| IS_NPC(victim) || !CLAN(victim) || ch == victim
		|| (ROOM_FLAGGED(IN_ROOM(victim), ROOM_ARENA) && !RENTABLE(victim)))
	{
		return;
	}
	CHAR_DATA *killer = ch;
	if (IS_NPC(killer)
		&& killer->master && !IS_NPC(killer->master))
	{
		killer = killer->master;
	}
	if (!IS_NPC(killer) && CLAN(killer) != CLAN(victim))
	{
		char timeBuf[20];
		time_t curr_time = time(0);
		strftime(timeBuf, sizeof(timeBuf), "%d-%m-%Y (%H:%M)", localtime(&curr_time));
		std::stringstream out;
		out << timeBuf << ": "
			<< GET_NAME(victim) << " ����" << GET_CH_SUF_6(victim) << " "
			<< GET_PAD(killer, 4) << "\r\n";

		CLAN(victim)->pk_log.add(out.str());
		if (CLAN(killer))
		{
			CLAN(killer)->pk_log.add(out.str());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// ClanExpHistory

void ClanExpHistory::add_exp(long exp)
{
	if (exp < 0)
	{
		return;
	}
	char time_str[10];
	time_t curr_time = time(0);
	strftime(time_str, sizeof(time_str), "%Y.%m", localtime(&curr_time));
	std::string time_cpp_str(time_str);

	HistoryExpListType::iterator it = list_.find(time_cpp_str);
	if (it != list_.end())
	{
		it->second += exp;
	}
	else
	{
		list_[time_cpp_str] = exp;
	}
}

void ClanExpHistory::load(const std::string &abbrev)
{
	std::string filename = LIB_HOUSE + abbrev + "/" + abbrev + "-history.exp";

	std::ifstream file(filename.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		log("Error open file: %s! (%s %s %d)", filename.c_str(), __FILE__, __func__, __LINE__);
		return;
	}

	std::string buffer;
	long long exp;
	while(file >> buffer >> exp)
	{
		list_[buffer] = exp;
	}
	file.close();
}

void ClanExpHistory::save(const std::string &abbrev) const
{
	std::string filename = LIB_HOUSE + abbrev + "/" + abbrev + "-history.exp";
	if (list_.empty())
	{
		remove(filename.c_str());
		return;
	}

	std::ofstream file(filename.c_str());
	if (!file.is_open())
	{
		log("Error open file: %s! (%s %s %d)", filename.c_str(), __FILE__, __func__, __LINE__);
		return;
	}

	for (HistoryExpListType::const_iterator i = list_.begin(); i != list_.end(); ++i)
	{
		file << i->first << " " << i->second << "\n";
	}
	file.close();
}

/**
* \param - ���������� ��������� �������, ����������� � �������
* \return - ��������� ������ ����� �� ��������� num �������
*/
long long ClanExpHistory::get(int month) const
{
	long long exp = 0;
	int count = 0;
	for (HistoryExpListType::const_reverse_iterator i = list_.rbegin(), iend = list_.rend(); i != iend; ++i, ++count)
	{
		if (count <= month)
		{
			exp += i->second;
		}
		else
		{
			break;
		}
	}
	return exp;
}

//
// � ������ ������ ��������� ��� ��������� ������ ����������� ������.
// ����� ������ ���� ������������ ������� - ������� ����� ������ �������� � ����
// ��� ������� 4 �������� (�������� �������� ����� + 2 ������ + ������� ��������).
// � ������ ���� �������������� ������ � ������ ������ � ����� ������.
//
long long ClanExpHistory::calc_exp_history() const
{
	long long exp = 0;
	int count = 1;
	for (HistoryExpListType::const_reverse_iterator i = list_.rbegin(), iend = list_.rend(); i != iend; ++i, ++count)
	{
		if (count > 3)
		{
			break;
		}
		if (count > 1)
		{
			exp += i->second;
		}
	}
	return exp;
}

bool ClanExpHistory::need_destroy() const
{
	if (list_.size() < 4)
	{
		return false;
	}
	return calc_exp_history() < MIN_EXP_HISTORY ? true : false;
}

void ClanExpHistory::show(CHAR_DATA *ch) const
{
	send_to_char(ch, "\r\n����, ��������� �� ��� ��������� ����������� ������ ��� ����� �������:\r\n");
	int num_print = list_.size() - 3;
	int count = 0;
	for (HistoryExpListType::const_iterator i = list_.begin(), iend = list_.end(); i != iend; ++i, ++count)
	{
		if (count >= num_print)
		{
			send_to_char(ch, "%s : %14s\r\n", i->first.c_str(), thousands_sep(i->second).c_str());
		}
	}
	send_to_char(ch, "����������, ��� � ������� �������������� ������� ���������� ������ �����������\r\n"
			"����, ��������� �� ��� ��������� ������ ����������� ������ ( >= %s � �����);\r\n"
			"������ �� ���������� %s.\r\n", thousands_sep(MIN_EXP_HISTORY).c_str(),
			                             thousands_sep(calc_exp_history()).c_str());
}

////////////////////////////////////////////////////////////////////////////////
void ClanChestLog::add(const std::string &text)
{
	char timeBuf[20];
	time_t curr_time = time(0);
	strftime(timeBuf, sizeof(timeBuf), "[%d/%m/%y %H:%M] ", localtime(&curr_time)); // prool: add %y

	if (chest_log_.size() >= MAX_CHEST_LOG)
	{
		chest_log_.pop_back();
	}

	chest_log_.push_front(timeBuf + text);
	need_save_ = true;
}

void ClanChestLog::print(CHAR_DATA *ch, std::string &text) const
{
	boost::trim(text);
	std::string out;
	if (text.empty())
	{
		out += "������� ��������� �������:\r\n";
		for (std::list<std::string>::const_iterator i = chest_log_.begin(),
			iend = chest_log_.end(); i != iend; ++i)
		{
			out += *i;
		}
		out += "\r\n";
		page_string(ch->desc, out);
	}
	else
	{
		out += "������� �� ������� ��������� ������� (" + text + "):\r\n";
		for (std::list<std::string>::const_iterator i = chest_log_.begin(),
			iend = chest_log_.end(); i != iend; ++i)
		{
			if ((*i).find(text) != std::string::npos)
			{
				out += *i;
			}
		}
		out += "\r\n";
		page_string(ch->desc, out);
	}
}

void ClanChestLog::save(const std::string &abbrev)
{
	if (!need_save_)
	{
		return;
	}

	std::string filename = LIB_HOUSE + abbrev + "/" + abbrev + ".log";
	if (chest_log_.empty())
	{
		remove(filename.c_str());
		return;
	}

	std::ofstream file(filename.c_str());
	if (!file.is_open())
	{
		log("Error open file: %s! (%s %s %d)",
				filename.c_str(), __FILE__, __func__, __LINE__);
		return;
	}

	for (std::list<std::string>::const_iterator i = chest_log_.begin(),
		iend = chest_log_.end(); i != iend; ++i)
	{
		file << *i;
	}

	file.close();
	need_save_ = false;
}

void ClanChestLog::load(const std::string &abbrev)
{
	std::string filename = LIB_HOUSE + abbrev + "/" + abbrev + ".log";

	std::ifstream file(filename.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		return;
	}

	std::string buffer;
	while(std::getline(file, buffer))
	{
		boost::trim(buffer);
		buffer += "\r\n";
		chest_log_.push_back(buffer);
	}
	file.close();
}
////////////////////////////////////////////////////////////////////////////////
