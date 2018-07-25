/* ************************************************************************
*   File: act.movement.cpp                              Part of Bylins    *
*  Usage: movement commands, door handling, & sleep/rest/etc state        *
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
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "skills.h"
#include "house.h"
#include "constants.h"
#include "dg_scripts.h"
#include "screen.h"
#include "pk.h"
#include "features.hpp"
#include "deathtrap.hpp"
#include "privilege.hpp"
#include "char.hpp"
#include "corpse.hpp"
#include "room.hpp"
#include "named_stuff.hpp"
#include "fight.h"

// external functs
void death_cry(CHAR_DATA * ch);
void set_wait(CHAR_DATA * ch, int waittime, int victim_in_room);
int find_eq_pos(CHAR_DATA * ch, OBJ_DATA * obj, char *arg);
int attack_best(CHAR_DATA * ch, CHAR_DATA * victim);
int awake_others(CHAR_DATA * ch);
void affect_from_char(CHAR_DATA * ch, int type);
void do_aggressive_room(CHAR_DATA * ch, int check_sneak);
void die(CHAR_DATA * ch, CHAR_DATA * killer);
// local functions
void check_ice(int room);
int has_boat(CHAR_DATA * ch);
int find_door(CHAR_DATA * ch, const char *type, char *dir, const char *cmdname);
int has_key(CHAR_DATA * ch, obj_vnum key);
void do_doorcmd(CHAR_DATA * ch, OBJ_DATA * obj, int door, int scmd);
int ok_pick(CHAR_DATA * ch, obj_vnum keynum, OBJ_DATA* obj, int door, int scmd);
extern int get_pick_chance(int skill_pick, int lock_complexity);

ACMD(do_gen_door);
ACMD(do_enter);
ACMD(do_stand);
ACMD(do_sit);
ACMD(do_rest);
ACMD(do_sleep);
ACMD(do_wake);
ACMD(do_follow);

const int Reverse[NUM_OF_DIRS] = { 2, 3, 0, 1, 5, 4 };
const char *DirIs[] =
{
	"�����",
	"������",
	"��",
	"�����",
	"�����",
	"����",
	"\n"
};

// check ice in room
int check_death_ice(int room, CHAR_DATA * ch)
{
	int sector, mass = 0, result = FALSE;
	CHAR_DATA *vict; // *next_vict

	if (room == NOWHERE)
		return (FALSE);
	sector = SECT(room);
	if (sector != SECT_WATER_SWIM && sector != SECT_WATER_NOSWIM)
		return (FALSE);
	if ((sector = real_sector(room)) != SECT_THIN_ICE && sector != SECT_NORMAL_ICE)
		return (FALSE);
	for (vict = world[room]->people; vict; vict = vict->next_in_room)
		if (!IS_NPC(vict) && !AFF_FLAGGED(vict, AFF_FLY))
			mass += GET_WEIGHT(vict) + IS_CARRYING_W(vict);
	if (!mass)
		return (FALSE);
	if ((sector == SECT_THIN_ICE && mass > 500) || (sector == SECT_NORMAL_ICE && mass > 1500))
	{
		act("��� ���������� ��� ����� ��������.", FALSE, world[room]->people, 0, 0, TO_ROOM);
		act("��� ���������� ��� ����� ��������.", FALSE, world[room]->people, 0, 0, TO_CHAR);
		world[room]->weather.icelevel = 0;
		world[room]->ices = 2;
		SET_BIT(ROOM_FLAGS(room, ROOM_ICEDEATH), ROOM_ICEDEATH);
		DeathTrap::add(world[room]);
	}
	else
		return (FALSE);

	return (result);
}

// simple function to determine if char can walk on water
int has_boat(CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	int i;

	//if (ROOM_IDENTITY(ch->in_room) == DEAD_SEA)
	//	return (1);

	if (IS_IMMORTAL(ch))
		return (TRUE);

	if (AFF_FLAGGED(ch, AFF_WATERWALK))
		return (TRUE);

	if (AFF_FLAGGED(ch, AFF_WATERBREATH))
		return (TRUE);

	if (AFF_FLAGGED(ch, AFF_FLY))
		return (TRUE);

	// non-wearable boats in inventory will do it
	for (obj = ch->carrying; obj; obj = obj->next_content)
		if (GET_OBJ_TYPE(obj) == ITEM_BOAT && (find_eq_pos(ch, obj, NULL) < 0))
			return (TRUE);

	// and any boat you're wearing will do it too
	for (i = 0; i < NUM_WEARS; i++)
		if (GET_EQ(ch, i) && GET_OBJ_TYPE(GET_EQ(ch, i)) == ITEM_BOAT)
			return (TRUE);

	return (FALSE);
}

void make_visible(CHAR_DATA * ch, int affect)
{
	char to_room[MAX_STRING_LENGTH], to_char[MAX_STRING_LENGTH];

	*to_room = *to_char = 0;

	switch (affect)
	{
	case AFF_HIDE:
		strcpy(to_char, "�� ���������� ���������.\r\n");
		strcpy(to_room, "$n ���������$g ���������.\r\n");
		break;
	case AFF_CAMOUFLAGE:
		strcpy(to_char, "�� ���������� �������������.\r\n");
		strcpy(to_room, "$n ���������$g �������������.\r\n");
		break;
	}
	REMOVE_BIT(AFF_FLAGS(ch, affect), affect);
	CHECK_AGRO(ch) = TRUE;
	if (*to_char)
		send_to_char(to_char, ch);
	if (*to_room)
		act(to_room, FALSE, ch, 0, 0, TO_ROOM);
}


int skip_hiding(CHAR_DATA * ch, CHAR_DATA * vict)
{
	int percent, prob;

	if (MAY_SEE(vict, ch) && (AFF_FLAGGED(ch, AFF_HIDE) || affected_by_spell(ch, SPELL_HIDE)))
	{
		if (awake_hide(ch))  	//if (affected_by_spell(ch, SPELL_HIDE))
		{
			send_to_char("�� ���������� ����������, �� ���� ���������� ������ ���.\r\n", ch);
			affect_from_char(ch, SPELL_HIDE);
			make_visible(ch, AFF_HIDE);
			SET_BIT(EXTRA_FLAGS(ch, EXTRA_FAILHIDE), EXTRA_FAILHIDE);
		}
		else if (affected_by_spell(ch, SPELL_HIDE))
		{
			percent = number(1, 82 + GET_REAL_INT(vict));
			prob = calculate_skill(ch, SKILL_HIDE, percent, vict);
			if (percent > prob)
			{
				affect_from_char(ch, SPELL_HIDE);
				if (!AFF_FLAGGED(ch, AFF_HIDE))
				{
					improove_skill(ch, SKILL_HIDE, FALSE, vict);
					act("�� �� ������ �������� ����������.", FALSE, ch, 0, vict, TO_CHAR);
				}
			}
			else
			{
				improove_skill(ch, SKILL_HIDE, TRUE, vict);
				act("��� ������� �������� ����������.\r\n", FALSE, ch, 0, vict, TO_CHAR);
				return (TRUE);
			}
		}
	}
	return (FALSE);
}

int skip_camouflage(CHAR_DATA * ch, CHAR_DATA * vict)
{
	int percent, prob;

	if (MAY_SEE(vict, ch) && (AFF_FLAGGED(ch, AFF_CAMOUFLAGE)
							  || affected_by_spell(ch, SPELL_CAMOUFLAGE)))
	{
		if (awake_camouflage(ch))  	//if (affected_by_spell(ch,SPELL_CAMOUFLAGE))
		{
			send_to_char("�� ���������� ���������������, �� ���� ���������� ������ ���.\r\n", ch);
			affect_from_char(ch, SPELL_CAMOUFLAGE);
			make_visible(ch, AFF_CAMOUFLAGE);
			SET_BIT(EXTRA_FLAGS(ch, EXTRA_FAILCAMOUFLAGE), EXTRA_FAILCAMOUFLAGE);
		}
		else if (affected_by_spell(ch, SPELL_CAMOUFLAGE))
		{
			percent = number(1, 82 + GET_REAL_INT(vict));
			prob = calculate_skill(ch, SKILL_CAMOUFLAGE, percent, vict);
			if (percent > prob)
			{
				affect_from_char(ch, SPELL_CAMOUFLAGE);
				if (!AFF_FLAGGED(ch, AFF_CAMOUFLAGE))
				{
					improove_skill(ch, SKILL_CAMOUFLAGE, FALSE, vict);
					act("�� �� ������ ��������� ���������������.", FALSE, ch, 0, vict, TO_CHAR);
				}
			}
			else
			{
				improove_skill(ch, SKILL_CAMOUFLAGE, TRUE, vict);
				act("���� ���������� ��������� �� ������.\r\n", FALSE, ch, 0, vict, TO_CHAR);
				return (TRUE);
			}
		}
	}
	return (FALSE);
}


int skip_sneaking(CHAR_DATA * ch, CHAR_DATA * vict)
{
	int percent, prob;

	if (MAY_SEE(vict, ch) && (AFF_FLAGGED(ch, AFF_SNEAK) || affected_by_spell(ch, SPELL_SNEAK)))
	{
		if (awake_sneak(ch))  	//if (affected_by_spell(ch,SPELL_SNEAK))
		{
			send_to_char("�� ���������� �����������, �� ���� ���������� ������ ���.\r\n", ch);
			affect_from_char(ch, SPELL_SNEAK);
			if (affected_by_spell(ch, SPELL_HIDE))
				affect_from_char(ch, SPELL_HIDE);
			make_visible(ch, AFF_SNEAK);
			SET_BIT(EXTRA_FLAGS(ch, EXTRA_FAILSNEAK), EXTRA_FAILSNEAK);
		}
		else if (affected_by_spell(ch, SPELL_SNEAK))
		{
			percent = number(1, 82 + GET_REAL_INT(vict));
			prob = calculate_skill(ch, SKILL_SNEAK, percent, vict);
			if (percent > prob)
			{
				affect_from_char(ch, SPELL_SNEAK);
				if (affected_by_spell(ch, SPELL_HIDE))
					affect_from_char(ch, SPELL_HIDE);
				if (!AFF_FLAGGED(ch, AFF_SNEAK))
				{
					improove_skill(ch, SKILL_SNEAK, FALSE, vict);
					act("�� �� ������ ���������� ���������.", FALSE, ch, 0, vict, TO_CHAR);
				}
			}
			else
			{
				improove_skill(ch, SKILL_SNEAK, TRUE, vict);
				act("��� ������� ����������� ���������.\r\n", FALSE, ch, 0, vict, TO_CHAR);
				return (TRUE);
			}
		}
	}
	return (FALSE);
}

/* do_simple_move assumes
 *    1. That there is no master and no followers.
 *    2. That the direction exists.
 *
 *   Returns :
 *   1 : If succes.
 *   0 : If fail
 */
/*
 * Check for special routines (North is 1 in command list, but 0 here) Note
 * -- only check if following; this avoids 'double spec-proc' bug
 */

