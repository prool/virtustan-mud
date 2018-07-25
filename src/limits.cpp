/* ************************************************************************
*   File: limits.cpp                                      Part of Bylins    *
*  Usage: limits & gain funcs for HMV, exp, hunger/thirst, idle time      *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
* 									  *
*  $Author$                                                        *
*  $Date$                                           *
*  $Revision$                                                      *
************************************************************************ */

#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "spells.h"
#include "skills.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "screen.h"
#include "interpreter.h"
#include "constants.h"
#include "dg_scripts.h"
#include "house.h"
#include "constants.h"
#include "exchange.h"
#include "top.h"
#include "deathtrap.hpp"
#include "ban.hpp"
#include "depot.hpp"
#include "glory.hpp"
#include "features.hpp"
#include "char.hpp"
#include "char_player.hpp"
#include "room.hpp"
#include "birth_places.hpp"
#include "objsave.h"
#include "fight.h"
#include "ext_money.hpp"
#include "mob_stat.hpp"

#include "virtustan.h" // prool

extern int check_dupes_host(DESCRIPTOR_DATA * d, bool autocheck = 0);

extern room_rnum r_unreg_start_room;

extern CHAR_DATA *character_list;
extern CHAR_DATA *mob_proto;
extern OBJ_DATA *object_list;

extern DESCRIPTOR_DATA *descriptor_list;
extern struct zone_data *zone_table;
extern INDEX_DATA *obj_index;
extern int idle_rent_time;
extern int idle_max_level;
extern int idle_void;
extern int free_rent;
extern unsigned long dg_global_pulse;
extern room_rnum r_mortal_start_room;
extern room_rnum r_immort_start_room;
extern room_rnum r_helled_start_room;
extern room_rnum r_named_start_room;
extern struct spell_create_type spell_create[];
extern const unsigned RECALL_SPELLS_INTERVAL;
extern int CheckProxy(DESCRIPTOR_DATA * ch);
extern int check_death_ice(int room, CHAR_DATA * ch);
extern int get_max_slot(CHAR_DATA* ch);

extern char mudname[]; // prool

void decrease_level(CHAR_DATA * ch);
int max_exp_gain_pc(CHAR_DATA * ch);
int max_exp_loss_pc(CHAR_DATA * ch);
int average_day_temp(void);
int calc_loadroom(CHAR_DATA * ch, int bplace_mode = BIRTH_PLACE_UNDEFINED);

int mag_manacost(CHAR_DATA * ch, int spellnum);

// local functions
int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6);
int level_exp(CHAR_DATA * ch, int level);
void update_char_objects(CHAR_DATA * ch);	// handler.cpp
// Delete this, if you delete overflow fix in beat_points_update below.
void die(CHAR_DATA * ch, CHAR_DATA * killer);

// When age < 20 return the value p0 //
// When age in 20..29 calculate the line between p1 & p2 //
// When age in 30..34 calculate the line between p2 & p3 //
// When age in 35..49 calculate the line between p3 & p4 //
// When age in 50..74 calculate the line between p4 & p5 //
// When age >= 75 return the value p6 //
int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6)
{

	if (age < 20)
		return (p0);	// < 20   //
	else if (age <= 29)
		return (int)(p1 + (((age - 20) * (p2 - p1)) / 10));	// 20..29 //
	else if (age <= 34)
		return (int)(p2 + (((age - 30) * (p3 - p2)) / 5));	// 30..34 //
	else if (age <= 49)
		return (int)(p3 + (((age - 35) * (p4 - p3)) / 15));	// 35..59 //
	else if (age <= 74)
		return (int)(p4 + (((age - 50) * (p5 - p4)) / 25));	// 50..74 //
	else
		return (p6);	// >= 80 //
}

void handle_recall_spells(CHAR_DATA* ch)
{
	AFFECT_DATA* aff = NULL;
	for(AFFECT_DATA* af = ch->affected; af; af = af->next)
		if (af->type == SPELL_RECALL_SPELLS)
		{
			aff = af;
			break;
		}
	if (!aff) return;
	//������������ ��������� ���� ����
	unsigned max_slot = get_max_slot(ch);
	//������������ ������ ������ RECALL_SPELLS_INTERVAL ������
	int secs_left = (SECS_PER_PLAYER_AFFECT*aff->duration)/SECS_PER_MUD_HOUR -SECS_PER_PLAYER_AFFECT;
	if (secs_left / RECALL_SPELLS_INTERVAL < max_slot -aff->modifier || secs_left <= 2)
	{
		int slot_to_restore = aff->modifier++;

		bool found_spells = false;
		struct spell_mem_queue_item *next = NULL, *prev=NULL, *i = ch->MemQueue.queue;
		while (i)
		{
			next = i->link;
			if (spell_info[i->spellnum].slot_forc[(int) GET_CLASS(ch)][(int) GET_KIN(ch)] == slot_to_restore)
			{
				if (!found_spells)
				{
					send_to_char("���� ������ �����������, � ������ ������� ��������� ����� ����������.\r\n", ch);
					found_spells = true;
				}
				if (prev) prev->link = next;
				if (i == ch->MemQueue.queue)
				{
					 ch->MemQueue.queue = next;
					GET_MEM_COMPLETED(ch) = 0;
				}
				GET_MEM_TOTAL(ch) = MAX(0, GET_MEM_TOTAL(ch) - mag_manacost(ch, i->spellnum));
				sprintf(buf, "�� ��������� ���������� \"%s%s%s\".\r\n",
						CCICYN(ch, C_NRM), spell_info[i->spellnum].name, CCNRM(ch, C_NRM));
				send_to_char(buf, ch);
				GET_SPELL_MEM(ch, i->spellnum)++;
				free(i);
			} else prev = i;
			i = next;
		}
	}
}

/*
 * The hit_limit, mana_limit, and move_limit functions are gone.  They
 * added an unnecessary level of complexity to the internal structure,
 * weren't particularly useful, and led to some annoying bugs.  From the
 * players' point of view, the only difference the removal of these
 * functions will make is that a character's age will now only affect
 * the HMV gain per tick, and _not_ the HMV maximums.
 */

// manapoint gain pr. game hour
int mana_gain(CHAR_DATA * ch)
{
	int gain = 0, restore = int_app[GET_REAL_INT(ch)].mana_per_tic, percent = 100;
	int stopmem = FALSE;

	if (IS_NPC(ch))
	{
		gain = GET_LEVEL(ch);
	}
	else
	{
		if (!ch->desc || STATE(ch->desc) != CON_PLAYING)
			return (0);

		if (!IS_MANA_CASTER(ch))
			gain =
				graf(age(ch)->year, restore - 8, restore - 4, restore,
					 restore + 5, restore, restore - 4, restore - 8);
		else
			gain = mana_gain_cs[GET_REAL_INT(ch)];

		// Room specification
		if (LIKE_ROOM(ch))
			percent += 25;
		// Weather specification
		if (average_day_temp() < -20)
			percent -= 10;
		else if (average_day_temp() < -10)
			percent -= 5;
	}

	if (world[IN_ROOM(ch)]->fires)
		percent += MAX(50, 10 + world[IN_ROOM(ch)]->fires * 5);

	if (AFF_FLAGGED(ch, AFF_DEAFNESS))
		percent += 15;

	// Skill/Spell calculations


	// Position calculations
	if (ch->get_fighting())
		percent -= 90;
	else
		switch (GET_POS(ch))
		{
		case POS_SLEEPING:
			if (IS_MANA_CASTER(ch))
			{
				percent += 80;
			}
			else
			{
				stopmem = TRUE;
				percent = 0;
			}
			break;
		case POS_RESTING:
			percent += 45;
			break;
		case POS_SITTING:
			percent += 30;
			break;
		case POS_STANDING:
			break;
		default:
			stopmem = TRUE;
			percent = 0;
			break;
		}

	if (!IS_MANA_CASTER(ch) &&
			(AFF_FLAGGED(ch, AFF_HOLD) ||
			 AFF_FLAGGED(ch, AFF_BLIND) ||
			 AFF_FLAGGED(ch, AFF_SLEEP) ||
			 ((IN_ROOM(ch) != NOWHERE) && IS_DARK(IN_ROOM(ch)) && !can_use_feat(ch, DARK_READING_FEAT))))
	{
		stopmem = TRUE;
		percent = 0;
	}
	if (!IS_NPC(ch))
	{
		if (GET_COND(ch, FULL) == 0)
			percent -= 50;
		if (GET_COND(ch, THIRST) == 0)
			percent -= 25;
		if (GET_COND(ch, DRUNK) >= CHAR_DRUNKED)
			percent -= 10;
	}

	if (!IS_MANA_CASTER(ch))
		percent += GET_MANAREG(ch);
	if (AFF_FLAGGED(ch, AFF_POISON) && percent > 0)
		percent /= 4;
	percent = MAX(0, MIN(250, percent));
	gain = gain * percent / 100;
	return (stopmem ? 0 : gain);
}


