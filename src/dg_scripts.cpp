/**************************************************************************
*  File: dg_scripts.cpp                                   Part of Bylins  *
*  Usage: contains general functions for using scripts.                   *
*                                                                         *
*                                                                         *
*  $Author$                                                        *
*  $Date$                                           *
*  $Revision$                                                      *
**************************************************************************/

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "dg_scripts.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "dg_event.h"
#include "db.h"
#include "screen.h"
#include "house.h"
#include "constants.h"
#include "top.h"
#include "features.hpp"
#include "char.hpp"
#include "char_player.hpp"
#include "name_list.hpp"
#include "modify.h"
#include "room.hpp"
#include "named_stuff.hpp"
#include "spells.h"
#include "skills.h"
#include "noob.hpp"
#include "genchar.h"

#define PULSES_PER_MUD_HOUR     (SECS_PER_MUD_HOUR*PASSES_PER_SEC)

#define IS_CHARMED(ch)          (IS_HORSE(ch)||AFF_FLAGGED(ch, AFF_CHARM))

// ����� ��������� � �������� ����������� ������������ DGScript
#define	DG_CODE_ANALYZE

// external vars from triggers.cpp
extern const char *trig_types[], *otrig_types[], *wtrig_types[];
const char *attach_name[] = { "mob", "obj", "room", "unknown!!!" };

int last_trig_vnum=0;

// other external vars

extern CHAR_DATA *character_list;
extern CHAR_DATA *combat_list;
extern OBJ_DATA *object_list;
extern const char *item_types[];
extern const char *genders[];
extern const char *pc_class_types[];
//extern const char *race_types[];
extern const char *exit_bits[];
extern INDEX_DATA *mob_index;
extern INDEX_DATA *obj_index;
extern TIME_INFO_DATA time_info;
extern struct zone_data *zone_table;
const char *spell_name(int num);

extern int can_take_obj(CHAR_DATA * ch, OBJ_DATA * obj);
extern void split_or_clan_tax(CHAR_DATA *ch, long amount);

// external functions
int ext_search_block(const char *arg, const char **list, int exact);
room_rnum find_target_room(CHAR_DATA * ch, char *rawroomstr, int trig);
void free_varlist(struct trig_var_data *vd);
int obj_room(OBJ_DATA * obj);
int is_empty(int zone_nr);
TRIG_DATA *read_trigger(int nr);
OBJ_DATA *get_object_in_equip(CHAR_DATA * ch, char *name);
void extract_trigger(TRIG_DATA * trig);
int eval_lhs_op_rhs(char *expr, char *result, void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type);
const char * skill_percent(CHAR_DATA * ch, char *skill);
bool feat_owner(CHAR_DATA * ch, char *feat);
const char * spell_count(CHAR_DATA * ch, char *spell);
const char * spell_knowledge(CHAR_DATA * ch, char *spell);
int find_eq_pos(CHAR_DATA * ch, OBJ_DATA * obj, char *arg);
void reset_zone(int znum);

void free_script(SCRIPT_DATA * sc);

ACMD(do_restore);
ACMD(do_mpurge);
ACMD(do_mjunk);

// function protos from this file
void extract_value(SCRIPT_DATA * sc, TRIG_DATA * trig, char *cmd);
int script_driver(void *go, TRIG_DATA * trig, int type, int mode);
int trgvar_in_room(int vnum);

void proto_script_copy(struct trig_proto_list **pdst, struct trig_proto_list *src)
/*++
   ������� ������ ���������� � �������� � ���� ��������� �� src.
   ���������, ��� pdst ��� �� �������� ������
   ��� ������������� �������������� �������� proto_script_free();

     pdst - ����� ����������, � ������� ����� ��������� �����
     src - ������, ����� �������� ����� �������
--*/
{
	for (pdst[0] = NULL; src; src = src->next, pdst = &(pdst[0]->next))
	{
		CREATE(pdst[0], struct trig_proto_list, 1);
		pdst[0]->vnum = src->vnum;
		// pdst[0]->next ���������� � ������� CREATE()
	}
}

void proto_script_free(struct trig_proto_list *src)
/*++
   ����������� ������, ���������� ������� ����������
     src - ������ ������ ����������
--*/
{
	struct trig_proto_list *tmp;
	while (src)
	{
		src = ((tmp = src)->next);
		free(tmp);
	}
}

void script_log(const char *msg, const int type)
{
	char tmpbuf[MAX_INPUT_LENGTH];
	char *pos;
	sprintf(tmpbuf, "SCRIPT LOG %s", msg);
	// ������ ������� �� ����� %-���.
	while ((pos = strchr(tmpbuf, '%')) != NULL)
		* pos = '$';

	mudlog(tmpbuf, type ? type : NRM, LVL_BUILDER, ERRLOG, TRUE);

}

/*
 *  Logs any errors caused by scripts to the system log.
 *  Will eventually allow on-line view of script errors.
 */
void trig_log(TRIG_DATA * trig, const char *msg, const int type)
{
	char tmpbuf[MAX_INPUT_LENGTH];
	sprintf(tmpbuf, "(Trigger: %s, VNum: %d) : %s", GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig), msg);
	script_log(tmpbuf, type);
}



struct cmdlist_element *find_end(TRIG_DATA * trig, struct cmdlist_element *cl);
struct cmdlist_element *find_done(TRIG_DATA * trig, struct cmdlist_element *cl);
struct cmdlist_element *find_case(TRIG_DATA * trig,
								  struct cmdlist_element *cl, void *go, SCRIPT_DATA * sc, int type, char *cond);
struct cmdlist_element *find_else_end(TRIG_DATA * trig,
									  struct cmdlist_element *cl, void *go, SCRIPT_DATA * sc, int type);

struct trig_var_data *worlds_vars;


// ��� ������������ ������ ������ "wat" � "mat"
int reloc_target = -1;
TRIG_DATA *cur_trig = NULL;

TRIG_DATA *trigger_list = NULL;	// all attached triggers

int trgvar_in_room(int vnum)
{
	int rnum = real_room(vnum);
	int i = 0;
	CHAR_DATA *ch;

	if (NOWHERE == rnum)
	{
		script_log("people.vnum: world[rnum] does not exist");
		return (-1);
	}

	for (ch = world[rnum]->people; ch != NULL; ch = ch->next_in_room)
		i++;

	return i;
};

OBJ_DATA *get_obj_in_list(char *name, OBJ_DATA * list)
{
	OBJ_DATA *i;
	long id;

	if (*name == UID_OBJ)
	{
		id = atoi(name + 1);

		for (i = list; i; i = i->next_content)
			if (id == GET_ID(i))
				return i;
	}
	else
	{
		for (i = list; i; i = i->next_content)
			if (isname(name, i->aliases))
				return i;
	}

	return NULL;
}

OBJ_DATA *get_object_in_equip(CHAR_DATA * ch, char *name)
{
	int j, n = 0, number;
	OBJ_DATA *obj;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;
	long id;

	if (*name == UID_OBJ)
	{
		id = atoi(name + 1);

		for (j = 0; j < NUM_WEARS; j++)
			if ((obj = GET_EQ(ch, j)))
				if (id == GET_ID(obj))
					return (obj);
	}
	else
	{
		strcpy(tmp, name);
		if (!(number = get_number(&tmp)))
			return NULL;

		for (j = 0; (j < NUM_WEARS) && (n <= number); j++)
			if ((obj = GET_EQ(ch, j)))
				if (isname(tmp, obj->aliases))
					if (++n == number)
						return (obj);
	}

	return NULL;
}

// * return number of object in world
const char * get_objs_in_world(OBJ_DATA * obj)
{
	int i;
	static char retval[16];
	if (!obj)
		return "";
	if ((i = GET_OBJ_RNUM(obj)) < 0)
	{
		log("DG_SCRIPTS : attemp count unknown object");
		return "";
	}
	sprintf(retval, "%d", obj_index[i].number + obj_index[i].stored);
	return retval;
}

// * return number of obj|mob in world by vnum
int gcount_char_vnum(long n)
{
	int count = 0;
	CHAR_DATA *ch;

	for (ch = character_list; ch; ch = ch->next)
		if (n == GET_MOB_VNUM(ch))
			count++;

	return (count);
}

int count_char_vnum(long n)
{
	int i;
	if ((i = real_mobile(n)) < 0)
		return 0;
	return (mob_index[i].number);
}

int gcount_obj_vnum(long n)
{
	int i;
	if ((i = real_object(n)) < 0)
		return 0;
	return (obj_index[i].number);
}

int count_obj_vnum(long n)
{
	int i;
	if ((i = real_object(n)) < 0)
		return 0;
	return (obj_index[i].number + obj_index[i].stored);
}

/************************************************************
 * search by number routines
 ************************************************************/

// return object with UID n
OBJ_DATA *find_obj(long n)
{
	OBJ_DATA *i;
	for (i = object_list; i; i = i->next)
		if (n == GET_ID(i))
			return i;
	return NULL;
}

// return room with UID n
room_data *find_room(long n)
{
	n = real_room(n - ROOM_ID_BASE);

	if ((n >= FIRST_ROOM) && (n <= top_of_world))
		return world[n];

	return NULL;
}

/************************************************************
 * search by VNUM routines
 ************************************************************/

/**
* ���������� id ���� ���������� �����.
* \param num - ���� ���� � ������ 0 - ���������� id �� ������� ����, � ���������� ����������� ������.
*/
int find_char_vnum(long n, int num = 0)
{
	CHAR_DATA *ch;
	int count = 0;
	for (ch = character_list; ch; ch = ch->next)
	{
		if (n == GET_MOB_VNUM(ch) && IN_ROOM(ch) != NOWHERE)
		{
			++count;
			if (num > 0 && num != count)
			{
				continue;
			}
			else
			{
				return (GET_ID(ch));
			}
		}
	}
	return -1;
}

// * ���������� find_char_vnum, ������ ��� ��������.
int find_obj_vnum(long n, int num = 0)
{
	OBJ_DATA *i;
	int count = 0;
	for (i = object_list; i; i = i->next)
	{
		if (n == GET_OBJ_VNUM(i))
		{
			++count;
			if (num > 0 && num != count)
			{
				continue;
			}
			else
			{
				return (GET_ID(i));
			}
		}
	}
	return -1;
}

// return room with VNUM n
// ��������! ��� ������� UID = ROOM_ID_BASE+VNUM, �.�.
// RNUM ����� ���� ���������� ������� � ������� OLC
int find_room_vnum(long n)
{
//  return (real_room (n) != NOWHERE) ? ROOM_ID_BASE + n : -1;
	return (real_room(n) != NOWHERE) ? n : -1;
}

int find_room_uid(long n)
{
	return (real_room(n) != NOWHERE) ? ROOM_ID_BASE + n : -1;
//return (real_room (n) != NOWHERE) ? n : -1;
}

/************************************************************
 * generic searches based only on name
 ************************************************************/

// search the entire world for a char, and return a pointer
CHAR_DATA *get_char(char *name, int vnum)
{
	CHAR_DATA *i;

	// �������� ����� ����� UID-��.
	if ((*name == UID_OBJ) || (*name == UID_ROOM))
		return NULL;

	if (*name == UID_CHAR)
	{
		i = find_char(atoi(name + 1));

		if (i && (IS_NPC(i) || !GET_INVIS_LEV(i)))
			return i;
	}
	else
	{
		for (i = character_list; i; i = i->next)
			if (isname(name, i->get_pc_name()) && (IS_NPC(i) || !GET_INVIS_LEV(i)))
				return i;
//		return CharacterAlias::get_by_name(name);
	}

	return NULL;
}


// returns the object in the world with name name, or NULL if not found
OBJ_DATA *get_obj(char *name, int vnum)
{
	long id;

	if ((*name == UID_CHAR) || (*name == UID_ROOM))
		return NULL;

	if (*name == UID_OBJ)
	{
		id = atoi(name + 1);
		return find_obj(id);
	}
	else
	{
		for (OBJ_DATA *obj = object_list; obj; obj = obj->next)
			if (isname(name, obj->aliases))
				return obj;
//		return ObjectAlias::get_by_name(name);
	}
	return NULL;
}


// finds room by with name.  returns NULL if not found
room_data *get_room(char *name)
{
	int nr;

	if ((*name == UID_CHAR) || (*name == UID_OBJ))
		return NULL;

	if (*name == UID_ROOM)
		return find_room(atoi(name + 1));
	else if ((nr = real_room(atoi(name))) == NOWHERE)
		return NULL;
	else
		return world[nr];
}


/*
 * returns a pointer to the first character in world by name name,
 * or NULL if none found.  Starts searching with the person owing the object
 */
CHAR_DATA *get_char_by_obj(OBJ_DATA * obj, char *name)
{
	CHAR_DATA *ch;

	if ((*name == UID_ROOM) || (*name == UID_OBJ))
		return NULL;

	if (*name == UID_CHAR)
	{
		ch = find_char(atoi(name + 1));

		if (ch && (IS_NPC(ch) || !GET_INVIS_LEV(ch)))
			return ch;
	}
	else
	{
		if (obj->carried_by &&
				isname(name, obj->carried_by->get_pc_name()) &&
				(IS_NPC(obj->carried_by) || !GET_INVIS_LEV(obj->carried_by)))
			return obj->carried_by;

		if (obj->worn_by &&
				isname(name, obj->worn_by->get_pc_name()) && (IS_NPC(obj->worn_by) || !GET_INVIS_LEV(obj->worn_by)))
			return obj->worn_by;

		for (ch = character_list; ch; ch = ch->next)
			if (isname(name, ch->get_pc_name()) && (IS_NPC(ch) || !GET_INVIS_LEV(ch)))
				return ch;
	}

	return NULL;
}


/*
 * returns a pointer to the first character in world by name name,
 * or NULL if none found.  Starts searching in room room first
 */
CHAR_DATA *get_char_by_room(room_data * room, char *name)
{
	CHAR_DATA *ch;

	if ((*name == UID_ROOM) || (*name == UID_OBJ))
		return NULL;

	if (*name == UID_CHAR)
	{
		ch = find_char(atoi(name + 1));

		if (ch && (IS_NPC(ch) || !GET_INVIS_LEV(ch)))
			return ch;
	}
	else
	{
		for (ch = room->people; ch; ch = ch->next_in_room)
			if (isname(name, ch->get_pc_name()) && (IS_NPC(ch) || !GET_INVIS_LEV(ch)))
				return ch;

		for (ch = character_list; ch; ch = ch->next)
			if (isname(name, ch->get_pc_name()) && (IS_NPC(ch) || !GET_INVIS_LEV(ch)))
				return ch;
	}

	return NULL;
}


/*
 * returns the object in the world with name name, or NULL if not found
 * search based on obj
 */
OBJ_DATA *get_obj_by_obj(OBJ_DATA * obj, char *name)
{
	OBJ_DATA *i = NULL;
	int rm;
	long id;

	if ((*name == UID_ROOM) || (*name == UID_CHAR))
		return NULL;

	if (!str_cmp(name, "self") || !str_cmp(name, "me"))
		return obj;

	if (obj->contains && (i = get_obj_in_list(name, obj->contains)))
		return i;

	if (obj->in_obj)
	{
		if (*name == UID_OBJ)
		{
			id = atoi(name + 1);

			if (id == GET_ID(obj->in_obj))
				return obj->in_obj;
		}
		else if (isname(name, obj->in_obj->aliases))
			return obj->in_obj;
	}
	else if (obj->worn_by && (i = get_object_in_equip(obj->worn_by, name)))
		return i;
	else if (obj->carried_by && (i = get_obj_in_list(name, obj->carried_by->carrying)))
		return i;
	else if (((rm = obj_room(obj)) != NOWHERE) && (i = get_obj_in_list(name, world[rm]->contents)))
		return i;

	if (*name == UID_OBJ)
	{
		id = atoi(name + 1);

		for (i = object_list; i; i = i->next)
			if (id == GET_ID(i))
				break;
	}
	else
	{
		for (i = object_list; i; i = i->next)
			if (isname(name, i->aliases))
				break;
	}

	return i;
}


// returns obj with name
OBJ_DATA *get_obj_by_room(room_data * room, char *name)
{
	OBJ_DATA *obj;
	long id;
	if ((*name == UID_ROOM) || (*name == UID_CHAR))
		return NULL;

	if (*name == UID_OBJ)
	{
		id = atoi(name + 1);

		for (obj = room->contents; obj; obj = obj->next_content)
			if (id == GET_ID(obj))
				return obj;

		for (obj = object_list; obj; obj = obj->next)
			if (id == GET_ID(obj))
				return obj;
	}
	else
	{
		for (obj = room->contents; obj; obj = obj->next_content)
			if (isname(name, obj->aliases))
				return obj;

		for (obj = object_list; obj; obj = obj->next)
			if (isname(name, obj->aliases))
				return obj;
	}

	return NULL;
}


// returns obj with name
OBJ_DATA *get_obj_by_char(CHAR_DATA * ch, char *name)
{
	OBJ_DATA *obj;
	long id;
	if ((*name == UID_ROOM) || (*name == UID_CHAR))
		return NULL;

	if (*name == UID_OBJ)
	{
		id = atoi(name + 1);
		if (ch)
			for (obj = ch->carrying; obj; obj = obj->next_content)
				if (id == GET_ID(obj))
					return obj;

		for (obj = object_list; obj; obj = obj->next)
			if (id == GET_ID(obj))
				return obj;
	}
	else
	{
		if (ch)
			for (obj = ch->carrying; obj; obj = obj->next_content)
				if (isname(name, obj->aliases))
					return obj;

		for (obj = object_list; obj; obj = obj->next)
			if (isname(name, obj->aliases))
				return obj;
	}

	return NULL;
}


// checks every PLUSE_SCRIPT for random triggers
void script_trigger_check(void)
{
	CHAR_DATA *ch;
	OBJ_DATA *obj;
	ROOM_DATA *room = NULL;
	int nr;
	SCRIPT_DATA *sc;

	for (ch = character_list; ch; ch = ch->next)
	{
		if (SCRIPT(ch))
		{
			sc = SCRIPT(ch);

			if (IS_SET(SCRIPT_TYPES(sc), MTRIG_RANDOM) &&
					(!is_empty(world[IN_ROOM(ch)]->zone) || IS_SET(SCRIPT_TYPES(sc), MTRIG_GLOBAL)))
				random_mtrigger(ch);
		}
	}

	for (obj = object_list; obj; obj = obj->next)
	{
		if(OBJ_FLAGGED(obj, ITEM_NAMED))
		{
			if(obj->worn_by && number(1, 100) <= 5)
				NamedStuff::wear_msg(obj->worn_by, obj);
		}
		else if (SCRIPT(obj))
		{
			{
				sc = SCRIPT(obj);
				if (IS_SET(SCRIPT_TYPES(sc), OTRIG_RANDOM))
					random_otrigger(obj);
			}
		}
	}

	for (nr = FIRST_ROOM; nr <= top_of_world; nr++)
	{
		if (SCRIPT(world[nr]))
		{
			room = world[nr];
			sc = SCRIPT(room);

			if (IS_SET(SCRIPT_TYPES(sc), WTRIG_RANDOM) &&
					(!is_empty(room->zone) || IS_SET(SCRIPT_TYPES(sc), WTRIG_GLOBAL)))
// ���� ����� ���� (� �� ���������� �����) ����� ����� ���������� ��������� � �����
				random_wtrigger(room, room->number, sc, sc->types, sc->trig_list, sc->next);
		}
	}
}

// �������� ������ ��� �� ����� ��������� �������
void script_timechange_trigger_check(const int time)
{
	CHAR_DATA *ch;
	OBJ_DATA *obj;
	ROOM_DATA *room = NULL;
	int nr;
	SCRIPT_DATA *sc;

	for (ch = character_list; ch; ch = ch->next)
	{
		if (SCRIPT(ch))
		{
			sc = SCRIPT(ch);

			if (IS_SET(SCRIPT_TYPES(sc), MTRIG_TIMECHANGE) &&
					(!is_empty(world[IN_ROOM(ch)]->zone) || IS_SET(SCRIPT_TYPES(sc), MTRIG_GLOBAL)))
				timechange_mtrigger(ch, time);
		}
	}

	for (obj = object_list; obj; obj = obj->next)
	{
		if (SCRIPT(obj))
		{
			{
				sc = SCRIPT(obj);
				if (IS_SET(SCRIPT_TYPES(sc), OTRIG_TIMECHANGE))
					timechange_otrigger(obj, time);
			}
		}
	}

	for (nr = FIRST_ROOM; nr <= top_of_world; nr++)
	{
		if (SCRIPT(world[nr]))
		{
			room = world[nr];
			sc = SCRIPT(room);

			if (IS_SET(SCRIPT_TYPES(sc), WTRIG_TIMECHANGE) &&
					(!is_empty(room->zone) || IS_SET(SCRIPT_TYPES(sc), WTRIG_GLOBAL)))
				timechange_wtrigger(room, time);
		}
	}
}

EVENT(trig_wait_event)
{
	struct wait_event_data *wait_event_obj = (struct wait_event_data *) info;
	TRIG_DATA *trig;
	void *go;
	int type;

	trig = wait_event_obj->trigger;
	go = wait_event_obj->go;
	type = wait_event_obj->type;

	GET_TRIG_WAIT(trig) = NULL;

	script_driver(go, trig, type, TRIG_RESTART);
	free(wait_event_obj);
}