int real_forest_paths_sect(int sect)
{
	switch (sect)
	{
	case SECT_FOREST:
		return SECT_FIELD;
		break;
	case SECT_FOREST_SNOW:
		return SECT_FIELD_SNOW;
		break;
	case SECT_FOREST_RAIN:
		return SECT_FIELD_RAIN;
		break;
	}
	return sect;
}

int real_mountains_paths_sect(int sect)
{
	switch (sect)
	{
	case SECT_HILLS:
	case SECT_MOUNTAIN:
		return SECT_FIELD;
		break;
	case SECT_HILLS_RAIN:
		return SECT_FIELD_RAIN;
		break;
	case SECT_HILLS_SNOW:
	case SECT_MOUNTAIN_SNOW:
		return SECT_FIELD_SNOW;
		break;
	}
	return sect;
}

int legal_dir(CHAR_DATA * ch, int dir, int need_specials_check, int show_msg)
{
	int need_movement = 0, ch_inroom, ch_toroom;
	CHAR_DATA *tch;

	buf2[0] = '\0';
	if (need_specials_check && special(ch, dir + 1, buf2, 1))
		return (FALSE);

	if (!CAN_GO(ch, dir))
		return (FALSE);

	// �� ������� � ������� ����� ��, ���� ��� ��� ������� �����
	if (DeathTrap::check_tunnel_death(ch, EXIT(ch, dir)->to_room))
	{
		if (show_msg)
		{
			send_to_char("Error 3. � ����� � ������� ���������� ��������� �������� ����������.\r\n", ch);
		}
		return (FALSE);
	}

	// charmed
	if (AFF_FLAGGED(ch, AFF_CHARM) && ch->master && ch->in_room == ch->master->in_room)
	{
		if (show_msg)
		{
			send_to_char("�� �� ������ �������� ���� �����.\r\n", ch);
			act("$N �������$U �������� ���.", FALSE, ch->master, 0, ch, TO_CHAR);
		}
		return (FALSE);
	}

	// check NPC's
	if (IS_NPC(ch))
	{
		if (GET_DEST(ch) != NOWHERE)
			return (TRUE);
		//  if this room or the one we're going to needs a boat, check for one */
		if (!MOB_FLAGGED(ch, MOB_SWIMMING) &&
				!MOB_FLAGGED(ch, MOB_FLYING) &&
				!AFF_FLAGGED(ch, AFF_FLY) &&
				(real_sector(ch->in_room) == SECT_WATER_NOSWIM ||
				 real_sector(EXIT(ch, dir)->to_room) == SECT_WATER_NOSWIM))
		{
			if (!has_boat(ch))
				return (FALSE);
		}

		// ��������� �������� �� �� ��� ��� ����� ������� �����
		if (EXIT_FLAGGED(EXIT(ch, dir), EX_CLOSED) &&
				!MOB_FLAGGED(ch, MOB_OPENDOOR))
			return (FALSE);

		if (!MOB_FLAGGED(ch, MOB_FLYING) &&
				!AFF_FLAGGED(ch, AFF_FLY) && SECT(EXIT(ch, dir)->to_room) == SECT_FLYING)
			return (FALSE);

		if (MOB_FLAGGED(ch, MOB_ONLYSWIMMING) &&
				!(real_sector(EXIT(ch, dir)->to_room) == SECT_WATER_SWIM ||
				  real_sector(EXIT(ch, dir)->to_room) == SECT_WATER_NOSWIM ||
				  real_sector(EXIT(ch, dir)->to_room) == SECT_UNDERWATER))
			return (FALSE);

		if (ROOM_FLAGGED(EXIT(ch, dir)->to_room, ROOM_NOMOB) &&
				!IS_HORSE(ch) &&
				!AFF_FLAGGED(ch, AFF_CHARM) && !MOB_FLAGGED(ch, MOB_ANGEL) && !MOB_FLAGGED(ch, MOB_IGNORNOMOB))
			return (FALSE);

		if (ROOM_FLAGGED(EXIT(ch, dir)->to_room, ROOM_DEATH) && !IS_HORSE(ch))
			return (FALSE);

		if (ROOM_FLAGGED(EXIT(ch, dir)->to_room, ROOM_GODROOM))
			return (FALSE);

		if (ROOM_FLAGGED(EXIT(ch, dir)->to_room, ROOM_NOHORSE) && IS_HORSE(ch))
			return (FALSE);

		if (!entry_mtrigger(ch))
			return (FALSE);
		if (!enter_wtrigger(world[EXIT(ch, dir)->to_room], ch, dir))
			return (FALSE);
	}
	else
	{
		if (real_sector(ch->in_room) == SECT_WATER_NOSWIM ||
				real_sector(EXIT(ch, dir)->to_room) == SECT_WATER_NOSWIM)
		{
			if (!has_boat(ch))
			{
				if (show_msg)
					send_to_char("��� ����� �����, ����� ������� ����.\r\n", ch);
				return (FALSE);
			}
		}
		if (real_sector(EXIT(ch, dir)->to_room) == SECT_FLYING
			&& !IS_GOD(ch)
			&& !AFF_FLAGGED(ch, AFF_FLY))
		{
			if (show_msg)
			{
				send_to_char("���� ����� ������ �������.\r\n", ch);
			}
			return (FALSE);
		}
		// move points needed is avg. move loss for src and destination sect type
		ch_inroom = real_sector(ch->in_room);
		ch_toroom = real_sector(EXIT(ch, dir)->to_room);

		if (can_use_feat(ch, FOREST_PATHS_FEAT))
		{
			ch_inroom = real_forest_paths_sect(ch_inroom);
			ch_toroom = real_forest_paths_sect(ch_toroom);
		}

		if (can_use_feat(ch, MOUNTAIN_PATHS_FEAT))
		{
			ch_inroom = real_mountains_paths_sect(ch_inroom);
			ch_toroom = real_mountains_paths_sect(ch_toroom);
		}

		need_movement = (IS_FLY(ch) || on_horse(ch)) ? 1 :
						(movement_loss[ch_inroom] + movement_loss[ch_toroom]) / 2;

		if (IS_IMMORTAL(ch))
			need_movement = 0;
		else if (affected_by_spell(ch, SPELL_CAMOUFLAGE))
			need_movement += CAMOUFLAGE_MOVES;
		else if (affected_by_spell(ch, SPELL_SNEAK))
			need_movement += SNEAK_MOVES;

		if (GET_MOVE(ch) < need_movement)
		{
			if (need_specials_check && ch->master)
			{
				if (show_msg)
					send_to_char("�� ������� ������, ����� ��������� ����.\r\n", ch);
			}
			else
			{
				if (show_msg)
					send_to_char("�� ������� ������.\r\n", ch);
			}
			return (FALSE);
		}

		if (ROOM_FLAGGED(ch->in_room, ROOM_ATRIUM))
		{
			if (!Clan::MayEnter(ch, EXIT(ch, dir)->to_room, HCE_ATRIUM))
			{
				if (show_msg)
					send_to_char("������� �������������! ���� ���������!\r\n", ch);
				return (FALSE);
			}
		}
		//����� ���� �� ��� � ������� � ������ !������
		if (on_horse(ch)
				&& !legal_dir(get_horse(ch), dir, need_specials_check, FALSE))
		{
			if (show_msg)
			{
				act("$Z $N ������������ ���� ����, � ��� �������� ���������.",
					FALSE, ch, 0, get_horse(ch), TO_CHAR);
				act("$n ��������$g � $N1.", FALSE, ch, 0, get_horse(ch), TO_ROOM | TO_ARENA_LISTEN);
				REMOVE_BIT(AFF_FLAGS(ch, AFF_HORSE), AFF_HORSE);
			}
		}
		//�������� �� ������: ��������� ������ � ����, ���� ��� ��������
		if (ROOM_FLAGGED(EXIT(ch, dir)->to_room, ROOM_TUNNEL) &&
				(num_pc_in_room((world[EXIT(ch, dir)->to_room])) > 0 || on_horse(ch)))
		{
			if (num_pc_in_room((world[EXIT(ch, dir)->to_room])) > 0)
			{
				if (show_msg)
					send_to_char("������� ���� �����.\r\n", ch);
				return (FALSE);
			}
			else if (show_msg)
			{
				act("$Z $N ���������$U, � ��� �������� ���������.",
					FALSE, ch, 0, get_horse(ch), TO_CHAR);
				act("$n ��������$g � $N1.", FALSE, ch, 0, get_horse(ch), TO_ROOM | TO_ARENA_LISTEN);
				REMOVE_BIT(AFF_FLAGS(ch, AFF_HORSE), AFF_HORSE);
			}
		}

		if (on_horse(ch) && GET_HORSESTATE(get_horse(ch)) <= 0)
		{
			if (show_msg)
				act("$Z $N ������$G ���������, ��� �� ����� ����� ��� �� ����.",
					FALSE, ch, 0, get_horse(ch), TO_CHAR);
			return (FALSE);
		}

		if (on_horse(ch) && (AFF_FLAGGED(get_horse(ch), AFF_HOLD) || AFF_FLAGGED(get_horse(ch), AFF_SLEEP)))
		{
			if (show_msg)
				act("$Z $N �� � ��������� ����� ��� �� ����.\r\n", FALSE, ch, 0, get_horse(ch),
					TO_CHAR);
			return (FALSE);
		}

		if (ROOM_FLAGGED(EXIT(ch, dir)->to_room, ROOM_GODROOM) && !IS_GRGOD(ch))
		{
			if (show_msg)
				send_to_char("�� �� ����� �����������, ��� ��� �������!\r\n", ch);
			return (FALSE);
		}

		if (!entry_mtrigger(ch))
			return (FALSE);
		if (!enter_wtrigger(world[EXIT(ch, dir)->to_room], ch, dir))
			return (FALSE);

		for (tch = world[IN_ROOM(ch)]->people; tch; tch = tch->next_in_room)
		{
			if (!IS_NPC(tch))
				continue;
			if (NPC_FLAGGED(tch, 1 << dir) &&
					AWAKE(tch) &&
					GET_POS(tch) > POS_SLEEPING && CAN_SEE(tch, ch) &&
					!AFF_FLAGGED(tch, AFF_CHARM) && !AFF_FLAGGED(tch, AFF_HOLD))
			{
				if (show_msg)
					act("$N ���������$G ��� ����.", FALSE, ch, 0, tch, TO_CHAR);
				return (FALSE);
			}
		}
	}

	return (need_movement ? need_movement : 1);
}

#define MOB_AGGR_TO_ALIGN (MOB_AGGR_EVIL | MOB_AGGR_NEUTRAL | MOB_AGGR_GOOD)

#define MAX_DRUNK_SONG 6
const char *drunk_songs[MAX_DRUNK_SONG] = { "\"����� �����, �-�-�..., ������� �������\"",
		"\"���� ��, ��������, ���� ������\"",
		"\"�����, ���� �������\"",
		"\"� ��� ����� ���� �� ������\"",
		"\"�� ��� ���� ����, �������� ����\"",
		"\"��������� �����, ����������\"",
										  };

