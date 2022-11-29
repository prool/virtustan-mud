/* ************************************************************************
*   File: act.other.cpp                                 Part of Bylins    *
*  Usage: Miscellaneous player-level commands                             *
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

#define __ACT_OTHER_C__

#include "conf.h"
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <sys/stat.h>

#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "skills.h"
#include "screen.h"
#include "house.h"
#include "constants.h"
#include "dg_scripts.h"
#include "pk.h"
#include "fight.h"
#include "magic.h"
#include "features.hpp"
#include "depot.hpp"
#include "privilege.hpp"
#include "random.hpp"
#include "char.hpp"
#include "char_player.hpp"
#include "remember.hpp"
#include "room.hpp"
#include "objsave.h"
#include "shop_ext.hpp"
#include "noob.hpp"

#include "virtustan.h" // prool

using std::ifstream;
using std::fstream;
using std::map;
using std::iterator;

extern class insert_wanted_gem iwg;

// extern variables
extern DESCRIPTOR_DATA *descriptor_list;
extern INDEX_DATA *mob_index;
extern char const *class_abbrevs[];
extern int free_rent;
extern int max_filesize;
extern int nameserver_is_slow;
extern int auto_save;
extern struct skillvariables_dig dig_vars;
extern struct skillvariables_insgem insgem_vars;

// extern procedures
void list_feats(CHAR_DATA * ch, CHAR_DATA * vict, bool all_feats);
void list_skills(CHAR_DATA * ch, CHAR_DATA * vict);
void list_spells(CHAR_DATA * ch, CHAR_DATA * vict, int all_spells);
void appear(CHAR_DATA * ch);
void write_aliases(CHAR_DATA * ch);
void perform_immort_vis(CHAR_DATA * ch);
int have_mind(CHAR_DATA * ch);
ACMD(do_gen_comm);
int Crash_delete_file(char *name, int mask);
int HaveMind(CHAR_DATA * ch);
extern char *color_value(CHAR_DATA * ch, int real, int max);
int posi_value(int real, int max);
int invalid_no_class(CHAR_DATA * ch, const OBJ_DATA * obj);
extern void split_or_clan_tax(CHAR_DATA *ch, long amount);

// local functions
ACMD(do_antigods);
ACMD(do_quit);
ACMD(do_save);
ACMD(do_not_here);
ACMD(do_sneak);
ACMD(do_hide);
ACMD(do_steal);
ACMD(do_spells);
ACMD(do_features);
ACMD(do_skills);
ACMD(do_visible);
int perform_group(CHAR_DATA * ch, CHAR_DATA * vict);
void print_group(CHAR_DATA * ch);
ACMD(do_group);
ACMD(do_ungroup);
ACMD(do_report);
ACMD(do_split);
ACMD(do_use);
ACMD(do_wimpy);
ACMD(do_display);
ACMD(do_gen_tog);
ACMD(do_courage);
ACMD(do_toggle);
ACMD(do_color);
ACMD(do_recall);
ACMD(do_dig);

ACMD(do_antigods)
{
	if (IS_IMMORTAL(ch))
	{
		send_to_char("��� ��� ����?\r\n", ch);
		return;
	}
	if (AFF_FLAGGED(ch, AFF_SHIELD))
	{
		if (affected_by_spell(ch, SPELL_SHIELD))
			affect_from_char(ch, SPELL_SHIELD);
		REMOVE_BIT(AFF_FLAGS(ch, AFF_SHIELD), AFF_SHIELD);
		send_to_char("������� ����� ������ ������ ���� ����.\r\n", ch);
		act("&W$n �������$g ������, ���������� ������.&n", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
	}
	else
		send_to_char("�� � ��� �� ��� ������� �����.\r\n", ch);
}

ACMD(do_quit)
{
	DESCRIPTOR_DATA *d, *next_d;

	if (IS_NPC(ch) || !ch->desc)
		return;

	if (subcmd != SCMD_QUIT)
		send_to_char("��� ����� ������� ��� ������� ��������� �� ��������� �������������!\r\n", ch);
	else if (GET_POS(ch) == POS_FIGHTING)
		send_to_char("���! ���-�-�! ��, ��������, ��������!\r\n", ch);
	else if (GET_POS(ch) < POS_STUNNED)
	{
		send_to_char("��� ���������� � ���� ��������� ����...\r\n", ch);
		die(ch, NULL);
	}
	else if (AFF_FLAGGED(ch, AFF_SLEEP))
	{
		return;
	}
	else if (*argument)
	{
		send_to_char("���� �� ������ ����� �� ���� � ������� ���� �����, �� ������ �������� '�����'.\r\n", ch);
	}
	else
	{
//		int loadroom = ch->in_room;
		if (RENTABLE(ch) && (GET_LEVEL(ch)!=LVL_IMPL)) // prool: ���� ����� �������� �� ���� � ��������� ������ ��������
		{
			send_to_char("Error 2. � ����� � ������� ���������� ��������� �������� ����������.\r\n", ch);
			return;
		}
		if (!GET_INVIS_LEV(ch))
			act("$n �������$g ����.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);

		perslog("renting", GET_NAME(ch)); // prool
		sprintf(buf, "%s quit the game.", GET_NAME(ch));
		mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), SYSLOG, TRUE);
		send_to_char("�� ��������, ��������... �� ���� ���� �����!\r\n", ch);

		int depot_cost = Depot::get_total_cost_per_day(ch);
		if (depot_cost)
		{
			send_to_char(ch, "�� ���� � ��������� �������� ��������� %ld %s � ����.\r\n", depot_cost, desc_count(depot_cost, WHAT_MONEYu));
			int deadline = ((ch->get_gold() + ch->get_bank()) / depot_cost);
			send_to_char(ch, "����� ����� ������ �� %ld %s.\r\n", deadline, desc_count(deadline, WHAT_DAY));
		}
		Depot::exit_char(ch);
		Clan::clan_invoice(ch, false);

		/*
		 * kill off all sockets connected to the same player as the one who is
		 * trying to quit.  Helps to maintain sanity as well as prevent duping.
		 */
		for (d = descriptor_list; d; d = next_d)
		{
			next_d = d->next;
			if (d == ch->desc)
				continue;
			if (d->character && (GET_IDNUM(d->character) == GET_IDNUM(ch)))
				STATE(d) = CON_DISCONNECT;
		}

		if (free_rent || IS_GOD(ch))
			Crash_rentsave(ch, 0);
		extract_char(ch, FALSE);

	}
}



ACMD(do_save)
{
	if (IS_NPC(ch) || !ch->desc)
		return;

	// Only tell the char we're saving if they actually typed "save"
	if (cmd)
	{		/*
				 * This prevents item duplication by two PC's using coordinated saves
				 * (or one PC with a house) and system crashes. Note that houses are
				 * still automatically saved without this enabled. This code assumes
				 * that guest immortals aren't trustworthy. If you've disabled guest
				 * immortal advances from mortality, you may want < instead of <=.
				 */
		if (auto_save && GET_LEVEL(ch) <= LVL_IMMORT)
		{
			send_to_char("��������� ��������.\r\n", ch);
			write_aliases(ch);
			return;
		}
		sprintf(buf, "��������� %s � ������.\r\n", GET_NAME(ch));
		send_to_char(buf, ch);
	}

	write_aliases(ch);
	ch->save_char();
	Crash_crashsave(ch);
}


// generic function for commands which are normally overridden by
// special procedures - i.e., shop commands, mail commands, etc.
ACMD(do_not_here)
{
	send_to_char("��� ������� ���������� � ���� �����!\r\n", ch);
}

int check_awake(CHAR_DATA * ch, int what)
{
	int i, retval = 0, wgt = 0;

	if (!IS_GOD(ch))
	{
		if (IS_SET(what, ACHECK_AFFECTS) && (AFF_FLAGGED(ch, AFF_STAIRS) || AFF_FLAGGED(ch, AFF_SANCTUARY)))
			SET_BIT(retval, ACHECK_AFFECTS);

		if (IS_SET(what, ACHECK_LIGHT) &&
				IS_DEFAULTDARK(IN_ROOM(ch)) && (AFF_FLAGGED(ch, AFF_SINGLELIGHT) || AFF_FLAGGED(ch, AFF_HOLYLIGHT)))
			SET_BIT(retval, ACHECK_LIGHT);

		for (i = 0; i < NUM_WEARS; i++)
		{
			if (!GET_EQ(ch, i))
				continue;

			if (IS_SET(what, ACHECK_HUMMING) && OBJ_FLAGGED(GET_EQ(ch, i), ITEM_HUM))
				SET_BIT(retval, ACHECK_HUMMING);

			if (IS_SET(what, ACHECK_GLOWING) && OBJ_FLAGGED(GET_EQ(ch, i), ITEM_GLOW))
				SET_BIT(retval, ACHECK_GLOWING);

			if (IS_SET(what, ACHECK_LIGHT) &&
					IS_DEFAULTDARK(IN_ROOM(ch)) &&
					GET_OBJ_TYPE(GET_EQ(ch, i)) == ITEM_LIGHT && GET_OBJ_VAL(GET_EQ(ch, i), 2))
				SET_BIT(retval, ACHECK_LIGHT);

			if (ObjSystem::is_armor_type(GET_EQ(ch, i)) && GET_OBJ_MATER(GET_EQ(ch, i)) <= MAT_COLOR)
				wgt += GET_OBJ_WEIGHT(GET_EQ(ch, i));
		}

		if (IS_SET(what, ACHECK_WEIGHT) && wgt > GET_REAL_STR(ch) * 2)
			SET_BIT(retval, ACHECK_WEIGHT);
	}
	return (retval);
}

int awake_hide(CHAR_DATA * ch)
{
	if (IS_GOD(ch))
		return (FALSE);
	return check_awake(ch, ACHECK_AFFECTS | ACHECK_LIGHT | ACHECK_HUMMING
		| ACHECK_GLOWING | ACHECK_WEIGHT);
}

int awake_sneak(CHAR_DATA * ch)
{
	return awake_hide(ch);
}

int awake_invis(CHAR_DATA * ch)
{
	if (IS_GOD(ch))
		return (FALSE);
	return check_awake(ch, ACHECK_AFFECTS | ACHECK_LIGHT | ACHECK_HUMMING
		| ACHECK_GLOWING);
}

int awake_camouflage(CHAR_DATA * ch)
{
	return awake_invis(ch);
}

int awaking(CHAR_DATA * ch, int mode)
{
	if (IS_GOD(ch))
		return (FALSE);
	if (IS_SET(mode, AW_HIDE) && awake_hide(ch))
		return (TRUE);
	if (IS_SET(mode, AW_INVIS) && awake_invis(ch))
		return (TRUE);
	if (IS_SET(mode, AW_CAMOUFLAGE) && awake_camouflage(ch))
		return (TRUE);
	if (IS_SET(mode, AW_SNEAK) && awake_sneak(ch))
		return (TRUE);
	return (FALSE);
}

int char_humming(CHAR_DATA * ch)
{
	int i;

	if (IS_NPC(ch) && !AFF_FLAGGED(ch, AFF_CHARM))
		return (FALSE);

	for (i = 0; i < NUM_WEARS; i++)
	{
		if (GET_EQ(ch, i) && OBJ_FLAGGED(GET_EQ(ch, i), ITEM_HUM))
			return (TRUE);
	}
	return (FALSE);
}

/*
int char_glowing(CHAR_DATA * ch)
{
	int i;

	if (IS_NPC(ch) && !AFF_FLAGGED(ch, AFF_CHARM))
		return (FALSE);

	for (i = 0; i < NUM_WEARS; i++)
	{
		if (GET_EQ(ch, i) && OBJ_FLAGGED(GET_EQ(ch, i), ITEM_GLOW))
			return (TRUE);
	}
	return (FALSE);
}
*/

ACMD(do_sneak)
{
	AFFECT_DATA af;
	ubyte prob, percent;

	if (IS_NPC(ch) || !ch->get_skill(SKILL_SNEAK))
	{
		send_to_char("�� �� �� ������ ���.\r\n", ch);
		return;
	}

	if (on_horse(ch))
	{
		act("��� ����� �������� � ������ ����� ��� $N1", FALSE, ch, 0, get_horse(ch), TO_CHAR);
		return;
	}

	if (affected_by_spell(ch, SPELL_GLITTERDUST))
	{
		send_to_char("�� �������� ���������, ���������� ������ ��������� ��������...\r\n", ch);
		return;
	}

	affect_from_char(ch, SPELL_SNEAK);

	if (affected_by_spell(ch, SPELL_SNEAK))
	{
		send_to_char("�� ��� ��������� ��������.\r\n", ch);
		return;
	}

	send_to_char("������, �� ����������� ��������� ��������.\r\n", ch);
	REMOVE_BIT(EXTRA_FLAGS(ch, EXTRA_FAILSNEAK), EXTRA_FAILSNEAK);
	percent = number(1, skill_info[SKILL_SNEAK].max_percent);
	prob = calculate_skill(ch, SKILL_SNEAK, skill_info[SKILL_SNEAK].max_percent, 0);

	af.type = SPELL_SNEAK;
	af.duration = pc_duration(ch, 0, GET_LEVEL(ch), 8, 0, 1);
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.battleflag = 0;
	if (percent > prob)
		af.bitvector = 0;
	else
		af.bitvector = AFF_SNEAK;
	affect_to_char(ch, &af);
}

ACMD(do_camouflage)
{
	AFFECT_DATA af;
	struct timed_type timed;
	ubyte prob, percent;

	if (IS_NPC(ch) || !ch->get_skill(SKILL_CAMOUFLAGE))
	{
		send_to_char("�� �� �� ������ ���.\r\n", ch);
		return;
	}

	if (on_horse(ch))
	{
		send_to_char("�� ��������������� ��� ������ ���� �����������.\r\n", ch);
		return;
	}

	if (affected_by_spell(ch, SPELL_GLITTERDUST))
	{
		send_to_char("�� ��������������� ��� ������� �����.\r\n", ch);
		return;
	}


	if (timed_by_skill(ch, SKILL_CAMOUFLAGE))
	{
		send_to_char("� ��� ���� �� ������� ��������. �������� ������� ����� �����.\r\n", ch);
		return;
	}

	if (IS_IMMORTAL(ch))
		affect_from_char(ch, SPELL_CAMOUFLAGE);

	if (affected_by_spell(ch, SPELL_CAMOUFLAGE))
	{
		send_to_char("�� ��� ������������.\r\n", ch);
		return;
	}

	send_to_char("�� ������ �������� �������������.\r\n", ch);
	REMOVE_BIT(EXTRA_FLAGS(ch, EXTRA_FAILCAMOUFLAGE), EXTRA_FAILCAMOUFLAGE);
	percent = number(1, skill_info[SKILL_CAMOUFLAGE].max_percent);
	prob = calculate_skill(ch, SKILL_CAMOUFLAGE, skill_info[SKILL_CAMOUFLAGE].max_percent, 0);

	af.type = SPELL_CAMOUFLAGE;
	af.duration = pc_duration(ch, 0, GET_LEVEL(ch), 6, 0, 2);
	af.modifier = world[IN_ROOM(ch)]->zone;
	af.location = APPLY_NONE;
	af.battleflag = 0;
	if (percent > prob)
		af.bitvector = 0;
	else
		af.bitvector = AFF_CAMOUFLAGE;
	affect_to_char(ch, &af);
	if (!IS_IMMORTAL(ch))
	{
		timed.skill = SKILL_CAMOUFLAGE;
		timed.time = 2;
		timed_to_char(ch, &timed);
	}
}

ACMD(do_hide)
{
	AFFECT_DATA af;
	ubyte prob, percent;

	if (IS_NPC(ch) || !ch->get_skill(SKILL_HIDE))
	{
		send_to_char("�� �� �� ������ ���.\r\n", ch);
		return;
	}

	if (on_horse(ch))
	{
		act("� ���� �� ������ �������� $N3?", FALSE, ch, 0, get_horse(ch), TO_CHAR);
		return;
	}

	affect_from_char(ch, SPELL_HIDE);

	if (affected_by_spell(ch, SPELL_HIDE))
	{
		send_to_char("�� ��� ��������� ����������.\r\n", ch);
		return;
	}
	if (AFF_FLAGGED(ch, AFF_BLIND))
	{
		send_to_char("�� ����� � �� ������ ���� ���������.\r\n", ch);
		return;
	}

	if (affected_by_spell(ch, SPELL_GLITTERDUST))
	{
		send_to_char("����������?! �� �� ��������� ��� ������ �� ����� �������!.\r\n", ch);
		return;
	}

	send_to_char("������, �� ����������� ����������.\r\n", ch);
	REMOVE_BIT(EXTRA_FLAGS(ch, EXTRA_FAILHIDE), EXTRA_FAILHIDE);
	percent = number(1, skill_info[SKILL_HIDE].max_percent);
	prob = calculate_skill(ch, SKILL_HIDE, skill_info[SKILL_HIDE].max_percent, 0);

	af.type = SPELL_HIDE;
	af.duration = pc_duration(ch, 0, GET_LEVEL(ch), 8, 0, 1);
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.battleflag = 0;
	if (percent > prob)
		af.bitvector = 0;
	else
		af.bitvector = AFF_HIDE;
	affect_to_char(ch, &af);
}