void do_stat_trigger(CHAR_DATA * ch, TRIG_DATA * trig)
{
	struct cmdlist_element *cmd_list;
	char sb[MAX_EXTEND_LENGTH];

	if (!trig)
	{
		log("SYSERR: NULL trigger passed to do_stat_trigger.");
		return;
	}

	sprintf(sb, "Name: '%s%s%s',  VNum: [%s%5d%s], RNum: [%5d]\r\n",
			CCYEL(ch, C_NRM), GET_TRIG_NAME(trig), CCNRM(ch, C_NRM),
			CCGRN(ch, C_NRM), GET_TRIG_VNUM(trig), CCNRM(ch, C_NRM), GET_TRIG_RNUM(trig));

	if (trig->attach_type == MOB_TRIGGER)
	{
		send_to_char("Trigger Intended Assignment: Mobiles\r\n", ch);
		sprintbit(GET_TRIG_TYPE(trig), trig_types, buf);
	}
	else if (trig->attach_type == OBJ_TRIGGER)
	{
		send_to_char("Trigger Intended Assignment: Objects\r\n", ch);
		sprintbit(GET_TRIG_TYPE(trig), otrig_types, buf);
	}
	else if (trig->attach_type == WLD_TRIGGER)
	{
		send_to_char("Trigger Intended Assignment: Rooms\r\n", ch);
		sprintbit(GET_TRIG_TYPE(trig), wtrig_types, buf);
	}
	else
	{
		send_to_char(ch, "Trigger Intended Assignment: undefined (attach_type=%d)\r\n",
					 static_cast<int>(trig->attach_type));
	}

	sprintf(sb, "Trigger Type: %s, Numeric Arg: %d, Arg list: %s\r\n",
			buf, GET_TRIG_NARG(trig), ((GET_TRIG_ARG(trig) && *GET_TRIG_ARG(trig))
									   ? GET_TRIG_ARG(trig) : "None"));

	strcat(sb, "Commands:\r\n   ");

	cmd_list = trig->cmdlist;
	while (cmd_list)
	{
		if (cmd_list->cmd)
		{
			strcat(sb, cmd_list->cmd);
			strcat(sb, "\r\n   ");
		}

		cmd_list = cmd_list->next;
	}

	page_string(ch->desc, sb, 1);
}


// find the name of what the uid points to
void find_uid_name(char *uid, char *name)
{
	CHAR_DATA *ch;
	OBJ_DATA *obj;

	if ((ch = get_char(uid)))
		strcpy(name, ch->get_pc_name());
	else if ((obj = get_obj(uid)))
		strcpy(name, obj->aliases);
	else
		sprintf(name, "uid = %s, (not found)", uid + 1);
}


// general function to display stats on script sc
void script_stat(CHAR_DATA * ch, SCRIPT_DATA * sc)
{
	struct trig_var_data *tv;
	TRIG_DATA *t;
	char name[MAX_INPUT_LENGTH];
	char namebuf[512];

	sprintf(buf, "Global Variables: %s\r\n", sc->global_vars ? "" : "None");
	send_to_char(buf, ch);
	sprintf(buf, "Global context: %ld\r\n", sc->context);
	send_to_char(buf, ch);

	for (tv = sc->global_vars; tv; tv = tv->next)
	{
		sprintf(namebuf, "%s:%ld", tv->name, tv->context);
		if (*(tv->value) == UID_CHAR || *(tv->value) == UID_ROOM || *(tv->value) == UID_OBJ)
		{
			find_uid_name(tv->value, name);
			sprintf(buf, "    %15s:  %s\r\n", tv->context ? namebuf : tv->name, name);
		}
		else
			sprintf(buf, "    %15s:  %s\r\n", tv->context ? namebuf : tv->name, tv->value);
		send_to_char(buf, ch);
	}

	for (t = TRIGGERS(sc); t; t = t->next)
	{
		sprintf(buf, "\r\n  Trigger: %s%s%s, VNum: [%s%5d%s], RNum: [%5d]\r\n",
				CCYEL(ch, C_NRM), GET_TRIG_NAME(t), CCNRM(ch, C_NRM),
				CCGRN(ch, C_NRM), GET_TRIG_VNUM(t), CCNRM(ch, C_NRM), GET_TRIG_RNUM(t));
		send_to_char(buf, ch);

		if (t->attach_type == MOB_TRIGGER)
		{
			send_to_char("  Trigger Intended Assignment: Mobiles\r\n", ch);
			sprintbit(GET_TRIG_TYPE(t), trig_types, buf1);
		}
		else if (t->attach_type == OBJ_TRIGGER)
		{
			send_to_char("  Trigger Intended Assignment: Objects\r\n", ch);
			sprintbit(GET_TRIG_TYPE(t), otrig_types, buf1);
		}
		else if (t->attach_type == WLD_TRIGGER)
		{
			send_to_char("  Trigger Intended Assignment: Rooms\r\n", ch);
			sprintbit(GET_TRIG_TYPE(t), wtrig_types, buf1);
		}
		else
		{
			send_to_char(ch, "Trigger Intended Assignment: undefined (attach_type=%d)\r\n",
						 static_cast<int>(t->attach_type));
		}

		sprintf(buf, "  Trigger Type: %s, Numeric Arg: %d, Arg list: %s\r\n",
				buf1, GET_TRIG_NARG(t), ((GET_TRIG_ARG(t) && *GET_TRIG_ARG(t)) ? GET_TRIG_ARG(t) : "None"));
		send_to_char(buf, ch);

		if (GET_TRIG_WAIT(t))
		{
			if (t->curr_state != NULL)
			{
				sprintf(buf, "    Wait: %d, Current line: %s\r\n",
						GET_TRIG_WAIT(t)->time_remaining, t->curr_state->cmd);
				send_to_char(buf, ch);
			}
			else
			{
				sprintf(buf, "    Wait: %d\r\n", GET_TRIG_WAIT(t)->time_remaining);
				send_to_char(buf, ch);
			}

			sprintf(buf, "  Variables: %s\r\n", GET_TRIG_VARS(t) ? "" : "None");
			send_to_char(buf, ch);

			for (tv = GET_TRIG_VARS(t); tv; tv = tv->next)
			{
				if (*(tv->value) == UID_CHAR || *(tv->value) == UID_ROOM || *(tv->value) == UID_OBJ)
				{
					find_uid_name(tv->value, name);
					sprintf(buf, "    %15s:  %s\r\n", tv->name, name);
				}
				else
					sprintf(buf, "    %15s:  %s\r\n", tv->name, tv->value);
				send_to_char(buf, ch);
			}
		}
	}
}


void do_sstat_room(CHAR_DATA * ch)
{
	ROOM_DATA *rm = world[ch->in_room];

	do_sstat_room(rm, ch);

}
void do_sstat_room(ROOM_DATA *rm, CHAR_DATA * ch)
{
	send_to_char("Script information:\r\n", ch);
	if (!SCRIPT(rm))
	{
		send_to_char("  None.\r\n", ch);
		return;
	}

	script_stat(ch, SCRIPT(rm));
}


void do_sstat_object(CHAR_DATA * ch, OBJ_DATA * j)
{
	send_to_char("Script information:\r\n", ch);
	if (!SCRIPT(j))
	{
		send_to_char("  None.\r\n", ch);
		return;
	}

	script_stat(ch, SCRIPT(j));
}


void do_sstat_character(CHAR_DATA * ch, CHAR_DATA * k)
{
	send_to_char("Script information:\r\n", ch);
	if (!SCRIPT(k))
	{
		send_to_char("  None.\r\n", ch);
		return;
	}

	script_stat(ch, SCRIPT(k));
}


/*
 * adds the trigger t to script sc in in location loc.  loc = -1 means
 * add to the end, loc = 0 means add before all other triggers.
 */
void add_trigger(SCRIPT_DATA * sc, TRIG_DATA * t, int loc)
{
	TRIG_DATA *i;
	int n;

	if (!t || !sc)
		return;
	for (i = TRIGGERS(sc); i; i = i->next)
		if (t->nr == i->nr)
		{
			/*
			 * � ��� ��� ������������� ����������!
			 * ���������� ������� ������ ��������, ��� ������� ����� ��������
			 * � ��������������� ������, � �� �������, ��� ����� ������� ���
			 * �� �����. �������� ����������� � ���, ��� �� ���� ������ �� �����!
			 */
			extract_trigger(t);
			return;
		}

	for (n = loc, i = TRIGGERS(sc); i && i->next && (n != 0); n--, i = i->next);

	if (!loc)
	{
		t->next = TRIGGERS(sc);
		TRIGGERS(sc) = t;
	}
	else if (!i)
		TRIGGERS(sc) = t;
	else
	{
		t->next = i->next;
		i->next = t;
	}

	SCRIPT_TYPES(sc) |= GET_TRIG_TYPE(t);

	t->next_in_world = trigger_list;
	trigger_list = t;
}


ACMD(do_attach)
{
	CHAR_DATA *victim;
	OBJ_DATA *object;
	TRIG_DATA *trig;
	char targ_name[MAX_INPUT_LENGTH], trig_name[MAX_INPUT_LENGTH];
	char loc_name[MAX_INPUT_LENGTH];
	int loc, room, tn, rn;


	argument = two_arguments(argument, arg, trig_name);
	two_arguments(argument, targ_name, loc_name);

	if (!*arg || !*targ_name || !*trig_name)
	{
		send_to_char("Usage: attach { mtr | otr | wtr } { trigger } { name } [ location ]\r\n", ch);
		return;
	}

	tn = atoi(trig_name);
	loc = (*loc_name) ? atoi(loc_name) : -1;

	rn = real_trigger(tn);
	if (rn >= 0 && ((is_abbrev(arg, "mtr") && trig_index[rn]->proto->attach_type != MOB_TRIGGER) ||
					(is_abbrev(arg, "otr") && trig_index[rn]->proto->attach_type != OBJ_TRIGGER) ||
					(is_abbrev(arg, "wtr") && trig_index[rn]->proto->attach_type != WLD_TRIGGER)))
	{
		tn = (is_abbrev(arg, "mtr") ? 0 : is_abbrev(arg, "otr") ? 1 : is_abbrev(arg, "wtr") ? 2 : 3);
		sprintf(buf, "Trigger %d (%s) has wrong attach_type %s expected %s.\r\n",
				tn, GET_TRIG_NAME(trig_index[rn]->proto), attach_name[(int)trig_index[rn]->proto->attach_type], attach_name[tn]);
		send_to_char(buf, ch);
		return;
	}
	if (is_abbrev(arg, "mtr"))
	{
		if ((victim = get_char_vis(ch, targ_name, FIND_CHAR_WORLD)))
		{
			//if (IS_NPC(victim))  	// have a valid mob, now get trigger
			{
				rn = real_trigger(tn);
				if ((rn >= 0) && (trig = read_trigger(rn)))
				{
					if (!SCRIPT(victim))
						CREATE(SCRIPT(victim), SCRIPT_DATA, 1);
					add_trigger(SCRIPT(victim), trig, loc);

					sprintf(buf, "Trigger %d (%s) attached to %s.\r\n",
							tn, GET_TRIG_NAME(trig), GET_SHORT(victim));
					send_to_char(buf, ch);
				}
				else
					send_to_char("That trigger does not exist.\r\n", ch);
			}
			//else //������ � ������ ����� ���� � �������
			//	send_to_char("Players can't have scripts.\r\n", ch);
		}
		else
			send_to_char("That mob does not exist.\r\n", ch);
	}
	else if (is_abbrev(arg, "otr"))
	{
		if ((object = get_obj_vis(ch, targ_name)))  	// have a valid obj, now get trigger
		{
			rn = real_trigger(tn);
			if ((rn >= 0) && (trig = read_trigger(rn)))
			{
				if (!SCRIPT(object))
					CREATE(SCRIPT(object), SCRIPT_DATA, 1);
				add_trigger(SCRIPT(object), trig, loc);

				sprintf(buf, "Trigger %d (%s) attached to %s.\r\n",
						tn, GET_TRIG_NAME(trig),
						(object->short_description ? object->short_description : object->aliases));
				send_to_char(buf, ch);
			}
			else
				send_to_char("That trigger does not exist.\r\n", ch);
		}
		else
			send_to_char("That object does not exist.\r\n", ch);
	}
	else if (is_abbrev(arg, "wtr"))
	{
		if (isdigit(*targ_name) && !strchr(targ_name, '.'))
		{
			if ((room = find_target_room(ch, targ_name, 0)) != NOWHERE)  	// have a valid room, now get trigger
			{
				rn = real_trigger(tn);
				if ((rn >= 0) && (trig = read_trigger(rn)))
				{
					if (!(world[room]->script))
						CREATE(world[room]->script, SCRIPT_DATA, 1);
					add_trigger(world[room]->script, trig, loc);

					sprintf(buf, "Trigger %d (%s) attached to room %d.\r\n",
							tn, GET_TRIG_NAME(trig), world[room]->number);
					send_to_char(buf, ch);
				}
				else
					send_to_char("That trigger does not exist.\r\n", ch);
			}
		}
		else
			send_to_char("You need to supply a room number.\r\n", ch);
	}
	else
		send_to_char("Please specify 'mtr', otr', or 'wtr'.\r\n", ch);
}


/*
 *  removes the trigger specified by name, and the script of o if
 *  it removes the last trigger.  name can either be a number, or
 *  a 'silly' name for the trigger, including things like 2.beggar-death.
 *  returns 0 if did not find the trigger, otherwise 1.  If it matters,
 *  you might need to check to see if all the triggers were removed after
 *  this function returns, in order to remove the script.
 */
int remove_trigger(SCRIPT_DATA * sc, char *name, TRIG_DATA ** trig_addr)
{
	TRIG_DATA *i, *j;
	int num = 0, string = FALSE, n;
	char *cname;


	if (!sc)
		return 0;

	if ((cname = strstr(name, ".")) || (!isdigit(*name)))
	{
		string = TRUE;
		if (cname)
		{
			*cname = '\0';
			num = atoi(name);
			name = ++cname;
		}
	}
	else
		num = atoi(name);

	for (n = 0, j = NULL, i = TRIGGERS(sc); i; j = i, i = i->next)
	{
		if (string)
		{
			if (isname(name, GET_TRIG_NAME(i)))
				if (++n >= num)
					break;
		}

		// this isn't clean...
		// a numeric value will match if it's position OR vnum
		// is found. originally the number was position-only
		else if (++n >= num)
			break;
		else if (trig_index[i->nr]->vnum == num)
			break;
	}

	if (i)
	{
		if (i == *trig_addr)
			*trig_addr = NULL;
		if (j)
		{
			j->next = i->next;
			extract_trigger(i);
		}
		// this was the first trigger
		else
		{
			TRIGGERS(sc) = i->next;
			extract_trigger(i);
		}
		// update the script type bitvector
		SCRIPT_TYPES(sc) = 0;
		for (i = TRIGGERS(sc); i; i = i->next)
			SCRIPT_TYPES(sc) |= GET_TRIG_TYPE(i);
		return 1;
	}
	else
		return 0;
}

ACMD(do_detach)
{
	CHAR_DATA *victim = NULL;
	OBJ_DATA *object = NULL;
	ROOM_DATA *room;
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
	char *trigger = 0;
	int tmp;
	TRIG_DATA *dummy;

	argument = two_arguments(argument, arg1, arg2);
	one_argument(argument, arg3);

	if (!*arg1 || !*arg2)
	{
		send_to_char("Usage: detach [ mob | object | room ] { target } { trigger |" " 'all' }\r\n", ch);
		return;
	}

	if (!str_cmp(arg1, "room"))
	{
		room = world[IN_ROOM(ch)];
		if (!SCRIPT(room))
			send_to_char("This room does not have any triggers.\r\n", ch);
		else if (!str_cmp(arg2, "all") || !str_cmp(arg2, "���"))
		{
			free_script(SCRIPT(room));
			SCRIPT(room) = NULL;
			send_to_char("All triggers removed from room.\r\n", ch);
		}
		else if (remove_trigger(SCRIPT(room), arg2, &dummy))
		{
			send_to_char("Trigger removed.\r\n", ch);
			if (!TRIGGERS(SCRIPT(room)))
			{
				free_script(SCRIPT(room));
				SCRIPT(room) = NULL;
			}
		}
		else
			send_to_char("That trigger was not found.\r\n", ch);
	}
	else
	{
		if (is_abbrev(arg1, "mob"))
		{
			if (!(victim = get_char_vis(ch, arg2, FIND_CHAR_WORLD)))
				send_to_char("No such mobile around.\r\n", ch);
			else if (!*arg3)
				send_to_char("You must specify a trigger to remove.\r\n", ch);
			else
				trigger = arg3;
		}
		else if (is_abbrev(arg1, "object"))
		{
			if (!(object = get_obj_vis(ch, arg2)))
				send_to_char("No such object around.\r\n", ch);
			else if (!*arg3)
				send_to_char("You must specify a trigger to remove.\r\n", ch);
			else
				trigger = arg3;
		}
		else
		{
			if ((object = get_object_in_equip_vis(ch, arg1, ch->equipment, &tmp)));
			else if ((object = get_obj_in_list_vis(ch, arg1, ch->carrying)));
			else if ((victim = get_char_room_vis(ch, arg1)));
			else if ((object = get_obj_in_list_vis(ch, arg1, world[IN_ROOM(ch)]->contents)));
			else if ((victim = get_char_vis(ch, arg1, FIND_CHAR_WORLD)));
			else if ((object = get_obj_vis(ch, arg1)));
			else
				send_to_char("Nothing around by that name.\r\n", ch);
			trigger = arg2;
		}

		if (victim)
		{
			if (!IS_NPC(victim))
				send_to_char("Players don't have triggers.\r\n", ch);
			else if (!SCRIPT(victim))
				send_to_char("That mob doesn't have any triggers.\r\n", ch);
			else if (!str_cmp(arg2, "all") || !str_cmp(arg2, "���"))
			{
				free_script(SCRIPT(victim));
				SCRIPT(victim) = NULL;
				sprintf(buf, "All triggers removed from %s.\r\n", GET_SHORT(victim));
				send_to_char(buf, ch);
			}
			else if (trigger && remove_trigger(SCRIPT(victim), trigger, &dummy))
			{
				send_to_char("Trigger removed.\r\n", ch);
				if (!TRIGGERS(SCRIPT(victim)))
				{
					free_script(SCRIPT(victim));
					SCRIPT(victim) = NULL;
				}
			}
			else
				send_to_char("That trigger was not found.\r\n", ch);
		}
		else if (object)
		{
			if (!SCRIPT(object))
				send_to_char("That object doesn't have any triggers.\r\n", ch);
			else if (!str_cmp(arg2, "all") || !str_cmp(arg2, "���"))
			{
				free_script(SCRIPT(object));
				SCRIPT(object) = NULL;
				sprintf(buf, "All triggers removed from %s.\r\n",
						object->short_description ? object->short_description : object->aliases);
				send_to_char(buf, ch);
			}
			else if (remove_trigger(SCRIPT(object), trigger, &dummy))
			{
				send_to_char("Trigger removed.\r\n", ch);
				if (!TRIGGERS(SCRIPT(object)))
				{
					free_script(SCRIPT(object));
					SCRIPT(object) = NULL;
				}
			}
			else
				send_to_char("That trigger was not found.\r\n", ch);
		}
	}

}


// frees memory associated with var
void free_var_el(struct trig_var_data *var)
{
	free(var->name);
	free(var->value);
	free(var);
}


void add_var_cntx(struct trig_var_data **var_list, const char *name, const char *value, long id)
/*++
	���������� ���������� � ������ � ������ ��������� (������� �����).
    ��� ���������� � ������ ��������� ���������� �������� ������ ���� 0.

	var_list	- ��������� �� ������ ������� ������ ����������
    name		- ��� ����������
	value		- �������� ����������
	id			- �������� ����������
--*/
{
	struct trig_var_data *vd, *vd_prev;

// ������� ��������, ��� ��� ���������� ���������� ������ ���������� ����������
	for (vd_prev = NULL, vd = *var_list;
			vd && ((vd->context != id) || str_cmp(vd->name, name)); vd_prev = vd, vd = vd->next);

	if (vd)
	{
// ���������� ����������, �������� ��������
		free(vd->value);
	}
	else
	{
// ������� ����� ����������
		CREATE(vd, struct trig_var_data, 1);
		CREATE(vd->name, char, strlen(name) + 1);
		strcpy(vd->name, name);

		vd->context = id;

// ���������� ���������� � ������
// ��� �������� ��������� � ����� ������, ��� ���������� - � ������
		if (id)
		{
			vd->next = *var_list;
			*var_list = vd;
		}
		else
		{
			vd->next = NULL;
			if (vd_prev)
				vd_prev->next = vd;
			else
				*var_list = vd;
		}
	}

	CREATE(vd->value, char, strlen(value) + 1);
	strcpy(vd->value, value);
}