#define MAX_DRUNK_VOICE 5
const char *drunk_voice[MAX_DRUNK_VOICE] = { " - �������$g $n",
		" - �����$g $n.",
		" - ���������$g $n.",
		" - ����� �������$g $n.",
		" - ����������� ��������$g $n.",
										   };



int do_simple_move(CHAR_DATA * ch, int dir, int need_specials_check, CHAR_DATA * leader)
{
	struct track_data *track;
	room_rnum was_in, go_to;
	int need_movement, i, ndir = -1, nm, invis = 0, use_horse = 0, is_horse = 0, direction = 0;
	int IsFlee = dir & 0x80, mob_rnum = -1;
	CHAR_DATA *vict, *horse = NULL;

	dir = dir & 0x7f;

	if (!(need_movement = legal_dir(ch, dir, need_specials_check, TRUE)))
		return (FALSE);

	// Mortally drunked - it is loss direction
	if (!IS_NPC(ch) && !leader && GET_COND(ch, DRUNK) >= CHAR_MORTALLY_DRUNKED && !on_horse(ch) &&
			GET_COND(ch, DRUNK) >= number(CHAR_DRUNKED, 50))
	{
		for (i = 0; i < NUM_OF_DIRS && ndir < 0; i++)
		{
			ndir = number(0, 5);
			if (!EXIT(ch, ndir) || EXIT(ch, ndir)->to_room == NOWHERE ||
					EXIT_FLAGGED(EXIT(ch, ndir), EX_CLOSED) ||
					!(nm = legal_dir(ch, ndir, need_specials_check, TRUE)))
				ndir = -1;
			else
			{
				if (dir != ndir)
				{
					sprintf(buf, "���� ���� �� ����� ��������� ���...\r\n");
					send_to_char(buf, ch);
				}
				if (!ch->get_fighting() && number(10, 24) < GET_COND(ch, DRUNK))
				{
					sprintf(buf, "%s", drunk_songs[number(0, MAX_DRUNK_SONG - 1)]);
					send_to_char(buf, ch);
					send_to_char("\r\n", ch);
					strcat(buf, drunk_voice[number(0, MAX_DRUNK_VOICE - 1)]);
					act(buf, FALSE, ch, 0, 0, TO_ROOM | CHECK_DEAF);
					affect_from_char(ch, SPELL_SNEAK);
					affect_from_char(ch, SPELL_CAMOUFLAGE);
				};
				dir = ndir;
				need_movement = nm;
			}
		}
	}

	// Now we know we're allow to go into the room.
	if (!IS_IMMORTAL(ch) && !IS_NPC(ch))
		GET_MOVE(ch) -= need_movement;

	i = skill_info[SKILL_SNEAK].max_percent;
	if (AFF_FLAGGED(ch, AFF_SNEAK) && !IsFlee)
	{
		if (IS_NPC(ch))
			invis = 1;
		else if (awake_sneak(ch))
		{
			affect_from_char(ch, SPELL_SNEAK);
		}
		else
			if (!affected_by_spell(ch, SPELL_SNEAK) || calculate_skill(ch, SKILL_SNEAK, i, 0) >= number(1, i))
				invis = 1;
	}

	i = skill_info[SKILL_CAMOUFLAGE].max_percent;
	if (AFF_FLAGGED(ch, AFF_CAMOUFLAGE) && !IsFlee)
	{
		if (IS_NPC(ch))
			invis = 1;
		else if (awake_camouflage(ch))
		{
			affect_from_char(ch, SPELL_CAMOUFLAGE);
		}
		else
			if (!affected_by_spell(ch, SPELL_CAMOUFLAGE) ||
					calculate_skill(ch, SKILL_CAMOUFLAGE, i, 0) >= number(1, i))
				invis = 1;
	}

	if (!IsFlee)
	{
		sprintf(buf, "�� ��������� %s%s.", leader ? "������ �� $N4 " : "", DirsTo[dir]);
		act(buf, FALSE, ch, 0, leader, TO_CHAR);
	}

	was_in = ch->in_room;
	go_to = world[was_in]->dir_option[dir]->to_room;
	direction = dir + 1;
	use_horse = AFF_FLAGGED(ch, AFF_HORSE) && has_horse(ch, FALSE) &&
				(IN_ROOM(get_horse(ch)) == was_in || IN_ROOM(get_horse(ch)) == go_to);
	is_horse = IS_HORSE(ch) && (ch->master)
			   && !AFF_FLAGGED(ch->master, AFF_INVISIBLE)
			   && (IN_ROOM(ch->master) == was_in || IN_ROOM(ch->master) == go_to);

	if (!invis && !is_horse)
	{
		if (IsFlee)
			strcpy(buf1, "������$g");
		else if (IS_NPC(ch) && NPC_FLAGGED(ch, NPC_MOVERUN))
			strcpy(buf1, "������$g");
		else if ((!use_horse && AFF_FLAGGED(ch, AFF_FLY))
			|| (IS_NPC(ch) && NPC_FLAGGED(ch, NPC_MOVEFLY)))
		{
			strcpy(buf1, "������$g");
		}
		else if (IS_NPC(ch)
			&& NPC_FLAGGED(ch, NPC_MOVESWIM)
			&& (real_sector(was_in) == SECT_WATER_SWIM
				|| real_sector(was_in) == SECT_WATER_NOSWIM
				|| real_sector(was_in) == SECT_UNDERWATER))
		{
			strcpy(buf1, "�����$g");
		}
		else if (IS_NPC(ch) && NPC_FLAGGED(ch, NPC_MOVEJUMP))
			strcpy(buf1, "�������$g");
		else if (IS_NPC(ch) && NPC_FLAGGED(ch, NPC_MOVECREEP))
			strcpy(buf1, "�����$q");
		else if (real_sector(was_in) == SECT_WATER_SWIM
			|| real_sector(was_in) == SECT_WATER_NOSWIM
			|| real_sector(was_in) == SECT_UNDERWATER)
		{
			strcpy(buf1, "�����$g");
		}
		else if (use_horse)
		{
			CHAR_DATA *horse = get_horse(ch);
			if (horse && AFF_FLAGGED(horse, AFF_FLY))
			{
				strcpy(buf1, "������$g");
			}
			else
			{
				strcpy(buf1, "�����$g");
			}
		}
		else
			strcpy(buf1, "��$y");

		if (IsFlee && !IS_NPC(ch) && can_use_feat(ch, WRIGGLER_FEAT))
			sprintf(buf2, "$n %s.", buf1);
		else
			sprintf(buf2, "$n %s %s.", buf1, DirsTo[dir]);
		act(buf2, TRUE, ch, 0, 0, TO_ROOM);
	}

	if (invis && !is_horse)
	{
		act("���-�� ���� �������� ������.", TRUE, ch, 0, 0, TO_ROOM_HIDE);
	}

	if (on_horse(ch)) // || has_horse(ch, TRUE))
		horse = get_horse(ch);

	// ���� �������, � �� ���������� ����� �� ����, �� ������� � ���� ������
	if (IsFlee)
	{
		stop_fighting(ch, TRUE);
	}

	// track improovment
	if (!IS_NPC(ch) && IS_BITS(ch->track_dirs, dir))
	{
		send_to_char("�� ��������� �� �����.\r\n", ch);
		improove_skill(ch, SKILL_TRACK, TRUE, 0);
	}

	char_from_room(ch);
	char_to_room(ch, go_to);
	if (horse)
	{
		GET_HORSESTATE(horse) -= 1;
		char_from_room(horse);
		char_to_room(horse, go_to);
	}

	if (!invis && !is_horse)
	{
		if (IsFlee || (IS_NPC(ch) && NPC_FLAGGED(ch, NPC_MOVERUN)))
			strcpy(buf1, "��������$g");
		else if ((!use_horse && AFF_FLAGGED(ch, AFF_FLY))
			|| (IS_NPC(ch) && NPC_FLAGGED(ch, NPC_MOVEFLY)))
		{
			strcpy(buf1, "��������$g");
		}
		else if (IS_NPC(ch) && NPC_FLAGGED(ch, NPC_MOVESWIM)
			&& (real_sector(go_to) == SECT_WATER_SWIM
				|| real_sector(go_to) == SECT_WATER_NOSWIM
				|| real_sector(go_to) == SECT_UNDERWATER))
		{
			strcpy(buf1, "�������$g");
		}
		else if (IS_NPC(ch) && NPC_FLAGGED(ch, NPC_MOVEJUMP))
			strcpy(buf1, "���������$g");
		else if (IS_NPC(ch) && NPC_FLAGGED(ch, NPC_MOVECREEP))
			strcpy(buf1, "�������$q");
		else if (real_sector(go_to) == SECT_WATER_SWIM
			|| real_sector(go_to) == SECT_WATER_NOSWIM
			|| real_sector(go_to) == SECT_UNDERWATER)
		{
			strcpy(buf1, "�������$g");
		}
		else if (use_horse)
		{
			CHAR_DATA *horse = get_horse(ch);
			if (horse && AFF_FLAGGED(horse, AFF_FLY))
			{
				strcpy(buf1, "��������$g");
			}
			else
			{
				strcpy(buf1, "�������$g");
			}

		}
		else
			strcpy(buf1, "����$y");

		//log("%s-%d",GET_NAME(ch),IN_ROOM(ch));
		sprintf(buf2, "$n %s %s.", buf1, DirsFrom[dir]);
		//log(buf2);
		act(buf2, TRUE, ch, 0, 0, TO_ROOM);
		//log("ACT OK !");
	};

	if (invis && !is_horse)
	{
		act("���-�� ���� ��������� ����.", TRUE, ch, 0, 0, TO_ROOM_HIDE);
	}

	if (ch->desc != NULL)
		look_at_room(ch, 0);

	if (DeathTrap::check_death_trap(ch))
	{
		if (horse)
			extract_char(horse, FALSE);
		return (FALSE);
	}
	if (check_death_ice(go_to, ch))
	{
		return (FALSE);
	}
	if (DeathTrap::tunnel_damage(ch))
	{
		return (FALSE);
	}

	entry_memory_mtrigger(ch);

	if (!greet_mtrigger(ch, dir) || !greet_otrigger(ch, dir))
	{
		char_from_room(ch);
		char_to_room(ch, was_in);
		if (horse)
		{
			char_from_room(horse);
			char_to_room(horse, was_in);
		}
		look_at_room(ch, 0);
		return (FALSE);
	}
	else
	{
		greet_memory_mtrigger(ch);
		// add track info
		if (!AFF_FLAGGED(ch, AFF_NOTRACK) && (!IS_NPC(ch) || (mob_rnum = GET_MOB_RNUM(ch)) >= 0))
		{
			for (track = world[go_to]->track; track; track = track->next)
				if ((IS_NPC(ch) && IS_SET(track->track_info, TRACK_NPC)
						&& track->who == mob_rnum) || (!IS_NPC(ch)
													   && !IS_SET(track->track_info, TRACK_NPC)
													   && track->who == GET_IDNUM(ch)))
					break;

			if (!track && !ROOM_FLAGGED(go_to, ROOM_NOTRACK))
			{
				CREATE(track, struct track_data, 1);
				track->track_info = IS_NPC(ch) ? TRACK_NPC : 0;
				track->who = IS_NPC(ch) ? mob_rnum : GET_IDNUM(ch);
				track->next = world[go_to]->track;
				world[go_to]->track = track;
			}

			if (track)
			{
				SET_BIT(track->time_income[Reverse[dir]], 1);
				if (affected_by_spell(ch, SPELL_LIGHT_WALK) && !on_horse(ch))
					if (AFF_FLAGGED(ch, AFF_LIGHT_WALK))
						track->time_income[Reverse[dir]] <<= number(15, 30);
				REMOVE_BIT(track->track_info, TRACK_HIDE);
			}

			for (track = world[was_in]->track; track; track = track->next)
				if ((IS_NPC(ch) && IS_SET(track->track_info, TRACK_NPC)
						&& track->who == mob_rnum) || (!IS_NPC(ch)
													   && !IS_SET(track->track_info, TRACK_NPC)
													   && track->who == GET_IDNUM(ch)))
					break;

			if (!track && !ROOM_FLAGGED(was_in, ROOM_NOTRACK))
			{
				CREATE(track, struct track_data, 1);
				track->track_info = IS_NPC(ch) ? TRACK_NPC : 0;
				track->who = IS_NPC(ch) ? mob_rnum : GET_IDNUM(ch);
				track->next = world[was_in]->track;
				world[was_in]->track = track;
			}
			if (track)
			{
				SET_BIT(track->time_outgone[dir], 1);
				if (affected_by_spell(ch, SPELL_LIGHT_WALK) && !on_horse(ch))
					if (AFF_FLAGGED(ch, AFF_LIGHT_WALK))
						track->time_outgone[dir] <<= number(15, 30);
				REMOVE_BIT(track->track_info, TRACK_HIDE);
			}
		}
	}


	// hide improovment
	if (IS_NPC(ch))
		for (vict = world[ch->in_room]->people; vict; vict = vict->next_in_room)
		{
			if (!IS_NPC(vict))
				skip_hiding(vict, ch);
		}

	income_mtrigger(ch, direction - 1);

	// char income, go mobs action
	for (vict = world[IN_ROOM(ch)]->people; !IS_NPC(ch) && vict; vict = vict->next_in_room)
	{
		if (!IS_NPC(vict))
			continue;

		if (!CAN_SEE(vict, ch) ||
				AFF_FLAGGED(ch, AFF_SNEAK) ||
				AFF_FLAGGED(ch, AFF_CAMOUFLAGE) || vict->get_fighting() || GET_POS(vict) < POS_RESTING)
			continue;

		// AWARE mobs
		if (MOB_FLAGGED(vict, MOB_AWARE) &&
				GET_POS(vict) < POS_FIGHTING && !AFF_FLAGGED(vict, AFF_HOLD) && GET_POS(vict) > POS_SLEEPING)
		{
			act("$n ������$u.", FALSE, vict, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
			GET_POS(vict) = POS_STANDING;
		}
	}

	// If flee - go agressive mobs
	if (!IS_NPC(ch) && IsFlee)
		do_aggressive_room(ch, FALSE);

	return (direction);
}


int perform_move(CHAR_DATA *ch, int dir, int need_specials_check, int checkmob, CHAR_DATA *master)
{
	if (AFF_FLAGGED(ch, AFF_BANDAGE))
	{
		send_to_char("��������� ���� ��������!\r\n", ch);
		affect_from_char(ch, SPELL_BANDAGE);
	}
	ch->set_motion(true);

	room_rnum was_in;
	struct follow_type *k, *next;

	if (ch == NULL || dir < 0 || dir >= NUM_OF_DIRS || ch->get_fighting())
		return (0);
	else if (!EXIT(ch, dir) || EXIT(ch, dir)->to_room == NOWHERE)
		send_to_char("�� �� ������� ���� ������...\r\n", ch);
	else if (EXIT_FLAGGED(EXIT(ch, dir), EX_CLOSED))
	{
		if (EXIT(ch, dir)->keyword)
		{
			sprintf(buf2, "������� (%s).\r\n", EXIT(ch, dir)->keyword);
			send_to_char(buf2, ch);
		}
		else
			send_to_char("�������.\r\n", ch);
	}
	else
	{
		if (!ch->followers)
		{
			if (!do_simple_move(ch, dir, need_specials_check, master))
				return (FALSE);
		}
		else
		{
			was_in = ch->in_room;
			// When leader mortally drunked - he change direction
			// So returned value set to FALSE or DIR + 1
			if (!(dir = do_simple_move(ch, dir, need_specials_check, master)))
				return (FALSE);
			dir--;
			for (k = ch->followers; k && k->follower->master; k = next)
			{
				next = k->next;
				if (k->follower->in_room == was_in &&
						!k->follower->get_fighting() &&
						HERE(k->follower) &&
						!GET_MOB_HOLD(k->follower) &&
						AWAKE(k->follower) &&
						(IS_NPC(k->follower) ||
						 (!PLR_FLAGGED(k->follower, PLR_MAILING) &&
						  !PLR_FLAGGED(k->follower, PLR_WRITING))) &&
						(!IS_HORSE(k->follower) || !AFF_FLAGGED(k->follower, AFF_TETHERED)))
				{
					if (GET_POS(k->follower) < POS_STANDING)
					{
						if (IS_NPC(k->follower) &&
								IS_NPC(k->follower->master) &&
								GET_POS(k->follower) > POS_SLEEPING
								&& !GET_WAIT(k->follower)
							)
						{
							act("$n ������$u.", FALSE, k->follower, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
							GET_POS(k->follower) = POS_STANDING;
						}
						else
							continue;
					}
//                   act("�� ��������� ������ �� $N4.",FALSE,k->follower,0,ch,TO_CHAR);
					perform_move(k->follower, dir, 1, FALSE, ch);
				}
			}
		}
		if (checkmob)
		{
			do_aggressive_room(ch, TRUE);
		}
		return (TRUE);
	}
	return (FALSE);
}


ACMD(do_move)
{
	/*
	 * This is basically a mapping of cmd numbers to perform_move indices.
	 * It cannot be done in perform_move because perform_move is called
	 * by other functions which do not require the remapping.
	 */
	perform_move(ch, subcmd - 1, 0, TRUE, 0);
}

ACMD(do_hidemove)
{
	int dir = 0, sneaking = affected_by_spell(ch, SPELL_SNEAK);
	AFFECT_DATA af;

	skip_spaces(&argument);
	if (!ch->get_skill(SKILL_SNEAK))
	{
		send_to_char("�� �� ������ �����.\r\n", ch);
		return;
	}

	if (!*argument)
	{
		send_to_char("� ���� ��� �� �������������?\r\n", ch);
		return;
	}

	if ((dir = search_block(argument, dirs, FALSE)) < 0 && (dir = search_block(argument, DirIs, FALSE)) < 0)
	{
		send_to_char("����������� �����������.\r\n", ch);
		return;
	}
	if (on_horse(ch))
	{
		act("��� ������ $N.", FALSE, ch, 0, get_horse(ch), TO_CHAR);
		return;
	}
	if (!sneaking)
	{
		af.type = SPELL_SNEAK;
		af.location = 0;
		af.modifier = 0;
		af.duration = 1;
		af.bitvector = (number(1, skill_info[SKILL_SNEAK].max_percent) <
						calculate_skill(ch, SKILL_SNEAK,
										skill_info[SKILL_SNEAK].max_percent, 0)) ? AFF_SNEAK : 0;
		af.battleflag = 0;
		affect_join(ch, &af, FALSE, FALSE, FALSE, FALSE);
	}
	perform_move(ch, dir, 0, TRUE, 0);
	if (!sneaking || affected_by_spell(ch, SPELL_GLITTERDUST))
		affect_from_char(ch, SPELL_SNEAK);
}

#define DOOR_IS_OPENABLE(ch, obj, door)	((obj) ? \
			((GET_OBJ_TYPE(obj) == ITEM_CONTAINER) && \
			OBJVAL_FLAGGED(obj, CONT_CLOSEABLE)) :\
			(EXIT_FLAGGED(EXIT(ch, door), EX_ISDOOR)))
#define DOOR_IS(ch, door)	((EXIT_FLAGGED(EXIT(ch, door), EX_ISDOOR)))

#define DOOR_IS_OPEN(ch, obj, door)	((obj) ? \
			(!OBJVAL_FLAGGED(obj, CONT_CLOSED)) :\
			(!EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED)))
