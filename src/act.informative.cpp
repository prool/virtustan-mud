/* ************************************************************************
*   File: act.informative.cpp                           Part of Bylins    *
*  Usage: Player-level commands of an informative nature                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
*                                                                         *
*  $Author$                                                        *
*  $Date$                                           *
*  $Revision$                                                       *
************************************************************************ */

#include "conf.h"
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "skills.h"
#include "fight.h"
#include "screen.h"
#include "constants.h"
#include "pk.h"
#include "dg_scripts.h"
#include "mail.h"
#include "features.hpp"
#include "im.h"
#include "house.h"
#include "description.h"
#include "privilege.hpp"
#include "depot.hpp"
#include "glory.hpp"
#include "random.hpp"
#include "char.hpp"
#include "char_player.hpp"
#include "parcel.hpp"
#include "liquid.hpp"
#include "modify.h"
#include "room.hpp"
#include "glory_const.hpp"
#include "player_races.hpp"
#include "corpse.hpp"
#include "sets_drop.hpp"
#include "help.hpp"
#include "map.hpp"
#include "ext_money.hpp"
#include "mob_stat.hpp"

#include "virtustan.h" // prool

using std::string;

// extern variables
extern DESCRIPTOR_DATA *descriptor_list;
extern CHAR_DATA *character_list;
extern OBJ_DATA *object_list;
extern vector < OBJ_DATA * >obj_proto;
extern int top_of_socialk;
extern char *credits;
extern char *info;
extern char *motd;
extern char *rules;
extern char *immlist;
extern char *policies;
extern char *handbook;
extern char const *class_abbrevs[];
extern char const *kin_abbrevs[];
extern const char *material_name[];
extern im_type *imtypes;
extern int top_imtypes;
extern void show_code_date(CHAR_DATA *ch);
extern int nameserver_is_slow; //config.cpp
extern int top_of_p_table;

// prool's extern:
extern int total_players;
extern int webstat;
extern char mudname[];
extern char wholist_file[];

// extern functions
long find_class_bitvector(char arg);
int level_exp(CHAR_DATA * ch, int level);
TIME_INFO_DATA *real_time_passed(time_t t2, time_t t1);
int compute_armor_class(CHAR_DATA * ch);
int low_charm(CHAR_DATA * ch);
int pk_count(CHAR_DATA * ch);
// local functions
void print_object_location(int num, OBJ_DATA * obj, CHAR_DATA * ch, int recur);
const char *show_obj_to_char(OBJ_DATA * object, CHAR_DATA * ch, int mode, int show_state, int how);
void list_obj_to_char(OBJ_DATA * list, CHAR_DATA * ch, int mode, int show);
char *diag_obj_to_char(CHAR_DATA * i, OBJ_DATA * obj, int mode);
const char *diag_obj_timer(const OBJ_DATA * obj);
char *diag_timer_to_char(const OBJ_DATA * obj);
const char * print_god_or_player(int level);
int get_pick_chance(int skill_pick, int lock_complexity);
int thaco(int class_num, int level);

ACMD(do_affects);
ACMD(do_look);
ACMD(do_examine);
ACMD(do_gold);
ACMD(do_score);
ACMD(do_inventory);
ACMD(do_equipment);
ACMD(do_time);
ACMD(do_weather);
ACMD(do_who);
ACMD(do_users);
ACMD(do_gen_ps);
void perform_mortal_where(CHAR_DATA * ch, char *arg);
void perform_immort_where(CHAR_DATA * ch, char *arg);
ACMD(do_where);
ACMD(do_levels);
ACMD(do_consider);
ACMD(do_diagnose);
ACMD(do_color);
ACMD(do_toggle);
void sort_commands(void);
ACMD(do_commands);
ACMD(do_looking);
ACMD(do_hearing);
ACMD(do_sides);
void diag_char_to_char(CHAR_DATA * i, CHAR_DATA * ch);
void look_at_char(CHAR_DATA * i, CHAR_DATA * ch);
void list_one_char(CHAR_DATA * i, CHAR_DATA * ch, int skill_mode);
void list_char_to_char(CHAR_DATA * list, CHAR_DATA * ch);
void do_auto_exits(CHAR_DATA * ch);
ACMD(do_exits);
void look_in_direction(CHAR_DATA * ch, int dir, int info_is);
void look_in_obj(CHAR_DATA * ch, char *arg);
char *find_exdesc(char *word, EXTRA_DESCR_DATA * list);
bool look_at_target(CHAR_DATA * ch, char *arg, int subcmd);
void gods_day_now(CHAR_DATA * ch);
#define EXIT_SHOW_WALL    (1 << 0)
#define EXIT_SHOW_LOOKING (1 << 1)
/*
 * This function screams bitvector... -gg 6/45/98
 */

int param_sort = 0;

const char *Dirs[NUM_OF_DIRS + 1] = { "�����",
									  "������",
									  "��",
									  "�����",
									  "�����",
									  "����",
									  "\n"
									};

const char *ObjState[8][2] = { {"�����������", "�����������"},
	{"��������", "� ��������� ���������"},
	{"�����", "� ������ ���������"},
	{"�������", "� �������� ���������"},
	{"������", "� ������� ���������"},
	{"������", "� ������� ���������"},
	{"����� ������", "� ����� ������� ���������"},
	{"�����������", "� ������������ ���������"}
};

const char *Locks[4][2] =
{
	{"%s �� � ����� �� ������ ��������� �����.%s\r\n", KIRED},
	{"%s ����� ����� �������.%s\r\n", KIYEL},
	{"%s ������� �����. ��� �� �� �������.%s\r\n", KIGRN},
	{"%s ������� �����. ��� ��������.%s\r\n", KGRN}
};

char *diag_obj_to_char(CHAR_DATA * i, OBJ_DATA * obj, int mode)
{
	static char out_str[80] = "\0";
	const char *color;
	int percent;

	if (GET_OBJ_MAX(obj) > 0)
		percent = 100 * GET_OBJ_CUR(obj) / GET_OBJ_MAX(obj);
	else
		percent = -1;

	if (percent >= 100)
	{
		percent = 7;
		color = CCWHT(i, C_NRM);
	}
	else if (percent >= 90)
	{
		percent = 6;
		color = CCIGRN(i, C_NRM);
	}
	else if (percent >= 75)
	{
		percent = 5;
		color = CCGRN(i, C_NRM);
	}
	else if (percent >= 50)
	{
		percent = 4;
		color = CCIYEL(i, C_NRM);
	}
	else if (percent >= 30)
	{
		percent = 3;
		color = CCIRED(i, C_NRM);
	}
	else if (percent >= 15)
	{
		percent = 2;
		color = CCRED(i, C_NRM);
	}
	else if (percent > 0)
	{
		percent = 1;
		color = CCNRM(i, C_NRM);
	}
	else
	{
		percent = 0;
		color = CCINRM(i, C_NRM);
	}

	if (mode == 1)
		sprintf(out_str, " %s<%s>%s", color, ObjState[percent][0], CCNRM(i, C_NRM));
	else if (mode == 2)
		strcpy(out_str, ObjState[percent][1]);
	return out_str;
}


const char *weapon_class[] = { "����",
							   "�������� ������",
							   "������� ������",
							   "������",
							   "������ � ������",
							   "���� ������",
							   "����������",
							   "����������� ������",
							   "����� � ��������"
							 };

char *diag_weapon_to_char(const OBJ_DATA * obj, int show_wear)
{
	static char out_str[MAX_STRING_LENGTH];
	int skill = 0;
	int need_str = 0;

	*out_str = '\0';
	switch (GET_OBJ_TYPE(obj))
	{
	case ITEM_WEAPON:
		switch (GET_OBJ_SKILL(obj))
		{
		case SKILL_BOWS:
			skill = 1;
			break;
		case SKILL_SHORTS:
			skill = 2;
			break;
		case SKILL_LONGS:
			skill = 3;
			break;
		case SKILL_AXES:
			skill = 4;
			break;
		case SKILL_CLUBS:
			skill = 5;
			break;
		case SKILL_NONSTANDART:
			skill = 6;
			break;
		case SKILL_BOTHHANDS:
			skill = 7;
			break;
		case SKILL_PICK:
			skill = 8;
			break;
		case SKILL_SPADES:
			skill = 9;
			break;
		default:
			sprintf(out_str, "!! �� ����������� � ��������� ����� ������ - �������� ����� !!\r\n");
		}
		if (skill)
			sprintf(out_str, "����������� � ������ \"%s\".\r\n", weapon_class[skill - 1]);
	default:
		if (show_wear)
		{
			if (CAN_WEAR(obj, ITEM_WEAR_FINGER))
				sprintf(out_str + strlen(out_str), "����� ������ �� �����.\r\n");
			if (CAN_WEAR(obj, ITEM_WEAR_NECK))
				sprintf(out_str + strlen(out_str), "����� ������ �� ���.\r\n");
			if (CAN_WEAR(obj, ITEM_WEAR_BODY))
				sprintf(out_str + strlen(out_str), "����� ������ �� ��������.\r\n");
			if (CAN_WEAR(obj, ITEM_WEAR_HEAD))
				sprintf(out_str + strlen(out_str), "����� ������ �� ������.\r\n");
			if (CAN_WEAR(obj, ITEM_WEAR_LEGS))
				sprintf(out_str + strlen(out_str), "����� ������ �� ����.\r\n");
			if (CAN_WEAR(obj, ITEM_WEAR_FEET))
				sprintf(out_str + strlen(out_str), "����� �����.\r\n");
			if (CAN_WEAR(obj, ITEM_WEAR_HANDS))
				sprintf(out_str + strlen(out_str), "����� ������ �� �����.\r\n");
			if (CAN_WEAR(obj, ITEM_WEAR_ARMS))
				sprintf(out_str + strlen(out_str), "����� ������ �� ����.\r\n");
			if (CAN_WEAR(obj, ITEM_WEAR_ABOUT))
				sprintf(out_str + strlen(out_str), "����� ������ �� �����.\r\n");
			if (CAN_WEAR(obj, ITEM_WEAR_WAIST))
				sprintf(out_str + strlen(out_str), "����� ������ �� ����.\r\n");
			if (CAN_WEAR(obj, ITEM_WEAR_WRIST))
				sprintf(out_str + strlen(out_str), "����� ������ �� ��������.\r\n");
			if (show_wear > 1)
			{
				if (CAN_WEAR(obj, ITEM_WEAR_SHIELD))
				{
					need_str = MAX(0, calc_str_req((GET_OBJ_WEIGHT(obj)+1)/2, STR_HOLD_W));
					sprintf(out_str + strlen(out_str), "����� ������������ ��� ��� (��������� %d %s).\r\n", need_str, desc_count(need_str, WHAT_STR));
				}
				if (CAN_WEAR(obj, ITEM_WEAR_WIELD))
				{
					need_str = MAX(0, calc_str_req(GET_OBJ_WEIGHT(obj), STR_WIELD_W));
					sprintf(out_str + strlen(out_str), "����� ����� � ������ ���� (��������� %d %s).\r\n", need_str, desc_count(need_str, WHAT_STR));
				}
				if (CAN_WEAR(obj, ITEM_WEAR_HOLD))
				{
					need_str = MAX(0, calc_str_req(GET_OBJ_WEIGHT(obj), STR_HOLD_W));
					sprintf(out_str + strlen(out_str), "����� ����� � ����� ���� (��������� %d %s).\r\n", need_str, desc_count(need_str, WHAT_STR));
				}
				if (CAN_WEAR(obj, ITEM_WEAR_BOTHS))
				{
					need_str = MAX(0, calc_str_req(GET_OBJ_WEIGHT(obj), STR_BOTH_W));
					sprintf(out_str + strlen(out_str), "����� ����� � ��� ���� (��������� %d %s).\r\n", need_str, desc_count(need_str, WHAT_STR));
				}
			}
			else
			{
				if (CAN_WEAR(obj, ITEM_WEAR_SHIELD))
					sprintf(out_str + strlen(out_str), "����� ������������ ��� ���.\r\n");
				if (CAN_WEAR(obj, ITEM_WEAR_WIELD))
					sprintf(out_str + strlen(out_str), "����� ����� � ������ ����.\r\n");
				if (CAN_WEAR(obj, ITEM_WEAR_HOLD))
					sprintf(out_str + strlen(out_str), "����� ����� � ����� ����.\r\n");
				if (CAN_WEAR(obj, ITEM_WEAR_BOTHS))
					sprintf(out_str + strlen(out_str), "����� ����� � ��� ����.\r\n");
			}
		}
	}
	return (out_str);
}

// ����� ����� ���� �������� ������ ������ ��������
const char *diag_obj_timer(const OBJ_DATA * obj)
{
	if (GET_OBJ_RNUM(obj) != NOTHING)
	{
		int prot_timer = obj_proto[GET_OBJ_RNUM(obj)]->get_timer();
		if (!prot_timer)
		{
			return "�������� �������� ����� ������� ������!\r\n";
		}
		int tm = (obj->get_timer() * 100 / prot_timer);
		return print_obj_state(tm);
	}
	return "";
}

char *diag_timer_to_char(const OBJ_DATA * obj)
{
	static char out_str[MAX_STRING_LENGTH];
	*out_str = 0;
	sprintf(out_str, "���������: %s.\r\n", diag_obj_timer(obj));
	return (out_str);
}


char *diag_uses_to_char(OBJ_DATA * obj, CHAR_DATA * ch)
{
	static char out_str[MAX_STRING_LENGTH];

	*out_str = 0;
	if (GET_OBJ_TYPE(obj) == ITEM_INGRADIENT && IS_SET(GET_OBJ_SKILL(obj), ITEM_CHECK_USES)
			&& GET_CLASS(ch) == CLASS_DRUID)
	{
		int i = -1;
		if ((i = real_object(GET_OBJ_VAL(obj, 1))) >= 0)
		{
			sprintf(out_str, "��������: %s%s%s.\r\n",
					CCICYN(ch, C_NRM), obj_proto[i]->PNames[0], CCNRM(ch, C_NRM));
		}
		sprintf(out_str + strlen(out_str), "�������� ����������: %s%d&n.\r\n",
				GET_OBJ_VAL(obj, 2) > 100 ? "&G" : "&R", GET_OBJ_VAL(obj, 2));
	}
	return (out_str);
}

/**
* ��� ������ ����� � ������� ���� � ��� �������� ����������� � ������ ������ ������ ������
* (��� ������ ������), ��������� ������� ������� ������ �����!
*/
std::string space_before_string(char const *text)
{
	if (text)
	{
		std::string tmp(" ");
		tmp += text;
		boost::replace_all(tmp, "\n", "\n ");
		boost::trim_right_if(tmp, boost::is_any_of(std::string(" ")));
		return tmp;
	}
	return "";
}

namespace
{

std::string diag_armor_type_to_char(const OBJ_DATA *obj)
{
	if (GET_OBJ_TYPE(obj) == ITEM_ARMOR_LIGHT)
	{
		return "������ ��� ��������.\r\n";
	}
	if (GET_OBJ_TYPE(obj) == ITEM_ARMOR_MEDIAN)
	{
		return "������� ��� ��������.\r\n";
	}
	if (GET_OBJ_TYPE(obj) == ITEM_ARMOR_HEAVY)
	{
		return "������� ��� ��������.\r\n";
	}
	return "";
}

} // namespace

// ��� ������������� � ������:
// ���������� ����� ��������, ���� ��� ���� � ��������� �������� �� ������� ��� �������� ������ �����. �����
std::string char_get_custom_label(OBJ_DATA *obj, CHAR_DATA *ch)
{
	const char *delim_l = NULL;
	const char *delim_r = NULL;

	// ������ ������ ��� �������� � ������
	if (obj->custom_label != NULL && (ch->player_specials->clan && obj->custom_label->clan != NULL &&
	    !strcmp(obj->custom_label->clan, ch->player_specials->clan->GetAbbrev())))
	{
		delim_l = " *";
		delim_r = "*";
	} else {
		delim_l = " (";
		delim_r = ")";
	}

	if (AUTH_CUSTOM_LABEL(obj, ch))
		return boost::str(boost::format("%s%s%s") % delim_l % obj->custom_label->label_text % delim_r);

	return "";
}

// mode 1 show_state 3 ��� ��������� (4 - ��������� ������)
const char *show_obj_to_char(OBJ_DATA * object, CHAR_DATA * ch, int mode, int show_state, int how)
{
	*buf = '\0';
	if ((mode < 5) && PRF_FLAGGED(ch, PRF_ROOMFLAGS))
		sprintf(buf, "[%5d] ", GET_OBJ_VNUM(object));

	if ((mode == 0) && object->description)
	{
		strcat(buf, object->description);
		strcat(buf, char_get_custom_label(object, ch).c_str());
	}
	else if (object->short_description && ((mode == 1) || (mode == 2) || (mode == 3) || (mode == 4)))
	{
		strcat(buf, object->short_description);
		strcat(buf, char_get_custom_label(object, ch).c_str());
	}
	else if (mode == 5)
	{
		if (GET_OBJ_TYPE(object) == ITEM_NOTE)
		{
			if (object->action_description)
			{
				strcpy(buf, "�� ��������� ��������� :\r\n\r\n");
				strcat(buf, space_before_string(object->action_description).c_str());
				page_string(ch->desc, buf, 1);
			}
			else
				send_to_char("�����.\r\n", ch);
			return 0;
		}
		else if (GET_OBJ_TYPE(object) == ITEM_BANDAGE)
		{
			strcpy(buf, "����� ��� ��������� ��� ('����������').\r\n");
			snprintf(buf2, MAX_STRING_LENGTH,
					"�������� ����������: %d, ��������������: %d",
					GET_OBJ_WEIGHT(object), GET_OBJ_VAL(object, 0) * 10);
			strcat(buf, buf2);
		}
		else if (GET_OBJ_TYPE(object) != ITEM_DRINKCON)
		{
			strcpy(buf, "�� �� ������ ������ ����������.");
		}
		else		// ITEM_TYPE == ITEM_DRINKCON||FOUNTAIN
			strcpy(buf, "��� ������� ��� ��������.");
	}

	if (show_state && show_state != 3 && show_state != 4)
	{
		*buf2 = '\0';
		if (mode == 1 && how <= 1)
		{
			if (GET_OBJ_TYPE(object) == ITEM_LIGHT)
			{
				if (GET_OBJ_VAL(object, 2) == -1)
					strcpy(buf2, " (������ ����)");
				else if (GET_OBJ_VAL(object, 2) == 0)
					sprintf(buf2, " (�����%s)", GET_OBJ_SUF_4(object));
				else
					sprintf(buf2, " (%d %s)",
							GET_OBJ_VAL(object, 2), desc_count(GET_OBJ_VAL(object, 2), WHAT_HOUR));
			}
			else
			{
				if (object->timed_spell.is_spell_poisoned() != -1)
				{
					sprintf(buf2, " %s*%s%s", CCGRN(ch, C_NRM),
						CCNRM(ch, C_NRM), diag_obj_to_char(ch, object, 1));
				}
				else
				{
					sprintf(buf2, " %s", diag_obj_to_char(ch, object, 1));
				}
			}
			if (GET_OBJ_TYPE(object) == ITEM_CONTAINER)
			{
				if (object->contains)
					strcat(buf2, " (���� ����������)");
				else
					sprintf(buf2 + strlen(buf2), " (����%s)", GET_OBJ_SUF_6(object));
			}
		}
		else if (mode >= 2 && how <= 1)
		{
			std::string obj_name = OBJN(object, ch, 0);
			obj_name[0] = UPPER(obj_name[0]);
			if (GET_OBJ_TYPE(object) == ITEM_LIGHT)
			{
				if (GET_OBJ_VAL(object, 2) == -1)
					sprintf(buf2, "\r\n%s ���� ������ ����.", obj_name.c_str());
				else if (GET_OBJ_VAL(object, 2) == 0)
					sprintf(buf2, "\r\n%s �����%s.", obj_name.c_str(), GET_OBJ_SUF_4(object));
				else
					sprintf(buf2, "\r\n%s ����� ������� %d %s.", obj_name.c_str(), GET_OBJ_VAL(object, 2),
							desc_count(GET_OBJ_VAL(object, 2), WHAT_HOUR));
			}
			else if (GET_OBJ_CUR(object) < GET_OBJ_MAX(object))
				sprintf(buf2, "\r\n%s %s.", obj_name.c_str(), diag_obj_to_char(ch, object, 2));
		}
		strcat(buf, buf2);
	}
	if (how > 1)
		sprintf(buf + strlen(buf), " [%d]", how);
	if (mode != 3 && how <= 1)
	{
		if (IS_OBJ_STAT(object, ITEM_INVISIBLE))
		{
			sprintf(buf2, " (�������%s)", GET_OBJ_SUF_6(object));
			strcat(buf, buf2);
		}
		if (IS_OBJ_STAT(object, ITEM_BLESS)
				&& AFF_FLAGGED(ch, AFF_DETECT_ALIGN))
			strcat(buf, " ..������� ����!");
		if (IS_OBJ_STAT(object, ITEM_MAGIC)
				&& AFF_FLAGGED(ch, AFF_DETECT_MAGIC))
			strcat(buf, " ..������ ����!");
		if (IS_OBJ_STAT(object, ITEM_POISONED)
				&& AFF_FLAGGED(ch, AFF_DETECT_POISON))
		{
			sprintf(buf2, "..��������%s!", GET_OBJ_SUF_6(object));
			strcat(buf, buf2);
		}
		if (IS_OBJ_STAT(object, ITEM_GLOW))
			strcat(buf, " ..�������!");
		if (IS_OBJ_STAT(object, ITEM_HUM) && !AFF_FLAGGED(ch, AFF_SIELENCE))
			strcat(buf, " ..�����!");
		if (IS_OBJ_STAT(object, ITEM_FIRE))
			strcat(buf, " ..�����!");
		if (IS_OBJ_STAT(object, ITEM_BLOODY))
			{
		sprintf(buf2, " %s..������%s ������!%s", CCIRED(ch, C_NRM), GET_OBJ_SUF_6(object), CCNRM(ch, C_NRM));
			strcat(buf, buf2);
		}
	}

	if (mode == 1)
	{
		// ����-������, ������� ������ ����� �����������
		if (show_state == 3)
		{
			sprintf(buf + strlen(buf), " [%d %s]\r\n",
					GET_OBJ_RENTEQ(object) * CLAN_STOREHOUSE_COEFF / 100,
					desc_count(GET_OBJ_RENTEQ(object) * CLAN_STOREHOUSE_COEFF / 100, WHAT_MONEYa));
			return buf;
		}
		// �����
		else if (show_state == 4)
		{
			sprintf(buf + strlen(buf), " [%d %s]\r\n", GET_OBJ_RENT(object),
					desc_count(GET_OBJ_RENT(object), WHAT_MONEYa));
			return buf;
		}
	}

	strcat(buf, "\r\n");
	if (mode >= 5)
	{
		strcat(buf, diag_weapon_to_char(object, TRUE));
		strcat(buf, diag_armor_type_to_char(object).c_str());
		strcat(buf, diag_timer_to_char(object));
		//strcat(buf, diag_uses_to_char(object, ch)); // commented by WorM ������� � obj_info ����� ������ ��� ���� ����� �� ������/����
		strcat(buf, object->timed_spell.diag_to_char(ch).c_str());
	}
	page_string(ch->desc, buf, TRUE);
	return 0;
}

void list_obj_to_char(OBJ_DATA * list, CHAR_DATA * ch, int mode, int show)
{
	OBJ_DATA *i, *push = NULL;
	bool found = FALSE;
	int push_count = 0;
	std::ostringstream buffer;
	long count = 0, cost = 0;

	bool clan_chest = false;
	if (mode == 1 && (show == 3 || show == 4))
	{
		clan_chest = true;
	}

	for (i = list; i; i = i->next_content)
	{
		if (CAN_SEE_OBJ(ch, i))
		{
			if (!push)
			{
				push = i;
				push_count = 1;
			}
			else if (!equal_obj(i, push))
			{
				if (clan_chest)
				{
					buffer << show_obj_to_char(push, ch, mode, show, push_count);
					count += push_count;
					cost += GET_OBJ_RENTEQ(push) * push_count;
				}
				else
					show_obj_to_char(push, ch, mode, show, push_count);
				push = i;
				push_count = 1;
			}
			else
				push_count++;
			found = TRUE;
		}
	}
	if (push && push_count)
	{
		if (clan_chest)
		{
			buffer << show_obj_to_char(push, ch, mode, show, push_count);
			count += push_count;
			cost += GET_OBJ_RENTEQ(push) * push_count;
		}
		else
			show_obj_to_char(push, ch, mode, show, push_count);
	}
	if (!found && show)
	{
		if (show == 1)
			send_to_char(" ������ ������ ���.\r\n", ch);
		else if (show == 2)
			send_to_char(" �� ������ �� ������.\r\n", ch);
		else if (show == 3)
		{
			send_to_char(" �����...\r\n", ch);
			return;
		}
	}
	if (clan_chest)
		page_string(ch->desc, buffer.str());
}

void diag_char_to_char(CHAR_DATA * i, CHAR_DATA * ch)
{
	int percent;

	if (GET_REAL_MAX_HIT(i) > 0)
		percent = (100 * GET_HIT(i)) / GET_REAL_MAX_HIT(i);
	else
		percent = -1;	// How could MAX_HIT be < 1??

	strcpy(buf, PERS(i, ch, 0));
	CAP(buf);

	if (percent >= 100)
	{
		sprintf(buf2, " ��������%s", GET_CH_SUF_6(i));
		strcat(buf, buf2);
	}
	else if (percent >= 90)
	{
		sprintf(buf2, " ������ ���������%s", GET_CH_SUF_6(i));
		strcat(buf, buf2);
	}
	else if (percent >= 75)
	{
		sprintf(buf2, " ����� �����%s", GET_CH_SUF_6(i));
		strcat(buf, buf2);
	}
	else if (percent >= 50)
	{
		sprintf(buf2, " �����%s", GET_CH_SUF_6(i));
		strcat(buf, buf2);
	}
	else if (percent >= 30)
	{
		sprintf(buf2, " ������ �����%s", GET_CH_SUF_6(i));
		strcat(buf, buf2);
	}
	else if (percent >= 15)
	{
		sprintf(buf2, " ���������� �����%s", GET_CH_SUF_6(i));
		strcat(buf, buf2);
	}
	else if (percent >= 0)
		strcat(buf, " � ������� ���������");
	else
		strcat(buf, " �������");

	if (!on_horse(i))
		switch (GET_POS(i))
			{
			case POS_MORTALLYW:
				strcat(buf, ".");
			break;
			case POS_INCAP:
				strcat(buf, IS_POLY(i) ? ", ����� ��� ��������." : ", ����� ��� ��������.");
			break;
			case POS_STUNNED:
				strcat(buf, IS_POLY(i) ? ", ����� � ��������." : ", ����� � ��������.");
			break;
			case POS_SLEEPING:
				strcat(buf, IS_POLY(i) ? ", ����." : ", ����.");
			break;
			case POS_RESTING:
				strcat(buf, IS_POLY(i) ? ", ��������." : ", ��������.");
			break;
			case POS_SITTING:
				strcat(buf, IS_POLY(i) ? ", �����." : ", �����.");
			break;
			case POS_STANDING:
				strcat(buf, IS_POLY(i) ? ", �����." : ", �����.");
			break;
			case POS_FIGHTING:
				if (i->get_fighting())
					strcat(buf, IS_POLY(i) ? ", ���������." : ", ���������.");
				else
					strcat(buf, IS_POLY(i) ? ", ������ ��������." : ", ������ ��������.");
			break;
			default:
				return;
			break;
		}
	else
		strcat(buf, IS_POLY(i) ? ", ����� ������." : ", ����� ������.");

	if (AFF_FLAGGED(ch, AFF_DETECT_POISON))
		if (AFF_FLAGGED(i, AFF_POISON))
		{
			sprintf(buf2, " (��������%s)", GET_CH_SUF_6(i));
			strcat(buf, buf2);
		}

	strcat(buf, "\r\n");
	send_to_char(buf, ch);

}