struct trig_var_data *find_var_cntx(struct trig_var_data **var_list, char *name, long id)
/*++
	����� ���������� � ������ ��������� (��������� �����).

	����� �������������� �� ���� ���:��������.
	1. ��� ���������� ������ ��������� � ���������� name
    2. �������� ���������� ������ ��������� � ���������� id, ����
       ����� ���������� ���, ������������ ������� ����� ����������
       � ���������� 0.

	var_list	- ��������� �� ������ ������� ������ ����������
    name		- ��� ����������
	id			- �������� ����������
--*/
{
	struct trig_var_data *vd;

	for (vd = *var_list; vd && ((vd->context && vd->context != id) || str_cmp(vd->name, name)); vd = vd->next);

	return vd;
}


int remove_var_cntx(struct trig_var_data **var_list, char *name, long id)
/*++
	�������� ���������� �� ������ � ������ ��������� (������� �����).

	����� �������.

	var_list	- ��������� �� ������ ������� ������ ����������
    name		- ��� ����������
	id			- �������� ����������

	����������:
       1 - ���������� ������� � �������
       0 - ���������� �� �������

--*/
{
	struct trig_var_data *vd, *vd_prev;

	for (vd_prev = NULL, vd = *var_list;
			vd && ((vd->context != id) || str_cmp(vd->name, name)); vd_prev = vd, vd = vd->next);

	if (!vd)
		return 0;	// �� ������� �����

	if (vd_prev)
		vd_prev->next = vd->next;
	else
		*var_list = vd->next;

	free_var_el(vd);

	return 1;
}


// * ��������� ��������� ������������� ���������
long gm_char_field(CHAR_DATA * ch, char *field, char *subfield, long val)
{
	int tmpval;
	if (*subfield)
	{
		sprintf(buf, "DG_Script: Set %s with <%s> for %s.", field, subfield, GET_NAME(ch));
		mudlog(buf, NRM, -1, ERRLOG, TRUE);
		if (*subfield == '-')
			return (val - atoi(subfield + 1));
		else if (*subfield == '+')
			return (val + atoi(subfield + 1));
		else if ((tmpval = atoi(subfield)) > 0)
			return tmpval;
	}
	return val;
}

// ��������� ���������� �����
void gm_flag(char *subfield, const char **list, FLAG_DATA & val, char *res)
{
	long flag;

	strcpy(res, "0");

	if (*subfield)
	{
		if (*subfield == '-')
		{
			flag = ext_search_block(subfield + 1, list, FALSE);
			if (flag && REMOVE_BIT(GET_FLAG(val, flag), flag))
				strcpy(res, "1");
		}
		else if (*subfield == '+')
		{
			flag = ext_search_block(subfield + 1, list, FALSE);
			if (flag && SET_BIT(GET_FLAG(val, flag), flag))
				strcpy(res, "1");

		}
		else
		{
			flag = ext_search_block(subfield, list, FALSE);
			if (flag && IS_SET(GET_FLAG(val, flag), flag))
				strcpy(res, "1");
		}
	}
	return;
}

int text_processed(char *field, char *subfield, struct trig_var_data *vd, char *str)
{
	char *p, *p2;
	*str = '\0';
	if (!vd || !vd->name || !vd->value)
		return FALSE;

	if (!str_cmp(field, "strlen"))  	// strlen
	{
		sprintf(str, "%lu", static_cast<unsigned long>(strlen(vd->value)));
		return TRUE;
	}
	else if (!str_cmp(field, "trim"))  	// trim
	{
		// trim whitespace from ends
		p = vd->value;
		p2 = vd->value + strlen(vd->value) - 1;
		while (*p && a_isspace(*p))
			p++;
		while ((p >= p2) && a_isspace(*p2))
			p2--;
		if (p > p2)  	// nothing left
		{
			*str = '\0';
			return TRUE;
		}
		while (p <= p2)
			*str++ = *p++;
		*str = '\0';
		return TRUE;
	}
	else if (!str_cmp(field, "contains"))  	// contains
	{
		if (str_str(vd->value, subfield))
			sprintf(str, "1");
		else
			sprintf(str, "0");
		return TRUE;
	}
	else if (!str_cmp(field, "car"))  	// car
	{
		char *car = vd->value;
		while (*car && !a_isspace(*car))
			*str++ = *car++;
		*str = '\0';
		return TRUE;
	}
	else if (!str_cmp(field, "cdr"))  	// cdr
	{
		char *cdr = vd->value;
		while (*cdr && !a_isspace(*cdr))
			cdr++;	// skip 1st field
		while (*cdr && a_isspace(*cdr))
			cdr++;	// skip to next
		while (*cdr)
			*str++ = *cdr++;
		*str = '\0';
		return TRUE;
	}
	else if (!str_cmp(field, "words"))
	{
		int n = 0;
		// ������� ���������� ���� ��� ��������� �����
		char buf1[MAX_INPUT_LENGTH];
		char buf2[MAX_INPUT_LENGTH];
		buf1[0] = 0;
		strcpy(buf2, vd->value);
		if (*subfield)
		{
			for (n = atoi(subfield); n; --n)
			{
				half_chop(buf2, buf1, buf2);
			}
			strcpy(str, buf1);
		}
		else
		{
			while (buf2[0] != 0)
			{
				half_chop(buf2, buf1, buf2);
				++n;
			}
			sprintf(str, "%d", n);
		}
		return TRUE;
	}
	else if (!str_cmp(field, "mudcommand"))  	// find the mud command returned from this text
	{
		// NOTE: you may need to replace "cmd_info" with "complete_cmd_info",
		// depending on what patches you've got applied.
		extern const struct command_info cmd_info[];
		// on older source bases:    extern struct command_info *cmd_info;
		int length, cmd;
		for (length = strlen(vd->value), cmd = 0; *cmd_info[cmd].command != '\n'; cmd++)
			if (!strncmp(cmd_info[cmd].command, vd->value, length))
				break;

		if (*cmd_info[cmd].command == '\n')
			strcpy(str, "");
		else
			strcpy(str, cmd_info[cmd].command);
		return TRUE;
	}

	return FALSE;
}