void go_steal(CHAR_DATA * ch, CHAR_DATA * vict, char *obj_name)
{
	int percent, gold, eq_pos, ohoh = 0, success = 0, prob;
	OBJ_DATA *obj;

	if (!vict)
		return;

	if (!WAITLESS(ch) && vict->get_fighting())
	{
		act("$N ������� ������ ������������.", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}

	if (!WAITLESS(ch) && ROOM_FLAGGED(IN_ROOM(vict), ROOM_ARENA))
	{
		send_to_char("��������� ��� �������� �����������.\r\n", ch);
		return;
	}

	// 101% is a complete failure
	percent = number(1, skill_info[SKILL_STEAL].max_percent);

	if (WAITLESS(ch) || (GET_POS(vict) <= POS_SLEEPING && !AFF_FLAGGED(vict, AFF_SLEEP)))
		success = 1;	// ALWAYS SUCCESS, unless heavy object.

	if (!AWAKE(vict))	// Easier to steal from sleeping people.
		percent = MAX(percent - 50, 0);

	if (IS_IMMORTAL(vict)) // prool
	{
	send_to_char("� ���������� �������� ������. �����\r\n", ch);
	return;
	}
	// NO NO With Imp's and Shopkeepers, and if player thieving is not allowed
	if ((IS_IMMORTAL(vict) || GET_GOD_FLAG(vict, GF_GODSLIKE) || GET_MOB_SPEC(vict) == shop_ext)
		&& !IS_IMPL(ch))
	{
		send_to_char("�� ������������ ������ � ������ �������� ��������.\r\n", ch);
		return;
	}

	if (str_cmp(obj_name, "coins") &&
			str_cmp(obj_name, "gold") && str_cmp(obj_name, "���") && str_cmp(obj_name, "������"))
	{
		if (!(obj = get_obj_in_list_vis(ch, obj_name, vict->carrying)))
		{
			for (eq_pos = 0; eq_pos < NUM_WEARS; eq_pos++)
				if (GET_EQ(vict, eq_pos) &&
						(isname(obj_name, GET_EQ(vict, eq_pos)->aliases)) &&
						CAN_SEE_OBJ(ch, GET_EQ(vict, eq_pos)))
				{
					obj = GET_EQ(vict, eq_pos);
					break;
				}
			if (!obj)
			{
				act("� � �$S ����� � ��� - ��-��-�� (2 ����)...", FALSE, ch, 0, vict, TO_CHAR);
				return;
			}
			else  	// It is equipment
			{
				if (!success)
				{
					send_to_char("�������? �� ����������? ���-�-�!\r\n", ch);
					return;
				}
				else if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
				{
					send_to_char("�� �� ������� ������ ������� ���������.\r\n", ch);
					return;
				}
				else if (IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj) > CAN_CARRY_W(ch))
				{
					send_to_char("�� �� ������� ������ ����� ���.\r\n", ch);
					return;
				}
				else if (IS_OBJ_STAT(obj, ITEM_BLOODY))
				{
					send_to_char("\"�������� ������!\" - ���������� � ��� � ������, � �� ������� ������ ��������� ����, �� ������������ � �����.\r\n", ch);
					return;
				}
				else
				{
					act("�� ������� $N3 � ����� $o3.", FALSE, ch, obj, vict, TO_CHAR);
					act("$n �����$g $o3 � $N1.", FALSE, ch, obj, vict, TO_NOTVICT | TO_ARENA_LISTEN);
					obj_to_char(unequip_char(vict, eq_pos), ch);
				}
			}
		}
		else  	// obj found in inventory
		{
			if (IS_OBJ_STAT(obj, ITEM_BLOODY))
			{
				send_to_char("\"�������� ������!\" - ���������� � ��� � ������, � �� ������� ������ ��������� ����, �� ������������ � �����.\r\n", ch);
				return;
			}
			percent += GET_OBJ_WEIGHT(obj);	// Make heavy harder
			prob = calculate_skill(ch, SKILL_STEAL, percent, vict);

			if (AFF_FLAGGED(ch, AFF_HIDE))
				prob += 5;	// Add by Alez - Improove in hide steal probability
			if (!WAITLESS(ch) && AFF_FLAGGED(vict, AFF_SLEEP))
				prob = 0;
			if (percent > prob && !success)
			{
				ohoh = TRUE;
				if (AFF_FLAGGED(ch, AFF_HIDE))
				{
					affect_from_char(ch, SPELL_HIDE);
					send_to_char("�� ���������� ���������.\r\n", ch);
					act("$n ���������$g ���������.", FALSE, ch, 0, 0, TO_ROOM);
				};
				send_to_char("����.. ������� �� �����!\r\n", ch);
				act("$n �����$u ��������� ���!", FALSE, ch, 0, vict, TO_VICT);
				act("$n �����$u ������� ����� � $N1.", TRUE, ch, 0, vict, TO_NOTVICT | TO_ARENA_LISTEN);
			}
			else  	// Steal the item
			{
				if (IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))
				{
					if (IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj) < CAN_CARRY_W(ch))
					{
						obj_from_char(obj);
						obj_to_char(obj, ch);
						act("�� ������ $o3 � $N1!", FALSE, ch, obj, vict, TO_CHAR);
					}
				}
				else
				{
					send_to_char("�� �� ������ ������� �����.\r\n", ch);
					return;
				}
			}
			if (CAN_SEE(vict, ch) && AWAKE(vict))
				improove_skill(ch, SKILL_STEAL, 0, vict);
		}
	}
	else  		// Steal some coins
	{
		prob = calculate_skill(ch, SKILL_STEAL, percent, vict);
		if (AFF_FLAGGED(ch, AFF_HIDE))
			prob += 5;	// Add by Alez - Improove in hide steal probability
		if (!WAITLESS(ch) && AFF_FLAGGED(vict, AFF_SLEEP))
			prob = 0;
		if (percent > prob && !success)
		{
			ohoh = TRUE;
			if (AFF_FLAGGED(ch, AFF_HIDE))
			{
				affect_from_char(ch, SPELL_HIDE);
				send_to_char("�� ���������� ���������.\r\n", ch);
				act("$n ���������$g ���������.", FALSE, ch, 0, 0, TO_ROOM);
			};
			send_to_char("�� ������... ��� �������... � �� �� �������..\r\n", ch);
			act("�� ���������� ���� $n1 � ����� �������.", FALSE, ch, 0, vict, TO_VICT);
			act("$n �����$u ���������� ������ � $N1.", TRUE, ch, 0, vict, TO_NOTVICT | TO_ARENA_LISTEN);
		}
		else  	// Steal some gold coins
		{
			if (!vict->get_gold())
			{
				act("$E �����$A, ��� �������� ���� :)", FALSE, ch, 0, vict, TO_CHAR);
				return;
			}
			else
			{
				// ������� ����������� ����-��������� (��������� ���� �����)
				if ((number(1, 100) - ch->get_skill(SKILL_STEAL) -
						ch->get_dex() + vict->get_wis() + vict->get_gold() / 500) < 0)
				{
					act("����� ������� $N1 ����������� � ���.", TRUE, ch, 0, vict, TO_CHAR);
					gold = vict->get_gold();
				}
				else
					gold = (int)((vict->get_gold() * number(1, 75)) / 100);

				if (gold > 0)
				{
					if (gold > 1)
					{
						sprintf(buf, "��-�-�-�! �� ���� ������ %d %s.\r\n",
								gold, desc_count(gold, WHAT_MONEYu));
						send_to_char(buf, ch);
					}
					else
					{
						send_to_char("���-�-� ! �� ������ :) 1 (����) ���� :(.\r\n", ch);
					}
					ch->add_gold(gold);
					split_or_clan_tax(ch, gold);
					vict->remove_gold(gold);
				}
				else
					send_to_char("�� ������ �� ������ �������...\r\n", ch);
			}
		}
		if (CAN_SEE(vict, ch) && AWAKE(vict))
			improove_skill(ch, SKILL_STEAL, 0, vict);
	}
	if (!WAITLESS(ch) && ohoh)
		WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
	pk_thiefs_action(ch, vict);
	if (ohoh && IS_NPC(vict) && AWAKE(vict) && CAN_SEE(vict, ch) && MAY_ATTACK(vict))
		hit(vict, ch, TYPE_UNDEFINED, 1);
}


ACMD(do_steal)
{
	CHAR_DATA *vict;
	char vict_name[MAX_INPUT_LENGTH], obj_name[MAX_INPUT_LENGTH];

	if (IS_NPC(ch) || !ch->get_skill(SKILL_STEAL))
	{
		send_to_char("�� �� �� ������ ���.\r\n", ch);
		return;
	}

	if (!WAITLESS(ch) && on_horse(ch))
	{
		send_to_char("������ ��� ������� ��������������.\r\n", ch);
		return;
	}

	two_arguments(argument, obj_name, vict_name);

	if (!(vict = get_char_vis(ch, vict_name, FIND_CHAR_ROOM)))
	{
		send_to_char("������� � ����?\r\n", ch);
		return;
	}
	else if (vict == ch)
	{
		send_to_char("���������� ������� \"������� <n> ���\".\r\n", ch);
		return;
	}

	if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL) && !(IS_IMMORTAL(ch) || GET_GOD_FLAG(ch, GF_GODSLIKE)))
	{
		send_to_char("����� ������� �����. ��� �� ������� �������� ��� ���������...\r\n", ch);
		return;
	}

	if (IS_NPC(vict)
			&& (MOB_FLAGGED(vict, MOB_NOFIGHT) || AFF_FLAGGED(vict, AFF_SHIELD) || MOB_FLAGGED(vict, MOB_PROTECT))
			&& !(IS_IMMORTAL(ch) || GET_GOD_FLAG(ch, GF_GODSLIKE)))
	{
		send_to_char("� ����� �������? ������� ����!\r\n������� ���, �� ���������� �� ���� ���������.\r\n", ch);
		return;
	}

	go_steal(ch, vict, obj_name);
}

ACMD(do_features)
{
	if (IS_NPC(ch))
		return;
	skip_spaces(&argument);
	if (is_abbrev(argument, "���") || is_abbrev(argument, "all"))
		list_feats(ch, ch, TRUE);
	else
		list_feats(ch, ch, FALSE);
}

ACMD(do_skills)
{
	if (IS_NPC(ch))
		return;
	list_skills(ch, ch);
}

ACMD(do_spells)
{
	if (IS_NPC(ch))
		return;
	skip_spaces(&argument);
	if (is_abbrev(argument, "���") || is_abbrev(argument, "all"))
		list_spells(ch, ch, TRUE);
	else
		list_spells(ch, ch, FALSE);
}


ACMD(do_visible)
{
	if (IS_IMMORTAL(ch))
	{
		perform_immort_vis(ch);
		return;
	}

	if (AFF_FLAGGED(ch, AFF_INVISIBLE)
		|| AFF_FLAGGED(ch, AFF_CAMOUFLAGE)
		|| AFF_FLAGGED(ch, AFF_HIDE)
		|| AFF_FLAGGED(ch, AFF_SNEAK))
	{
		appear(ch);
		send_to_char("�� ��������� ���� ���������.\r\n", ch);
	}
	else
		send_to_char("�� � ��� ������.\r\n", ch);
}

ACMD(do_courage)
{
	OBJ_DATA *obj;
	AFFECT_DATA af[4];
	int prob;
	struct timed_type timed;
	int i;
	if (IS_NPC(ch))		// Cannot use GET_COND() on mobs.
		return;

	if (!ch->get_skill(SKILL_COURAGE))
	{
		send_to_char("��� ��� �� �� �����.\r\n", ch);
		return;
	}

	if (timed_by_skill(ch, SKILL_COURAGE))
	{
		send_to_char("�� �� ������ ������� ����� ������� � ������.\r\n", ch);
		return;
	}

	timed.skill = SKILL_COURAGE;
	timed.time = 6;
	timed_to_char(ch, &timed);
	/******************* Remove to hit()
	percent = number(1,skill_info[SKILL_COURAGE].max_percent+GET_REAL_MAX_HIT(ch)-GET_HIT(ch));
	prob    = train_skill(ch,SKILL_COURAGE,skill_info[SKILL_COURAGE].max_percent,0);
	if (percent > prob)
	   {af[0].type      = SPELL_COURAGE;
	    af[0].duration  = pc_duration(ch,3,0,0,0,0);
	    af[0].modifier  = 0;
	    af[0].location  = APPLY_DAMROLL;
	    af[0].bitvector = AFF_NOFLEE;
	    af[0].battleflag= 0;
	    af[1].type      = SPELL_COURAGE;
	    af[1].duration  = pc_duration(ch,3,0,0,0,0);
	    af[1].modifier  = 0;
	    af[1].location  = APPLY_HITROLL;
	    af[1].bitvector = AFF_NOFLEE;
	    af[1].battleflag= 0;
	    af[2].type      = SPELL_COURAGE;
	    af[2].duration  = pc_duration(ch,3,0,0,0,0);
	    af[2].modifier  = 20;
	    af[2].location  = APPLY_AC;
	    af[2].bitvector = AFF_NOFLEE;
	    af[2].battleflag= 0;
	   }
	else
	   {af[0].type      = SPELL_COURAGE;
	    af[0].duration  = pc_duration(ch,3,0,0,0,0);
	    af[0].modifier  = MAX(1, (prob+19) / 20);
	    af[0].location  = APPLY_DAMROLL;
	    af[0].bitvector = AFF_NOFLEE;
	    af[0].battleflag= 0;
	    af[1].type      = SPELL_COURAGE;
	    af[1].duration  = pc_duration(ch,3,0,0,0,0);
	    af[1].modifier  = MAX(1, (prob+9) / 10);
	    af[1].location  = APPLY_HITROLL;
	    af[1].bitvector = AFF_NOFLEE;
	    af[1].battleflag= 0;
	    af[2].type      = SPELL_COURAGE;
	    af[2].duration  = pc_duration(ch,3,0,0,0,0);
	    af[2].modifier  = 20;
	    af[2].location  = APPLY_AC;
	    af[2].bitvector = AFF_NOFLEE;
	    af[2].battleflag= 0;
	   }
	 for (prob = 0; prob < 3; prob++)
	     affect_join(ch,&af[prob],TRUE,FALSE,TRUE,FALSE);
	 ************************************/
	prob = calculate_skill(ch, SKILL_COURAGE, skill_info[SKILL_COURAGE].max_percent, 0) / 20;
	af[0].type = SPELL_COURAGE;
	af[0].duration = pc_duration(ch, 3, 0, 0, 0, 0);
	af[0].modifier = 40;
	af[0].location = APPLY_AC;
	af[0].bitvector = AFF_NOFLEE;
	af[0].battleflag = 0;
	af[1].type = SPELL_COURAGE;
	af[1].duration = pc_duration(ch, 3, 0, 0, 0, 0);
	af[1].modifier = MAX(1, prob);
	af[1].location = APPLY_DAMROLL;
	af[1].bitvector = AFF_NOFLEE;
	af[1].battleflag = 0;
	af[2].type = SPELL_COURAGE;
	af[2].duration = pc_duration(ch, 3, 0, 0, 0, 0);
	af[2].modifier = MAX(1, prob * 7);
	af[2].location = APPLY_ABSORBE;
	af[2].bitvector = AFF_NOFLEE;
	af[2].battleflag = 0;
	af[3].type = SPELL_COURAGE;
	af[3].duration = pc_duration(ch, 3, 0, 0, 0, 0);
	af[3].modifier = 50;
	af[3].location = APPLY_HITREG;
	af[3].bitvector = AFF_NOFLEE;
	af[3].battleflag = 0;

	for (i = 0; i < 4; i++)
		affect_join(ch, &af[i], TRUE, FALSE, TRUE, FALSE);

	send_to_char("�� ������ � ������.\r\n", ch);
	if ((obj = GET_EQ(ch, WEAR_WIELD)) || (obj = GET_EQ(ch, WEAR_BOTHS)))
		strcpy(buf, "����� $n1 �������� ������ � $e ������� ����$g � ����� $o3.");
	else
		strcpy(buf, "����� $n1 �������� ������.");
	act(buf, FALSE, ch, obj, 0, TO_ROOM | TO_ARENA_LISTEN);
}

int max_group_size(CHAR_DATA *ch)
{
	return MAX_GROUPED_FOLLOWERS + (int) VPOSI((ch->get_skill(SKILL_LEADERSHIP) - 80) / 5, 0, 4);
}

bool is_group_member(CHAR_DATA *ch, CHAR_DATA *vict)
{
	if (IS_NPC(vict) || !AFF_FLAGGED(vict, AFF_GROUP) || vict->master != ch)
		return false;
	else
		return true;
}

/**
* ����� ������ ������ �� ��������� � ���� ��������.
* ��� ����� ��� ���� �������� � ������, ���� �� �����.
* \param vict - ����� �����, ���� ����� ���������� �� ������� '�� ����� ���',
* ������ ����� �������������� ��������� �������, ���� �������, �� �������, ���
* ��������� ������ ������� ������ � ����� ���������� �� ���������� �������.
*/
void change_leader(CHAR_DATA *ch, CHAR_DATA *vict)
{
	if (IS_NPC(ch) || ch->master || !ch->followers)
		return;

	CHAR_DATA *leader = vict;
	if (!leader)
	{
		// ����� ����, ���� ���������� � ������������ ������� �������
		for (struct follow_type *l = ch->followers; l; l = l->next)
		{
			if (!is_group_member(ch, l->follower))
				continue;
			if (!leader)
				leader = l->follower;
			else if (l->follower->get_skill(SKILL_LEADERSHIP) > leader->get_skill(SKILL_LEADERSHIP))
				leader = l->follower;
		}
	}
	if (!leader) return;

	// ��� ������������ ���������� ����������� �������
	std::vector<CHAR_DATA *> temp_list;
	for (struct follow_type *n = 0, *l = ch->followers; l; l = n)
	{
		n = l->next;
		if (!is_group_member(ch, l->follower))
			continue;
		else
		{
			CHAR_DATA *temp_vict = l->follower;
			if (temp_vict->master && stop_follower(temp_vict, SF_SILENCE))
				continue;
			if (temp_vict != leader)
				temp_list.push_back(temp_vict);
		}
	}

	// ��� ��� ����� ������ ��� ����, ����� ��� ������������ �������� �� ������ � ��������
	// ����������� � ��������� ������������� ������� ���������� � ������
	if (!temp_list.empty())
		for (std::vector<CHAR_DATA *>::reverse_iterator it = temp_list.rbegin(); it != temp_list.rend(); ++it)
			add_follower(*it, leader, true);

	// ������� ������ ��������� ���������� ������� � ������, ���� �� �����
	if (vict)
	{
		// ���� ������ ���� �����, ����� ��� ������� �� ����� �������� � ������ ������
		REMOVE_BIT(AFF_FLAGS(ch, AFF_GROUP), AFF_GROUP);
		add_follower(ch, leader, true);
	}

	if (!leader->followers)
	{
		return;
	}

	ch->dps_copy(leader);
	perform_group(leader, leader);
	int followers = 0;
	for (struct follow_type *f = leader->followers; f; f = f->next)
	{
		if (followers < max_group_size(leader))
		{
			if (perform_group(leader, f->follower))
				++followers;
		}
		else
		{
			send_to_char("�� ������ ������ �� ������ ������� � ������.\r\n", ch);
			return;
		}
	}
}