void look_at_char(CHAR_DATA * i, CHAR_DATA * ch)
{
	int j, found, push_count = 0;
	OBJ_DATA *tmp_obj, *push = NULL;

	if (!ch->desc)
		return;

	if (i->player_data.description && *i->player_data.description)
	{
		if (IS_NPC(i))
			send_to_char(ch, " * %s", i->player_data.description);
		else
			send_to_char(ch, "*\r\n%s*\r\n", space_before_string(i->player_data.description).c_str());
	}
	else if (!IS_NPC(i))
	{
		strcpy(buf, "\r\n���");
		if (i->is_morphed())
			strcat(buf, string(" "+i->get_morph_desc()+".\r\n").c_str());
		else
		if (IS_FEMALE(i))
		{
			if (GET_HEIGHT(i) <= 151)
			{
				if (GET_WEIGHT(i) >= 140)
					strcat(buf, " ��������� ������� �������.\r\n");
				else if (GET_WEIGHT(i) >= 125)
					strcat(buf, " ��������� �������.\r\n");
				else
					strcat(buf, " ����������� �������.\r\n");
			}
			else if (GET_HEIGHT(i) <= 159)
			{
				if (GET_WEIGHT(i) >= 145)
					strcat(buf, " ��������� ������� �����.\r\n");
				else if (GET_WEIGHT(i) >= 130)
					strcat(buf, " ��������� �������.\r\n");
				else
					strcat(buf, " ������� ����.\r\n");
			}
			else if (GET_HEIGHT(i) <= 165)
			{
				if (GET_WEIGHT(i) >= 145)
					strcat(buf, " �������� ����� �������.\r\n");
				else
					strcat(buf, " �������� ����� ������� ���������.\r\n");
			}
			else if (GET_HEIGHT(i) <= 175)
			{
				if (GET_WEIGHT(i) >= 150)
					strcat(buf, " ������� �������� ����.\r\n");
				else if (GET_WEIGHT(i) >= 135)
					strcat(buf, " ������� �������� �������.\r\n");
				else
					strcat(buf, " ������� ������� �������.\r\n");
			}
			else
			{
				if (GET_WEIGHT(i) >= 155)
					strcat(buf, " ����� ������� ������� ����.\r\n");
				else if (GET_WEIGHT(i) >= 140)
					strcat(buf, " ����� ������� �������� �������.\r\n");
				else
					strcat(buf, " ����� ������� ��������� �������.\r\n");
			}
		}
		else
		{
			if (GET_HEIGHT(i) <= 165)
			{
				if (GET_WEIGHT(i) >= 170)
					strcat(buf, " ���������, ������� �� �������, �������.\r\n");
				else if (GET_WEIGHT(i) >= 150)
					strcat(buf, " ��������� ������� �������.\r\n");
				else
					strcat(buf, " ��������� ������������ ���������.\r\n");
			}
			else if (GET_HEIGHT(i) <= 175)
			{
				if (GET_WEIGHT(i) >= 175)
					strcat(buf, " ��������� ���������� ������.\r\n");
				else if (GET_WEIGHT(i) >= 160)
					strcat(buf, " ��������� ������� �������.\r\n");
				else
					strcat(buf, " ��������� ��������� �������.\r\n");
			}
			else if (GET_HEIGHT(i) <= 185)
			{
				if (GET_WEIGHT(i) >= 180)
					strcat(buf, " �������� ����� ���������� �������.\r\n");
				else if (GET_WEIGHT(i) >= 165)
					strcat(buf, " �������� ����� ������� �������.\r\n");
				else
					strcat(buf, " �������� ����� ��������� �������.\r\n");
			}
			else if (GET_HEIGHT(i) <= 195)
			{
				if (GET_WEIGHT(i) >= 185)
					strcat(buf, " ������� ������� �������.\r\n");
				else if (GET_WEIGHT(i) >= 170)
					strcat(buf, " ������� �������� �������.\r\n");
				else
					strcat(buf, " �������, ��������� �������.\r\n");
			}
			else
			{
				if (GET_WEIGHT(i) >= 190)
					strcat(buf, " �������� �����.\r\n");
				else if (GET_WEIGHT(i) >= 180)
					strcat(buf, " ����� �������, ������� �����.\r\n");
				else
					strcat(buf, " ���������, ������� �� ����� �������.\r\n");
			}
		}
		send_to_char(buf, ch);
	}
	else
		act("\r\n������ ���������� � $n5 �� �� ��������.", FALSE, i, 0, ch, TO_VICT);

	if (AFF_FLAGGED(i, AFF_CHARM) && i->master == ch)
	{
		if (low_charm(i))
			act("$n ����� ���������� ��������� �� ����.", FALSE, i, 0, ch, TO_VICT);
		else
		{
			AFFECT_DATA *aff;
			for (aff = i->affected; aff; aff = aff->next)
				if (aff->type == SPELL_CHARM)
				{
					sprintf(buf, IS_POLY(i) ? "$n ����� ��������� ��� ��� %d %s." : "$n ����� ��������� ��� ��� %d %s.", aff->duration / 2, desc_count(aff->duration / 2, 1));
					act(buf, FALSE, i, 0, ch, TO_VICT);
					break;
				}
		}

	}

	if (IS_HORSE(i) && i->master == ch)
	{
		strcpy(buf, "\r\n��� ��� ������. �� ");
		if (GET_HORSESTATE(i) <= 0)
			strcat(buf, "������.\r\n");
		else if (GET_HORSESTATE(i) <= 20)
			strcat(buf, "���� � ����.\r\n");
		else if (GET_HORSESTATE(i) <= 80)
			strcat(buf, "� ������� ���������.\r\n");
		else
			strcat(buf, "�������� ������ ������.\r\n");
		send_to_char(buf, ch);
	};

	diag_char_to_char(i, ch);

	if (i->is_morphed())
	{
		send_to_char("\r\n", ch);
		std::string coverDesc = "$n ������$a " + i->get_cover_desc()+".";
		act(coverDesc.c_str(), FALSE, i, 0, ch, TO_VICT);
		send_to_char("\r\n", ch);
	}
	else
	{
		found = FALSE;
		for (j = 0; !found && j < NUM_WEARS; j++)
			if (GET_EQ(i, j) && CAN_SEE_OBJ(ch, GET_EQ(i, j)))
				found = TRUE;

		if (found)
		{
			send_to_char("\r\n", ch);
			act("$n ����$a :", FALSE, i, 0, ch, TO_VICT);
			for (j = 0; j < NUM_WEARS; j++)
				if (GET_EQ(i, j) && CAN_SEE_OBJ(ch, GET_EQ(i, j)))
				{
					send_to_char(where[j], ch);
					if (i->master && IS_NPC(i))
						show_obj_to_char(GET_EQ(i, j), ch, 1, ch == i->master, 1);
					else
						show_obj_to_char(GET_EQ(i, j), ch, 1, ch == i, 1);
				}
		}
	}

	if (ch != i && (ch->get_skill(SKILL_LOOK_HIDE) || IS_IMMORTAL(ch)))
	{
		found = FALSE;
		act("\r\n�� ���������� ��������� � $s ����:", FALSE, i, 0, ch, TO_VICT);
		for (tmp_obj = i->carrying; tmp_obj; tmp_obj = tmp_obj->next_content)
		{
			if (CAN_SEE_OBJ(ch, tmp_obj) && (number(0, 30) < GET_LEVEL(ch)))
			{
				if (!push)
				{
					push = tmp_obj;
					push_count = 1;
				}
				else if (GET_OBJ_VNUM(push) != GET_OBJ_VNUM(tmp_obj)
						 || GET_OBJ_VNUM(push) == -1)
				{
					show_obj_to_char(push, ch, 1, ch == i, push_count);
					push = tmp_obj;
					push_count = 1;
				}
				else
					push_count++;
				found = TRUE;
			}
		}
		if (push && push_count)
			show_obj_to_char(push, ch, 1, ch == i, push_count);
		if (!found)
			send_to_char("...� ������ �� ����������.\r\n", ch);
	}
}


void list_one_char(CHAR_DATA * i, CHAR_DATA * ch, int skill_mode)
{
	int sector = SECT_CITY;
	int n;
	char aura_txt[200];
	const char *positions[] =
	{
		"����� �����, �������. ",
		"����� �����, ��� ������. ",
		"����� �����, ��� ��������. ",
		"����� �����, � ��������. ",
		"���� �����. ",
		"�������� �����. ",
		"����� �����. ",
		"���������! ",
		"����� �����. "
	};

	// ����� � ����� ��� ������������� IS_POLY() - ���� ��� ����������� ������� ����� ���� "���" -- ��������
	const char *poly_positions[] =
	{
		"����� �����, �������. ",
		"����� �����, ��� ������. ",
		"����� �����, ��� ��������. ",
		"����� �����, � ��������. ",
		"���� �����. ",
		"�������� �����. ",
		"����� �����. ",
		"���������! ",
		"����� �����. "
	};

	if (IS_HORSE(i) && on_horse(i->master))
	{
		if (ch == i->master)
		{
			if (!IS_POLY(i))
				act("$N ����� ��� �� ����� �����.", FALSE, ch, 0, i, TO_CHAR);
			else
				act("$N ����� ��� �� ����� �����.", FALSE, ch, 0, i, TO_CHAR);
		}
		return;
	}

	if (skill_mode == SKILL_LOOKING)
	{
		if (HERE(i) && INVIS_OK(ch, i) && GET_REAL_LEVEL(ch) >= (IS_NPC(i) ? 0 : GET_INVIS_LEV(i)))
		{
			sprintf(buf, "�� ���������� %s.\r\n", GET_PAD(i, 3));
			send_to_char(buf, ch);
		}
		return;
	}

	if (!CAN_SEE(ch, i))
	{
		skill_mode =
			check_awake(i, ACHECK_AFFECTS | ACHECK_LIGHT | ACHECK_HUMMING | ACHECK_GLOWING | ACHECK_WEIGHT);
		*buf = 0;
		if (IS_SET(skill_mode, ACHECK_AFFECTS))
		{
			REMOVE_BIT(skill_mode, ACHECK_AFFECTS);
			sprintf(buf + strlen(buf), "���������� �����%s", skill_mode ? ", " : " ");
		}
		if (IS_SET(skill_mode, ACHECK_LIGHT))
		{
			REMOVE_BIT(skill_mode, ACHECK_LIGHT);
			sprintf(buf + strlen(buf), "����� ����%s", skill_mode ? ", " : " ");
		}
		if (IS_SET(skill_mode, ACHECK_GLOWING)
				&& IS_SET(skill_mode, ACHECK_HUMMING)
				&& !AFF_FLAGGED(ch, AFF_SIELENCE))
		{
			REMOVE_BIT(skill_mode, ACHECK_GLOWING);
			REMOVE_BIT(skill_mode, ACHECK_HUMMING);
			sprintf(buf + strlen(buf), "��� � ����� ����������%s", skill_mode ? ", " : " ");
		}
		if (IS_SET(skill_mode, ACHECK_GLOWING))
		{
			REMOVE_BIT(skill_mode, ACHECK_GLOWING);
			sprintf(buf + strlen(buf), "����� ����������%s", skill_mode ? ", " : " ");
		}
		if (IS_SET(skill_mode, ACHECK_HUMMING)
				&& !AFF_FLAGGED(ch, AFF_SIELENCE))
		{
			REMOVE_BIT(skill_mode, ACHECK_HUMMING);
			sprintf(buf + strlen(buf), "��� ����������%s", skill_mode ? ", " : " ");
		}
		if (IS_SET(skill_mode, ACHECK_WEIGHT)
				&& !AFF_FLAGGED(ch, AFF_SIELENCE))
		{
			REMOVE_BIT(skill_mode, ACHECK_WEIGHT);
			sprintf(buf + strlen(buf), "�������� �������%s", skill_mode ? ", " : " ");
		}
		strcat(buf, "������ ���-�� �����������.\r\n");
		send_to_char(CAP(buf), ch);
		return;
	}

	if (IS_NPC(i) &&
			i->player_data.long_descr &&
			GET_POS(i) == GET_DEFAULT_POS(i) &&
			IN_ROOM(ch) == IN_ROOM(i) && !AFF_FLAGGED(i, AFF_CHARM) && !IS_HORSE(i))
	{
		*buf = '\0';
		if (PRF_FLAGGED(ch, PRF_ROOMFLAGS))
			sprintf(buf, "[%5d] ", GET_MOB_VNUM(i));

		if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC)
				&& !AFF_FLAGGED(ch, AFF_DETECT_ALIGN))
		{
			if (AFF_FLAGGED(i, AFF_EVILESS))
				strcat(buf, "(������ ����) ");
		}
		if (AFF_FLAGGED(ch, AFF_DETECT_ALIGN))
		{
			if (IS_NPC(i))
			{

				if (NPC_FLAGGED(i, NPC_AIRCREATURE))
					sprintf(buf + strlen(buf), "%s(���� �������)%s ",
							CCIBLU(ch, C_CMP), CCIRED(ch, C_CMP));
				else if (NPC_FLAGGED(i, NPC_WATERCREATURE))
					sprintf(buf + strlen(buf), "%s(���� ����)%s ",
							CCICYN(ch, C_CMP), CCIRED(ch, C_CMP));
				else if (NPC_FLAGGED(i, NPC_FIRECREATURE))
					sprintf(buf + strlen(buf), "%s(���� ����)%s ",
							CCIMAG(ch, C_CMP), CCIRED(ch, C_CMP));
				else if (NPC_FLAGGED(i, NPC_EARTHCREATURE))
					sprintf(buf + strlen(buf), "%s(���� �����)%s ",
							CCIGRN(ch, C_CMP), CCIRED(ch, C_CMP));
			}
		}
		if (AFF_FLAGGED(i, AFF_INVISIBLE))
			sprintf(buf + strlen(buf), "(�������%s) ", GET_CH_SUF_6(i));
		if (AFF_FLAGGED(i, AFF_HIDE))
			sprintf(buf + strlen(buf), "(�������%s) ", GET_CH_SUF_2(i));
		if (AFF_FLAGGED(i, AFF_CAMOUFLAGE))
			sprintf(buf + strlen(buf), "(������������%s) ", GET_CH_SUF_2(i));
		if (AFF_FLAGGED(i, AFF_FLY))
			strcat(buf, IS_POLY(i) ? "(�����) " : "(�����) ");
		if (AFF_FLAGGED(i, AFF_HORSE))
			strcat(buf, "(��� ������) ");

		strcat(buf, i->player_data.long_descr);
		send_to_char(buf, ch);

		*aura_txt = '\0';
		if (AFF_FLAGGED(i, AFF_SHIELD))
		{
			strcat(aura_txt, "...������");
			strcat(aura_txt, GET_CH_SUF_6(i));
			strcat(aura_txt, " ���������� ������� ");
		}
		if (AFF_FLAGGED(i, AFF_SANCTUARY))
			strcat(aura_txt, IS_POLY(i) ? "...�������� ����� ������� " : "...�������� ����� ������� ");
		else if (AFF_FLAGGED(i, AFF_PRISMATICAURA))
			strcat(aura_txt, IS_POLY(i) ? "...������������ ����� ������� " : "...������������ ����� ������� ");
		act(aura_txt, FALSE, i, 0, ch, TO_VICT);

		*aura_txt = '\0';
		n = 0;
		strcat(aura_txt, "...�������");
		strcat(aura_txt, GET_CH_SUF_6(i));
		if (AFF_FLAGGED(i, AFF_AIRSHIELD))
		{
			strcat(aura_txt, " ���������");
			n++;
		}
		if (AFF_FLAGGED(i, AFF_FIRESHIELD))
		{
			if (n > 0)
				strcat(aura_txt, ", ��������");
			else
				strcat(aura_txt, " ��������");
			n++;
		}
		if (AFF_FLAGGED(i, AFF_ICESHIELD))
		{
			if (n > 0)
				strcat(aura_txt, ", �������");
			else
				strcat(aura_txt, " �������");
			n++;
		}
		if (n == 1)
			strcat(aura_txt, " ����� ");
		else if (n > 1)
			strcat(aura_txt, " ������ ");
		if (n > 0)
			act(aura_txt, FALSE, i, 0, ch, TO_VICT);

		if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC))
		{
			*aura_txt = '\0';
			n = 0;
			strcat(aura_txt, "...");
			if (AFF_FLAGGED(i, AFF_AIRAURA))
			{
				strcat(aura_txt, "���������");
				n++;
			}
			if (AFF_FLAGGED(i, AFF_FIREAURA))
			{
				if (n > 0)
					strcat(aura_txt, ", ��������");
				else
					strcat(aura_txt, "��������");
				n++;
			}
			if (AFF_FLAGGED(i, AFF_ICEAURA))
			{
				if (n > 0)
					strcat(aura_txt, ", �������");
				else
					strcat(aura_txt, "�������");
				n++;
			}
			if (AFF_FLAGGED(i, AFF_MAGICGLASS))
			{
				if (n > 0)
					strcat(aura_txt, ", �����������");
				else
					strcat(aura_txt, "�����������");
				n++;
			}
			if (AFF_FLAGGED(i, AFF_BROKEN_CHAINS))
			{
				if (n > 0)
					strcat(aura_txt, ", ����-�����");
				else
					strcat(aura_txt, "����-�����");
				n++;
			}
			if (AFF_FLAGGED(i, AFF_EVILESS))
			{
				if (n > 0)
					strcat(aura_txt, ", ������");
				else
					strcat(aura_txt, "������");
				n++;
			}
			if (n == 1)
				strcat(aura_txt, " ���� ");
			else if (n > 1)
				strcat(aura_txt, " ���� ");

			if (n > 0)
				act(aura_txt, FALSE, i, 0, ch, TO_VICT);
		}
		*aura_txt = '\0';
		if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC))
		{
			if (AFF_FLAGGED(i, AFF_HOLD))
				strcat(aura_txt, "...�����������$a");
			if (AFF_FLAGGED(i, AFF_SIELENCE))
				strcat(aura_txt, "...���$a");
		}
		if (AFF_FLAGGED(i, AFF_BLIND))
			strcat(aura_txt, "...����$a");
		if (AFF_FLAGGED(i, AFF_DEAFNESS))
			strcat(aura_txt, "...����$a");
		if (AFF_FLAGGED(i, AFF_STRANGLED))
			strcat(aura_txt, "...����������.");

		if (*aura_txt)
			act(aura_txt, FALSE, i, 0, ch, TO_VICT);

		return;
	}

	if (IS_NPC(i))
	{
		strcpy(buf1, i->get_npc_name());
		strcat(buf1, " ");
		if (AFF_FLAGGED(i, AFF_HORSE))
			strcat(buf1, "(��� ������) ");
		CAP(buf1);
	}
	else
	{
		sprintf(buf1, "%s%s ", i->get_morphed_title().c_str(), PLR_FLAGGED(i, PLR_KILLER) ? " <�������>" : "");
	}

	sprintf(buf, "%s%s", AFF_FLAGGED(i, AFF_CHARM) ? "*" : "", buf1);
	if (AFF_FLAGGED(i, AFF_INVISIBLE))
		sprintf(buf + strlen(buf), "(�������%s) ", GET_CH_SUF_6(i));
	if (AFF_FLAGGED(i, AFF_HIDE))
		sprintf(buf + strlen(buf), "(�������%s) ", GET_CH_SUF_2(i));
	if (AFF_FLAGGED(i, AFF_CAMOUFLAGE))
		sprintf(buf + strlen(buf), "(������������%s) ", GET_CH_SUF_2(i));
	if (!IS_NPC(i) && !i->desc)
		sprintf(buf + strlen(buf), "(�������%s �����) ", GET_CH_SUF_1(i));
	if (!IS_NPC(i) && PLR_FLAGGED(i, PLR_WRITING))
		strcat(buf, "(�����) ");

	if (GET_POS(i) != POS_FIGHTING)
	{
		if (on_horse(i))
		{
			CHAR_DATA *horse = get_horse(i);
			if (horse)
			{
				const char *msg =
					AFF_FLAGGED(horse, AFF_FLY) ? "������" : "�����";
				sprintf(buf + strlen(buf), "%s ����� ������ �� %s. ",
					msg, PERS(horse, ch, 5));
			}
		}
		else if (IS_HORSE(i) && AFF_FLAGGED(i, AFF_TETHERED))
			sprintf(buf + strlen(buf), "��������%s �����. ", GET_CH_SUF_6(i));
		else if ((sector = real_sector(IN_ROOM(i))) == SECT_FLYING)
			strcat(buf, IS_POLY(i) ? "������ �����. " : "������ �����. ");
		else if (sector == SECT_UNDERWATER)
			strcat(buf, IS_POLY(i) ? "������� �����. " : "������� �����. ");
		else if (GET_POS(i) > POS_SLEEPING && AFF_FLAGGED(i, AFF_FLY))
			strcat(buf, IS_POLY(i) ? "������ �����. " : "������ �����. ");
		else if (sector == SECT_WATER_SWIM || sector == SECT_WATER_NOSWIM)
			strcat(buf, IS_POLY(i) ? "������� �����. " : "������� �����. ");
		else
			strcat(buf, IS_POLY(i) ? poly_positions[(int) GET_POS(i)] : positions[(int) GET_POS(i)]);
		if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC) && IS_NPC(i) && affected_by_spell(i, SPELL_CAPABLE))
			sprintf(buf + strlen(buf), "(���� �����) ");
	}
	else
	{
		if (i->get_fighting())
		{
			strcat(buf, IS_POLY(i) ? "��������� � " : "��������� � ");
			if (i->in_room != i->get_fighting()->in_room)
				strcat(buf, "����-�� �����");
			else if (i->get_fighting() == ch)
				strcat(buf, "����");
			else
				strcat(buf, GET_PAD(i->get_fighting(), 4));
			if (on_horse(i))
				sprintf(buf + strlen(buf), ", ���� ������ �� %s! ", PERS(get_horse(i), ch, 5));
			else
				strcat(buf, "! ");
		}
		else		// NIL fighting pointer
		{
			strcat(buf, IS_POLY(i) ? "������� �� �������" : "������� �� �������");
			if (on_horse(i))
				sprintf(buf + strlen(buf), ", ���� ������ �� %s. ", PERS(get_horse(i), ch, 5));
			else
				strcat(buf, ". ");
		}
	}

	if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC)
			&& !AFF_FLAGGED(ch, AFF_DETECT_ALIGN))
	{
		if (AFF_FLAGGED(i, AFF_EVILESS))
			strcat(buf, "(������ ����) ");
	}
	if (AFF_FLAGGED(ch, AFF_DETECT_ALIGN))
	{
		if (IS_NPC(i))
		{
			if (IS_EVIL(i))
			{
				if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC)
						&& AFF_FLAGGED(i, AFF_EVILESS))
					strcat(buf, "(������-������ ����) ");
				else
					strcat(buf, "(������ ����) ");
			}
			else if (IS_GOOD(i))
			{
				if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC)
						&& AFF_FLAGGED(i, AFF_EVILESS))
					strcat(buf, "(����� ����) ");
				else
					strcat(buf, "(������� ����) ");
			}
			else
			{
				if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC)
						&& AFF_FLAGGED(i, AFF_EVILESS))
					strcat(buf, "(������ ����) ");
			}
		}
		else
		{
			aura(ch, C_CMP, i, aura_txt);
			strcat(buf, aura_txt);
			strcat(buf, " ");
		}
	}
	if (AFF_FLAGGED(ch, AFF_DETECT_POISON))
		if (AFF_FLAGGED(i, AFF_POISON))
			sprintf(buf + strlen(buf), "(��������%s) ", GET_CH_SUF_6(i));

	strcat(buf, "\r\n");
	send_to_char(buf, ch);

	*aura_txt = '\0';
	if (AFF_FLAGGED(i, AFF_SHIELD))
	{
		strcat(aura_txt, "...������");
		strcat(aura_txt, GET_CH_SUF_6(i));
		strcat(aura_txt, " ���������� ������� ");
	}
	if (AFF_FLAGGED(i, AFF_SANCTUARY))
		strcat(aura_txt, IS_POLY(i) ? "...�������� ����� ������� " : "...�������� ����� ������� ");
	else if (AFF_FLAGGED(i, AFF_PRISMATICAURA))
		strcat(aura_txt, IS_POLY(i) ? "...������������ ����� ������� " : "...������������ ����� ������� ");
	act(aura_txt, FALSE, i, 0, ch, TO_VICT);

	*aura_txt = '\0';
	n = 0;
	strcat(aura_txt, "...�������");
	strcat(aura_txt, GET_CH_SUF_6(i));
	if (AFF_FLAGGED(i, AFF_AIRSHIELD))
	{
		strcat(aura_txt, " ���������");
		n++;
	}
	if (AFF_FLAGGED(i, AFF_FIRESHIELD))
	{
		if (n > 0)
			strcat(aura_txt, ", ��������");
		else
			strcat(aura_txt, " ��������");
		n++;
	}
	if (AFF_FLAGGED(i, AFF_ICESHIELD))
	{
		if (n > 0)
			strcat(aura_txt, ", �������");
		else
			strcat(aura_txt, " �������");
		n++;
	}
	if (n == 1)
		strcat(aura_txt, " ����� ");
	else if (n > 1)
		strcat(aura_txt, " ������ ");
	if (n > 0)
		act(aura_txt, FALSE, i, 0, ch, TO_VICT);

	if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC))
	{
		*aura_txt = '\0';
		n = 0;
		strcat(aura_txt, " ..");
		if (AFF_FLAGGED(i, AFF_AIRAURA))
		{
			strcat(aura_txt, "���������");
			n++;
		}
		if (AFF_FLAGGED(i, AFF_FIREAURA))
		{
			if (n > 0)
				strcat(aura_txt, ", ��������");
			else
				strcat(aura_txt, "��������");
			n++;
		}
		if (AFF_FLAGGED(i, AFF_ICEAURA))
		{
			if (n > 0)
				strcat(aura_txt, ", �������");
			else
				strcat(aura_txt, "�������");
			n++;
		}
		if (AFF_FLAGGED(i, AFF_MAGICGLASS))
		{
			if (n > 0)
				strcat(aura_txt, ", �����������");
			else
				strcat(aura_txt, "�����������");
			n++;
		}
		if (AFF_FLAGGED(i, AFF_BROKEN_CHAINS))
		{
			if (n > 0)
				strcat(aura_txt, ", ����-�����");
			else
				strcat(aura_txt, "����-�����");
			n++;
		}
		if (n == 1)
			strcat(aura_txt, " ���� ");
		else if (n > 1)
			strcat(aura_txt, " ���� ");

		if (n > 0)
			act(aura_txt, FALSE, i, 0, ch, TO_VICT);
	}
	*aura_txt = '\0';
	if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC))
	{
		if (AFF_FLAGGED(i, AFF_HOLD))
			strcat(aura_txt, " ...�����������$a");
		if (AFF_FLAGGED(i, AFF_SIELENCE))
			strcat(aura_txt, " ...���$a");
	}
	if (AFF_FLAGGED(i, AFF_BLIND))
		strcat(aura_txt, " ...����$a");
	if (AFF_FLAGGED(i, AFF_DEAFNESS))
		strcat(aura_txt, " ...����$a");
	if (AFF_FLAGGED(i, AFF_STRANGLED))
		strcat(aura_txt, " ...����������");
	if (*aura_txt)
		act(aura_txt, FALSE, i, 0, ch, TO_VICT);
}