#define DOOR_IS_BROKEN(ch, obj, door)	((obj) ? \
	(OBJVAL_FLAGGED(obj, CONT_BROKEN)) :\
	(EXIT_FLAGGED(EXIT(ch, door), EX_BROKEN)))
#define DOOR_IS_UNLOCKED(ch, obj, door)	((obj) ? \
			(!OBJVAL_FLAGGED(obj, CONT_LOCKED)) :\
			(!EXIT_FLAGGED(EXIT(ch, door), EX_LOCKED)))
#define DOOR_IS_PICKPROOF(ch, obj, door) ((obj) ? \
	(OBJVAL_FLAGGED(obj, CONT_PICKPROOF) || OBJVAL_FLAGGED(obj, CONT_BROKEN)) : \
	(EXIT_FLAGGED(EXIT(ch, door), EX_PICKPROOF) || EXIT_FLAGGED(EXIT(ch, door), EX_BROKEN)))

#define DOOR_IS_CLOSED(ch, obj, door)	(!(DOOR_IS_OPEN(ch, obj, door)))
#define DOOR_IS_LOCKED(ch, obj, door)	(!(DOOR_IS_UNLOCKED(ch, obj, door)))
#define DOOR_KEY(ch, obj, door)		((obj) ? (GET_OBJ_VAL(obj, 2)) : \
					(EXIT(ch, door)->key))
#define DOOR_LOCK(ch, obj, door)	((obj) ? (GET_OBJ_VAL(obj, 1)) : \
					(EXIT(ch, door)->exit_info))
#define DOOR_LOCK_COMPLEX(ch, obj, door) ((obj) ? \
			(GET_OBJ_VAL(obj,3)) :\
			(EXIT(ch, door)->lock_complexity))