int perform_group(CHAR_DATA * ch, CHAR_DATA * vict)
{
	if (AFF_FLAGGED(vict, AFF_GROUP) ||
			AFF_FLAGGED(vict, AFF_CHARM) || MOB_FLAGGED(vict, MOB_ANGEL) || IS_HORSE(vict))
		return (FALSE);

	SET_BIT(AFF_FLAGS(vict, AFF_GROUP), AFF_GROUP);
	if (ch != vict)
	{
		act("$N ������$A � ����� ������ ������ (����-��, ������ :).", FALSE, ch, 0, vict, TO_CHAR);
		act("�� ������� � ������ $n1.", FALSE, ch, 0, vict, TO_VICT);
		act("$N ������$A � ������ $n1.", FALSE, ch, 0, vict, TO_NOTVICT | TO_ARENA_LISTEN);
	}
	return (TRUE);
}

int low_charm(CHAR_DATA * ch)
{
	AFFECT_DATA *aff;
	for (aff = ch->affected; aff; aff = aff->next)
		if (aff->type == SPELL_CHARM && aff->duration <= 1)
			return (TRUE);
	return (FALSE);
}

void print_one_line(CHAR_DATA * ch, CHAR_DATA * k, int leader, int header)
{
	int ok, ok2, div;
	const char *WORD_STATE[] = { "��� ������",
								 "��.���.���",
								 "��.���.���",
								 " ���.�����",
								 " ���.�����",
								 "  �����   ",
								 "  �����   ",
								 "  �����   ",
								 "���.����� ",
								 "���.����� ",
								 "����.�����",
								 " �������� "
							   };
	const char *MOVE_STATE[] = { "�������",
								 "�������",
								 "�.�����",
								 " ����� ",
								 " ����� ",
								 "�.�����",
								 "�.�����",
								 "������ ",
								 "������ ",
								 "������ ",
								 "������.",
								 " ����� "
							   };
	const char *POS_STATE[] = { "����",
								"�������� ������",
								"��� ������",
								"��� ��������",
								"����",
								"��������",
								"�����",
								"���������",
								"�����"
							  };

	if (IS_NPC(k))
	{
		if (!header)
//       send_to_char("��������       | �������� |�����| ��� | ���������     | ������.\r\n",ch);
			send_to_char("��������            | �������� |�����| ������ | ���������\r\n", ch);
		std::string name = GET_NAME(k);
		name[0] = UPPER(name[0]);
		sprintf(buf, "%s%-20s%s|", CCIBLU(ch, C_NRM),
				name.substr(0, 20).c_str(), CCNRM(ch, C_NRM));
		sprintf(buf + strlen(buf), "%s%10s%s|",
				color_value(ch, GET_HIT(k), GET_REAL_MAX_HIT(k)),
				WORD_STATE[posi_value(GET_HIT(k), GET_REAL_MAX_HIT(k)) + 1], CCNRM(ch, C_NRM));

		ok = IN_ROOM(ch) == IN_ROOM(k);
		sprintf(buf + strlen(buf), "%s%5s%s|",
				ok ? CCGRN(ch, C_NRM) : CCRED(ch, C_NRM), ok ? " ��  " : " ��� ", CCNRM(ch, C_NRM));

		sprintf(buf + strlen(buf), " %s%s%s%s%s%s%s%s%s%s%s%s%s |",
				CCIRED(ch, C_NRM), AFF_FLAGGED(k, AFF_SANCTUARY) ? "�" : (AFF_FLAGGED(k, AFF_PRISMATICAURA)
						? "�" : " "), CCGRN(ch,
											 C_NRM),
				AFF_FLAGGED(k, AFF_WATERBREATH) ? "�" : " ", CCICYN(ch,
						C_NRM),
				AFF_FLAGGED(k, AFF_INVISIBLE) ? "�" : " ", CCIYEL(ch, C_NRM), (AFF_FLAGGED(k, AFF_SINGLELIGHT)
						|| AFF_FLAGGED(k, AFF_HOLYLIGHT)
						|| (GET_EQ(k, WEAR_LIGHT)
							&&
							GET_OBJ_VAL(GET_EQ
										(k, WEAR_LIGHT),
										2))) ? "�" : " ",
				CCIBLU(ch, C_NRM), AFF_FLAGGED(k, AFF_FLY) ? "�" : " ", CCYEL(ch, C_NRM),
				low_charm(k) ? "�" : " ", CCNRM(ch, C_NRM));

//      sprintf(buf+strlen(buf),"%-15s| %d",POS_STATE[(int) GET_POS(k)],
//                                        on_charm_points(k));
		sprintf(buf + strlen(buf), "%-15s", POS_STATE[(int) GET_POS(k)]);

		act(buf, FALSE, ch, 0, k, TO_CHAR);
	}
	else
	{
		if (!header)
			send_to_char
			("��������            | �������� |�������|�����|�����| ������ | ��� | ���������\r\n", ch);

		std::string name = GET_NAME(k);
		name[0] = UPPER(name[0]);
		sprintf(buf, "%s%-20s%s|", CCIBLU(ch, C_NRM), name.c_str(), CCNRM(ch, C_NRM));
		sprintf(buf + strlen(buf), "%s%10s%s|",
				color_value(ch, GET_HIT(k), GET_REAL_MAX_HIT(k)),
				WORD_STATE[posi_value(GET_HIT(k), GET_REAL_MAX_HIT(k)) + 1], CCNRM(ch, C_NRM));

		sprintf(buf + strlen(buf), "%s%7s%s|",
				color_value(ch, GET_MOVE(k), GET_REAL_MAX_MOVE(k)),
				MOVE_STATE[posi_value(GET_MOVE(k), GET_REAL_MAX_MOVE(k)) + 1], CCNRM(ch, C_NRM));

		ok = IN_ROOM(ch) == IN_ROOM(k);
		sprintf(buf + strlen(buf), "%s%5s%s|",
				ok ? CCGRN(ch, C_NRM) : CCRED(ch, C_NRM), ok ? " ��  " : " ��� ", CCNRM(ch, C_NRM));

		if ((!IS_MANA_CASTER(k) && !MEMQUEUE_EMPTY(k)) ||
				(IS_MANA_CASTER(k) && GET_MANA_STORED(k) < GET_MAX_MANA(k)))
		{
			div = mana_gain(k);
			if (div > 0)
			{
				if (!IS_MANA_CASTER(k))
				{
					ok2 = MAX(0, 1 + GET_MEM_TOTAL(k) - GET_MEM_COMPLETED(k));
					ok2 = ok2 * 60 / div;	// ����� ���� � ���
				}
				else
				{
					ok2 = MAX(0, 1 + GET_MAX_MANA(k) - GET_MANA_STORED(k));
					ok2 = ok2 / div;	// ����� �������������� � ��������
				}
				ok = ok2 / 60;
				ok2 %= 60;
				if (ok > 99)
					sprintf(buf + strlen(buf), "&g%5d&n|", ok);
				else
					sprintf(buf + strlen(buf), "&g%2d:%02d&n|", ok, ok2);
			}
			else
			{
				sprintf(buf + strlen(buf), "&r    -&n|");
			}
		}
		else
			sprintf(buf + strlen(buf), "     |");

		sprintf(buf + strlen(buf), " %s%s%s%s%s%s%s%s%s%s%s%s%s |",
				CCIRED(ch, C_NRM), AFF_FLAGGED(k, AFF_SANCTUARY) ? "�" : (AFF_FLAGGED(k, AFF_PRISMATICAURA)
						? "�" : " "), CCGRN(ch,
											 C_NRM),
				AFF_FLAGGED(k, AFF_WATERBREATH) ? "�" : " ", CCICYN(ch,
						C_NRM),
				AFF_FLAGGED(k, AFF_INVISIBLE) ? "�" : " ", CCIYEL(ch, C_NRM), (AFF_FLAGGED(k, AFF_SINGLELIGHT)
						|| AFF_FLAGGED(k, AFF_HOLYLIGHT)
						|| (GET_EQ(k, WEAR_LIGHT)
							&&
							GET_OBJ_VAL(GET_EQ
										(k, WEAR_LIGHT),
										2))) ? "�" : " ",
				CCIBLU(ch, C_NRM), AFF_FLAGGED(k, AFF_FLY) ? "�" : " ", CCYEL(ch, C_NRM),
				on_horse(k) ? "�" : " ", CCNRM(ch, C_NRM));

		sprintf(buf + strlen(buf), "%5s|", leader ? "�����" : "");
		sprintf(buf + strlen(buf), "%s", POS_STATE[(int) GET_POS(k)]);
		act(buf, FALSE, ch, 0, k, TO_CHAR);
	}
}


void print_group(CHAR_DATA * ch)
{
	int gfound = 0, cfound = 0;
	CHAR_DATA *k;
	struct follow_type *f, *g;

	k = (ch->master ? ch->master : ch);
	if (AFF_FLAGGED(ch, AFF_GROUP))
	{
		send_to_char("���� ������ ������� ��:\r\n", ch);
		if (AFF_FLAGGED(k, AFF_GROUP))
			print_one_line(ch, k, TRUE, gfound++);
		for (f = k->followers; f; f = f->next)
		{
			if (!AFF_FLAGGED(f->follower, AFF_GROUP))
				continue;
			print_one_line(ch, f->follower, FALSE, gfound++);
		}
	}
	for (f = ch->followers; f; f = f->next)
	{
		if (!(AFF_FLAGGED(f->follower, AFF_CHARM)
				|| MOB_FLAGGED(f->follower, MOB_ANGEL)))
			continue;
		if (!cfound)
			send_to_char("���� �������������:\r\n", ch);
		print_one_line(ch, f->follower, FALSE, cfound++);
	}
	if (!gfound && !cfound)
	{
		send_to_char("�� �� �� �� ���� (� ������ ������ ����� �����) ������!\r\n", ch);
		return;
	}
	if (PRF_FLAGGED(ch, PRF_SHOWGROUP))
		for (g = k->followers, cfound = 0; g; g = g->next)
		{
			for (f = g->follower->followers; f; f = f->next)
			{
				if (!(AFF_FLAGGED(f->follower, AFF_CHARM)
						|| MOB_FLAGGED(f->follower, MOB_ANGEL))
						|| !AFF_FLAGGED(ch, AFF_GROUP))
					continue;
				if (f->follower->master == ch || !AFF_FLAGGED(f->follower->master, AFF_GROUP))
					continue;
// shapirus: ��� ���������� ������ �� ���������� ������ � ����������
				if (PRF_FLAGGED(ch, PRF_NOCLONES) &&
						IS_NPC(f->follower) &&
						(MOB_FLAGGED(f->follower, MOB_CLONE) || GET_MOB_VNUM(f->follower) == MOB_KEEPER))
					continue;
				if (!cfound)
					send_to_char("������������� ������ ����� ������:\r\n", ch);
				print_one_line(ch, f->follower, FALSE, cfound++);
			}
			if (ch->master)
			{
				if (!(AFF_FLAGGED(g->follower, AFF_CHARM)
						|| MOB_FLAGGED(g->follower, MOB_ANGEL))
						|| !AFF_FLAGGED(ch, AFF_GROUP))
					continue;
// shapirus: ��� ���������� ������ �� ���������� ������ � ����������
				if (PRF_FLAGGED(ch, PRF_NOCLONES) &&
						IS_NPC(g->follower) &&
						(MOB_FLAGGED(g->follower, MOB_CLONE) || GET_MOB_VNUM(g->follower) == MOB_KEEPER))
					continue;
				if (!cfound)
					send_to_char("������������� ������ ����� ������:\r\n", ch);
				print_one_line(ch, g->follower, FALSE, cfound++);
			}
		}
}

ACMD(do_group)
{
	CHAR_DATA *vict;
	struct follow_type *f;
	int found, f_number;

	argument = one_argument(argument, buf);

	if (!*buf)
	{
		print_group(ch);
		return;
	}

	if (GET_POS(ch) < POS_RESTING)
	{
		send_to_char("������ ��������� ������� � ����� ���������.\r\n", ch);
		return;
	}

	if (ch->master)
	{
		act("�� �� ������ ��������� �������. �� ��� �� �������.", FALSE, ch, 0, 0, TO_CHAR);
		return;
	}

	if (!ch->followers)
	{
		send_to_char("�� ���� ����� �� �������.\r\n", ch);
		return;
	}
// ��������� ���������� ��������������
	for (f_number = 0, f = ch->followers; f; f = f->next)
		if (AFF_FLAGGED(f->follower, AFF_GROUP))
			f_number++;

	if (!str_cmp(buf, "all") || !str_cmp(buf, "���"))
	{
		perform_group(ch, ch);
		for (found = 0, f = ch->followers; f; f = f->next)
		{
			if ((f_number + found) >= max_group_size(ch))
			{
				send_to_char("�� ������ ������ �� ������ ������� � ������.\r\n", ch);
				return;
			}
			found += perform_group(ch, f->follower);
		}
		if (!found)
			send_to_char("���, ��� �� ���� �������, ��� �������� � ���� ������.\r\n", ch);
		return;
	}
	else if (!str_cmp(buf, "leader") || !str_cmp(buf, "�����"))
	{
		vict = get_player_vis(ch, argument, FIND_CHAR_WORLD);
		// added by WorM (�������) ���� ������ ���� � ��� ������ ��������
		// � �� ���� ���-�� ������ ������� %1 �� �������� ������ ����� ������.
		if (vict && IS_NPC(vict) && MOB_FLAGGED(vict, MOB_CLONE) && AFF_FLAGGED(vict, AFF_CHARM) && vict->master && !IS_NPC(vict->master))
		{
			if (CAN_SEE(ch, vict->master))
			{
				vict = vict->master;
			}
			else
			{
				vict = NULL;
			}
		}
		// end by WorM
		if (!vict)
		{
			send_to_char("��� ������ ���������.\r\n", ch);
			return;
		}
		else if (vict == ch)
		{
			send_to_char("�� � ��� ����� ������...\r\n", ch);
			return;
		}
		else if (!AFF_FLAGGED(vict, AFF_GROUP) || vict->master != ch)
		{
			send_to_char(ch, "%s �� �������� ������ ����� ������.\r\n", GET_NAME(vict));
			return;
		}
		change_leader(ch, vict);
		return;
	}

	if (!(vict = get_char_vis(ch, buf, FIND_CHAR_ROOM)))
		send_to_char(NOPERSON, ch);
	else if ((vict->master != ch) && (vict != ch))
		act("$N2 ����� ��������� �� ����, ����� ����� ������ ����� ������.", FALSE, ch, 0, vict, TO_CHAR);
	else
	{
		if (!AFF_FLAGGED(vict, AFF_GROUP))
		{
			if (AFF_FLAGGED(vict, AFF_CHARM) || MOB_FLAGGED(vict, MOB_ANGEL) || IS_HORSE(vict))
			{
				send_to_char("������ ������������ ��������� ����� ���� �������� � ������.\r\n", ch);
				send_to_char("������ ������������ ��������� ����� ���� �������� � ������.\r\n", vict);
			};
			if (f_number >= max_group_size(ch))
			{
				send_to_char("�� ������ ������ �� ������ ������� � ������.\r\n", ch);
				return;
			}
			perform_group(ch, ch);
			perform_group(ch, vict);
		}
		else if (ch != vict)
		{
			act("$N ��������$A �� ������� ����� ������.", FALSE, ch, 0, vict, TO_CHAR);
			act("�� ��������� �� ������ $n1!", FALSE, ch, 0, vict, TO_VICT);
			act("$N ���$G ��������$A �� ������ $n1!", FALSE, ch, 0, vict, TO_NOTVICT | TO_ARENA_LISTEN);
			REMOVE_BIT(AFF_FLAGS(vict, AFF_GROUP), AFF_GROUP);
		}
	}
}

ACMD(do_ungroup)
{
	struct follow_type *f, *next_fol;
	CHAR_DATA *tch;

	one_argument(argument, buf);

	if (ch->master || !(AFF_FLAGGED(ch, AFF_GROUP)))
	{
		send_to_char("�� �� �� ����� ������!\r\n", ch);
		return;
	}

	if (!*buf)
	{
		sprintf(buf2, "�� ��������� �� ������ %s.\r\n", GET_PAD(ch, 1));
		for (f = ch->followers; f; f = next_fol)
		{
			next_fol = f->next;
			if (AFF_FLAGGED(f->follower, AFF_GROUP))
			{
				REMOVE_BIT(AFF_FLAGS(f->follower, AFF_GROUP), AFF_GROUP);
				send_to_char(buf2, f->follower);
				if (!AFF_FLAGGED(f->follower, AFF_CHARM) && !(IS_NPC(f->follower)
						&& AFF_FLAGGED(f->follower, AFF_HORSE)))
					stop_follower(f->follower, SF_EMPTY);
			}
		}
		REMOVE_BIT(AFF_FLAGS(ch, AFF_GROUP), AFF_GROUP);
		send_to_char("�� ���������� ������.\r\n", ch);
		return;
	}
	for (f = ch->followers; f; f = next_fol)
	{
		next_fol = f->next;
		tch = f->follower;
		if (isname(buf, tch->get_pc_name()) && !AFF_FLAGGED(tch, AFF_CHARM) && !IS_HORSE(tch))
		{
			REMOVE_BIT(AFF_FLAGS(tch, AFF_GROUP), AFF_GROUP);
			act("$N ����� �� ���� ����� ������.", FALSE, ch, 0, tch, TO_CHAR);
			act("�� ��������� �� ������ $n1!", FALSE, ch, 0, tch, TO_VICT);
			act("$N ���$G ������$A �� ������ $n1!", FALSE, ch, 0, tch, TO_NOTVICT | TO_ARENA_LISTEN);
			stop_follower(tch, SF_EMPTY);
			return;
		}
	}
	send_to_char("���� ����� �� ������ � ������ ����� ������.\r\n", ch);
	return;
}