// Hitpoint gain pr. game hour
int hit_gain(CHAR_DATA * ch)
{
	int gain = 0, restore = MAX(10, GET_REAL_CON(ch) * 3 / 2), percent = 100;

	if (IS_NPC(ch))
		gain = GET_LEVEL(ch) + GET_REAL_CON(ch);
	else
	{
		if (!ch->desc || STATE(ch->desc) != CON_PLAYING)
			return (0);

		if (!AFF_FLAGGED(ch, AFF_NOOB_REGEN))
		{
			gain = graf(age(ch)->year, restore - 3, restore, restore, restore - 2,
				restore - 3, restore - 5, restore - 7);
		}
		else
		{
			const double base_hp = std::max(1, PlayerSystem::con_total_hp(ch));
			const double rest_time = 80 + 10 * GET_LEVEL(ch);
			gain = base_hp / rest_time * 60;
		}

		// Room specification    //
		if (LIKE_ROOM(ch))
			percent += 25;
		// Weather specification //
		if (average_day_temp() < -20)
			percent -= 15;
		else if (average_day_temp() < -10)
			percent -= 10;
	}

	if (world[IN_ROOM(ch)]->fires)
		percent += MAX(50, 10 + world[IN_ROOM(ch)]->fires * 5);

	// Skill/Spell calculations //

	// Position calculations    //
	switch (GET_POS(ch))
	{
	case POS_SLEEPING:
		percent += 25;
		break;
	case POS_RESTING:
		percent += 15;
		break;
	case POS_SITTING:
		percent += 10;
		break;
	}

	if (!IS_NPC(ch))
	{
		if (GET_COND(ch, FULL) == 0)
			percent -= 50;
		if (GET_COND(ch, THIRST) == 0)
			percent -= 25;
	}

	percent += GET_HITREG(ch);

	// TODO: ������������ �� apply_������
	if (AFF_FLAGGED(ch, AFF_POISON) && percent > 0)
		percent /= 4;

	percent = MAX(0, MIN(250, percent));
	gain = gain * percent / 100;
	if (!IS_NPC(ch))
	{
		if (GET_POS(ch) == POS_INCAP || GET_POS(ch) == POS_MORTALLYW)
			gain = 0;
	}
	return (gain);
}



// move gain pr. game hour //
int move_gain(CHAR_DATA * ch)
{
	int gain = 0, restore = GET_REAL_CON(ch) / 2, percent = 100;

	if (IS_NPC(ch))
		gain = GET_LEVEL(ch);
	else
	{
		if (!ch->desc || STATE(ch->desc) != CON_PLAYING)
			return (0);
		gain =
			graf(age(ch)->year, 15 + restore, 20 + restore, 25 + restore,
				 20 + restore, 16 + restore, 12 + restore, 8 + restore);
		// Room specification    //
		if (LIKE_ROOM(ch))
			percent += 25;
		// Weather specification //
		if (average_day_temp() < -20)
			percent -= 10;
		else if (average_day_temp() < -10)
			percent -= 5;
	}

	if (world[IN_ROOM(ch)]->fires)
		percent += MAX(50, 10 + world[IN_ROOM(ch)]->fires * 5);

	// Class/Level calculations //

	// Skill/Spell calculations //


	// Position calculations    //
	switch (GET_POS(ch))
	{
	case POS_SLEEPING:
		percent += 25;
		break;
	case POS_RESTING:
		percent += 15;
		break;
	case POS_SITTING:
		percent += 10;
		break;
	}

	if (!IS_NPC(ch))
	{
		if (GET_COND(ch, FULL) == 0)
			percent -= 50;
		if (GET_COND(ch, THIRST) == 0)
			percent -= 25;
		if (!IS_IMMORTAL(ch) && affected_by_spell(ch, SPELL_HIDE))
			percent -= 20;
		if (!IS_IMMORTAL(ch) && affected_by_spell(ch, SPELL_CAMOUFLAGE))
			percent -= 30;
	}

	percent += GET_MOVEREG(ch);
	if (AFF_FLAGGED(ch, AFF_POISON) && percent > 0)
		percent /= 4;
	percent = MAX(0, MIN(250, percent));
	gain = gain * percent / 100;
	return (gain);
}

#define MINUTE            60
#define UPDATE_PC_ON_BEAT TRUE