int find_door(CHAR_DATA * ch, const char *type, char *dir, const char *cmdname)
{
	int door;

	if (*dir)  //������� ����������� (������ ��������)
	{
		//��������� ������������� �� �������� ���������� ��� ������� ������������
		if ((door = search_block(dir, dirs, FALSE)) == -1 && (door = search_block(dir, DirIs, FALSE)) == -1)  	// Partial Match
		{
			//strcpy(doorbuf,"�������� �����������.\r\n");
			return (-1); //�������� �����������
		}
		if (EXIT(ch, door)) //��������� ���� �� ����� ����� � ��������� �����������
		{
			if (EXIT(ch, door)->keyword && EXIT(ch, door)->vkeyword) //����� ���-�� ��-���������� ����������?
			{
				if (isname(type, EXIT(ch, door)->keyword) || isname(type, EXIT(ch, door)->vkeyword))
					//������ �������� ������������� ������������� ��� ������������ ������ �����
					return (door);
				else
				{
					return (-2); //�� ��������� ������� ����� � ���� �����������
				}
			}
			else if (is_abbrev(type, "�����") || is_abbrev(type, "door"))
				//�������� ������������� "�����" ��� "door" � ���� � ��������� �����������
				return (door);
			else
				//����� � ��������� "�����" ����, �� �������� �� �������������
				return (-2);
		}
		else
		{
			return (-3); //� ���� ����������� ��� ������
		}
	}
	else //����������� �� �������, ���� ����� �� ��������
	{
		if (!*type) //�������� �� �������
		{
			return (-4); //�� ������� ����������
		}
		for (door = 0; door < NUM_OF_DIRS; door++) //��������� ��� �����������, �� �������� �� �����?
		{
			if (EXIT(ch, door)) //���� ����� � ���� �����������
			{
				if (EXIT(ch, door)->keyword && EXIT(ch, door)->vkeyword) //����� ���-�� ��-���������� ����������?
				{
					if (isname(type, EXIT(ch, door)->keyword) || isname(type, EXIT(ch, door)->vkeyword))
						//�������� ������������� ����� ���� �����
						return (door);
				}
				else if (DOOR_IS(ch, door) && (is_abbrev(type, "�����") || is_abbrev(type, "door")))
					//����� �� ����� ������ �������, �������� ������������� �����
					return (door);
			}
		}
		return (-5); //����������� ������� ����� ��� �������� �����������
	}
}

int has_key(CHAR_DATA * ch, obj_vnum key)
{
	OBJ_DATA *o;

	for (o = ch->carrying; o; o = o->next_content)
		if (GET_OBJ_VNUM(o) == key && key != -1)
			return (TRUE);

	if (GET_EQ(ch, WEAR_HOLD))
		if (GET_OBJ_VNUM(GET_EQ(ch, WEAR_HOLD)) == key && key != -1)
			return (TRUE);

	return (FALSE);
}



#define NEED_OPEN	(1 << 0)
#define NEED_CLOSED	(1 << 1)
#define NEED_UNLOCKED	(1 << 2)
#define NEED_LOCKED	(1 << 3)

const char *cmd_door[] =
{
	"������$g",
	"������$g",
	"�����$q",
	"�����$q",
	"�������$g"
};

const char *a_cmd_door[] =
{
	"�������",
	"�������",
	"��������",
	"��������",
	"��������"
};

const int flags_door[] =
{
	NEED_CLOSED | NEED_UNLOCKED,
	NEED_OPEN,
	NEED_CLOSED | NEED_LOCKED,
	NEED_CLOSED | NEED_UNLOCKED,
	NEED_CLOSED | NEED_LOCKED
};


#define EXITN(room, door)		(world[room]->dir_option[door])
#define OPEN_DOOR(room, obj, door)	((obj) ?\
		(TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_CLOSED)) :\
		(TOGGLE_BIT(EXITN(room, door)->exit_info, EX_CLOSED)))
#define LOCK_DOOR(room, obj, door)	((obj) ?\
		(TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_LOCKED)) :\
		(TOGGLE_BIT(EXITN(room, door)->exit_info, EX_LOCKED)))

void do_doorcmd(CHAR_DATA * ch, OBJ_DATA * obj, int door, int scmd)
{
	int other_room = 0;
	EXIT_DATA *back = 0;
	CHAR_DATA * to;
	int rev_dir[] = { SOUTH, WEST, NORTH, EAST, DOWN, UP };
	char local_buf[MAX_STRING_LENGTH]; // ���������� buf � ������ ��������������

	sprintf(local_buf, "$n %s ", cmd_door[scmd]);
//  if (IS_NPC(ch))
//     log("MOB DOOR Moving:��� %s %s ����� � ������� %d",GET_NAME(ch),cmd_door[scmd],GET_ROOM_VNUM(IN_ROOM(ch)));
	if (!obj && ((other_room = EXIT(ch, door)->to_room) != NOWHERE))
		if ((back = world[other_room]->dir_option[rev_dir[door]]) != NULL)
			if ((back->to_room != ch->in_room) ||
					((EXITDATA(ch->in_room, door)->exit_info ^
					  EXITDATA(other_room, rev_dir[door])->exit_info) & (EX_ISDOOR | EX_CLOSED | EX_LOCKED)))
				back = 0;
	switch (scmd)
	{
	case SCMD_OPEN:
	case SCMD_CLOSE:
		if (scmd == SCMD_OPEN && obj && !open_otrigger(obj, ch, FALSE))
			return;
		if (scmd == SCMD_CLOSE && obj && !close_otrigger(obj, ch, FALSE))
			return;
		if (scmd == SCMD_OPEN && !obj && !open_wtrigger(world[ch->in_room], ch, door, FALSE))
			return;
		if (scmd == SCMD_CLOSE && !obj && !close_wtrigger(world[ch->in_room], ch, door, FALSE))
			return;
		if (scmd == SCMD_OPEN && !obj && back && !open_wtrigger(world[other_room], ch, rev_dir[door], FALSE))
			return;
		if (scmd == SCMD_CLOSE && !obj && back && !close_wtrigger(world[other_room], ch, rev_dir[door], FALSE))
			return;
		OPEN_DOOR(ch->in_room, obj, door);
		if (back)
		{
			OPEN_DOOR(other_room, obj, rev_dir[door]);
		}
		// ����������� � ���� ��������
		if (obj && system_obj::is_purse(obj))
		{
			system_obj::process_open_purse(ch, obj);
			return;
		}
		else
		{
			send_to_char(OK, ch);
		}
		break;

	case SCMD_UNLOCK:
	case SCMD_LOCK:
		if (scmd == SCMD_UNLOCK && obj && !open_otrigger(obj, ch, TRUE))
			return;
		if (scmd == SCMD_LOCK && obj && !close_otrigger(obj, ch, TRUE))
			return;
		if (scmd == SCMD_UNLOCK && !obj && !open_wtrigger(world[ch->in_room], ch, door, TRUE))
			return;
		if (scmd == SCMD_LOCK && !obj && !close_wtrigger(world[ch->in_room], ch, door, TRUE))
			return;
		if (scmd == SCMD_UNLOCK && !obj && back && !open_wtrigger(world[other_room], ch, rev_dir[door], TRUE))
			return;
		if (scmd == SCMD_LOCK && !obj && back && !close_wtrigger(world[other_room], ch, rev_dir[door], TRUE))
			return;
		LOCK_DOOR(ch->in_room, obj, door);
		if (back)
			LOCK_DOOR(other_room, obj, rev_dir[door]);
		if (!AFF_FLAGGED(ch, AFF_DEAFNESS))
			send_to_char("*����*\r\n", ch);
		break;

	case SCMD_PICK:
		if (obj && !pick_otrigger(obj, ch))
			return;
		if (!obj && !pick_wtrigger(world[ch->in_room], ch, door))
			return;
		if (!obj && back && !pick_wtrigger(world[other_room], ch, rev_dir[door]))
			return;
		LOCK_DOOR(ch->in_room, obj, door);
		if (back)
			LOCK_DOOR(other_room, obj, rev_dir[door]);
		send_to_char("����� ����� ����� �������� ��� ����� ��������.\r\n", ch);
		strcpy(local_buf, "$n ����� �������$g ");
		break;
	}

	// Notify the room
	sprintf(local_buf + strlen(local_buf), "%s.", (obj) ? "$p" : (EXIT(ch, door)->vkeyword ? "$F" : "�����"));
	if (!(obj) || (obj->in_room != NOWHERE))
		act(local_buf, FALSE, ch, obj, obj ? 0 : EXIT(ch, door)->vkeyword, TO_ROOM);

	// Notify the other room
	if ((scmd == SCMD_OPEN || scmd == SCMD_CLOSE) && back)
	{
		if ((to = world[EXIT(ch, door)->to_room]->people))
		{
			sprintf(local_buf + strlen(local_buf) - 1, " � ��� �������.");
			for (int stopcount = 0; to && stopcount < 1000; to = to->next_in_room, stopcount++)
			{
				perform_act(local_buf, ch, obj, obj ? 0 : EXIT(ch, door)->vkeyword, to);
			}
		}
	}
}


int ok_pick(CHAR_DATA * ch, obj_vnum keynum, OBJ_DATA* obj, int door, int scmd)
{
	int percent;
	int pickproof = DOOR_IS_PICKPROOF(ch, obj, door);
	percent = number(1, skill_info[SKILL_PICK_LOCK].max_percent);

	if (scmd == SCMD_PICK)
	{
		if (pickproof)
			send_to_char("�� ������� �� ������� �������� ���.\r\n", ch);
		else if (!check_moves(ch, PICKLOCK_MOVES));
		else if (DOOR_LOCK_COMPLEX(ch, obj, door) - ch->get_skill(SKILL_PICK_LOCK) > 10)//Polud ��������� magic number...
		//���� ����� ������ ��������� �� 10 � ����� - ���� ��������� �� ����� ����� ������
			send_to_char("� ����� ������� ������ ���� � �������� �� �������...\r\n", ch);
		else if ((ch->get_skill(SKILL_PICK_LOCK) - DOOR_LOCK_COMPLEX(ch, obj, door) <= 10)  && //���� ����� ������ ��������� �� 10 � ����� - ���� ��������� �� ����� ����� ������
			(percent > train_skill(ch, SKILL_PICK_LOCK, skill_info[SKILL_PICK_LOCK].max_percent, NULL)))
			send_to_char("�������� �� ��� ���� ��� ����������.\r\n", ch);
		else if (get_pick_chance(ch->get_skill(SKILL_PICK_LOCK), DOOR_LOCK_COMPLEX(ch, obj, door)) < number(1,10))
		{
			send_to_char("�� ���-���� ������� ���� �����...\r\n", ch);
			if (obj)
				SET_BIT(GET_OBJ_VAL(obj, 1), CONT_BROKEN);
			if (door > -1)
				SET_BIT(EXIT(ch, door)->exit_info, EX_BROKEN);
		}
		else
			return (1);
		return (0);
	}
	return (1);
}