ACMD(do_report)
{
	CHAR_DATA *k;
	struct follow_type *f;

	if (!AFF_FLAGGED(ch, AFF_GROUP) && !AFF_FLAGGED(ch, AFF_CHARM))
	{
		send_to_char("� ����� ��� �� �������������?\r\n", ch);
		return;
	}
	if (IS_MANA_CASTER(ch))
	{
		sprintf(buf, "%s �������%s : %d(%d)H, %d(%d)V, %d(%d)M\r\n",
				GET_NAME(ch), GET_CH_SUF_1(ch),
				GET_HIT(ch), GET_REAL_MAX_HIT(ch),
				GET_MOVE(ch), GET_REAL_MAX_MOVE(ch),
				GET_MANA_STORED(ch), GET_MAX_MANA(ch));
	}
	else if (AFF_FLAGGED(ch, AFF_CHARM))
	{
		AFFECT_DATA *aff;
		int loyalty = 0;
		for (aff = ch->affected; aff; aff = aff->next)
			if (aff->type == SPELL_CHARM)
			{
				loyalty = aff->duration / 2;
				break;
			}
		sprintf(buf, "%s �������%s : %d(%d)H, %d(%d)V, %dL\r\n",
				GET_NAME(ch), GET_CH_SUF_1(ch),
				GET_HIT(ch), GET_REAL_MAX_HIT(ch),
				GET_MOVE(ch), GET_REAL_MAX_MOVE(ch),
				loyalty);
	}
	else
	{
		sprintf(buf, "%s �������%s : %d(%d)H, %d(%d)V\r\n",
				GET_NAME(ch), GET_CH_SUF_1(ch),
				GET_HIT(ch), GET_REAL_MAX_HIT(ch),
				GET_MOVE(ch), GET_REAL_MAX_MOVE(ch));
	}
	CAP(buf);
	k = (ch->master ? ch->master : ch);
	for (f = k->followers; f; f = f->next)
		if (AFF_FLAGGED(f->follower, AFF_GROUP) && f->follower != ch && !AFF_FLAGGED(f->follower, AFF_DEAFNESS))
			send_to_char(buf, f->follower);
	if (k != ch && !AFF_FLAGGED(k, AFF_DEAFNESS))
		send_to_char(buf, k);
	send_to_char("�� �������� � ��������� ���� ������ ����� ������.\r\n", ch);
}



ACMD(do_split)
{
	int amount, num, share, rest;
	CHAR_DATA *k;
	struct follow_type *f;

	if (IS_NPC(ch))
		return;

	one_argument(argument, buf);

	if (is_number(buf))
	{
		amount = atoi(buf);
		if (amount <= 0)
		{
			send_to_char("� ��� �� ��� ���������� �������?\r\n", ch);
			return;
		}
		if (amount > ch->get_gold())
		{
			send_to_char("� ��� �� ����� ��� ������� �����?.\r\n", ch);
			return;
		}

		k = (ch->master ? ch->master : ch);

		if (AFF_FLAGGED(k, AFF_GROUP) && (k->in_room == ch->in_room))
			num = 1;
		else
			num = 0;

		for (f = k->followers; f; f = f->next)
			if (AFF_FLAGGED(f->follower, AFF_GROUP) &&
					!IS_NPC(f->follower) && IN_ROOM(f->follower) == IN_ROOM(ch))
				num++;

		if (num && AFF_FLAGGED(ch, AFF_GROUP))
		{
			share = amount / num;
			rest = amount % num;
		}
		else
		{
			send_to_char("� ��� �� ������ ��������� ��� �����?\r\n", ch);
			return;
		}

		ch->remove_gold(share * (num - 1));

		sprintf(buf, "%s ��������%s %d %s; ��� ��������� %d.\r\n",
				GET_NAME(ch), GET_CH_SUF_1(ch), amount, desc_count(amount, WHAT_MONEYu), share);
		if (AFF_FLAGGED(k, AFF_GROUP) && IN_ROOM(k) == IN_ROOM(ch) && !IS_NPC(k) && k != ch)
		{
			send_to_char(buf, k);
			k->add_gold(share, true, true);
		}
		for (f = k->followers; f; f = f->next)
		{
			if (AFF_FLAGGED(f->follower, AFF_GROUP) &&
					!IS_NPC(f->follower) && IN_ROOM(f->follower) == IN_ROOM(ch) && f->follower != ch)
			{
				send_to_char(buf, f->follower);
				f->follower->add_gold(share, true, true);
			}
		}
		sprintf(buf, "�� ��������� %d %s �� %d  -  �� %d �������.\r\n",
				amount, desc_count(amount, WHAT_MONEYu), num, share);
		if (rest)
		{
			sprintf(buf + strlen(buf),
				"��� �������� ����� �� �������� %d %s (������� �� ������ ��������� ������) ����.\r\n",
				rest, desc_count(rest, WHAT_MONEYu));
		}
		send_to_char(buf, ch);
		// ����-����� ������ � ��� �����, ������� ����� ������� � ������
		const long clan_tax = ClanSystem::do_gold_tax(ch, share);
		ch->remove_gold(clan_tax);
	}
	else
	{
		send_to_char("������� � ���� �� ������ ���������?\r\n", ch);
		return;
	}
}

OBJ_DATA * get_obj_equip_or_carry(CHAR_DATA *ch, const std::string &text)
{
	int eq_num = 0;
	OBJ_DATA *obj = get_object_in_equip_vis(ch, text, ch->equipment, &eq_num);
	if (!obj)
	{
		obj = get_obj_in_list_vis(ch, text, ch->carrying);
	}
	return obj;
}

void apply_enchant(CHAR_DATA *ch, OBJ_DATA *obj, std::string text)
{
	std::string tmp_buf;
	GetOneParam(text, tmp_buf);
	if (tmp_buf.empty())
	{
		send_to_char("������� ���� ����������.\r\n", ch);
		return;
	}

	OBJ_DATA *target = get_obj_equip_or_carry(ch, tmp_buf);
	if (!target)
	{
		send_to_char(ch, "���������, ��� � ��� %s.\r\n", arg);
		return;
	}

	if (OBJ_FLAGGED(target, ITEM_SETSTUFF))
	{
		std::string name = GET_OBJ_PNAME(target, 0) ? GET_OBJ_PNAME(target, 0) : "<null>";
		name[0] = UPPER(name[0]);
		send_to_char(ch, "%s �� %s ���� ���������%s, �.�. %s ������ ������ ���������.\r\n",
				name.c_str(),
				GET_OBJ_SEX(target) == SEX_POLY ? "�����" : "�����",
				GET_OBJ_VIS_SUF_6(target, ch),
				GET_OBJ_SEX(target) == SEX_POLY ? "��������" : "��������");
		return;
	}
	if (target->enchants.check(obj::ENCHANT_FROM_OBJ))
	{
		send_to_char(ch, "�� %s ��� �������� �����������.\r\n",
			GET_OBJ_PNAME(target, 3));
		return;
	}

	int check_slots = GET_OBJ_WEAR(obj) & GET_OBJ_WEAR(target);
	if (check_slots > 0 && check_slots != ITEM_WEAR_TAKE)
	{
		send_to_char(ch, "�� ������� ���������� %s.\r\n", GET_OBJ_PNAME(target, 0));
		obj::enchant ench(obj);
		ench.apply_to_obj(target);
		extract_obj(obj);
	}
	else
	{
		int slots = obj->obj_flags.wear_flags;
		REMOVE_BIT(slots, ITEM_WEAR_TAKE);
		if (sprintbit(slots, wear_bits, buf2))
		{
			send_to_char(ch,
				"��� ����������� ����������� � ��������� �� ������� ���������: %s\r\n", buf2);
		}
		else
		{
			send_to_char(ch,
				"����������� �����������, �� ����������� ����� ���������.\r\n", buf2);
		}
	}
}

ACMD(do_use)
{
	OBJ_DATA *mag_item;
	int do_hold = 0;

	two_arguments(argument, arg, buf);
	if (!*arg)
	{
		sprintf(buf2, "��� �� ������ %s?\r\n", CMD_NAME);
		send_to_char(buf2, ch);
		return;
	}

	if (IS_SET(PRF_FLAGS(ch, PRF_IRON_WIND), PRF_IRON_WIND))
	{
		send_to_char("�� � ���, � ��� ������ �� �� ���� ���������� ����������!\r\n", ch);
		return;
	}

	mag_item = GET_EQ(ch, WEAR_HOLD);

	if (!mag_item || !isname(arg, mag_item->aliases))
	{
		switch (subcmd)
		{
		case SCMD_RECITE:
		case SCMD_QUAFF:
			if (!(mag_item = get_obj_in_list_vis(ch, arg, ch->carrying)))
			{
				sprintf(buf2, "���������, ��� � ��� %s.\r\n", arg);
				send_to_char(buf2, ch);
				return;
			}
			break;
		case SCMD_USE:
			mag_item = get_obj_in_list_vis(ch, arg, ch->carrying);
			if (!mag_item || GET_OBJ_TYPE(mag_item) != ITEM_ENCHANT)
			{
				sprintf(buf2, "�������� � ���� '%s' ����� �����������!\r\n", arg);
				send_to_char(buf2, ch);
				return;
			}
			break;
		default:
			log("SYSERR: Unknown subcmd %d passed to do_use.", subcmd);
			return;
		}
	}
	switch (subcmd)
	{
	case SCMD_QUAFF:
		if (IS_SET(PRF_FLAGS(ch, PRF_IRON_WIND), PRF_IRON_WIND))
		{
			send_to_char("�� ����� ����������� � ���!\r\n", ch);
			return;
		}
		if (GET_OBJ_TYPE(mag_item) != ITEM_POTION)
		{
			send_to_char("������� �� ������ ������ ������� (��, ����� ��� ���� �� ����� ;)\r\n", ch);
			return;
		}
		do_hold = 1;
		break;
	case SCMD_RECITE:
		if (GET_OBJ_TYPE(mag_item) != ITEM_SCROLL)
		{
			send_to_char("�������� ��� ����������� ������ ������.\r\n", ch);
			return;
		}
		do_hold = 1;
		break;
	case SCMD_USE:
		if (GET_OBJ_TYPE(mag_item) == ITEM_ENCHANT)
		{
			apply_enchant(ch, mag_item, buf);
			return;
		}
		if ((GET_OBJ_TYPE(mag_item) != ITEM_WAND) && (GET_OBJ_TYPE(mag_item) != ITEM_STAFF))
		{
			send_to_char("��������� ����� ������ ���������� ��������!\r\n", ch);
			return;
		}
		// ������� � �������/����������� ����� ������ ������� � ����� �� 25 ������
		if (GET_OBJ_VAL(mag_item, 3) == SPELL_CHARM
				|| GET_OBJ_VAL(mag_item, 3) == SPELL_ANIMATE_DEAD
				|| GET_OBJ_VAL(mag_item, 3) == SPELL_RESSURECTION)
		{
			if (!can_use_feat(ch, MAGIC_USER_FEAT))
			{
				send_to_char("��, ����� ���� ����������! �� ���������� ��������� ��� �� ������������. :(\r\n", ch);
				return;
			}
		}
		break;
	}
	if (do_hold && GET_EQ(ch, WEAR_HOLD) != mag_item)
	{
		if (GET_EQ(ch, WEAR_BOTHS))
			do_hold = WEAR_BOTHS;
		else if (GET_EQ(ch, WEAR_SHIELD))
			do_hold = WEAR_SHIELD;
		else
			do_hold = WEAR_HOLD;

		if (GET_EQ(ch, do_hold))
		{
			act("�� ���������� ������������ $o3.", FALSE, ch, GET_EQ(ch, do_hold), 0, TO_CHAR);
			act("$n ���������$g ������������ $o3.", FALSE, ch, GET_EQ(ch, do_hold), 0, TO_ROOM | TO_ARENA_LISTEN);
			obj_to_char(unequip_char(ch, do_hold), ch);
		}
		if (GET_EQ(ch, WEAR_HOLD))
			obj_to_char(unequip_char(ch, WEAR_HOLD), ch);
		//obj_from_char(mag_item);
		equip_char(ch, mag_item, WEAR_HOLD);
	}
	if ((do_hold && GET_EQ(ch, WEAR_HOLD) == mag_item) || (!do_hold))
		mag_objectmagic(ch, mag_item, buf);
}



ACMD(do_wimpy)
{
	int wimp_lev;

	// 'wimp_level' is a player_special. -gg 2/25/98
	if (IS_NPC(ch))
		return;

	one_argument(argument, arg);

	if (!*arg)
	{
		if (GET_WIMP_LEV(ch))
		{
			sprintf(buf, "�� ����������� ������ ��� %d ��.\r\n", GET_WIMP_LEV(ch));
			send_to_char(buf, ch);
			return;
		}
		else
		{
			send_to_char("�� ������ �������, ������� � ������� (���� �� �������, �����...)\r\n", ch);
			return;
		}
	}
	if (isdigit(*arg))
	{
		if ((wimp_lev = atoi(arg)) != 0)
		{
			if (wimp_lev < 0)
				send_to_char("��, �������� ������. � ������ ������ �� � ��� ������� :)\r\n", ch);
			else if (wimp_lev > GET_REAL_MAX_HIT(ch))
				send_to_char("�������� ������ ������ �� ������ ���������� ��.\r\n", ch);
			else if (wimp_lev > (GET_REAL_MAX_HIT(ch) / 2))
				send_to_char
				("������������. ����� �� �����, �� �� ����� �������� ������������ ��.\r\n", ch);
			else
			{
				sprintf(buf, "�������. �� ������� (��� �������) �� ���������� %d ��.\r\n", wimp_lev);
				send_to_char(buf, ch);
				GET_WIMP_LEV(ch) = wimp_lev;
			}
		}
		else
		{
			send_to_char("�� ������ ��������� �� ����� (������ ����� ������ ;).\r\n", ch);
			GET_WIMP_LEV(ch) = 0;
		}
	}
	else
		send_to_char
		("��������, ��� ���������� ������ ���������� �� �� ���������� ������� (0 - ������� �� ������)\r\n",
		 ch);
}

void set_display_bits(CHAR_DATA *ch, bool flag)
{
	if (flag)
	{
		SET_BIT(PRF_FLAGS(ch, PRF_DISPHP), PRF_DISPHP);
		SET_BIT(PRF_FLAGS(ch, PRF_DISPMANA), PRF_DISPMANA);
		SET_BIT(PRF_FLAGS(ch, PRF_DISPMOVE), PRF_DISPMOVE);
		SET_BIT(PRF_FLAGS(ch, PRF_DISPEXITS), PRF_DISPEXITS);
		SET_BIT(PRF_FLAGS(ch, PRF_DISPGOLD), PRF_DISPGOLD);
		SET_BIT(PRF_FLAGS(ch, PRF_DISPLEVEL), PRF_DISPLEVEL);
		SET_BIT(PRF_FLAGS(ch, PRF_DISPEXP), PRF_DISPEXP);
		SET_BIT(PRF_FLAGS(ch, PRF_DISPFIGHT), PRF_DISPFIGHT);
		if (!IS_IMMORTAL(ch))
		{
			SET_BIT(PRF_FLAGS(ch, PRF_DISP_TIMED), PRF_DISP_TIMED);
		}
	}
	else
	{
		REMOVE_BIT(PRF_FLAGS(ch, PRF_DISPHP), PRF_DISPHP);
		REMOVE_BIT(PRF_FLAGS(ch, PRF_DISPMANA), PRF_DISPMANA);
		REMOVE_BIT(PRF_FLAGS(ch, PRF_DISPMOVE), PRF_DISPMOVE);
		REMOVE_BIT(PRF_FLAGS(ch, PRF_DISPEXITS), PRF_DISPEXITS);
		REMOVE_BIT(PRF_FLAGS(ch, PRF_DISPGOLD), PRF_DISPGOLD);
		REMOVE_BIT(PRF_FLAGS(ch, PRF_DISPLEVEL), PRF_DISPLEVEL);
		REMOVE_BIT(PRF_FLAGS(ch, PRF_DISPEXP), PRF_DISPEXP);
		REMOVE_BIT(PRF_FLAGS(ch, PRF_DISPFIGHT), PRF_DISPFIGHT);
		REMOVE_BIT(PRF_FLAGS(ch, PRF_DISP_TIMED), PRF_DISP_TIMED);
	}
}

const char *DISPLAY_HELP =
	"������: ������ { { � | � | � | � | � | � | � | � | � } | ��� | ��� }\r\n";

ACMD(do_display)
{
	if (IS_NPC(ch))
	{
		send_to_char("� ����� ��� �������? �� �����������.\r\n", ch);
		return;
	}
	skip_spaces(&argument);

	if (!*argument)
	{
		send_to_char(DISPLAY_HELP, ch);
		return;
	}

	if (!str_cmp(argument, "on") || !str_cmp(argument, "all") ||
			!str_cmp(argument, "���") || !str_cmp(argument, "���"))
	{
		set_display_bits(ch, true);
	}
	else if (!str_cmp(argument, "off")
		|| !str_cmp(argument, "none")
		|| !str_cmp(argument, "����")
		|| !str_cmp(argument, "���"))
	{
		set_display_bits(ch, false);
	}
	else
	{
		set_display_bits(ch, false);

		const size_t len = strlen(argument);
		for (size_t i = 0; i < len; i++)
		{
			switch (LOWER(argument[i]))
			{
			case 'h':
			case '�':
				SET_BIT(PRF_FLAGS(ch, PRF_DISPHP), PRF_DISPHP);
				break;
			case 'w':
			case '�':
				SET_BIT(PRF_FLAGS(ch, PRF_DISPMANA), PRF_DISPMANA);
				break;
			case 'm':
			case '�':
				SET_BIT(PRF_FLAGS(ch, PRF_DISPMOVE), PRF_DISPMOVE);
				break;
			case 'e':
			case '�':
				SET_BIT(PRF_FLAGS(ch, PRF_DISPEXITS), PRF_DISPEXITS);
				break;
			case 'g':
			case '�':
				SET_BIT(PRF_FLAGS(ch, PRF_DISPGOLD), PRF_DISPGOLD);
				break;
			case 'l':
			case '�':
				SET_BIT(PRF_FLAGS(ch, PRF_DISPLEVEL), PRF_DISPLEVEL);
				break;
			case 'x':
			case '�':
				SET_BIT(PRF_FLAGS(ch, PRF_DISPEXP), PRF_DISPEXP);
				break;
			case '�':
			case 'f':
				SET_BIT(PRF_FLAGS(ch, PRF_DISPFIGHT), PRF_DISPFIGHT);
				break;
			case '�':
			case 't':
				SET_BIT(PRF_FLAGS(ch, PRF_DISP_TIMED), PRF_DISP_TIMED);
				break;
			case ' ':
				break;
			default:
				send_to_char(DISPLAY_HELP, ch);
				return;
			}
		}
	}

	send_to_char(OK, ch);
}