int interpolate(int min_value, int pulse)
{
	int sign = 1, int_p, frac_p, i, carry, x, y;

	if (min_value < 0)
	{
		sign = -1;
		min_value = -min_value;
	}
	int_p = min_value / MINUTE;
	frac_p = min_value % MINUTE;
	if (!frac_p)
		return (sign * int_p);
	pulse = time(NULL) % MINUTE + 1;
	x = MINUTE;
	y = 0;
	for (i = 0, carry = 0; i <= pulse; i++)
	{
		y += frac_p;
		if (y >= x)
		{
			x += MINUTE;
			carry = 1;
		}
		else
			carry = 0;
	}
	return (sign * (int_p + carry));
}
void beat_punish(CHAR_DATA * i)
{
	int restore;
	// ��������� �� ������ ���� �� �������
	if (PLR_FLAGGED(i, PLR_HELLED) && HELL_DURATION(i) && HELL_DURATION(i) <= time(NULL))
	{
		restore = PLR_TOG_CHK(i, PLR_HELLED);
		if (HELL_REASON(i))
			free(HELL_REASON(i));
		HELL_REASON(i) = 0;
		GET_HELL_LEV(i) = 0;
		HELL_GODID(i) = 0;
		HELL_DURATION(i) = 0;
		send_to_char("��� ��������� �� �������.\r\n", i);
		if ((restore = GET_LOADROOM(i)) == NOWHERE)
			restore = calc_loadroom(i);
		restore = real_room(restore);
		if (restore == NOWHERE)
		{
			if (GET_LEVEL(i) >= LVL_IMMORT)
				restore = r_immort_start_room;
			else
				restore = r_mortal_start_room;
		}
		char_from_room(i);
		char_to_room(i, restore);
		look_at_room(i, restore);
		act("����������� \"�� ������ �� ������...\", $n ������$u � ������ �������.",
			FALSE, i, 0, 0, TO_ROOM);
	}
	if (PLR_FLAGGED(i, PLR_NAMED) && NAME_DURATION(i) && NAME_DURATION(i) <= time(NULL))
	{
		restore = PLR_TOG_CHK(i, PLR_NAMED);
		if (NAME_REASON(i))
			free(NAME_REASON(i));
		NAME_REASON(i) = 0;
		GET_NAME_LEV(i) = 0;
		NAME_GODID(i) = 0;
		NAME_DURATION(i) = 0;
		send_to_char("��� ��������� �� ������� �����.\r\n", i);

		if ((restore = GET_LOADROOM(i)) == NOWHERE)
			restore = calc_loadroom(i);
		restore = real_room(restore);
		if (restore == NOWHERE)
		{
			if (GET_LEVEL(i) >= LVL_IMMORT)
				restore = r_immort_start_room;
			else
				restore = r_mortal_start_room;
		}
		char_from_room(i);
		char_to_room(i, restore);
		look_at_room(i, restore);
		act("� ����� \"����, ������, ����...\", $n ������$u � ������ �������.",
			FALSE, i, 0, 0, TO_ROOM);
	}
	if (PLR_FLAGGED(i, PLR_MUTE) && MUTE_DURATION(i) != 0 && MUTE_DURATION(i) <= time(NULL))
	{
		restore = PLR_TOG_CHK(i, PLR_MUTE);
		if (MUTE_REASON(i))
			free(MUTE_REASON(i));
		MUTE_REASON(i) = 0;
		GET_MUTE_LEV(i) = 0;
		MUTE_GODID(i) = 0;
		MUTE_DURATION(i) = 0;
		send_to_char("�� ������ �����.\r\n", i);
	}
	if (PLR_FLAGGED(i, PLR_DUMB) && DUMB_DURATION(i) != 0 && DUMB_DURATION(i) <= time(NULL))
	{
		restore = PLR_TOG_CHK(i, PLR_DUMB);
		if (DUMB_REASON(i))
			free(DUMB_REASON(i));
		DUMB_REASON(i) = 0;
		GET_DUMB_LEV(i) = 0;
		DUMB_GODID(i) = 0;
		DUMB_DURATION(i) = 0;
		send_to_char("�� ������ ��������.\r\n", i);
	}

	if (!PLR_FLAGGED(i, PLR_REGISTERED) && UNREG_DURATION(i) != 0 && UNREG_DURATION(i) <= time(NULL))
	{
		restore = PLR_TOG_CHK(i, PLR_REGISTERED);
		if (UNREG_REASON(i))
			free(UNREG_REASON(i));
		UNREG_REASON(i) = 0;
		GET_UNREG_LEV(i) = 0;
		UNREG_GODID(i) = 0;
		UNREG_DURATION(i) = 0;
		send_to_char("���� ����������� �������������.\r\n", i);

		if (IN_ROOM(i) == r_unreg_start_room)
		{
			if ((restore = GET_LOADROOM(i)) == NOWHERE)
				restore = calc_loadroom(i);

			restore = real_room(restore);

			if (restore == NOWHERE)
			{
				if (GET_LEVEL(i) >= LVL_IMMORT)
					restore = r_immort_start_room;
				else
					restore = r_mortal_start_room;
			}

			char_from_room(i);
			char_to_room(i, restore);
			look_at_room(i, restore);

			act("$n ������$u � ������ �������, � ��������� ��������� ���� ������� �����������!",
				FALSE, i, 0, 0, TO_ROOM);
		};

	}

	if (GET_GOD_FLAG(i, GF_GODSLIKE) && GCURSE_DURATION(i) != 0 && GCURSE_DURATION(i) <= time(NULL))
	{
		CLR_GOD_FLAG(i, GF_GODSLIKE);
		send_to_char("�� ����� �� ��� ������� �����.\r\n", i);
	}
	if (GET_GOD_FLAG(i, GF_GODSCURSE) && GCURSE_DURATION(i) != 0 && GCURSE_DURATION(i) <= time(NULL))
	{
		CLR_GOD_FLAG(i, GF_GODSCURSE);
		send_to_char("���� ����� �� � ����� �� ���.\r\n", i);
	}
	if (PLR_FLAGGED(i, PLR_FROZEN) && FREEZE_DURATION(i) != 0 && FREEZE_DURATION(i) <= time(NULL))
	{
		restore = PLR_TOG_CHK(i, PLR_FROZEN);
		if (FREEZE_REASON(i))
			free(FREEZE_REASON(i));
		FREEZE_REASON(i) = 0;
		GET_FREEZE_LEV(i) = 0;
		FREEZE_GODID(i) = 0;
		FREEZE_DURATION(i) = 0;
		send_to_char("�� �������.\r\n", i);
		Glory::remove_freeze(GET_UNIQUE(i));
	}
	// ��������� � ��� �� �� ��� ������ ���� �� ������.
	if (IN_ROOM(i) == STRANGE_ROOM)
		restore = i->get_was_in_room();
	else
		restore = IN_ROOM(i);

	if (PLR_FLAGGED(i, PLR_HELLED))
	{
		if (restore != r_helled_start_room)
		{
			if (IN_ROOM(i) == STRANGE_ROOM)
				i->set_was_in_room(r_helled_start_room);
			else
			{
				send_to_char("���-�� ���� ���� ������� ��� � �������.\r\n", i);
				act("$n ���������$a � �������.",
					FALSE, i, 0, 0, TO_ROOM);

				char_from_room(i);
				char_to_room(i, r_helled_start_room);
				look_at_room(i, r_helled_start_room);
				i->set_was_in_room(NOWHERE);
			};
		}
	}
	else if (PLR_FLAGGED(i, PLR_NAMED))
	{
		if (restore != r_named_start_room)
		{
			if (IN_ROOM(i) == STRANGE_ROOM)
				i->set_was_in_room(r_named_start_room);
			else
			{
				send_to_char("���-�� ���� ���� ������� ��� � ������� �����.\r\n", i);
				act("$n ���������$a � ������� �����.",
					FALSE, i, 0, 0, TO_ROOM);
				char_from_room(i);
				char_to_room(i, r_named_start_room);
				look_at_room(i, r_named_start_room);
				i->set_was_in_room(NOWHERE);
			};
		};
	}
	else if (0/*!RegisterSystem::is_registered(i) && i->desc && STATE(i->desc) == CON_PLAYING*/) // prool: multing enable!
	{
		if (restore != r_unreg_start_room
				&& !RENTABLE(i)
				&& !DeathTrap::is_slow_dt(IN_ROOM(i))
				&& !check_dupes_host(i->desc, 1))
		{
			if (IN_ROOM(i) == STRANGE_ROOM)
				i->set_was_in_room(r_unreg_start_room);
			else
			{
				act("$n ��������$a � ������� ��� �������������������� �������, �������� ����� ������.\r\n",
					FALSE, i, 0, 0, TO_ROOM);
				char_from_room(i);
				char_to_room(i, r_unreg_start_room);
				look_at_room(i, r_unreg_start_room);
				i->set_was_in_room(NOWHERE);
			};
		}
		else if (restore == r_unreg_start_room && check_dupes_host(i->desc, 1) && !IS_IMMORTAL(i))
		{
			send_to_char("��������� ���������� ��� �� ������� ��� �������������������� �������.\r\n", i);
			act("$n ������$u � ������ �������, ������ ��� �������� �����������...\r\n",
				FALSE, i, 0, 0, TO_ROOM);
			restore = i->get_was_in_room();
			if (restore == NOWHERE || restore == r_unreg_start_room)
			{
				restore = GET_LOADROOM(i);
				if (restore == NOWHERE)
					restore = calc_loadroom(i);
				restore = real_room(restore);
			}
			char_from_room(i);
			char_to_room(i, restore);
			look_at_room(i, restore);
			i->set_was_in_room(NOWHERE);
		}
	}
}