ACMD(do_gen_door)
{
	int door = -1;
	obj_vnum keynum;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH];
	OBJ_DATA *obj = NULL;
	CHAR_DATA *victim = NULL;
	int where_bits = FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP;

	if (AFF_FLAGGED(ch, AFF_BLIND))
	{
		send_to_char("��������, �� �� �����!\r\n", ch);
		return;
	}

	if (subcmd == SCMD_PICK && !ch->get_skill(SKILL_PICK_LOCK))
	{
		send_to_char("��� ������ ��� ����������.\r\n", ch);
		return;
	}
	skip_spaces(&argument);
	if (!*argument)
	{
		sprintf(buf, "%s ���?\r\n", a_cmd_door[subcmd]);
		send_to_char(CAP(buf), ch);
		return;
	}
	two_arguments(argument, type, dir);

	if (isname(dir, "����� ������� room ground"))
		where_bits = FIND_OBJ_ROOM;
	else if (isname(dir, "��������� inventory"))
		where_bits = FIND_OBJ_INV;
	else if (isname(dir, "���������� equipment"))
		where_bits = FIND_OBJ_EQUIP;

	//������� ���� �����, ������ ������ �������� ��������� �� ������� �����
	door = find_door(ch, type, dir, a_cmd_door[subcmd]);
	//���� ����� �� �������, ��������� ������� � ����������, ���������, �� �����
	if (door < 0)
		if (!generic_find(type, where_bits, ch, &victim, &obj))
		{
			//���� � �������� �� �������, ������ ���� �� ��������� �� ������
			switch (door)
			{
			case -1: //�������� �����������
				send_to_char("�������� �����������.\r\n",ch);
				break;
			case -2: //�� ��������� ������� ����� � ���� �����������
				sprintf(buf, "�� �� ������ '%s' � ���� �������.\r\n", type);
				send_to_char(buf, ch);
				break;
			case -3: //� ���� ����������� ��� ������
				sprintf(buf, "�� �� ������ ��� '%s'.\r\n", a_cmd_door[subcmd]);
				send_to_char(buf, ch);
				break;
			case -4: //�� ������� ����������
				sprintf(buf, "��� �� ������ '%s'?\r\n", a_cmd_door[subcmd]);
				send_to_char(buf, ch);
				break;
			case -5: //����������� ������� ����� ��� �������� �����������
				sprintf(buf, "�� �� ������ ����� '%s'.\r\n", type);
				send_to_char(buf, ch);
				break;
			}
			return;
		}
	if ((obj) || (door >= 0))
	{
		if ((obj) && !IS_IMMORTAL(ch) && (OBJ_FLAGGED(obj, ITEM_NAMED)) && NamedStuff::check_named(ch, obj, true))//������� ������� ���������(���������) ����� ������ ��������
		{
			if (!NamedStuff::wear_msg(ch, obj))
				send_to_char("������� �� �������! ������� �������������!\r\n", ch);
			return;
		}
		keynum = DOOR_KEY(ch, obj, door);
		if ((subcmd == SCMD_CLOSE || subcmd == SCMD_LOCK) && !IS_NPC(ch) && RENTABLE(ch))
			send_to_char("������ ���� �������� �� ����� ������ ��������!\r\n", ch);
		else if (!(DOOR_IS_OPENABLE(ch, obj, door)))
			act("�� ������� �� ������� $F ���!", FALSE, ch, 0, a_cmd_door[subcmd], TO_CHAR);
		else if (!DOOR_IS_OPEN(ch, obj, door)
				 && IS_SET(flags_door[subcmd], NEED_OPEN))
			send_to_char("������-�� ����� �������!\r\n", ch);
		else if (!DOOR_IS_CLOSED(ch, obj, door) && IS_SET(flags_door[subcmd], NEED_CLOSED))
			send_to_char("��� �������!\r\n", ch);
		else if (!(DOOR_IS_LOCKED(ch, obj, door)) && IS_SET(flags_door[subcmd], NEED_LOCKED))
			send_to_char("�� ������� ��� ���...\r\n", ch);
		else if (!(DOOR_IS_UNLOCKED(ch, obj, door)) && IS_SET(flags_door[subcmd], NEED_UNLOCKED))
			send_to_char("���, �������.\r\n", ch);
		else if (!has_key(ch, keynum) && !Privilege::check_flag(ch, Privilege::USE_SKILLS) && ((subcmd == SCMD_LOCK) || (subcmd == SCMD_UNLOCK)))
			send_to_char("� ��� ��� �����.\r\n", ch);
		else if (DOOR_IS_BROKEN(ch, obj, door) && !Privilege::check_flag(ch, Privilege::USE_SKILLS) && ((subcmd == SCMD_LOCK) || (subcmd == SCMD_UNLOCK)))
			send_to_char("����� ������.\r\n", ch);
		else if (ok_pick(ch, keynum, obj, door, subcmd))
			do_doorcmd(ch, obj, door, subcmd);
	}
	return;
}



ACMD(do_enter)
{
	int door, from_room;
	const char *p_str = "�����������";
	struct follow_type *k, *k_next;

	one_argument(argument, buf);

	if (*buf)
//     {if (!str_cmp("�����������",buf))
	{
		if (isname(buf, p_str))
		{
			if (!world[IN_ROOM(ch)]->portal_time)
				send_to_char("�� �� ������ ����� �����������.\r\n", ch);
			else
			{
				from_room = IN_ROOM(ch);
				door = world[IN_ROOM(ch)]->portal_room;
				// �� ������� ������ �� ��������� ����
				if (on_horse(ch) && GET_MOB_HOLD(get_horse(ch)))
				{
					act("$Z $N �� � ��������� ����� ��� �� ����.\r\n",
						FALSE, ch, 0, get_horse(ch), TO_CHAR);
					return;
				}
				// �� ������� � ������� ����� ��, ���� ��� ��� ������� �����
				if (DeathTrap::check_tunnel_death(ch, door))
				{
					send_to_char("Error 4. � ����� � ������� ���������� ��������� �������� ����������.\r\n", ch);
					return;
				}
				// ���� ��� ��� ������, � ������ �������������, �� �� �������
				if (RENTABLE(ch) && !IS_NPC(ch) && !world[door]->portal_time)
				{
					send_to_char("����� ������ ��� ��������������� �������.\r\n", ch);
					return;
				}
				//�������� �� ���� ������_������
				if (ROOM_FLAGGED(door, ROOM_NOHORSE) && on_horse(ch))
				{
					act("$Z $N ������������ ���� ����, � ��� �������� ���������.",
						FALSE, ch, 0, get_horse(ch), TO_CHAR);
					act("$n ��������$g � $N1.", FALSE, ch, 0, get_horse(ch), TO_ROOM | TO_ARENA_LISTEN);
					REMOVE_BIT(AFF_FLAGS(ch, AFF_HORSE), AFF_HORSE);
				}
				//�������� �� ������ � ������
				if (ROOM_FLAGGED(door, ROOM_TUNNEL) &&
						(num_pc_in_room(world[door]) > 0 || on_horse(ch)))
				{
					if (num_pc_in_room(world[door]) > 0)
					{
						send_to_char("������� ���� �����.\r\n", ch);
						return;
					}
					else
					{
						act("$Z $N ���������$U, � ��� �������� ���������.",
							FALSE, ch, 0, get_horse(ch), TO_CHAR);
						act("$n ��������$g � $N1.", FALSE, ch, 0, get_horse(ch), TO_ROOM | TO_ARENA_LISTEN);
						REMOVE_BIT(AFF_FLAGS(ch, AFF_HORSE), AFF_HORSE);
					}
				}
				// ��������� ������ NOTELEPORTIN � NOTELEPORTOUT ����� ��
				if (!IS_IMMORTAL(ch) && ((!IS_NPC(ch) && (!Clan::MayEnter(ch, door, HCE_PORTAL)
											|| (GET_LEVEL(ch) <= 10 && world[door]->portal_time)))
											|| (ROOM_FLAGGED(from_room, ROOM_NOTELEPORTOUT)
												|| ROOM_FLAGGED(door, ROOM_NOTELEPORTIN))
											|| AFF_FLAGGED(ch, AFF_NOTELEPORT)
											|| (world[door]->pkPenterUnique && (ROOM_FLAGGED(door, ROOM_ARENA) || ROOM_FLAGGED(door, ROOM_HOUSE)))
											))
				{
					sprintf(buf, "%s����������� ������������ ���������!%s\r\n",
							CCWHT(ch, C_NRM), CCNRM(ch, C_NRM));
					act(buf, TRUE, ch, 0, 0, TO_CHAR);
					act(buf, TRUE, ch, 0, 0, TO_ROOM);

					send_to_char("������ ������ ��� ���������� �� �����������.\r\n", ch);
					act("$n � ������ �������$g �� �����������.\r\n", TRUE, ch,
						0, 0, TO_ROOM | CHECK_DEAF);
					act("$n �������$g �� �����������.\r\n", TRUE, ch, 0, 0, TO_ROOM | CHECK_NODEAF);
					WAIT_STATE(ch, PULSE_VIOLENCE);
					return;
				}
				act("$n �����$q � �����������.", TRUE, ch, 0, 0, TO_ROOM);
				if (world[from_room]->pkPenterUnique && world[from_room]->pkPenterUnique != GET_UNIQUE(ch) && !IS_IMMORTAL(ch))
				{
					send_to_char(ch, "%s��� �������� ��� �������� ��� ������������ �����������.%s\r\n",
						CCIRED(ch, C_NRM), CCINRM(ch, C_NRM));
					pkPortal(ch);
				}
				char_from_room(ch);
				char_to_room(ch, door);
				set_wait(ch, 3, FALSE);
				act("$n ������$u �� �����������.", TRUE, ch, 0, 0, TO_ROOM);
				// ���� ������ � ������
				for (k = ch->followers; k; k = k_next)
				{
					k_next = k->next;
					if (IS_HORSE(k->follower) &&
							!k->follower->get_fighting() &&
							!GET_MOB_HOLD(k->follower) &&
							IN_ROOM(k->follower) == from_room && AWAKE(k->follower))
					{
						if (!ROOM_FLAGGED(door, ROOM_NOHORSE))
						{
							char_from_room(k->follower);
							char_to_room(k->follower, door);
						}
					}
					if (AFF_FLAGGED(k->follower, AFF_HELPER) &&
							!GET_MOB_HOLD(k->follower) &&
							MOB_FLAGGED(k->follower, MOB_ANGEL) &&
							!k->follower->get_fighting() &&
							IN_ROOM(k->follower) == from_room &&
							AWAKE(k->follower))
					{
						act("$n �����$q � �����������.", TRUE,
							k->follower, 0, 0, TO_ROOM);
						char_from_room(k->follower);
						char_to_room(k->follower, door);
						set_wait(k->follower, 3, FALSE);
						act("$n ������$u �� �����������.", TRUE,
							k->follower, 0, 0, TO_ROOM);
					}
					if (IS_CHARMICE(k->follower) &&
							!GET_MOB_HOLD(k->follower) &&
							GET_POS(k->follower) == POS_STANDING &&
							IN_ROOM(k->follower) == from_room)
					{
						snprintf(buf2, MAX_STRING_LENGTH, "����� �����������");
						command_interpreter(k->follower, buf2);
					}
				}
				if (ch->desc != NULL)
					look_at_room(ch, 0);
			}
		}
		else
		{	// an argument was supplied, search for door keyword
			for (door = 0; door < NUM_OF_DIRS; door++)
			{
				if (EXIT(ch, door)
					&& (isname(buf, EXIT(ch, door)->keyword)
						|| isname(buf, EXIT(ch, door)->vkeyword)))
				{
					perform_move(ch, door, 1, TRUE, 0);
					return;
				}
			}
			sprintf(buf2, "�� �� ����� ����� '%s'.\r\n", buf);
			send_to_char(buf2, ch);
		}
	}
	else if (ROOM_FLAGGED(ch->in_room, ROOM_INDOORS))
		send_to_char("�� ��� ������.\r\n", ch);
	else  			// try to locate an entrance
	{
		for (door = 0; door < NUM_OF_DIRS; door++)
			if (EXIT(ch, door))
				if (EXIT(ch, door)->to_room != NOWHERE)
					if (!EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED) &&
							ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_INDOORS))
					{
						perform_move(ch, door, 1, TRUE, 0);
						return;
					}
		send_to_char("�� �� ������ ����� ����.\r\n", ch);
	}
}