#define TOG_OFF 0
#define TOG_ON  1
const char *gen_tog_type[] = { "����������", "autoexits",
							   "�������", "brief",
							   "������", "compact",
							   "����", "color",
							   "������", "norepeat",
							   "���������", "notell",
							   "���-��", "noinvistell",
							   "�������", "nogossip",
							   "�������", "noshout",
							   "�����", "noholler",
							   "������������", "nogratz",
							   "�������", "noauction",
							   "�����", "exchange",
							   "�������", "quest",
							   "��������������", "automem",
							   "��� �����", "nohassle",
							   "������", "nosummon",
							   "�������", "nowiz",
							   "����� ������", "roomflags",
							   "����������", "slowns",
							   "������������", "trackthru",
							   "������������", "holylight",
							   "�����", "coder",
							   "��������������", "goahead",
							   "������", "showgroup",
							   "��� ���������", "noclones",
							   "����������", "autoassist",
							   "����������", "autoloot",
							   "���������", "autosplit",
							   "����� ����", "automoney",
							   "�����", "arena",
							   "������", "length",
							   "������", "width",
							   "�����", "screen",
							   "�������", "news",
							   "�����", "boards",
							   "���������", "chest",
							   "������", "pklist",
							   "��������", "politics",
							   "��������", "pkformat",
							   "����������", "workmate",
							   "������", "offtop",
							   "������ �����", "disconnect",
							   "�����������", "ingredient",
							   "���������", "remember",
							   "�����������", "notify",
							   "�����", "map",
							   "���� � ����", "enter zone",
							   "��������", "misprint",
							   "�������", "mageshields",
							   "����������", "autonosummon",
							   "\n"
							 };




struct gen_tog_param_type
{
	int level;
	int subcmd;
} gen_tog_param[] =
{
	{
		0, SCMD_AUTOEXIT}, {
		0, SCMD_BRIEF}, {
		0, SCMD_COMPACT}, {
		0, SCMD_COLOR}, {
		0, SCMD_NOREPEAT}, {
		0, SCMD_NOTELL}, {
		0, SCMD_NOINVISTELL}, {
		0, SCMD_NOGOSSIP}, {
		0, SCMD_NOSHOUT}, {
		0, SCMD_NOHOLLER}, {
		0, SCMD_NOGRATZ}, {
		0, SCMD_NOAUCTION}, {
		0, SCMD_NOEXCHANGE}, {
		0, SCMD_QUEST}, {
		0, SCMD_AUTOMEM}, {
		LVL_GRGOD, SCMD_NOHASSLE}, {
		0, SCMD_NOSUMMON}, {
		LVL_GOD, SCMD_NOWIZ}, {
		LVL_GRGOD, SCMD_ROOMFLAGS}, {
		LVL_IMPL, SCMD_SLOWNS}, {
		LVL_GOD, SCMD_TRACK}, {
		LVL_GOD, SCMD_HOLYLIGHT}, {
		LVL_IMPL, SCMD_CODERINFO}, {
		0, SCMD_GOAHEAD}, {
		0, SCMD_SHOWGROUP}, {
		0, SCMD_NOCLONES}, {
		0, SCMD_AUTOASSIST}, {
		0, SCMD_AUTOLOOT}, {
		0, SCMD_AUTOSPLIT}, {
		0, SCMD_AUTOMONEY}, {
		0, SCMD_NOARENA}, {
		0, SCMD_LENGTH}, {
		0, SCMD_WIDTH}, {
		0, SCMD_SCREEN}, {
		0, SCMD_NEWS_MODE}, {
		0, SCMD_BOARD_MODE}, {
		0, SCMD_CHEST_MODE}, {
		0, SCMD_PKL_MODE}, {
		0, SCMD_POLIT_MODE} , {
		0, SCMD_PKFORMAT_MODE}, {
		0, SCMD_WORKMATE_MODE}, {
		0, SCMD_OFFTOP_MODE}, {
		0, SCMD_ANTIDC_MODE}, {
		0, SCMD_NOINGR_MODE}, {
		0, SCMD_REMEMBER}, {
		0, SCMD_NOTIFY_EXCH}, {
		0, SCMD_DRAW_MAP}, {
		0, SCMD_ENTER_ZONE}, {
		LVL_GOD, SCMD_MISPRINT}, {
		0, SCMD_BRIEF_SHIELDS}, {
		0, SCMD_AUTO_NOSUMMON}
};

ACMD(do_mode)
{
	int i, showhelp = FALSE;
	if (IS_NPC(ch))
		return;

	argument = one_argument(argument, arg);
	if (!*arg)
	{
		do_toggle(ch, argument, 0, 0);
		return;
	}
	else if (*arg == '?')
		showhelp = TRUE;
	else if ((i = search_block(arg, gen_tog_type, FALSE)) < 0)
		showhelp = TRUE;
	else if (GET_LEVEL(ch) < gen_tog_param[i >> 1].level && !Privilege::check_flag(ch, Privilege::KRODER))
	{
		send_to_char("��� ������� ��� ����������.\r\n", ch);
		showhelp = TRUE;
	}
	else
		do_gen_tog(ch, argument, 0, gen_tog_param[i >> 1].subcmd);

	if (showhelp)
	{
		strcpy(buf, "�� ������ ���������� ���������.\r\n");
		for (i = 0; *gen_tog_type[i << 1] != '\n'; i++)
			if (GET_LEVEL(ch) >= gen_tog_param[i].level || Privilege::check_flag(ch, Privilege::KRODER))
				sprintf(buf + strlen(buf), "%-20s(%s)\r\n", gen_tog_type[i << 1], gen_tog_type[(i << 1) + 1]);
		strcat(buf, "\r\n");
		send_to_char(buf, ch);
	}
}


// ��������� ������ flag: 0 - ������, 1 - ������
void SetScreen(CHAR_DATA * ch, char *argument, int flag)
{
	if (IS_NPC(ch))
		return;
	skip_spaces(&argument);
	int size = atoi(argument);

	if (!flag && (size < 30 || size > 300))
		send_to_char("������ ������ ������ ���� � �������� 30 - 300 ��������.\r\n", ch);
	else if (flag == 1 && (size < 10 || size > 100))
		send_to_char("������ ������ ������ ���� � �������� 10 - 100 �����.\r\n", ch);
	else if (!flag)
	{
		STRING_LENGTH(ch) = size;
		send_to_char("�������.\r\n", ch);
		ch->save_char();
	}
	else if (flag == 1)
	{
		STRING_WIDTH(ch) = size;
		send_to_char("�������.\r\n", ch);
		ch->save_char();
	}
	else
	{
		std::ostringstream buffer;
		for (int i = 50; i > 0; --i)
			buffer << i << "\r\n";
		send_to_char(buffer.str(), ch);
	}
}

// * '����� ���������� ���|�����������', �� ������� - ���
void set_autoloot_mode(CHAR_DATA *ch, char *argument)
{
	static const char *message_on = "�������������� ������ ������ �������.\r\n";
	static const char *message_no_ingr = "�������������� ������ ������, �������� ����������� � ���������� ����������, �������.\r\n";
	static const char *message_off = "�������������� ������ ������ ��������.\r\n";

	skip_spaces(&argument);
	if (!*argument)
	{
		if (PRF_TOG_CHK(ch, PRF_AUTOLOOT))
		{
			send_to_char(PRF_FLAGGED(ch, PRF_NOINGR_LOOT) ? message_no_ingr : message_on, ch);
		}
		else
		{
			send_to_char(message_off, ch);
		}
	}
	else if (is_abbrev(argument, "���"))
	{
		SET_BIT(PRF_FLAGS(ch, PRF_AUTOLOOT), PRF_AUTOLOOT);
		REMOVE_BIT(PRF_FLAGS(ch, PRF_NOINGR_LOOT), PRF_NOINGR_LOOT);
		send_to_char(message_on, ch);
	}
	else if (is_abbrev(argument, "�����������"))
	{
		SET_BIT(PRF_FLAGS(ch, PRF_AUTOLOOT), PRF_AUTOLOOT);
		SET_BIT(PRF_FLAGS(ch, PRF_NOINGR_LOOT), PRF_NOINGR_LOOT);
		send_to_char(message_no_ingr, ch);
	}
	else if (is_abbrev(argument, "���"))
	{
		REMOVE_BIT(PRF_FLAGS(ch, PRF_AUTOLOOT), PRF_AUTOLOOT);
		REMOVE_BIT(PRF_FLAGS(ch, PRF_NOINGR_LOOT), PRF_NOINGR_LOOT);
		send_to_char(message_off, ch);
	}
	else
	{
		send_to_char("������ �������: ����� ���������� <���-����������|���|�����������|���>\r\n", ch);
	}
}

//Polud ��������� �������-����������� ������
void setNotifyEchange(CHAR_DATA* ch, char *argument)
{
	skip_spaces(&argument);
	if (!*argument)
	{
		send_to_char(ch, "������ �������: ����� ����������� <����������� ����, ����� �� 0 �� %ld>.\r\n", 0x7fffffff);
		return;
	}
	long size = atol(argument);
	if (size>=100)
	{
		send_to_char(ch, "��� ����� ��������� ����������� � ������� � ������ ����� ����� ���������� �� ����� ��� %ld %s.\r\n",
			size, desc_count(size, WHAT_MONEYa));
		NOTIFY_EXCH_PRICE(ch) = size;
		ch->save_char();
	}
	else if (size>=0 && size<100)
	{
		send_to_char(ch, "��� �� ����� ��������� ����������� � ������� � ������ ����� �����, ��� ��� ������� ���� ������ 100 ���.\r\n");
		NOTIFY_EXCH_PRICE(ch) = 0;
		ch->save_char();
	}
	else
	{
		send_to_char(ch, "������� ��������� ���� �� 0 �� %ld\r\n", 0x7fffffff);
	}

}
//-Polud
ACMD(do_gen_tog)
{
	long result = 0;

	const char *tog_messages[][2] =
	{
		{"�� �������� �� �������.\r\n",
			"�� ������ ���� ��������.\r\n"},
		{"Nohassle disabled.\r\n",
		 "Nohassle enabled.\r\n"},
		{"������� ����� ��������.\r\n",
		 "������� ����� �������.\r\n"},
		{"������ ����� ��������.\r\n",
		 "������ ����� �������.\r\n"},
		{"� ��� ����� ����������.\r\n",
		 "�� ����� � ����������.\r\n"},
		{"��� ����� ���������� ��������� ��������.\r\n",
		 "�� ��������� �� ������� � ��������.\r\n"},
		{"�� ������� ��, ��� ����.\r\n",
		 "�� ����� � ����, ��� ����.\r\n"},
		{"�� ������� ��� ��������.\r\n",
		 "�� ����� � ��������.\r\n"},
		{"�� ������� ��� ������������.\r\n",
		 "�� ����� � �������������.\r\n"},
		{"You can now hear the Wiz-channel.\r\n",
		 "You are now deaf to the Wiz-channel.\r\n"},
		{"�� ������ �� ���������� �������.\r\n",
		 "�� ���������� �������!\r\n"},
		{"�� ������ �� ������ ������ ����� ������.\r\n",
		 "�� �������� ��������� ����� ������.\r\n"},
		{"���� ��������� ����� �������������.\r\n",
		 "���� ��������� �� ����� ������������� ���.\r\n"},
		{"HolyLight mode off.\r\n",
		 "HolyLight mode on.\r\n"},
		{"Nameserver_is_slow changed to NO; IP addresses will now be resolved.\r\n",
		 "Nameserver_is_slow changed to YES; sitenames will no longer be resolved.\r\n"},
		{"����� ������� ������������� ��������.\r\n",
		 "����� ������� ������������� �������.\r\n"},
		{"�� �� ������ ����� ��������� ������ �����.\r\n",
		 "�� �������� ����������� ������ �����.\r\n"},
		{"\r\n",
		 "\r\n"},
		{"����� ������ �������������� ���������� ��������.\r\n",
		 "����� ������ �������������� ���������� �������.\r\n"},
		{"�������������� ���������� ���������.\r\n",
		 "�������������� ���������� ��������.\r\n"},
		{"������ ���������.\r\n",
		 "������ ���������.\r\n"},
		{"\r\n",
		 "\r\n"},
		{"�� ������� ��� �����.\r\n",
		 "�� ����� � ������.\r\n"},
		{"����� �������������� (IAC GA) ��������.\r\n",
		 "����� �������������� (IAC GA) �������.\r\n"},
		{"��� ��������� ������� ������ ����� ������������ ������ ��������� � ���� �������������.\r\n",
		 "��� ��������� ������� ������ ����� ������������ ��� ��������� � �������������.\r\n"},
		{"�� �� ������ ������������� �������� �����������.\r\n",
		 "�� ������ ������������� �������� �����������.\r\n"},
		{"", ""}, // SCMD_AUTOLOOT
		{"�� �� ������ ������ ������, �������� � ������.\r\n",
		 "�� ������ ������������� ������ ������, �������� � ������.\r\n"},
		{"�� �� ������ ����� ����, ���������� � ������.\r\n",
		 "�� ������ ������������� ����� ����, ���������� � ������.\r\n"},
		{"�� ������ ������� ��������� � �����.\r\n",
		 "�� �� ������ ������� ��������� � �����.\r\n"},
		{"��� ����� ���������� ��������� ������.\r\n",
		 "�� ��������� �� ������� � ������.\r\n"},
		{"��� ��������� ������� ������ ����� ������������ ��� �������������.\r\n",
		 "��� ��������� ������� ������ �� ����� ������������ ����� �������� � ���������.\r\n"},
		{"� ��� ������ ���������� ��� ������.\r\n",
		 "� ��� ������ ���������� ������ ��, ���� �� ������.\r\n"},
		{"", ""}, // SCMD_LENGTH
		{"", ""}, // SCMD_WIDTH
		{"", ""}, // SCMD_SCREEN
		{"������� ������ �������� ����� � �������: �����.\r\n",
		 "������� ������ �������� ����� � �������: �����.\r\n"},
		{"�� �� ������ ����������� � ����� ���������� �� ������.\r\n",
		 "�� ��������� ����������� � ����� ���������� �� ������.\r\n"},
		{"", ""}, // SCMD_CHEST_MODE
		{"�� ����������� ����������� � ���������� ��� ������� ��� �� ������ ������.\r\n",
		 "�� ��������� ����������� � ���������� ��� ������� ��� �� ������ ������.\r\n"},
		{"�� ����������� ����������� �� ���������� �������� ����� � � ����� �������.\r\n",
		 "�� ��������� ����������� �� ���������� �������� ����� � � ����� �������.\r\n"},
		{"������ ������ ���/��� ���������� ��� '������'.\r\n",
		 "������ ������ ���/��� ���������� ��� '�������'.\r\n"},
		{"��� �� ����� ������������ ����� � ������ �� ���� ����� �����������.\r\n",
		 "�� ������ ����� � ������ �� ���� ����� �����������.\r\n"},
		{"�� ��������� �� ������ ������.\r\n",
		 "�� ������ ������� �������� � ������ ������.\r\n"},
		{"�� ��������� ������ �� ������ ����� �� ����� ���.\r\n",
		 "������ �� ������ ����� �� ����� ��� ��������.\r\n"},
		{"����� ������� � ������ ������������ � ������ ������ ��������.\r\n",
		 "����� ������� � ������ ������������ � ������ ������ �������.\r\n"},
		{"", ""}, 		// SCMD_REMEMBER
		{"", ""},		//SCMD_NOTIFY_EXCH
		{"����� ����� ������������ ��� ����������� ��������.\r\n",
		 "�� ������ ������ ����� ������������ ��� �����������.\r\n"},
		{"����� ���������� ��� ����� � ����� ���� ��������.\r\n",
		 "�� ������ ������ ���������� ��� ����� � ����� ����.\r\n"},
		{"����� ����������� ����� �������� ��������.\r\n",
		 "�� ������ ������ ����������� ����� ��������.\r\n"},
		{"����� ��������� ��� ������������ ���������� �����: ������.\r\n",
		 "����� ��������� ��� ������������ ���������� �����: �������.\r\n"},
		{"�������������� ����� ������ �� ������� ��������.\r\n",
		 "�� ������ ������������� �������� ����� ������ �� ������� ����� ��� �������������.\r\n"}
	};

	if (IS_NPC(ch))
		return;

	switch (subcmd)
	{
	case SCMD_NOSUMMON:
		result = PRF_TOG_CHK(ch, PRF_SUMMONABLE);
		break;
	case SCMD_NOHASSLE:
		result = PRF_TOG_CHK(ch, PRF_NOHASSLE);
		break;
	case SCMD_BRIEF:
		result = PRF_TOG_CHK(ch, PRF_BRIEF);
		break;
	case SCMD_COMPACT:
		result = PRF_TOG_CHK(ch, PRF_COMPACT);
		break;
	case SCMD_NOTELL:
		result = PRF_TOG_CHK(ch, PRF_NOTELL);
		break;
	case SCMD_NOAUCTION:
		result = PRF_TOG_CHK(ch, PRF_NOAUCT);
		break;
	case SCMD_NOHOLLER:
		result = PRF_TOG_CHK(ch, PRF_NOHOLLER);
		break;
	case SCMD_NOGOSSIP:
		result = PRF_TOG_CHK(ch, PRF_NOGOSS);
		break;
	case SCMD_NOSHOUT:
		result = PRF_TOG_CHK(ch, PRF_NOSHOUT);
		break;
	case SCMD_NOGRATZ:
		result = PRF_TOG_CHK(ch, PRF_NOGOSS);
		break;
	case SCMD_NOWIZ:
		result = PRF_TOG_CHK(ch, PRF_NOWIZ);
		break;
	case SCMD_QUEST:
		result = PRF_TOG_CHK(ch, PRF_QUEST);
		break;
	case SCMD_ROOMFLAGS:
		result = PRF_TOG_CHK(ch, PRF_ROOMFLAGS);
		break;
	case SCMD_NOREPEAT:
		result = PRF_TOG_CHK(ch, PRF_NOREPEAT);
		break;
	case SCMD_HOLYLIGHT:
		result = PRF_TOG_CHK(ch, PRF_HOLYLIGHT);
		break;
	case SCMD_SLOWNS:
		result = (nameserver_is_slow = !nameserver_is_slow);
		break;
	case SCMD_AUTOEXIT:
		result = PRF_TOG_CHK(ch, PRF_AUTOEXIT);
		break;
	case SCMD_CODERINFO:
		result = PRF_TOG_CHK(ch, PRF_CODERINFO);
		break;
	case SCMD_AUTOMEM:
		result = PRF_TOG_CHK(ch, PRF_AUTOMEM);
		break;
	case SCMD_COLOR:
		do_color(ch, argument, 0, 0);
		return;
		break;
#if defined(HAVE_ZLIB)
	case SCMD_COMPRESS:
		result = toggle_compression(ch->desc);
		break;
#else
	case SCMD_COMPRESS:
		send_to_char("Compression not supported.\r\n", ch);
		return;
#endif
	case SCMD_GOAHEAD:
		result = PRF_TOG_CHK(ch, PRF_GOAHEAD);
		break;
	case SCMD_SHOWGROUP:
		result = PRF_TOG_CHK(ch, PRF_SHOWGROUP);
		break;
	case SCMD_AUTOASSIST:
		result = PRF_TOG_CHK(ch, PRF_AUTOASSIST);
		break;
	case SCMD_AUTOLOOT:
		set_autoloot_mode(ch, argument);
		return;
	case SCMD_AUTOSPLIT:
		result = PRF_TOG_CHK(ch, PRF_AUTOSPLIT);
		break;
	case SCMD_AUTOMONEY:
		result = PRF_TOG_CHK(ch, PRF_AUTOMONEY);
		break;
	case SCMD_NOARENA:
		result = PRF_TOG_CHK(ch, PRF_NOARENA);
		break;
	case SCMD_NOEXCHANGE:
		result = PRF_TOG_CHK(ch, PRF_NOEXCHANGE);
		break;
	case SCMD_NOCLONES:
		result = PRF_TOG_CHK(ch, PRF_NOCLONES);
		break;
	case SCMD_NOINVISTELL:
		result = PRF_TOG_CHK(ch, PRF_NOINVISTELL);
		break;
	case SCMD_LENGTH:
		SetScreen(ch, argument, 0);
		return;
		break;
	case SCMD_WIDTH:
		SetScreen(ch, argument, 1);
		return;
		break;
	case SCMD_SCREEN:
		SetScreen(ch, argument, 2);
		return;
		break;
	case SCMD_NEWS_MODE:
		result = PRF_TOG_CHK(ch, PRF_NEWS_MODE);
		break;
	case SCMD_BOARD_MODE:
		result = PRF_TOG_CHK(ch, PRF_BOARD_MODE);
		break;
	case SCMD_CHEST_MODE:
	{
		std::string buffer = argument;
		SetChestMode(ch, buffer);
		break;
	}
	case SCMD_PKL_MODE:
		result = PRF_TOG_CHK(ch, PRF_PKL_MODE);
		break;
	case SCMD_POLIT_MODE:
		result = PRF_TOG_CHK(ch, PRF_POLIT_MODE);
		break;
	case SCMD_PKFORMAT_MODE:
		result = PRF_TOG_CHK(ch, PRF_PKFORMAT_MODE);
		break;
	case SCMD_WORKMATE_MODE:
		result = PRF_TOG_CHK(ch, PRF_WORKMATE_MODE);
		break;
	case SCMD_OFFTOP_MODE:
		result = PRF_TOG_CHK(ch, PRF_OFFTOP_MODE);
		break;
	case SCMD_ANTIDC_MODE:
		result = PRF_TOG_CHK(ch, PRF_ANTIDC_MODE);
		break;
	case SCMD_NOINGR_MODE:
		result = PRF_TOG_CHK(ch, PRF_NOINGR_MODE);
		break;
	case SCMD_REMEMBER:
	{
		skip_spaces(&argument);
		if (!*argument)
		{
			send_to_char("������ �������: ����� ��������� <����� ����� �� 1 �� 100>.\r\n", ch);
			return;
		}
		unsigned int size = atoi(argument);
		if (ch->remember_set_num(size))
		{
			send_to_char(ch, "���������� ��������� ����� �� ������� '���������' ����������� � %d.\r\n", size);
			ch->save_char();
		}
		else
		{
			send_to_char(ch, "���������� ����� ��� ������ ����� ���� � �������� �� 1 �� %d.\r\n", Remember::MAX_REMEMBER_NUM);
		}
		return;
	}
	case SCMD_NOTIFY_EXCH:
	{
		setNotifyEchange(ch, argument);
		return;
	}
	case SCMD_DRAW_MAP:
		result = PRF_TOG_CHK(ch, PRF_DRAW_MAP);
		break;
	case SCMD_ENTER_ZONE:
		result = PRF_TOG_CHK(ch, PRF_ENTER_ZONE);
		break;
	case SCMD_MISPRINT:
		result = PRF_TOG_CHK(ch, PRF_MISPRINT);
		break;
	case SCMD_BRIEF_SHIELDS:
		result = PRF_TOG_CHK(ch, PRF_BRIEF_SHIELDS);
		break;
	case SCMD_AUTO_NOSUMMON:
		result = PRF_TOG_CHK(ch, PRF_AUTO_NOSUMMON);
		break;
	default:
		log("SYSERR: Unknown subcmd %d in do_gen_toggle.", subcmd);
		return;
	}

	if (result)
		send_to_char(tog_messages[subcmd][TOG_ON], ch);
	else
		send_to_char(tog_messages[subcmd][TOG_OFF], ch);

	return;
}