//WorM: ������� ��� ������ can_get_spell
//extern int slot_for_char(CHAR_DATA * ch, int slot_num);
//#define SpINFO spell_info[num]
// sets str to be the value of var.field
void find_replacement(void *go, SCRIPT_DATA * sc, TRIG_DATA * trig,
					  int type, char *var, char *field, char *subfield, char *str)
{
	struct trig_var_data *vd = NULL;
	CHAR_DATA *ch, *c = NULL, *rndm;
	OBJ_DATA *obj, *o = NULL;
	ROOM_DATA *room, *r = NULL;
	char *name;
	int num = 0, count = 0, value = 0, i;
	char uid_type = '\0';

	const char *send_cmd[] = { "msend", "osend", "wsend" };
	const char *echo_cmd[] = { "mecho", "oecho", "wecho" };
	const char *echoaround_cmd[] = { "mechoaround", "oechoaround", "wechoaround" };
	const char *door[] = { "mdoor", "odoor", "wdoor" };
	const char *force[] = { "mforce", "oforce", "wforce" };
	const char *load[] = { "mload", "oload", "wload" };
	const char *purge[] = { "mpurge", "opurge", "wpurge" };
	const char *teleport[] = { "mteleport", "oteleport", "wteleport" };
	const char *damage[] = { "mdamage", "odamage", "wdamage" };
	const char *featturn[] = { "mfeatturn", "ofeatturn", "wfeatturn" };
	const char *skillturn[] = { "mskillturn", "oskillturn", "wskillturn" };
	const char *skilladd[] = { "mskilladd", "oskilladd", "wskilladd" };
	const char *spellturn[] = { "mspellturn", "ospellturn", "wspellturn" };
	const char *spelladd[] = { "mspelladd", "ospelladd", "wspelladd" };
	const char *spellitem[] = { "mspellitem", "ospellitem", "wspellitem" };
	const char *portal[] = { "mportal", "oportal", "wportal" };

	if (!subfield)
	{
		subfield = 0/*'\0'*/;	// ����� �������� ������ ���� // prool: adaptation for osx/clang
	}

	// X.global() will have a NULL trig
	if (trig)
		vd = find_var_cntx(&GET_TRIG_VARS(trig), var, 0);
	if (!vd)
		vd = find_var_cntx(&(sc->global_vars), var, sc->context);
	if (!vd)
		vd = find_var_cntx(&worlds_vars, var, sc->context);

	*str = '\0';

	if (!field || !*field)
	{
		if (vd)
			strcpy(str, vd->value);
		else
		{
			if (!str_cmp(var, "self"))
			{
				long uid;
				// �������� �� UID
				switch (type)
				{
				case MOB_TRIGGER:
					uid = GET_ID((CHAR_DATA *) go);
					uid_type = UID_CHAR;
					break;
				case OBJ_TRIGGER:
					uid = GET_ID((OBJ_DATA *) go);
					uid_type = UID_OBJ;
					break;
				case WLD_TRIGGER:
					uid = find_room_uid(((ROOM_DATA *) go)->number);
					uid_type = UID_ROOM;
					break;
				default:
					strcpy(str, "self");
					return;
				}
				sprintf(str, "%c%ld", uid_type, uid);
			}
			else if (!str_cmp(var, "door"))
				strcpy(str, door[type]);
			else if (!str_cmp(var, "force"))
				strcpy(str, force[type]);
			else if (!str_cmp(var, "load"))
				strcpy(str, load[type]);
			else if (!str_cmp(var, "purge"))
				strcpy(str, purge[type]);
			else if (!str_cmp(var, "teleport"))
				strcpy(str, teleport[type]);
			else if (!str_cmp(var, "damage"))
				strcpy(str, damage[type]);
			else if (!str_cmp(var, "send"))
				strcpy(str, send_cmd[type]);
			else if (!str_cmp(var, "echo"))
				strcpy(str, echo_cmd[type]);
			else if (!str_cmp(var, "echoaround"))
				strcpy(str, echoaround_cmd[type]);
			else if (!str_cmp(var, "featturn"))
				strcpy(str, featturn[type]);
			else if (!str_cmp(var, "skillturn"))
				strcpy(str, skillturn[type]);
			else if (!str_cmp(var, "skilladd"))
				strcpy(str, skilladd[type]);
			else if (!str_cmp(var, "spellturn"))
				strcpy(str, spellturn[type]);
			else if (!str_cmp(var, "spelladd"))
				strcpy(str, spelladd[type]);
			else if (!str_cmp(var, "spellitem"))
				strcpy(str, spellitem[type]);
			else if (!str_cmp(var, "portal"))
				strcpy(str, portal[type]);
		}
		return;
	}

	if (vd)
	{
		name = vd->value;
		switch (type)
		{
		case MOB_TRIGGER:
			ch = (CHAR_DATA *) go;
			if (!name)
			{
				log("SYSERROR: null name (%s:%d %s)", __FILE__, __LINE__, __func__);
				break;
			}
			if (!ch)
			{
				log("SYSERROR: null ch (%s:%d %s)", __FILE__, __LINE__, __func__);
				break;
			}
			if ((o = get_object_in_equip(ch, name)));
			else if ((o = get_obj_in_list(name, ch->carrying)));
			else if ((c = get_char_room(name, IN_ROOM(ch))));
			else if ((o = get_obj_in_list(name, world[IN_ROOM(ch)]->contents)));
			else if ((c = get_char(name, GET_TRIG_VNUM(trig))));
			else if ((o = get_obj(name, GET_TRIG_VNUM(trig))));
			else if ((r = get_room(name)))
			{
			}
			break;
		case OBJ_TRIGGER:
			obj = (OBJ_DATA *) go;
			if ((c = get_char_by_obj(obj, name)));
			else if ((o = get_obj_by_obj(obj, name)));
			else if ((r = get_room(name)))
			{
			}
			break;
		case WLD_TRIGGER:
			room = (ROOM_DATA *) go;
			if ((c = get_char_by_room(room, name)));
			else if ((o = get_obj_by_room(room, name)));
			else if ((r = get_room(name)))
			{
			}
			break;
		}
	}
	else
	{
		if (!str_cmp(var, "self"))
		{
			switch (type)
			{
			case MOB_TRIGGER:
				c = (CHAR_DATA *) go;
				break;
			case OBJ_TRIGGER:
				o = (OBJ_DATA *) go;
				break;
			case WLD_TRIGGER:
				r = (ROOM_DATA *) go;
				break;
			}
		}
		else if (!str_cmp(var, "exist"))
		{
			if (!str_cmp(field, "mob") && (num = atoi(subfield)) > 0)
			{
				num = count_char_vnum(num);
				if (num >= 0)
					sprintf(str, "%c", num > 0 ? '1' : '0');
			}
			else if (!str_cmp(field, "obj") && (num = atoi(subfield)) > 0)
			{
				num = count_obj_vnum(num);
				if (num >= 0)
					sprintf(str, "%c", num > 0 ? '1' : '0');
			}
			return;
		}
		else if (!str_cmp(var, "world"))
		{
			num = atoi(subfield);
			if (!str_cmp(field, "curobjs") && num > 0)
			{
				num = count_obj_vnum(num);
				if (num >= 0)
					sprintf(str, "%d", num);
			}
			else if (!str_cmp(field, "gameobjs") && num > 0)
			{
				num = gcount_obj_vnum(num);
				if (num >= 0)
					sprintf(str, "%d", num);
			}
			else if (!str_cmp(field, "people") && num > 0)
			{
				sprintf(str, "%d", trgvar_in_room(num));
			}
			else if (!str_cmp(field, "curmobs") && num > 0)
			{
				num = count_char_vnum(num);
				if (num >= 0)
					sprintf(str, "%d", num);
			}
			else if (!str_cmp(field, "gamemobs") && num > 0)
			{
				num = gcount_char_vnum(num);
				if (num >= 0)
					sprintf(str, "%d", num);
			}
			else if (!str_cmp(field, "zreset") && num > 0)
			{
				int i;
				for (i = 0; i < top_of_zone_table; i++)
					if (zone_table[i].number == num)
						reset_zone(i);
			}
			else if (!str_cmp(field, "mob") && num > 0)
			{
				num = find_char_vnum(num);
				if (num >= 0)
					sprintf(str, "%c%d", UID_CHAR, num);
			}
			else if (!str_cmp(field, "obj") && num > 0)
			{
				num = find_obj_vnum(num);
				if (num >= 0)
					sprintf(str, "%c%d", UID_OBJ, num);
			}
			else if (!str_cmp(field, "room") && num > 0)
			{
				num = find_room_uid(num);
				if (num >= 0)
					sprintf(str, "%c%d", UID_ROOM, num);
			}
//Polud world.maxobj(vnum) ���������� ������������ ���������� ��������� � ����,
//������� ��������� � ����� �������� � ��������� vnum
			else if (!str_cmp(field, "maxobj") && num > 0)
			{
				num = real_object(num);
				if (num >= 0)
				{
					sprintf(str, "%d", GET_OBJ_MIW(obj_proto[num]));
				}
			}
//-Polud
			return;
		}
		else if (!str_cmp(var, "weather"))
		{
			if (!str_cmp(field, "temp"))
			{
				sprintf(str, "%d", weather_info.temperature);
			}
			else if (!str_cmp(field, "moon"))
			{
				sprintf(str, "%d", weather_info.moon_day);
			}
			else if (!str_cmp(field, "sky"))
			{
				num = -1;
				if ((num = atoi(subfield)) > 0)
					num = real_room(num);
				if (num != NOWHERE)
					sprintf(str, "%d", GET_ROOM_SKY(num));
				else
					sprintf(str, "%d", weather_info.sky);
			}
			else if (!str_cmp(field, "type"))
			{
				char c;
				int wt = weather_info.weather_type;
				num = -1;
				if ((num = atoi(subfield)) > 0)
					num = real_room(num);
				if (num != NOWHERE && world[num]->weather.duration > 0)
					wt = world[num]->weather.weather_type;
				for (c = 'a'; wt; ++c, wt >>= 1)
					if (wt & 1)
						sprintf(str + strlen(str), "%c", c);
			}
			else if (!str_cmp(field, "sunlight"))
			{
				switch (weather_info.sunlight)
				{
				case SUN_DARK:
					strcpy(str, "����");
					break;
				case SUN_SET:
					strcpy(str, "�����");
					break;
				case SUN_LIGHT:
					strcpy(str, "����");
					break;
				case SUN_RISE:
					strcpy(str, "�������");
					break;
				}

			}
			else if (!str_cmp(field, "season"))
			{
				switch (weather_info.season)
				{
				case SEASON_WINTER:
					strcat(str, "����");
					break;
				case SEASON_SPRING:
					strcat(str, "�����");
					break;
				case SEASON_SUMMER:
					strcat(str, "����");
					break;
				case SEASON_AUTUMN:
					strcat(str, "�����");
					break;
				}
			}
			return;
		}
		else if (!str_cmp(var, "time"))
		{
			if (!str_cmp(field, "hour"))
				sprintf(str, "%d", time_info.hours);
			else if (!str_cmp(field, "day"))
				sprintf(str, "%d", time_info.day + 1);
			else if (!str_cmp(field, "month"))
				sprintf(str, "%d", time_info.month + 1);
			else if (!str_cmp(field, "year"))
				sprintf(str, "%d", time_info.year);
			return;
		}
		else if (!str_cmp(var, "date"))
		{
			time_t now_time = time(0);
			if (!str_cmp(field, "unix"))
			{
				sprintf(str, "%ld", now_time);
			}
			else if (!str_cmp(field, "yday"))
			{
				strftime(str, MAX_INPUT_LENGTH, "%j", localtime(&now_time));
			}
			else if (!str_cmp(field, "wday"))
			{
				strftime(str, MAX_INPUT_LENGTH, "%w", localtime(&now_time));
			}
			else if (!str_cmp(field, "minute"))
			{
				strftime(str, MAX_INPUT_LENGTH, "%M", localtime(&now_time));
			}
			else if (!str_cmp(field, "hour"))
			{
				strftime(str, MAX_INPUT_LENGTH, "%H", localtime(&now_time));
			}
			else if (!str_cmp(field, "day"))
			{
				strftime(str, MAX_INPUT_LENGTH, "%d", localtime(&now_time));
			}
			else if (!str_cmp(field, "month"))
			{
				strftime(str, MAX_INPUT_LENGTH, "%m", localtime(&now_time));
			}
			else if (!str_cmp(field, "year"))
			{
				strftime(str, MAX_INPUT_LENGTH, "%y", localtime(&now_time));
			}
			return;
		}
		else if (!str_cmp(var, "random"))
		{
			if (!str_cmp(field, "char") || !str_cmp(field, "pc") || !str_cmp(field, "npc"))
			{
				rndm = NULL;
				count = 0;
				if (type == MOB_TRIGGER)
				{
					ch = (CHAR_DATA *) go;
					for (c = world[IN_ROOM(ch)]->people; c; c = c->next_in_room)
						if (!PRF_FLAGGED(c, PRF_NOHASSLE) && (c != ch)
								&& CAN_SEE(ch, c)
								&& ((IS_NPC(c) && *field != 'p')
									|| (!IS_NPC(c) && *field != 'n')))
						{
							if (!number(0, count))
								rndm = c;
							count++;
						}
				}
				else if (type == OBJ_TRIGGER)
				{
					for (c = world[obj_room((OBJ_DATA *) go)]->people; c;
							c = c->next_in_room)
						if ((IS_NPC(c) && *field != 'p') ||
								(!IS_NPC(c) && *field != 'n' &&
								 !PRF_FLAGGED(c, PRF_NOHASSLE)
								 && !GET_INVIS_LEV(c)))
						{
							if (!number(0, count))
								rndm = c;
							count++;
						}
				}
				else if (type == WLD_TRIGGER)
				{
					for (c = ((ROOM_DATA *) go)->people; c; c = c->next_in_room)
						if ((IS_NPC(c) && *field != 'p') ||
								(!IS_NPC(c) && *field != 'n' &&
								 !PRF_FLAGGED(c, PRF_NOHASSLE)
								 && !GET_INVIS_LEV(c)))
						{
							if (!number(0, count))
								rndm = c;
							count++;
						}
				}
				if (rndm)
					sprintf(str, "%c%ld", UID_CHAR, GET_ID(rndm));
			}
			else
			{
				if (!str_cmp(field, "num"))
					num = atoi(subfield);
				else
					num = atoi(field);
				sprintf(str, "%d", (num > 0) ? number(1, num) : 0);
			}
			return;
		}
	}

	if (c)
	{
		if (text_processed(field, subfield, vd, str))
			return;
		else if (!str_cmp(field, "global"))  	// get global of something else
		{
			if (IS_NPC(c) && c->script)
			{
				find_replacement(go, c->script, NULL, MOB_TRIGGER, subfield, NULL, NULL, str);
			}
		}
		else if (!str_cmp(field, "iname"))
			strcpy(str, GET_PAD(c, 0));
		else if (!str_cmp(field, "rname"))
			strcpy(str, GET_PAD(c, 1));
		else if (!str_cmp(field, "dname"))
			strcpy(str, GET_PAD(c, 2));
		else if (!str_cmp(field, "vname"))
			strcpy(str, GET_PAD(c, 3));
		else if (!str_cmp(field, "tname"))
			strcpy(str, GET_PAD(c, 4));
		else if (!str_cmp(field, "pname"))
			strcpy(str, GET_PAD(c, 5));
		else if (!str_cmp(field, "name"))
			strcpy(str, GET_NAME(c));
		else if (!str_cmp(field, "id"))
			sprintf(str, "%c%ld", UID_CHAR, GET_ID(c));
		else if (!str_cmp(field, "uniq"))
		{
			if (!IS_NPC(c))
				sprintf(str, "%d", GET_UNIQUE(c));
		}
		else if (!str_cmp(field, "alias"))
			strcpy(str, GET_PC_NAME(c));
		else if (!str_cmp(field, "level"))
			sprintf(str, "%d", GET_LEVEL(c));
		else if (!str_cmp(field, "remort"))
		{
			if (!IS_NPC(c))
				sprintf(str, "%d", GET_REMORT(c));
		}
		else if (!str_cmp(field, "hitp"))
		{
			GET_HIT(c) = (sh_int) MAX(1, gm_char_field(c, field, subfield, (long) GET_HIT(c)));
			sprintf(str, "%d", GET_HIT(c));
		}
		else if (!str_cmp(field, "arenahp"))
		{
			CHAR_DATA *k;
			struct follow_type *f;
			int arena_hp = GET_HIT(c);
			int can_use = 0;

			if(!IS_NPC(c))
			{
				k = (c->master ? c->master : c);
				if (GET_CLASS(c) == 8)//������ ����� ���������
				{
					can_use = 2;
				}
				else if (GET_CLASS(c) == 0 || GET_CLASS(c) == 13)//���� ��� ����� ����� ������������ ���������������
				{
					can_use = 1;
				}
				else if (AFF_FLAGGED(k, AFF_GROUP))
				{
					if (!IS_NPC(k) && (GET_CLASS(k) == 8 || GET_CLASS(k) == 13) //������ ��� ����� ����� ������������ ��� �� ���������
						&& world[IN_ROOM(k)]->zone == world[IN_ROOM(c)]->zone) //�� ������ ���� ��������� � ��� �� ����
					{
						can_use = 1;
					}
					if (!can_use)
					{
						for (f = k->followers; f; f = f->next)
						{
							if (IS_NPC(f->follower)
								|| !AFF_FLAGGED(f->follower, AFF_GROUP))
							{
								continue;
							}
							if ((GET_CLASS(f->follower) == 8
									|| GET_CLASS(f->follower) == 13) //������ ��� ����� ����� ������������ ��� �� ���������
								&& world[IN_ROOM(f->follower)]->zone
									== world[IN_ROOM(c)]->zone) //�� ������ ���� ��������� � ��� �� ����
							{
								can_use = 1;
								break;
							}
						}
					}
				}
				if (can_use == 2)//����
				{
					arena_hp = GET_REAL_MAX_HIT(c) + GET_REAL_MAX_HIT(c) * GET_LEVEL(c) / 10;
				}
				else if (can_use == 1)//��� � ������
				{
					arena_hp = GET_REAL_MAX_HIT(c) + GET_REAL_MAX_HIT(c) * 33 / 100;
				}
				else
				{
					arena_hp = GET_REAL_MAX_HIT(c);
				}
			}
			sprintf(str, "%d", arena_hp);
		}
		else if (!str_cmp(field, "hitpadd"))
		{
			GET_HIT_ADD(c) = (int) gm_char_field(c, field, subfield, (long) GET_HIT_ADD(c));
			sprintf(str, "%d", GET_HIT_ADD(c));
		}
		else if (!str_cmp(field, "maxhitp"))
		{
			// if (!IS_NPC(c))
			//   GET_MAX_HIT(c) = (sh_int) MAX(1,gm_char_field(c,field,subfield,(long)GET_MAX_HIT(c)));
			sprintf(str, "%d", GET_MAX_HIT(c));
		}
		else if (!str_cmp(field, "hitpreg"))
		{
			GET_HITREG(c) = (int) gm_char_field(c, field, subfield, (long) GET_HITREG(c));
			sprintf(str, "%d", GET_HITREG(c));
		}
		else if (!str_cmp(field, "mana"))
		{
			if (!IS_NPC(c))
				GET_MANA_STORED(c) =
					MAX(0, gm_char_field(c, field, subfield, (long) GET_MANA_STORED(c)));
			sprintf(str, "%d", GET_MANA_STORED(c));
		}
		else if (!str_cmp(field, "manareg"))
		{
			GET_MANAREG(c) = (int) gm_char_field(c, field, subfield, (long) GET_MANAREG(c));
			sprintf(str, "%d", GET_MANAREG(c));
		}
		else if (!str_cmp(field, "maxmana"))
			sprintf(str, "%d", GET_MAX_MANA(c));
		else if (!str_cmp(field, "move"))
		{
			if (!IS_NPC(c))
				GET_MOVE(c) =
					(sh_int) MAX(0, gm_char_field(c, field, subfield, (long) GET_MOVE(c)));
			sprintf(str, "%d", GET_MOVE(c));
		}
		else if (!str_cmp(field, "maxmove"))
		{
			//GET_MAX_MOVE(c) = (sh_int) MAX(1,gm_char_field(c,field,subfield,(long)GET_MAX_MOVE(c)));
			sprintf(str, "%d", GET_MAX_MOVE(c));
		}
		else if (!str_cmp(field, "moveadd"))
		{
			GET_MOVE_ADD(c) = (int) gm_char_field(c, field, subfield, (long) GET_MOVE_ADD(c));
			sprintf(str, "%d", GET_MOVE_ADD(c));
		}
		else if (!str_cmp(field, "movereg"))
		{
			GET_MOVEREG(c) = (int) gm_char_field(c, field, subfield, (long) GET_MOVEREG(c));
			sprintf(str, "%d", GET_MOVEREG(c));
		}
		else if (!str_cmp(field, "castsucc"))
		{
			GET_CAST_SUCCESS(c) =
				(int) gm_char_field(c, field, subfield, (long) GET_CAST_SUCCESS(c));
			sprintf(str, "%d", GET_CAST_SUCCESS(c));
		}
		else if (!str_cmp(field, "ageadd"))
		{
			if (!IS_NPC(c))
			{
				GET_AGE_ADD(c) = (int) gm_char_field(c, field, subfield, (long) GET_AGE_ADD(c));
				sprintf(str, "%d", GET_AGE_ADD(c));
			}
		}
		else if (!str_cmp(field, "age"))
		{
			if (!IS_NPC(c))
				sprintf(str, "%d", GET_REAL_AGE(c));
		}
		else if (!str_cmp(field, "hrbase"))
		{
			//GET_HR(c) = (int) gm_char_field(c,field,subfield,(long)GET_HR(c));
			sprintf(str, "%d", GET_HR(c));
		}
		else if (!str_cmp(field, "hradd"))
		{
			GET_HR_ADD(c) = (int) gm_char_field(c, field, subfield, (long) GET_HR(c));
			sprintf(str, "%d", GET_HR_ADD(c));
		}
		else if (!str_cmp(field, "hr"))
		{
			sprintf(str, "%d", GET_REAL_HR(c));
		}
		else if (!str_cmp(field, "drbase"))
		{
			//GET_DR(c) = (int) gm_char_field(c,field,subfield,(long)GET_DR(c));
			sprintf(str, "%d", GET_DR(c));
		}
		else if (!str_cmp(field, "dradd"))
		{
			GET_DR_ADD(c) = (int) gm_char_field(c, field, subfield, (long) GET_DR(c));
			sprintf(str, "%d", GET_DR_ADD(c));
		}
		else if (!str_cmp(field, "dr"))
		{
			sprintf(str, "%d", GET_REAL_DR(c));
		}
		else if (!str_cmp(field, "acbase"))
		{
			//GET_AC(c) = (int) gm_char_field(c,field,subfield,(long)GET_AC(c));
			sprintf(str, "%d", GET_AC(c));
		}
		else if (!str_cmp(field, "acadd"))
		{
			GET_AC_ADD(c) = (int) gm_char_field(c, field, subfield, (long) GET_AC(c));
			sprintf(str, "%d", GET_AC_ADD(c));
		}
		else if (!str_cmp(field, "ac"))
		{
			sprintf(str, "%d", GET_REAL_AC(c));
		}
		else if (!str_cmp(field, "morale")) // ����� ����� ������
		{
			//GET_MORALE(c) = (int) gm_char_field(c, field, subfield, (long) GET_MORALE(c));
			sprintf(str, "%d", c->calc_morale());
		}
		else if (!str_cmp(field, "moraleadd")) // ���������� ������
		{
			GET_MORALE(c) = (int) gm_char_field(c, field, subfield, (long) GET_MORALE(c));
			sprintf(str, "%d", GET_MORALE(c));
		}
		else if (!str_cmp(field, "poison"))
		{
			GET_POISON(c) = (int) gm_char_field(c, field, subfield, (long) GET_POISON(c));
			sprintf(str, "%d", GET_POISON(c));
		}
		else if (!str_cmp(field, "initiative"))
		{
			GET_INITIATIVE(c) = (int) gm_char_field(c, field, subfield, (long) GET_INITIATIVE(c));
			sprintf(str, "%d", GET_INITIATIVE(c));
		}
		else if (!str_cmp(field, "align"))
		{
			if (*subfield)
			{
				if (*subfield == '-')
					GET_ALIGNMENT(c) -= MAX(1, atoi(subfield + 1));
				else if (*subfield == '+')
					GET_ALIGNMENT(c) += MAX(1, atoi(subfield + 1));
			}
			sprintf(str, "%d", GET_ALIGNMENT(c));
		}
		else if (!str_cmp(field, "religion"))
		{
			if (*subfield && ((atoi(subfield) == RELIGION_POLY) || (atoi(subfield) == RELIGION_MONO)))
				GET_RELIGION(c) = atoi(subfield);
			else
				sprintf(str, "%d", GET_RELIGION(c));
		}
		else if (!str_cmp(field, "restore"))
		{
			do_restore(c, (char*)c->get_name(), 0, SCMD_RESTORE_TRIGGER);
			trig_log(trig, "��� ���������� ����� do_restore!");
		}
		else if (!str_cmp(field, "dispel"))
		{
			if (c->affected)
				send_to_char("�� ������ ������ ��������!\r\n", c);
			while (c->affected)
				affect_remove(c, c->affected);
		}
		else if (!str_cmp(field, "gold"))
		{
			const long before = c->get_gold();
			c->set_gold(MAX(0, gm_char_field(c, field, subfield, c->get_gold())));
			sprintf(str, "%ld", c->get_gold());
			// ����-�����
			const long diff = c->get_gold() - before;
			split_or_clan_tax(c, diff);
			// ����� ��� show money
			if (!IS_NPC(c) && IN_ROOM(c) > 0)
			{
				MoneyDropStat::add(
					zone_table[world[IN_ROOM(c)]->zone].number, diff);
			}
		}
		else if (!str_cmp(field, "bank"))
		{
			const long before = c->get_bank();
			c->set_bank(MAX(0, gm_char_field(c, field, subfield, c->get_bank())));
			sprintf(str, "%ld", c->get_bank());
			// ����-�����
			const long diff = c->get_bank() - before;
			split_or_clan_tax(c, diff);
			// ����� ��� show money
			if (!IS_NPC(c) && IN_ROOM(c) > 0)
			{
				MoneyDropStat::add(
					zone_table[world[IN_ROOM(c)]->zone].number, diff);
			}
		}
		else if (!str_cmp(field, "exp"))
		{
			if (*subfield)
			{
				if (*subfield == '-')
//                 GET_EXP(c) = MAX(1, GET_EXP(c) - atoi(subfield+1));
					gain_exp(c, -MAX(1, atoi(subfield + 1)));
				else if (*subfield == '+')
//                   GET_EXP(c) = MAX(1, GET_EXP(c) + atoi(subfield+1));
					gain_exp(c, + MAX(1, atoi(subfield + 1)));
				else if ((value = atoi(subfield)) > 0)
				{
					c->set_exp(value);
				}
			}
			sprintf(str, "%ld", GET_EXP(c));
		}
		else if (!str_cmp(field, "sex"))
			sprintf(str, "%d", (int) GET_SEX(c));
		else if (!str_cmp(field, "clan"))
		{
			if (CLAN(c))
			{
				sprintf(str, "%s", CLAN(c)->GetAbbrev());
				for (i = 0; str[i]; i++)
					str[i] = LOWER(str[i]);
			}
			else
				sprintf(str, "null");
		}
		else if (!str_cmp(field, "clanrank"))
		{
			if (CLAN(c) && CLAN_MEMBER(c))
				sprintf(str, "%d", CLAN_MEMBER(c)->rank_num);
			else
				sprintf(str, "null");
		}
		else if (!str_cmp(field, "m"))
			strcpy(str, HMHR(c));
		else if (!str_cmp(field, "s"))
			strcpy(str, HSHR(c));
		else if (!str_cmp(field, "e"))
			strcpy(str, HSSH(c));
		else if (!str_cmp(field, "g"))
			strcpy(str, GET_CH_SUF_1(c));
		else if (!str_cmp(field, "u"))
			strcpy(str, GET_CH_SUF_2(c));
		else if (!str_cmp(field, "w"))
			strcpy(str, GET_CH_SUF_3(c));
		else if (!str_cmp(field, "q"))
			strcpy(str, GET_CH_SUF_4(c));
		else if (!str_cmp(field, "y"))
			strcpy(str, GET_CH_SUF_5(c));
		else if (!str_cmp(field, "a"))
			strcpy(str, GET_CH_SUF_6(c));
		else if (!str_cmp(field, "r"))
			strcpy(str, GET_CH_SUF_7(c));
		else if (!str_cmp(field, "x"))
			strcpy(str, GET_CH_SUF_8(c));
		else if (!str_cmp(field, "weight"))
			sprintf(str, "%d", GET_WEIGHT(c));
		else if (!str_cmp(field, "canbeseen"))
		{
			if ((type == MOB_TRIGGER) && !CAN_SEE(((CHAR_DATA *) go), c))
				strcpy(str, "0");
			else
				strcpy(str, "1");
		}
		else if (!str_cmp(field, "class"))
			sprintf(str, "%d", (int) GET_CLASS(c));

#ifdef GET_RACE
		else if (!str_cmp(field, "race"))
			sprintf(str, "%d", (int) GET_RACE(c));
#endif

		else if (!str_cmp(field, "fighting"))
		{
			if (c->get_fighting())
				sprintf(str, "%c%ld", UID_CHAR, GET_ID(c->get_fighting()));
		}
		else if (!str_cmp(field, "is_killer"))
		{
			if (PLR_FLAGGED(c, PLR_KILLER))
				strcpy(str, "1");
			else
				strcpy(str, "0");
		}
		else if (!str_cmp(field, "is_thief"))
		{
			if (PLR_FLAGGED(c, PLR_THIEF))
				strcpy(str, "1");
			else
				strcpy(str, "0");
		}
		else if (!str_cmp(field, "rentable"))
		{
			if (!IS_NPC(c) && RENTABLE(c))
				strcpy(str, "0");
			else
				strcpy(str, "1");
		}
		else if (!str_cmp(field, "can_get_skill"))
		{
			if ((num = find_skill_num(subfield)) > 0)
			{
				if (can_get_skill(c, num))
					strcpy(str, "1");
				else
					strcpy(str, "0");
			}
			else
			{
				sprintf(buf, "wrong skill name '%s'!", subfield);
				trig_log(trig, buf);
				strcpy(str, "0");
			}
		}
		else if (!str_cmp(field, "can_get_spell"))
		{
			if ((num = find_spell_num(subfield)) > 0)
			{
				if (can_get_spell(c, num))
					strcpy(str, "1");
				else
					strcpy(str, "0");
			}
			else
			{
				sprintf(buf, "wrong spell name '%s'!", subfield);
				trig_log(trig, buf);
				strcpy(str, "0");
			}
		}
		else if (!str_cmp(field, "can_get_feat"))
		{
			if ((num = find_feat_num(subfield)) > 0)
			{
				if (can_get_feat(c, num))
					strcpy(str, "1");
				else
					strcpy(str, "0");
			}
			else
			{
				sprintf(buf, "wrong feature name '%s'!", subfield);
				trig_log(trig, buf);
				strcpy(str, "0");
			}
		}
		else if (!str_cmp(field, "agressor"))
		{
			if (AGRESSOR(c))
				sprintf(str, "%d", AGRESSOR(c));
			else
				strcpy(str, "0");
		}
#ifdef RIDING
		else if (!str_cmp(field, "riding"))
		{
			if (RIDING(c))
				sprintf(str, "%c%ld", UID_CHAR, GET_ID(RIDING(c)));
		}
#endif

#ifdef RIDDEN_BY
		else if (!str_cmp(field, "ridden_by"))
		{
			if (RIDDEN_BY(c))
				sprintf(str, "%c%ld", UID_CHAR, GET_ID(RIDDEN_BY(c)));
		}
#endif

		else if (!str_cmp(field, "vnum"))
			sprintf(str, "%d", GET_MOB_VNUM(c));
		else if (!str_cmp(field, "str"))
		{
			//GET_STR(c)=(sbyte) MAX(1,gm_char_field(c,field,subfield,(long) GET_STR(c)));
			sprintf(str, "%d", c->get_str());
		}
		else if (!str_cmp(field, "stradd"))
			sprintf(str, "%d", GET_STR_ADD(c));
		else if (!str_cmp(field, "int"))
		{
			//GET_INT(c)=(sbyte) MAX(1,gm_char_field(c,field,subfield,(long) GET_INT(c)));
			sprintf(str, "%d", c->get_int());
		}
		else if (!str_cmp(field, "intadd"))
			sprintf(str, "%d", GET_INT_ADD(c));
		else if (!str_cmp(field, "wis"))
		{
			//GET_WIS(c)=(sbyte) MAX(1,gm_char_field(c,field,subfield,(long) GET_WIS(c)));
			sprintf(str, "%d", c->get_wis());
		}
		else if (!str_cmp(field, "wisadd"))
			sprintf(str, "%d", GET_WIS_ADD(c));
		else if (!str_cmp(field, "dex"))
		{
			//GET_DEX(c)=(sbyte) MAX(1,gm_char_field(c,field,subfield,(long) GET_DEX(c)));
			sprintf(str, "%d", c->get_dex());
		}
		else if (!str_cmp(field, "dexadd"))
			sprintf(str, "%d", GET_DEX_ADD(c));
		else if (!str_cmp(field, "con"))
		{
			//GET_CON(c)=(sbyte) MAX(1,gm_char_field(c,field,subfield,(long) GET_CON(c)));
			sprintf(str, "%d", c->get_con());
		}
		else if (!str_cmp(field, "conadd"))
		{
			sprintf(str, "%d", GET_CON_ADD(c));
		}
		else if (!str_cmp(field, "cha"))
		{
			//GET_CHA(c)=(sbyte) MAX(1,gm_char_field(c,field,subfield,(long) GET_CHA(c)));
			sprintf(str, "%d", c->get_cha());
		}
		else if (!str_cmp(field, "chaadd"))
			sprintf(str, "%d", GET_CHA_ADD(c));
		else if (!str_cmp(field, "size"))
		{
			//GET_SIZE(c)=(sbyte) MAX(1,gm_char_field(c,field,subfield,(long) GET_SIZE(c)));
			sprintf(str, "%d", GET_SIZE(c));
		}
		else if (!str_cmp(field, "sizeadd"))
		{
			GET_SIZE_ADD(c) =
				(sbyte) MAX(1,
							gm_char_field(c, field, subfield,
										  (long) GET_SIZE_ADD(c)));
			sprintf(str, "%d", GET_SIZE_ADD(c));
		}
		else if (!str_cmp(field, "room"))
		{
			int n = find_room_uid(world[IN_ROOM(c)]->number);
			if (n >= 0)
				sprintf(str, "%c%d", UID_ROOM, n);
		}
		else if (!str_cmp(field, "realroom"))
			sprintf(str, "%d", world[IN_ROOM(c)]->number);
		else if (!str_cmp(field, "loadroom"))
		{
			int pos;
			if (!IS_NPC(c))
			{
				if (!*subfield || !(pos = atoi(subfield)))
					sprintf(str, "%d", GET_LOADROOM(c));
				else
				{
					GET_LOADROOM(c) = pos;
					c->save_char();
					sprintf(str, "%d", real_room(pos)); // TODO: ������ ����� ��� ����?
				}
			}
		}
		else if (!str_cmp(field, "skill"))
			strcpy(str, skill_percent(c, subfield));
		else if (!str_cmp(field, "feat"))
		{
			if (feat_owner(c, subfield))
				strcpy(str, "1");
			else
				strcpy(str, "0");
		}
		else if (!str_cmp(field, "spellcount"))
			strcpy(str, spell_count(c, subfield));
		else if (!str_cmp(field, "spelltype"))
			strcpy(str, spell_knowledge(c, subfield));
		else if (!str_cmp(field, "quested"))
		{
			if (*subfield && (num = atoi(subfield)) > 0)
			{
				if (c->quested_get(num))
					strcpy(str, "1");
				else
					strcpy(str, "0");
			}
		}
		else if (!str_cmp(field, "getquest"))
		{
			if (*subfield && (num = atoi(subfield)) > 0)
			{
				strcpy(str, (c->quested_get_text(num)).c_str());
			}
		}
		else if (!str_cmp(field, "setquest"))
		{
			if (*subfield)
			{
				subfield = one_argument(subfield, buf);
				skip_spaces(&subfield);
				if ((num = atoi(buf)) > 0)
				{
					c->quested_add(c, num, subfield);
					strcpy(str, "1");
				}
			}
		}
		else if (!str_cmp(field, "unsetquest"))
		{
			if (*subfield && (num = atoi(subfield)) > 0)
			{
				c->quested_remove(num);
				strcpy(str, "1");
			}
		}
		else if (!str_cmp(field, "eq"))
		{
			int pos = -1;
			if (isdigit(*subfield))
				pos = atoi(subfield);
			else if (*subfield)
				pos = find_eq_pos(c, NULL, subfield);
			if (!*subfield || pos < 0 || pos > NUM_WEARS)
				strcpy(str, "");
			else
			{
				if (!GET_EQ(c, pos))
					strcpy(str, "");
				else
					sprintf(str, "%c%ld", UID_OBJ, GET_ID(GET_EQ(c, pos)));
			}
		}
		else if (!str_cmp(field, "haveobj"))
		{
			int pos;
			if (isdigit(*subfield))
			{
				pos = atoi(subfield);
				for (obj = c->carrying; obj; obj = obj->next_content)
					if (GET_OBJ_VNUM(obj) == pos)
						break;
			}
			else
			{
				obj = get_obj_in_list_vis(c, subfield, c->carrying);
			}
			if (obj)
				sprintf(str, "%c%ld", UID_OBJ, GET_ID(obj));
			else
				strcpy(str, "0");
		}
		else if (!str_cmp(field, "varexists"))
		{
//           struct trig_var_data *vd;
			strcpy(str, "0");
			if (SCRIPT(c))
			{
				if (find_var_cntx
						(&((SCRIPT(c))->global_vars), subfield, sc->context))
					strcpy(str, "1");
			}
		}
		else if (!str_cmp(field, "next_in_room"))
		{
			if (c->next_in_room)
				sprintf(str, "%c%ld", UID_CHAR, GET_ID(c->next_in_room));
			else
				strcpy(str, "");
		}
		else if (!str_cmp(field, "position"))
		{
			int pos;

			if (!*subfield || (pos = atoi(subfield)) <= POS_DEAD)
				sprintf(str, "%d", GET_POS(c));
			else if (!WAITLESS(c))
				GET_POS(c) = pos;
		}
		else if (!str_cmp(field, "wait"))
		{
			int pos;

			if (!*subfield || (pos = atoi(subfield)) <= 0)
				sprintf(str, "%d", GET_WAIT(c));
			else if (!WAITLESS(c))
				WAIT_STATE(c, pos * PULSE_VIOLENCE);
		}
		else if (!str_cmp(field, "affect"))
		{
			gm_flag(subfield, affected_bits,
					c->char_specials.saved.affected_by, str);
		}
		//added by WorM
		//���������� ���������� ��� ����� �� �������� ���� �� ������ ������ ����� ����������� ����� affect
		//������ �����-�� ��������� ������� ����������,����� � �.�.
		//� ���� �� ��� � ��� ������ �� ��� ����� �������� ����� ��� �� ������������
		else if (!str_cmp(field, "affected_by"))
		{
			if ((num = find_spell_num(subfield)) > 0)
			{
				sprintf(str, "%d", (int)affected_by_spell(c, num));
			}
		}
		else if (!str_cmp(field, "action"))
		{
			if (IS_NPC(c))
				gm_flag(subfield, action_bits, c->char_specials.saved.act, str);
		}
		/*else if (!str_cmp(field, "function"))
		{
			if (IS_NPC(c))
				gm_flag(subfield, function_bits,
						c->mob_specials.npc_flags, str);
		}
		>> fatal error C1061: ����������� �����������: ������������ ������� �������� ������
		����� ����������� � ������ � 128 elseif-��,
		��������� ����� �� ���������������� function ���� ��������
		������ � ������ ������ dispel, ���� ��� ������ ��� �������
		��-��������� - ���� �������� ��������� ���� ���� 128 �������
		(�� ���� ���������) ����� �� ���� (������)
		*/
		else if (!str_cmp(field, "leader"))
		{
			if (c->master)
				sprintf(str, "%c%ld", UID_CHAR, GET_ID(c->master));
		}
		else if (!str_cmp(field, "group"))
		{
			CHAR_DATA *l;
			struct follow_type *f;
			if (!AFF_FLAGGED(c, AFF_GROUP))
				return;
			l = c->master;
			if (!l)
				l = c;
			// l - ����� ������
			sprintf(str + strlen(str), "%c%ld ", UID_CHAR, GET_ID(l));
			for (f = l->followers; f; f = f->next)
			{
				if (!AFF_FLAGGED(f->follower, AFF_GROUP))
					continue;
				sprintf(str + strlen(str), "%c%ld ", UID_CHAR,
						GET_ID(f->follower));
			}
		}
		else if (!str_cmp(field, "attackers"))
		{
			CHAR_DATA *t;
			for (t = combat_list; t; t = t->next_fighting)
			{
				if (t->get_fighting() != c)
					continue;
				sprintf(str + strlen(str), "%c%ld ", UID_CHAR, GET_ID(t));
			}
		}
		else if (!str_cmp(field, "people"))
		{
			if (world[IN_ROOM(c)]->people)
			{
				sprintf(str, "%c%ld", UID_CHAR,
						GET_ID(world[IN_ROOM(c)]->people));
			}
			else
				strcpy(str, "");
		}
//Polud ��������� ���� objs � ����, ���������� ������ �� ������� UID ��������� � ���������
		else if (!str_cmp(field, "objs"))
		{
			for (obj = c->carrying; obj; obj = obj->next_content)
				sprintf(str + strlen(str), "%c%ld ", UID_OBJ, GET_ID(obj));
		}
//-Polud
		else if (!str_cmp(field, "char") ||
				 !str_cmp(field, "pc") ||
				 !str_cmp(field, "npc") || !str_cmp(field, "all"))
		{
			int inroom;

			// ����������� ������ (��� mob)
			inroom = IN_ROOM(c);
			if (inroom == NOWHERE)
			{
				trig_log(trig, "mob-����������� ������ � NOWHERE");
				return;
			}

			for (rndm = world[inroom]->people; rndm; rndm = rndm->next_in_room)
			{
				if (!CAN_SEE(c, rndm))
					continue;
				if ((*field == 'a') ||
						(!IS_NPC(rndm) && *field != 'n') ||
						(IS_NPC(rndm) && IS_CHARMED(rndm)
						 && *field == 'c') || (IS_NPC(rndm)
											   && !IS_CHARMED(rndm)
											   && *field == 'n'))
					sprintf(str + strlen(str), "%c%ld ", UID_CHAR,
							GET_ID(rndm));
			}

			return;
		}
		else if (!str_cmp(field, "is_noob"))
		{
			strcpy(str, Noob::is_noob(c) ? "1" : "0");
		}
		else if (!str_cmp(field, "noob_outfit"))
		{
			std::string vnum_str = Noob::print_start_outfit(c);
			snprintf(str, MAX_INPUT_LENGTH, "%s", vnum_str.c_str());
		}
		else
		{
			if (SCRIPT(c))
			{
				vd = find_var_cntx(&((SCRIPT(c))->global_vars), field,
								   sc->context);
				if (vd)
					sprintf(str, "%s", vd->value);
				else
				{
					sprintf(buf2, "unknown char field: '%s'", field);
					trig_log(trig, buf2);
				}
			}
			else
			{
				sprintf(buf2, "unknown char field: '%s'", field);
				trig_log(trig, buf2);
			}
		}
	}
	else if (o)
	{
		if (text_processed(field, subfield, vd, str))
			return;
		else if (!str_cmp(field, "iname"))
			if (o->PNames[0])
				strcpy(str, o->PNames[0]);
			else
				strcpy(str, o->aliases);
		else if (!str_cmp(field, "rname"))
			if (o->PNames[1])
				strcpy(str, o->PNames[1]);
			else
				strcpy(str, o->aliases);
		else if (!str_cmp(field, "dname"))
			if (o->PNames[2])
				strcpy(str, o->PNames[2]);
			else
				strcpy(str, o->aliases);
		else if (!str_cmp(field, "vname"))
			if (o->PNames[3])
				strcpy(str, o->PNames[3]);
			else
				strcpy(str, o->aliases);
		else if (!str_cmp(field, "tname"))
			if (o->PNames[4])
				strcpy(str, o->PNames[4]);
			else
				strcpy(str, o->aliases);
		else if (!str_cmp(field, "pname"))
			if (o->PNames[5])
				strcpy(str, o->PNames[5]);
			else
				strcpy(str, o->aliases);
		else if (!str_cmp(field, "name"))
			strcpy(str, o->aliases);
		else if (!str_cmp(field, "id"))
			sprintf(str, "%c%ld", UID_OBJ, GET_ID(o));
		else if (!str_cmp(field, "uid"))
		{
			if (!GET_OBJ_UID(o))
			{
				set_uid(o);
			}
			sprintf(str, "%u", GET_OBJ_UID(o));
		}
		else if (!str_cmp(field, "skill"))
			sprintf(str, "%d", GET_OBJ_SKILL(o));
		else if (!str_cmp(field, "shortdesc"))
			strcpy(str, o->short_description);
		else if (!str_cmp(field, "vnum"))
			sprintf(str, "%d", GET_OBJ_VNUM(o));
		else if (!str_cmp(field, "type"))
			sprintf(str, "%d", (int) GET_OBJ_TYPE(o));
		else if (!str_cmp(field, "timer"))
			sprintf(str, "%d", o->get_timer());
		else if (!str_cmp(field, "val0"))
		{
			if (*subfield)
			{
				skip_spaces(&subfield);
				GET_OBJ_VAL(o, 0) = atoi(subfield);
			}
			else
			{
				sprintf(str, "%d", GET_OBJ_VAL(o, 0));
			}
		}
		else if (!str_cmp(field, "val1"))
		{
			if (*subfield)
			{
				skip_spaces(&subfield);
				GET_OBJ_VAL(o, 1) = atoi(subfield);
			}
			else
			{
				sprintf(str, "%d", GET_OBJ_VAL(o, 1));
			}
		}
		else if (!str_cmp(field, "val2"))
		{
			if (*subfield)
			{
				skip_spaces(&subfield);
				GET_OBJ_VAL(o, 2) = atoi(subfield);
			}
			else
			{
				sprintf(str, "%d", GET_OBJ_VAL(o, 2));
			}
		}
		else if (!str_cmp(field, "val3"))
		{
			if (*subfield)
			{
				skip_spaces(&subfield);
				GET_OBJ_VAL(o, 3) = atoi(subfield);
			}
			else
			{
				sprintf(str, "%d", GET_OBJ_VAL(o, 3));
			}
		}
		else if (!str_cmp(field, "maker"))
			sprintf(str, "%d", GET_OBJ_MAKER(o));
		else if (!str_cmp(field, "effect"))
			gm_flag(subfield, extra_bits, (o)->obj_flags.extra_flags, str);
		else if (!str_cmp(field, "affect"))
			gm_flag(subfield, weapon_affects, (o)->obj_flags.affects, str);
		else if (!str_cmp(field, "carried_by"))
			if (o->carried_by)
				sprintf(str, "%c%ld", UID_CHAR, GET_ID(o->carried_by));
			else
				strcpy(str, "");
		else if (!str_cmp(field, "worn_by"))
			if (o->worn_by)
				sprintf(str, "%c%ld", UID_CHAR, GET_ID(o->worn_by));
			else
				strcpy(str, "");
		else if (!str_cmp(field, "g"))
			strcpy(str, GET_OBJ_SUF_1(o));
		else if (!str_cmp(field, "q"))
			strcpy(str, GET_OBJ_SUF_4(o));
		else if (!str_cmp(field, "u"))
			strcpy(str, GET_OBJ_SUF_2(o));
		else if (!str_cmp(field, "w"))
			strcpy(str, GET_OBJ_SUF_3(o));
		else if (!str_cmp(field, "y"))
			strcpy(str, GET_OBJ_SUF_5(o));
		else if (!str_cmp(field, "a"))
			strcpy(str, GET_OBJ_SUF_6(o));
		else if (!str_cmp(field, "count"))
			strcpy(str, get_objs_in_world(o));
		else if (!str_cmp(field, "sex"))
			sprintf(str, "%d", (int) GET_OBJ_SEX(o));
		else if (!str_cmp(field, "room"))
			if (o->carried_by)
				sprintf(str, "%d", world[IN_ROOM(o->carried_by)]->number);
			else if (o->worn_by)
				sprintf(str, "%d", world[IN_ROOM(o->worn_by)]->number);
			else if (o->in_room != NOWHERE)
				sprintf(str, "%d", world[o->in_room]->number);
			else
				strcpy(str, "");
//Polud ��������� %obj.put(UID)% - �������� ��������� ������ � ���������, ������� ��� ��������� ����, � ����������� �� UID�
		else if (!str_cmp(field, "put"))
		{
			OBJ_DATA *obj_to=NULL;
			CHAR_DATA *char_to=NULL;
			ROOM_DATA *room_to=NULL;
			if (!((*subfield == UID_CHAR) || (*subfield == UID_OBJ) || (*subfield == UID_ROOM)))
			{
				trig_log(trig, "object.put: ������������ ��������, ���������� ������� UID");
				return;
			}
			if (*subfield == UID_OBJ)
			{
				obj_to = find_obj(atoi(subfield+1));
				if (!(obj_to && GET_OBJ_TYPE(obj_to) == ITEM_CONTAINER))
				{
					trig_log(trig, "object.put: ������-�������� �� ������ ��� �� �������� �����������");
					return;
				}
			}
			if (*subfield == UID_CHAR)
			{
				char_to = find_char(atoi(subfield+1));
				if (!(char_to && can_take_obj(char_to, o)))
				{
					trig_log(trig, "object.put: �������-�������� �� ������ ��� �� ����� ����� ���� ������");
					return;
				}
			}
			if (*subfield == UID_ROOM)
			{
				room_to = find_room(atoi(subfield+1));
				if (!(room_to && (room_to->number != NOWHERE)))
				{
					trig_log(trig, "object.put: ������������ ������� ��� ���������� �������");
					return;
				}
			}
			//found something to put our object
			//let's make it nobody's!
			if (o->worn_by)
				unequip_char(o->worn_by, o->worn_on);
			else if (o->carried_by)
				obj_from_char(o);
			else if (o->in_obj)
				obj_from_obj(o);
			else if (o->in_room > NOWHERE)
				obj_from_room(o);
			else
			{
				trig_log(trig, "object.put: �� ������� ������� ������");
				return;
			}
			//finally, put it to destination
			if (char_to)
				obj_to_char(o, char_to);
			else if (obj_to)
				obj_to_obj(o, obj_to);
			else if (room_to)
				obj_to_room(o, real_room(room_to->number));
			else
			{
				sprintf(buf2, "object.put: ATTENTION! �� ����� ���������� ������� >%s< � �������� �������� ������������ �������. ������ ������ � NOWHERE",
						o->short_description);
				trig_log(trig, buf2);
				return;
			}
		}
//-Polud
		else if (!str_cmp(field, "char") ||
				 !str_cmp(field, "pc") || !str_cmp(field, "npc") || !str_cmp(field, "all"))
		{
			int inroom;

			// ����������� ������ (��� obj)
			inroom = obj_room(o);
			if (inroom == NOWHERE)
			{
				trig_log(trig, "obj-����������� ������ � NOWHERE");
				return;
			}

			for (rndm = world[inroom]->people; rndm; rndm = rndm->next_in_room)
			{
				if ((*field == 'a') ||
						(!IS_NPC(rndm) && *field != 'n') ||
						(IS_NPC(rndm) && IS_CHARMED(rndm) && *field == 'c') ||
						(IS_NPC(rndm) && !IS_CHARMED(rndm) && *field == 'n'))
					sprintf(str + strlen(str), "%c%ld ", UID_CHAR, GET_ID(rndm));
			}

			return;
		}
		else if (!str_cmp(field, "owner"))
		{
			if (*subfield)
			{
				skip_spaces(&subfield);
				int num = atoi(subfield);
				// ����� ���� ��������. �� ���� 0 -- ���������� ���������.
				// ������������ ����������� �������� ��������� �� �����.
				//if (num > 0)
				//{
				GET_OBJ_OWNER(o) = num;
				//}
			}
			else
			{
				sprintf(str, "%d", GET_OBJ_OWNER(o));
			}
		}
		else if (!str_cmp(field, "varexists"))
		{
			strcpy(str, "0");
			if (SCRIPT(o))
			{
				if (find_var_cntx(&((SCRIPT(o))->global_vars), subfield, sc->context))
					strcpy(str, "1");
			}
		}
		else if (!str_cmp(field, "cost"))
		{
			if (*subfield && isdigit(*subfield))
			{
				skip_spaces(&subfield);
				o->set_cost(atoi(subfield));
			}
			else
			{
				sprintf(str, "%d", GET_OBJ_COST(o));
			}
		}
		else if (!str_cmp(field, "rent"))
		{
			if (*subfield && isdigit(*subfield))
			{
				skip_spaces(&subfield);
				o->set_rent(atoi(subfield));
			}
			else
			{
				sprintf(str, "%d", GET_OBJ_RENT(o));
			}
		}
		else if (!str_cmp(field, "rent_eq"))
		{
			if (*subfield && isdigit(*subfield))
			{
				skip_spaces(&subfield);
				o->set_rent_eq(atoi(subfield));
			}
			else
			{
				sprintf(str, "%d", GET_OBJ_RENTEQ(o));
			}
		}
		else //get global var. obj.varname
		{
			if (SCRIPT(o))
			{
				vd = find_var_cntx(&((SCRIPT(o))->global_vars), field, sc->context);
				if (vd)
					sprintf(str, "%s", vd->value);
				else
				{
					sprintf(buf2, "Type: %d. unknown object field: '%s'", type, field);
					trig_log(trig, buf2);
				}
			}
			else
			{
				sprintf(buf2, "Type: %d. unknown object field: '%s'", type, field);
				trig_log(trig, buf2);
			}
		}
	}
	else if (r)
	{
		if (text_processed(field, subfield, vd, str))
			return;
		else if (!str_cmp(field, "name"))
			strcpy(str, r->name);
		else if (!str_cmp(field, "north"))
		{
			if (r->dir_option[NORTH])
				sprintf(str, "%d",
						find_room_vnum(GET_ROOM_VNUM(r->dir_option[NORTH]->to_room)));
		}
		else if (!str_cmp(field, "east"))
		{
			if (r->dir_option[EAST])
				sprintf(str, "%d", find_room_vnum(GET_ROOM_VNUM(r->dir_option[EAST]->to_room)));
		}
		else if (!str_cmp(field, "south"))
		{
			if (r->dir_option[SOUTH])
				sprintf(str, "%d",
						find_room_vnum(GET_ROOM_VNUM(r->dir_option[SOUTH]->to_room)));
		}
		else if (!str_cmp(field, "west"))
		{
			if (r->dir_option[WEST])
				sprintf(str, "%d", find_room_vnum(GET_ROOM_VNUM(r->dir_option[WEST]->to_room)));
		}
		else if (!str_cmp(field, "up"))
		{
			if (r->dir_option[UP])
				sprintf(str, "%d", find_room_vnum(GET_ROOM_VNUM(r->dir_option[UP]->to_room)));
		}
		else if (!str_cmp(field, "down"))
		{
			if (r->dir_option[DOWN])
				sprintf(str, "%d", find_room_vnum(GET_ROOM_VNUM(r->dir_option[DOWN]->to_room)));
		}
		else if (!str_cmp(field, "vnum"))
		{
			sprintf(str, "%d", r->number);
		}
		else if (!str_cmp(field, "sectortype"))//Polud ���������� ������ - ��� �������
		{
			sprinttype(r->sector_type, sector_types, str);
		}
		else if (!str_cmp(field, "id"))
		{
			sprintf(str, "%c%d", UID_ROOM, find_room_uid(r->number));
		}
		else if (!str_cmp(field, "flag"))
			gm_flag(subfield, room_bits, r->room_flags, str);
		else if (!str_cmp(field, "people"))
		{
			if (r->people)
				sprintf(str, "%c%ld", UID_CHAR, GET_ID(r->people));
		}
		else if (!str_cmp(field, "char")
				 || !str_cmp(field, "pc")
				 || !str_cmp(field, "npc")
				 || !str_cmp(field, "all"))
		{
			int inroom;

			// ����������� ������ (��� room)
			inroom = real_room(r->number);
			if (inroom == NOWHERE)
			{
				trig_log(trig, "room-����������� ������ � NOWHERE");
				return;
			}

			for (rndm = world[inroom]->people; rndm; rndm = rndm->next_in_room)
			{
				if ((*field == 'a')
						|| (!IS_NPC(rndm) && *field != 'n')
						|| (IS_NPC(rndm) && IS_CHARMED(rndm) && *field == 'c')
						|| (IS_NPC(rndm) && !IS_CHARMED(rndm) && *field == 'n'))
				{
					sprintf(str + strlen(str), "%c%ld ", UID_CHAR, GET_ID(rndm));
				}
			}

			return;
		}
		else if (!str_cmp(field, "objects"))
		{
			//mixaz  ������ ������ �������� � �������
			int inroom;
			// ����������� ������ (��� room)
			inroom = real_room(r->number);
			if (inroom == NOWHERE)
			{
				trig_log(trig, "room-����������� ������ � NOWHERE");
				return;
			}
			for (obj = world[inroom]->contents; obj; obj = obj->next_content)
			{
				sprintf(str + strlen(str), "%c%ld ", UID_OBJ, GET_ID(obj));
			}
			return;
			//mixaz - end
		}
		else if (!str_cmp(field, "varexists"))
		{
			//room.varexists<0;1>
			strcpy(str, "0");
			if (SCRIPT(r))
			{
				if (find_var_cntx(&((SCRIPT(r))->global_vars), subfield, sc->context))
					strcpy(str, "1");
			}
		}
		else //get global var. room.varname
		{
			if (SCRIPT(r))
			{
				vd = find_var_cntx(&((SCRIPT(r))->global_vars), field, sc->context);
				if (vd)
					sprintf(str, "%s", vd->value);
				else
				{
					sprintf(buf2, "Type: %d. unknown room field: '%s'", type, field);
					trig_log(trig, buf2);
				}
			}
			else
			{
				sprintf(buf2, "Type: %d. unknown room field: '%s'", type, field);
				trig_log(trig, buf2);
			}
		}
	}
	else if (text_processed(field, subfield, vd, str))
		return;
}