ACMD(do_stand)
{
	if (GET_POS(ch) > POS_SLEEPING && AFF_FLAGGED(ch, AFF_SLEEP))
	{
		send_to_char("�� ������ ������� � ������ ��� ������� ���������.\r\n", ch);
		act("$n ������ ������$a � �����$a ��� ������� ���������.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_SLEEPING;
	}

	if (on_horse(ch))
	{
		act("������ �����, ��� ����� ������ � $N1.", FALSE, ch, 0, get_horse(ch), TO_CHAR);
		return;
	}
	switch (GET_POS(ch))
	{
	case POS_STANDING:
		send_to_char("� �� ��� ������.\r\n", ch);
		break;
	case POS_SITTING:
		send_to_char("�� ������.\r\n", ch);
		act("$n ������$u.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		// Will be sitting after a successful bash and may still be fighting.
		GET_POS(ch) = ch->get_fighting() ? POS_FIGHTING : POS_STANDING;
		break;
	case POS_RESTING:
		send_to_char("�� ���������� �������� � ������.\r\n", ch);
		act("$n ���������$g ����� � ������$u.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = ch->get_fighting() ? POS_FIGHTING : POS_STANDING;
		break;
	case POS_SLEEPING:
		send_to_char("�������, ������� ����� ����������!\r\n", ch);
		break;
	case POS_FIGHTING:
		send_to_char("�� ������� ����? ��� ���-�� ���������.\r\n", ch);
		break;
	default:
		send_to_char("�� ���������� ������ � ���������� �� ������� �����.\r\n", ch);
		act("$n �������$u �� �����.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_STANDING;
		break;
	}
}


ACMD(do_sit)
{
	if (on_horse(ch))
	{
		act("������ �����, ��� ����� ������ � $N1.", FALSE, ch, 0, get_horse(ch), TO_CHAR);
		return;
	}
	switch (GET_POS(ch))
	{
	case POS_STANDING:
		send_to_char("�� ����.\r\n", ch);
		act("$n ���$g.", FALSE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_SITTING;
		break;
	case POS_SITTING:
		send_to_char("� �� � ��� ������.\r\n", ch);
		break;
	case POS_RESTING:
		send_to_char("�� ���������� �������� � ����.\r\n", ch);
		act("$n �������$g ����� � ���$g.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_SITTING;
		break;
	case POS_SLEEPING:
		send_to_char("��� ����� ����������.\r\n", ch);
		break;
	case POS_FIGHTING:
		send_to_char("�����? �� ����� ���? �� ���� �� � ����.\r\n", ch);
		break;
	default:
		send_to_char("�� ���������� ���� ����� � ����.\r\n", ch);
		act("$n ���������$g ���� ����� � ���$g.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_SITTING;
		break;
	}
}


ACMD(do_rest)
{
	if (on_horse(ch))
	{
		act("������ �����, ��� ����� ������ � $N1.", FALSE, ch, 0, get_horse(ch), TO_CHAR);
		return;
	}
	switch (GET_POS(ch))
	{
	case POS_STANDING:
		send_to_char("�� ������� ���������.\r\n", ch);
		act("$n ������$g ���������.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_RESTING;
		break;
	case POS_SITTING:
		send_to_char("�� ������������ ��������� ��� ������.\r\n", ch);
		act("$n ���������$u ��������� ��� ������.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_RESTING;
		break;
	case POS_RESTING:
		send_to_char("�� � ��� ���������.\r\n", ch);
		break;
	case POS_SLEEPING:
		send_to_char("��� ����� ������� ����������.\r\n", ch);
		break;
	case POS_FIGHTING:
		send_to_char("������ � ��� ��� �� �����!\r\n", ch);
		break;
	default:
		send_to_char("�� ���������� ����� � ������� ���������.\r\n", ch);
		act("$n ���������$g ����� � ���������$u ��������� ��� ������.", FALSE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_SITTING;
		break;
	}
}


ACMD(do_sleep)
{
	if (GET_LEVEL(ch) >= LVL_IMMORT)
	{
		send_to_char("�� ����� ��� �����, ������ � ���������!\r\n", ch);
		return;
	}
	if (on_horse(ch))
	{
		act("������ �����, ��� ����� ������ � $N1.", FALSE, ch, 0, get_horse(ch), TO_CHAR);
		return;
	}
	switch (GET_POS(ch))
	{
	case POS_STANDING:
	case POS_SITTING:
	case POS_RESTING:
		send_to_char("�� �������.\r\n", ch);
		act("$n ������ ������$g � �����$g �������.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_SLEEPING;
		break;
	case POS_SLEEPING:
		send_to_char("� �� � ��� �����.\r\n", ch);
		break;
	case POS_FIGHTING:
		send_to_char("��� ����� ���������! ���������� ����� ������.\r\n", ch);
		break;
	default:
		send_to_char("�� ���������� ���� ����� � ������ �� ���.\r\n", ch);
		act("$n ���������$g ������ � ����� ������$g.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_SLEEPING;
		break;
	}
}

ACMD(do_horseon)
{
	CHAR_DATA *horse;

	if (IS_NPC(ch))
		return;

	if (!get_horse(ch))
	{
		send_to_char("� ��� ��� �������.\r\n", ch);
		return;
	}

	if (on_horse(ch))
	{
		send_to_char("�� ��������� ������� �� ���� �������.\r\n", ch);
		return;
	}

	one_argument(argument, arg);
	if (*arg)
		horse = get_char_vis(ch, arg, FIND_CHAR_ROOM);
	else
		horse = get_horse(ch);

	if (horse == NULL)
		send_to_char(NOPERSON, ch);
	else if (IN_ROOM(horse) != IN_ROOM(ch))
		send_to_char("��� ������ ������ �� ���.\r\n", ch);
	else if (!IS_HORSE(horse))
		send_to_char("��� �� ������.\r\n", ch);
	else if (horse->master != ch)
		send_to_char("��� �� ��� ������.\r\n", ch);
	else if (GET_POS(horse) < POS_FIGHTING)
		act("$N �� ������ ��� ����� � ����� ���������.", FALSE, ch, 0, horse, TO_CHAR);
	else if (AFF_FLAGGED(horse, AFF_TETHERED))
		act("��� ����� �������� $N3.", FALSE, ch, 0, horse, TO_CHAR);
	//���� �� ���������� � ��������
	else if (ROOM_FLAGGED(ch->in_room, ROOM_TUNNEL))
		send_to_char("������� ���� �����.\r\n", ch);
	else
	{
		if (affected_by_spell(ch, SPELL_SNEAK))
			affect_from_char(ch, SPELL_SNEAK);
		if (affected_by_spell(ch, SPELL_CAMOUFLAGE))
			affect_from_char(ch, SPELL_CAMOUFLAGE);
		act("�� ���������� �� ����� $N1.", FALSE, ch, 0, horse, TO_CHAR);
		act("$n �������$g �� $N3.", FALSE, ch, 0, horse, TO_ROOM | TO_ARENA_LISTEN);
		SET_BIT(AFF_FLAGS(ch, AFF_HORSE), AFF_HORSE);
	}
}

ACMD(do_horseoff)
{
	CHAR_DATA *horse;

	if (IS_NPC(ch))
		return;
	if (!(horse = get_horse(ch)))
	{
		send_to_char("� ��� ��� �������.\r\n", ch);
		return;
	}

	if (!on_horse(ch))
	{
		send_to_char("�� ���� � ��� �� �� ������.", ch);
		return;
	}

	act("�� ������ �� ����� $N1.", FALSE, ch, 0, horse, TO_CHAR);
	act("$n ��������$g � $N1.", FALSE, ch, 0, horse, TO_ROOM | TO_ARENA_LISTEN);
	REMOVE_BIT(AFF_FLAGS(ch, AFF_HORSE), AFF_HORSE);
}

ACMD(do_horseget)
{
	CHAR_DATA *horse;

	if (IS_NPC(ch))
		return;

	if (!get_horse(ch))
	{
		send_to_char("� ��� ��� �������.\r\n", ch);
		return;
	}

	if (on_horse(ch))
	{
		send_to_char("�� ��� ������ �� �������.\r\n", ch);
		return;
	}

	one_argument(argument, arg);
	if (*arg)
		horse = get_char_vis(ch, arg, FIND_CHAR_ROOM);
	else
		horse = get_horse(ch);

	if (horse == NULL)
		send_to_char(NOPERSON, ch);
	else if (IN_ROOM(horse) != IN_ROOM(ch))
		send_to_char("��� ������ ������ �� ���.\r\n", ch);
	else if (!IS_HORSE(horse))
		send_to_char("��� �� ������.\r\n", ch);
	else if (horse->master != ch)
		send_to_char("��� �� ��� ������.\r\n", ch);
	else if (!AFF_FLAGGED(horse, AFF_TETHERED))
		act("� $N � �� ��������$A.", FALSE, ch, 0, horse, TO_CHAR);
	else
	{
		act("�� �������� $N3.", FALSE, ch, 0, horse, TO_CHAR);
		act("$n �������$g $N3.", FALSE, ch, 0, horse, TO_ROOM | TO_ARENA_LISTEN);
		REMOVE_BIT(AFF_FLAGS(horse, AFF_TETHERED), AFF_TETHERED);
	}
}


ACMD(do_horseput)
{
	CHAR_DATA *horse;

	if (IS_NPC(ch))
		return;
	if (!get_horse(ch))
	{
		send_to_char("� ��� ��� �������.\r\n", ch);
		return;
	}

	if (on_horse(ch))
	{
		send_to_char("��� ����� ������ �� �������.\r\n", ch);
		return;
	}

	one_argument(argument, arg);
	if (*arg)
		horse = get_char_vis(ch, arg, FIND_CHAR_ROOM);
	else
		horse = get_horse(ch);
	if (horse == NULL)
		send_to_char(NOPERSON, ch);
	else if (IN_ROOM(horse) != IN_ROOM(ch))
		send_to_char("��� ������ ������ �� ���.\r\n", ch);
	else if (!IS_HORSE(horse))
		send_to_char("��� �� ������.\r\n", ch);
	else if (horse->master != ch)
		send_to_char("��� �� ��� ������.\r\n", ch);
	else if (AFF_FLAGGED(horse, AFF_TETHERED))
		act("� $N ��� � ��� ��������$A.", FALSE, ch, 0, horse, TO_CHAR);
	else
	{
		act("�� ��������� $N3.", FALSE, ch, 0, horse, TO_CHAR);
		act("$n ��������$g $N3.", FALSE, ch, 0, horse, TO_ROOM | TO_ARENA_LISTEN);
		SET_BIT(AFF_FLAGS(horse, AFF_TETHERED), AFF_TETHERED);
	}
}


ACMD(do_horsetake)
{
	int percent, prob;
	CHAR_DATA *horse = NULL;

	if (IS_NPC(ch))
		return;

	if (get_horse(ch))
	{
		send_to_char("����� ��� ������� ��������?\r\n", ch);
		return;
	}

	if (ch->is_morphed())
	{
		send_to_char("� ��� �� ����������� ��� ��������� ��� ��� � ��� ���, ������ ������?\r\n", ch);
		return;
	}

	one_argument(argument, arg);
	if (*arg)
		horse = get_char_vis(ch, arg, FIND_CHAR_ROOM);

	if (horse == NULL)
	{
		send_to_char(NOPERSON, ch);
		return;
	}
	else if (!IS_NPC(horse))
	{
		send_to_char("�������, �� ����...\r\n", ch);
		return;
	}
	// �������� ������ �� �������� �������� �������. -- ������ (13.10.10)
	else if (!IS_GOD(ch) && !MOB_FLAGGED(horse, MOB_MOUNTING) && !((horse->master) && AFF_FLAGGED(horse, AFF_HORSE)))
	{
		act("�� �� ������� �������� $N3.", FALSE, ch, 0, horse, TO_CHAR);
		return;
	}
	else if (get_horse(ch))
	{
		if (get_horse(ch) == horse)
			act("�� ����� ������� $S ��� ���.", FALSE, ch, 0, horse, TO_CHAR);
		else
			send_to_char("��� �� ������� ����� �� ���� ��������.\r\n", ch);
		return;
	}
	else if (GET_POS(horse) < POS_STANDING)
	{
		act("$N �� ������ ����� ����� ��������.", FALSE, ch, 0, horse, TO_CHAR);
		return;
	}
	else if (IS_HORSE(horse))
	{
		if (!IS_IMMORTAL(ch) && !(ch->get_skill(SKILL_STEAL)))
		{
			send_to_char("��� �� ��� ������.\r\n", ch);
			return;
		}
		if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL) && !(IS_IMMORTAL(ch) || GET_GOD_FLAG(ch, GF_GODSLIKE)))
		{
			send_to_char("��� �� ������� ���������� ������������� � ����� ������ �����.\r\n", ch);
			return;
		}
		if (on_horse(horse->master))
		{
			send_to_char("�� �� ������� ������ ������� ��-��� ������.\r\n", ch);
			return;
		}
		if (!IS_IMMORTAL(ch))
		{
			if (!ch->get_skill(SKILL_STEAL))
			{
				send_to_char("�� �� ������ ��������.\r\n", ch);
				return;
			}
			if (IS_IMMORTAL(horse->master) || GET_GOD_FLAG(horse->master, GF_GODSLIKE))
			{
				send_to_char("�� ������������ ������� ������� � ������ �������� ��������.\r\n", ch);
				return;
			}
			pk_thiefs_action(ch, horse->master);
			percent = number(1, skill_info[SKILL_STEAL].max_percent);
			if (AWAKE(horse->master) && (IN_ROOM(ch) == IN_ROOM(horse->master)))
				percent += 50;
			if (AFF_FLAGGED(horse, AFF_TETHERED))
				percent += 10;
			prob = train_skill(ch, SKILL_STEAL, skill_info[SKILL_STEAL].max_percent, 0);
			if (percent > prob)
			{
				act("�� �������� ���������� ������� ������� � $N1.", FALSE, ch,
					0, horse->master, TO_CHAR);
				act("$n �������� �������$u ������� ������� � $N1.", TRUE, ch,
					0, horse->master, TO_NOTVICT | TO_ARENA_LISTEN);
				if (IN_ROOM(ch) == IN_ROOM(horse->master))
					act("$n �����$u ������ ������ �������!", FALSE, ch, 0, horse->master, TO_VICT);
				WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
				return;
			}
		}
	}
	if (stop_follower(horse, SF_EMPTY))
		return;
	act("�� �������� $N3.", FALSE, ch, 0, horse, TO_CHAR);
	act("$n �������$g $N3.", FALSE, ch, 0, horse, TO_ROOM | TO_ARENA_LISTEN);
	make_horse(horse, ch);
}

ACMD(do_givehorse)
{
	CHAR_DATA *horse, *victim;

	if (IS_NPC(ch))
		return;

	if (!(horse = get_horse(ch)))
	{
		send_to_char("�� ���� � ��� �������.\r\n", ch);
		return;
	}
	if (!has_horse(ch, TRUE))
	{
		send_to_char("��� ������ ������ �� ���.\r\n", ch);
		return;
	}
	one_argument(argument, arg);
	if (!*arg)
	{
		send_to_char("���� �� ������ �������� �������?\r\n", ch);
		return;
	}
	if (!(victim = get_char_vis(ch, arg, FIND_CHAR_ROOM)))
	{
		send_to_char("��� ������ �������� �������.\r\n", ch);
		return;
	}
	else if (IS_NPC(victim))
	{
		send_to_char("�� � ��� ����� ���������.\r\n", ch);
		return;
	}
	if (get_horse(victim))
	{
		act("� $N1 ��� ���� ������.\r\n", FALSE, ch, 0, victim, TO_CHAR);
		return;
	}
	if (on_horse(ch))
	{
		send_to_char("��� ����� ������ �� �������.\r\n", ch);
		return;
	}
	if (AFF_FLAGGED(horse, AFF_TETHERED))
	{
		send_to_char("��� ����� ������ �������� ������ �������.\r\n", ch);
		return;
	}
	// ��������� �������� ��� �������� ����������� � ������ ������ ��� �� ������� �� �������������� ���� -- Krodo
	if (stop_follower(horse, SF_EMPTY))
		return;
	act("�� �������� ������ ������� $N2.", FALSE, ch, 0, victim, TO_CHAR);
	act("$n �������$g ��� ������ �������.", FALSE, ch, 0, victim, TO_VICT);
	act("$n �������$g ������ ������� $N2.", TRUE, ch, 0, victim, TO_NOTVICT | TO_ARENA_LISTEN);
	make_horse(horse, victim);
}

ACMD(do_stophorse)
{
	CHAR_DATA *horse;

	if (IS_NPC(ch))
		return;

	if (!(horse = get_horse(ch)))
	{
		send_to_char("�� ���� � ��� �������.\r\n", ch);
		return;
	}
	if (!has_horse(ch, TRUE))
	{
		send_to_char("��� ������ ������ �� ���.\r\n", ch);
		return;
	}
	if (on_horse(ch))
	{
		send_to_char("��� ����� ������ �� �������.\r\n", ch);
		return;
	}
	if (AFF_FLAGGED(horse, AFF_TETHERED))
	{
		send_to_char("��� ����� ������ �������� ������ �������.\r\n", ch);
		return;
	}
	if (stop_follower(horse, SF_EMPTY))
		return;
	act("�� ��������� $N3.", FALSE, ch, 0, horse, TO_CHAR);
	act("$n ��������$g $N3.", FALSE, ch, 0, horse, TO_ROOM | TO_ARENA_LISTEN);
	if (GET_MOB_VNUM(horse) == HORSE_VNUM)
	{
		act("$n ������$g � ���� �������.\r\n", FALSE, horse, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		extract_char(horse, FALSE);
	}
}





ACMD(do_wake)
{
	CHAR_DATA *vict;
	int self = 0;

	one_argument(argument, arg);

	if (subcmd == SCMD_WAKEUP)
	{
		if (!(*arg))
		{
			send_to_char("���� ������ �� �����???\r\n", ch);
			return;
		}
	}
	else
	{
		*arg = 0;
	}

	if (*arg)
	{
		if (GET_POS(ch) == POS_SLEEPING)
			send_to_char("����� ���� ��� ����� ����������?\r\n", ch);
		else if ((vict = get_char_vis(ch, arg, FIND_CHAR_ROOM)) == NULL)
			send_to_char(NOPERSON, ch);
		else if (vict == ch)
			self = 1;
		else if (AWAKE(vict))
			act("$E � �� ����$G.", FALSE, ch, 0, vict, TO_CHAR);
		else if (GET_POS(vict) < POS_SLEEPING)
			act("$M ��� �����! �������� $S � �����!", FALSE, ch, 0, vict, TO_CHAR);
		else
		{
			act("�� $S ���������.", FALSE, ch, 0, vict, TO_CHAR);
			act("$n ���������$g ���.", FALSE, ch, 0, vict, TO_VICT | TO_SLEEP);
			GET_POS(vict) = POS_SITTING;
		}
		if (!self)
			return;
	}
	if (AFF_FLAGGED(ch, AFF_SLEEP))
		send_to_char("�� �� ������ ����������!\r\n", ch);
	else if (GET_POS(ch) > POS_SLEEPING)
		send_to_char("� �� � �� �����...\r\n", ch);
	else
	{
		send_to_char("�� ���������� � ����.\r\n", ch);
		act("$n �������$u.", TRUE, ch, 0, 0, TO_ROOM | TO_ARENA_LISTEN);
		GET_POS(ch) = POS_SITTING;
	}
}


ACMD(do_follow)
{
	CHAR_DATA *leader;
	struct follow_type *f;

	one_argument(argument, buf);

	if (IS_NPC(ch) && AFF_FLAGGED(ch, AFF_CHARM) && ch->get_fighting())
		return;

	if (*buf)
	{
		if (!str_cmp(buf, "�") || !str_cmp(buf, "self") || !str_cmp(buf, "me"))
		{
			if (!ch->master)
				send_to_char("�� �� ���� �� �� ��� �� ��������...\r\n", ch);
			else
				stop_follower(ch, SF_EMPTY);
			return;
		}
		if (!(leader = get_char_vis(ch, buf, FIND_CHAR_ROOM)))
		{
			send_to_char(NOPERSON, ch);
			return;
		}
	}
	else
	{
		send_to_char("�� ��� �� ������ ���������?\r\n", ch);
		return;
	}

	if (ch->master == leader)
	{
		act("�� ��� �������� �� $N4.", FALSE, ch, 0, leader, TO_CHAR);
		return;
	}
	if (AFF_FLAGGED(ch, AFF_CHARM) && (ch->master))
	{
		act("�� �� ������ ��������� ������ �� $N4!", FALSE, ch, 0, ch->master, TO_CHAR);
	}
	else  		// Not Charmed follow person
	{
		if (leader == ch)
		{
			if (!ch->master)
			{
				send_to_char("�� ��� �������� �� �����.\r\n", ch);
				return;
			}
			stop_follower(ch, SF_EMPTY);
		}
		else  	//log("[Follow] Check circle...");
		{
			if (circle_follow(ch, leader))
			{
				send_to_char("��� � ��� ����� ������� ���������.\r\n", ch);
				return;
			}
			//log("[Follow] Stop last follow...");
			if (ch->master)
				stop_follower(ch, SF_EMPTY);
			REMOVE_BIT(AFF_FLAGS(ch, AFF_GROUP), AFF_GROUP);
			//also removing AFF_GROUP flag from all followers
			for (f = ch->followers; f; f = f->next)
				REMOVE_BIT(AFF_FLAGS(f->follower, AFF_GROUP), AFF_GROUP);
			//log("[Follow] Start new follow...");
			add_follower(ch, leader);
			//log("[Follow] Stop function...");
		}
	}
}