void list_char_to_char(CHAR_DATA * list, CHAR_DATA * ch)
{
	CHAR_DATA *i;

	for (i = list; i; i = i->next_in_room)
		if (ch != i)
		{
			if (HERE(i) && (CAN_SEE(ch, i)
							|| awaking(i, AW_HIDE | AW_INVIS | AW_CAMOUFLAGE)))
				list_one_char(i, ch, 0);
			else if (IS_DARK(i->in_room) &&
					 IN_ROOM(i) == IN_ROOM(ch) && !CAN_SEE_IN_DARK(ch) && AFF_FLAGGED(i, AFF_INFRAVISION))
				send_to_char("���� ���������� ���� ������� �� ���.\r\n", ch);
		}
}

void do_auto_exits(CHAR_DATA * ch)
{
	int door, slen = 0;

	*buf = '\0';

	for (door = 0; door < NUM_OF_DIRS; door++)
		// �������-�� ��������� ��������� � ����������� �������� ������
		if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE)
		{
			if (EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED))
				slen += sprintf(buf + slen, "(%c) ", LOWER(*dirs[door]));
			else if (!EXIT_FLAGGED(EXIT(ch, door), EX_HIDDEN))
			{
				if (world[EXIT(ch, door)->to_room]->zone==world[ch->in_room]->zone)
					slen += sprintf(buf + slen, "%c ", LOWER(*dirs[door]));
				else
					slen += sprintf(buf + slen, "%c ", UPPER(*dirs[door]));
			}
		}
	sprintf(buf2, "%s[ Exits: %s]%s\r\n", CCCYN(ch, C_NRM), *buf ? buf : "None! ", CCNRM(ch, C_NRM));

	send_to_char(buf2, ch);
}


ACMD(do_exits)
{
	int door;

	*buf = '\0';

	if (AFF_FLAGGED(ch, AFF_BLIND))
	{
		send_to_char("�� �����, ��� �������!\r\n", ch);
		return;
	}
	for (door = 0; door < NUM_OF_DIRS; door++)
		if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE && !EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED))
		{
			if (IS_GOD(ch))
				sprintf(buf2, "%-5s - [%5d] %s\r\n", Dirs[door],
						GET_ROOM_VNUM(EXIT(ch, door)->to_room), world[EXIT(ch, door)->to_room]->name);
			else
			{
				sprintf(buf2, "%-5s - ", Dirs[door]);
				if (IS_DARK(EXIT(ch, door)->to_room) && !CAN_SEE_IN_DARK(ch))
					strcat(buf2, "������� �����\r\n");
				else
				{
					strcat(buf2, world[EXIT(ch, door)->to_room]->name);
					strcat(buf2, "\r\n");
				}
			}
			strcat(buf, CAP(buf2));
		}
	send_to_char("������� ������:\r\n", ch);
	if (*buf)
		send_to_char(buf, ch);
	else
		send_to_char(" ����������, ������!\r\n", ch);
}


#define MAX_FIRES 6
const char *Fires[MAX_FIRES] = { "����� ��������� ����� ��������",
								 "����� ��������� ����� ��������",
								 "���-��� �������� ������",
								 "�������� ��������� ������",
								 "������ ������ ������",
								 "���� ������ ������"
							   };

#define TAG_NIGHT       "<night>"
#define TAG_DAY         "<day>"
#define TAG_WINTERNIGHT "<winternight>"
#define TAG_WINTERDAY   "<winterday>"
#define TAG_SPRINGNIGHT "<springnight>"
#define TAG_SPRINGDAY   "<springday>"
#define TAG_SUMMERNIGHT "<summernight>"
#define TAG_SUMMERDAY   "<summerday>"
#define TAG_AUTUMNNIGHT "<autumnnight>"
#define TAG_AUTUMNDAY   "<autumnday>"

int paste_description(char *string, const char *tag, int need)
{
	char *pos;
	if (!*string || !*tag)
		return (FALSE);
	if ((pos = str_str(string, tag)))
	{
		if (need)
		{
			for (; *pos && *pos != '>'; pos++);
			if (*pos)
				pos++;
			if (*pos == 'R')
			{
				pos++;
				buf[0] = '\0';
			}
			strcat(buf, pos);
			if ((pos = str_str(buf, tag)))
				* pos = '\0';
			return (TRUE);
		}
		else
		{
			*pos = '\0';
			if ((pos = str_str(pos + 1, tag)))
				strcat(string, pos + strlen(tag));
		}
	}
	return (FALSE);
}


void show_extend_room(const char * const description, CHAR_DATA * ch)
{
	int found = FALSE, i;
	char string[MAX_STRING_LENGTH], *pos;

	if (!description || !*description)
		return;

	strcpy(string, description);
	if ((pos = strchr(string, '<')))
		* pos = '\0';
	strcpy(buf, string);
	if (pos)
		*pos = '<';

	found = found || paste_description(string, TAG_WINTERNIGHT,
									   (weather_info.season == SEASON_WINTER
										&& (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)));
	found = found || paste_description(string, TAG_WINTERDAY,
									   (weather_info.season == SEASON_WINTER
										&& (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT)));
	found = found || paste_description(string, TAG_SPRINGNIGHT,
									   (weather_info.season == SEASON_SPRING
										&& (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)));
	found = found || paste_description(string, TAG_SPRINGDAY,
									   (weather_info.season == SEASON_SPRING
										&& (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT)));
	found = found || paste_description(string, TAG_SUMMERNIGHT,
									   (weather_info.season == SEASON_SUMMER
										&& (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)));
	found = found || paste_description(string, TAG_SUMMERDAY,
									   (weather_info.season == SEASON_SUMMER
										&& (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT)));
	found = found || paste_description(string, TAG_AUTUMNNIGHT,
									   (weather_info.season == SEASON_AUTUMN
										&& (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)));
	found = found || paste_description(string, TAG_AUTUMNDAY,
									   (weather_info.season == SEASON_AUTUMN
										&& (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT)));
	found = found || paste_description(string, TAG_NIGHT,
									   (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK));
	found = found || paste_description(string, TAG_DAY,
									   (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT));

	for (i = strlen(buf); i > 0 && *(buf + i) == '\n'; i--)
	{
		*(buf + i) = '\0';
		if (i > 0 && *(buf + i) == '\r')
			*(buf + --i) = '\0';
	}

	send_to_char(buf, ch);
	send_to_char("\r\n", ch);
}

bool put_delim(std::stringstream &out, bool delim)
{
	if (!delim)
	{
		out << " (";
	}
	else
	{
		out << ", ";
	}
	return true;
}

void print_zone_info(CHAR_DATA *ch)
{
	zone_data *zone = &zone_table[world[ch->in_room]->zone];
	std::stringstream out;
	out << "\r\n" << zone->name;

	bool delim = false;
	if (!zone->is_town)
	{
		delim = put_delim(out, delim);
		out << "������� �������: " << zone->mob_level;
	}
	if (zone->group > 1)
	{
		delim = put_delim(out, delim);
		out << "��������� �� " << zone->group
			<< " " << desc_count(zone->group, WHAT_PEOPLE);
	}
	if (delim)
	{
		out << ")";
	}
	out << ".\r\n";

	send_to_char(out.str(), ch);
}

void show_glow_objs(CHAR_DATA *ch)
{
	unsigned cnt = 0;
	for (OBJ_DATA *obj = world[ch->in_room]->contents;
		obj; obj = obj->next_content)
	{
		if (IS_OBJ_STAT(obj, ITEM_GLOW))
		{
			++cnt;
			if (cnt > 1)
			{
				break;
			}
		}
	}
	if (!cnt) return;

	const char *str = cnt > 1 ?
		"�� ������ ��������� �����-�� ��������� ���������.\r\n" :
		"�� ������ ��������� ������-�� ���������� ��������.\r\n";
	send_to_char(str, ch);
}

void look_at_room(CHAR_DATA * ch, int ignore_brief)
{
	if (!ch->desc)
		return;

	if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch) && !can_use_feat(ch, DARK_READING_FEAT))
	{
		send_to_char("������� �����...\r\n", ch);
		show_glow_objs(ch);
		return;
	}
	else if (AFF_FLAGGED(ch, AFF_BLIND))
	{
		send_to_char("�� ��� ��� �����...\r\n", ch);
		return;
	}
	else if (GET_POS(ch) < POS_SLEEPING)
	{
		return;
	}

	if (PRF_FLAGGED(ch, PRF_DRAW_MAP))
	{
		MapSystem::print_map(ch);
	}
	else if (ch->desc->snoop_by
		&& ch->desc->snoop_by->snoop_with_map
		&& ch->desc->snoop_by->character)
	{
		ch->map_print_to_snooper(ch->desc->snoop_by->character);
	}

	send_to_char(CCICYN(ch, C_NRM), ch);

	if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_ROOMFLAGS))
	{
		// ����� ��������� * �� ������ ������ ����� ����
		const bool has_flag = ROOM_FLAGGED(ch->in_room, ROOM_BFS_MARK) ? true : false;
		REMOVE_BIT(ROOM_FLAGS(ch->in_room, ROOM_BFS_MARK), ROOM_BFS_MARK);

		sprintbits(world[ch->in_room]->room_flags, room_bits, buf, ";");
		sprintf(buf2, "[%5d] %s [%s]", GET_ROOM_VNUM(IN_ROOM(ch)), world[ch->in_room]->name, buf);
		send_to_char(buf2, ch);

		if (has_flag)
		{
			SET_BIT(ROOM_FLAGS(ch->in_room, ROOM_BFS_MARK), ROOM_BFS_MARK);
		}
	}
	else
		send_to_char(world[ch->in_room]->name, ch);

	send_to_char(CCNRM(ch, C_NRM), ch);
	send_to_char("\r\n", ch);

	if (IS_DARK(IN_ROOM(ch)) && !PRF_FLAGGED(ch, PRF_HOLYLIGHT))
		send_to_char("������� �����...\r\n", ch);
	else if ((!IS_NPC(ch) && !PRF_FLAGGED(ch, PRF_BRIEF)) || ignore_brief || ROOM_FLAGGED(ch->in_room, ROOM_DEATH))
	{
		show_extend_room(RoomDescription::show_desc(world[ch->in_room]->description_num).c_str(), ch);
	}

	// autoexits
	if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_AUTOEXIT))
		do_auto_exits(ch);

	// ���������� ������� �������. ����� ����������� ����� �� ������ ���������� ������.
	if (AFF_FLAGGED(ch, AFF_DETECT_MAGIC) || IS_IMMORTAL(ch))
		sprintbits(world[ch->in_room]->affected_by, room_aff_invis_bits, buf2, "\n");
	else
		sprintbits(world[ch->in_room]->affected_by, room_aff_visib_bits, buf2, "\n");

	// ��������� ���� ��� ��������
	if (strcmp(buf2, "������"))
	{
		sprintf(buf, "%s\r\n\r\n", buf2);
		send_to_char(buf, ch);
	}

	// now list characters & objects
	if (world[IN_ROOM(ch)]->fires)
	{
		sprintf(buf, "%s� ������ %s.%s\r\n",
				CCRED(ch, C_NRM), Fires[MIN(world[IN_ROOM(ch)]->fires, MAX_FIRES - 1)], CCNRM(ch, C_NRM));
		send_to_char(buf, ch);
	}

	if (world[IN_ROOM(ch)]->portal_time)
	{
		if (world[IN_ROOM(ch)]->pkPenterUnique)
			sprintf(buf,
					"%s�������� ����������� %s� �������� ���������%s ���� �������� �����.%s\r\n",
					CCIBLU(ch, C_NRM), CCIRED(ch, C_NRM), CCIBLU(ch, C_NRM), CCNRM(ch, C_NRM));
		else
			sprintf(buf,
					"%s�������� ����������� ���� �������� �����.%s\r\n",
					CCIBLU(ch, C_NRM), CCNRM(ch, C_NRM));

		send_to_char(buf, ch);
	}

	if (world[IN_ROOM(ch)]->holes)
	{
		int ar = (int) roundup(world[IN_ROOM(ch)]->holes / HOLES_TIME);
		sprintf(buf, "%s����� �������� ���� �������� �������� � %i �����%s.%s\r\n",
				CCYEL(ch, C_NRM), ar, (ar == 1 ? "" : (ar < 5 ? "�" : "��")), (CCNRM(ch, C_NRM)));
		send_to_char(buf, ch);
	}

	if (IN_ROOM(ch) != NOWHERE && !ROOM_FLAGGED(IN_ROOM(ch), ROOM_NOWEATHER))
	{
		*buf = '\0';
		switch (real_sector(IN_ROOM(ch)))
		{
		case SECT_FIELD_SNOW:
		case SECT_FOREST_SNOW:
		case SECT_HILLS_SNOW:
		case SECT_MOUNTAIN_SNOW:
			sprintf(buf, "%s������� ����� ����� � ��� ��� ������.%s\r\n",
					CCWHT(ch, C_NRM), CCNRM(ch, C_NRM));
			break;
		case SECT_FIELD_RAIN:
		case SECT_FOREST_RAIN:
		case SECT_HILLS_RAIN:
			sprintf(buf, "%s�� ������ �������� � �����.%s\r\n", CCIWHT(ch, C_NRM), CCNRM(ch, C_NRM));
			break;
		case SECT_THICK_ICE:
			sprintf(buf, "%s� ��� ��� ������ ������� ���.%s\r\n", CCIBLU(ch, C_NRM), CCNRM(ch, C_NRM));
			break;
		case SECT_NORMAL_ICE:
			sprintf(buf, "%s� ��� ��� ������ ���������� ������� ���.%s\r\n",
					CCIBLU(ch, C_NRM), CCNRM(ch, C_NRM));
			break;
		case SECT_THIN_ICE:
			sprintf(buf,
					"%s��������� ����� ���-��� ���������� ��� ����.%s\r\n",
					CCICYN(ch, C_NRM), CCNRM(ch, C_NRM));
			break;
		};
		if (*buf)
			send_to_char(buf, ch);
	}

	send_to_char("&Y&q", ch);
//  if (IS_SET(GET_SPELL_TYPE(ch, SPELL_TOWNPORTAL),SPELL_KNOW))
	if (ch->get_skill(SKILL_TOWNPORTAL))
		if (find_portal_by_vnum(GET_ROOM_VNUM(ch->in_room)))
			send_to_char("������ ������ � ������������ ����������� ������� ��������� �� �����.\r\n", ch);
	list_obj_to_char(world[ch->in_room]->contents, ch, 0, FALSE);
	send_to_char("&R&q", ch);
	list_char_to_char(world[ch->in_room]->people, ch);
	send_to_char("&Q&n", ch);

	// ���� � ����� ����
	if (zone_table[world[ch->get_from_room()]->zone].number
		!= zone_table[world[ch->in_room]->zone].number
		&& PRF_FLAGGED(ch, PRF_ENTER_ZONE))
	{
		print_zone_info(ch);
	}
}

int get_pick_chance(int skill_pick, int lock_complexity)
{
	return (MIN(5, MAX(-5, skill_pick - lock_complexity)) + 5);
}

void look_in_direction(CHAR_DATA * ch, int dir, int info_is)
{
	int count = 0, probe, percent;
	EXIT_DATA *rdata = NULL;
	CHAR_DATA *tch;
	if (CAN_GO(ch, dir)
			|| (EXIT(ch, dir) && EXIT(ch, dir)->to_room != NOWHERE))
	{
		rdata = EXIT(ch, dir);
		count += sprintf(buf, "%s%s:%s ", CCYEL(ch, C_NRM), Dirs[dir], CCNRM(ch, C_NRM));
		if (EXIT_FLAGGED(rdata, EX_CLOSED))
		{
			if (rdata->keyword)
				count += sprintf(buf + count, " ������� (%s).\r\n", rdata->keyword);
			else
				count += sprintf(buf + count, " ������� (�������� �����).\r\n");
			int skill_pick = ch->get_skill(SKILL_PICK_LOCK) ;
			if (EXIT_FLAGGED(rdata, EX_LOCKED) && skill_pick)
			{
				if (EXIT_FLAGGED(rdata, EX_PICKPROOF))
				{
					count += sprintf(buf+count-2, "%s �� ������� �� ������� ��� ��������!%s\r\n", CCICYN(ch, C_NRM), CCNRM(ch, C_NRM));
				}
				else if (EXIT_FLAGGED(rdata, EX_BROKEN))
				{
					count += sprintf(buf+count-2, "%s ����� ������... %s\r\n", CCRED(ch, C_NRM), CCNRM(ch, C_NRM));
				}
				else
				{
					int chance = get_pick_chance(skill_pick, rdata->lock_complexity);
					int index = chance ? chance/5 + 1 : 0;

					std::string color = Locks[index][1];
					if (abs(skill_pick - rdata->lock_complexity)>10)
						color = KIDRK;
					if (COLOR_LEV(ch)>C_NRM)
						count += sprintf(buf+count-2, Locks[index][0], color.c_str(), KNRM);
					else
						count += sprintf(buf+count-2, Locks[index][0], KNUL, KNUL);
				}
			}

			send_to_char(buf, ch);
			return;
		};
		if (IS_TIMEDARK(rdata->to_room))
		{
			count += sprintf(buf + count, " ������� �����.\r\n");
			send_to_char(buf, ch);
			if (info_is & EXIT_SHOW_LOOKING)
			{
				send_to_char("&R&q", ch);
				for (count = 0, tch = world[rdata->to_room]->people; tch; tch = tch->next_in_room)
				{
					percent = number(1, skill_info[SKILL_LOOKING].max_percent);
					probe =
						train_skill(ch, SKILL_LOOKING, skill_info[SKILL_LOOKING].max_percent, tch);
					if (HERE(tch) && INVIS_OK(ch, tch) && probe >= percent
							&& (percent < 100 || IS_IMMORTAL(ch)))
					{
						list_one_char(tch, ch, SKILL_LOOKING);
						count++;
					}
				}
				if (!count)
					send_to_char("�� ������ �� ������ ����������!\r\n", ch);
				send_to_char("&Q&n", ch);
			}
		}
		else
		{
			if (rdata->general_description)
				count += sprintf(buf + count, "%s\r\n", rdata->general_description);
			else
				count += sprintf(buf + count, "%s\r\n", world[rdata->to_room]->name);
			send_to_char(buf, ch);
			send_to_char("&R&q", ch);
			list_char_to_char(world[rdata->to_room]->people, ch);
			send_to_char("&Q&n", ch);
		}
	}
	else if (info_is & EXIT_SHOW_WALL)
		send_to_char("� ��� �� ��� �������� �������?\r\n", ch);
}

void hear_in_direction(CHAR_DATA * ch, int dir, int info_is)
{
	int count = 0, percent = 0, probe = 0;
	EXIT_DATA *rdata;
	CHAR_DATA *tch;
	int fight_count = 0;
	string tmpstr = "";

	if (AFF_FLAGGED(ch, AFF_DEAFNESS))
	{
		send_to_char("�� ������, ��� �� �����?\r\n", ch);
		return;
	}
	if (CAN_GO(ch, dir))
	{
		rdata = EXIT(ch, dir);
		count += sprintf(buf, "%s%s:%s ", CCYEL(ch, C_NRM), Dirs[dir], CCNRM(ch, C_NRM));

		if (EXIT_FLAGGED(rdata, EX_CLOSED) && rdata->keyword)
		{
			count += sprintf(buf + count, " ������� (%s).\r\n", fname(rdata->keyword));
			send_to_char(buf, ch);
			return;
		};
		count += sprintf(buf + count, "\r\n%s", CCGRN(ch, C_NRM));
		send_to_char(buf, ch);
		for (count = 0, tch = world[rdata->to_room]->people; tch; tch = tch->next_in_room)
		{
			percent = number(1, skill_info[SKILL_HEARING].max_percent);
			probe = train_skill(ch, SKILL_HEARING, skill_info[SKILL_HEARING].max_percent, tch);
			// ���� ��������� �� ������ ������ ������.
			if (tch->get_fighting())
			{
				if (IS_NPC(tch))
					tmpstr += " �� ������� ��� ����-�� ������.\r\n";
				else
					tmpstr += " �� ������� ����� ����-�� ������.\r\n";
				fight_count++;
				continue;
			}
			if ((probe >= percent || ((!AFF_FLAGGED(tch, AFF_SNEAK) || !AFF_FLAGGED(tch, AFF_HIDE)) && (probe > percent * 2)))
					&& (percent < 100 || IS_IMMORTAL(ch))
					&& !fight_count)
			{
				if (IS_NPC(tch))
				{
					if (real_sector(IN_ROOM(ch)) != SECT_UNDERWATER)
					{
						if (GET_LEVEL(tch) < 5)
							tmpstr += " �� ������� ���-�� ����� �����.\r\n";
						else if (GET_LEVEL(tch) < 15)
							tmpstr += " �� ������� ���-�� �������.\r\n";
						else if (GET_LEVEL(tch) < 25)
							tmpstr += " �� ������� ���-�� ������� �������.\r\n";
						else
							tmpstr += " �� ������� ���-�� ������� �������.\r\n";
					}
					else
					{
						if (GET_LEVEL(tch) < 5)
							tmpstr += " �� ������� ����� ���������.\r\n";
						else if (GET_LEVEL(tch) < 15)
							tmpstr += " �� ������� ���������.\r\n";
						else if (GET_LEVEL(tch) < 25)
							tmpstr += " �� ������� ������� ���������.\r\n";
						else
							tmpstr += " �� ������� ������� ���������.\r\n";
					}
				}
				else
					tmpstr += " �� ������� ���-�� �����������.\r\n";
				count++;
			}
		}
		if ((!count) && (!fight_count))
			send_to_char(" ������ � �����.\r\n", ch);
		else
			send_to_char(tmpstr.c_str(), ch);
		send_to_char(CCNRM(ch, C_NRM), ch);
	}
	else
	{
		if (info_is & EXIT_SHOW_WALL)
			send_to_char("� ��� �� ��� ������ ��������?\r\n", ch);
	}
}



void look_in_obj(CHAR_DATA * ch, char *arg)
{
	OBJ_DATA *obj = NULL;
	CHAR_DATA *dummy = NULL;
	char whatp[MAX_INPUT_LENGTH], where[MAX_INPUT_LENGTH];
	int amt, bits;
	int where_bits = FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP;

	if (!*arg)
		send_to_char("�������� �� ���?\r\n", ch);
	else
		half_chop(arg, whatp, where);

	if (isname(where, "����� ������� room ground"))
		where_bits = FIND_OBJ_ROOM;
	else if (isname(where, "��������� inventory"))
		where_bits = FIND_OBJ_INV;
	else if (isname(where, "���������� equipment"))
		where_bits = FIND_OBJ_EQUIP;

	bits = generic_find(arg, where_bits, ch, &dummy, &obj);

	if ((obj == NULL) || !bits)
	{
		sprintf(buf, "�� �� ������ ����� '%s'.\r\n", arg);
		send_to_char(buf, ch);
	}
	else if ((GET_OBJ_TYPE(obj) != ITEM_DRINKCON)
			 && (GET_OBJ_TYPE(obj) != ITEM_FOUNTAIN)
			 && (GET_OBJ_TYPE(obj) != ITEM_CONTAINER))
		send_to_char("������ � ��� ���!\r\n", ch);
	else
	{
		if (Clan::ChestShow(obj, ch))
		{
			return;
		}
		if (ClanSystem::show_ingr_chest(obj, ch))
		{
			return;
		}
		if (Depot::is_depot(obj))
		{
			Depot::show_depot(ch);
			return;
		}

		if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER)
		{
			if (OBJVAL_FLAGGED(obj, CONT_CLOSED))
			{
				act("������$A.", FALSE, ch, obj, 0, TO_CHAR);
				int skill_pick = ch->get_skill(SKILL_PICK_LOCK) ;
				int count = sprintf(buf, "������%s.", GET_OBJ_SUF_6(obj));
				if (OBJVAL_FLAGGED(obj, CONT_LOCKED) && skill_pick)
				{
					if (OBJVAL_FLAGGED(obj, CONT_PICKPROOF))
						count += sprintf(buf+count, "%s �� ������� �� ������� ��� ��������!%s\r\n", CCICYN(ch, C_NRM), CCNRM(ch, C_NRM));
					else if (OBJVAL_FLAGGED(obj, CONT_BROKEN))
						count += sprintf(buf+count, "%s ����� ������... %s\r\n", CCRED(ch, C_NRM), CCNRM(ch, C_NRM));
					else
					{
						int chance = get_pick_chance(skill_pick, GET_OBJ_VAL(obj, 3));
						int index = chance ? chance/5 + 1 : 0;
						std::string color = Locks[index][1];
						if (abs(skill_pick - GET_OBJ_VAL(obj, 3))>10)
							color = KIDRK;

						if (COLOR_LEV(ch)>C_NRM)
							count += sprintf(buf + count, Locks[index][0], color.c_str(), KNRM);
						else
							count += sprintf(buf + count, Locks[index][0], KNUL, KNUL);
					}
					send_to_char(buf, ch);
				}
			}
			else
			{
				send_to_char(OBJN(obj, ch, 0), ch);
				switch (bits)
				{
				case FIND_OBJ_INV:
					send_to_char("(� �����)\r\n", ch);
					break;
				case FIND_OBJ_ROOM:
					send_to_char("(�� �����)\r\n", ch);
					break;
				case FIND_OBJ_EQUIP:
					send_to_char("(� ��������)\r\n", ch);
					break;
				}
				if (!obj->contains)
					send_to_char(" ������ ������ ���.\r\n", ch);
				else
				{
					if (GET_OBJ_VAL(obj, 0) > 0 && bits != FIND_OBJ_ROOM) {
						/* amt - ������ ������� �� 6 ��������� (0..5) � ��������� �������������
						   � ������� �������� ���. �������������� �� �������� ����������� ���� � ������������� ������ ����������,
						   ���������� ������� �� 0 �� 5. (������ 5 ����� ���� ��� ��������� ������ ����������)
						*/
						amt = MAX(0, MIN(5, (GET_OBJ_WEIGHT(obj) * 100) / (GET_OBJ_VAL(obj, 0) *  20)));
						//sprintf(buf, "DEBUG 1: %d 2: %d 3: %d.\r\n", GET_OBJ_WEIGHT(obj), GET_OBJ_VAL(obj, 0), amt);
						//send_to_char(buf, ch);
						sprintf(buf, "��������%s ���������� %s:\r\n", GET_OBJ_SUF_6(obj), fullness[amt]);
						send_to_char(buf, ch);
					}
					list_obj_to_char(obj->contains, ch, 1, bits != FIND_OBJ_ROOM);
				}
			}
		}
		else  	// item must be a fountain or drink container
		{
			if (GET_OBJ_VAL(obj, 1) <= 0)
				send_to_char("�����.\r\n", ch);
			else
			{
				if (GET_OBJ_VAL(obj, 0) <= 0 || GET_OBJ_VAL(obj, 1) > GET_OBJ_VAL(obj, 0))
				{
					sprintf(buf, "��������%s ��������?!\r\n", GET_OBJ_SUF_6(obj));	// BUG
				}
				else
				{
					amt = (GET_OBJ_VAL(obj, 1) * 5) / GET_OBJ_VAL(obj, 0);
					sprinttype(GET_OBJ_VAL(obj, 2), color_liquid, buf2);
					sprintf(buf, "��������%s %s%s%s ���������.\r\n",
							GET_OBJ_SUF_6(obj), fullness[amt],
							buf2,
							(AFF_FLAGGED(ch, AFF_DETECT_POISON) &&
							 GET_OBJ_VAL(obj, 3) > 0 ? "(�����������)" : ""));
				}
				send_to_char(buf, ch);
			}
		}
	}
}