ACMD(do_pray)
{
	int metter = -1, i;
	OBJ_DATA *obj = NULL;
	AFFECT_DATA af;
	struct timed_type timed;


	if (IS_NPC(ch))
		return;

	if (!IS_IMMORTAL(ch) &&
			((subcmd == SCMD_DONATE && GET_RELIGION(ch) != RELIGION_POLY) ||
			 (subcmd == SCMD_PRAY && GET_RELIGION(ch) != RELIGION_MONO)))
	{
		send_to_char("�� ������������!\r\n", ch);
		return;
	}

	if (subcmd == SCMD_DONATE && !ROOM_FLAGGED(IN_ROOM(ch), ROOM_POLY))
	{
		send_to_char("������� ���������� ����� ��� ����� ������.\r\n", ch);
		return;
	}
	if (subcmd == SCMD_PRAY && !ROOM_FLAGGED(IN_ROOM(ch), ROOM_MONO))
	{
		send_to_char("��� ����� �� �������� ����������� ���������.\r\n", ch);
		return;
	}

	half_chop(argument, arg, buf);

	if (!*arg || (metter = search_block(arg, pray_whom, FALSE)) < 0)
	{
		if (subcmd == SCMD_DONATE)
		{
			send_to_char("�� ������ �������� ������ :\r\n", ch);
			for (metter = 0; *(pray_metter[metter]) != '\n'; metter++)
				if (*(pray_metter[metter]) == '-')
				{
					send_to_char(pray_metter[metter], ch);
					send_to_char("\r\n", ch);
				}
			send_to_char("�������, ���� � ��� �� ������ ����������.\r\n", ch);
		}
		else if (subcmd == SCMD_PRAY)
		{
			send_to_char("�� ������ �������� ������� :\r\n", ch);
			for (metter = 0; *(pray_metter[metter]) != '\n'; metter++)
				if (*(pray_metter[metter]) == '*')
				{
					send_to_char(pray_metter[metter], ch);
					send_to_char("\r\n", ch);
				}
			send_to_char("�������, ���� �� ������ �������� �������.\r\n", ch);
		}
		return;
	}

	if (subcmd == SCMD_DONATE && *(pray_metter[metter]) != '-')
	{
		send_to_char("��������� ������ ������ ����� �����.\r\n", ch);
		return;
	}

	if (subcmd == SCMD_PRAY && *(pray_metter[metter]) != '*')
	{
		send_to_char("��������� ������� ������ ����� �����.\r\n", ch);
		return;
	}

	if (subcmd == SCMD_DONATE)
	{
		if (!*buf || !(obj = get_obj_in_list_vis(ch, buf, ch->carrying)))
		{
			send_to_char("�� ������ ������������ ���-�� �������.\r\n", ch);
			return;
		}
		if (GET_OBJ_TYPE(obj) != ITEM_FOOD && GET_OBJ_TYPE(obj) != ITEM_TREASURE)
		{
			send_to_char("����� �������� ��� ������.\r\n", ch);
			return;
		}
	}
	else if (subcmd == SCMD_PRAY)
	{
		if (ch->get_gold() < 10)
		{
			send_to_char("� ��� �� ������ ����� �� ������.\r\n", ch);
			return;
		}
	}
	else
		return;

	if (!IS_IMMORTAL(ch) && (timed_by_skill(ch, SKILL_RELIGION)
							 || affected_by_spell(ch, SPELL_RELIGION)))
	{
		send_to_char("�� �� ������ ��� ����� ������� � �����.\r\n", ch);
		return;
	}

	timed.skill = SKILL_RELIGION;
	timed.time = 12;
	timed_to_char(ch, &timed);

	for (i = 0; pray_affect[i].metter >= 0; i++)
		if (pray_affect[i].metter == metter)
		{
			af.type = SPELL_RELIGION;
			af.duration = pc_duration(ch, 12, 0, 0, 0, 0);
			af.modifier = pray_affect[i].modifier;
			af.location = pray_affect[i].location;
			af.bitvector = pray_affect[i].bitvector;
			af.battleflag = pray_affect[i].battleflag;
			affect_join(ch, &af, FALSE, FALSE, FALSE, FALSE);
		}

	if (subcmd == SCMD_PRAY)
	{
		sprintf(buf, "$n ��������$g ������ � ������$q ������� %s.", pray_whom[metter]);
		act(buf, FALSE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		sprintf(buf, "�� ��������� ������ � �������� ������� %s.", pray_whom[metter]);
		act(buf, FALSE, ch, 0, 0, TO_CHAR);
		ch->remove_gold(10);
	}
	else if (subcmd == SCMD_DONATE && obj)
	{
		sprintf(buf, "$n ������$q $o3 � ������ %s.", pray_whom[metter]);
		act(buf, FALSE, ch, obj, 0, TO_ROOM | TO_ARENA_LISTEN);
		sprintf(buf, "�� �������� $o3 � ������ %s.", pray_whom[metter]);
		act(buf, FALSE, ch, obj, 0, TO_CHAR);
		obj_from_char(obj);
		extract_obj(obj);
	}
}

ACMD(do_recall)
{
	if (IS_NPC(ch))
	{
		send_to_char("�������� ������ ������������!\r\n", ch);
		return;
	}

	const int rent_room = real_room(GET_LOADROOM(ch));
	if (rent_room == NOWHERE || ch->in_room == NOWHERE)
	{
		send_to_char("��� ������ ������������!\r\n", ch);
		return;
	}

	if (!IS_IMMORTAL(ch)
		&& (SECT(ch->in_room) == SECT_SECRET
			|| ROOM_FLAGGED(ch->in_room, ROOM_NOMAGIC)
			|| ROOM_FLAGGED(ch->in_room, ROOM_DEATH)
			|| ROOM_FLAGGED(ch->in_room, ROOM_SLOWDEATH)
			|| ROOM_FLAGGED(ch->in_room, ROOM_TUNNEL)
			|| ROOM_FLAGGED(ch->in_room, ROOM_NORELOCATEIN)
			|| ROOM_FLAGGED(ch->in_room, ROOM_NOTELEPORTIN)
			|| ROOM_FLAGGED(ch->in_room, ROOM_ICEDEATH)
			|| ROOM_FLAGGED(ch->in_room, ROOM_GODROOM)
			|| !Clan::MayEnter(ch, ch->in_room, HCE_PORTAL)
			|| !Clan::MayEnter(ch, rent_room, HCE_PORTAL)))
	{
		send_to_char("� ��� �� ���������� ���������!\r\n", ch);
		return;
	}

	send_to_char("��� ����� ���������� ��������� �������� �� ����� �����!\r\n", ch);
	if (1/*IS_GOD(ch) || Noob::is_noob(ch)*/) // prool
	{
		if (ch->in_room != rent_room)
		{
			send_to_char("�� �������������, ��� ���-�� �������� ���� ���������� ��� � ����-�� ������!\r\n", ch);
			act("$n ������$a ����� � ���� � �������� �����$q!", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
			char_from_room(ch);
			char_to_room(ch, rent_room);
			look_at_room(ch, 0);
			act("$n �������� ������$u � ������ �������!", TRUE, ch, 0, 0, TO_ROOM);
		}
		else
		{
			send_to_char("�� ��� � ��� ���������� �����...\r\n", ch);
		}
	}
	else
	{
		send_to_char("�� � ������ ���� �� ��������...\r\n", ch);
	}
}

void perform_beep(CHAR_DATA *ch, CHAR_DATA *vict)
{
	send_to_char(CCRED(vict, C_NRM), vict);
	sprintf(buf, "\007\007 $n �������� ���!");
	act(buf, FALSE, ch, 0, vict, TO_VICT | TO_SLEEP);
	send_to_char(CCNRM(vict, C_NRM), vict);

	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		send_to_char(OK, ch);
	else
	{
		send_to_char(CCRED(ch, C_CMP), ch);
		sprintf(buf, "�� ������� $N3.");
		act(buf, FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
		send_to_char(CCNRM(ch, C_CMP), ch);
	}
}

ACMD(do_beep)
{
	CHAR_DATA *vict = NULL;

	one_argument(argument, buf);

	if (!*buf)
		send_to_char("���� ��������?\r\n", ch);
	else if (!(vict = get_char_vis(ch, buf, FIND_CHAR_WORLD)) || IS_NPC(vict))
		send_to_char(NOPERSON, ch);
	else if (ch == vict)
		send_to_char("\007\007�� ������� ����!\r\n", ch);
	else if (PRF_FLAGGED(ch, PRF_NOTELL))
		send_to_char("�� �� ������ ������ � ������ ���������.\r\n", ch);
	else if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF))
		send_to_char("����� ��������� ��� ����.\r\n", ch);
	else if (!IS_NPC(vict) && !vict->desc)	// linkless
		act("$N ������� �����.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else if (PLR_FLAGGED(vict, PLR_WRITING))
		act("$N ����� ������; �������� �����.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else
		perform_beep(ch, vict);
}


//Polos.insert_wanted_gem

void insert_wanted_gem::show(CHAR_DATA *ch, int gem_vnum)
{
	map< int, alias_type >::iterator it;
	alias_type::iterator alias_it;
	char buf[MAX_INPUT_LENGTH];

	it = content.find(gem_vnum);
	if (it == content.end()) return;

	send_to_char("������ �������� ��������, �� ������ �������, ������ ������� �� ������� ��������: \r\n", ch);
	for (alias_it = it->second.begin();alias_it != it->second.end();++alias_it)
	{
		sprintf(buf, " %s\r\n", alias_it->first.c_str());
		send_to_char(buf, ch);
	}
}

void insert_wanted_gem::init()
{
	ifstream file;
	char dummy;
	char buf[MAX_INPUT_LENGTH];
	string str;
	int val, val2, curr_val = 0;
	map< int, alias_type >::iterator it;
	alias_type temp;
	alias_type::iterator alias_it;
	struct int3 arr;

	content.clear();
	temp.clear();

	file.open(LIB_MISC "insert_wanted.lst", fstream::in);
	if (!file.is_open()) return log("failed to open insert_wanted.lst.");

	file.width(MAX_INPUT_LENGTH);

	while (1)
	{
		if (!(file >> dummy)) break;

		if (dummy == '*')
		{
			if (!file.getline(buf, MAX_INPUT_LENGTH)) break;
			continue;
		}

		if (dummy == '#')
		{
			if (!(file >> val)) break;

			if (!temp.empty() && (curr_val != 0))
			{
				content.insert(std::make_pair(curr_val, temp));
				temp.clear();
			}
			curr_val = val;

			continue;
		}

		if (dummy == '$')
		{
			if (curr_val == 0) break;
			if (!(file >> str))  break;
			if (str.size() > MAX_ALIAS_LENGTH - 1) break;
			if (!(file >> val))  break;
			if (curr_val == 0) break;

			switch (val)
			{
			case 1:
				if (!(file >> val >> val2)) break;

				arr.type = 1;
				arr.bit = val;
				arr.qty = val2;
				temp.insert(std::make_pair(str, arr));

				break;

			case 2:
			case 3:
				if (!(file >> val2))  break;

				arr.type = val;
				arr.bit = val2;
				arr.qty = 0;
				temp.insert(std::make_pair(str, arr));

				break;
			default:
			{
				log("something goes wrong\r\nclosed insert_wanted.lst.");
				file.close();
				return;
			}
			};

		}

	}

	file.close();
	log("closed insert_wanted.lst.");

	if (!temp.empty())
	{
		content.insert(std::make_pair(curr_val, temp));
	}

	return;
}

int insert_wanted_gem::get_type(int gem_vnum, string str)
{
	return content[gem_vnum][str].type;
}

int insert_wanted_gem::get_bit(int gem_vnum, string str)
{
	return content[gem_vnum][str].bit;
}

int insert_wanted_gem::get_qty(int gem_vnum, string str)
{
	return content[gem_vnum][str].qty;
}

int insert_wanted_gem::exist(int gem_vnum, string str)
{
	map< int, alias_type >::iterator it;
	alias_type::iterator alias_it;

	it = content.find(gem_vnum);
	if (it == content.end()) return 0;
	alias_it = content[gem_vnum].find(str);
	if (alias_it == content[gem_vnum].end()) return 0;

	return 1;
}

//-Polos.insert_wanted_gem


int make_hole(CHAR_DATA *ch)
{
	if (roundup(world[ch->in_room]->holes / HOLES_TIME) >= dig_vars.hole_max_deep)
	{
		send_to_char("��� � ��� ��� ����������.\r\n", ch);
		return 0;
	}


	return 1;
}

void break_inst(CHAR_DATA *ch)
{
	int i;
	char buf[300];

	for (i = WEAR_WIELD; i <= WEAR_BOTHS; i++)
	{
		if (GET_EQ(ch, i) && (strstr(GET_EQ(ch, i)->aliases, "������") || strstr(GET_EQ(ch, i)->aliases, "�����")))
		{
			if (GET_OBJ_CUR(GET_EQ(ch, i)) > 1)
			{
				if (number(1, dig_vars.instr_crash_chance) == 1)
				{
					GET_OBJ_CUR(GET_EQ(ch, i)) -= 1;
				}
			}
			else
			{
				GET_EQ(ch, i)->set_timer(0);
			}
			if (GET_OBJ_CUR(GET_EQ(ch, i)) <= 1 && number(1, 3) == 1)
			{
				sprintf(buf, "���� %s ����������!\r\n", GET_EQ(ch, i)->short_description);
				send_to_char(buf, ch);
			}
		}
	}

}

int check_for_dig(CHAR_DATA *ch)
{
	int i;

	for (i = WEAR_WIELD; i <= WEAR_BOTHS; i++)
	{
		if (GET_EQ(ch, i) && (strstr(GET_EQ(ch, i)->aliases, "������") || strstr(GET_EQ(ch, i)->aliases, "�����")))
		{
			return 1;
		}
	}
	return 0;
}

int check_for_fish(CHAR_DATA *ch)
{
	int i;

	for (i = WEAR_WIELD; i <= WEAR_BOTHS; i++)
	{
		if (GET_EQ(ch, i) && (strstr(GET_EQ(ch, i)->aliases, "������") ))
		{
			return 1;
		}
	}
	return 0;
}

void dig_obj(CHAR_DATA *ch, struct obj_data *obj)
{
	char textbuf[300];

	if (GET_OBJ_MIW(obj) >=
			obj_index[GET_OBJ_RNUM(obj)].stored + obj_index[GET_OBJ_RNUM(obj)].number || GET_OBJ_MIW(obj) == -1)
	{
		sprintf(textbuf, "�� ����� %s!\r\n", obj->PNames[3]);
		send_to_char(textbuf, ch);
		sprintf(textbuf, "$n �������$g %s!\r\n", obj->PNames[3]);
		act(textbuf, FALSE, ch, 0, 0, TO_ROOM);
		if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
		{
			send_to_char("�� �� ������ ������ ������� ���������.\r\n", ch);
			obj_to_room(obj, ch->in_room);
		}
		else if (IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj) > CAN_CARRY_W(ch))
		{
			send_to_char("�� �� ������ ������ ����� ��c.\r\n", ch);
			obj_to_room(obj, ch->in_room);
		}
		else
			obj_to_char(obj, ch);
	}
}

void fish_obj(CHAR_DATA *ch, struct obj_data *obj) // by prool, from text of dig_obj()
{
	char textbuf[300];

	if (GET_OBJ_MIW(obj) >=
			obj_index[GET_OBJ_RNUM(obj)].stored + obj_index[GET_OBJ_RNUM(obj)].number || GET_OBJ_MIW(obj) == -1)
	{
		sprintf(textbuf, "�� ������� %s!\r\n", obj->PNames[3]);
		send_to_char(textbuf, ch);
		sprintf(textbuf, "$n ������$g %s!\r\n", obj->PNames[3]);
		act(textbuf, FALSE, ch, 0, 0, TO_ROOM);
		if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
		{
			send_to_char("�� �� ������ ������ ������� ���������.\r\n", ch);
			obj_to_room(obj, ch->in_room);
		}
		else if (IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj) > CAN_CARRY_W(ch))
		{
			send_to_char("�� �� ������ ������ ����� ��c.\r\n", ch);
			obj_to_room(obj, ch->in_room);
		}
		else
			obj_to_char(obj, ch);
	}
}

ACMD(do_dig)
{
	CHAR_DATA *mob;
	struct obj_data *obj;
	char textbuf[300];
	int percent, prob;
	int stone_num, random_stone;
	int vnum;
	int old_wis, old_int;
	int i;

	if (IS_NPC(ch) || !ch->get_skill(SKILL_DIG))
	{
		send_to_char("�� �� �� ������ ���.\r\n", ch);
		return;
	}

	if (!check_for_dig(ch) && !IS_IMMORTAL(ch))
	{
		send_to_char("��� �� ������ ����� � ����... ��� �����...\r\n", ch);
		return;
	}


	if (world[IN_ROOM(ch)]->sector_type != SECT_MOUNTAIN &&
			world[IN_ROOM(ch)]->sector_type != SECT_HILLS && !IS_IMMORTAL(ch))
	{
		send_to_char("�������� �������� ������� ������ � �������� ���������!\r\n", ch);
		return;
	}

	if (!WAITLESS(ch) && on_horse(ch))
	{
		send_to_char("������ ��� ������� ��������������.\r\n", ch);
		return;
	}

	if (AFF_FLAGGED(ch, AFF_BLIND) && !IS_IMMORTAL(ch))
	{
		send_to_char("�� ����� � �� ������ ��� ������.\r\n", ch);
		return;
	}

	if (IS_DARK(IN_ROOM(ch)) && !CAN_SEE_IN_DARK(ch) && !IS_IMMORTAL(ch))
	{
		send_to_char("���� ������? ���� ������? ������ �� �����...\r\n", ch);
		return;
	}

	if (!make_hole(ch) && !IS_IMMORTAL(ch))
		return;

	if (!check_moves(ch, dig_vars.need_moves))
		return;

	world[ch->in_room]->holes += HOLES_TIME;

	send_to_char("�� ����� ������� �������� ���������� �����...\r\n", ch);
	act("$n ����$g ������� �������� ���������� �����...", FALSE, ch, 0, 0, TO_ROOM);

	break_inst(ch);

	// ��������� �������
if (number(1,20)==1)
	{
	send_to_char("�� �������� ������������ � ������ �� ����.\r\n", ch);
	GET_HIT(ch)=GET_HIT(ch)-1;
	return;
	}

if (number(1,30)==1)
	{
	send_to_char("�� �������� � ������ ���������� ����������\r\n",ch);
	for (i = WEAR_WIELD; i <= WEAR_BOTHS; i++)
		{
		if (GET_EQ(ch, i) && (strstr(GET_EQ(ch, i)->aliases, "�����") ))
			{
			GET_OBJ_CUR(GET_EQ(ch,i))--;
			}
		if (GET_EQ(ch, i) && (strstr(GET_EQ(ch, i)->aliases, "������") ))
			{
			GET_OBJ_CUR(GET_EQ(ch,i))--;
			}
		}
	return;
	}

	if (number(1, dig_vars.treasure_chance) == 1)	// ������� ����
	{
		int gold = number(40000, 60000);
		send_to_char("�� ����� ����!\r\n", ch);
		act("$n �������$g ����!", FALSE, ch, 0, 0, TO_ROOM);
		sprintf(textbuf, "�� ��������� %i �����.\r\n", gold);
		send_to_char(textbuf, ch);
		ch->add_gold(gold);
		split_or_clan_tax(ch, gold);
		return;
	}

	if (number(1, dig_vars.mob_chance) == 1)	// ������� ��������
	{
		vnum = number(dig_vars.mob_vnum_start, dig_vars.mob_vnum_end);
		mob = read_mobile(real_mobile(vnum), REAL);
		if (mob)
		{
			if (GET_LEVEL(mob) <= GET_LEVEL(ch))
			{
				SET_BIT(MOB_FLAGS(mob, MOB_AGGRESSIVE), MOB_AGGRESSIVE);
				sprintf(textbuf, "�� �������� %s!\r\n", mob->player_data.PNames[3]);
				send_to_char(textbuf, ch);
				sprintf(textbuf, "$n �������$g %s!\r\n", mob->player_data.PNames[3]);
				act(textbuf, FALSE, ch, 0, 0, TO_ROOM);
				char_to_room(mob, ch->in_room);
				return;
			}
		}
		else
			send_to_char("�� ������ �������� �������!", ch);
	}
	if (number(1, dig_vars.pandora_chance) == 1)	// ������� �������� �������
	{
		vnum = dig_vars.pandora_vnum;
		obj = read_object(real_object(vnum), REAL);
		if (obj)
			dig_obj(ch, obj);
		else
			send_to_char("�� ������ �������� �������!", ch);
		return;
	}
	if (number(1, dig_vars.trash_chance) == 1)	// ������� �����
	{
		vnum = number(dig_vars.trash_vnum_start, dig_vars.trash_vnum_end);
		obj = read_object(real_object(vnum), REAL);
		if (obj)
			dig_obj(ch, obj);
		else
			send_to_char("�� ������ �������� �������!", ch);
		return;
	}

	percent = number(1, skill_info[SKILL_DIG].max_percent);
	prob = ch->get_skill(SKILL_DIG);
	old_int = ch->get_int();
	old_wis = ch->get_wis();
	ch->set_int(ch->get_int() + 14 - MAX(14, GET_REAL_INT(ch)));
	ch->set_wis(ch->get_wis() + 14 - MAX(14, GET_REAL_WIS(ch)));
	improove_skill(ch, SKILL_DIG, 0, 0);
	ch->set_int(old_int);
	ch->set_wis(old_wis);


	WAIT_STATE(ch, dig_vars.lag * PULSE_VIOLENCE);



	if (percent > prob / dig_vars.prob_divide)
	{
		send_to_char("�� ������ ��� ����������� ����� � ��������� �����.\r\n", ch);
		act("$n �����$g ������� ����.", FALSE, ch, 0, 0, TO_ROOM);
		return;
	}


	// ����������� ������ ������ ����� ������� �� ������

	random_stone = number(1, MIN(prob, 100));
	if (random_stone >= dig_vars.stone9_skill)
		stone_num = 9;
	else if (random_stone >= dig_vars.stone8_skill)
		stone_num = 8;
	else if (random_stone >= dig_vars.stone7_skill)
		stone_num = 7;
	else if (random_stone >= dig_vars.stone6_skill)
		stone_num = 6;
	else if (random_stone >= dig_vars.stone5_skill)
		stone_num = 5;
	else if (random_stone >= dig_vars.stone4_skill)
		stone_num = 4;
	else if (random_stone >= dig_vars.stone3_skill)
		stone_num = 3;
	else if (random_stone >= dig_vars.stone2_skill)
		stone_num = 2;
	else if (random_stone >= dig_vars.stone1_skill)
		stone_num = 1;
	else
		stone_num = 0;

	if (stone_num == 0)
	{
		send_to_char("�� ����� ������, �� ��� � �� ����� ������ ���������.\r\n", ch);
		act("$n ����� �����$g �����, �� ��� ��� �����.", FALSE, ch, 0, 0, TO_ROOM);
		return;
	}

	vnum = dig_vars.stone1_vnum - 1 + stone_num;
	obj = read_object(real_object(vnum), REAL);
	if (obj)
	{
		if (number(1, dig_vars.glass_chance) != 1)
			GET_OBJ_MATER(obj) = 11;
		else
			GET_OBJ_MATER(obj) = 18;

		dig_obj(ch, obj);
	}
	else
		send_to_char("�� ������ �������� �������!", ch);
}

#define FISH 410
#define WATER_SPIRIT 400

ACMD(do_fish) // by prool
{
struct obj_data *obj;
CHAR_DATA *mob;
char textbuf[300];
int i;

	if (!check_for_fish(ch) /*&& !IS_IMMORTAL(ch)*/)
	{
		send_to_char("������ ���� ����� ������ � ����! ����� ���� ����� ������ � �����!\r\n", ch);
		return;
	}

	if (world[IN_ROOM(ch)]->sector_type != SECT_WATER_SWIM &&
			world[IN_ROOM(ch)]->sector_type != SECT_WATER_NOSWIM /*&& !IS_IMMORTAL(ch)*/)
	{
		send_to_char("������ ���� ����� ������ � ����! ����� ���� ����� ������ � �����!\r\n", ch);
		return;
	}

send_to_char("�� ��������� ������.\r\n", ch);

if (number(1,20)==1)
	{
	send_to_char("�� ����� �������� � ���������� �������.\r\n", ch);
	GET_HIT(ch)=GET_HIT(ch)-1;
	return;
	}

	if (number(1, 30) == 1)	// ����������� ������� ����
	{
	send_to_char("�� ����� ��������: �� ����������� ����� �������� ����, ������������ ������� ����\r\n", ch);
		mob = read_mobile(real_mobile(WATER_SPIRIT), REAL);
		if (mob)
		{
			if (GET_LEVEL(mob) <= GET_LEVEL(ch))
			{
				SET_BIT(MOB_FLAGS(mob, MOB_AGGRESSIVE), MOB_AGGRESSIVE);
				sprintf(textbuf, "�� ������� %s!\r\n", mob->player_data.PNames[3]);
				send_to_char(textbuf, ch);
				sprintf(textbuf, "$n ������$g %s!\r\n", mob->player_data.PNames[3]);
				act(textbuf, FALSE, ch, 0, 0, TO_ROOM);
				char_to_room(mob, ch->in_room);
				return;
			}
		}
		else
			send_to_char("Mob error: �� ������ ��������", ch);
	} // end of ����������� ������� ����

if (number(1,30)==1)
	{
	send_to_char("�� �������� � ������ ���������� ������\r\n",ch);
	for (i = WEAR_WIELD; i <= WEAR_BOTHS; i++)
		{
		if (GET_EQ(ch, i) && (strstr(GET_EQ(ch, i)->aliases, "������") ))
			{
			GET_OBJ_CUR(GET_EQ(ch,i))--;
			}
		}
	return;
	}

if (number(1,10)!=1)
	{
	send_to_char("�� ������ �� �������!\r\n", ch);
	return;
	}
obj = read_object(real_object(FISH), REAL);
if (obj)
	{
	fish_obj(ch, obj);
	}
else
		send_to_char("Object error: �� ������ ��������!", ch);
}

void set_obj_aff(struct obj_data *itemobj, int bitv)
{
	int i;

	for (i = 0; weapon_affect[i].aff_bitvector != -1; i++)
	{
		if (weapon_affect[i].aff_bitvector == bitv)
		{
			SET_BIT(GET_OBJ_AFF(itemobj, weapon_affect[i].aff_pos), weapon_affect[i].aff_pos);
		}
	}
}

void set_obj_eff(struct obj_data *itemobj, int type, int mod)
{
	int i;

	for (i = 0; i < MAX_OBJ_AFFECT; i++)
		if (itemobj->affected[i].location == type)
		{
			(itemobj->affected[i].modifier) += mod;
			break;
		}
		else if (itemobj->affected[i].location == APPLY_NONE)
		{
			itemobj->affected[i].location = type;
			itemobj->affected[i].modifier = mod;
			break;
		}

}

extern struct index_data *obj_index;

bool is_dig_stone(OBJ_DATA *obj)
{
	if ((GET_OBJ_VNUM(obj) >= dig_vars.stone1_vnum
			&& GET_OBJ_VNUM(obj) <= dig_vars.stone1_vnum + 17)
		|| GET_OBJ_VNUM(obj) == DIG_GLASS_VNUM)
	{
		return true;
	}

	return false;
}

ACMD(do_insertgem)
{
	int percent, prob, i;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[300];
	char *gem, *item;
	struct obj_data *gemobj, *itemobj;

	argument = two_arguments(argument, arg1, arg2);

	if (IS_NPC(ch) || !ch->get_skill(SKILL_INSERTGEM))
	{
		send_to_char("�� �� �� ������ ���.\r\n", ch);
		return;
	}

	if (!IS_IMMORTAL(ch))
	{
		if (!ROOM_FLAGGED(IN_ROOM(ch), ROOM_SMITH))
		{
			send_to_char("��� ����� ������� � ������� ��� �����.\r\n", ch);
			return;
		}
	}

	if (!*arg1)
	{
		send_to_char("�������� ���?\r\n", ch);
		return;
	}
	else
		gem = arg1;

	if (!(gemobj = get_obj_in_list_vis(ch, gem, ch->carrying)))
	{
		sprintf(buf, "� ��� ��� '%s'.\r\n", gem);
		send_to_char(buf, ch);
		return;
	}

	if (!is_dig_stone(gemobj))
	{
		sprintf(buf, "�� �� ������ ��������� %s.\r\n", gemobj->PNames[3]);
		send_to_char(buf, ch);
		return;
	}

	if (!*arg2)
	{
		send_to_char("�������� ����?\r\n", ch);
		return;
	}
	else
		item = arg2;

	if (!(itemobj = get_obj_in_list_vis(ch, item, ch->carrying)))
	{
		sprintf(buf, "� ��� ��� '%s'.\r\n", item);
		send_to_char(buf, ch);
		return;
	}
	if (GET_OBJ_MATER(itemobj) < 1 || (GET_OBJ_MATER(itemobj) > 6 && GET_OBJ_MATER(itemobj) != 13))
	{
		sprintf(buf, "%s ������� �� ������������� ���������.\r\n", itemobj->PNames[0]);
		send_to_char(buf, ch);
		return;
	}
	/*(!(CAN_WEAR(itemobj, ITEM_WEAR_BODY)) &&
	   !(CAN_WEAR(itemobj, ITEM_WEAR_HEAD)) &&
	   !(CAN_WEAR(itemobj, ITEM_WEAR_LEGS)) &&
	   !(CAN_WEAR(itemobj, ITEM_WEAR_ARMS)) &&
	   !(CAN_WEAR(itemobj, ITEM_WEAR_SHIELD)) &&
	   !(CAN_WEAR(itemobj, ITEM_WEAR_WIELD)) &&
	   !(CAN_WEAR(itemobj, ITEM_WEAR_HOLD)) &&
	   !(CAN_WEAR(itemobj, ITEM_WEAR_BOTHS)))
	   {
	   send_to_char("��� ������ ��������� ������ � ������� ����� ����.\r\n", ch);
	   return;
	   } */

//    if (GET_OBJ_OWNER (itemobj) == GET_UNIQUE (ch))
//    {
//    if ((!(CAN_WEAR(itemobj, ITEM_WEAR_BODY)) && OBJ_FLAGGED(itemobj, ITEM_WITH2SLOTS)) ||
//      (OBJ_FLAGGED(itemobj, ITEM_WITH3SLOTS)))
//       {
//      send_to_char("� ���� ������� ��������� ������������ ���������� ������.\r\n", ch);
//        return;
//       }
//    }
//    else
//    {
//    if (((CAN_WEAR(itemobj, ITEM_WEAR_BODY) ||
//        CAN_WEAR(itemobj, ITEM_WEAR_SHIELD)) && OBJ_FLAGGED(itemobj, ITEM_WITH2SLOTS)))
//       {
//      send_to_char("� ���� ������� ��������� ������������ ���������� ������.\r\n", ch);
//        return;
//       }
	if (!OBJ_FLAGGED(itemobj, ITEM_WITH1SLOT) && !OBJ_FLAGGED(itemobj, ITEM_WITH2SLOTS)
			&& !OBJ_FLAGGED(itemobj, ITEM_WITH3SLOTS))
	{
		send_to_char("�� �� ������ ���� ����� ����� �������� ������.\r\n", ch);
		return;
	}
//    }

	if (!WAITLESS(ch) && on_horse(ch))
	{
		send_to_char("������ ��� ������� ��������������.\r\n", ch);
		return;
	}

	if (AFF_FLAGGED(ch, AFF_BLIND))
	{
		send_to_char("�� �����!\r\n", ch);
		return;
	}

	if (IS_DARK(IN_ROOM(ch)) && !CAN_SEE_IN_DARK(ch) && !IS_IMMORTAL(ch))
	{
		send_to_char("�� ��� ����� ���� ����� ������...\r\n", ch);
		return;
	}

	percent = number(1, skill_info[SKILL_INSERTGEM].max_percent);
	prob = ch->get_skill(SKILL_INSERTGEM);

	WAIT_STATE(ch, PULSE_VIOLENCE);

	for (i = 0; i < MAX_OBJ_AFFECT; i++)
		if (itemobj->affected[i].location == APPLY_NONE)
		{
			prob -= i * insgem_vars.minus_for_affect;
			break;
		}

	for (i = 0; weapon_affect[i].aff_bitvector != -1; i++)
	{
		if (IS_SET(GET_OBJ_AFF(itemobj, weapon_affect[i].aff_pos), weapon_affect[i].aff_pos))
		{
			prob -= insgem_vars.minus_for_affect;
		}
	}

//Polos.insert_wanted_gem

	argument = one_argument(argument, arg3);

	if (!*arg3)
	{
//-Polos.insert_wanted_gem
		improove_skill(ch, SKILL_INSERTGEM, 0, 0);

		if (percent > prob / insgem_vars.prob_divide)
		{
			sprintf(buf, "�� �������� ���������� �������� %s � %s, �������� ������...\r\n", gemobj->short_description,
					itemobj->PNames[3]);
			send_to_char(buf, ch);
			sprintf(buf, "$n ��������$g %s, �������� ��� � %s!\r\n", gemobj->PNames[3], itemobj->PNames[3]);
			act(buf, FALSE, ch, 0, 0, TO_ROOM);
			extract_obj(gemobj);
			if (number(1, 100) <= insgem_vars.dikey_percent)
			{
				sprintf(buf, "...� �������� ������� ����!\r\n");
				send_to_char(buf, ch);
				sprintf(buf, "$n ��������$g %s!\r\n", itemobj->PNames[3]);
				act(buf, FALSE, ch, 0, 0, TO_ROOM);
				extract_obj(itemobj);
			}
			return;
		}
//Polos.insert_wanted_gem
	}
	else
	{
		if (ch->get_skill(SKILL_INSERTGEM) < 80)
		{
			sprintf(buf, "�� ������ ���������� ���������� � ������ ������, ����� ��������� �������� �������!\r\n");
			send_to_char(buf, ch);
			return;

		}
		if (GET_OBJ_OWNER(itemobj) != GET_UNIQUE(ch))
		{
			sprintf(buf, "�� ������ ��������� �������� ������� ������ � ���������!\r\n");
			send_to_char(buf, ch);
			return;
		}

		string str(arg3);
		if (!iwg.exist(GET_OBJ_VNUM(gemobj), str))
		{
			iwg.show(ch, GET_OBJ_VNUM(gemobj));
			return;
		}

		improove_skill(ch, SKILL_INSERTGEM, 0, 0);

		//����� ��� ����? ��� 80% ����� ����� 30% ��� 100% ����� 50% ��� 200% ����� ����� 75%
		if (number(1, ch->get_skill(SKILL_INSERTGEM)) > (ch->get_skill(SKILL_INSERTGEM) - 50))
		{
			sprintf(buf, "�� �������� ���������� �������� %s � %s, �������� ������...\r\n", gemobj->short_description,
					itemobj->PNames[3]);
			send_to_char(buf, ch);
			sprintf(buf, "$n ��������$g %s, �������� ��� � %s!\r\n", gemobj->PNames[3], itemobj->PNames[3]);
			act(buf, FALSE, ch, 0, 0, TO_ROOM);
			extract_obj(gemobj);
			return;
		}

	}
//-Polos.insert_wanted_gem

	sprintf(buf, "�� �������� %s � %s!\r\n", gemobj->PNames[3], itemobj->PNames[3]);
	send_to_char(buf, ch);
	sprintf(buf, "$n �������$g %s � %s.\r\n", gemobj->PNames[3], itemobj->PNames[3]);
	act(buf, FALSE, ch, 0, 0, TO_ROOM);

	if (GET_OBJ_OWNER(itemobj) == GET_UNIQUE(ch))
	{
		int timer = itemobj->get_timer() + itemobj->get_timer() / 100 * insgem_vars.timer_plus_percent;
		itemobj->set_timer(timer);
	}
	else
	{
		int timer = itemobj->get_timer() - itemobj->get_timer() / 100 * insgem_vars.timer_minus_percent;
		itemobj->set_timer(timer);
	}

	if (GET_OBJ_MATER(gemobj) == 18)
	{
//Polos.insert_wanted_gem
		if (!*arg3)
		{
//-Polos.insert_wanted_gem
			if (GET_OBJ_VNUM(gemobj) == dig_vars.stone1_vnum)
				switch (number(1, 6))
				{
				case 1:
					set_obj_aff(itemobj, AFF_DETECT_INVIS);
					break;
				case 2:
					set_obj_aff(itemobj, AFF_DETECT_MAGIC);
					break;
				case 3:
					set_obj_aff(itemobj, AFF_DETECT_ALIGN);
					break;
				case 4:
					set_obj_aff(itemobj, AFF_BLESS);
					break;
				case 5:
					set_obj_aff(itemobj, AFF_HASTE);
					break;

				case 6:
					set_obj_eff(itemobj, APPLY_HITROLL, -1);
					break;

				}
			if (GET_OBJ_VNUM(gemobj) == dig_vars.stone1_vnum + 1)
				switch (number(1, 6))
				{
				case 1:
					set_obj_eff(itemobj, APPLY_HITROLL, 1);
					break;
				case 2:
					set_obj_eff(itemobj, APPLY_AC, -20);
					set_obj_eff(itemobj, APPLY_SAVING_REFLEX, -5);
					set_obj_eff(itemobj, APPLY_SAVING_STABILITY, -5);
					break;
				case 3:
					set_obj_aff(itemobj, AFF_WATERWALK);
					break;
				case 4:
					set_obj_aff(itemobj, AFF_SINGLELIGHT);
					break;
				case 5:
					set_obj_aff(itemobj, AFF_INFRAVISION);
					break;
				case 6:
					set_obj_aff(itemobj, AFF_CURSE);
					break;

				}
			if (GET_OBJ_VNUM(gemobj) == dig_vars.stone1_vnum + 2)
				switch (number(1, 6))
				{
				case 1:
					set_obj_eff(itemobj, APPLY_HIT, 20);
					break;
				case 2:
					set_obj_eff(itemobj, APPLY_MOVE, 20);
					break;
				case 3:
					set_obj_aff(itemobj, AFF_PROTECT_EVIL);
					break;
				case 4:
					set_obj_aff(itemobj, AFF_PROTECT_GOOD);
					break;
				case 5:
					set_obj_aff(itemobj, AFF_AWARNESS);
					break;
				case 6:
					set_obj_eff(itemobj, APPLY_MOVEREG, -5);
					break;

				}
			if (GET_OBJ_VNUM(gemobj) == dig_vars.stone1_vnum + 3)
				switch (number(1, 6))
				{
				case 1:
					set_obj_eff(itemobj, APPLY_SAVING_REFLEX, -10);
					break;
				case 2:
					set_obj_eff(itemobj, APPLY_HITREG, 10);
					break;
				case 3:
					set_obj_aff(itemobj, AFF_HOLYDARK);
					break;
				case 4:
					if (!CAN_WEAR(itemobj, ITEM_WEAR_WIELD) &&
							!CAN_WEAR(itemobj, ITEM_WEAR_HOLD) && !CAN_WEAR(itemobj, ITEM_WEAR_BOTHS))
						set_obj_eff(itemobj, APPLY_SIZE, 7);
					else
						set_obj_eff(itemobj, APPLY_MORALE, 3);
					break;

				case 5:
					set_obj_eff(itemobj, APPLY_MORALE, 3);
					break;

				case 6:
					set_obj_eff(itemobj, APPLY_HITREG, -5);
					break;
				}
			if (GET_OBJ_VNUM(gemobj) == dig_vars.stone1_vnum + 4)
				switch (number(1, 6))
				{
				case 1:
					set_obj_eff(itemobj, APPLY_SAVING_STABILITY, -10);
					break;
				case 2:
					set_obj_eff(itemobj, APPLY_SAVING_CRITICAL, -10);
					break;
				case 3:
					set_obj_eff(itemobj, APPLY_RESIST_AIR, 15);
					break;
				case 4:
					set_obj_eff(itemobj, APPLY_RESIST_WATER, 15);
					break;
				case 5:
					set_obj_eff(itemobj, APPLY_RESIST_FIRE, 15);
					break;
				case 6:
					set_obj_eff(itemobj, APPLY_SAVING_CRITICAL, 10);
					break;
				}
			if (GET_OBJ_VNUM(gemobj) == dig_vars.stone1_vnum + 5)
				switch (number(1, 6))
				{
				case 1:
					set_obj_aff(itemobj, AFF_SANCTUARY);
					break;
				case 2:
					set_obj_aff(itemobj, AFF_BLINK);
					break;
				case 3:
					if (!CAN_WEAR(itemobj, ITEM_WEAR_WIELD) &&
							!CAN_WEAR(itemobj, ITEM_WEAR_HOLD) && !CAN_WEAR(itemobj, ITEM_WEAR_BOTHS))
						set_obj_eff(itemobj, APPLY_ABSORBE, 5);
					else
						set_obj_eff(itemobj, APPLY_HITREG, 25);
					break;
				case 4:
					set_obj_aff(itemobj, AFF_FLY);
					break;
				case 5:
					set_obj_eff(itemobj, APPLY_MANAREG, 10);
					break;
				case 6:
					set_obj_eff(itemobj, APPLY_SAVING_STABILITY, -10);
					break;
				}
			if (GET_OBJ_VNUM(gemobj) == dig_vars.stone1_vnum + 6)
				switch (number(1, 6))
				{
				case 1:
					set_obj_eff(itemobj, APPLY_DAMROLL, 3);
					break;
				case 2:
					set_obj_eff(itemobj, APPLY_HITROLL, 3);
					break;
				case 3:
					if ((CAN_WEAR(itemobj, ITEM_WEAR_WIELD) ||
							CAN_WEAR(itemobj, ITEM_WEAR_HOLD) || CAN_WEAR(itemobj, ITEM_WEAR_BOTHS))
							&& !OBJ_FLAGGED(itemobj, ITEM_NODISARM))
						SET_BIT(GET_OBJ_EXTRA(itemobj, ITEM_NODISARM), ITEM_NODISARM);
					else
						set_obj_eff(itemobj, APPLY_HITROLL, 3);
					break;
				case 4:
					set_obj_eff(itemobj, APPLY_SAVING_WILL, -10);
					break;
				case 5:
					set_obj_aff(itemobj, AFF_INVISIBLE);
					break;
				case 6:
					set_obj_eff(itemobj, APPLY_SAVING_WILL, 10);
					break;
				}
			if (GET_OBJ_VNUM(gemobj) == dig_vars.stone1_vnum + 7)
				switch (number(1, 6))
				{
				case 1:
					set_obj_eff(itemobj, APPLY_INT, 1);
					break;
				case 2:
					set_obj_eff(itemobj, APPLY_WIS, 1);
					break;
				case 3:
					set_obj_eff(itemobj, APPLY_DEX, 1);
					break;
				case 4:
					set_obj_eff(itemobj, APPLY_STR, 1);
					break;
				case 5:
					set_obj_eff(itemobj, APPLY_CON, 1);
					break;
				case 6:
					set_obj_eff(itemobj, APPLY_CHA, 1);
					break;
				}
			if (GET_OBJ_VNUM(gemobj) == dig_vars.stone1_vnum + 8)
				switch (number(1, 6))
				{
				case 1:
					set_obj_eff(itemobj, APPLY_INT, 2);
					break;
				case 2:
					set_obj_eff(itemobj, APPLY_WIS, 2);
					break;
				case 3:
					set_obj_eff(itemobj, APPLY_DEX, 2);
					break;
				case 4:
					set_obj_eff(itemobj, APPLY_STR, 2);
					break;
				case 5:
					set_obj_eff(itemobj, APPLY_CON, 2);
					break;
				case 6:
					set_obj_eff(itemobj, APPLY_CHA, 2);
					break;
				}
//Polos.insert_wanted_gem
		}
		else
		{
			int tmp_type, tmp_bit, tmp_qty;
			string str(arg3);

			tmp_type = iwg.get_type(GET_OBJ_VNUM(gemobj), str);

			tmp_bit = iwg.get_bit(GET_OBJ_VNUM(gemobj), str);
			tmp_qty = iwg.get_qty(GET_OBJ_VNUM(gemobj), str);
			switch (tmp_type)
			{
			case 1:
				set_obj_eff(itemobj, tmp_bit, tmp_qty);
				break;

			case 2:
				set_obj_aff(itemobj, tmp_bit);
				break;

			case 3:
				SET_BIT(GET_OBJ_EXTRA(itemobj, tmp_bit), tmp_bit);
				break;

			default:
				break;

			};
		}
//-Polos.insert_wanted_gem

		// �����, ������������, ������� �������� ��������� ������
		if (OBJ_FLAGGED(itemobj, ITEM_WITH3SLOTS))
		{
			REMOVE_BIT(GET_OBJ_EXTRA(itemobj, ITEM_WITH3SLOTS), ITEM_WITH3SLOTS);
			SET_BIT(GET_OBJ_EXTRA(itemobj, ITEM_WITH2SLOTS), ITEM_WITH2SLOTS);
		}
		else if (OBJ_FLAGGED(itemobj, ITEM_WITH2SLOTS))
		{
			REMOVE_BIT(GET_OBJ_EXTRA(itemobj, ITEM_WITH2SLOTS), ITEM_WITH2SLOTS);
			SET_BIT(GET_OBJ_EXTRA(itemobj, ITEM_WITH1SLOT), ITEM_WITH1SLOT);
		}
		else if (OBJ_FLAGGED(itemobj, ITEM_WITH1SLOT))
		{
			REMOVE_BIT(GET_OBJ_EXTRA(itemobj, ITEM_WITH1SLOT), ITEM_WITH1SLOT);
		}
	}
	extract_obj(gemobj);
}

ACMD(do_bandage)
{
	if (IS_NPC(ch))
	{
		return;
	}
	if (GET_HIT(ch) == GET_REAL_MAX_HIT(ch))
	{
		send_to_char("�� �� ���������� � ���������!\r\n", ch);
		return;
	}
	if (ch->get_fighting())
	{
		send_to_char("�� �� ������ ������������ ���� �� ����� ���!\r\n", ch);
		return;
	}
	if (AFF_FLAGGED(ch, AFF_BANDAGE))
	{
		send_to_char("�� � ��� ��� ����������� ����������!\r\n", ch);
		return;
	}
	if (AFF_FLAGGED(ch, AFF_NO_BANDAGE))
	{
		send_to_char("�� �� ������ ������������ ���� ���� ���� ���� � ������!\r\n", ch);
		return;
	}

	OBJ_DATA *bandage = 0;
	for (OBJ_DATA *i = ch->carrying; i ; i = i->next_content)
	{
		if (GET_OBJ_TYPE(i) == ITEM_BANDAGE)
		{
			bandage = i;
			break;
		}
	}
	if (!bandage || GET_OBJ_WEIGHT(bandage) <= 0)
	{
		send_to_char("� ����� ��������� ��� ���������� ��� ��������� ������!\r\n", ch);
		return;
	}

	send_to_char("�� ������� ����� � ������ ��������� ���� ������ ������...\r\n", ch);
	act("$n �����$g ������������ ���� ����.&n", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);

	AFFECT_DATA af;
	af.type = SPELL_BANDAGE;
	af.location = APPLY_NONE;
	af.modifier = GET_OBJ_VAL(bandage, 0);
	af.duration = pc_duration(ch, 10, 0, 0, 0, 0);
	af.bitvector = AFF_BANDAGE;
	af.battleflag = AF_PULSEDEC;
	affect_join(ch, &af, 0, 0, 0, 0);

	af.type = SPELL_NO_BANDAGE;
	af.location = APPLY_NONE;
	af.duration = pc_duration(ch, 60, 0, 0, 0, 0);
	af.bitvector = AFF_NO_BANDAGE;
	af.battleflag = AF_PULSEDEC;
	affect_join(ch, &af, 0, 0, 0, 0);

	GET_OBJ_WEIGHT(bandage) -= 1;
	IS_CARRYING_W(ch) -= 1;
	if (GET_OBJ_WEIGHT(bandage) <= 0)
	{
		send_to_char("��������� ����� ������ ������� � �����.\r\n", ch);
		extract_obj(bandage);
	}
}