void beat_points_update(int pulse)
{
	CHAR_DATA *i, *next_char;
	int restore;

	if (!UPDATE_PC_ON_BEAT)
		return;

	// only for PC's
	for (i = character_list; i; i = next_char)
	{
		next_char = i->next;
		if (IS_NPC(i))
			continue;

		if (IN_ROOM(i) == NOWHERE)
		{
			log("SYSERR: Pulse character in NOWHERE.");
			continue;
		}

		if (RENTABLE(i) <= time(NULL))
		{
			RENTABLE(i) = 0;
			AGRESSOR(i) = 0;
			AGRO(i) = 0;
		}
		if (AGRO(i) < time(NULL))
			AGRO(i) = 0;

		beat_punish(i);

// This line is used only to control all situations when someone is
// dead (POS_DEAD). You can comment it to minimize heartbeat function
// working time, if you're sure, that you control these situations
// everywhere. To the time of this code revision I've fix some of them
// and haven't seen any other.
//             if (GET_POS(i) == POS_DEAD)
//                     die(i, NULL);

		if (GET_POS(i) < POS_STUNNED)
			continue;

		// Restore hitpoints
		restore = hit_gain(i);
		restore = interpolate(restore, pulse);

		if (AFF_FLAGGED(i, AFF_BANDAGE))
		{
			AFFECT_DATA* aff;
			for(aff = i->affected; aff; aff = aff->next)
			{
				if (aff->type == SPELL_BANDAGE)
				{
					restore += MIN(GET_REAL_MAX_HIT(i) / 10, aff->modifier);
					break;
				}
			}
		}

		if (GET_HIT(i) < GET_REAL_MAX_HIT(i))
			GET_HIT(i) = MIN(GET_HIT(i) + restore, GET_REAL_MAX_HIT(i));

		// �������� ������� !�����������!. �������� ������ �����,
		// �� ���� ��� ������ ����� ���������� ����� ���������� =)
		//Gorrah: ������� � handler::affect_total
		//check_berserk(i);

		// Restore PC caster mem
		if (!IS_MANA_CASTER(i) && !MEMQUEUE_EMPTY(i))
		{
			restore = mana_gain(i);
			restore = interpolate(restore, pulse);
			GET_MEM_COMPLETED(i) += restore;

	if (AFF_FLAGGED(i, AFF_RECALL_SPELLS))
		handle_recall_spells(i);

			while (GET_MEM_COMPLETED(i) > GET_MEM_CURRENT(i)
					&& !MEMQUEUE_EMPTY(i))
			{
				int spellnum;
				spellnum = MemQ_learn(i);
				GET_SPELL_MEM(i, spellnum)++;
				GET_CASTER(i) += spell_info[spellnum].danger;
			}

			if (MEMQUEUE_EMPTY(i))
			{
				if (GET_RELIGION(i) == RELIGION_MONO)
				{
					send_to_char
					("������� ���� ������� ��������. �� � ������� ���������� ���� ��������.\r\n",
					 i);
					act("������� �������, $n � ������� ���������$g ��������.",
						FALSE, i, 0, 0, TO_ROOM);
				}
				else
				{
					send_to_char
					("������� ���� ������� ��������. �� � ������� ������ ���� ����.\r\n", i);
					act("������� �������, $n � ������� �����$g ����.", FALSE, i, 0, 0, TO_ROOM);
				}
			}
		}

		if (!IS_MANA_CASTER(i) && MEMQUEUE_EMPTY(i))
		{
			GET_MEM_COMPLETED(i) = 0;
		}

		// ���� ���� � �������
		if (IS_MANA_CASTER(i) && GET_MANA_STORED(i) < GET_MAX_MANA(i))
		{
			GET_MANA_STORED(i) += mana_gain(i);
			if (GET_MANA_STORED(i) >= GET_MAX_MANA(i))
			{
				GET_MANA_STORED(i) = GET_MAX_MANA(i);
				send_to_char("���� ���������� ������� ��������� ��������������\r\n", i);
			}
		}
		if (IS_MANA_CASTER(i) && GET_MANA_STORED(i) > GET_MAX_MANA(i))
		{
			GET_MANA_STORED(i) = GET_MAX_MANA(i);
		}
		// Restore moves
		restore = move_gain(i);
		restore = interpolate(restore, pulse);
//		GET_MOVE(i) = MIN(GET_MOVE(i) + restore, GET_REAL_MAX_MOVE(i));
//MZ.overflow_fix
		if (GET_MOVE(i) < GET_REAL_MAX_MOVE(i))
			GET_MOVE(i) = MIN(GET_MOVE(i) + restore, GET_REAL_MAX_MOVE(i));
//-MZ.overflow_fix
	}
}

void update_clan_exp(CHAR_DATA *ch, int gain)
{
	if (CLAN(ch) && gain != 0)
	{
		// ����� ��� ������ ����� (+ ������ �� �����, - �����, �� /5)
		if (gain < 0 || GET_GOD_FLAG(ch, GF_REMORT))
		{
			int tmp = gain > 0 ? gain : gain / 5;
			CLAN(ch)->SetClanExp(ch, tmp);
		}
		// ����� ��� ���� ������ �� ����� (����������� ��� + � -)
		CLAN(ch)->last_exp.add_temp(gain);
		// ����� ��� ���� ������ �� ��� ����� (����������� ��� + � -)
		CLAN(ch)->AddTopExp(ch, gain);
		// ����� ��� ����-������� ������ (����������� ������ +)
		if (gain > 0)
		{
			CLAN(ch)->exp_history.add_exp(gain);
		}
	}
}