// substitutes any variables into line and returns it as buf
void var_subst(void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type, const char *line, char *buf)
{
	char tmp[MAX_INPUT_LENGTH], repl_str[MAX_INPUT_LENGTH], *var, *field, *p;
	char *subfield_p, subfield[MAX_INPUT_LENGTH];
	char *local_p, local[MAX_INPUT_LENGTH];
	int left = 0 , len = 0;
	int paren_count = 0;

	if (!strchr(line, '%'))
	{
		strcpy(buf, line);
		return;
	}

	p = strcpy(tmp, line);
	subfield_p = subfield;

	left = MAX_INPUT_LENGTH - 1;

	while (*p && (left > 0))
	{
		while (*p && (*p != '%') && (left > 0))
		{
			*(buf++) = *(p++);
			left--;
		}

		*buf = '\0';

		// double %
		if (*p && (*(++p) == '%') && (left > 0))
		{
			*(buf++) = *(p++);
			*buf = '\0';
			left--;
			continue;
		}
		else if (*p && (left > 0))
		{
			for (var = p; *p && (*p != '%') && (*p != '.'); p++);
			field = p;
			subfield_p = subfield;	//new
			if (*p == '.')
			{
				*(p++) = '\0';
				local_p = local;
				for (field = p; *p && local_p && ((*p != '%') || (paren_count)); p++)
				{
					if (*p == '(')
					{
						if (!paren_count)
							*p = '\0';
						else
							*local_p++ = *p;
						paren_count++;
					}
					else if (*p == ')')
					{
						if (paren_count == 1)
							*p = '\0';
						else
							*local_p++ = *p;
						paren_count--;
						if (!paren_count)
						{
							*local_p = '\0';
//							log("Recoursive VAR_SUBST <%s>", local);
							var_subst(go, sc, trig, type, local, subfield_p);
//							log("Get value  VAR_SUBST <%s>", subfield_p);
							local_p = NULL;
							subfield_p = subfield + strlen(subfield);
						}
					}
					else if (paren_count)
					{
						*local_p++ = *p;
						//*subfield_p++ = *p;
					}
				}
			}
			*(p++) = '\0';
			*subfield_p = '\0';
			*repl_str = '\0';
			find_replacement(go, sc, trig, type, var, field, subfield, repl_str);

			strncat(buf, repl_str, left);
			len = strlen(repl_str);
			buf += len;
			left -= len;
		}
	}
}