char *find_exdesc(char *word, EXTRA_DESCR_DATA * list)
{
	EXTRA_DESCR_DATA *i;

	for (i = list; i; i = i->next)
		if (isname(word, i->keyword))
			return (i->description);

	return (NULL);
}
//�-��� ������ ��� ���� �� �������
//buf ��� ����� � ������� ���������� ����, � ��� ��� ����� ���� ���-�� ����� ���� ����� ������� ��������� *buf='\0'
void obj_info(CHAR_DATA * ch, OBJ_DATA *obj, char buf[MAX_STRING_LENGTH])
{
	int j;
		if (can_use_feat(ch, SKILLED_TRADER_FEAT) || PRF_FLAGGED(ch, PRF_HOLYLIGHT))
		{
			sprintf(buf+strlen(buf), "�������� : %s", CCCYN(ch, C_NRM));
			sprinttype(obj->obj_flags.Obj_mater, material_name, buf+strlen(buf));
			sprintf(buf+strlen(buf), "\r\n%s", CCNRM(ch, C_NRM));
		}

		if (GET_OBJ_TYPE(obj) == ITEM_MING && (can_use_feat(ch, BREW_POTION_FEAT) || PRF_FLAGGED(ch, PRF_HOLYLIGHT)))
		{
			for (j = 0; imtypes[j].id != GET_OBJ_VAL(obj, IM_TYPE_SLOT)  && j <= top_imtypes;)
				j++;
			sprintf(buf+strlen(buf), "��� ���������� ���� '%s'.\r\n", imtypes[j].name);
			int imquality = GET_OBJ_VAL(obj, IM_POWER_SLOT);
			if (GET_LEVEL(ch) >= imquality)
			{
				sprintf(buf+strlen(buf), "�������� ����������� ");
				if (imquality > 25)
					strcat(buf+strlen(buf), "���������.\r\n");
				else if (imquality > 20)
					strcat(buf+strlen(buf), "��������.\r\n");
				else if (imquality > 15)
					strcat(buf+strlen(buf), "����� �������.\r\n");
				else if (imquality > 10)
					strcat(buf+strlen(buf), "���� ��������.\r\n");
				else if (imquality > 5)
					strcat(buf+strlen(buf), "������ ��������������.\r\n");
				else
					strcat(buf+strlen(buf), "���� �� ������.\r\n");
			}
			else
			{
				strcat(buf+strlen(buf), "�� �� � ��������� ���������� �������� ����� �����������.\r\n");
			}
		}

 		//|| PRF_FLAGGED(ch, PRF_HOLYLIGHT)
		if (can_use_feat(ch, MASTER_JEWELER_FEAT))
		{
			sprintf(buf+strlen(buf), "����� : %s", CCCYN(ch, C_NRM));
			if (OBJ_FLAGGED(obj, ITEM_WITH3SLOTS))
				strcat(buf, "�������� 3 �����\r\n");
			else if (OBJ_FLAGGED(obj, ITEM_WITH2SLOTS))
				strcat(buf, "�������� 2 �����\r\n");
			else if (OBJ_FLAGGED(obj, ITEM_WITH1SLOT))
				strcat(buf, "�������� 1 ����\r\n");
			else
				strcat(buf, "��� ������\r\n");
			sprintf(buf+strlen(buf), "\r\n%s", CCNRM(ch, C_NRM));
		}
		if (AUTH_CUSTOM_LABEL(obj, ch) && obj->custom_label->label_text)
		{
			if (obj->custom_label->clan)
				strcat(buf, "����� �������: ");
			else
				strcat(buf, "���� �����: ");
			sprintf(buf + strlen(buf), "%s\r\n", obj->custom_label->label_text);
		}
		sprintf(buf+strlen(buf), "%s", diag_uses_to_char(obj, ch));
}
/*
 * Given the argument "look at <target>", figure out what object or char
 * matches the target.  First, see if there is another char in the room
 * with the name.  Then check local objs for exdescs.
 *
 * Thanks to Angus Mezick <angus@EDGIL.CCMAIL.COMPUSERVE.COM> for the
 * suggested fix to this problem.
 * \return ���� ���� ������� � ����-������, ����� ����� ������� �� �������� ������ ��� �� look_in_obj
 */
bool look_at_target(CHAR_DATA * ch, char *arg, int subcmd)
{
	int bits, found = FALSE, fnum, i = 0, cn = 0;
	struct portals_list_type *port;
	CHAR_DATA *found_char = NULL;
	OBJ_DATA *found_obj = NULL;
	struct char_portal_type *tmp;
	char *desc, *what, whatp[MAX_INPUT_LENGTH], where[MAX_INPUT_LENGTH];
	int where_bits = FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP | FIND_CHAR_ROOM | FIND_OBJ_EXDESC;

	if (!ch->desc)
		return 0;

	if (!*arg)
	{
		send_to_char("�� ��� �� ��� �������� ����������?\r\n", ch);
		return 0;
	}

	half_chop(arg, whatp, where);
	what = whatp;

	if (isname(where, "����� ������� room ground"))
		where_bits = FIND_OBJ_ROOM | FIND_CHAR_ROOM;
	else if (isname(where, "��������� inventory"))
		where_bits = FIND_OBJ_INV;
	else if (isname(where, "���������� equipment"))
		where_bits = FIND_OBJ_EQUIP;

	// ��� townportal
	if (isname(whatp, "������") &&
//       IS_SET(GET_SPELL_TYPE(ch, SPELL_TOWNPORTAL), SPELL_KNOW) &&
			ch->get_skill(SKILL_TOWNPORTAL) &&
			(port = get_portal(GET_ROOM_VNUM(ch->in_room), NULL)) != NULL && IS_SET(where_bits, FIND_OBJ_ROOM))
	{
		if (GET_LEVEL(ch) < MAX(1, port->level - GET_REMORT(ch) / 2))
		{
			send_to_char("�� ����� ���-�� �������� ��������� �������.\r\n", ch);
			send_to_char("�� �� ��� ������������ �������, ����� ��������� �����.\r\n", ch);
			return 0;
		}
		else
		{
			for (tmp = GET_PORTALS(ch); tmp; tmp = tmp->next)
			{
				cn++;
			}
			if (cn >= MAX_PORTALS(ch))
			{
				send_to_char
				("��� ��������� ��� ����� ��� ���������, ������� � ���������� ���.\r\n", ch);
				return 0;
			}
			send_to_char("�� ����� ��������� ������� �������� ����� '&R", ch);
			send_to_char(port->wrd, ch);
			send_to_char("&n'.\r\n", ch);
			// ������ ��������� � ������ ����
			add_portal_to_char(ch, GET_ROOM_VNUM(ch->in_room));
			check_portals(ch);
			return 0;
		}
	}

	// ��������� � �����������
	if (isname(whatp, "�����������") && world[IN_ROOM(ch)]->portal_time && IS_SET(where_bits, FIND_OBJ_ROOM))
	{
		int r = IN_ROOM(ch), to_room;
		to_room = world[r]->portal_room;
		send_to_char("������������� � �����������, �� ��������� ��������� � ���.\r\n\r\n", ch);
		act("$n0 ��������� ��������$g � �����������.\r\n", TRUE, ch, 0, 0, TO_ROOM);
		if (world[to_room]->portal_time && (r == world[to_room]->portal_room))
		{
			send_to_char
			("����� ����, ������ � ���������������� ����� �������, ��������� ��� �����.\r\n\r\n", ch);
			return 0;
		}
		IN_ROOM(ch) = world[IN_ROOM(ch)]->portal_room;
		look_at_room(ch, 1);
		IN_ROOM(ch) = r;
		return 0;
	}

	bits = generic_find(what, where_bits, ch, &found_char, &found_obj);
	// Is the target a character?
	if (found_char != NULL)
	{
		if (subcmd == SCMD_LOOK_HIDE && !check_moves(ch, LOOKHIDE_MOVES))
			return 0;
		look_at_char(found_char, ch);
		if (ch != found_char)
		{
			if (subcmd == SCMD_LOOK_HIDE && ch->get_skill(SKILL_LOOK_HIDE) > 0)
			{
				fnum = number(1, skill_info[SKILL_LOOK_HIDE].max_percent);
				found =
					train_skill(ch, SKILL_LOOK_HIDE,
								skill_info[SKILL_LOOK_HIDE].max_percent, found_char);
				if (!WAITLESS(ch))
					WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
				if (found >= fnum && (fnum < 100 || IS_IMMORTAL(ch)) && !IS_IMMORTAL(found_char))
					return 0;
			}
			if (CAN_SEE(found_char, ch))
				act("$n �������$g ��� � ������ �� ���.", TRUE, ch, 0, found_char, TO_VICT);
			act("$n ���������$g �� $N3.", TRUE, ch, 0, found_char, TO_NOTVICT);
		}
		return 0;
	}

	// Strip off "number." from 2.foo and friends.
	if (!(fnum = get_number(&what)))
	{
		send_to_char("��� �����������?\r\n", ch);
		return 0;
	}

	// Does the argument match an extra desc in the room?
	if ((desc = find_exdesc(what, world[ch->in_room]->ex_description)) != NULL && ++i == fnum)
	{
		page_string(ch->desc, desc, FALSE);
		return 0;
	}

	// If an object was found back in generic_find
	if (bits && (found_obj != NULL))
	{

		if (Clan::ChestShow(found_obj, ch))
		{
			return 1;
		}
		if (ClanSystem::show_ingr_chest(found_obj, ch))
		{
			return 1;
		}
		if (Depot::is_depot(found_obj))
		{
			Depot::show_depot(ch);
			return 1;
		}

		// ���������� ���������. ������ �������� "if (!found)" ������� ��������
		// ������� �������� � �������, ���������� �������� "generic_find"
		if (!(desc = find_exdesc(what, found_obj->ex_description)))
			show_obj_to_char(found_obj, ch, 5, TRUE, 1);	// Show no-description
		else
		{
			send_to_char(desc, ch);
			show_obj_to_char(found_obj, ch, 6, TRUE, 1);	// Find hum, glow etc
		}

		*buf = '\0';
		obj_info(ch, found_obj, buf);
		send_to_char(buf, ch);
	}
	else
		send_to_char("������, ����� ����� ���!\r\n", ch);

	return 0;
}


void skip_hide_on_look(CHAR_DATA * ch)
{

	if (AFF_FLAGGED(ch, AFF_HIDE) &&
			((!ch->get_skill(SKILL_LOOK_HIDE) ||
			  ((number(1, 100) -
				calculate_skill(ch, SKILL_LOOK_HIDE,
								skill_info[SKILL_LOOK_HIDE].max_percent, 0) - 2 * (ch->get_wis() - 9)) > 0))))
	{
		affect_from_char(ch, SPELL_HIDE);
		if (!AFF_FLAGGED(ch, AFF_HIDE))
		{
			send_to_char("�� ���������� ���������.\r\n", ch);
			act("$n ���������$g ���������.", FALSE, ch, 0, 0, TO_ROOM);
		}
	}
	return;
}


ACMD(do_look)
{
	char arg2[MAX_INPUT_LENGTH];
	int look_type;

	if (!ch->desc)
		return;

	if (GET_POS(ch) < POS_SLEEPING)
		send_to_char("������� ����� ��� �����������...\r\n", ch);
	else if (AFF_FLAGGED(ch, AFF_BLIND))
		send_to_char("�� ���������!\r\n", ch);
	else if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch))
	{
		skip_hide_on_look(ch);

		send_to_char("������� �����...\r\n", ch);
		list_char_to_char(world[ch->in_room]->people, ch);	// glowing red eyes
		show_glow_objs(ch);
	}
	else
	{
		half_chop(argument, arg, arg2);

		skip_hide_on_look(ch);

		if (subcmd == SCMD_READ)
		{
			if (!*arg)
				send_to_char("��� �� ������ ���������?\r\n", ch);
			else
				look_at_target(ch, arg, subcmd);
			return;
		}
		if (!*arg)	// "look" alone, without an argument at all
			look_at_room(ch, 1);
		else if (is_abbrev(arg, "in") || is_abbrev(arg, "������"))
			look_in_obj(ch, arg2);
		// did the char type 'look <direction>?'
		else if (((look_type = search_block(arg, dirs, FALSE)) >= 0) ||
				 ((look_type = search_block(arg, Dirs, FALSE)) >= 0))
			look_in_direction(ch, look_type, EXIT_SHOW_WALL);
		else if (is_abbrev(arg, "at") || is_abbrev(arg, "��"))
			look_at_target(ch, arg2, subcmd);
		else
			look_at_target(ch, argument, subcmd);
	}
}

ACMD(do_sides)
{
	int i;

	if (!ch->desc)
		return;

	if (GET_POS(ch) <= POS_SLEEPING)
		send_to_char("������� ����� ��� �����������...\r\n", ch);
	else if (AFF_FLAGGED(ch, AFF_BLIND))
		send_to_char("�� ���������!\r\n", ch);
	else
	{
		skip_hide_on_look(ch);
		send_to_char("�� ���������� �� ��������.\r\n", ch);
		for (i = 0; i < NUM_OF_DIRS; i++)
		{
//         log("Look sides from %d to %d",world[IN_ROOM(ch)]->number, i);
			look_in_direction(ch, i, 0);
//           log("Look Ok !");
		}
	}
}


ACMD(do_looking)
{
	int i;

	if (!ch->desc)
		return;

	if (GET_POS(ch) < POS_SLEEPING)
		send_to_char("����� ����� ������ ����� ����, ������ ��������� ��������.\r\n", ch);
	if (GET_POS(ch) == POS_SLEEPING)
		send_to_char("������� ����� ��� �����������...\r\n", ch);
	else if (AFF_FLAGGED(ch, AFF_BLIND))
		send_to_char("�� ���������!\r\n", ch);
	else if (ch->get_skill(SKILL_LOOKING))
	{
		if (check_moves(ch, LOOKING_MOVES))
		{
			send_to_char("�� �������� ������ � ������ ��������������� �� ��������.\r\n", ch);
			for (i = 0; i < NUM_OF_DIRS; i++)
				look_in_direction(ch, i, EXIT_SHOW_LOOKING);
			if (!(IS_IMMORTAL(ch) || GET_GOD_FLAG(ch, GF_GODSLIKE)))
				WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
		}
	}
	else
		send_to_char("��� ���� �� ������� ����� ������.\r\n", ch);
}

ACMD(do_hearing)
{
	int i;

	if (!ch->desc)
		return;

	if (AFF_FLAGGED(ch, AFF_DEAFNESS))
	{
		send_to_char("�� ����� � ��� ����� ������ �� ��������.\r\n", ch);
		return;
	}

	if (GET_POS(ch) < POS_SLEEPING)
		send_to_char("��� ������ ��������� ������ �������, ������� ��� � ����.\r\n", ch);
	if (GET_POS(ch) == POS_SLEEPING)
		send_to_char("������ �������� ��������� ������ ����� �� ������� � ������� �����������.\r\n", ch);
	else if (ch->get_skill(SKILL_HEARING))
	{
		if (check_moves(ch, HEARING_MOVES))
		{
			send_to_char("�� ������ �������������� ��������������.\r\n", ch);
			for (i = 0; i < NUM_OF_DIRS; i++)
				hear_in_direction(ch, i, 0);
			if (!(IS_IMMORTAL(ch) || GET_GOD_FLAG(ch, GF_GODSLIKE)))
				WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
		}
	}
	else
		send_to_char("������� ������� ��� ��� ������� ������.\r\n", ch);
}



ACMD(do_examine)
{
	CHAR_DATA *tmp_char;
	OBJ_DATA *tmp_object;
	char where[MAX_INPUT_LENGTH];
	int where_bits = FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP | FIND_CHAR_ROOM | FIND_OBJ_EXDESC;


	if (GET_POS(ch) < POS_SLEEPING)
	{
		send_to_char("������� ����� ��� �����������...\r\n", ch);
		return;
	}
	else if (AFF_FLAGGED(ch, AFF_BLIND))
	{
		send_to_char("�� ���������!\r\n", ch);
		return;
	}

	two_arguments(argument, arg, where);

	if (!*arg)
	{
		send_to_char("��� �� ������� ���������?\r\n", ch);
		return;
	}

	if (isname(where, "����� ������� room ground"))
		where_bits = FIND_OBJ_ROOM | FIND_CHAR_ROOM;
	else if (isname(where, "��������� inventory"))
		where_bits = FIND_OBJ_INV;
	else if (isname(where, "���������� equipment"))
		where_bits = FIND_OBJ_EQUIP;

	skip_hide_on_look(ch);

	if (look_at_target(ch, argument, subcmd))
		return;

	if (isname(arg, "�����������") && world[IN_ROOM(ch)]->portal_time && IS_SET(where_bits, FIND_OBJ_ROOM))
		return;

	if (isname(arg, "������") &&
			ch->get_skill(SKILL_TOWNPORTAL) &&
			(get_portal(GET_ROOM_VNUM(ch->in_room), NULL)) != NULL && IS_SET(where_bits, FIND_OBJ_ROOM))
		return;

	generic_find(arg, where_bits, ch, &tmp_char, &tmp_object);
	if (tmp_object)
	{
		if ((GET_OBJ_TYPE(tmp_object) == ITEM_DRINKCON) ||
				(GET_OBJ_TYPE(tmp_object) == ITEM_FOUNTAIN) || (GET_OBJ_TYPE(tmp_object) == ITEM_CONTAINER))
			look_in_obj(ch, argument);
	}
}

ACMD(do_gold)
{
	int count = 0;
	if (ch->get_gold() == 0)
		send_to_char("�� ��������!\r\n", ch);
	else if (ch->get_gold() == 1)
		send_to_char("� ��� ���� ����� ���� ���� ����.\r\n", ch);
	else
	{
		count += sprintf(buf, "� ��� ���� %ld %s.\r\n", ch->get_gold(), desc_count(ch->get_gold(), WHAT_MONEYa));
		send_to_char(buf, ch);
	}
}

/// �� pc_class_name
const char *class_name[] = { "������",
							 "������",
							 "����",
							 "��������",
							 "�������",
							 "���������",
							 "��������",
							 "���������",
							 "������������",
							 "������",
							 "�������",
							 "������",
							 "�����",
							 "�����",
							 "����",
							 "�����",
							 "�����",
							 "�������",
							 "�������",
							 "�������",
							 "������",
							 "��������",
							 "����",
							 "�����",
							 "������",
							 "�����",
							 "������",
							 "������",
							 "�������",
							 "�����",
							 "�����",
							 "�����",
							 "�������",
							 "����",
							 "����������",
							 "�������",
							 "���������",
							 "�����",
							 "�������",
							 "������",
							 "������",
							 "����"
						   };


const char *ac_text[] =
{
	"&W�� �������� ��� ���",	//  -30
	"&W�� �������� ��� ���",	//  -29
	"&W�� �������� ��� ���",	//  -28
	"&g�� �������� ����� ��� ���",	//  -27
	"&g�� �������� ����� ��� ���",	//  -26
	"&g�� �������� ����� ��� ���",	//  -25
	"&g��������� ������",	//  -24
	"&g��������� ������",	//  -23
	"&g��������� ������",	//  -22
	"&g������������ ������",	//  -21
	"&g������������ ������",	//  -20
	"&g������������ ������",	//  -19
	"&g�������� ������",	//  -18
	"&g�������� ������",	//  -17
	"&g�������� ������",	//  -16
	"&G����� ������� ������",	//  -15
	"&G����� ������� ������",	//  -14
	"&G����� ������� ������",	//  -13
	"&G������ ������� ������",	//  -12
	"&G������ ������� ������",	//  -11
	"&G������ ������� ������",	//  -10
	"&G������� ������",	//   -9
	"&G������� ������",	//   -8
	"&G������� ������",	//   -7
	"&G�������� ������",	//   -6
	"&G�������� ������",	//   -5
	"&G�������� ������",	//   -4
	"&Y������ ���� ���� ��������",	//   -3
	"&Y������ ���� ���� ��������",	//   -2
	"&Y������ ���� ���� ��������",	//   -1
	"&Y������� ������",	//    0
	"&Y������ ���� ���� ��������",
	"&Y������ ������",
	"&R������ ������",
	"&R����� ������ ������",
	"&R�� ������� ��������",	// 5
	"&R�� ������ ������� ��������",
	"&r�� ����-���� ��������",
	"&r�� ����� �������",
	"&r�� ����� ��������� �������",
	"&r�� ��������� �������",	// 10
};