void gain_exp(CHAR_DATA * ch, int gain)
{
	int is_altered = FALSE;
	int num_levels = 0;
	char buf[128];

	if (IS_NPC(ch))
	{
		ch->set_exp(ch->get_exp() + gain);
		return;
	}
	else
	{
		ch->dps_add_exp(gain);
		ZoneExpStat::add(zone_table[world[IN_ROOM(ch)]->zone].number, gain);
	}

	if (!IS_NPC(ch) && ((GET_LEVEL(ch) < 1 || GET_LEVEL(ch) >= LVL_IMMORT)))
		return;

	if (gain > 0 && GET_LEVEL(ch) < LVL_IMMORT)
	{
		gain = MIN(max_exp_gain_pc(ch), gain);	// put a cap on the max gain per kill
		ch->set_exp(ch->get_exp() + gain);
		if (GET_EXP(ch) >= level_exp(ch, LVL_IMMORT))
		{
			if (!GET_GOD_FLAG(ch, GF_REMORT) && GET_REMORT(ch) < MAX_REMORT)
			{
				if (Remort::can_remort_now(ch))
				{
					send_to_char(ch, "%s�����������, �� �������� ����� �� ��������������!%s\r\n",
						CCIGRN(ch, C_NRM), CCNRM(ch, C_NRM));
				}
				else
				{
					send_to_char(ch,
						"%s�����������, �� ������� ������������ ���������� �����!\r\n"
						"%s%s\r\n", CCIGRN(ch, C_NRM), Remort::WHERE_TO_REMORT_STR.c_str(), CCNRM(ch, C_NRM));
				}
				SET_GOD_FLAG(ch, GF_REMORT);
			}
		}
		ch->set_exp(MIN(GET_EXP(ch), level_exp(ch, LVL_IMMORT) - 1));
		while (GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= level_exp(ch, GET_LEVEL(ch) + 1))
		{
			ch->set_level(ch->get_level() + 1);
			num_levels++;
			sprintf(buf, "%s�� �������� ���������� ������!%s\r\n", CCWHT(ch, C_NRM), CCNRM(ch, C_NRM));
			send_to_char(buf, ch);
			advance_level(ch);
			is_altered = TRUE;
		}

		if (is_altered)
		{
			sprintf(buf, "%s advanced %d level%s to level %d.",
					GET_NAME(ch), num_levels, num_levels == 1 ? "" : "s", GET_LEVEL(ch));
			mudlog(buf, BRF, LVL_IMPL, SYSLOG, TRUE);
		}
	}
	else if (gain < 0 && GET_LEVEL(ch) < LVL_IMMORT)
	{
		gain = MAX(-max_exp_loss_pc(ch), gain);	// Cap max exp lost per death
		ch->set_exp(ch->get_exp() + gain);
		while (GET_LEVEL(ch) > 1 && GET_EXP(ch) < level_exp(ch, GET_LEVEL(ch)))
		{
			ch->set_level(ch->get_level() - 1);
			num_levels++;
			sprintf(buf,
					"%s�� �������� �������!%s\r\n",
					CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
			send_to_char(buf, ch);
			decrease_level(ch);
			is_altered = TRUE;
		}
		if (is_altered)
		{
			sprintf(buf, "%s decreases %d level%s to level %d.",
					GET_NAME(ch), num_levels, num_levels == 1 ? "" : "s", GET_LEVEL(ch));
			mudlog(buf, BRF, LVL_IMPL, SYSLOG, TRUE);
		}
	}
	if ((GET_EXP(ch) < level_exp(ch, LVL_IMMORT) - 1)
		&& GET_GOD_FLAG(ch, GF_REMORT)
		&& gain
		&& (GET_LEVEL(ch) < LVL_IMMORT))
	{
		if (Remort::can_remort_now(ch))
		{
			send_to_char(ch, "%s�� �������� ����� �� ��������������!%s\r\n",
				CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
		}
		CLR_GOD_FLAG(ch, GF_REMORT);
	}

	char_stat::add_class_exp(GET_CLASS(ch), gain);
	update_clan_exp(ch, gain);
}

// ������� ������������� � act.wizards.cpp � ��� �������� "advance" � "set exp".
void gain_exp_regardless(CHAR_DATA * ch, int gain)
{
	int is_altered = FALSE;
	int num_levels = 0;

	ch->set_exp(ch->get_exp() + gain);
	if (!IS_NPC(ch))
	{
		if (gain > 0)
		{
			while (GET_LEVEL(ch) < LVL_IMPL && GET_EXP(ch) >= level_exp(ch, GET_LEVEL(ch) + 1))
			{
				ch->set_level(ch->get_level() + 1);
				num_levels++;
				sprintf(buf, "%s�� �������� ���������� ������!%s\r\n",
						CCWHT(ch, C_NRM), CCNRM(ch, C_NRM));
				send_to_char(buf, ch);

				advance_level(ch);
				is_altered = TRUE;
			}

			if (is_altered)
			{
				sprintf(buf, "%s advanced %d level%s to level %d.",
						GET_NAME(ch), num_levels, num_levels == 1 ? "" : "s", GET_LEVEL(ch));
				mudlog(buf, BRF, LVL_IMPL, SYSLOG, TRUE);
			}
		}
		else if (gain < 0)
		{
// Pereplut: ������ ������� ����.
//			gain = MAX(-max_exp_loss_pc(ch), gain);	// Cap max exp lost per death
//			GET_EXP(ch) += gain;
//			if (GET_EXP(ch) < 0)
//				GET_EXP(ch) = 0;
			while (GET_LEVEL(ch) > 1 && GET_EXP(ch) < level_exp(ch, GET_LEVEL(ch)))
			{
				ch->set_level(ch->get_level() - 1);
				num_levels++;
				sprintf(buf,
						"%s�� �������� �������!%s\r\n",
						CCIRED(ch, C_NRM), CCNRM(ch, C_NRM));
				send_to_char(buf, ch);
				decrease_level(ch);
				is_altered = TRUE;
			}
			if (is_altered)
			{
				sprintf(buf, "%s decreases %d level%s to level %d.",
						GET_NAME(ch), num_levels, num_levels == 1 ? "" : "s", GET_LEVEL(ch));
				mudlog(buf, BRF, LVL_IMPL, SYSLOG, TRUE);
			}
		}

	}
}

void gain_condition(CHAR_DATA * ch, unsigned condition, int value)
{
	if (condition >= ch->player_specials->saved.conditions.size())
	{
		log("SYSERROR : condition=%d (%s:%d)", condition, __FILE__, __LINE__);
		return;
	}
	if (IS_NPC(ch) || GET_COND(ch, condition) == -1)
	{
		return;
	}

	bool intoxicated = (GET_COND(ch, DRUNK) >= CHAR_DRUNKED);

	GET_COND(ch, condition) += value;
	GET_COND(ch, condition) = MAX(0, GET_COND(ch, condition));
	GET_COND(ch, condition) = MIN(MAX_COND_VALUE, GET_COND(ch, condition));

	if (GET_COND(ch, condition) || PLR_FLAGGED(ch, PLR_WRITING))
		return;

	switch (condition)
	{
	case FULL:
		send_to_char("�� �������.\r\n", ch);
		return;
	case THIRST:
		send_to_char("��� ������ �����.\r\n", ch);
		return;
	case DRUNK:
		if (intoxicated && GET_COND(ch, DRUNK) < CHAR_DRUNKED)
			send_to_char("�������-�� �� �����������.\r\n", ch);
		GET_DRUNK_STATE(ch) = 0;
		return;
	default:
		break;
	}

}

void underwater_check(void)
{
	DESCRIPTOR_DATA *d;
	for (d = descriptor_list; d; d = d->next)
	{
		if (d->character
			&& SECT(d->character->in_room) == SECT_UNDERWATER
			&& !IS_GOD(d->character)
			&& !AFF_FLAGGED(d->character, AFF_WATERBREATH))
		{
			sprintf(buf, "Player %s died under water (room %d)",
				GET_NAME(d->character), GET_ROOM_VNUM(d->character->in_room));

			Damage dmg(SimpleDmg(TYPE_WATERDEATH), MAX(1, GET_REAL_MAX_HIT(d->character) >> 2), FightSystem::UNDEF_DMG);
			dmg.flags.set(FightSystem::NO_FLEE);

			if (dmg.process(d->character, d->character) < 0)
			{
				log(buf);
			}
		}
	}

}

void check_idling(CHAR_DATA * ch)
{
	if (!RENTABLE(ch))
	{
		if (++(ch->char_specials.timer) > idle_void)
		{
			ch->set_motion(false);
			if (ch->get_was_in_room() == NOWHERE && ch->in_room != NOWHERE)
			{
				ch->set_was_in_room(ch->in_room);
				if (ch->get_fighting())
				{
					stop_fighting(ch->get_fighting(), FALSE);
					stop_fighting(ch, TRUE);
				}
				act("$n ���������$u � �������.", TRUE, ch, 0, 0, TO_ROOM);
				send_to_char("�� ������� � ������� ����� ����.\r\n", ch);
				ch->save_char();
				Crash_crashsave(ch);
				char_from_room(ch);
				char_to_room(ch, STRANGE_ROOM);
				remove_rune_label(ch);
			}
			else if (ch->char_specials.timer > idle_rent_time)
			{
				if (ch->in_room != NOWHERE)
					char_from_room(ch);
				char_to_room(ch, STRANGE_ROOM);
				if (ch->desc)
				{
					STATE(ch->desc) = CON_DISCONNECT;
					/*
					 * For the 'if (d->character)' test in close_socket().
					 * -gg 3/1/98 (Happy anniversary.)
					 */
					ch->desc->character = NULL;
					ch->desc = NULL;
				}
				if (free_rent)
					Crash_rentsave(ch, 0);
				else
					Crash_idlesave(ch);
				Depot::exit_char(ch);
				Clan::clan_invoice(ch, false);
				sprintf(buf, "%s force-rented and extracted (idle).", GET_NAME(ch));
				perslog(" ", buf); // prool
				mudlog(buf, NRM, LVL_GOD, SYSLOG, TRUE);
				extract_char(ch, FALSE);
			}
		}
	}
}



// Update PCs, NPCs, and objects
#define NO_DESTROY(obj) ((obj)->carried_by   || \
                         (obj)->worn_by      || \
                         (obj)->in_obj       || \
                         (obj)->script       || \
                         GET_OBJ_TYPE(obj) == ITEM_FOUNTAIN || \
	                 obj->in_room == NOWHERE || \
			 (OBJ_FLAGGED(obj, ITEM_NODECAY) && !ROOM_FLAGGED(obj->in_room, ROOM_DEATH)))
#define NO_TIMER(obj)   (GET_OBJ_TYPE(obj) == ITEM_FOUNTAIN)
// || OBJ_FLAGGED(obj, ITEM_NODECAY))

int up_obj_where(OBJ_DATA * obj)
{
	if (obj->in_obj)
		return up_obj_where(obj->in_obj);
	else
		return OBJ_WHERE(obj);
}


void hour_update(void)
{
	DESCRIPTOR_DATA *i;

	for (i = descriptor_list; i; i = i->next)
	{
		if (STATE(i) != CON_PLAYING || i->character == NULL || PLR_FLAGGED(i->character, PLR_WRITING))
			continue;
		if (mudname[0]) // prool
			sprintf(buf, "%s������ ��� ( %s ) %s\r\n",
				CCIRED(i->character, C_NRM), mudname, CCNRM(i->character, C_NRM));
		else
			sprintf(buf, "%s������ ��� (VMUD)%s\r\n",
				CCIRED(i->character, C_NRM), CCNRM(i->character, C_NRM));
		SEND_TO_Q(buf, i);
	}
}

void room_point_update()
{
	int mana;
	for (int count = FIRST_ROOM; count <= top_of_world; count++)
	{
		if (world[count]->fires)
		{
			switch (get_room_sky(count))
			{
			case SKY_CLOUDY:
			case SKY_CLOUDLESS:
				mana = number(1, 2);
				break;
			case SKY_RAINING:
				mana = 2;
				break;
			default:
				mana = 1;
			}
			world[count]->fires -= MIN(mana, world[count]->fires);
			if (world[count]->fires <= 0)
			{
				act("������ �����.", FALSE, world[count]->people, 0, 0, TO_ROOM);
				act("������ �����.", FALSE, world[count]->people, 0, 0, TO_CHAR);
				world[count]->fires = 0;
			}
		}

		if (world[count]->portal_time)
		{
			world[count]->portal_time--;
			if (!world[count]->portal_time)
			{
				OneWayPortal::remove(world[count]);
				world[count]->pkPenterUnique = 0;
				act("����������� �������� ��������.", FALSE, world[count]->people, 0, 0, TO_ROOM);
				act("����������� �������� ��������.", FALSE, world[count]->people, 0, 0, TO_CHAR);
			}
		}
		if (world[count]->holes)
		{
			world[count]->holes--;
			if (!world[count]->holes || roundup(world[count]->holes) == world[count]->holes)
			{
				act("���� ��������� ������...", FALSE, world[count]->people, 0, 0, TO_ROOM);
				act("���� ��������� ������...", FALSE, world[count]->people, 0, 0, TO_CHAR);
			}
		}
		if (world[count]->ices)
			if (!--world[count]->ices)
			{
				REMOVE_BIT(ROOM_FLAGS(count, ROOM_ICEDEATH), ROOM_ICEDEATH);
				DeathTrap::remove(world[count]);
			}

		world[count]->glight = MAX(0, world[count]->glight);
		world[count]->gdark = MAX(0, world[count]->gdark);

		struct track_data *track, *next_track, *temp;
		int spellnum;
		for (track = world[count]->track, temp = NULL; track; track = next_track)
		{
			next_track = track->next;
			switch (real_sector(count))
			{
			case SECT_FLYING:
			case SECT_UNDERWATER:
			case SECT_SECRET:
			case SECT_WATER_SWIM:
			case SECT_WATER_NOSWIM:
				spellnum = 31;
				break;
			case SECT_THICK_ICE:
			case SECT_NORMAL_ICE:
			case SECT_THIN_ICE:
				spellnum = 16;
				break;
			case SECT_CITY:
				spellnum = 4;
				break;
			case SECT_FIELD:
			case SECT_FIELD_RAIN:
				spellnum = 2;
				break;
			case SECT_FIELD_SNOW:
				spellnum = 1;
				break;
			case SECT_FOREST:
			case SECT_FOREST_RAIN:
				spellnum = 2;
				break;
			case SECT_FOREST_SNOW:
				spellnum = 1;
				break;
			case SECT_HILLS:
			case SECT_HILLS_RAIN:
				spellnum = 3;
				break;
			case SECT_HILLS_SNOW:
				spellnum = 1;
				break;
			case SECT_MOUNTAIN:
				spellnum = 4;
				break;
			case SECT_MOUNTAIN_SNOW:
				spellnum = 1;
				break;
			default:
				spellnum = 2;
			}

			int restore;
			for (mana = 0, restore = FALSE; mana < NUM_OF_DIRS; mana++)
			{
				if ((track->time_income[mana] <<= spellnum))
					restore = TRUE;
				if ((track->time_outgone[mana] <<= spellnum))
					restore = TRUE;
			}
			if (!restore)
			{
				if (temp)
					temp->next = next_track;
				else
					world[count]->track = next_track;
				free(track);
			}
			else
				temp = track;
		}

		check_death_ice(count, NULL);
	}
}

void exchange_point_update()
{
	EXCHANGE_ITEM_DATA *exch_item, *next_exch_item;
	for (exch_item = exchange_item_list; exch_item; exch_item = next_exch_item)
	{
		next_exch_item = exch_item->next;
		if (GET_EXCHANGE_ITEM(exch_item)->get_timer() > 0)
		{
			GET_EXCHANGE_ITEM(exch_item)->dec_timer();
		}

		if (GET_EXCHANGE_ITEM(exch_item)->get_timer() <= 0)
		{
			sprintf(buf, "Exchange: - %s ��������%s �� ����������� �������������.\r\n",
					CAP(GET_EXCHANGE_ITEM(exch_item)->PNames[0]),
					GET_OBJ_SUF_2(GET_EXCHANGE_ITEM(exch_item)));
			log(buf);
			extract_exchange_item(exch_item);
		}
	}

}

// * ���������� � ����� ������ �� ����� � ����-�����.
void clan_chest_invoice(OBJ_DATA *j)
{
	const int room = GET_ROOM_VNUM(j->in_obj->in_room);

	if (room <= 0)
	{
		snprintf(buf, sizeof(buf), "clan_chest_invoice: room=%d, obj_vnum=%d",
			room, GET_OBJ_VNUM(j));
		mudlog(buf, CMP, LVL_IMMORT, SYSLOG, TRUE);
		return;
	}

	for (DESCRIPTOR_DATA *d = descriptor_list; d; d = d->next)
	{
		if (d->character
			&& STATE(d) == CON_PLAYING
			&& !AFF_FLAGGED(d->character, AFF_DEAFNESS)
			&& PRF_FLAGGED(d->character, PRF_DECAY_MODE)
			&& CLAN(d->character)
			&& CLAN(d->character)->GetRent() == room)
		{
			send_to_char(d->character, "[���������]: %s'%s%s ��������%s � ����'%s\r\n",
				CCIRED(d->character, C_NRM),
				j->short_description,
				clan_get_custom_label(j, CLAN(d->character)).c_str(),
				GET_OBJ_SUF_2(j),
				CCNRM(d->character, C_NRM));
		}
	}

	for (ClanListType::iterator i = Clan::ClanList.begin(),
		iend = Clan::ClanList.end(); i != iend; ++i)
	{
		if ((*i)->GetRent() == room)
		{
			std::string log_text = boost::str(boost::format("%s%s ��������%s � ����\r\n")
				% j->short_description
				% clan_get_custom_label(j, *i)
				% GET_OBJ_SUF_2(j));
			(*i)->chest_log.add(log_text);
			return;
		}
	}
}

// * ����� ������ � ����-�����.
void clan_chest_point_update(OBJ_DATA *j)
{
	if (j->get_timer() > 0)
	{
		j->dec_timer();
	}

	if ((OBJ_FLAGGED(j, ITEM_ZONEDECAY)
			&& GET_OBJ_ZONE(j) != NOWHERE
			&& up_obj_where(j->in_obj) != NOWHERE
			&& GET_OBJ_ZONE(j) != world[up_obj_where(j->in_obj)]->zone)
		|| j->get_timer() <= 0)
	{
		clan_chest_invoice(j);
		obj_from_obj(j);
		extract_obj(j);
	}
}

// ��������� ����� ������� ��� ����� ����� �� �������
/* �������� where ���������� ��������� ����:
   0: ���� ��� ���������
   1: �����
   2: ���������
*/
void charmee_obj_decay_tell(CHAR_DATA *charmee, OBJ_DATA *obj, int where)
{
	char buf[MAX_STRING_LENGTH];
	char buf1[128]; // �� �� ������ �� ���� malloc

	if (!charmee->master)
		return;

	if (where == 0)
		sprintf(buf1, "� ����� �����");
	else if (where == 1)
		sprintf(buf1, "����� �� ���");
	else if (where == 2 && obj->in_obj)
		snprintf(buf1, 128, "� %s", obj->in_obj->PNames[5]);
	else
		sprintf(buf1, "��������� ���"); // ��� ������ -- ���� ���������� �������� �� ������

	/*
	   ���������� ����� �� ����� ��������, �� ������ ����� ����������� ��������� �� �������:
	   ����� do_tell ������� ������ �� �����, � ���� ��� ����� ��������� �� ��� ����,
	   �� � ���� ����� ��������� �������������.
	   ������, ����������� ��������������, ���� ���-������ ��������� �����.
	*/
	snprintf(buf, MAX_STRING_LENGTH, "%s ������%s ��� : '%s ��������%s %s...'",
	         GET_NAME(charmee),
	         GET_CH_SUF_1(charmee),
	         CAP(OBJ_PAD(obj, 0)),
	         GET_OBJ_SUF_2(obj),
	         buf1);
	send_to_char(charmee->master, "%s%s%s\r\n", CCICYN(charmee->master, C_NRM), CAP(buf), CCNRM(charmee->master, C_NRM));
}

void obj_point_update()
{
	OBJ_DATA *j, *next_thing, *jj, *next_thing2;
	int count, cont = 0;

	for (j = object_list; j; j = next_thing)
	{
		next_thing = j->next;	// Next in object list

		// ������� ����-�������
		if (j->in_obj && Clan::is_clan_chest(j->in_obj))
		{
			clan_chest_point_update(j);
			continue;
		}

		// ���������� �� ����� � ������ !�����, �� �� ����������� � ���� �������, � �� ��� ���������
		// ���������� ������� ���������� �� ������ ������ ������� ������ ����, �� � ������ ���� �����������
		// �� �����, �� � �� ��� �� � ����� �� ����� ���� ����� ��������� �����, ������� ���������
		// � ��� ������ ��������� ��������� ����� � �������� ������ ��� ������� � ����� �����
		if (j->in_obj
				&& !j->in_obj->carried_by
				&& !j->in_obj->worn_by
				&& OBJ_FLAGGED(j->in_obj, ITEM_NODECAY)
				&& GET_ROOM_VNUM(IN_ROOM(j->in_obj)) % 100 != 99)
		{
			int zone = world[j->in_obj->in_room]->zone;
			bool find = 0;
			ClanListType::const_iterator clan = Clan::IsClanRoom(j->in_obj->in_room);
			if (clan == Clan::ClanList.end())   // ������ ������ ���� � �������� �� �����
			{
				for (int cmd_no = 0; zone_table[zone].cmd[cmd_no].command != 'S'; ++cmd_no)
				{
					if (zone_table[zone].cmd[cmd_no].command == 'O'
							&& zone_table[zone].cmd[cmd_no].arg1 == GET_OBJ_RNUM(j->in_obj)
							&& zone_table[zone].cmd[cmd_no].arg3 == IN_ROOM(j->in_obj))
					{
						find = 1;
						break;
					}
				}
			}

			if (!find && j->get_timer() > 0)
			{
				j->dec_timer();
			}
		}

		// If this is a corpse
		if (IS_CORPSE(j))  	// timer count down
		{
			if (j->get_timer() > 0)
			{
				j->dec_timer();
			}
			if (j->get_timer() <= 0)
			{
				for (jj = j->contains; jj; jj = next_thing2)
				{
					next_thing2 = jj->next_content;	// Next in inventory
					obj_from_obj(jj);
					if (j->in_obj)
						obj_to_obj(jj, j->in_obj);
					else if (j->carried_by)
						obj_to_char(jj, j->carried_by);
					else if (j->in_room != NOWHERE)
						obj_to_room(jj, j->in_room);
					else
					{
						log("SYSERR: extract %s from %s to NOTHING !!!",
							jj->PNames[0], j->PNames[0]);
						// core_dump();
						extract_obj(jj);
					}
				}
				// ��������� ��������
//              next_thing = j->next; // ���� �� obj_to_room � �����, �� ������ �� ������ ������
				// ����� ������
				if (j->carried_by)
				{
					act("$p ��������$U � ����� �����.", FALSE, j->carried_by, j, 0, TO_CHAR);
					obj_from_char(j);
				}
				else if (j->in_room != NOWHERE)
				{
					if (world[j->in_room]->people)
					{
						act("����� ��������� ������� $o3.",
							TRUE, world[j->in_room]->people, j, 0, TO_ROOM);
						act("����� �� �������� �� $o1 � �����.",
							TRUE, world[j->in_room]->people, j, 0, TO_CHAR);
					}
					obj_from_room(j);
				}
				else if (j->in_obj)
					obj_from_obj(j);
				extract_obj(j);
			}
		}
		// If the timer is set, count it down and at 0, try the trigger
		// note to .rej hand-patchers: make this last in your point-update()
		else
		{
			if (SCRIPT_CHECK(j, OTRIG_TIMER))
			{
				if (j->get_timer() > 0 && OBJ_FLAGGED(j, ITEM_TICKTIMER))
				{
					j->dec_timer();
				}
				if (!j->get_timer())
				{
					timer_otrigger(j);
					j = NULL;
				}
			}
			else if (GET_OBJ_DESTROY(j) > 0 && !NO_DESTROY(j))
				GET_OBJ_DESTROY(j)--;

			if (j && (j->in_room != NOWHERE) && j->get_timer() > 0 && !NO_DESTROY(j))
			{
				j->dec_timer();
			}

			if (j && (
					(OBJ_FLAGGED(j, ITEM_ZONEDECAY)
					&& GET_OBJ_ZONE(j) != NOWHERE
					&& up_obj_where(j) != NOWHERE
					&& GET_OBJ_ZONE(j) != world[up_obj_where(j)]->zone)
					|| (j->get_timer() <= 0 && !NO_TIMER(j))
					|| (GET_OBJ_DESTROY(j) == 0 && !NO_DESTROY(j))))
			{
				// *** ���������� �������
				for (jj = j->contains; jj; jj = next_thing2)
				{
					next_thing2 = jj->next_content;
					obj_from_obj(jj);
					if (j->in_obj)
						obj_to_obj(jj, j->in_obj);
					else if (j->worn_by)
						obj_to_char(jj, j->worn_by);
					else if (j->carried_by)
						obj_to_char(jj, j->carried_by);
					else if (j->in_room != NOWHERE)
						obj_to_room(jj, j->in_room);
					else
					{
						log("SYSERR: extract %s from %s to NOTHING !!!",
							jj->PNames[0], j->PNames[0]);
						// core_dump();
						extract_obj(jj);
					}
				}
				// ��������� ��������
//              next_thing = j->next; // ���� �� obj_to_room � �����, �� ������ �� ������ ������
				// ����� ������
				if (j->worn_by)
				{
					switch (j->worn_on)
					{
					case WEAR_LIGHT:
					case WEAR_SHIELD:
					case WEAR_WIELD:
					case WEAR_HOLD:
					case WEAR_BOTHS:
						if (IS_CHARMICE(j->worn_by))
							charmee_obj_decay_tell(j->worn_by, j, 0);
						else
							act("$o ��������$U � ����� �����...", FALSE, j->worn_by, j, 0, TO_CHAR);
						break;
					default:
						if (IS_CHARMICE(j->worn_by))
							charmee_obj_decay_tell(j->worn_by, j, 1);
						else
							act("$o ��������$U ����� �� ���...", FALSE, j->worn_by, j, 0, TO_CHAR);
						break;
					}
					unequip_char(j->worn_by, j->worn_on);
				}
				else if (j->carried_by)
				{
					if (IS_CHARMICE(j->carried_by))
						charmee_obj_decay_tell(j->carried_by, j, 0);
					else
						act("$o ��������$U � ����� �����...", FALSE, j->carried_by, j, 0, TO_CHAR);
					obj_from_char(j);
				}
				else if (j->in_room != NOWHERE)
				{
					if (world[j->in_room]->people)
					{
						act("$o ��������$U � ����, ������� ��� ������� ������...",
							FALSE, world[j->in_room]->people, j, 0, TO_CHAR);
						act("$o ��������$U � ����, ������� ��� ������� ������...",
							FALSE, world[j->in_room]->people, j, 0, TO_ROOM);
					}
					obj_from_room(j);
				}
				else if (j->in_obj) {
					// ���� ������� � ����������� � ���� ��� ������� ����������, �� �� ���� ���� ��������
					CHAR_DATA *cont_owner = NULL;
					if (j->in_obj->carried_by)
						cont_owner = j->in_obj->carried_by;
					else if (j->in_obj->worn_by)
						cont_owner = j->in_obj->worn_by;

					if (cont_owner)
					{
						if (IS_CHARMICE(cont_owner))
							charmee_obj_decay_tell(cont_owner, j, 2);
						else
						{
							char buf[MAX_STRING_LENGTH];
							snprintf(buf, MAX_STRING_LENGTH, "$o ��������$U � %s...", j->in_obj->PNames[5]);
							act(buf, FALSE, cont_owner, j, 0, TO_CHAR);
						}
					}
					obj_from_obj(j);
				}
				extract_obj(j);
			}
			else
			{
				if (!j)
					continue;

				// decay poision && other affects
				for (count = 0; count < MAX_OBJ_AFFECT; count++)
				{
					if (j->affected[count].location == APPLY_POISON)
					{
						j->affected[count].modifier--;
						if (j->affected[count].modifier <= 0)
						{
							j->affected[count].location = APPLY_NONE;
							j->affected[count].modifier = 0;
						}
					}
				}
			}
		}
	}

	// �������, ��������, � ��������� �������.
	for (j = object_list; j; j = next_thing)
	{
		next_thing = j->next;	// Next in object list
		if (j->contains)
		{
			cont = TRUE;
		}
		else
		{
			cont = FALSE;
		}
		if (obj_decay(j))
		{
			if (cont)
			{
				next_thing = object_list;
			}
		}
	}
}

void point_update(void)
{
	memory_rec *mem, *nmem, *pmem;
	CHAR_DATA *i, *next_char;
	int count, mob_num, spellnum, mana;
	boost::array<int, MAX_SPELLS + 1> buffer_mem;
	boost::array<int, MAX_SPELLS + 1> real_spell;

	for (count = 0; count <= MAX_SPELLS; count++)
	{
		buffer_mem[count] = count;
		real_spell[count] = 0;
	}
	for (spellnum = MAX_SPELLS; spellnum > 0; spellnum--)
	{
		count = number(1, spellnum);
		real_spell[MAX_SPELLS - spellnum] = buffer_mem[count];
		for (; count < MAX_SPELLS; buffer_mem[count] = buffer_mem[count + 1], count++);
	}
	// characters
	for (i = character_list; i; i = next_char)
	{
		if (IS_NPC(i))
		{
			i->inc_restore_timer(SECS_PER_MUD_HOUR);
		}
		next_char = i->next;
		/* ���� ��� ��� ��� ��������� ���������� � �� ��� ������ ���,
		   �� �� ����� ������ �������� � ��� */
		if (AFF_FLAGGED(i, AFF_SLEEP) && GET_POS(i) > POS_SLEEPING)
		{
			GET_POS(i) = POS_SLEEPING;
			send_to_char("�� ���������� ��������, �� ����� ������� � ����� ������.\r\n", i);
			act("$n �������$u ��������, �� ����� ������$a � ����$a ������.", TRUE, i, 0, 0, TO_ROOM);
		}
		if (!IS_NPC(i))
		{
			if (average_day_temp() < -20)
				gain_condition(i, FULL, -2);
			else if (average_day_temp() < -5)
				gain_condition(i, FULL, number(-2, -1));
			else
				gain_condition(i, FULL, -1);

			gain_condition(i, DRUNK, -1);

			if (average_day_temp() > 25)
				gain_condition(i, THIRST, -2);
			else if (average_day_temp() > 20)
				gain_condition(i, THIRST, number(-2, -1));
			else
				gain_condition(i, THIRST, -1);

		}
		if (GET_POS(i) >= POS_STUNNED)  	// Restore hitpoints
		{
			if (IS_NPC(i) || !UPDATE_PC_ON_BEAT)
			{
				count = hit_gain(i);
				if (GET_HIT(i) < GET_REAL_MAX_HIT(i))
					GET_HIT(i) = MIN(GET_HIT(i) + count, GET_REAL_MAX_HIT(i));
			}
			// Restore mobs
			if (IS_NPC(i) && !i->get_fighting())  	// Restore horse
			{
				if (IS_HORSE(i))
				{
					switch (real_sector(IN_ROOM(i)))
					{
					case SECT_FLYING:
					case SECT_UNDERWATER:
					case SECT_SECRET:
					case SECT_WATER_SWIM:
					case SECT_WATER_NOSWIM:
					case SECT_THICK_ICE:
					case SECT_NORMAL_ICE:
					case SECT_THIN_ICE:
						mana = 0;
						break;
					case SECT_CITY:
						mana = 20;
						break;
					case SECT_FIELD:
					case SECT_FIELD_RAIN:
						mana = 100;
						break;
					case SECT_FIELD_SNOW:
						mana = 40;
						break;
					case SECT_FOREST:
					case SECT_FOREST_RAIN:
						mana = 80;
						break;
					case SECT_FOREST_SNOW:
						mana = 30;
						break;
					case SECT_HILLS:
					case SECT_HILLS_RAIN:
						mana = 70;
						break;
					case SECT_HILLS_SNOW:
						mana = 25;
						break;
					case SECT_MOUNTAIN:
						mana = 25;
						break;
					case SECT_MOUNTAIN_SNOW:
						mana = 10;
						break;
					default:
						mana = 10;
					}
					if (on_horse(i->master))
						mana /= 2;
					GET_HORSESTATE(i) = MIN(200, GET_HORSESTATE(i) + mana);
				}
				// Forget PC's
				for (mem = MEMORY(i), pmem = NULL; mem; mem = nmem)
				{
					nmem = mem->next;
					if (mem->time <= 0 && i->get_fighting())
					{
						pmem = mem;
						continue;
					}
					if (mem->time < time(NULL) || mem->time <= 0)
					{
						if (pmem)
							pmem->next = nmem;
						else
							MEMORY(i) = nmem;
						free(mem);
					}
					else
						pmem = mem;
				}
				// Remember some spells
				if ((mob_num = GET_MOB_RNUM(i)) >= 0)
					for (count = 0, mana = 0;
							count < MAX_SPELLS && mana < GET_REAL_INT(i) * 10; count++)
					{
						spellnum = real_spell[count];
						if (GET_SPELL_MEM(mob_proto + mob_num, spellnum) >
								GET_SPELL_MEM(i, spellnum))
						{
							GET_SPELL_MEM(i, spellnum)++;
							mana +=
								((spell_info[spellnum].mana_max +
								  spell_info[spellnum].mana_min) / 2);
							GET_CASTER(i) +=
								(IS_SET
								 (spell_info[spellnum].routines, NPC_CALCULATE) ? 1 : 0);
							// sprintf(buf,"Remember spell %s for mob %s.\r\n",spell_info[spellnum].name,GET_NAME(i));
							// send_to_gods(buf);
						}
					}
			}
			// Restore moves
			if (IS_NPC(i) || !UPDATE_PC_ON_BEAT)
//				GET_MOVE(i) = MIN(GET_MOVE(i) + move_gain(i), GET_REAL_MAX_MOVE(i));
//MZ.overflow_fix
				if (GET_MOVE(i) < GET_REAL_MAX_MOVE(i))
					GET_MOVE(i) = MIN(GET_MOVE(i) + move_gain(i), GET_REAL_MAX_MOVE(i));
//-MZ.overflow_fix

			// Update PC/NPC position
			if (GET_POS(i) <= POS_STUNNED)
				update_pos(i);
		}
		else if (GET_POS(i) == POS_INCAP)
		{
			Damage dmg(SimpleDmg(TYPE_SUFFERING), 1, FightSystem::UNDEF_DMG);
			dmg.flags.set(FightSystem::NO_FLEE);

			if (dmg.process(i, i) == -1)
				continue;
		}
		else if (GET_POS(i) == POS_MORTALLYW)
		{
			Damage dmg(SimpleDmg(TYPE_SUFFERING), 2, FightSystem::UNDEF_DMG);
			dmg.flags.set(FightSystem::NO_FLEE);

			if (dmg.process(i, i) == -1)
				continue;
		}
		update_char_objects(i);
		if (!IS_NPC(i) && GET_LEVEL(i) < idle_max_level && !PRF_FLAGGED(i, PRF_CODERINFO))
			check_idling(i);
	}
}

void repop_decay(zone_rnum zone)
{				// ���������� �������� ITEM_REPOP_DECAY
	OBJ_DATA *j, *next_thing;
	int cont = FALSE;
	zone_vnum obj_zone_num, zone_num;

	zone_num = zone_table[zone].number;
	for (j = object_list; j; j = next_thing)
	{
		next_thing = j->next;	// Next in object list
		if (j->contains)
		{
			cont = TRUE;
		}
		else
		{
			cont = FALSE;
		}
		obj_zone_num = GET_OBJ_VNUM(j) / 100;
		if (((obj_zone_num == zone_num) && IS_OBJ_STAT(j, ITEM_REPOP_DECAY)))
		{
			/* F@N
			 * ���� ��� ���-������ �������� ��������� ����� ����������� �������,
			 * ���� ����� �����������
			*/
//                 || (GET_OBJ_TYPE(j) == ITEM_INGRADIENT && GET_OBJ_SKILL(j) > 19)
			if (j->worn_by)
				act("$o ��������$U, �������� ����� ������...", FALSE, j->worn_by, j, 0, TO_CHAR);
			else if (j->carried_by)
				act("$o ��������$U � ����� �����, �������� ����� ������...",
					FALSE, j->carried_by, j, 0, TO_CHAR);
			else if (j->in_room != NOWHERE)
			{
				if (world[j->in_room]->people)
				{
					act("$o ��������$U, �������� ����� ������...",
						FALSE, world[j->in_room]->people, j, 0, TO_CHAR);
					act("$o ��������$U, �������� ����� ������...",
						FALSE, world[j->in_room]->people, j, 0, TO_ROOM);
				}
			}
			extract_obj(j);
			if (cont)
				next_thing = object_list;
		}
	}
}