// returns 1 if string is all digits, else 0
int is_num(char *num)
{
	while (*num && (isdigit(*num) || *num == '-'))
		num++;

	if (!*num || a_isspace(*num))
		return 1;
	else
		return 0;
}


// evaluates 'lhs op rhs', and copies to result
void eval_op(const char *op, char *lhs, char *rhs, char *result, void *go, SCRIPT_DATA * sc, TRIG_DATA * trig)
{
	char *p = 0;
	int n;

	// strip off extra spaces at begin and end
	while (*lhs && a_isspace(*lhs))
		lhs++;
	while (*rhs && a_isspace(*rhs))
		rhs++;

	for (p = lhs; *p; p++);
	for (--p; (p >= lhs) && a_isspace(*p); *p-- = '\0');
	for (p = rhs; *p; p++);
	for (--p; (p >= rhs) && a_isspace(*p); *p-- = '\0');


	// find the op, and figure out the value
	if (!strcmp("||", op))
	{
		if ((!*lhs || (*lhs == '0')) && (!*rhs || (*rhs == '0')))
			strcpy(result, "0");
		else
			strcpy(result, "1");
	}
	else if (!strcmp("&&", op))
	{
		if (!*lhs || (*lhs == '0') || !*rhs || (*rhs == '0'))
			strcpy(result, "0");
		else
			strcpy(result, "1");
	}
	else if (!strcmp("==", op))
	{
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) == atoi(rhs));
		else
			sprintf(result, "%d", !str_cmp(lhs, rhs));
	}
	else if (!strcmp("!=", op))
	{
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) != atoi(rhs));
		else
			sprintf(result, "%d", str_cmp(lhs, rhs));
	}
	else if (!strcmp("<=", op))
	{
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) <= atoi(rhs));
		else
			sprintf(result, "%d", str_cmp(lhs, rhs) <= 0);
	}
	else if (!strcmp(">=", op))
	{
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) >= atoi(rhs));
		else
			sprintf(result, "%d", str_cmp(lhs, rhs) <= 0);
	}
	else if (!strcmp("<", op))
	{
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) < atoi(rhs));
		else
			sprintf(result, "%d", str_cmp(lhs, rhs) < 0);
	}
	else if (!strcmp(">", op))
	{
		if (is_num(lhs) && is_num(rhs))
			sprintf(result, "%d", atoi(lhs) > atoi(rhs));
		else
			sprintf(result, "%d", str_cmp(lhs, rhs) > 0);
	}
	else if (!strcmp("/=", op))
		sprintf(result, "%c", str_str(lhs, rhs) ? '1' : '0');
	else if (!strcmp("*", op))
		sprintf(result, "%d", atoi(lhs) * atoi(rhs));
	else if (!strcmp("/", op))
		sprintf(result, "%d", (n = atoi(rhs)) ? (atoi(lhs) / n) : 0);
	else if (!strcmp("+", op))
		sprintf(result, "%d", atoi(lhs) + atoi(rhs));
	else if (!strcmp("-", op))
		sprintf(result, "%d", atoi(lhs) - atoi(rhs));
	else if (!strcmp("!", op))
	{
		if (is_num(rhs))
			sprintf(result, "%d", !atoi(rhs));
		else
			sprintf(result, "%d", !*rhs);
	}
}


/*
 * p points to the first quote, returns the matching
 * end quote, or the last non-null char in p.
*/
char *matching_quote(char *p)
{
	for (p++; *p && (*p != '"'); p++)
	{
		if (*p == '\\')
			p++;
	}

	if (!*p)
		p--;

	return p;
}

/*
 * p points to the first paren.  returns a pointer to the
 * matching closing paren, or the last non-null char in p.
 */
char *matching_paren(char *p)
{
	int i;

	for (p++, i = 1; *p && i; p++)
	{
		if (*p == '(')
			i++;
		else if (*p == ')')
			i--;
		else if (*p == '"')
			p = matching_quote(p);
	}

	return --p;
}


// evaluates line, and returns answer in result
void eval_expr(char *line, char *result, void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type)
{
	char expr[MAX_INPUT_LENGTH], *p;

	while (*line && a_isspace(*line))
		line++;

	if (eval_lhs_op_rhs(line, result, go, sc, trig, type));
	else if (*line == '(')
	{
		strcpy(expr, line);
		p = matching_paren(expr);
		*p = '\0';
		eval_expr(expr + 1, result, go, sc, trig, type);
	}
	else
		var_subst(go, sc, trig, type, line, result);
}


/*
 * evaluates expr if it is in the form lhs op rhs, and copies
 * answer in result.  returns 1 if expr is evaluated, else 0
 */
int eval_lhs_op_rhs(char *expr, char *result, void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type)
{
	char *p, *tokens[MAX_INPUT_LENGTH];
	char line[MAX_INPUT_LENGTH], lhr[MAX_INPUT_LENGTH], rhr[MAX_INPUT_LENGTH];
	int i, j;

	/*
	 * valid operands, in order of priority
	 * each must also be defined in eval_op()
	 */
	static const char *ops[] =
	{
		"||",
		"&&",
		"==",
		"!=",
		"<=",
		">=",
		"<",
		">",
		"/=",
		"-",
		"+",
		"/",
		"*",
		"!",
		"\n"
	};

	p = strcpy(line, expr);

	/*
	 * initialize tokens, an array of pointers to locations
	 * in line where the ops could possibly occur.
	 */
	for (j = 0; *p; j++)
	{
		tokens[j] = p;
		if (*p == '(')
			p = matching_paren(p) + 1;
		else if (*p == '"')
			p = matching_quote(p) + 1;
		else if (a_isalnum(*p))
			for (p++; *p && (a_isalnum(*p) || a_isspace(*p)); p++);
		else
			p++;
	}
	tokens[j] = NULL;

	for (i = 0; *ops[i] != '\n'; i++)
		for (j = 0; tokens[j]; j++)
			if (!strn_cmp(ops[i], tokens[j], strlen(ops[i])))
			{
				*tokens[j] = '\0';
				p = tokens[j] + strlen(ops[i]);

				eval_expr(line, lhr, go, sc, trig, type);
				eval_expr(p, rhr, go, sc, trig, type);
				eval_op(ops[i], lhr, rhr, result, go, sc, trig);

				return 1;
			}

	return 0;
}


// returns 1 if next iteration, else 0
int process_foreach(char *cond, void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type)
/*++
      cond - ������ ���������� �����. ��� ������� "foreach i .." cond = "i .."
      go - ��������� �� MOB/OBJ/ROOM (��. type)
      sc - SCRIPT(go)
      trig - ����������� �������
      type - ��� (MOB_TRIGGER,OBJ_TRIGGER,WLD_TRIGGER)

������
foreach i <������>
�������� ���:

1. ���� ������ ������ - �����
2. ���� ������� �� ����� ���������� i ��� �������� ���������� �� ����� ��
   ������ �������� ������ (��������� ���������), ���������� i ������ �������
   �������� � ��������� ����
3. ���������� i ����� �-��� �������� ������. ���� ��� ��������� ������� - �����
   ����� i = ����. ������� � ��������� ����
--*/
{
	char name[MAX_INPUT_LENGTH];
	char value[MAX_INPUT_LENGTH];
	char result[MAX_INPUT_LENGTH];
	char *list, *p;
	struct trig_var_data *v, *pos;
	int v_strpos = MAX_INPUT_LENGTH;


	*value = '\0';
	skip_spaces(&cond);
	list = one_argument(cond, name);
	skip_spaces(&list);

	if (!*name)
	{
		trig_log(trig, "foreach w/o an variable");
		return 0;
	}

	eval_expr(list, result, go, sc, trig, type);
	list = result;

	v = find_var_cntx(&GET_TRIG_VARS(trig), name, 0);
	//��������� ������� � ������ �� ������� �� ��������� ������������ �����
	sprintf(value, "%s_strpos", name);
	pos = find_var_cntx(&GET_TRIG_VARS(trig), value, 0);
	if (v)
	{
		char *ptr = strstr(list, v->value);
		// ���������� ��� �� �� � ���� ���� �� ��� ����� ������� �� �������� ���� %self.pc%,
		// ������� �������� �� ������ �������� ����� � ������� �� ��������, ������� ��������� ��� ������
		// ����� �� ��������� ������ � ������ � ������ ������� �� ����������� �� ��������� ����������
		if (pos && pos->value && ((unsigned)atoi(pos->value) < strlen(list)) && !(strncmp(list + v_strpos, v->value, strlen(v->value))))
		{
			v_strpos = atoi(pos->value);
			ptr = list + v_strpos;
		}
		else
			v_strpos = ptr - list;
		// ��������� �� ������� ������� ����� �������� ������� � ����� ���
		while (ptr)
		{
			if ((ptr != list) && !a_isspace(*(ptr - 1)))
			{
				while (*ptr && !a_isspace(*ptr))
					++ptr;
				list = ptr;
				ptr = strstr(list, v->value);
				continue;
			}

			list = ptr + strlen(v->value);
			// x5 � x534
			if (*list && !a_isspace(*(list)))
			{
				while (*list && !a_isspace(*list))
					++list;
				ptr = strstr(list, v->value);
			}
			else
				break;
		}
	}
	//list = one_argument(list, value);
	// one_argument() ������������ ������, �.�. �� ����������� ������ � lowcase
	p = value;
	while (*list && a_isspace(*list))
		++list;		// ������� ��������
	v_strpos = list - result;
	while (*list && !a_isspace(*list))
		*p++ = *list++;	// ����������� �����
	*p = 0;

	if (!*value)
	{
		if (pos)
		{
			strcat(name, "_strpos");
			remove_var_cntx(&GET_TRIG_VARS(trig), name, 0);
		}
		return 0;
	}
	add_var_cntx(&GET_TRIG_VARS(trig), name, value, 0);
	//��������� ������� � ������ �� ������� �� ��������� ������������ �����
	//������ ��� ��� ����� �� ����� ���� ������ ������ foreach ������ foreach ����� ������������ ���� ����������
	strcat(name, "_strpos");
	sprintf(value, "%d", v_strpos);
	add_var_cntx(&GET_TRIG_VARS(trig), name, value, 0);
	return 1;
}

// returns 1 if cond is true, else 0
int process_if(char *cond, void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type)
{
	char result[MAX_INPUT_LENGTH], *p;

	eval_expr(cond, result, go, sc, trig, type);

	p = result;
	skip_spaces(&p);

	if (!*p || *p == '0')
		return 0;
	else
		return 1;
}


/*
 * scans for end of if-block.
 * returns the line containg 'end', or NULL
 */
struct cmdlist_element *find_end(TRIG_DATA * trig, struct cmdlist_element *cl)
{
	char tmpbuf[MAX_INPUT_LENGTH];
	char *p;
#ifdef DG_CODE_ANALYZE
	const char *cmd = cl ? cl->cmd : "<NULL>";
#endif

	while ((cl = cl ? cl->next : cl) != NULL)
	{
		for (p = cl->cmd; *p && a_isspace(*p); p++);

		if (!strn_cmp("if ", p, 3))
			cl = find_end(trig, cl);
		else if (!strn_cmp("end", p, 3))
			break;
	}

#ifdef DG_CODE_ANALYZE
	if (!cl)
	{
		sprintf(tmpbuf, "end not found for '%s'.", cmd);
		trig_log(trig, tmpbuf);
	}
#endif

	return cl;
}


/*
 * searches for valid elseif, else, or end to continue execution at.
 * returns line of elseif, else, or end if found, or NULL.
 */
struct cmdlist_element *find_else_end(TRIG_DATA * trig,
									  struct cmdlist_element *cl, void *go, SCRIPT_DATA * sc, int type)
{
	char *p;
#ifdef DG_CODE_ANALYZE
	const char *cmd = cl ? cl->cmd : "<NULL>";
#endif

	while ((cl = cl ? cl->next : cl) != NULL)
	{
		for (p = cl->cmd; *p && a_isspace(*p); p++);

		if (!strn_cmp("if ", p, 3))
			cl = find_end(trig, cl);
		else if (!strn_cmp("elseif ", p, 7))
		{
			if (process_if(p + 7, go, sc, trig, type))
			{
				GET_TRIG_DEPTH(trig)++;
				break;
			}
		}
		else if (!strn_cmp("else", p, 4))
		{
			GET_TRIG_DEPTH(trig)++;
			break;
		}
		else if (!strn_cmp("end", p, 3))
			break;
	}

#ifdef DG_CODE_ANALYZE
	if (!cl)
	{
		sprintf(buf, "closing 'else/end' is not found for '%s'", cmd);
		trig_log(trig, buf);
	}
#endif

	return cl;
}


/*
* scans for end of while/foreach/switch-blocks.
* returns the line containg 'end', or NULL
*/
struct cmdlist_element *find_done(TRIG_DATA * trig, struct cmdlist_element *cl)
{
	char *p;
#ifdef DG_CODE_ANALYZE
	const char *cmd = cl ? cl->cmd : "<NULL>";
#endif

	while ((cl = cl ? cl->next : cl) != NULL)
	{
		for (p = cl->cmd; *p && isspace(*p); p++);

		if (!strn_cmp("while ", p, 6) || !strn_cmp("switch ", p, 7) || !strn_cmp("foreach ", p, 8))
			cl = find_done(trig, cl);
		else if (!strn_cmp("done", p, 4))
			break;
	}

#ifdef DG_CODE_ANALYZE
	if (!cl)
	{
		sprintf(buf, "closing 'done' is not found for '%s'", cmd);
		trig_log(trig, buf);
	}
#endif

	return cl;
}

/*
* scans for a case/default instance
* returns the line containg the correct case instance, or NULL
*/
struct cmdlist_element *find_case(TRIG_DATA * trig,
								  struct cmdlist_element *cl, void *go, SCRIPT_DATA * sc, int type, char *cond)
{
	char result[MAX_INPUT_LENGTH];
	char *p;
#ifdef DG_CODE_ANALYZE
	const char *cmd = cl ? cl->cmd : "<NULL>";
#endif

	eval_expr(cond, result, go, sc, trig, type);

	while ((cl = cl ? cl->next : cl) != NULL)
	{
		for (p = cl->cmd; *p && isspace(*p); p++);

		if (!strn_cmp("while ", p, 6) || !strn_cmp("switch ", p, 7) || !strn_cmp("foreach ", p, 8))
			cl = find_done(trig, cl);
		else if (!strn_cmp("case ", p, 5))
		{
			char *tmpbuf = (char *) malloc(MAX_STRING_LENGTH);
			eval_op("==", result, p + 5, tmpbuf, go, sc, trig);
			if (*tmpbuf && *tmpbuf != '0')
			{
				free(tmpbuf);
				break;
			}
			free(tmpbuf);
		}
		else if (!strn_cmp("default", p, 7))
			break;
		else if (!strn_cmp("done", p, 4))
			break;
	}

#ifdef DG_CODE_ANALYZE
	if (!cl)
	{
		sprintf(buf, "closing 'done' not found for '%s'", cmd);
		trig_log(trig, buf);
	}
#endif

	return cl;
}


// processes any 'wait' commands in a trigger
void process_wait(void *go, TRIG_DATA * trig, int type, char *cmd, struct cmdlist_element *cl)
{
	char *arg;
	struct wait_event_data *wait_event_obj;
	long time = 0, hr, min, ntime;
	char c;

	extern TIME_INFO_DATA time_info;
	extern unsigned long dg_global_pulse;

	if (trig->attach_type == MOB_TRIGGER && IS_SET(GET_TRIG_TYPE(trig), MTRIG_DEATH))
	{
		sprintf(buf,
				"&Y��������&G ������������ wait � DEATH �������� '%s' (VNUM=%d).",
				GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig));
		mudlog(buf, BRF, LVL_BUILDER, ERRLOG, TRUE);
		sprintf(buf, "&G��� �������� ����� wait �������� �� �����!");
		mudlog(buf, BRF, LVL_BUILDER, ERRLOG, TRUE);
	}

	arg = any_one_arg(cmd, buf);
	skip_spaces(&arg);

	if (!*arg)
	{
		sprintf(buf2, "wait w/o an arg: '%s'", cl->cmd);
		trig_log(trig, buf2);
	}
	else if (!strn_cmp(arg, "until ", 6))  	// valid forms of time are 14:30 and 1430
	{
		if (sscanf(arg, "until %ld:%ld", &hr, &min) == 2)
			min += (hr * 60);
		else
			min = (hr % 100) + ((hr / 100) * 60);

		// calculate the pulse of the day of "until" time
		ntime = (min * SECS_PER_MUD_HOUR * PASSES_PER_SEC) / 60;

		// calculate pulse of day of current time
		time = (dg_global_pulse % (SECS_PER_MUD_HOUR * PASSES_PER_SEC)) +
			   (time_info.hours * SECS_PER_MUD_HOUR * PASSES_PER_SEC);

		if (time >= ntime)	// adjust for next day
			time = (SECS_PER_MUD_DAY * PASSES_PER_SEC) - time + ntime;
		else
			time = ntime - time;
	}
	else
	{
		if (sscanf(arg, "%ld %c", &time, &c) == 2)
		{
			if (c == 't')
				time *= PULSES_PER_MUD_HOUR;
			else if (c == 's')
				time *= PASSES_PER_SEC;
		}
	}

	CREATE(wait_event_obj, struct wait_event_data, 1);
	wait_event_obj->trigger = trig;
	wait_event_obj->go = go;
	wait_event_obj->type = type;

	if (GET_TRIG_WAIT(trig))
	{
		trig_log(trig, "Wait structure already allocated for trigger");
	}

	GET_TRIG_WAIT(trig) = add_event(time, trig_wait_event, wait_event_obj);
	trig->curr_state = cl->next;
}