void print_do_score_all(CHAR_DATA *ch)
{
	int ac, max_dam = 0, hr = 0, resist, modi = 0, skill = SKILL_BOTHHANDS;

	std::string sum = string("�� ") + string(GET_PAD(ch, 0)) + string(", ")
		+ string(class_name[(int) GET_CLASS(ch)+14*GET_KIN(ch)]) + string(".");

	sprintf(buf,
			" %s-------------------------------------------------------------------------------------\r\n"
			" || %s%-80s%s||\r\n"
			" -------------------------------------------------------------------------------------\r\n",
			CCCYN(ch, C_NRM),
			CCNRM(ch, C_NRM), sum.substr(0, 80).c_str(), CCCYN(ch, C_NRM));

	sprintf(buf + strlen(buf),
			" || %s�����: %-11s %s|"
			" %s����:        %-3d(%-3d) %s|"
			" %s�����:       %4d %s|"
			" %s�������������: %s||\r\n",
			CCNRM(ch, C_NRM),
			string(PlayerRace::GetKinNameByNum(GET_KIN(ch),GET_SEX(ch))).substr(0, 14).c_str(),
			CCCYN(ch, C_NRM),
			CCICYN(ch, C_NRM), GET_HEIGHT(ch), GET_REAL_HEIGHT(ch), CCCYN(ch, C_NRM),
			CCIGRN(ch, C_NRM), GET_ARMOUR(ch), CCCYN(ch, C_NRM),
			CCIYEL(ch, C_NRM), CCCYN(ch, C_NRM));

	ac = compute_armor_class(ch) / 10;
	resist = MIN(GET_RESIST(ch, FIRE_RESISTANCE), 75);
	sprintf(buf + strlen(buf),
			" || %s���: %-13s %s|"
			" %s���:         %3d(%3d) %s|"
			" %s������:       %3d %s|"
			" %s����:      %3d %s||\r\n",
			CCNRM(ch, C_NRM),
            string(PlayerRace::GetRaceNameByNum(GET_KIN(ch),GET_RACE(ch),GET_SEX(ch))).substr(0, 14).c_str(),
			CCCYN(ch, C_NRM),
			CCICYN(ch, C_NRM), GET_WEIGHT(ch), GET_REAL_WEIGHT(ch), CCCYN(ch, C_NRM),
			CCIGRN(ch, C_NRM), ac, CCCYN(ch, C_NRM),
			CCIRED(ch, C_NRM), resist, CCCYN(ch, C_NRM));

	resist = MIN(GET_RESIST(ch, AIR_RESISTANCE), 75);
	sprintf(buf + strlen(buf),
			" || %s����: %-13s%s|"
			" %s������:      %3d(%3d) %s|"
			" %s����������:   %3d %s|"
			" %s�������:   %3d %s||\r\n",
			CCNRM(ch, C_NRM),
			string(religion_name[GET_RELIGION(ch)][(int) GET_SEX(ch)]).substr(0, 13).c_str(),
			CCCYN(ch, C_NRM),
			CCICYN(ch, C_NRM), GET_SIZE(ch), GET_REAL_SIZE(ch), CCCYN(ch, C_NRM),
			CCIGRN(ch, C_NRM), GET_ABSORBE(ch), CCCYN(ch, C_NRM),
			CCWHT(ch, C_NRM), resist, CCCYN(ch, C_NRM));

	max_dam = GET_REAL_DR(ch) + str_bonus(GET_REAL_STR(ch), STR_TO_DAM);

	if (can_use_feat(ch, BULLY_FEAT))
	{
		modi = 10 * (5 + (GET_EQ(ch, WEAR_HANDS) ? GET_OBJ_WEIGHT(GET_EQ(ch, WEAR_HANDS)) : 0));
		modi = 10 * (5 + (GET_EQ(ch, WEAR_HANDS) ? GET_OBJ_WEIGHT(GET_EQ(ch, WEAR_HANDS)) : 0));
		modi = MAX(100, modi);
		max_dam += modi * max_dam / 50;
		max_dam += MAX(0, GET_REAL_STR(ch) - 25);
	}
	else
		max_dam += 6 + 2 * GET_LEVEL(ch) / 3;

	OBJ_DATA* weapon = GET_EQ(ch, WEAR_BOTHS);
	if (weapon)
	{
		if (GET_OBJ_TYPE(weapon) == ITEM_WEAPON)
		{
			max_dam += GET_OBJ_VAL(weapon, 1) * (GET_OBJ_VAL(weapon, 2) + 1);
			skill = GET_OBJ_SKILL(weapon);
			if (ch->get_skill(skill) == 0)
			{
				hr -= (50 - MIN(50, GET_REAL_INT(ch))) / 3;
				max_dam -= (50 - MIN(50, GET_REAL_INT(ch))) / 6;
			}
			else
				apply_weapon_bonus(GET_CLASS(ch), skill, &max_dam, &hr);

		}
	}
	else
	{
		weapon = GET_EQ(ch, WEAR_WIELD);
		if (weapon)
		{
			if (GET_OBJ_TYPE(weapon) == ITEM_WEAPON)
			{
				max_dam += GET_OBJ_VAL(weapon, 1) * (GET_OBJ_VAL(weapon, 2) + 1) / 2;
				skill = GET_OBJ_SKILL(weapon);
				if (ch->get_skill(skill) == 0)
				{
					hr -= (50 - MIN(50, GET_REAL_INT(ch))) / 3;
					max_dam -= (50 - MIN(50, GET_REAL_INT(ch))) / 6;
				}
				else
					apply_weapon_bonus(GET_CLASS(ch), skill, &max_dam, &hr);
			}
		}

		weapon = GET_EQ(ch, WEAR_HOLD);
		if (weapon)
		{
			if (GET_OBJ_TYPE(weapon) == ITEM_WEAPON)
			{
				max_dam += GET_OBJ_VAL(weapon, 1) * (GET_OBJ_VAL(weapon, 2) + 1) / 2;
				skill = GET_OBJ_SKILL(weapon);
				if (ch->get_skill(skill) == 0)
				{
					hr -= (50 - MIN(50, GET_REAL_INT(ch))) / 3;
					max_dam -= (50 - MIN(50, GET_REAL_INT(ch))) / 6;
				}
				else
					apply_weapon_bonus(GET_CLASS(ch), skill, &max_dam, &hr);
			}
		}
	}

	if (can_use_feat(ch, WEAPON_FINESSE_FEAT))
	{
		if (weapon && GET_OBJ_WEIGHT(weapon) > 20)
		{
			hr += str_bonus(GET_REAL_STR(ch), STR_TO_HIT);
		}
		else
		{
			hr += str_bonus(GET_REAL_DEX(ch), STR_TO_HIT);
		}
	}
	else
	{
		hr += str_bonus(GET_REAL_STR(ch), STR_TO_HIT);
	}
	hr += GET_REAL_HR(ch) - thaco((int) GET_CLASS(ch), (int) GET_LEVEL(ch));
	if (PRF_FLAGGED(ch, PRF_POWERATTACK)) {
		hr -= 2;
		max_dam += 5;
	}
	if (PRF_FLAGGED(ch, PRF_GREATPOWERATTACK)) {
		hr -= 4;
		max_dam += 10;
	}
	if (PRF_FLAGGED(ch, PRF_AIMINGATTACK)) {
		hr += 2;
		max_dam -= 5;
	}
	if (PRF_FLAGGED(ch, PRF_GREATAIMINGATTACK)) {
		hr += 4;
		max_dam -= 10;
	}
	max_dam += ch->obj_bonus().calc_phys_dmg(max_dam);
	max_dam = MAX(0, max_dam);

	resist = MIN(GET_RESIST(ch, WATER_RESISTANCE), 75);
	sprintf(buf + strlen(buf),
			" || %s�������: %s%-2d        %s|"
			" %s����:          %2d(%2d) %s|"
			" %s�����:        %3d %s|"
			" %s����:      %3d %s||\r\n",
			CCNRM(ch, C_NRM), CCWHT(ch, C_NRM), GET_LEVEL(ch), CCCYN(ch, C_NRM),
			CCICYN(ch, C_NRM), ch->get_str(), GET_REAL_STR(ch), CCCYN(ch, C_NRM),
			CCIGRN(ch, C_NRM), hr, CCCYN(ch, C_NRM),
			CCICYN(ch, C_NRM), resist, CCCYN(ch, C_NRM));

	resist = MIN(GET_RESIST(ch, EARTH_RESISTANCE), 75);
	sprintf(buf + strlen(buf),
			" || %s��������������: %s%-2d %s|"
			" %s��������:      %2d(%2d) %s|"
			" %s����:        %4d %s|"
			" %s�����:     %3d %s||\r\n",
			CCNRM(ch, C_NRM), CCWHT(ch, C_NRM), GET_REMORT(ch), CCCYN(ch, C_NRM),
			CCICYN(ch, C_NRM), ch->get_dex(), GET_REAL_DEX(ch), CCCYN(ch, C_NRM),
			CCIGRN(ch, C_NRM), max_dam, CCCYN(ch, C_NRM),
			CCYEL(ch, C_NRM), resist, CCCYN(ch, C_NRM));

	sprintf(buf + strlen(buf),
			" || %s�������: %s%-3d       %s|"
			" %s������������:  %2d(%2d) %s|-------------------|----------------||\r\n",
			CCNRM(ch, C_NRM), CCWHT(ch, C_NRM), GET_AGE(ch), CCCYN(ch, C_NRM),
			CCICYN(ch, C_NRM), ch->get_con(), GET_REAL_CON(ch), CCCYN(ch, C_NRM));

	resist = MIN(GET_RESIST(ch, VITALITY_RESISTANCE), 75);
	sprintf(buf + strlen(buf),
			" || %s����: %s%-10ld   %s|"
			" %s��������:      %2d(%2d) %s|"
			" %s����������:   %3d %s|"
			" %s���������: %3d %s||\r\n",
			CCNRM(ch, C_NRM), CCWHT(ch, C_NRM), GET_EXP(ch), CCCYN(ch, C_NRM),
			CCICYN(ch, C_NRM), ch->get_wis(), GET_REAL_WIS(ch), CCCYN(ch, C_NRM),
			CCIGRN(ch, C_NRM), GET_CAST_SUCCESS(ch), CCCYN(ch, C_NRM),
			CCIYEL(ch, C_NRM), resist, CCCYN(ch, C_NRM));

	resist = MIN(GET_RESIST(ch, MIND_RESISTANCE), 75);

	if (IS_IMMORTAL(ch))
		sprintf(buf + strlen(buf), " || %s���: %s1%s             |",
				CCNRM(ch, C_NRM), CCWHT(ch, C_NRM), CCCYN(ch, C_NRM));
	else
		sprintf(buf + strlen(buf),
				" || %s���: %s%-10ld    %s|",
				CCNRM(ch, C_NRM), CCWHT(ch, C_NRM), level_exp(ch, GET_LEVEL(ch) + 1) - GET_EXP(ch), CCCYN(ch, C_NRM));

	sprintf(buf + strlen(buf),
			" %s��:            %2d(%2d) %s|"
			" %s�����������: %4d %s|"
			" %s�����:     %3d %s||\r\n",
			CCICYN(ch, C_NRM), ch->get_int(), GET_REAL_INT(ch), CCCYN(ch, C_NRM),
			CCIGRN(ch, C_NRM), GET_MANAREG(ch), CCCYN(ch, C_NRM),
			CCIYEL(ch, C_NRM), resist, CCCYN(ch, C_NRM));

	resist = MIN(GET_RESIST(ch, IMMUNITY_RESISTANCE), 75);
	sprintf(buf + strlen(buf),
			" || %s�����: %s%-8ld    %s|"
			" %s�������:       %2d(%2d) %s|-------------------|"
			" %s���������: %3d %s||\r\n",
			CCNRM(ch, C_NRM), CCWHT(ch, C_NRM), ch->get_gold(), CCCYN(ch, C_NRM),
			CCICYN(ch, C_NRM), ch->get_cha(), GET_REAL_CHA(ch), CCCYN(ch, C_NRM),
			CCIYEL(ch, C_NRM), resist, CCCYN(ch, C_NRM));

	sprintf(buf + strlen(buf),
			" || %s�� �����: %s%-8ld %s|"
			" %s�����:     %4d(%4d) %s|"
			" %s����:         %3d%s |----------------||\r\n",
			CCNRM(ch, C_NRM), CCWHT(ch, C_NRM), ch->get_bank(), CCCYN(ch, C_NRM),
			CCICYN(ch, C_NRM), GET_HIT(ch), GET_REAL_MAX_HIT(ch), CCCYN(ch, C_NRM),
			CCGRN(ch, C_NRM), GET_REAL_WIS(ch) - GET_SAVE(ch, SAVING_WILL), CCCYN(ch, C_NRM));

	if (!on_horse(ch))
		switch (GET_POS(ch))
		{
		case POS_DEAD:
			sprintf(buf + strlen(buf), " || %s%-19s%s|",
					CCIRED(ch, C_NRM), string("�� ������!").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			break;
		case POS_MORTALLYW:
			sprintf(buf + strlen(buf), " || %s%-19s%s|",
					CCIRED(ch, C_NRM), string("�� ��������!").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			break;
		case POS_INCAP:
			sprintf(buf + strlen(buf), " || %s%-19s%s|",
					CCRED(ch, C_NRM), string("�� ��� ��������.").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			break;
		case POS_STUNNED:
			sprintf(buf + strlen(buf), " || %s%-19s%s|",
					CCIYEL(ch, C_NRM), string("�� � ��������!").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			break;
		case POS_SLEEPING:
			sprintf(buf + strlen(buf), " || %s%-19s%s|",
					CCIGRN(ch, C_NRM), string("�� �����.").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			break;
		case POS_RESTING:
			sprintf(buf + strlen(buf), " || %s%-19s%s|",
					CCGRN(ch, C_NRM), string("�� ���������.").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			break;
		case POS_SITTING:
			sprintf(buf + strlen(buf), " || %s%-19s%s|",
					CCIGRN(ch, C_NRM), string("�� ������.").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			break;
		case POS_FIGHTING:
			if (ch->get_fighting())
				sprintf(buf + strlen(buf), " || %s%-19s%s|",
						CCIRED(ch, C_NRM), string("�� ����������!").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			else
				sprintf(buf + strlen(buf), " || %s%-19s%s|",
						CCRED(ch, C_NRM), string("�� ������ ��������.").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			break;
		case POS_STANDING:
			sprintf(buf + strlen(buf), " || %s%-19s%s|",
					CCNRM(ch, C_NRM), string("�� ������.").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			break;
		default:
			sprintf(buf + strlen(buf), " || %s%-19s%s|",
					CCNRM(ch, C_NRM), string("You are floating..").substr(0, 19).c_str(), CCCYN(ch, C_NRM));
			break;
		}
	else
		sprintf(buf + strlen(buf), " || %s%-19s%s|",
				CCNRM(ch, C_NRM), string("�� ������ ������.").substr(0, 19).c_str(), CCCYN(ch, C_NRM));

	sprintf(buf + strlen(buf),
			" %s������.:     %3d(%3d) %s|"
			" %s��������:     %3d %s|"
			" %s�����. �����:  %s||\r\n",
			CCICYN(ch, C_NRM), GET_MOVE(ch), GET_REAL_MAX_MOVE(ch), CCCYN(ch, C_NRM),
			CCGRN(ch, C_NRM), GET_REAL_CON(ch) - GET_SAVE(ch, SAVING_CRITICAL), CCCYN(ch, C_NRM),
			CCRED(ch, C_NRM), CCCYN(ch, C_NRM));

	if (GET_COND(ch, FULL) == 0)
		sprintf(buf + strlen(buf), " || %s�������: %s��� :(%s    |", CCNRM(ch, C_NRM), CCIRED(ch, C_NRM), CCCYN(ch, C_NRM));
	else
		sprintf(buf + strlen(buf), " || %s�������: %s���%s       |", CCNRM(ch, C_NRM), CCGRN(ch, C_NRM), CCCYN(ch, C_NRM));

	if (IS_MANA_CASTER(ch))
		sprintf(buf + strlen(buf),
				" %s���. ����: %4d(%4d) %s|",
				CCICYN(ch, C_NRM), GET_MANA_STORED(ch), GET_MAX_MANA(ch), CCCYN(ch, C_NRM));
	else
		strcat(buf, "                       |");

	sprintf(buf + strlen(buf),
			" %s���������:    %3d %s|"
			" %s  %+4d%% (%+4d) %s||\r\n",
			CCGRN(ch, C_NRM), GET_REAL_CON(ch) - GET_SAVE(ch, SAVING_STABILITY), CCCYN(ch, C_NRM),
			CCRED(ch, C_NRM), GET_HITREG(ch), hit_gain(ch), CCCYN(ch, C_NRM)
		   );

	if (GET_COND(ch, THIRST) == 0)
		sprintf(buf + strlen(buf),
				" || %s�����: %s�������!%s    |",
				CCNRM(ch, C_NRM), CCIRED(ch, C_NRM), CCCYN(ch, C_NRM));
	else
		sprintf(buf + strlen(buf),
				" || %s�����: %s���%s         |",
				CCNRM(ch, C_NRM), CCGRN(ch, C_NRM), CCCYN(ch, C_NRM));

	if (IS_MANA_CASTER(ch))
		sprintf(buf + strlen(buf),
				" %s�������.:    %3d ���. %s|",
				CCICYN(ch, C_NRM), mana_gain(ch), CCCYN(ch, C_NRM));
	else
		strcat(buf, "                       |");

	sprintf(buf + strlen(buf),
			" %s�������:      %3d %s|"
			" %s�����. ���:    %s||\r\n",
			CCGRN(ch, C_NRM), - GET_SAVE(ch, SAVING_REFLEX) + dex_bonus(GET_REAL_DEX(ch)), CCCYN(ch, C_NRM),
			CCRED(ch, C_NRM), CCCYN(ch, C_NRM));

	if (GET_COND(ch, DRUNK) >= CHAR_DRUNKED)
	{
		if (affected_by_spell(ch, SPELL_ABSTINENT))
			sprintf(buf + strlen(buf),
					" || %s��������.          %s|                       |",
					CCIYEL(ch, C_NRM), CCCYN(ch, C_NRM));
		else
			sprintf(buf + strlen(buf),
					" || %s�� �����.          %s|                       |",
					CCIGRN(ch, C_NRM), CCCYN(ch, C_NRM));
	}
	else
	{
		strcat(buf, " ||                    |                       |");
	}
	sprintf(buf + strlen(buf),
			" %s�����:       %4d %s|"
			" %s  %+4d%% (%+4d) %s||\r\n",
			CCGRN(ch, C_NRM), ch->calc_morale(), CCCYN(ch, C_NRM),
			CCRED(ch, C_NRM), GET_MOVEREG(ch), move_gain(ch), CCCYN(ch, C_NRM));

	const unsigned wdex = PlayerSystem::weight_dex_penalty(ch);
	if (wdex == 0)
	{
		strcat(buf, " ||                    |                       |");
	}
	else
	{
		sprintf(buf + strlen(buf),
			" || %s��������!%s          |                       |",
			wdex == 1 ? CCIYEL(ch, C_NRM) : CCIRED(ch, C_NRM),
			CCCYN(ch, C_NRM));
	}
	sprintf(buf + strlen(buf),
		" %s����������:  %4d %s|                ||\r\n"
		" -------------------------------------------------------------------------------------\r\n",
		CCGRN(ch, C_NRM), GET_INITIATIVE(ch), CCCYN(ch, C_NRM));

	if (has_horse(ch, FALSE))
	{
		if (on_horse(ch))
			sprintf(buf + strlen(buf),
					" %s|| %s�� ������ �� %-67s%s||\r\n"
					" -------------------------------------------------------------------------------------\r\n",
					CCCYN(ch, C_NRM), CCIGRN(ch, C_NRM),
					(string(GET_PAD(get_horse(ch), 5)) + string(".")).substr(0, 67).c_str(), CCCYN(ch, C_NRM));
		else
			sprintf(buf + strlen(buf),
					" %s|| %s� ��� ���� %-69s%s||\r\n"
					" -------------------------------------------------------------------------------------\r\n",
					CCCYN(ch, C_NRM), CCIGRN(ch, C_NRM),
					(string(GET_NAME(get_horse(ch))) + string(".")).substr(0, 69).c_str(), CCCYN(ch, C_NRM));
	}

	//���������� � �����, ���� ��� ����.
    ROOM_DATA *label_room = RoomSpells::find_affected_roomt(GET_ID(ch), SPELL_RUNE_LABEL);
	if (label_room)
	{
		sprintf(buf + strlen(buf),
				" %s|| &G&q�� ��������� ������ ����� � ������� %s%s||\r\n",
				CCCYN(ch, C_NRM),
				colored_name(string(string("'")+label_room->name+string("&n&Q'.")).c_str(), 44),
				CCCYN(ch, C_NRM));
	}

	int glory = Glory::get_glory(GET_UNIQUE(ch));
	if (glory)
		sprintf(buf + strlen(buf),
				" %s|| %s�� ��������� %5d %-61s%s||\r\n",
				CCCYN(ch, C_NRM), CCWHT(ch, C_NRM), glory,
				(string(desc_count(glory, WHAT_POINT)) + string(" �����.")).substr(0, 61).c_str(),
				CCCYN(ch, C_NRM));
	glory = GloryConst::get_glory(GET_UNIQUE(ch));
	if (glory)
		sprintf(buf + strlen(buf),
				" %s|| %s�� ��������� %5d %-61s%s||\r\n",
				CCCYN(ch, C_NRM), CCWHT(ch, C_NRM), glory,
				(string(desc_count(glory, WHAT_POINT)) + string(" ���������� �����.")).substr(0, 61).c_str(),
				CCCYN(ch, C_NRM));

	if (GET_GOD_FLAG(ch, GF_REMORT) && CLAN(ch))
	{
		sprintf(buf + strlen(buf),
			" || �� �������������� ������� ���� ���������� ���� ����� �������.                   ||\r\n");
	}
/*
	////////////////////////////////////////////////////////////////////////////
	std::stringstream tmp_out;

	// ��������� �������� ������, ���� ��� ��������� �����
	ch->add_today_torc(0);
	tmp_out << "��������� ������: "
		<< CCIYEL(ch, C_NRM) << ch->get_ext_money(ExtMoney::TORC_GOLD) << "� "
		<< CCWHT(ch, C_NRM) << ch->get_ext_money(ExtMoney::TORC_SILVER) << "� "
		<< CCYEL(ch, C_NRM) << ch->get_ext_money(ExtMoney::TORC_BRONZE) << "�"
		<< CCCYN(ch, C_NRM) << " " << ExtMoney::draw_daily_limit(ch);

	if (COLOR_LEV(ch) < C_NRM)
	{
		sprintf(buf + strlen(buf),
			" %s|| %-24s %54s %s||\r\n",
			CCCYN(ch, C_NRM),
			(PRF_FLAGGED(ch, PRF_SUMMONABLE) ? "�� ������ ���� ��������." : "�� �������� �� �������."),
			tmp_out.str().c_str(), CCCYN(ch, C_NRM));
	}
	else
	{
		sprintf(buf + strlen(buf),
			" %s|| %-24s %82s %s||\r\n",
			CCCYN(ch, C_NRM),
			(PRF_FLAGGED(ch, PRF_SUMMONABLE) ? "�� ������ ���� ��������." : "�� �������� �� �������."),
			tmp_out.str().c_str(), CCCYN(ch, C_NRM));
	}
	////////////////////////////////////////////////////////////////////////////
*/
	if (PRF_FLAGGED(ch, PRF_SUMMONABLE))
		sprintf(buf + strlen(buf),
				" || �� ������ ���� ��������.                                                        ||\r\n");
	else
		sprintf(buf + strlen(buf),
				" || �� �������� �� �������.                                                         ||\r\n");

	if (!NAME_GOD(ch) && GET_LEVEL(ch) <= NAME_LEVEL)
	{
		sprintf(buf + strlen(buf),
				" &c|| &R��������!&n ���� ��� �� ������� ����� �� �����!&c                                   ||\r\n");
		sprintf(buf + strlen(buf),
				" || &nC���� �� ���������� �������� ����, ���������� � ����� ��� ��������� �����.      &c||\r\n");
	}
	else if (NAME_BAD(ch))
	{
		sprintf(buf + strlen(buf),
				" || &R��������!&n ���� ��� ��������� ������. ����� ����� �� ���������� �������� ����.   &c||\r\n");
	}

	if (GET_LEVEL(ch) < LVL_IMMORT)
		sprintf(buf + strlen(buf),
				" || %s�� ������ �������� � ������ � ������������ ��������                             %s||\r\n"
				" || %s� %2d %-75s%s||\r\n",
				CCNRM(ch, C_NRM), CCCYN(ch, C_NRM), CCNRM(ch, C_NRM),
				grouping[(int)GET_CLASS(ch)][(int)GET_REMORT(ch)],
				(string(desc_count(grouping[(int)GET_CLASS(ch)][(int)GET_REMORT(ch)], WHAT_LEVEL))
				 + string(" ��� ������ ��� �����.")).substr(0, 76).c_str(), CCCYN(ch, C_NRM));

	if (RENTABLE(ch))
	{
		time_t rent_time = RENTABLE(ch) - time(0);
		int minutes = rent_time > 60 ? rent_time / 60 : 0;
		sprintf(buf + strlen(buf),
				" || %s� ����� � ������� ���������� �� �� ������ ���� �� ������ ��� %-18s%s ||\r\n",
				CCIRED(ch, C_NRM),
				minutes ? (boost::lexical_cast<std::string>(minutes) + string(" ") + string(desc_count(minutes, WHAT_MINu)) + string(".")).substr(0, 18).c_str()
						: (boost::lexical_cast<std::string>(rent_time) + string(" ") + string(desc_count(rent_time, WHAT_SEC)) + string(".")).substr(0, 18).c_str(),
				CCCYN(ch, C_NRM));
	}
	else if ((IN_ROOM(ch) != NOWHERE) && ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL) && !PLR_FLAGGED(ch, PLR_KILLER))
		sprintf(buf + strlen(buf),
				" || %s��� �� ���������� ���� � ������������.                                          %s||\r\n",
				CCIGRN(ch, C_NRM), CCCYN(ch, C_NRM));

	if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_SMITH) && (ch->get_skill(SKILL_INSERTGEM) || ch->get_skill(SKILL_REPAIR) || ch->get_skill(SKILL_TRANSFORMWEAPON)))
		sprintf(buf + strlen(buf),
				" || %s��� ����� ������� �������� ��� ������� ��������� �����.                         %s||\r\n",
				CCIGRN(ch, C_NRM), CCCYN(ch, C_NRM));

	if (mail::has_mail(ch->get_uid()))
		sprintf(buf + strlen(buf),
				" || %s��� ������� ����� ������, ������� �� �����.                                     %s||\r\n",
				CCIGRN(ch, C_NRM), CCCYN(ch, C_NRM));

	if (ch->get_protecting())
		sprintf(buf + strlen(buf),
				" || %s�� ����������� %-65s%s||\r\n",
				CCIGRN(ch, C_NRM), string(GET_PAD(ch->get_protecting(),3)+string(" �� ���������.")).substr(0,65).c_str(),
				CCCYN(ch, C_NRM));

	if (GET_GOD_FLAG(ch, GF_GODSCURSE) && GCURSE_DURATION(ch))
	{
		int hrs = (GCURSE_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((GCURSE_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf + strlen(buf),
				" || %s�� �������� ������ �� %3d %-5s %2d %-45s%s||\r\n",
				CCRED(ch, C_NRM), hrs, string(desc_count(hrs, WHAT_HOUR)).substr(0, 5).c_str(),
				mins, (string(desc_count(mins, WHAT_MINu)) + string(".")).substr(0, 45).c_str(), CCCYN(ch, C_NRM));
	}

	if (PLR_FLAGGED(ch, PLR_HELLED) && HELL_DURATION(ch) && HELL_DURATION(ch) > time(NULL))
	{
		int hrs = (HELL_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((HELL_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf + strlen(buf),
				" || %s��� ��������� �������� � ������� ��� %6d %-5s %2d %-27s%s||\r\n"
				" || %s[%-79s%s||\r\n",
				CCRED(ch, C_NRM), hrs, string(desc_count(hrs, WHAT_HOUR)).substr(0, 5).c_str(),
				mins, (string(desc_count(mins, WHAT_MINu)) + string(".")).substr(0, 27).c_str(),
				CCCYN(ch, C_NRM), CCRED(ch, C_NRM),
				(string(HELL_REASON(ch) ? HELL_REASON(ch) : "-") + string("].")).substr(0, 79).c_str(),
				CCCYN(ch, C_NRM));
	}

	if (PLR_FLAGGED(ch, PLR_MUTE) && MUTE_DURATION(ch) != 0 && MUTE_DURATION(ch) > time(NULL))
	{
		int hrs = (MUTE_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((MUTE_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf + strlen(buf),
				" || %s�� �� ������� ������� ��� %6d %-5s %2d %-38s%s||\r\n"
				" || %s[%-79s%s||\r\n",
				CCRED(ch, C_NRM), hrs, string(desc_count(hrs, WHAT_HOUR)).substr(0, 5).c_str(),
				mins, (string(desc_count(mins, WHAT_MINu)) + string(".")).substr(0, 38).c_str(),
				CCCYN(ch, C_NRM), CCRED(ch, C_NRM),
				(string(MUTE_REASON(ch) ? MUTE_REASON(ch) : "-") + string("].")).substr(0, 79).c_str(),
				CCCYN(ch, C_NRM));
	}

	if (!PLR_FLAGGED(ch, PLR_REGISTERED) && UNREG_DURATION(ch) != 0 && UNREG_DURATION(ch) > time(NULL))
	{
		int hrs = (UNREG_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((UNREG_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf + strlen(buf),
				" || %s�� �� ������� ������� � ������ IP ��� %6d %-5s %2d %-26s%s||\r\n"
				" || %s[%-79s%s||\r\n",
				CCRED(ch, C_NRM), hrs, string(desc_count(hrs, WHAT_HOUR)).substr(0, 5).c_str(),
				mins, (string(desc_count(mins, WHAT_MINu)) + string(".")).substr(0, 38).c_str(),
				CCCYN(ch, C_NRM), CCRED(ch, C_NRM),
				(string(UNREG_REASON(ch) ? UNREG_REASON(ch) : "-") + string("].")).substr(0, 79).c_str(),
				CCCYN(ch, C_NRM));
	}

	if (PLR_FLAGGED(ch, PLR_DUMB) && DUMB_DURATION(ch) != 0 && DUMB_DURATION(ch) > time(NULL))
	{
		int hrs = (DUMB_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((DUMB_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf + strlen(buf),
				" || %s�� ������ ������� ��� %6d %-5s %2d %-42s%s||\r\n"
				" || %s[%-79s%s||\r\n",
				CCRED(ch, C_NRM), hrs, string(desc_count(hrs, WHAT_HOUR)).substr(0, 5).c_str(),
				mins, (string(desc_count(mins, WHAT_MINu)) + string(".")).substr(0, 42).c_str(),
				CCCYN(ch, C_NRM), CCRED(ch, C_NRM),
				(string(DUMB_REASON(ch) ? DUMB_REASON(ch) : "-") + string("].")).substr(0, 79).c_str(),
				CCCYN(ch, C_NRM));
	}

	if (PLR_FLAGGED(ch, PLR_FROZEN) && FREEZE_DURATION(ch) != 0 && FREEZE_DURATION(ch) > time(NULL))
	{
		int hrs = (FREEZE_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((FREEZE_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf + strlen(buf),
				" || %s�� ������ ���������� ��� %6d %-5s %2d %-39s%s||\r\n"
				" || %s[%-79s%s||\r\n",
				CCRED(ch, C_NRM), hrs, string(desc_count(hrs, WHAT_HOUR)).substr(0, 5).c_str(),
				mins, (string(desc_count(mins, WHAT_MINu)) + string(".")).substr(0, 42).c_str(),
				CCCYN(ch, C_NRM), CCRED(ch, C_NRM),
				(string(FREEZE_REASON(ch) ? FREEZE_REASON(ch) : "-") + string("].")).substr(0, 79).c_str(),
				CCCYN(ch, C_NRM));
	}

	if (ch->is_morphed())
	{
		sprintf(buf + strlen(buf),
			" || %s�� ���������� � �������� ����� - %-47s%s||\r\n",
			CCYEL(ch, C_NRM),
			ch->get_morph_desc().substr(0, 47).c_str(),
			CCCYN(ch, C_NRM));
	}
	strcat(buf, " ||                                                                                 ||\r\n");
	strcat(buf, " -------------------------------------------------------------------------------------\r\n");
	strcat(buf, CCNRM(ch, C_NRM));
	send_to_char(buf, ch);
//	test_self_hitroll(ch);
}

ACMD(do_score)
{
	TIME_INFO_DATA playing_time;
	int ac, ac_t;

	skip_spaces(&argument);

	if (IS_NPC(ch))
		return;

	//��������� ������� "���� ���", ������� Adept. ������ ������� - 85 �������� + ������.
	if (is_abbrev(argument, "���") || is_abbrev(argument, "all"))
	{
		print_do_score_all(ch);
		return;
	}

	sprintf(buf, "�� %s (%s, %s, %s, %s %d ������).\r\n",
			ch->only_title().c_str(),
			string(PlayerRace::GetKinNameByNum(GET_KIN(ch),GET_SEX(ch))).c_str(),
            string(PlayerRace::GetRaceNameByNum(GET_KIN(ch),GET_RACE(ch),GET_SEX(ch))).c_str(),
			religion_name[GET_RELIGION(ch)][(int) GET_SEX(ch)],
			class_name[(int) GET_CLASS(ch)+14*GET_KIN(ch)], GET_LEVEL(ch));

	if (!NAME_GOD(ch) && GET_LEVEL(ch) <= NAME_LEVEL)
	{
		sprintf(buf + strlen(buf), "\r\n&R��������!&n ���� ��� �� ������� ����� �� �����!\r\n");
		sprintf(buf + strlen(buf), "����� ����� �� ���������� �������� ����,\r\n");
		sprintf(buf + strlen(buf), "���������� � ����� ��� ��������� �����.\r\n\r\n");
	}
	else if (NAME_BAD(ch))
	{
		sprintf(buf + strlen(buf), "\r\n&R��������!&n ���� ��� ��������� ������.\r\n");
		sprintf(buf + strlen(buf), "����� ����� �� ���������� �������� ����.\r\n\r\n");
	}

	sprintf(buf + strlen(buf), "������ ��� %d %s. ", GET_REAL_AGE(ch), desc_count(GET_REAL_AGE(ch), WHAT_YEAR));

	if (age(ch)->month == 0 && age(ch)->day == 0)
	{
		sprintf(buf2, "%s� ��� ������� ���� �������!%s\r\n", CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
		strcat(buf, buf2);
	}
	else
		strcat(buf, "\r\n");

	sprintf(buf + strlen(buf),
			"�� ������ ��������� %d(%d) %s �����������, � ������ %d(%d) %s �� ������ ���������.\r\n",
			GET_HIT(ch), GET_REAL_MAX_HIT(ch), desc_count(GET_HIT(ch),
					WHAT_ONEu),
			GET_MOVE(ch), GET_REAL_MAX_MOVE(ch), desc_count(GET_MOVE(ch), WHAT_MOVEu));

	if (IS_MANA_CASTER(ch))
	{
		sprintf(buf + strlen(buf),
				"���� ���������� ������� %d(%d) � �� ���������������� %d � ���.\r\n",
				GET_MANA_STORED(ch), GET_MAX_MANA(ch), mana_gain(ch));
	}

	sprintf(buf + strlen(buf),
			"%s���� �������������� :\r\n"
			"  ���� : %2d(%2d)"
			"  ���� : %2d(%2d)"
			"  ���� : %2d(%2d)"
			"  ���� : %2d(%2d)"
			"  ��   : %2d(%2d)"
			"  �����: %2d(%2d)\r\n"
			"  ������ %3d(%3d)"
			"  ����   %3d(%3d)"
			"  ���    %3d(%3d)%s\r\n",
			CCICYN(ch, C_NRM), ch->get_str(), GET_REAL_STR(ch),
			ch->get_dex(), GET_REAL_DEX(ch),
			ch->get_con(), GET_REAL_CON(ch),
			ch->get_wis(), GET_REAL_WIS(ch),
			ch->get_int(), GET_REAL_INT(ch),
			ch->get_cha(), GET_REAL_CHA(ch),
			GET_SIZE(ch), GET_REAL_SIZE(ch),
			GET_HEIGHT(ch), GET_REAL_HEIGHT(ch), GET_WEIGHT(ch), GET_REAL_WEIGHT(ch), CCNRM(ch, C_NRM));

	if (IS_IMMORTAL(ch))
	{
		sprintf(buf + strlen(buf),
				"%s���� ������ �������� :\r\n"
				"  AC   : %4d(%4d)"
				"  DR   : %4d(%4d)%s\r\n",
				CCIGRN(ch, C_NRM), GET_AC(ch), compute_armor_class(ch),
				GET_DR(ch), GET_REAL_DR(ch), CCNRM(ch, C_NRM));
	}
	else
	{
		ac = compute_armor_class(ch) / 10;
		ac_t = MAX(MIN(ac + 30, 40), 0);
		sprintf(buf + strlen(buf), "&G���� ������ �������� :\r\n"
				"  ������  (AC)     : %4d - %s&G\r\n"
				"  �����/���������� : %4d/%d&n\r\n",
				ac, ac_text[ac_t], GET_ARMOUR(ch), GET_ABSORBE(ch));
	}
	/*  if (charm_points(ch) > 0) {
	     sprintf(buf + strlen(buf),  " ������� ����������: %4d\r\n",
	             charm_points(ch));
	     sprintf(buf + strlen(buf),  " �� ��� ��������   : %4d\r\n",
	             charm_points(ch) - used_charm_points(ch));
	  } */
	sprintf(buf + strlen(buf), "��� ���� - %ld %s, � ��� �� ����� %ld %s",
			GET_EXP(ch), desc_count(GET_EXP(ch), WHAT_POINT), ch->get_gold(), desc_count(ch->get_gold(), WHAT_MONEYa));
	if (ch->get_bank() > 0)
		sprintf(buf + strlen(buf), "(� ��� %ld %s ���������� � �����).\r\n",
				ch->get_bank(), desc_count(ch->get_bank(), WHAT_MONEYa));
	else
		strcat(buf, ".\r\n");

	if (GET_LEVEL(ch) < LVL_IMMORT)
		sprintf(buf + strlen(buf),
				"��� �������� ������� %ld %s �� ���������� ������.\r\n",
				level_exp(ch, GET_LEVEL(ch) + 1) - GET_EXP(ch),
				desc_count(level_exp(ch, GET_LEVEL(ch) + 1) - GET_EXP(ch), WHAT_POINT));
	if (GET_LEVEL(ch) < LVL_IMMORT)
		sprintf(buf + strlen(buf),
				"�� ������ �������� � ������ � ������������ �������� � %d %s ��� ������ ��� �����.\r\n",
				grouping[(int)GET_CLASS(ch)][(int)GET_REMORT(ch)],
				desc_count(grouping[(int)GET_CLASS(ch)][(int)GET_REMORT(ch)], WHAT_LEVEL));

	//���������� � �����, ���� ��� ����.
    ROOM_DATA *label_room = RoomSpells::find_affected_roomt(GET_ID(ch), SPELL_RUNE_LABEL);
    if (label_room)
        sprintf(buf + strlen(buf),
                "&G&q�� ��������� ������ ����� � ������� '%s'.&Q&n\r\n",
                string(label_room->name).c_str());

	int glory = Glory::get_glory(GET_UNIQUE(ch));
	if (glory)
		sprintf(buf + strlen(buf), "�� ��������� %d %s �����.\r\n",
				glory, desc_count(glory, WHAT_POINT));
	glory = GloryConst::get_glory(GET_UNIQUE(ch));
	if (glory)
		sprintf(buf + strlen(buf), "�� ��������� %d %s ���������� �����.\r\n",
				glory, desc_count(glory, WHAT_POINT));

	playing_time = *real_time_passed((time(0) - ch->player_data.time.logon) + ch->player_data.time.played, 0);
	sprintf(buf + strlen(buf), "�� ������� %d %s %d %s ��������� �������.\r\n",
			playing_time.day, desc_count(playing_time.day, WHAT_DAY),
			playing_time.hours, desc_count(playing_time.hours, WHAT_HOUR));

	if (!on_horse(ch))
		switch (GET_POS(ch))
		{
		case POS_DEAD:
			strcat(buf, "�� ������!\r\n");
			break;
		case POS_MORTALLYW:
			strcat(buf, "�� ���������� ������ � ���������� � ������!\r\n");
			break;
		case POS_INCAP:
			strcat(buf, "�� ��� �������� � �������� ��������...\r\n");
			break;
		case POS_STUNNED:
			strcat(buf, "�� � ��������!\r\n");
			break;
		case POS_SLEEPING:
			strcat(buf, "�� �����.\r\n");
			break;
		case POS_RESTING:
			strcat(buf, "�� ���������.\r\n");
			break;
		case POS_SITTING:
			strcat(buf, "�� ������.\r\n");
			break;
		case POS_FIGHTING:
			if (ch->get_fighting())
				sprintf(buf + strlen(buf), "�� ���������� � %s.\r\n", GET_PAD(ch->get_fighting(), 4));
			else
				strcat(buf, "�� ������ �������� �� �������.\r\n");
			break;
		case POS_STANDING:
			strcat(buf, "�� ������.\r\n");
			break;
		default:
			strcat(buf, "You are floating.\r\n");
			break;
		}
	send_to_char(buf, ch);

	strcpy(buf, CCIGRN(ch, C_NRM));
	if (GET_COND(ch, DRUNK) >= CHAR_DRUNKED)
	{
		if (affected_by_spell(ch, SPELL_ABSTINENT))
			strcat(buf, "������ � �������� ������!\r\n");
		else
			strcat(buf, "�� �����.\r\n");
	}
	if (GET_COND(ch, FULL) == 0)
		strcat(buf, "�� �������.\r\n");
	if (GET_COND(ch, THIRST) == 0)
		strcat(buf, "��� ������ �����.\r\n");
	/*
	   strcat(buf, CCICYN(ch, C_NRM));
	   strcat(buf,"������� :\r\n");
	   sprintbits((ch)->char_specials.saved.affected_by, affected_bits, buf2, "\r\n");
	   strcat(buf,buf2);
	 */
	if (PRF_FLAGGED(ch, PRF_SUMMONABLE))
		strcat(buf, "�� ������ ���� ��������.\r\n");

	if (has_horse(ch, FALSE))
	{
		if (on_horse(ch))
			sprintf(buf + strlen(buf), "�� ������ �� %s.\r\n", GET_PAD(get_horse(ch), 5));
		else
			sprintf(buf + strlen(buf), "� ��� ���� %s.\r\n", GET_NAME(get_horse(ch)));
	}
	strcat(buf, CCNRM(ch, C_NRM));
	send_to_char(buf, ch);
	if (RENTABLE(ch))
	{
		sprintf(buf,
				"%s� ����� � ������� ���������� �� �� ������ ���� �� ������.%s\r\n",
				CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
		send_to_char(buf, ch);
	}
	else if ((IN_ROOM(ch) != NOWHERE) && ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL) && !PLR_FLAGGED(ch, PLR_KILLER))
	{
		sprintf(buf, "%s��� �� ���������� ���� � ������������.%s\r\n", CCIGRN(ch, C_NRM), CCNRM(ch, C_NRM));
		send_to_char(buf, ch);
	}

	if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_SMITH) && (ch->get_skill(SKILL_INSERTGEM) || ch->get_skill(SKILL_REPAIR) || ch->get_skill(SKILL_TRANSFORMWEAPON)))
	{
		sprintf(buf, "%s��� ����� ������� �������� ��� ������� ��������� �����.%s\r\n", CCIGRN(ch, C_NRM), CCNRM(ch, C_NRM));
		send_to_char(buf, ch);
	}

	if (mail::has_mail(ch->get_uid()))
	{
		sprintf(buf, "%s��� ������� ����� ������, ������� �� �����!%s\r\n", CCIGRN(ch, C_NRM), CCNRM(ch, C_NRM));
		send_to_char(buf, ch);
	}

	if (PLR_FLAGGED(ch, PLR_HELLED) && HELL_DURATION(ch) && HELL_DURATION(ch) > time(NULL))
	{
		int hrs = (HELL_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((HELL_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf,
				"��� ��������� �������� � ������� ��� %d %s %d %s [%s].\r\n",
				hrs, desc_count(hrs, WHAT_HOUR), mins, desc_count(mins,
						WHAT_MINu),
				HELL_REASON(ch) ? HELL_REASON(ch) : "-");
		send_to_char(buf, ch);
	}
	if (PLR_FLAGGED(ch, PLR_MUTE) && MUTE_DURATION(ch) != 0 && MUTE_DURATION(ch) > time(NULL))
	{
		int hrs = (MUTE_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((MUTE_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf, "�� �� ������� ������� ��� %d %s %d %s [%s].\r\n",
				hrs, desc_count(hrs, WHAT_HOUR),
				mins, desc_count(mins, WHAT_MINu), MUTE_REASON(ch) ? MUTE_REASON(ch) : "-");
		send_to_char(buf, ch);
	}
	if (PLR_FLAGGED(ch, PLR_DUMB) && DUMB_DURATION(ch) != 0 && DUMB_DURATION(ch) > time(NULL))
	{
		int hrs = (DUMB_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((DUMB_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf, "�� ������ ������� ��� %d %s %d %s [%s].\r\n",
				hrs, desc_count(hrs, WHAT_HOUR),
				mins, desc_count(mins, WHAT_MINu), DUMB_REASON(ch) ? DUMB_REASON(ch) : "-");
		send_to_char(buf, ch);
	}
	if (PLR_FLAGGED(ch, PLR_FROZEN) && FREEZE_DURATION(ch) != 0 && FREEZE_DURATION(ch) > time(NULL))
	{
		int hrs = (FREEZE_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((FREEZE_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf, "�� ������ ���������� ��� %d %s %d %s [%s].\r\n",
				hrs, desc_count(hrs, WHAT_HOUR),
				mins, desc_count(mins, WHAT_MINu), FREEZE_REASON(ch) ? FREEZE_REASON(ch) : "-");
		send_to_char(buf, ch);
	}

	if (!PLR_FLAGGED(ch, PLR_REGISTERED) && UNREG_DURATION(ch) != 0 && UNREG_DURATION(ch) > time(NULL))
	{
		int hrs = (UNREG_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((UNREG_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf, "�� �� ������� �������� � ������ IP ��� %d %s %d %s [%s].\r\n",
				hrs, desc_count(hrs, WHAT_HOUR),
				mins, desc_count(mins, WHAT_MINu), UNREG_REASON(ch) ? UNREG_REASON(ch) : "-");
		send_to_char(buf, ch);
	}

	if (GET_GOD_FLAG(ch, GF_GODSCURSE) && GCURSE_DURATION(ch))
	{
		int hrs = (GCURSE_DURATION(ch) - time(NULL)) / 3600;
		int mins = ((GCURSE_DURATION(ch) - time(NULL)) % 3600 + 59) / 60;
		sprintf(buf, "�� �������� ������ �� %d %s %d %s.\r\n",
				hrs, desc_count(hrs, WHAT_HOUR), mins, desc_count(mins, WHAT_MINu));
		send_to_char(buf, ch);
	}

	if (ch->is_morphed())
	{
		sprintf(buf, "�� ���������� � �������� ����� - %s.\r\n", ch->get_morph_desc().c_str());
		send_to_char(buf, ch);
	}
}

//29.11.09 ����������� ���������� ����� (�) ��������
// edited by WorM 2011.05.21
ACMD(do_mystat)
{
	skip_spaces(&argument);
	if (is_abbrev(argument, "��������") || is_abbrev(argument, "clear"))
	{
		GET_RIP_MOBTHIS(ch) = GET_EXP_MOBTHIS(ch) = GET_RIP_MOB(ch) = GET_EXP_MOB(ch) =
		GET_RIP_PKTHIS(ch) = GET_EXP_PKTHIS(ch) = GET_RIP_PK(ch) = GET_EXP_PK(ch) =
		GET_RIP_DTTHIS(ch) = GET_EXP_DTTHIS (ch) = GET_RIP_DT(ch) = GET_EXP_DT(ch) =
		GET_RIP_OTHERTHIS(ch) = GET_EXP_OTHERTHIS(ch) = GET_RIP_OTHER(ch) = GET_EXP_OTHER(ch) =
		GET_WIN_ARENA(ch) = GET_RIP_ARENA(ch) = GET_EXP_ARENA(ch) = 0;
		send_to_char("���������� �������.\r\n", ch);
	}
	else
	{
		sprintf(buf,    " &C--------------------------------------------------------------------------------------&n\r\n"
				" &C||&n   ���������� ����� �������   &C|&n         &W�������&n         &C|&n                         &C||&n\r\n"
				" &C||&n (����������, �������� �����) &C|&n      &W��������������&n     &C|&n           &K�����&n         &C||&n\r\n"
				" &C--------------------------------------------------------------------------------------&n\r\n"
				" &C||&n    � �������� ��� � �������: &C|&n &W%4d (%16llu)&n &C|&n &K%4d (%16llu)&n &C||&n\r\n"
				" &C||&n    � �������� ��� � �������: &C|&n &W%4d (%16llu)&n &C|&n &K%4d (%16llu)&n &C||&n\r\n"
				" &C||&n             � ������ ������: &C|&n &W%4d (%16llu)&n &C|&n &K%4d (%16llu)&n &C||&n\r\n"
				" &C||&n   �� �������� �������������: &C|&n &W%4d (%16llu)&n &C|&n &K%4d (%16llu)&n &C||&n\r\n"
				" &C--------------------------------------------------------------------------------------&n\r\n"
				" &C||&n                       &y�����:&n &C|&n &W%4d (%16llu)&n &C| &K%4d (%16llu)&n &n&C||&n\r\n"
				" &C--------------------------------------------------------------------------------------&n\r\n"
				" &C||&n &W�� ����� (�����):                                                                &n&C||&n\r\n"
				" &C||&n   &w����� �������:&n&r%4d&n     &w�������:&n&r%4d&n           &w�������� �����:&n &r%16llu&n &C||&n\r\n"
				" &C--------------------------------------------------------------------------------------&n\r\n"
				,
				GET_RIP_MOBTHIS(ch),GET_EXP_MOBTHIS(ch), GET_RIP_MOB(ch), GET_EXP_MOB(ch),
				GET_RIP_PKTHIS(ch),GET_EXP_PKTHIS(ch), GET_RIP_PK(ch), GET_EXP_PK(ch),
				GET_RIP_DTTHIS(ch),GET_EXP_DTTHIS (ch), GET_RIP_DT(ch), GET_EXP_DT(ch),
				GET_RIP_OTHERTHIS(ch),GET_EXP_OTHERTHIS(ch), GET_RIP_OTHER(ch), GET_EXP_OTHER(ch),
				GET_RIP_MOBTHIS(ch)+GET_RIP_PKTHIS(ch)+GET_RIP_DTTHIS(ch)+GET_RIP_OTHERTHIS(ch),
				GET_EXP_MOBTHIS(ch)+GET_EXP_PKTHIS(ch)+GET_EXP_DTTHIS(ch)+GET_EXP_OTHERTHIS(ch)+GET_EXP_ARENA(ch),
				GET_RIP_MOB(ch)+GET_RIP_PK(ch)+GET_RIP_DT(ch)+GET_RIP_OTHER(ch),
				GET_EXP_MOB(ch)+GET_EXP_PK(ch)+GET_EXP_DT(ch)+GET_EXP_OTHER(ch)+GET_EXP_ARENA(ch),
				GET_WIN_ARENA(ch),GET_RIP_ARENA(ch), GET_EXP_ARENA(ch));
		send_to_char(buf, ch);
	}
}
// end by WorM
// ����� ������ (�) ��������

ACMD(do_inventory)
{
	send_to_char("�� ������:\r\n", ch);
	list_obj_to_char(ch->carrying, ch, 1, 2);
}


ACMD(do_equipment)
{
	int i, found = 0;
	skip_spaces(&argument);

	send_to_char("�� ��� ������:\r\n", ch);
	for (i = 0; i < NUM_WEARS; i++)
	{
		if (GET_EQ(ch, i))
		{
			if (CAN_SEE_OBJ(ch, GET_EQ(ch, i)))
			{
				send_to_char(where[i], ch);
				show_obj_to_char(GET_EQ(ch, i), ch, 1, TRUE, 1);
				found = TRUE;
			}
			else
			{
				send_to_char(where[i], ch);
				send_to_char("���-��.\r\n", ch);
				found = TRUE;
			}
		}
		else		// added by Pereplut
		{
			if (is_abbrev(argument, "���") || is_abbrev(argument, "all"))
			{
				send_to_char(where[i], ch);
//           send_to_char("&K[ ������ ]&n\r\n", ch);
				sprintf(buf, "%s[ ������ ]%s\r\n", CCINRM(ch, C_NRM), CCNRM(ch, C_NRM));
				send_to_char(buf, ch);
				found = TRUE;
			}
		}
	}
	if (!found)
	{
		if (IS_FEMALE(ch))
			send_to_char("������ ��� ��� ����� ���� :)\r\n", ch);
		else
			send_to_char(" �� ����, ��� �����.\r\n", ch);
	}
}


ACMD(do_time)
{
	int day, month, days_go;
	if (IS_NPC(ch))
		return;
	sprintf(buf, "������ ");
	switch (time_info.hours % 24)
	{
	case 0:
		sprintf(buf + strlen(buf), "�������, ");
		break;
	case 1:
		sprintf(buf + strlen(buf), "1 ��� ����, ");
		break;
	case 2:
	case 3:
	case 4:
		sprintf(buf + strlen(buf), "%d ���� ����, ", time_info.hours);
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		sprintf(buf + strlen(buf), "%d ����� ����, ", time_info.hours);
		break;
	case 12:
		sprintf(buf + strlen(buf), "�������, ");
		break;
	case 13:
		sprintf(buf + strlen(buf), "1 ��� ���������, ");
		break;
	case 14:
	case 15:
	case 16:
		sprintf(buf + strlen(buf), "%d ���� ���������, ", time_info.hours - 12);
		break;
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
		sprintf(buf + strlen(buf), "%d ����� ������, ", time_info.hours - 12);
		break;
	}

	if (GET_RELIGION(ch) == RELIGION_POLY)
		strcat(buf, weekdays_poly[weather_info.week_day_poly]);
	else
		strcat(buf, weekdays[weather_info.week_day_mono]);
	switch (weather_info.sunlight)
	{
	case SUN_DARK:
		strcat(buf, ", ����");
		break;
	case SUN_SET:
		strcat(buf, ", �����");
		break;
	case SUN_LIGHT:
		strcat(buf, ", ����");
		break;
	case SUN_RISE:
		strcat(buf, ", �������");
		break;
	}
	strcat(buf, ".\r\n");
	send_to_char(buf, ch);

	day = time_info.day + 1;	// day in [1..35]
	*buf = '\0';
	if (GET_RELIGION(ch) == RELIGION_POLY || IS_IMMORTAL(ch))
	{
		days_go = time_info.month * DAYS_PER_MONTH + time_info.day;
		month = days_go / 40;
		days_go = (days_go % 40) + 1;
		sprintf(buf + strlen(buf), "%s, %d� ����, ��� %d%s",
				month_name_poly[month], days_go, time_info.year, IS_IMMORTAL(ch) ? ".\r\n" : "");
	}
	if (GET_RELIGION(ch) == RELIGION_MONO || IS_IMMORTAL(ch))
		sprintf(buf + strlen(buf), "%s, %d� ����, ��� %d",
				month_name[(int) time_info.month], day, time_info.year);
	if (IS_IMMORTAL(ch))
		sprintf(buf + strlen(buf), "\r\n %d.%d.%d", day, time_info.month+1, time_info.year);
	switch (weather_info.season)
	{
	case SEASON_WINTER:
		strcat(buf, ", ����");
		break;
	case SEASON_SPRING:
		strcat(buf, ", �����");
		break;
	case SEASON_SUMMER:
		strcat(buf, ", ����");
		break;
	case SEASON_AUTUMN:
		strcat(buf, ", �����");
		break;
	}
	strcat(buf, ".\r\n");
	send_to_char(buf, ch);
	gods_day_now(ch);
}

int get_moon(int sky)
{
	if (weather_info.sunlight == SUN_RISE || weather_info.sunlight == SUN_LIGHT || sky == SKY_RAINING)
		return (0);
	else if (weather_info.moon_day <= NEWMOONSTOP || weather_info.moon_day >= NEWMOONSTART)
		return (1);
	else if (weather_info.moon_day < HALFMOONSTART)
		return (2);
	else if (weather_info.moon_day < FULLMOONSTART)
		return (3);
	else if (weather_info.moon_day <= FULLMOONSTOP)
		return (4);
	else if (weather_info.moon_day < LASTHALFMOONSTART)
		return (5);
	else
		return (6);
	return (0);
}



ACMD(do_weather)
{
	int sky = weather_info.sky, weather_type = weather_info.weather_type;
	const char *sky_look[] = { "��������",
							   "���������",
							   "������� �������� ������",
							   "�����"
							 };
	const char *moon_look[] = { "���������.",
								"�������� ���� ����.",
								"�������� ����.",
								"����������.",
								"��������� ����.",
								"��������� ���� ����."
							  };

	if (OUTSIDE(ch))
	{
		*buf = '\0';
		if (world[IN_ROOM(ch)]->weather.duration > 0)
		{
			sky = world[IN_ROOM(ch)]->weather.sky;
			weather_type = world[IN_ROOM(ch)]->weather.weather_type;
		}
		sprintf(buf + strlen(buf),
				"���� %s. %s\r\n%s\r\n", sky_look[sky],
				get_moon(sky) ? moon_look[get_moon(sky) - 1] : "",
				(weather_info.change >=
				 0 ? "����������� �������� ����������." : "����������� �������� ����������."));
		sprintf(buf + strlen(buf), "�� ����� %d %s.\r\n",
				weather_info.temperature, desc_count(weather_info.temperature, WHAT_DEGREE));

		if (IS_SET(weather_info.weather_type, WEATHER_BIGWIND))
			strcat(buf, "������� �����.\r\n");
		else if (IS_SET(weather_info.weather_type, WEATHER_MEDIUMWIND))
			strcat(buf, "��������� �����.\r\n");
		else if (IS_SET(weather_info.weather_type, WEATHER_LIGHTWIND))
			strcat(buf, "������ �������.\r\n");

		if (IS_SET(weather_type, WEATHER_BIGSNOW))
			strcat(buf, "����� ����.\r\n");
		else if (IS_SET(weather_type, WEATHER_MEDIUMSNOW))
			strcat(buf, "��������.\r\n");
		else if (IS_SET(weather_type, WEATHER_LIGHTSNOW))
			strcat(buf, "������ ������.\r\n");

		if (IS_SET(weather_type, WEATHER_GRAD))
			strcat(buf, "����� � ������.\r\n");
		else if (IS_SET(weather_type, WEATHER_BIGRAIN))
			strcat(buf, "����, ��� �� �����.\r\n");
		else if (IS_SET(weather_type, WEATHER_MEDIUMRAIN))
			strcat(buf, "���� �����.\r\n");
		else if (IS_SET(weather_type, WEATHER_LIGHTRAIN))
			strcat(buf, "������� ������.\r\n");

		send_to_char(buf, ch);
	}
	else
		send_to_char("�� ������ �� ������ ������� � ������ �������.\r\n", ch);
	if (IS_GOD(ch))
	{
		sprintf(buf, "����: %d �����: %s ���: %d ���� = %d\r\n"
				"����������� =%-5d, �� ���� = %-8d, �� ������ = %-8d\r\n"
				"��������    =%-5d, �� ���� = %-8d, �� ������ = %-8d\r\n"
				"������ ����� = %d(%d), ����� = %d(%d). ��� = %d(%d). ������ = %08x(%08x).\r\n",
				time_info.day, month_name[time_info.month], time_info.hours,
				weather_info.hours_go, weather_info.temperature,
				weather_info.temp_last_day, weather_info.temp_last_week,
				weather_info.pressure, weather_info.press_last_day,
				weather_info.press_last_week, weather_info.rainlevel,
				world[IN_ROOM(ch)]->weather.rainlevel, weather_info.snowlevel,
				world[IN_ROOM(ch)]->weather.snowlevel, weather_info.icelevel,
				world[IN_ROOM(ch)]->weather.icelevel,
				weather_info.weather_type, world[IN_ROOM(ch)]->weather.weather_type);
		send_to_char(buf, ch);
	}
}

namespace
{

const char* IMM_WHO_FORMAT =
"������: ��� [�������[-��������]] [-n ���] [-c ��������] [-s] [-r] [-z] [-h] [-b|-�]\r\n";

const char* MORT_WHO_FORMAT = "������: ��� [���] [-?]\r\n";

} // namespace

ACMD(do_who)
{
	char name_search[MAX_INPUT_LENGTH];
	name_search[0] = '\0';

	// ����� ��� �����
	int low = 0, high = LVL_IMPL;
	int showclass = 0, num_can_see = 0;
	int imms_num = 0, morts_num = 0, demigods_num = 0;
	bool localwho = false, short_list = false;
	bool who_room = false, showname = false;

	skip_spaces(&argument);
	strcpy(buf, argument);

	// �������� ���������� ������� "���"
	while (*buf)
	{
		half_chop(buf, arg, buf1);
		if (!str_cmp(arg, "����") && strlen(arg) == 4)
		{
			low = LVL_IMMORT;
			high = LVL_IMPL;
			strcpy(buf, buf1);
		}
		else if (a_isdigit(*arg))
		{
			if (IS_GOD(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
				sscanf(arg, "%d-%d", &low, &high);
			strcpy(buf, buf1);
		}
		else if (*arg == '-')
		{
			char mode = *(arg + 1);	// just in case; we destroy arg in the switch
			switch (mode)
			{
			case 'b':
			case '�':
				if (IS_IMMORTAL(ch) || GET_GOD_FLAG(ch, GF_DEMIGOD) || PRF_FLAGGED(ch, PRF_CODERINFO))
					showname = true;
				strcpy(buf, buf1);
				break;
			case 'z':
				if (IS_GOD(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
					localwho = true;
				strcpy(buf, buf1);
				break;
			case 's':
				if (IS_IMMORTAL(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
					short_list = true;
				strcpy(buf, buf1);
				break;
			case 'l':
				half_chop(buf1, arg, buf);
				if (IS_GOD(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
					sscanf(arg, "%d-%d", &low, &high);
				break;
			case 'n':
				half_chop(buf1, name_search, buf);
				break;
			case 'r':
				if (IS_GOD(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
					who_room = true;
				strcpy(buf, buf1);
				break;
			case 'c':
				half_chop(buf1, arg, buf);
				if (IS_GOD(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
				{
					const size_t len = strlen(arg);
					for (size_t i = 0; i < len; i++)
					{
						showclass |= find_class_bitvector(arg[i]);
					}
				}
				break;
			case 'h':
			case '?':
			default:
				if (IS_IMMORTAL(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
					send_to_char(IMM_WHO_FORMAT, ch);
				else
					send_to_char(MORT_WHO_FORMAT, ch);
				return;
			}	// end of switch
		}
		else  	// endif
		{
			strcpy(name_search, arg);
			strcpy(buf, buf1);

		}
	}			// end while (parser)

	if (who_spamcontrol(ch, strlen(name_search) ? WHO_LISTNAME : WHO_LISTALL))
		return;

	// ������ ���������� �����
	sprintf(buf, "%s����%s\r\n", CCICYN(ch, C_NRM), CCNRM(ch, C_NRM));
	std::string imms(buf);

	sprintf(buf, "%s�����������������%s\r\n", CCCYN(ch, C_NRM), CCNRM(ch, C_NRM));
	std::string demigods(buf);

	sprintf(buf, "%s������%s\r\n", CCCYN(ch, C_NRM), CCNRM(ch, C_NRM));
	std::string morts(buf);

	int all = 0;

	for (CHAR_DATA* tch = character_list; tch; tch = tch->next)
	{
		if (IS_NPC(tch))
			continue;

		if (!HERE(tch))
			continue;

		if (!*argument && GET_LEVEL(tch) < LVL_IMMORT)
			++all;

		if (*name_search && !(isname(name_search, GET_NAME(tch))))
			continue;

		if (!CAN_SEE_CHAR(ch, tch) || GET_LEVEL(tch) < low || GET_LEVEL(tch) > high)
			continue;
		if (localwho && world[ch->in_room]->zone != world[tch->in_room]->zone)
			continue;
		if (who_room && (tch->in_room != ch->in_room))
			continue;
		if (showclass && !(showclass & (1 << GET_CLASS(tch))))
			continue;
		if (showname && !(!NAME_GOD(tch) && GET_LEVEL(tch) <= NAME_LEVEL))
			continue;
		if (PLR_FLAGGED(tch, PLR_NAMED) && NAME_DURATION(tch) && !IS_IMMORTAL(ch) && !PRF_FLAGGED(ch, PRF_CODERINFO) && ch != tch)
			continue;

		*buf = '\0';
		num_can_see++;

		if (short_list)
		{
			char tmp[MAX_INPUT_LENGTH];
			snprintf(tmp, sizeof(tmp), "%s%s%s",
				CCPK(ch, C_NRM, tch), GET_NAME(tch), CCNRM(ch, C_NRM));
			if (IS_IMPL(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
			{
				sprintf(buf, "%s[%2d %s %s] %-30s%s",
					IS_GOD(tch) ? CCWHT(ch, C_SPR) : "",
					GET_LEVEL(tch), KIN_ABBR(tch), CLASS_ABBR(tch),
					tmp, IS_GOD(tch) ? CCNRM(ch, C_SPR) : "");
			}
			else
			{
				sprintf(buf, "%s%-30s%s",
					IS_IMMORTAL(tch) ? CCWHT(ch, C_SPR) : "",
					tmp, IS_IMMORTAL(tch) ? CCNRM(ch, C_SPR) : "");
			}
		}
		else
		{
			if (IS_IMPL(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
				sprintf(buf, "%s[%2d %2d %s(%5d)] %s%s%s%s",
						IS_IMMORTAL(tch) ? CCWHT(ch, C_SPR) : "",
						GET_LEVEL(tch),
						GET_REMORT(tch),
						CLASS_ABBR(tch),
						tch->get_pfilepos(),
						CCPK(ch, C_NRM, tch),
						IS_IMMORTAL(tch) ? CCWHT(ch, C_SPR) : "", tch->race_or_title().c_str(), CCNRM(ch, C_NRM));
			else
				sprintf(buf, "%s %s%s%s",
						CCPK(ch, C_NRM, tch),
						IS_IMMORTAL(tch) ? CCWHT(ch, C_SPR) : "", tch->race_or_title().c_str(), CCNRM(ch, C_NRM));

			if (GET_INVIS_LEV(tch))
				sprintf(buf + strlen(buf), " (i%d)", GET_INVIS_LEV(tch));
			else if (AFF_FLAGGED(tch, AFF_INVISIBLE))
				sprintf(buf + strlen(buf), " (�������%s)", GET_CH_SUF_6(tch));
			if (AFF_FLAGGED(tch, AFF_HIDE))
				strcat(buf, " (��������)");
			if (AFF_FLAGGED(tch, AFF_CAMOUFLAGE))
				strcat(buf, " (�����������)");

			if (PLR_FLAGGED(tch, PLR_MAILING))
				strcat(buf, " (���������� ������)");
			else if (PLR_FLAGGED(tch, PLR_WRITING))
				strcat(buf, " (�����)");

			if (PRF_FLAGGED(tch, PRF_NOHOLLER))
				sprintf(buf + strlen(buf), " (����%s)", GET_CH_SUF_1(tch));
			if (PRF_FLAGGED(tch, PRF_NOTELL))
				sprintf(buf + strlen(buf), " (�����%s)", GET_CH_SUF_6(tch));
			if (PLR_FLAGGED(tch, PLR_MUTE))
				sprintf(buf + strlen(buf), " (������)");
			if (PLR_FLAGGED(tch, PLR_DUMB))
				sprintf(buf + strlen(buf), " (���%s)", GET_CH_SUF_6(tch));
			if (PLR_FLAGGED(tch, PLR_KILLER) == PLR_KILLER)
				sprintf(buf + strlen(buf), "&R (�������)&n");
			if (IS_IMMORTAL(ch) && !NAME_GOD(tch)
					&& GET_LEVEL(tch) <= NAME_LEVEL)
			{
				sprintf(buf + strlen(buf), " &W!�� ��������!&n");
				if (showname)
				{
					sprintf(buf + strlen(buf),
							"\r\n������: %s/%s/%s/%s/%s/%s Email: &S%s&s ���: %s",
							GET_PAD(tch, 0), GET_PAD(tch, 1), GET_PAD(tch, 2),
							GET_PAD(tch, 3), GET_PAD(tch, 4), GET_PAD(tch, 5), GET_EMAIL(tch),
							genders[(int)GET_SEX(tch)]);
				}
			}
			else if ((IS_IMMORTAL(ch) || PRF_FLAGGED(ch, PRF_CODERINFO)) && NAME_BAD(tch))
			{
				sprintf(buf + strlen(buf), " &W������ %s!&n", get_name_by_id(NAME_ID_GOD(tch)));
			}
			if (IS_IMMORTAL(tch))
				strcat(buf, CCNRM(ch, C_SPR));
		}		// endif shortlist

		if (IS_IMMORTAL(tch))
		{
			imms_num++;
			imms += buf;
			if (!short_list || !(imms_num % 4))
			{
				imms += "\r\n";
			}
		}
		else if (GET_GOD_FLAG(tch, GF_DEMIGOD)
			&& (IS_IMMORTAL(ch) || PRF_FLAGGED(ch, PRF_CODERINFO)))
		{
			demigods_num++;
			demigods += buf;
			if (!short_list || !(demigods_num % 4))
			{
				demigods += "\r\n";
			}
		}
		else
		{
			morts_num++;
			morts += buf;
			if (!short_list || !(morts_num % 4))
				morts += "\r\n";
		}
	}			// end of for

	if (morts_num + imms_num + demigods_num == 0)
	{
		send_to_char("\r\n�� ������ �� ������.\r\n", ch);
		// !!!
		return;
	}

	std::string out;

	if (imms_num > 0)
	{
		out += imms;
	}
	if (demigods_num > 0)
	{
		if (short_list)
		{
			out += "\r\n";
		}
		out += demigods;
	}
	if (morts_num > 0)
	{
		if (short_list)
		{
			out += "\r\n";
		}
		out += morts;
	}

	out += "\r\n�����:";
	if (imms_num)
	{
		sprintf(buf, " ����������� %d", imms_num);
		out += buf;
	}
	if (demigods_num)
	{
		sprintf(buf, " ����������������� %d", demigods_num);
		out += buf;
	}
	if (all && morts_num)
	{
		sprintf(buf, " �������� %d (������� %d)", all, morts_num);
		out += buf;
	}
	else if (morts_num)
	{
		sprintf(buf, " �������� %d", morts_num);
		out += buf;
	}

	out += ".\r\n";
	page_string(ch->desc, out);
}

extern time_t boot_time;

std::string print_server_uptime()
{
	time_t diff = time(0) - boot_time;
	int d = diff / 86400;
	int h = (diff / 3600) % 24;
	int m = (diff / 60) % 60;
	int s = diff % 60;
	return boost::str(boost::format(
		"������� � ������������: %d� %02d:%02d:%02d\r\n") % d % h % m % s);
}

ACMD(do_statistic)
{
	CHAR_DATA *tch;
	int proff[NUM_CLASSES][2], ptot[NUM_CLASSES], i, clan = 0, noclan = 0, hilvl = 0, lowlvl = 0,
			all = 0, rem = 0, norem = 0, pk = 0, nopk = 0;

	for (i = 0; i < NUM_CLASSES; i++)
	{
		proff[i][0] = 0;
		proff[i][1] = 0;
		ptot[i] = 0;
	}

	for (tch = character_list; tch; tch = tch->next)
	{
		if (IS_NPC(tch) || GET_LEVEL(tch) >= LVL_IMMORT || !HERE(tch))
			continue;

		if (CLAN(tch))
			clan++;
		else
			noclan++;
		if (GET_LEVEL(tch) >= 25)
			hilvl++;
		else
			lowlvl++;
		if (GET_REMORT(tch) >= 1)
			rem++;
		else
			norem++;
		all++;
		if (pk_count(tch) >= 1)
			pk++;
		else
			nopk++;

		if (GET_LEVEL(tch) >= 25)
			proff[(int)GET_CLASS(tch)][0]++;
		else
			proff[(int)GET_CLASS(tch)][1]++;
		ptot[(int)GET_CLASS(tch)]++;
	}
	sprintf(buf, "%s���������� �� �������, ����������� � ���� (����� / 25 � ���� / ���� 25):%s\r\n", CCICYN(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "������        %s[%s%2d/%2d/%2d%s]%s       ",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_CLERIC], proff[CLASS_CLERIC][0], proff[CLASS_CLERIC][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "�������     %s[%s%2d/%2d/%2d%s]%s\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_BATTLEMAGE], proff[CLASS_BATTLEMAGE][0],
			proff[CLASS_BATTLEMAGE][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "����          %s[%s%2d/%2d/%2d%s]%s       ",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_THIEF], proff[CLASS_THIEF][0], proff[CLASS_THIEF][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "��������    %s[%s%2d/%2d/%2d%s]%s\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_WARRIOR], proff[CLASS_WARRIOR][0], proff[CLASS_WARRIOR][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "��������      %s[%s%2d/%2d/%2d%s]%s       ",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_ASSASINE], proff[CLASS_ASSASINE][0],
			proff[CLASS_ASSASINE][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "����������  %s[%s%2d/%2d/%2d%s]%s\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_GUARD], proff[CLASS_GUARD][0], proff[CLASS_GUARD][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "���������     %s[%s%2d/%2d/%2d%s]%s       ",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_CHARMMAGE], proff[CLASS_CHARMMAGE][0],
			proff[CLASS_CHARMMAGE][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "����������  %s[%s%2d/%2d/%2d%s]%s\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM),
			ptot[CLASS_DEFENDERMAGE], proff[CLASS_DEFENDERMAGE][0], proff[CLASS_DEFENDERMAGE][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "������������� %s[%s%2d/%2d/%2d%s]%s       ",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_NECROMANCER], proff[CLASS_NECROMANCER][0],
			proff[CLASS_NECROMANCER][1], CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "������      %s[%s%2d/%2d/%2d%s]%s\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_PALADINE], proff[CLASS_PALADINE][0],
			proff[CLASS_PALADINE][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "��������      %s[%s%2d/%2d/%2d%s]%s       ",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_RANGER], proff[CLASS_RANGER][0], proff[CLASS_RANGER][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "�������     %s[%s%2d/%2d/%2d%s]%s\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_SMITH], proff[CLASS_SMITH][0], proff[CLASS_SMITH][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "�����         %s[%s%2d/%2d/%2d%s]%s       ",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_MERCHANT], proff[CLASS_MERCHANT][0],
			proff[CLASS_MERCHANT][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "������      %s[%s%2d/%2d/%2d%s]%s\r\n\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), ptot[CLASS_DRUID], proff[CLASS_DRUID][0], proff[CLASS_DRUID][1],
			CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf),
			"������� ����|���� 25 ������     %s[%s%*d%s|%s%*d%s]%s\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), 3, hilvl, CCIRED(ch,
					C_NRM),
			CCICYN(ch, C_NRM), 3, lowlvl, CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf),
			"������� � ����������������|���  %s[%s%*d%s|%s%*d%s]%s\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), 3, rem, CCIRED(ch, C_NRM),
			CCICYN(ch, C_NRM), 3, norem, CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf),
			"��������|����������� �������    %s[%s%*d%s|%s%*d%s]%s\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), 3, clan, CCIRED(ch,
					C_NRM),
			CCICYN(ch, C_NRM), 3, noclan, CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf),
			"������� � ������� ��|��� ��     %s[%s%*d%s|%s%*d%s]%s\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), 3, pk, CCIRED(ch,
					C_NRM),
			CCICYN(ch, C_NRM), 3, nopk, CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	sprintf(buf + strlen(buf), "����� ������� %s[%s%*d%s]%s\r\n\r\n",
			CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), 3, all, CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	send_to_char(buf, ch);

	char buf_[MAX_INPUT_LENGTH];
	std::string out;

	out += print_server_uptime();
	snprintf(buf_, sizeof(buf_),
		"������ (��� ��) | ������ �����  %s[%s%3d%s|%s %2d%s]%s\r\n",
		CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), char_stat::pkilled,
		CCIRED(ch, C_NRM), CCICYN(ch, C_NRM), char_stat::mkilled,
		CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
	out += buf_;
	out += char_stat::print_class_exp(ch);

	send_to_char(out, ch);
}


#define USERS_FORMAT \
"������: users [-l minlevel[-maxlevel]] [-n name] [-h host] [-c classlist] [-o] [-p]\r\n"
#define MAX_LIST_LEN 200
ACMD(do_users)
{
	const char *format = "%3d %-7s %-12s %-14s %-3s %-8s ";
	char line[200], line2[220], idletime[10], classname[20];
	char state[30] = "\0", *timeptr, mode;
	char name_search[MAX_INPUT_LENGTH] = "\0", host_search[MAX_INPUT_LENGTH];
// ����
	char host_by_name[MAX_INPUT_LENGTH] = "\0";
	DESCRIPTOR_DATA *list_players[MAX_LIST_LEN];
	DESCRIPTOR_DATA *d_tmp;
	int count_pl;
	int cycle_i, is, flag_change;
	unsigned long a1, a2;
	int showremorts = 0, showemail = 0, locating = 0;
	char sorting = '!';
	register CHAR_DATA *ci;
// ---
	CHAR_DATA *tch, *t, *t_tmp;
	DESCRIPTOR_DATA *d;
	int low = 0, high = LVL_IMPL, num_can_see = 0;
	int showclass = 0, outlaws = 0, playing = 0, deadweight = 0;

	host_search[0] = name_search[0] = '\0';

	strcpy(buf, argument);
	while (*buf)
	{
		half_chop(buf, arg, buf1);
		if (*arg == '-')
		{
			mode = *(arg + 1);	// just in case; we destroy arg in the switch
			switch (mode)
			{
			case 'o':
			case 'k':
				outlaws = 1;
				playing = 1;
				strcpy(buf, buf1);
				break;
			case 'p':
				playing = 1;
				strcpy(buf, buf1);
				break;
			case 'd':
				deadweight = 1;
				strcpy(buf, buf1);
				break;
			case 'l':
				if (!IS_GOD(ch))
					return;
				playing = 1;
				half_chop(buf1, arg, buf);
				sscanf(arg, "%d-%d", &low, &high);
				break;
			case 'n':
				playing = 1;
				half_chop(buf1, name_search, buf);
				break;
			case 'h':
				playing = 1;
				half_chop(buf1, host_search, buf);
				break;
			case 'u':
				playing = 1;
				half_chop(buf1, host_by_name, buf);
				break;
			case 'w':
				if (!IS_GRGOD(ch))
					return;
				playing = 1;
				locating = 1;
				strcpy(buf, buf1);
				break;
			case 'c':
			{
				playing = 1;
				half_chop(buf1, arg, buf);
				const size_t len = strlen(arg);
				for (size_t i = 0; i < len; i++)
				{
					showclass |= find_class_bitvector(arg[i]);
				}
				break;
			}
			case 'e':
				showemail = 1;
				strcpy(buf, buf1);
				break;
			case 'r':
				showremorts = 1;
				strcpy(buf, buf1);
				break;

			case 's':
				//sorting = 'i';
				sorting = *(arg + 2);
				strcpy(buf, buf1);
				break;
			default:
				send_to_char(USERS_FORMAT, ch);
				return;
			}	// end of switch

		}
		else  	// endif
		{
			strcpy(name_search, arg);
			strcpy(buf, buf1);
		}
	}			// end while (parser)
	if (showemail)
	{
		strcpy(line, "��� �������       ���         ���������       Idl �����    ����       E-mail\r\n");
	}
	else
	{
		strcpy(line, "��� �������       ���         ���������       Idl �����    ����\r\n");
	}
	strcat(line, "--- ---------- ------------ ----------------- --- -------- ----------------------------\r\n");
	send_to_char(line, ch);

	one_argument(argument, arg);

// ����
	if (strlen(host_by_name) != 0)
		strcpy(host_search, "!");
	for (d = descriptor_list, count_pl = 0; d && count_pl < MAX_LIST_LEN; d = d->next, count_pl++)
	{
		list_players[count_pl] = d;
		if (d->original)
			tch = d->original;
		else if (!(tch = d->character))
			continue;
		if (host_by_name != 0)
			if (isname(host_by_name, GET_NAME(tch)))
				strcpy(host_search, d->host);
	}
	if (sorting != '!')
	{
		is = 1;
		while (is)
		{
			is = 0;
			for (cycle_i = 1; cycle_i < count_pl; cycle_i++)
			{
				flag_change = 0;
				d = list_players[cycle_i - 1];
				if (d->original)
					t = d->original;
				else
					t = d->character;
				d_tmp = list_players[cycle_i];
				if (d_tmp->original)
					t_tmp = d_tmp->original;
				else
					t_tmp = d_tmp->character;
				switch (sorting)
				{
				case 'n':
					if (strcoll(t ? t->get_pc_name() : "", t_tmp ? t_tmp->get_pc_name() : "") > 0)
						flag_change = 1;
					break;
				case 'e':
					if (strcoll(t ? GET_EMAIL(t) : "", t_tmp ? GET_EMAIL(t_tmp) : "") > 0)
						flag_change = 1;
					break;
				default:
					a1 = get_ip((const char *) d->host);
					a2 = get_ip((const char *) d_tmp->host);
					if (a1 > a2)
						flag_change = 1;
				}
				if (flag_change)
				{
					list_players[cycle_i - 1] = d_tmp;
					list_players[cycle_i] = d;
					is = 1;
				}
			}
		}
	}

	for (cycle_i = 0; cycle_i < count_pl; cycle_i++)
	{
		d = (DESCRIPTOR_DATA *) list_players[cycle_i];
// ---
		if (STATE(d) != CON_PLAYING && playing)
			continue;
		if (STATE(d) == CON_PLAYING && deadweight)
			continue;
		if (STATE(d) == CON_PLAYING)
		{
			if (d->original)
				tch = d->original;
			else if (!(tch = d->character))
				continue;

			if (*host_search && !strstr(d->host, host_search))
				continue;
			if (*name_search && !isname(name_search, GET_NAME(tch)))
				continue;
			if (!CAN_SEE(ch, tch) || GET_LEVEL(tch) < low || GET_LEVEL(tch) > high)
				continue;
			if (outlaws && !PLR_FLAGGED((ch), PLR_KILLER))
				continue;
			if (showclass && !(showclass & (1 << GET_CLASS(tch))))
				continue;
			if (GET_INVIS_LEV(tch) > GET_LEVEL(ch))
				continue;

			if (d->original)
				if (showremorts)
					sprintf(classname, "[%2d %2d %s %s]", GET_LEVEL(d->original), GET_REMORT(d->original), KIN_ABBR(d->original), CLASS_ABBR(d->original));
				else
					sprintf(classname, "[%2d %s %s]   ", GET_LEVEL(d->original), KIN_ABBR(d->original), CLASS_ABBR(d->original));
			else
				if (showremorts)
					sprintf(classname, "[%2d %2d %s %s]", GET_LEVEL(d->character), GET_REMORT(d->character), KIN_ABBR(d->character), CLASS_ABBR(d->character));
				else
					sprintf(classname, "[%2d %s %s]   ", GET_LEVEL(d->character), KIN_ABBR(d->character), CLASS_ABBR(d->character));
		}
		else
			strcpy(classname, "      -      ");
		if (GET_LEVEL(ch) < LVL_IMPL && !PRF_FLAGGED(ch, PRF_CODERINFO))
			strcpy(classname, "      -      ");
		timeptr = asctime(localtime(&d->login_time));
		timeptr += 11;
		*(timeptr + 8) = '\0';

		if (STATE(d) == CON_PLAYING && d->original)
			strcpy(state, "Switched");
		else
			sprinttype(STATE(d), connected_types, state);

		if (d->character && STATE(d) == CON_PLAYING && !IS_GOD(d->character))
			sprintf(idletime, "%3d", d->character->char_specials.timer *
					SECS_PER_MUD_HOUR / SECS_PER_REAL_MIN);
		else
			strcpy(idletime, "");

		if (d->character && d->character->get_pc_name())
		{
			if (d->original)
				sprintf(line, format, d->desc_num, classname,
						d->original->get_pc_name(), state, idletime, timeptr);
			else
				sprintf(line, format, d->desc_num, classname,
						d->character->get_pc_name(), state, idletime, timeptr);
		}
		else
			sprintf(line, format, d->desc_num, "   -   ", "UNDEFINED", state, idletime, timeptr);
// ����
		if (d->host && *d->host)
		{
			sprintf(line2, "[%s] [%s]", d->host, nslookup(d->host)); // prool
			strcat(line, line2);
		}
		else
			strcat(line, "[����������� ����]");

		if (showemail)
		{
			sprintf(line2, "[&S%s&s]",
					d->original ? GET_EMAIL(d->original) : d->character ? GET_EMAIL(d->character) : "");
			strcat(line, line2);
		}

		if (locating && (*name_search || *host_by_name))
			if ((STATE(d) == CON_PLAYING))
			{
				ci = (d->original ? d->original : d->character);
				if (ci && CAN_SEE(ch, ci) && (ci->in_room != NOWHERE))
				{
					if (d->original && d->character)
						sprintf(line2, " [%5d] %s (in %s)",
								GET_ROOM_VNUM(IN_ROOM(d->character)),
								world[d->character->in_room]->name, GET_NAME(d->character));
					else
						sprintf(line2, " [%5d] %s",
								GET_ROOM_VNUM(IN_ROOM(ci)), world[ci->in_room]->name);
				}
				strcat(line, line2);
			}
//--
		strcat(line, "\r\n");
		if (STATE(d) != CON_PLAYING)
		{
			sprintf(line2, "%s%s%s", CCGRN(ch, C_SPR), line, CCNRM(ch, C_SPR));
			strcpy(line, line2);
		}
		if (STATE(d) != CON_PLAYING || (STATE(d) == CON_PLAYING && d->character && CAN_SEE(ch, d->character)))
		{
			send_to_char(line, ch);
			num_can_see++;
		}
	}

	sprintf(line, "\r\n%d ������� ����������.\r\n", num_can_see);
//  send_to_char(line, ch);
	page_string(ch->desc, line, TRUE);
}

// Generic page_string function for displaying text
ACMD(do_gen_ps)
{
	//DESCRIPTOR_DATA *d;
	switch (subcmd)
	{
	case SCMD_CREDITS:
		page_string(ch->desc, credits, 0);
		break;
	case SCMD_INFO:
		page_string(ch->desc, info, 0);
		break;
	case SCMD_IMMLIST:
		page_string(ch->desc, immlist, 0);
		break;
	case SCMD_HANDBOOK:
		page_string(ch->desc, handbook, 0);
		break;
	case SCMD_POLICIES:
		page_string(ch->desc, policies, 0);
		break;
	case SCMD_MOTD:
		page_string(ch->desc, motd, 0);
		break;
	case SCMD_RULES:
		page_string(ch->desc, rules, 0);
		break;
	case SCMD_CLEAR:
		send_to_char("\033[H\033[J", ch);
		break;
	case SCMD_VERSION:
		show_code_date(ch);
		break;
	case SCMD_WHOAMI:
	{
		//���������. ������.
		sprintf(buf, "�������� : %s\r\n", GET_NAME(ch));
		sprintf(buf + strlen(buf),
				"������ : &W%s&n/&W%s&n/&W%s&n/&W%s&n/&W%s&n/&W%s&n\r\n",
				GET_PAD(ch, 0), GET_PAD(ch, 1), GET_PAD(ch, 2),
				GET_PAD(ch, 3), GET_PAD(ch, 4), GET_PAD(ch, 5));

		sprintf(buf + strlen(buf), "��� e-mail : &S%s&s\r\n", GET_EMAIL(ch));
		time_t birt = ch->player_data.time.birth;
		sprintf(buf + strlen(buf), "���� ������ �������� : %s\r\n", rustime(localtime(&birt)));
		sprintf(buf + strlen(buf), "��� IP-����� : %s\r\n", ch->desc ? ch->desc->host : "Unknown");
//               GET_LASTIP (ch));
		send_to_char(buf, ch);
		if (!NAME_GOD(ch))
		{
			sprintf(buf, "��� ����� �� ��������!\r\n");
			send_to_char(buf, ch);
		}
		else
		{
			int god_level = NAME_GOD(ch) > 1000 ? NAME_GOD(ch) - 1000 : NAME_GOD(ch);
			sprintf(buf1, "%s", get_name_by_id(NAME_ID_GOD(ch)));
			*buf1 = UPPER(*buf1);
			if (NAME_GOD(ch) < 1000)
				sprintf(buf, "&R��� ��������� %s %s&n\r\n", print_god_or_player(god_level), buf1);
			else
				sprintf(buf, "&W��� �������� %s %s&n\r\n", print_god_or_player(god_level), buf1);
			send_to_char(buf, ch);
		}
		sprintf(buf, "��������������: %d\r\n", GET_REMORT(ch));
		send_to_char(buf, ch);
		//����� ���������. ������.
		Clan::CheckPkList(ch);
		break;
	}
	default:
		log("SYSERR: Unhandled case in do_gen_ps. (%d)", subcmd);
		return;
	}
}


void perform_mortal_where(CHAR_DATA * ch, char *arg)
{
	register CHAR_DATA *i;
	register DESCRIPTOR_DATA *d;

	send_to_char("��� ����� �����, ��� ����� ����.\r\n", ch);
	return;

	if (!*arg)
	{
		send_to_char("������, ����������� � ����\r\n--------------------\r\n", ch);
		for (d = descriptor_list; d; d = d->next)
		{
			if (STATE(d) != CON_PLAYING || d->character == ch)
				continue;
			if ((i = (d->original ? d->original : d->character)) == NULL)
				continue;
			if (i->in_room == NOWHERE || !CAN_SEE(ch, i))
				continue;
			if (world[ch->in_room]->zone != world[i->in_room]->zone)
				continue;
			sprintf(buf, "%-20s - %s\r\n", GET_NAME(i), world[i->in_room]->name);
			send_to_char(buf, ch);
		}
	}
	else  		// print only FIRST char, not all.
	{
		for (i = character_list; i; i = i->next)
		{
			if (i->in_room == NOWHERE || i == ch)
				continue;
			if (!CAN_SEE(ch, i)
					|| world[i->in_room]->zone != world[ch->in_room]->zone)
				continue;
			if (!isname(arg, i->get_pc_name()))
				continue;
			sprintf(buf, "%-25s - %s\r\n", GET_NAME(i), world[i->in_room]->name);
			send_to_char(buf, ch);
			return;
		}
		send_to_char("������ �������� � ���� ������ ���.\r\n", ch);
	}
}


void print_object_location(int num, OBJ_DATA * obj, CHAR_DATA * ch, int recur)
{
	if (num > 0)
		sprintf(buf, "O%3d. %-25s - ", num, obj->short_description);
	else
		sprintf(buf, "%34s", " - ");

	if (obj->in_room > NOWHERE)
	{
		sprintf(buf + strlen(buf), "[%5d] %s\r\n", GET_ROOM_VNUM(IN_ROOM(obj)), world[obj->in_room]->name);
		send_to_char(buf, ch);
	}
	else if (obj->carried_by)
	{
		sprintf(buf + strlen(buf), "�������� %s[%d] � ������� [%d]\r\n", PERS(obj->carried_by, ch, 4), GET_MOB_VNUM(obj->carried_by), world[obj->carried_by->in_room]->number);
		send_to_char(buf, ch);
	}
	else if (obj->worn_by)
	{
		sprintf(buf + strlen(buf), "����� �� %s[%d] � ������� [%d]\r\n", PERS(obj->worn_by, ch, 3), GET_MOB_VNUM(obj->worn_by), world[obj->worn_by->in_room]->number);
		send_to_char(buf, ch);
	}
	else if (obj->in_obj)
	{
		sprintf(buf + strlen(buf), "����� � %s%s\r\n",
				GET_OBJ_PNAME(obj->in_obj, 5), (recur ? ", ������� ��������� " : " "));
		send_to_char(buf, ch);
		if (recur)
			print_object_location(0, obj->in_obj, ch, recur);
	}
	else
	{
		sprintf(buf + strlen(buf), "��������� ���-�� ���, ������-������.\r\n");
		send_to_char(buf, ch);
	}
}

/**
* ������� ����� ������ �� '���' � �������� ��� �� ����������� ������, ���
* � �� ������� �������� � �����.
*/
bool print_imm_where_obj(CHAR_DATA *ch, char *arg, int num)
{
	bool found = false;
	//int num = 1;
	for (OBJ_DATA *k = object_list; k; k = k->next)
	{
		if (isname(arg, k->aliases))
		{
			found = true;
			print_object_location(num++, k, ch, TRUE);
		}
	}

	int tmp_num = num;
	if (IS_IMPL(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
	{
		tmp_num = Depot::print_imm_where_obj(ch, arg, tmp_num);
		tmp_num = Parcel::print_imm_where_obj(ch, arg, tmp_num);
	}

	if (!found && tmp_num == num)
		return false;
	else
	{
		num = tmp_num;
		return true;
	}
}

void perform_immort_where(CHAR_DATA * ch, char *arg)
{
	register CHAR_DATA *i;
	DESCRIPTOR_DATA *d;
	int num = 1, found = 0;

	if (!*arg)
	{
		if (GET_LEVEL(ch) < LVL_IMPL && !PRF_FLAGGED(ch, PRF_CODERINFO))
			send_to_char("��� ��� ���������?", ch);
		else
		{
			send_to_char("������\r\n------\r\n", ch);
			for (d = descriptor_list; d; d = d->next)
				if (STATE(d) == CON_PLAYING)
				{
					i = (d->original ? d->original : d->character);
					if (i && CAN_SEE(ch, i) && (i->in_room != NOWHERE))
					{
						if (d->original)
							sprintf(buf, "%-20s - [%5d] %s (in %s)\r\n",
									GET_NAME(i),
									GET_ROOM_VNUM(IN_ROOM(d->character)),
									world[d->character->in_room]->name,
									GET_NAME(d->character));
						else
							sprintf(buf, "%-20s - [%5d] %s\r\n", GET_NAME(i),
									GET_ROOM_VNUM(IN_ROOM(i)), world[i->in_room]->name);
						send_to_char(buf, ch);
					}
				}
		}
	}
	else
	{
		for (i = character_list; i; i = i->next)
		{
			if (CAN_SEE(ch, i) && i->in_room != NOWHERE && isname(arg, i->get_pc_name()))
			{
				found = 1;
				sprintf(buf, "M%3d. %-25s - [%5d] %s\r\n", num++, GET_NAME(i),
						GET_ROOM_VNUM(IN_ROOM(i)), world[IN_ROOM(i)]->name);
				send_to_char(buf, ch);
			}
		}
		if (!print_imm_where_obj(ch, arg, num) && !found)
			send_to_char("��� ������ ��������.\r\n", ch);
	}
}



ACMD(do_where)
{
	one_argument(argument, arg);

	if (IS_GRGOD(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
		perform_immort_where(ch, arg);
	else
		perform_mortal_where(ch, arg);
}



ACMD(do_levels)
{
	int i;

	if (IS_NPC(ch))
	{
		send_to_char("���� ��� ��������� ��� �������.\r\n", ch);
		return;
	}
	*buf = '\0';

	sprintf(buf, "�������          ����            ���� �� ���.\r\n");
	for (i = 1; i < LVL_IMMORT; i++)
		sprintf(buf + strlen(buf), "[%2d] %13s-%-13s %-13s\r\n", i, thousands_sep(level_exp(ch, i)).c_str(),
			thousands_sep(level_exp(ch, i + 1) - 1).c_str(),
			thousands_sep((int) (level_exp(ch, i + 1) - level_exp(ch, i)) / (10 + GET_REMORT(ch))).c_str());

	sprintf(buf + strlen(buf), "[%2d] %13s               (����������)\r\n", LVL_IMMORT, thousands_sep(level_exp(ch, LVL_IMMORT)).c_str());
	page_string(ch->desc, buf, 1);
}



ACMD(do_consider)
{
	CHAR_DATA *victim;
	int diff;

	one_argument(argument, buf);

	if (!(victim = get_char_vis(ch, buf, FIND_CHAR_ROOM)))
	{
		send_to_char("���� �� ������ �������?\r\n", ch);
		return;
	}
	if (victim == ch)
	{
		send_to_char("�����! �������� �������� <������� ��������>!\r\n", ch);
		return;
	}
	if (!IS_NPC(victim))
	{
		send_to_char("���������� ������� ���� - ��� � �� ��������.\r\n", ch);
		return;
	}
	diff = (GET_LEVEL(victim) - GET_LEVEL(ch) - GET_REMORT(ch));

	if (diff <= -10)
		send_to_char("���-����, ��� ��������.\r\n", ch);
	else if (diff <= -5)
		send_to_char("\"������� ��� ���� � ����!\"\r\n", ch);
	else if (diff <= -2)
		send_to_char("�����.\r\n", ch);
	else if (diff <= -1)
		send_to_char("������������ �����.\r\n", ch);
	else if (diff == 0)
		send_to_char("������ ��������!\r\n", ch);
	else if (diff <= 1)
		send_to_char("��� ����������� ������� �����!\r\n", ch);
	else if (diff <= 2)
		send_to_char("��� ����������� �������!\r\n", ch);
	else if (diff <= 3)
		send_to_char("����� � ������� ���������� ��� ������ ����������!\r\n", ch);
	else if (diff <= 5)
		send_to_char("�� ������ �� ���� ������� �����.\r\n", ch);
	else if (diff <= 10)
		send_to_char("�����, ������� ��� ���.\r\n", ch);
	else if (diff <= 100)
		send_to_char("������ � ��������� - �� ��������� ������ �������!\r\n", ch);

}



ACMD(do_diagnose)
{
	CHAR_DATA *vict;

	one_argument(argument, buf);

	if (*buf)
	{
		if (!(vict = get_char_vis(ch, buf, FIND_CHAR_ROOM)))
			send_to_char(NOPERSON, ch);
		else
			diag_char_to_char(vict, ch);
	}
	else
	{
		if (ch->get_fighting())
			diag_char_to_char(ch->get_fighting(), ch);
		else
			send_to_char("�� ���� �� ������ ���������?\r\n", ch);
	}
}


const char *ctypes[] = { "��������", "�������", "�������", "������", "\n"
					   };

ACMD(do_color)
{
	int tp;

	if (IS_NPC(ch))
		return;

	one_argument(argument, arg);

	if (!*arg)
	{
		sprintf(buf, "%s %s��������%s �����.\r\n", ctypes[COLOR_LEV(ch)], CCRED(ch, C_SPR), CCNRM(ch, C_OFF));
		send_to_char(CAP(buf), ch);
		return;
	}
	if (((tp = search_block(arg, ctypes, FALSE)) == -1))
	{
		send_to_char("������: [�����] ���� { ���� | ������� | ������� | ������ }\r\n", ch);
		return;
	}
	REMOVE_BIT(PRF_FLAGS(ch, PRF_COLOR_1), PRF_COLOR_1);
	REMOVE_BIT(PRF_FLAGS(ch, PRF_COLOR_2), PRF_COLOR_2);

	SET_BIT(PRF_FLAGS(ch, PRF_COLOR_1), (PRF_COLOR_1 * (tp & 1)));
	SET_BIT(PRF_FLAGS(ch, PRF_COLOR_1), (PRF_COLOR_2 * (tp & 2) >> 1));

	sprintf(buf, "%s %s��������%s �����.\r\n", ctypes[tp], CCRED(ch, C_SPR), CCNRM(ch, C_OFF));
	send_to_char(CAP(buf), ch);
}


ACMD(do_toggle)
{
	if (IS_NPC(ch))
		return;
	if (GET_WIMP_LEV(ch) == 0)
		strcpy(buf2, "���");
	else
		sprintf(buf2, "%-3d", GET_WIMP_LEV(ch));

	if (GET_LEVEL(ch) >= LVL_IMMORT || PRF_FLAGGED(ch, PRF_CODERINFO))
	{
		sprintf(buf,
				" ��� �����     : %-3s     "
				" ������������  : %-3s     "
				" ����� ������  : %-3s \r\n"
				" ������� ����� : %-3s     "
				" ����������    : %-3s     "
				" �����         : %-3s \r\n"
				" ��������      : %-3s \r\n",
				ONOFF(PRF_FLAGGED(ch, PRF_NOHASSLE)),
				ONOFF(PRF_FLAGGED(ch, PRF_HOLYLIGHT)),
				ONOFF(PRF_FLAGGED(ch, PRF_ROOMFLAGS)),
				ONOFF(PRF_FLAGGED(ch, PRF_NOWIZ)),
				ONOFF(nameserver_is_slow),
				ONOFF(PRF_FLAGGED(ch, PRF_CODERINFO)),
				ONOFF(PRF_FLAGGED(ch, PRF_MISPRINT)));
		send_to_char(buf, ch);
	}

	sprintf(buf,
			" ����������    : %-3s     "
			" ������� ����� : %-3s     "
			" ������ �����  : %-3s \r\n"
			" ������ ������ : %-3s     "
			" ���������     : %-3s     "
			" ����          : %-8s\r\n"
			" ���-��        : %-6s  "
			" �������       : %-3s     "
			" �����         : %-3s \r\n"
			" �������       : %-3s     "
			" �����         : %-3s     "
			" ��������������: %-3s \r\n"
			" ������        : %-3s     "
			" ��������������: %-3s     "
			" ������ (���)  : %-7s \r\n"
			" ��� ��������� : %-3s     "
			" ���������     : %-3s     "
			" ����������    : %-7s \r\n"
			" ����� ����    : %-3s     "
			" �����         : %-3s     "
			" ��������      : %-3s \r\n"
			" ������ ������ : %-3d     "
			" ������ ������ : %-3d     "
			" ������        : %-6s \r\n"
			" ������� (���) : %-5s   "
			" �����         : %-3s     "
			" ���������     : %-10s\r\n"
			" ������        : %-3s     "
			" ��������      : %-3s     "
			" ��������      : %-10s\r\n"
			" ����������    : %-3s     "
			" ������        : %-3s     "
			" ������ �����  : %-3s \r\n"
			" �����������   : %-3s     "
			" ���������     : %-3u     ",
			ONOFF(PRF_FLAGGED(ch, PRF_AUTOEXIT)),
			ONOFF(PRF_FLAGGED(ch, PRF_BRIEF)),
			ONOFF(PRF_FLAGGED(ch, PRF_COMPACT)),
			YESNO(!PRF_FLAGGED(ch, PRF_NOREPEAT)),
			ONOFF(!PRF_FLAGGED(ch, PRF_NOTELL)),
			ctypes[COLOR_LEV(ch)],
			PRF_FLAGGED(ch, PRF_NOINVISTELL) ? "������" : "�����",
			ONOFF(!PRF_FLAGGED(ch, PRF_NOGOSS)),
			ONOFF(!PRF_FLAGGED(ch, PRF_NOHOLLER)),
			ONOFF(!PRF_FLAGGED(ch, PRF_NOAUCT)),
			ONOFF(!PRF_FLAGGED(ch, PRF_NOEXCHANGE)),
			ONOFF(PRF_FLAGGED(ch, PRF_AUTOMEM)),
			ONOFF(PRF_FLAGGED(ch, PRF_SUMMONABLE)),
			ONOFF(PRF_FLAGGED(ch, PRF_GOAHEAD)),
			PRF_FLAGGED(ch, PRF_SHOWGROUP) ? "������" : "�������",
			ONOFF(PRF_FLAGGED(ch, PRF_NOCLONES)),
			ONOFF(PRF_FLAGGED(ch, PRF_AUTOSPLIT)),
			PRF_FLAGGED(ch, PRF_AUTOLOOT) ? PRF_FLAGGED(ch, PRF_NOINGR_LOOT) ? "NO-INGR" : "ALL    " : "OFF    ",
			ONOFF(PRF_FLAGGED(ch, PRF_AUTOMONEY)),
			ONOFF(!PRF_FLAGGED(ch, PRF_NOARENA)),
			buf2,
			STRING_LENGTH(ch),
			STRING_WIDTH(ch),
#if defined(HAVE_ZLIB)
			ch->desc->deflate == NULL ? "���" : (ch->desc->mccp_version == 2 ? "MCCPv2" : "MCCPv1"),
#else
			"N/A",
#endif
			PRF_FLAGGED(ch, PRF_NEWS_MODE) ? "�����" : "�����",
			ONOFF(PRF_FLAGGED(ch, PRF_BOARD_MODE)),
			GetChestMode(ch).c_str(),
			ONOFF(PRF_FLAGGED(ch, PRF_PKL_MODE)),
			ONOFF(PRF_FLAGGED(ch, PRF_POLIT_MODE)),
			PRF_FLAGGED(ch, PRF_PKFORMAT_MODE) ? "�������" : "������",
			ONOFF(PRF_FLAGGED(ch, PRF_WORKMATE_MODE)),
			ONOFF(PRF_FLAGGED(ch, PRF_OFFTOP_MODE)),
			ONOFF(PRF_FLAGGED(ch, PRF_ANTIDC_MODE)),
			ONOFF(PRF_FLAGGED(ch, PRF_NOINGR_MODE)),
			ch->remember_get_num());
	send_to_char(buf, ch);
	if (NOTIFY_EXCH_PRICE(ch) > 0)
	{
		sprintf(buf,  " �����������   : %-3ld \r\n", NOTIFY_EXCH_PRICE(ch));
	}
	else
	{
		sprintf(buf,  " �����������   : %-3s \r\n", "���");
	}
	send_to_char(buf, ch);

	sprintf(buf,
		" �����         : %-3s     "
		" ���� � ����   : %-3s     "
		" ������� (���) : %s\r\n"
		" ����������    : %-3s\r\n",
		ONOFF(PRF_FLAGGED(ch, PRF_DRAW_MAP)),
		ONOFF(PRF_FLAGGED(ch, PRF_ENTER_ZONE)),
		(PRF_FLAGGED(ch, PRF_BRIEF_SHIELDS) ? "�������" : "������"),
		ONOFF(PRF_FLAGGED(ch, PRF_AUTO_NOSUMMON)));
	send_to_char(buf, ch);
}

ACMD(do_zone)
{
	if (ch->desc
		&& !(IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch) && !can_use_feat(ch, DARK_READING_FEAT))
		&& !AFF_FLAGGED(ch, AFF_BLIND))
	{
		MapSystem::print_map(ch);
	}

	print_zone_info(ch);

	if ((IS_IMMORTAL(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
		&& zone_table[world[ch->in_room]->zone].comment)
	{
		send_to_char(ch, "�����������: %s.\r\n",
			zone_table[world[ch->in_room]->zone].comment);
	}
}


struct sort_struct
{
	int sort_pos;
	byte is_social;
} *cmd_sort_info = NULL;

int num_of_cmds;


void sort_commands(void)
{
	int a, b, tmp;

	num_of_cmds = 0;

	 // first, count commands (num_of_commands is actually one greater than the
	 // number of commands; it inclues the '\n'.

	while (*cmd_info[num_of_cmds].command != '\n')
		num_of_cmds++;

	// create data array
	CREATE(cmd_sort_info, struct sort_struct, num_of_cmds);

	// initialize it
	for (a = 1; a < num_of_cmds; a++)
	{
		cmd_sort_info[a].sort_pos = a;
		cmd_sort_info[a].is_social = FALSE;
	}

	// the infernal special case
	cmd_sort_info[find_command("insult")].is_social = TRUE;

	// Sort.  'a' starts at 1, not 0, to remove 'RESERVED'
	for (a = 1; a < num_of_cmds - 1; a++)
		for (b = a + 1; b < num_of_cmds; b++)
			if (strcmp(cmd_info[cmd_sort_info[a].sort_pos].command,
					   cmd_info[cmd_sort_info[b].sort_pos].command) > 0)
			{
				tmp = cmd_sort_info[a].sort_pos;
				cmd_sort_info[a].sort_pos = cmd_sort_info[b].sort_pos;
				cmd_sort_info[b].sort_pos = tmp;
			}
}



ACMD(do_commands)
{
	int no, i, cmd_num, num_of;
	int wizhelp = 0, socials = 0;
	CHAR_DATA *vict = ch;

	one_argument(argument, arg);

	if (subcmd == SCMD_SOCIALS)
		socials = 1;
	else if (subcmd == SCMD_WIZHELP)
		wizhelp = 1;

	sprintf(buf, "��������� %s%s �������� %s:\r\n",
			wizhelp ? "����������������� " : "",
			socials ? "�������" : "�������", vict == ch ? "���" : GET_PAD(vict, 2));

	if (socials)
		num_of = top_of_socialk + 1;
	else
		num_of = num_of_cmds - 1;

	// cmd_num starts at 1, not 0, to remove 'RESERVED'
	for (no = 1, cmd_num = socials ? 0 : 1; cmd_num < num_of; cmd_num++)
		if (socials)
		{
			sprintf(buf + strlen(buf), "%-19s", soc_keys_list[cmd_num].keyword);
			if (!(no % 4))
				strcat(buf, "\r\n");
			no++;
		}
		else
		{
			i = cmd_sort_info[cmd_num].sort_pos;
			if (cmd_info[i].minimum_level >= 0
					&& (Privilege::can_do_priv(vict, std::string(cmd_info[i].command), i, 0))
					&& (cmd_info[i].minimum_level >= LVL_IMMORT) == wizhelp
					&& (wizhelp || socials == cmd_sort_info[i].is_social))
			{
				sprintf(buf + strlen(buf), "%-15s", cmd_info[i].command);
				if (!(no % 5))
					strcat(buf, "\r\n");
				no++;
			}
		}

	strcat(buf, "\r\n");
	send_to_char(buf, ch);
}

int hiding[] = { AFF_SNEAK,
				 AFF_HIDE,
				 AFF_CAMOUFLAGE,
				 0
			   };

ACMD(do_affects)
{
	AFFECT_DATA *aff;
	FLAG_DATA saved;
	int i, j;
	char sp_name[MAX_STRING_LENGTH];

	// Showing the bitvector
	saved = ch->char_specials.saved.affected_by;
	for (i = 0; (j = hiding[i]); i++)
	{
		if (IS_SET(GET_FLAG(saved, j), j))
			SET_BIT(GET_FLAG(saved, j), j);
		REMOVE_BIT(AFF_FLAGS(ch, j), j);
	}
	sprintbits(ch->char_specials.saved.affected_by, affected_bits, buf2, ",");
	sprintf(buf, "�������: %s%s%s\r\n", CCIYEL(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
	send_to_char(buf, ch);
	for (i = 0; (j = hiding[i]); i++)
		if (IS_SET(GET_FLAG(saved, j), j))
			SET_BIT(AFF_FLAGS(ch, j), j);

	// Routine to show what spells a char is affected by
	if (ch->affected)
	{
		for (aff = ch->affected; aff; aff = aff->next)
		{
			*buf2 = '\0';
			strcpy(sp_name, spell_name(aff->type));
			(aff->duration + 1) / SECS_PER_MUD_HOUR ? sprintf(buf2, "%d %s", (aff->duration + 1) / SECS_PER_MUD_HOUR + 1, desc_count((aff->duration + 1) / SECS_PER_MUD_HOUR + 1, WHAT_HOUR)) : sprintf(buf2, "����� ����");

			sprintf(buf, "%s%s%-21s (%s)%s",
					*sp_name == '!' ? "���������  : " : "���������� : ",
					CCICYN(ch, C_NRM), sp_name, buf2, CCNRM(ch, C_NRM));
			*buf2 = '\0';
			if (!IS_IMMORTAL(ch))
			{
				if (aff->next && aff->type == aff->next->type)
					continue;
			}
			else
			{
				if (aff->modifier)
				{
					sprintf(buf2, "%+d � %s", aff->modifier, apply_types[(int) aff->location]);
					strcat(buf, buf2);
				}
				if (aff->bitvector)
				{
					if (*buf2)
						strcat(buf, ", ������������� ");
					else
						strcat(buf, "������������� ");
					strcat(buf, CCIRED(ch, C_NRM));
					sprintbit(aff->bitvector, affected_bits, buf2);
					strcat(buf, buf2);
					strcat(buf, CCNRM(ch, C_NRM));
				}
			}
			send_to_char(strcat(buf, "\r\n"), ch);
		}
	}
	if (ch->is_morphed())
	{
		*buf2 = '\0';
		send_to_char("����������� �������� �����: " , ch);
		std::vector<long> affs = ch->GetMorphAffects();
		for (std::vector<long>::const_iterator it = affs.begin();it!=affs.end();)
		{
			sprintbit(*it, affected_bits, buf2);
			send_to_char(string(CCIYEL(ch, C_NRM))+ string(buf2)+ string(CCNRM(ch, C_NRM)), ch);
			if (++it != affs.end())
				send_to_char(", ", ch);
		}
	}
}

// Create web-page with users list
void make_who2html(void)
{
	FILE *opf;

	DESCRIPTOR_DATA *d;
	CHAR_DATA *ch;
	CHAR_DATA *vict;
	OBJ_DATA *obj;

	int imms_num = 0, morts_num = 0, j, k;

	char *imms = NULL;
	char *morts = NULL;
	char *buffer = NULL;
	time_t mytime;
	float day;

	char utf_buf [PROOL_MAX_STRLEN];

	if (webstat==0) return;

	if ((opf = fopen(wholist_file, "w")) == 0)
		return;		// or log it ? *shrug*

	fprintf(opf, "<HTML><HEAD><TITLE>Who in Virtustan MUD</TITLE>\n\
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=koi8-r\">\n\
</HEAD>\n");
	if (mudname[0])
	    fprintf(opf, "<BODY>%s <b>Who in %s</b><HR>\n",ptime(), mudname);
	else
	    fprintf(opf, "<BODY>%s <b>Who in Virtustan MUD</b><HR>\n",ptime());

	sprintf(buf, "Gods <BR> \r\n");
	imms = str_add(imms, buf);

	sprintf(buf, "<BR>Players<BR> \r\n  ");
	morts = str_add(morts, buf);

	for (d = descriptor_list; d; d = d->next)
		if (STATE(d) == CON_PLAYING && GET_INVIS_LEV(d->character) < 31)
		{
			ch = d->character;
			sprintf(buf, "%s <BR> \r\n ", ch->race_or_title().c_str());

			if (IS_IMMORTAL(ch))
			{
				imms_num++;
				imms = str_add(imms, buf);
			}
			else
			{
				morts_num++;
				morts = str_add(morts, buf);
			}
		}

	if (0/*morts_num + imms_num == 0*/) // prool
	{
		sprintf(buf, "No one <BR>");
		buffer = str_add(buffer, buf);
	}
	else
	{
		if (imms_num > 0)
			buffer = str_add(buffer, imms);
		if (morts_num > 0)
			buffer = str_add(buffer, morts);

		buffer = str_add(buffer, " <BR> \r\n Total:");

		if (1/*imms_num*/) // prool
		{
			// sprintf(buf+strlen(buf)," ����������� %d",imms_num);
			sprintf(buf, " immortals %d", imms_num);
			buffer = str_add(buffer, buf);
		}
		if (1/*morts_num*/) // prool
		{
			// sprintf(buf+strlen(buf)," �������� %d",morts_num);
			sprintf(buf, " mortals %d", morts_num);
			buffer = str_add(buffer, buf);
		}

		buffer = str_add(buffer, "\n");
	}

	sprintf(buf, "<hr> Total online %d", total_players=imms_num+morts_num); // prool
	buffer = str_add(buffer, buf);

	fprintf(opf, buffer);

	free(buffer);
	free(imms);
	free(morts);
	fprintf(opf, " <HR>\n");

		// mob/obj statistics
		
		j = 0;
		k = 0;
		for (vict = character_list; vict; vict = vict->next)
		{
			if (IS_NPC(vict))
				j++;
		}

		for (obj = object_list; obj; obj = obj->next)
			k++;

	fprintf(opf, "Total mobs %i\n<BR>\n", j);
	fprintf(opf, "Total objects %i\n<BR>\n", k);
	fprintf(opf, "Total registered players %i\n<BR>\n", top_of_p_table + 1);
		mytime = time(0) - boot_time;
		day=mytime;
		day/=86400;
	fprintf(opf, "MUD server uptime %.2f days\n<BR>\n", day);
	fprintf(opf, " </BODY></HTML>\n");
	fclose(opf);
}