// processes a script set command
void process_set(SCRIPT_DATA * sc, TRIG_DATA * trig, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH], *value;

	value = two_arguments(cmd, arg, name);

	skip_spaces(&value);

	if (!*name)
	{
		sprintf(buf2, "set w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	add_var_cntx(&GET_TRIG_VARS(trig), name, value, 0);
}

// processes a script eval command
void process_eval(void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH];
	char result[MAX_INPUT_LENGTH], *expr;

	expr = two_arguments(cmd, arg, name);

	skip_spaces(&expr);

	if (!*name)
	{
		sprintf(buf2, "eval w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	eval_expr(expr, result, go, sc, trig, type);
	add_var_cntx(&GET_TRIG_VARS(trig), name, result, 0);
}


// script attaching a trigger to something
void process_attach(void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], trignum_s[MAX_INPUT_LENGTH];
	char result[MAX_INPUT_LENGTH], *id_p;
	TRIG_DATA *newtrig;
	CHAR_DATA *c = NULL;
	OBJ_DATA *o = NULL;
	room_data *r = NULL;
	long trignum;

	id_p = two_arguments(cmd, arg, trignum_s);
	skip_spaces(&id_p);

	if (!*trignum_s)
	{
		sprintf(buf2, "attach w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (!id_p || !*id_p || atoi(id_p + 1) == 0)
	{
		sprintf(buf2, "attach invalid id(1) arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	// parse and locate the id specified
	eval_expr(id_p, result, go, sc, trig, type);

	c = get_char(id_p);
	if (!c)
	{
		o = get_obj(id_p);
		if (!o)
		{
			r = get_room(id_p);
			if (!r)
			{
				sprintf(buf2, "attach invalid id arg(3): '%s'", cmd);
				trig_log(trig, buf2);
				return;
			}
		}
	}

	// locate and load the trigger specified
	trignum = real_trigger(atoi(trignum_s));
	if (trignum >=0 && (((c) && trig_index[trignum]->proto->attach_type != MOB_TRIGGER) ||
						((o) && trig_index[trignum]->proto->attach_type != OBJ_TRIGGER) ||
						((r) && trig_index[trignum]->proto->attach_type != WLD_TRIGGER)))
	{
		sprintf(buf2, "attach trigger : '%s' invalid attach_type: %s expected %s", trignum_s,
				attach_name[(int)trig_index[trignum]->proto->attach_type],
				attach_name[(c?0:(o?1:(r?2:3)))]);
		trig_log(trig, buf2);
		return;
	}
	if (trignum < 0 || !(newtrig = read_trigger(trignum)))
	{
		sprintf(buf2, "attach invalid trigger: '%s'", trignum_s);
		trig_log(trig, buf2);
		return;
	}

	if (c)
	{
		if (!SCRIPT(c))
			CREATE(SCRIPT(c), SCRIPT_DATA, 1);
		add_trigger(SCRIPT(c), newtrig, -1);
		return;
	}

	if (o)
	{
		if (!SCRIPT(o))
			CREATE(SCRIPT(o), SCRIPT_DATA, 1);
		add_trigger(SCRIPT(o), newtrig, -1);
		return;
	}

	if (r)
	{
		if (!SCRIPT(r))
			CREATE(SCRIPT(r), SCRIPT_DATA, 1);
		add_trigger(SCRIPT(r), newtrig, -1);
		return;
	}

	return;
}


// script detaching a trigger from something
TRIG_DATA *process_detach(void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], trignum_s[MAX_INPUT_LENGTH];
	char result[MAX_INPUT_LENGTH], *id_p;
	CHAR_DATA *c = NULL;
	OBJ_DATA *o = NULL;
	room_data *r = NULL;
	TRIG_DATA *retval = trig;


	id_p = two_arguments(cmd, arg, trignum_s);
	skip_spaces(&id_p);

	if (!*trignum_s)
	{
		sprintf(buf2, "detach w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return retval;
	}

	if (!id_p || !*id_p || atoi(id_p + 1) == 0)
	{
		sprintf(buf2, "detach invalid id arg(1): '%s'", cmd);
		trig_log(trig, buf2);
		return retval;
	}

	// parse and locate the id specified
	eval_expr(id_p, result, go, sc, trig, type);

	c = get_char(id_p);
	if (!c)
	{
		o = get_obj(id_p);
		if (!o)
		{
			r = get_room(id_p);
			if (!r)
			{
				sprintf(buf2, "detach invalid id arg(3): '%s'", cmd);
				trig_log(trig, buf2);
				return retval;
			}
		}
	}

	if (c && SCRIPT(c))
	{
		if (remove_trigger(SCRIPT(c), trignum_s, &retval))
		{
			if (!TRIGGERS(SCRIPT(c)))
			{
				free_script(SCRIPT(c));
				SCRIPT(c) = NULL;
			}
		}
		return retval;
	}

	if (o && SCRIPT(o))
	{
		if (remove_trigger(SCRIPT(o), trignum_s, &retval))
		{
			if (!TRIGGERS(SCRIPT(o)))
			{
				free_script(SCRIPT(o));
				SCRIPT(o) = NULL;
			}
		}
		return retval;
	}

	if (r && SCRIPT(r))
	{
		if (remove_trigger(SCRIPT(r), trignum_s, &retval))
		{
			if (!TRIGGERS(SCRIPT(r)))
			{
				free_script(SCRIPT(r));
				SCRIPT(r) = NULL;
			}
		}
		return retval;
	}

	return retval;
}

/* script run a trigger for something
   return TRUE   - trigger find and runned
          FALSE  - trigger not runned
*/
int process_run(void *go, SCRIPT_DATA ** sc, TRIG_DATA ** trig, int type, char *cmd, int *retval)
{
	char arg[MAX_INPUT_LENGTH], trignum_s[MAX_INPUT_LENGTH], *name, *cname;
	char result[MAX_INPUT_LENGTH], *id_p;
	TRIG_DATA *runtrig = NULL;
//	SCRIPT_DATA *runsc = NULL;
	struct trig_var_data *vd;
	CHAR_DATA *c = NULL;
	OBJ_DATA *o = NULL;
	room_data *r = NULL;
	void *trggo = NULL;
	int trgtype = 0, string = FALSE, num = 0, n;

	id_p = two_arguments(cmd, arg, trignum_s);
	skip_spaces(&id_p);

	if (!*trignum_s)
	{
		sprintf(buf2, "run w/o an arg: '%s'", cmd);
		trig_log(*trig, buf2);
		return (FALSE);
	}

	if (!id_p || !*id_p || atoi(id_p + 1) == 0)
	{
		sprintf(buf2, "run invalid id arg(1): '%s'", cmd);
		trig_log(*trig, buf2);
		return (FALSE);
	}

	// parse and locate the id specified
	eval_expr(id_p, result, go, *sc, *trig, type);

	c = get_char(id_p);
	if (!c)
	{
		o = get_obj(id_p);
		if (!o)
		{
			r = get_room(id_p);
			if (!r)
			{
				sprintf(buf2, "run invalid id arg(3): '%s'", cmd);
				trig_log(*trig, buf2);
				return (FALSE);
			}
		}
	}

	if (c && SCRIPT(c))
	{
		runtrig = TRIGGERS(SCRIPT(c));
		//runsc = SCRIPT(c);
		trgtype = MOB_TRIGGER;
		trggo = (void *) c;
	}
	else if (o && SCRIPT(o))
	{
		runtrig = TRIGGERS(SCRIPT(o));
		//runsc = SCRIPT(o);
		trgtype = OBJ_TRIGGER;
		trggo = (void *) o;
	}
	else if (r && SCRIPT(r))
	{
		runtrig = TRIGGERS(SCRIPT(r));
		//runsc = SCRIPT(r);
		trgtype = WLD_TRIGGER;
		trggo = (void *) r;
	};

	name = trignum_s;
	if ((cname = strstr(name, ".")) || (!isdigit(*name)))
	{
		string = TRUE;
		if (cname)
		{
			*cname = '\0';
			num = atoi(name);
			name = ++cname;
		}
	}
	else
		num = atoi(name);

	for (n = 0; runtrig; runtrig = runtrig->next)
	{
		if (string)
		{
			if (isname(name, GET_TRIG_NAME(runtrig)))
				if (++n >= num)
					break;
		}
		else if (++n >= num)
			break;
		else if (trig_index[runtrig->nr]->vnum == num)
			break;
	}

	if (!runtrig)
		return (FALSE);


	// copy variables
	if (*trig && runtrig)
		for (vd = GET_TRIG_VARS(*trig); vd; vd = vd->next)
		{
			if (vd->context)
			{
				sprintf(buf2, "Local variable %s with nonzero context %ld", vd->name, vd->context);
				trig_log(*trig, buf2);
			}
			add_var_cntx(&GET_TRIG_VARS(runtrig), vd->name, vd->value, 0);
		}
// � ������������ ���������� ���������� ����� ���-�� ���������
//
//  if (*sc && runtrig)
//     for (vd = (*sc)->global_vars; vd; vd = vd->next)
//         add_var(&GET_TRIG_VARS(runtrig), vd->name, vd->value, vd->context);
//

	if (!GET_TRIG_DEPTH(runtrig))
	{
		*retval = script_driver(trggo, runtrig, trgtype, TRIG_NEW);
	}
	else
	{
		trig_log(runtrig, "Attempt to run waiting trigger", LGH);
	}

	if (go && type == MOB_TRIGGER && reinterpret_cast<CHAR_DATA *>(go)->purged())
	{
		*sc = NULL;
		*trig = NULL;
		return (FALSE);
	}

	runtrig = NULL;
	if (!go || (type == MOB_TRIGGER ? SCRIPT((CHAR_DATA *) go) :
				type == OBJ_TRIGGER ? SCRIPT((OBJ_DATA *) go) :
				type == WLD_TRIGGER ? SCRIPT((ROOM_DATA *) go) : NULL) != *sc)
	{
		*sc = NULL;
		*trig = NULL;
	}
	else
		for (runtrig =
					type == MOB_TRIGGER ? TRIGGERS(SCRIPT((CHAR_DATA *) go)) : type ==
					OBJ_TRIGGER ? TRIGGERS(SCRIPT((OBJ_DATA *) go)) : type ==
					WLD_TRIGGER ? TRIGGERS(SCRIPT((ROOM_DATA *) go)) : NULL; runtrig; runtrig = runtrig->next)
			if (runtrig == *trig)
				break;
	*trig = runtrig;

	return (TRUE);
}


ROOM_DATA *dg_room_of_obj(OBJ_DATA * obj)
{
	if (obj->in_room > NOWHERE)
		return world[obj->in_room];
	if (obj->carried_by)
		return world[obj->carried_by->in_room];
	if (obj->worn_by)
		return world[obj->worn_by->in_room];
	if (obj->in_obj)
		return (dg_room_of_obj(obj->in_obj));
	return NULL;
}


// create a UID variable from the id number
void makeuid_var(void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], varname[MAX_INPUT_LENGTH];
	char result[MAX_INPUT_LENGTH], *uid_p;
	char uid[MAX_INPUT_LENGTH];

	uid_p = two_arguments(cmd, arg, varname);
	skip_spaces(&uid_p);

	if (!*varname)
	{
		sprintf(buf2, "makeuid w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (!uid_p || !*uid_p || atoi(uid_p + 1) == 0)
	{
		sprintf(buf2, "makeuid invalid id arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	eval_expr(uid_p, result, go, sc, trig, type);
	sprintf(uid, "%s", result);
	add_var_cntx(&GET_TRIG_VARS(trig), varname, uid, 0);
}

/**
* Added 17/04/2000
* calculate a UID variable from the VNUM
* calcuid <���������� ���� ������� id> <����> <room|mob|obj> <���������� ����� �� 1 �� �>
* ���� ���������� �� ������ - ������������ ������ ���������.
*/
void calcuid_var(void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], varname[MAX_INPUT_LENGTH];
	char *t, vnum[MAX_INPUT_LENGTH], what[MAX_INPUT_LENGTH];
	char uid[MAX_INPUT_LENGTH], count[MAX_INPUT_LENGTH];
	char uid_type;
	int result = -1;

	t = two_arguments(cmd, arg, varname);
	three_arguments(t, vnum, what, count);

	if (!*varname)
	{
		sprintf(buf2, "calcuid w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (!*vnum || (result = atoi(vnum)) == 0)
	{
		sprintf(buf2, "calcuid invalid VNUM arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (!*what)
	{
		sprintf(buf2, "calcuid exceed TYPE arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	int count_num = 0;
	if (*count)
	{
		count_num = atoi(count);
	}

	if (!str_cmp(what, "room"))
	{
		uid_type = UID_ROOM;
		result = find_room_uid(result);
	}
	else if (!str_cmp(what, "mob"))
	{
		uid_type = UID_CHAR;
		result = find_char_vnum(result, count_num);
	}
	else if (!str_cmp(what, "obj"))
	{
		uid_type = UID_OBJ;
		result = find_obj_vnum(result, count_num);
	}
	else
	{
		sprintf(buf2, "calcuid unknown TYPE arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (result <= -1)
	{
		sprintf(buf2, "calcuid target not found vnum: %s, count: %d", vnum, count_num);
		trig_log(trig, buf2);
		*uid = '\0';
		return;
	}

	sprintf(uid, "%c%d", uid_type, result);
	add_var_cntx(&GET_TRIG_VARS(trig), varname, uid, 0);
}

/*
 * ����� ����� � ������� � ���������� UID-� � ������ ������� ��������
 * ���������� � ��������� ���������� UID ������� PC, � ������ ��������
 * ��������� ��������
 */
void charuid_var(void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, char *cmd)
{
	CHAR_DATA *tch;
	char arg[MAX_INPUT_LENGTH], varname[MAX_INPUT_LENGTH];
	char who[MAX_INPUT_LENGTH], uid[MAX_INPUT_LENGTH];
	char uid_type = UID_CHAR;

	int result = -1;

	three_arguments(cmd, arg, varname, who);

	if (!*varname)
	{
		sprintf(buf2, "charuid w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (!*who)
	{
		sprintf(buf2, "charuid name is missing: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	for (tch = character_list; tch; tch = tch->next)
	{
		if (IS_NPC(tch))
			continue;

		if (!HERE(tch))
			continue;

		if (*who && !isname(who, GET_NAME(tch)))
			continue;

		if (IN_ROOM(tch) != NOWHERE)
			result = GET_ID(tch);
	}


	if (result <= -1)
	{
		sprintf(buf2, "charuid target not found, name: '%s'",who);
		trig_log(trig, buf2);
		*uid = '\0';
		return;
	}

	sprintf(uid, "%c%d", uid_type, result);
	add_var_cntx(&GET_TRIG_VARS(trig), varname, uid, 0);
}

// * ����� ����� ��� calcuidall_var.
bool find_all_char_vnum(long n, char *str)
{
	int count = 0;
	for (CHAR_DATA *ch = character_list; ch; ch = ch->next)
	{
		if (n == GET_MOB_VNUM(ch) && IN_ROOM(ch) != NOWHERE && count < 25)
		{
			snprintf(str + strlen(str), MAX_INPUT_LENGTH, "%c%ld ", UID_CHAR, GET_ID(ch));
			++count;
		}
	}
	return count ? true : false;
}

// * ����� ��������� ��� calcuidall_var.
bool find_all_obj_vnum(long n, char *str)
{
	int count = 0;
	for (OBJ_DATA *i = object_list; i; i = i->next)
	{
		if (n == GET_OBJ_VNUM(i) && count < 25)
		{
			snprintf(str + strlen(str), MAX_INPUT_LENGTH, "%c%ld ", UID_OBJ, GET_ID(i));
			++count;
		}
	}
	return count ? true : false;
}

// * ����-���� � calcuid_var ��� �������� ������ �� ����� ���������� ������ �����/��������� (�� 25�� ���������).
void calcuidall_var(void *go, SCRIPT_DATA * sc, TRIG_DATA * trig, int type, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], varname[MAX_INPUT_LENGTH];
	char *t, vnum[MAX_INPUT_LENGTH], what[MAX_INPUT_LENGTH];
	char uid[MAX_INPUT_LENGTH];
	int result = -1;

	uid[0] = '\0';
	t = two_arguments(cmd, arg, varname);
	two_arguments(t, vnum, what);

	if (!*varname)
	{
		sprintf(buf2, "calcuidall w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (!*vnum || (result = atoi(vnum)) == 0)
	{
		sprintf(buf2, "calcuidall invalid VNUM arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (!*what)
	{
		sprintf(buf2, "calcuidall exceed TYPE arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (!str_cmp(what, "mob"))
	{
		result = find_all_char_vnum(result, uid);
	}
	else if (!str_cmp(what, "obj"))
	{
		result = find_all_obj_vnum(result, uid);
	}
	else
	{
		sprintf(buf2, "calcuidall unknown TYPE arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (!result)
	{
		sprintf(buf2, "calcuidall target not found '%s'", vnum);
		trig_log(trig, buf2);
		*uid = '\0';
		return;
	}
	add_var_cntx(&GET_TRIG_VARS(trig), varname, uid, 0);
}

/*
 * processes a script return command.
 * returns the new value for the script to return.
 */
int process_return(TRIG_DATA * trig, char *cmd)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

	two_arguments(cmd, arg1, arg2);

	if (!*arg2)
	{
		sprintf(buf2, "return w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return 1;
	}

	return atoi(arg2);
}


/*
 * removes a variable from the global vars of sc,
 * or the local vars of trig if not found in global list.
 */
void process_unset(SCRIPT_DATA * sc, TRIG_DATA * trig, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], *var;

	var = any_one_arg(cmd, arg);

	skip_spaces(&var);

	if (!*var)
	{
		sprintf(buf2, "unset w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	if (!remove_var_cntx(&worlds_vars, var, sc->context))
		if (!remove_var_cntx(&(sc->global_vars), var, sc->context))
			remove_var_cntx(&GET_TRIG_VARS(trig), var, 0);
}


/*
 * copy a locally owned variable to the globals of another script
 *     'remote <variable_name> <uid>'
 */
void process_remote(SCRIPT_DATA * sc, TRIG_DATA * trig, char *cmd)
{
	struct trig_var_data *vd;
	SCRIPT_DATA *sc_remote = NULL;
	char *line, *var, *uid_p;
	char arg[MAX_INPUT_LENGTH];
	long uid, context;
	room_data *room;
	CHAR_DATA *mob;
	OBJ_DATA *obj;

	line = any_one_arg(cmd, arg);
	two_arguments(line, buf, buf2);
	var = buf;
	uid_p = buf2;
	skip_spaces(&var);
	skip_spaces(&uid_p);


	if (!*buf || !*buf2)
	{
		sprintf(buf2, "remote: invalid arguments '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	// find the locally owned variable
	vd = find_var_cntx(&GET_TRIG_VARS(trig), var, 0);
	if (!vd)
		vd = find_var_cntx(&(sc->global_vars), var, sc->context);

	if (!vd)
	{
		sprintf(buf2, "local var '%s' not found in remote call", buf);
		trig_log(trig, buf2);
		return;
	}

	// find the target script from the uid number
	uid = atoi(buf2 + 1);
	if (uid <= 0)
	{
		sprintf(buf, "remote: illegal uid '%s'", buf2);
		trig_log(trig, buf);
		return;
	}

	// for all but PC's, context comes from the existing context.
	// for PC's, context is 0 (global)
//  context = vd->context;
// �������� ����� ����� ��� vd->context ��� sc->context
// ���� ����� vd->context, �� ������ �������� ��� �������� ��������� ����������
// ���� ����� sc->context, �� ��-����� ��������� ����������, � ������:
// ��� ��������� ���������� �������� �������� �� ������, �.�.
// ���� ���� ������� ��������� ��������� ���������� ������� ����������
// �������� � 0. ��� ���������� ���������� ���������� � ���������� 0
// "���������" ������������� ���������
	context = sc->context;

	if ((room = get_room(buf2)))
	{
		sc_remote = SCRIPT(room);
	}
	else if ((mob = get_char(buf2)))
	{
		sc_remote = SCRIPT(mob);
		if (!IS_NPC(mob))
			context = 0;
	}
	else if ((obj = get_obj(buf2)))
	{
		sc_remote = SCRIPT(obj);
	}
	else
	{
		sprintf(buf, "remote: uid '%ld' invalid", uid);
		trig_log(trig, buf);
		return;
	}

	if (sc_remote == NULL)
		return;		// no script to assign

	add_var_cntx(&(sc_remote->global_vars), vd->name, vd->value, context);
}


/*
 * command-line interface to rdelete
 * named vdelete so people didn't think it was to delete rooms
 */
ACMD(do_vdelete)
{
//  struct trig_var_data *vd, *vd_prev=NULL;
	SCRIPT_DATA *sc_remote = NULL;
	char *var, *uid_p;
	long uid; //, context;
	room_data *room;
	CHAR_DATA *mob;
	OBJ_DATA *obj;

	argument = two_arguments(argument, buf, buf2);
	var = buf;
	uid_p = buf2;
	skip_spaces(&var);
	skip_spaces(&uid_p);


	if (!*buf || !*buf2)
	{
		send_to_char("Usage: vdelete <variablename> <id>\r\n", ch);
		return;
	}


	// find the target script from the uid number
	uid = atoi(buf2 + 1);
	if (uid <= 0)
	{
		send_to_char("vdelete: illegal id specified.\r\n", ch);
		return;
	}


	if ((room = get_room(buf2)))
	{
		sc_remote = SCRIPT(room);
	}
	else if ((mob = get_char(buf2)))
	{
		sc_remote = SCRIPT(mob);
//		if (!IS_NPC(mob))
//			context = 0;
	}
	else if ((obj = get_obj(buf2)))
	{
		sc_remote = SCRIPT(obj);
	}
	else
	{
		send_to_char("vdelete: cannot resolve specified id.\r\n", ch);
		return;
	}

	if ((sc_remote == NULL) || (sc_remote->global_vars == NULL))
	{
		send_to_char("That id represents no global variables.\r\n", ch);
		return;
	}

	// find the global
	if (remove_var_cntx(&(sc_remote->global_vars), var, 0))
	{
		send_to_char("Deleted.\r\n", ch);
	}
	else
	{
		send_to_char("That variable cannot be located.\r\n", ch);
	}
}

/*
 * delete a variable from the globals of another script
 *     'rdelete <variable_name> <uid>'
 */
void process_rdelete(SCRIPT_DATA * sc, TRIG_DATA * trig, char *cmd)
{
//  struct trig_var_data *vd, *vd_prev=NULL;
	SCRIPT_DATA *sc_remote = NULL;
	char *line, *var, *uid_p;
	char arg[MAX_INPUT_LENGTH];
	long uid; //, context;
	room_data *room;
	CHAR_DATA *mob;
	OBJ_DATA *obj;

	line = any_one_arg(cmd, arg);
	two_arguments(line, buf, buf2);
	var = buf;
	uid_p = buf2;
	skip_spaces(&var);
	skip_spaces(&uid_p);



	if (!*buf || !*buf2)
	{
		sprintf(buf2, "rdelete: invalid arguments '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}


	// find the target script from the uid number
	uid = atoi(buf2 + 1);
	if (uid <= 0)
	{
		sprintf(buf, "rdelete: illegal uid '%s'", buf2);
		trig_log(trig, buf);
		return;
	}


	if ((room = get_room(buf2)))
	{
		sc_remote = SCRIPT(room);
	}
	else if ((mob = get_char(buf2)))
	{
		sc_remote = SCRIPT(mob);
//		if (!IS_NPC(mob))
//			context = 0;
	}
	else if ((obj = get_obj(buf2)))
	{
		sc_remote = SCRIPT(obj);
	}
	else
	{
		sprintf(buf, "remote: uid '%ld' invalid", uid);
		trig_log(trig, buf);
		return;
	}

	if (sc_remote == NULL)
		return;		// no script to delete a trigger from
	if (sc_remote->global_vars == NULL)
		return;		// no script globals

	// find the global
	remove_var_cntx(&(sc_remote->global_vars), var, sc->context);
}


// * makes a local variable into a global variable
void process_global(SCRIPT_DATA * sc, TRIG_DATA * trig, char *cmd, long id)
{
	struct trig_var_data *vd;
	char arg[MAX_INPUT_LENGTH], *var;

	var = any_one_arg(cmd, arg);

	skip_spaces(&var);

	if (!*var)
	{
		sprintf(buf2, "global w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	vd = find_var_cntx(&GET_TRIG_VARS(trig), var, 0);

	if (!vd)
	{
		sprintf(buf2, "local var '%s' not found in global call", var);
		trig_log(trig, buf2);
		return;
	}

	add_var_cntx(&(sc->global_vars), vd->name, vd->value, id);
	remove_var_cntx(&GET_TRIG_VARS(trig), vd->name, 0);
}


// * makes a local variable into a world variable
void process_worlds(SCRIPT_DATA * sc, TRIG_DATA * trig, char *cmd, long id)
{
	struct trig_var_data *vd;
	char arg[MAX_INPUT_LENGTH], *var;

	var = any_one_arg(cmd, arg);

	skip_spaces(&var);

	if (!*var)
	{
		sprintf(buf2, "worlds w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	vd = find_var_cntx(&GET_TRIG_VARS(trig), var, 0);

	if (!vd)
	{
		sprintf(buf2, "local var '%s' not found in worlds call", var);
		trig_log(trig, buf2);
		return;
	}

	add_var_cntx(&(worlds_vars), vd->name, vd->value, id);
	remove_var_cntx(&GET_TRIG_VARS(trig), vd->name, 0);
}




// set the current context for a script
void process_context(SCRIPT_DATA * sc, TRIG_DATA * trig, char *cmd)
{
	char arg[MAX_INPUT_LENGTH], *var;

	var = any_one_arg(cmd, arg);

	skip_spaces(&var);

	if (!*var)
	{
		sprintf(buf2, "context w/o an arg: '%s'", cmd);
		trig_log(trig, buf2);
		return;
	}

	sc->context = atol(var);
}

void extract_value(SCRIPT_DATA * sc, TRIG_DATA * trig, char *cmd)
{
	char buf2[MAX_INPUT_LENGTH];
	char *buf3;
	char to[128];
	int num;

	buf3 = any_one_arg(cmd, buf);
	half_chop(buf3, buf2, buf);
	strcpy(to, buf2);

	num = atoi(buf);
	if (num < 1)
	{
		trig_log(trig, "extract number < 1!");
		return;
	}

	half_chop(buf, buf3, buf2);

	while (num > 0)
	{
		half_chop(buf2, buf, buf2);
		num--;
	}

	add_var_cntx(&GET_TRIG_VARS(trig), to, buf, 0);
}

int dg_owner_purged;

//  This is the core driver for scripts.
//  define this if you want measure time of you scripts
#define TIMED_SCRIPT

#ifdef TIMED_SCRIPT
int timed_script_driver(void *go, TRIG_DATA * trig, int type, int mode);
int script_driver(void *go, TRIG_DATA * trig, int type, int mode)
{
	int i;
	TRIG_DATA ttrig;

	struct timeval start, stop, result;

	memcpy(&ttrig, trig, sizeof(TRIG_DATA));	// ������ �.�. ��� �� ������ ����� trig
	gettimeofday(&start, NULL);

	i = timed_script_driver(go, trig, type, mode);

	gettimeofday(&stop, NULL);
	timediff(&result, &stop, &start);

	if (result.tv_sec > 0 || result.tv_usec >= MAX_TRIG_USEC)
	{
		sprintf(buf, "[TrigVNum: %d] : ", GET_TRIG_VNUM(&ttrig));
		sprintf(buf + strlen(buf), "work time overflow %ld sec. %ld us.", result.tv_sec, result.tv_usec);
		mudlog(buf, BRF, -1, ERRLOG, TRUE);
	};
	// Stop time
	return i;
}

int timed_script_driver(void *go, TRIG_DATA * trig, int type, int mode)
#else
int script_driver(void *go, TRIG_DATA * trig, int type, int mode)
#endif
{
	static int depth = 0;
	int ret_val = 1, stop = FALSE;
	struct cmdlist_element *cl;
	char cmd[MAX_INPUT_LENGTH], *p, *orig_cmd;
	SCRIPT_DATA *sc = 0;
	struct cmdlist_element *temp;
	unsigned long loops = 0;
	TRIG_DATA *prev_trig;

	void obj_command_interpreter(OBJ_DATA * obj, char *argument);
	void wld_command_interpreter(ROOM_DATA * room, char *argument);

	sprintf(buf,
			"[%s] %s (VNUM=%d)",
			mode == TRIG_NEW ? "NEW" : "OLD", GET_TRIG_NAME(trig), GET_TRIG_VNUM(trig));
	mudlog(buf, BRF, -1, ERRLOG, TRUE);

	if (depth > MAX_SCRIPT_DEPTH)
	{
		trig_log(trig, "Triggers recursed beyond maximum allowed depth.");
		return ret_val;
	}

	prev_trig = cur_trig;
	cur_trig = trig;

	depth++;

	switch (type)
	{
	case MOB_TRIGGER:
		sc = SCRIPT((CHAR_DATA *) go);
		break;
	case OBJ_TRIGGER:
		sc = SCRIPT((OBJ_DATA *) go);
		break;
	case WLD_TRIGGER:
		sc = SCRIPT((ROOM_DATA *) go);
		break;
	}

	if (mode == TRIG_NEW)
	{
		GET_TRIG_DEPTH(trig) = 1;
		GET_TRIG_LOOPS(trig) = 0;
		sc->context = 0;
	}

	dg_owner_purged = 0;

	for (cl = (mode == TRIG_NEW) ? trig->cmdlist : trig->curr_state; !stop && cl && trig && GET_TRIG_DEPTH(trig); cl = cl ? cl->next : cl)  	//log("Drive go <%s>",cl->cmd);
	{
		for (p = cl->cmd; !stop && trig && *p && a_isspace(*p); p++);
		if (*p == '*')	// comment
			continue;
		else if (!strn_cmp(p, "if ", 3))
		{
			if (process_if(p + 3, go, sc, trig, type))
				GET_TRIG_DEPTH(trig)++;
			else
				cl = find_else_end(trig, cl, go, sc, type);
		}
		else if (!strn_cmp("elseif ", p, 7) || !strn_cmp("else", p, 4))
		{
			cl = find_end(trig, cl);
			GET_TRIG_DEPTH(trig)--;
		}
		else if (!strn_cmp("while ", p, 6))
		{
			temp = find_done(trig, cl);
			if (process_if(p + 6, go, sc, trig, type))
			{
				if (temp)
					temp->original = cl;
			}
			else
			{
				cl = temp;
				loops = 0;
			}
		}
		else if (!strn_cmp("foreach ", p, 8))
		{
			temp = find_done(trig, cl);
			if (process_foreach(p + 8, go, sc, trig, type))
			{
				if (temp)
					temp->original = cl;
			}
			else
			{
				cl = temp;
				loops = 0;
			}
		}
		else if (!strn_cmp("switch ", p, 7))
		{
			cl = find_case(trig, cl, go, sc, type, p + 7);
		}
		else if (!strn_cmp("end", p, 3))
		{
			GET_TRIG_DEPTH(trig)--;
		}
		else if (!strn_cmp("done", p, 4))
		{
			if (cl->original)
			{
				orig_cmd = cl->original->cmd;
				while (*orig_cmd && a_isspace(*orig_cmd))
					orig_cmd++;

				if ((*orig_cmd == 'w' && process_if(orig_cmd + 6, go, sc, trig, type))
						|| (*orig_cmd == 'f' && process_foreach(orig_cmd + 8, go, sc, trig, type)))
				{
					cl = cl->original;
					loops++;
					GET_TRIG_LOOPS(trig)++;
					if (loops == 30)
					{
						snprintf(buf2, MAX_STRING_LENGTH, "wait 1");
						process_wait(go, trig, type, buf2, cl);
						depth--;
						cur_trig = prev_trig;
						return ret_val;
					}
					if (GET_TRIG_LOOPS(trig) == 300)
						trig_log(trig, "looping 300 times.", LGH);
					if (GET_TRIG_LOOPS(trig) == 1000)
						trig_log(trig, "looping 1000 times.", DEF);
				}
			}
		}
		else if (!strn_cmp("break", p, 5))
		{
			cl = find_done(trig, cl);
		}
		else if (!strn_cmp("case", p, 4))  	// Do nothing, this allows multiple cases to a single instance
		{
		}
		else
		{
			var_subst(go, sc, trig, type, p, cmd);
			if (!strn_cmp(cmd, "eval ", 5))
				process_eval(go, sc, trig, type, cmd);
			else if (!strn_cmp(cmd, "nop ", 4));	// nop: do nothing
			else if (!strn_cmp(cmd, "extract ", 8))
				extract_value(sc, trig, cmd);
			else if (!strn_cmp(cmd, "makeuid ", 8))
				makeuid_var(go, sc, trig, type, cmd);
			else if (!strn_cmp(cmd, "calcuid ", 8))
				calcuid_var(go, sc, trig, type, cmd);
			else if (!strn_cmp(cmd, "calcuidall ", 11))
				calcuidall_var(go, sc, trig, type, cmd);
			else if (!strn_cmp(cmd, "charuid ", 8))
				charuid_var(go, sc, trig, cmd);
			else if (!strn_cmp(cmd, "halt", 4))
				break;
			else if (!strn_cmp(cmd, "dg_cast ", 8))
			{
				do_dg_cast(go, sc, trig, type, cmd);
				if (type == MOB_TRIGGER && reinterpret_cast<CHAR_DATA *>(go)->purged())
				{
					depth--;
					cur_trig = prev_trig;
					return ret_val;
				}
			}
			else if (!strn_cmp(cmd, "dg_affect ", 10))
				do_dg_affect(go, sc, trig, type, cmd);
			else if (!strn_cmp(cmd, "global ", 7))
				process_global(sc, trig, cmd, sc->context);
			else if (!strn_cmp(cmd, "worlds ", 7))
				process_worlds(sc, trig, cmd, sc->context);
			else if (!strn_cmp(cmd, "context ", 8))
				process_context(sc, trig, cmd);
			else if (!strn_cmp(cmd, "remote ", 7))
				process_remote(sc, trig, cmd);
			else if (!strn_cmp(cmd, "rdelete ", 8))
				process_rdelete(sc, trig, cmd);
			else if (!strn_cmp(cmd, "return ", 7))
				ret_val = process_return(trig, cmd);
			else if (!strn_cmp(cmd, "set ", 4))
				process_set(sc, trig, cmd);
			else if (!strn_cmp(cmd, "unset ", 6))
				process_unset(sc, trig, cmd);
			else if (!strn_cmp(cmd, "log ", 4))
			{
				trig_log(trig, cmd + 4);
			}
			else if (!strn_cmp(cmd, "wait ", 5))
			{
				process_wait(go, trig, type, cmd, cl);
				depth--;
				cur_trig = prev_trig;
				return ret_val;
			}
			else if (!strn_cmp(cmd, "attach ", 7))
				process_attach(go, sc, trig, type, cmd);
			else if (!strn_cmp(cmd, "detach ", 7))
			{
				trig = process_detach(go, sc, trig, type, cmd);
			}
			else if (!strn_cmp(cmd, "run ", 4))
			{
				process_run(go, &sc, &trig, type, cmd, &ret_val);
				if (!trig || !sc)
				{
					depth--;
					cur_trig = prev_trig;
					return ret_val;
				}
				stop = ret_val;
			}
			else if (!strn_cmp(cmd, "exec ", 5))
			{
				process_run(go, &sc, &trig, type, cmd, &ret_val);
				if (!trig || !sc)
				{
					depth--;
					cur_trig = prev_trig;
					return ret_val;
				}
			}
			else if (!strn_cmp(cmd, "version", 7))
			{
				mudlog(DG_SCRIPT_VERSION, BRF, LVL_BUILDER, SYSLOG, TRUE);
			}
			else
			{
//Polud ����� ��������� mpurge � mjunk �� command_interpreter.
//���� ����� ������� ���� - � ������ ������� �������� ����
//TODO: �������� mob_command_interpreter � ������ � ���� ��������� ���� mob-������.
// ������������ �������� ����, ��� ������ ����� �� free_varlist, ���� ������ ���� � ������ ����� �� dg_owner_purged -- Krodo
				if (!strn_cmp(cmd, "mpurge", 6))
					do_mpurge((CHAR_DATA *) go, cmd + 6, 0, 0);
				else if (!strn_cmp(cmd, "mjunk", 5))
					do_mjunk((CHAR_DATA *) go, cmd + 5, 0, 0);
				else
				{
					switch (type)
					{
					case MOB_TRIGGER:
						last_trig_vnum = GET_TRIG_VNUM(trig);
						command_interpreter((CHAR_DATA *) go, cmd);
						break;
					case OBJ_TRIGGER:
						last_trig_vnum = GET_TRIG_VNUM(trig);
						obj_command_interpreter((OBJ_DATA *) go, cmd);
						break;
					case WLD_TRIGGER:
						last_trig_vnum = GET_TRIG_VNUM(trig);
						wld_command_interpreter((ROOM_DATA *) go, cmd);
						break;
					}
				}
				if (dg_owner_purged || (type == MOB_TRIGGER && reinterpret_cast<CHAR_DATA *>(go)->purged()))
				{
					depth--;
					cur_trig = prev_trig;
					return ret_val;
				}
			}
		}
	}

	if (trig)
	{
		free_varlist(GET_TRIG_VARS(trig));
		GET_TRIG_VARS(trig) = NULL;
		GET_TRIG_DEPTH(trig) = 0;
	}
	depth--;
	cur_trig = prev_trig;
	return ret_val;
}

ACMD(do_tlist)
{

	int first, last, nr, found = 0;
	char pagebuf[65536];

	strcpy(pagebuf, "");

	two_arguments(argument, buf, buf2);

	if (!*buf)
	{
		send_to_char("Usage: tlist <begining number or zone> [<ending number>]\r\n", ch);
		return;
	}

	first = atoi(buf);
	if (*buf2)
		last = atoi(buf2);
	else
	{
		first *= 100;
		last = first + 99;
	}

	if ((first < 0) || (first > MAX_PROTO_NUMBER) || (last < 0) || (last > MAX_PROTO_NUMBER))
	{
		sprintf(buf, "�������� ������ ���� ����� 0 � %d.\n\r", MAX_PROTO_NUMBER);
		send_to_char(buf, ch);
	}

	if (first >= last)
	{
		send_to_char("Second value must be greater than first.\n\r", ch);
		return;
	}

	for (nr = 0; nr < top_of_trigt && (trig_index[nr]->vnum <= last); nr++)
	{
		if (trig_index[nr]->vnum >= first)
		{
			sprintf(buf, "%5d. [%5d] %s\r\n", ++found, trig_index[nr]->vnum, trig_index[nr]->proto->name);
			strcat(pagebuf, buf);
		}
	}

	if (!found)
		send_to_char("No triggers were found in those parameters.\n\r", ch);
	else
		page_string(ch->desc, pagebuf, TRUE);
}

int real_trigger(int vnum)
{
	int rnum;

	for (rnum = 0; rnum < top_of_trigt; rnum++)
	{
		if (trig_index[rnum]->vnum == vnum)
			break;
	}

	if (rnum == top_of_trigt)
		rnum = -1;
	return (rnum);
}

ACMD(do_tstat)
{
	int vnum, rnum;
	char str[MAX_INPUT_LENGTH];

	half_chop(argument, str, argument);
	if (*str)
	{
		vnum = atoi(str);
		rnum = real_trigger(vnum);
		if (rnum < 0)
		{
			send_to_char("That vnum does not exist.\r\n", ch);
			return;
		}

		do_stat_trigger(ch, trig_index[rnum]->proto);
	}
	else
		send_to_char("Usage: tstat <vnum>\r\n", ch);
}

// read a line in from a file, return the number of chars read
int fgetline(FILE * file, char *p)
{
	int count = 0;

	do
	{
		*p = fgetc(file);
		if (*p != '\n' && !feof(file))
		{
			p++;
			count++;
		}
	}
	while (*p != '\n' && !feof(file));

	if (*p == '\n')
		*p = '\0';

	return count;
}


// load in a character's saved variables
void read_saved_vars(CHAR_DATA * ch)
{
	FILE *file;
	long context;
	char fn[127];
	char input_line[1024], *p;
	char varname[32], *v;
	char context_str[16], *c;

	// create the space for the script structure which holds the vars
	CREATE(SCRIPT(ch), SCRIPT_DATA, 1);

	// find the file that holds the saved variables and open it
	get_filename(GET_NAME(ch), fn, SCRIPT_VARS_FILE);
	file = fopen(fn, "r");

	// if we failed to open the file, return
	if (!file)
		return;

	// walk through each line in the file parsing variables
	do
	{
		if (fgetline(file, input_line) > 0)
		{
			p = input_line;
			v = varname;
			c = context_str;
			skip_spaces(&p);
			while (*p && *p != ' ' && *p != '\t')
				*v++ = *p++;
			*v = '\0';
			skip_spaces(&p);
			while (*p && *p != ' ' && *p != '\t')
				*c++ = *p++;
			*c = '\0';
			skip_spaces(&p);

			context = atol(context_str);
			add_var_cntx(&(SCRIPT(ch)->global_vars), varname, p, context);
		}
	}
	while (!feof(file));

	// close the file and return
	fclose(file);
}

// save a characters variables out to disk
void save_char_vars(CHAR_DATA * ch)
{
	FILE *file;
	char fn[127];
	struct trig_var_data *vars;

	// immediate return if no script (and therefore no variables) structure
	// has been created. this will happen when the player is logging in
	if (SCRIPT(ch) == NULL)
		return;

	// we should never be called for an NPC, but just in case...
	if (IS_NPC(ch))
		return;

	get_filename(GET_NAME(ch), fn, SCRIPT_VARS_FILE);
	std::remove(fn);

	// make sure this char has global variables to save
	if (ch->script->global_vars == NULL)
		return;
	vars = ch->script->global_vars;

	file = fopen(fn, "wt");

	// note that currently, context will always be zero. this may change
	// in the future
	while (vars)
	{
		if (*vars->name != '-')	// don't save if it begins with -
			fprintf(file, "%s %ld %s\n", vars->name, vars->context, vars->value);
		vars = vars->next;
	}

	fclose(file);
}
