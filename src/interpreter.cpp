/* ************************************************************************
*   File: interpreter.cpp                               Part of Bylins    *
*  Usage: parse user commands, search for specials, call ACMD functions   *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
*                                                                         *
*  $Author$                                                        *
*  $Date$                                           *
*  $Revision$                                                       *
************************************************************************ */

//#define PROOLDEBUG
//#define PROOLDEBUG2

//#define I3

#define __INTERPRETER_C__

#include "conf.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "sysdep.h"
#include "structs.h"
#include "comm.h"
#include "interpreter.h"
#include "constants.h"
#include "db.h"
#include "utils.h"
#include "spells.h"
#include "skills.h"
#include "handler.h"
#include "house.h"
#include "mail.h"
#include "screen.h"
#include "olc.h"
#include "dg_scripts.h"
#include "pk.h"
#include "genchar.h"
#include "ban.hpp"
#include "item.creation.hpp"
#include "features.hpp"
#include "boards.h"
#include "top.h"
#include "title.hpp"
#include "password.hpp"
#include "privilege.hpp"
#include "depot.hpp"
#include "glory.hpp"
#include "char.hpp"
#include "char_player.hpp"
#include "parcel.hpp"
#include "liquid.hpp"
#include "name_list.hpp"
#include "modify.h"
#include "room.hpp"
#include "glory_const.hpp"
#include "glory_misc.hpp"
#include "named_stuff.hpp"
//python_off #include "scripting.hpp"
#include "player_races.hpp"
#include "birth_places.hpp"
#include "help.hpp"
#include "map.hpp"
#include "ext_money.hpp"
#include "noob.hpp"
#include "reset_stats.hpp"
#include "obj_sets.hpp"

#ifdef I3
#include "i3.h"
#endif

#include "virtustan.h" // prool

extern room_rnum r_mortal_start_room;
extern room_rnum r_immort_start_room;
extern room_rnum r_frozen_start_room;
extern room_rnum r_helled_start_room;
extern room_rnum r_named_start_room;
extern room_rnum r_unreg_start_room;
extern const char *class_menu;
extern const char *class_menu_vik;
extern const char *class_menu_step;
extern const char *religion_menu;
extern const char *color_menu;
extern char *motd;
extern char *rules;
extern char *background;
extern const char *MENU;
extern const char *WELC_MESSG;
extern const char *START_MESSG;
extern CHAR_DATA *character_list;
extern DESCRIPTOR_DATA *descriptor_list;
extern struct player_index_element *player_table;
extern int top_of_p_table;
extern int circle_restrict;
extern int no_specials;
extern int max_bad_pws;
extern INDEX_DATA *mob_index;
extern INDEX_DATA *obj_index;

extern struct pclean_criteria_data pclean_criteria[];

extern char *GREETINGS;
extern const char *pc_class_types[];
extern const char *pc_class_types_vik[];
extern const char *pc_class_types_step[];
//extern const char *race_types[];
extern const char *race_types_step[];
extern const char *race_types_vik[];
extern const char *kin_types[];
extern struct set_struct set_fields[];
extern struct show_struct show_fields[];
extern BanList *ban;
extern char *name_rules;

// prool:
extern int total_players;
extern int send_email;
extern char email_notif[];
extern char noname[MAX_NONAME][PROOL_MAX_STRLEN];

// external functions
void do_start(CHAR_DATA * ch, int newbie);
int parse_class(char arg);
int parse_class_vik(char arg);
int parse_class_step(char arg);
int Valid_Name(char *newname);
int Is_Valid_Name(char *newname);
int Is_Valid_Dc(char *newname);
void read_aliases(CHAR_DATA * ch);
void read_saved_vars(CHAR_DATA * ch);
void oedit_parse(DESCRIPTOR_DATA * d, char *arg);
void redit_parse(DESCRIPTOR_DATA * d, char *arg);
void zedit_parse(DESCRIPTOR_DATA * d, char *arg);
void medit_parse(DESCRIPTOR_DATA * d, char *arg);
void trigedit_parse(DESCRIPTOR_DATA * d, char *arg);
void Crash_timer_obj(int index, long timer_dec);
int find_social(char *name);
int calc_loadroom(CHAR_DATA * ch, int bplace_mode = BIRTH_PLACE_UNDEFINED);
void do_aggressive_room(CHAR_DATA * ch, int check_sneak);
extern int process_auto_agreement(DESCRIPTOR_DATA * d);
extern int CheckProxy(DESCRIPTOR_DATA * ch);
extern void NewNameShow(CHAR_DATA * ch);
extern void NewNameAdd(CHAR_DATA * ch, bool save = 1);
extern void check_max_hp(CHAR_DATA *ch);

// local functions
int perform_dupe_check(DESCRIPTOR_DATA * d);
struct alias_data *find_alias(struct alias_data *alias_list, char *str);
void free_alias(struct alias_data *a);
void perform_complex_alias(struct txt_q *input_q, char *orig, struct alias_data *a);
int perform_alias(DESCRIPTOR_DATA * d, char *orig);
int reserved_word(const char *argument);
int _parse_name(char *arg, char *name);
void add_logon_record(DESCRIPTOR_DATA * d);
// prototypes for all do_x functions.
int find_action(char *cmd);
int do_social(CHAR_DATA * ch, char *argument);
void single_god_invoice(CHAR_DATA* ch);
void login_change_invoice(CHAR_DATA* ch);

ACMD(do_advance);
ACMD(do_alias);
ACMD(do_antigods);
ACMD(do_assist);
ACMD(do_at);
ACMD(do_affects);
ACMD(do_backstab);
ACMD(do_ban);
ACMD(do_bash);
ACMD(do_beep);
ACMD(do_cast);
ACMD(do_warcry);
ACMD(do_clanstuff);
ACMD(do_create);
ACMD(do_mixture);
ACMD(do_color);
ACMD(do_courage);
ACMD(do_commands);
ACMD(do_consider);
ACMD(do_credits);
ACMD(do_date);
ACMD(do_dc);
ACMD(do_diagnose);
ACMD(do_display);
ACMD(do_drink);
ACMD(do_drunkoff);
ACMD(do_features);
ACMD(do_featset);
ACMD(do_findhelpee);
ACMD(do_firstaid);
ACMD(do_fire);
ACMD(do_drop);
ACMD(do_eat);
ACMD(do_echo);
//python_off ACMD(do_email);
ACMD(do_enter);
ACMD(do_manadrain);
ACMD(do_equipment);
ACMD(do_examine);
ACMD(do_revenge);
ACMD(do_remort);
ACMD(do_remember_char);
ACMD(do_exit);
ACMD(do_exits);
ACMD(do_flee);
ACMD(do_follow);
ACMD(do_horseon);
ACMD(do_horseoff);
ACMD(do_horseput);
ACMD(do_horseget);
ACMD(do_horsetake);
ACMD(do_hidetrack);
ACMD(do_hidemove);
ACMD(do_fit);
ACMD(do_force);
ACMD(do_extinguish);
ACMD(do_forcetime);
ACMD(do_glory);
ACMD(do_gecho);
ACMD(do_gen_comm);
ACMD(do_mobshout);
ACMD(do_gen_door);
ACMD(do_gen_ps);
ACMD(do_get);
ACMD(do_give);
ACMD(do_givehorse);
ACMD(do_gold);
ACMD(do_goto);
ACMD(do_grab);
ACMD(do_group);
ACMD(do_gsay);
ACMD(do_hide);
ACMD(do_hit);
ACMD(do_info);
ACMD(do_inspect);
ACMD(do_insult);
ACMD(do_inventory);
ACMD(do_invis);
ACMD(do_kick);
ACMD(do_kill);
ACMD(do_last);
ACMD(do_mode);
ACMD(do_mark);
ACMD(do_makefood);
ACMD(do_name);
ACMD(do_disarm);
ACMD(do_chopoff);
ACMD(do_deviate);
ACMD(do_levels);
ACMD(do_liblist);
ACMD(do_lightwalk);
ACMD(do_load);
ACMD(do_look);
ACMD(do_sides);
ACMD(do_not_here);
ACMD(do_offer);
ACMD(do_olc);
ACMD(do_order);
ACMD(do_page);
ACMD(do_pray);
ACMD(do_poofset);
ACMD(do_pour);
ACMD(do_skills);
ACMD(do_statistic);
ACMD(do_spells);
ACMD(do_spellstat);
ACMD(do_remember);
ACMD(do_learn);
ACMD(do_forget);
ACMD(do_purge);
ACMD(do_put);
ACMD(do_quit);
ACMD(do_reboot);
ACMD(do_remove);
ACMD(do_rent);
ACMD(do_reply);
ACMD(do_report);
ACMD(do_rescue);
ACMD(do_stopfight);
ACMD(do_stophorse);
ACMD(do_rest);
ACMD(do_restore);
ACMD(do_return);
ACMD(do_save);
ACMD(do_say);
ACMD(do_score);
ACMD(do_send);
ACMD(do_set);
ACMD(do_show);
ACMD(do_shutdown);
ACMD(do_sit);
ACMD(do_skillset);
ACMD(do_sleep);
ACMD(do_sneak);
ACMD(do_snoop);
ACMD(do_spec_comm);
ACMD(do_spell_capable);
ACMD(do_split);
ACMD(do_stand);
ACMD(do_stat);
ACMD(do_steal);
ACMD(do_switch);
ACMD(do_syslog);
ACMD(do_throw);
ACMD(do_teleport);
ACMD(do_tell);
ACMD(do_time);
ACMD(do_toggle);
ACMD(do_track);
ACMD(do_sense);
ACMD(do_unban);
ACMD(do_ungroup);
ACMD(do_use);
ACMD(do_users);
ACMD(do_visible);
ACMD(do_vnum);
ACMD(do_vstat);
ACMD(do_wake);
ACMD(do_wear);
ACMD(do_weather);
ACMD(do_where);
ACMD(do_who);
ACMD(do_wield);
ACMD(do_wimpy);
ACMD(do_wizlock);
ACMD(do_wiznet);
ACMD(do_wizutil);
ACMD(do_write);
ACMD(do_zreset);
ACMD(do_parry);
ACMD(do_multyparry);
ACMD(do_style);
ACMD(do_poisoned);
ACMD(do_repair);
ACMD(do_camouflage);
ACMD(do_stupor);
ACMD(do_mighthit);
ACMD(do_block);
ACMD(do_touch);
ACMD(do_transform_weapon);
ACMD(do_protect);
ACMD(do_dig);
ACMD(do_insertgem);
ACMD(do_ignore);
ACMD(do_proxy);
ACMD(do_turn_undead);
ACMD(do_iron_wind);
ACMD(do_exchange);
ACMD(do_godtest);
ACMD(do_print_armor);
ACMD(do_relocate);
ACMD(do_strangle);
ACMD(do_custom_label);

// DG Script ACMD's
ACMD(do_attach);
ACMD(do_detach);
ACMD(do_tlist);
ACMD(do_tstat);
ACMD(do_masound);
ACMD(do_mkill);
ACMD(do_mjunk);
ACMD(do_mdamage);
ACMD(do_mdoor);
ACMD(do_mechoaround);
ACMD(do_msend);
ACMD(do_mecho);
ACMD(do_mload);
ACMD(do_mpurge);
ACMD(do_mgoto);
ACMD(do_mat);
ACMD(do_mteleport);
ACMD(do_mforce);
ACMD(do_mexp);
ACMD(do_mgold);
ACMD(do_mremember);
ACMD(do_mforget);
ACMD(do_mfeatturn);
ACMD(do_mtransform);
ACMD(do_mskillturn);
ACMD(do_mskilladd);
ACMD(do_mspellturn);
ACMD(do_mspelladd);
ACMD(do_mspellitem);
ACMD(do_vdelete);
ACMD(do_hearing);
ACMD(do_looking);
ACMD(do_ident);
ACMD(do_upgrade);
ACMD(do_armored);
ACMD(do_recall);
ACMD(do_pray_gods);
ACMD(do_rset);
ACMD(do_recipes);
ACMD(do_cook);
ACMD(do_forgive);
ACMD(do_imlist);
ACMD(do_townportal);
ACMD(DoBoard);
ACMD(DoBoardList);
ACMD(DoHouse);
ACMD(DoClanChannel);
ACMD(DoClanList);
ACMD(DoShowPolitics);
ACMD(DoShowWars);
ACMD(DoHcontrol);
ACMD(DoWhoClan);
ACMD(DoClanPkList);
ACMD(DoStoreHouse);
ACMD(do_clanstuff);
ACMD(DoBest);
ACMD(do_offtop);
ACMD(do_dmeter);
ACMD(do_mystat);
ACMD(do_zone);
ACMD(do_bandage);
ACMD(do_sanitize);
ACMD(do_morph);
ACMD(do_morphset);
//python_off ACMD(do_console);
ACMD(do_shops_list);

// prool:
ACMD(do_kogda);
ACMD(do_igroki);
ACMD(do_duhmada);
ACMD(do_host); 
ACMD(do_whois); 
ACMD(do_system); 
ACMD(do_fflush);
ACMD(do_build);
ACMD(do_konsole);
ACMD(do_room_title);
ACMD(do_room_descr);
ACMD(do_room_type);
ACMD(do_room_flag);
ACMD(do_newpass);
ACMD(do_virtustan);
ACMD(do_fish);

/* This is the Master Command List(tm).

 * You can put new commands in, take commands out, change the order
 * they appear in, etc.  You can adjust the "priority" of commands
 * simply by changing the order they appear in the command list.
 * (For example, if you want "as" to mean "assist" instead of "ask",
 * just put "assist" above "ask" in the Master Command List(tm).
 *
 * In general, utility commands such as "at" should have high priority;
 * infrequently used and dangerously destructive commands should have low
 * priority.
 */

// ���� ������� ����� ��������� � ����� (�� �� ��������) �� ��������� ��������:
// - ��� ����� ����� ������� ��������� ��� ����� ����������� ������,
// � ���� ��� ������: ������ �������� ��� � ��� ���������, � � ������� �� ��������
// ������� ����� � ��� ����� ��������������� ����� �. � ������� ��������� ��������
// ������� � ���-�������.
// - �� ����� ����� ��� ������������� ����������� ����
// - ����� ��� ����������� �� ���������� �� ����, � ������� � ����� ����� �����, ��� ���������
// ������� � �������� ������ (������ ���������� ����������, ������� ����� ���������� ��������� �������).
// prool

#define MAGIC_NUM 419
#define MAGIC_LEN 8

cpp_extern const struct command_info cmd_info[] =
{
	{"RESERVED", 0, 0, 0, 0, 0},	// this must be first -- for specprocs

	// directions must come before other commands but after RESERVED
	{"�����", POS_STANDING, do_move, 0, SCMD_NORTH, -2},
	{"������", POS_STANDING, do_move, 0, SCMD_EAST, -2},
	{"��", POS_STANDING, do_move, 0, SCMD_SOUTH, -2},
	{"�����", POS_STANDING, do_move, 0, SCMD_WEST, -2},
	{"�����", POS_STANDING, do_move, 0, SCMD_UP, -2},
	{"����", POS_STANDING, do_move, 0, SCMD_DOWN, -2},
	{"north", POS_STANDING, do_move, 0, SCMD_NORTH, -2},
	{"east", POS_STANDING, do_move, 0, SCMD_EAST, -2},
	{"south", POS_STANDING, do_move, 0, SCMD_SOUTH, -2},
	{"west", POS_STANDING, do_move, 0, SCMD_WEST, -2},
	{"up", POS_STANDING, do_move, 0, SCMD_UP, -2},
	{"down", POS_STANDING, do_move, 0, SCMD_DOWN, -2},

	{"�������", POS_DEAD, do_affects, 0, SCMD_AUCTION, 0},
	{"������", POS_DEAD, do_gen_ps, 0, SCMD_CREDITS, 0},
	{"���������", POS_FIGHTING, do_hit, 0, SCMD_MURDER, -1},
	{"�������", POS_RESTING, do_gen_comm, 0, SCMD_AUCTION, 100},
	{"������", POS_DEAD, DoBoard, 1, Boards::NOTICE_BOARD, -1},

	{"�����", POS_RESTING, do_exchange, 1, 0, -1},
	{"������", POS_STANDING, do_not_here, 1, 0, 0},
	{"����", POS_DEAD, DoBoard, 1, Boards::ERROR_BOARD, 0},
	{"������", POS_FIGHTING, do_flee, 1, 0, -1},
	{"���������", POS_RESTING, do_bandage, 0, 0, 0},
	{"������", POS_DEAD, DoBoard, 1,Boards:: GODBUILD_BOARD, -1},
	{"����", POS_FIGHTING, do_block, 0, 0, -1},
	{"�������", POS_DEAD, DoBoard, 1, Boards::PERS_BOARD, -1},
	{"����", POS_DEAD, do_gen_ps, 0, SCMD_IMMLIST, 0},
	{"��������", POS_DEAD, DoBoard, 1, Boards::GODGENERAL_BOARD, -1},
	{"�������", POS_RESTING, do_gen_comm, 0, SCMD_GOSSIP, -1},
	{"�������", POS_RESTING, do_drop, 0, SCMD_DROP, -1},

	{"������", POS_RESTING, do_cook, 0, 0, 200},
	{"������", POS_DEAD, do_gen_ps, 0, SCMD_VERSION, 0},
	{"����", POS_DEAD, DoBoard, 1, Boards::GENERAL_BOARD, -1},
	{"�����", POS_RESTING, do_get, 0, 0, 200},
	{"���������", POS_RESTING, do_diagnose, 0, 0, 100},
	{"��������", POS_STANDING, do_gen_door, 1, SCMD_PICK, -1},
	{"�����", POS_FIGHTING, do_iron_wind, 0, 0, -1},
	{"�������", POS_STANDING, do_not_here, 1, 0, -1},
	{"�������", POS_STANDING, do_not_here, 0, 0, -1},
	{"���������", POS_DEAD, do_return, 0, 0, -1},
	{"�����", POS_STANDING, do_enter, 0, 0, -2},
	{"�����", POS_RESTING, DoShowWars, 0, 0, 0},
	{"�����������", POS_RESTING, do_wield, 0, 0, 200},
	{"�������", POS_RESTING, do_recall, 0, 0, -1},
	{"��������", POS_DEAD, do_pray_gods, 0, 0, -1},
	{"��������", POS_STANDING, do_insertgem, 0, SKILL_INSERTGEM, -1},
	{"�����", POS_DEAD, do_time, 0, 0, 0},
	{"�����", POS_SITTING, do_townportal, 1, 0, -1},
	{"��������", POS_FIGHTING, do_horseon, 0, 0, 500},
	{"������", POS_RESTING, do_stand, 0, 0, 500},
	{"���������", POS_DEAD, do_remember_char, 0, 0, 0},
	{"���������", POS_RESTING, do_drop, 0, 0 /*SCMD_DONATE */ , 300},
	{"���������", POS_STANDING, do_track, 0, 0, 500},
	{"������", POS_STANDING, do_pour, 0, SCMD_POUR, 500},
	{"������", POS_RESTING, do_exits, 0, 0, 0},

	{"��������", POS_RESTING, do_say, 0, 0, -1},
	{"�������", POS_SLEEPING, do_gsay, 0, 0, 500},
	{"���������", POS_SLEEPING, do_gsay, 0, 0, 500},
	{"�������", POS_SLEEPING, DoClanChannel, 0, SCMD_CHANNEL, 0},
	{"���", POS_RESTING, do_where, LVL_IMMORT, 0, 0},
	{"����", POS_RESTING, do_zone, 0, 0, 0},
	{"������", POS_RESTING, do_drink, 0, SCMD_SIP, 200},
	{"������", POS_SLEEPING, do_group, 1, 0, -1},
	{"����������", POS_SLEEPING, DoClanChannel, 0, SCMD_ACHANNEL, 0},
	{"����", POS_DEAD, do_gecho, LVL_GOD, 0, 0},
	{"������", POS_DEAD, do_wiznet, LVL_IMMORT, 0, 0},

	{"����", POS_RESTING, do_give, 0, 0, 500},
	{"����", POS_DEAD, do_date, 0, SCMD_DATE, 0},
	{"������", POS_RESTING, do_split, 1, 0, 200},
	{"�������", POS_RESTING, do_grab, 0, 0, 300},
	{"�����", POS_DEAD, do_dmeter, 0, 0, 0},
	{"��������", POS_RESTING, do_report, 0, 0, 500},
	{"�����", POS_DEAD, DoBoardList, 0, 0, 0},
	{"�������", POS_DEAD, DoClanList, 0, 0, 0},
	{"���������", POS_DEAD, DoBoard, 1, Boards::CLANNEWS_BOARD, -1},
	{"������", POS_DEAD, DoBoard, 1, Boards::CLAN_BOARD, -1},
	{"������", POS_DEAD, DoClanPkList, 0, 1, 0},
	{"�������", POS_DEAD, do_duhmada, 0, 1, 0}, // prool

	{"����", POS_RESTING, do_eat, 0, SCMD_EAT, 500},

	{"����������", POS_STANDING, do_pray, 1, SCMD_DONATE, -1},

	{"��������", POS_STANDING, do_backstab, 1, 0, 1},
	{"������", POS_RESTING, do_forget, 0, 0, 0},
	{"���������", POS_STANDING, do_not_here, 1, 0, -1},
	{"����������", POS_SLEEPING, do_spells, 0, 0, 0},
	{"��������", POS_DEAD, do_spellstat, LVL_GRGOD, 0, 0},
	{"�������", POS_SITTING, do_gen_door, 0, SCMD_CLOSE, 500},
	{"�������", POS_STANDING, do_hidetrack, 1, 0, -1},
	{"���������", POS_DEAD, do_wizutil, LVL_GOD, SCMD_MUTE, 0},
	{"����������", POS_DEAD, do_wizutil, LVL_FREEZE, SCMD_FREEZE, 0},
	{"���������", POS_RESTING, do_remember, 0, 0, 0},
	{"��������", POS_SITTING, do_gen_door, 0, SCMD_LOCK, 500},
	{"������", POS_DEAD, do_ban, LVL_GRGOD, 0, 0},
	{"�������", POS_SLEEPING, do_sleep, 0, 0, -1},
	{"��������", POS_DEAD, do_gen_ps, 0, SCMD_MOTD, 0},
	{"���������", POS_SLEEPING, do_force, LVL_GRGOD, 0, 0},
	{"���������", POS_STANDING, do_extinguish, 0, 0, 0},
	{"��������", POS_RESTING, do_upgrade, 0, 0, 500},
	{"�������", POS_RESTING, do_remember, 0, 0, 0},
	{"��������", POS_RESTING, do_use, 0, SCMD_RECITE, 500},
	{"����������", POS_STANDING, do_spell_capable, 1, 0, 0},
	{"���������", POS_DEAD, do_sanitize, LVL_GRGOD, 0, 0},
	{"������", POS_RESTING, do_gold, 0, 0, 0},
	{"����", POS_RESTING, do_zone, 0, 0, 0},

	{"���������", POS_SLEEPING, do_inventory, 0, 0, 0},
	{"������������", POS_DEAD, do_ignore, 0, 0, 0},
	{"������", POS_DEAD, do_igroki, 0, 0, 0}, // prool
	{"����", POS_DEAD, DoBoard, 1, Boards::IDEA_BOARD, 0},
	{"������� ������", POS_RESTING, do_turn_undead, 0, 0, -1},
	{"�������", POS_SITTING, do_learn, 0, 0, 0},
	{"����������", POS_SLEEPING, do_gen_ps, 0, SCMD_INFO, 0},
	{"������", POS_RESTING, do_use, 0, SCMD_QUAFF, 500},
	{"���", POS_SLEEPING, do_name, LVL_IMMORT, 0, 0},

	{"���������", POS_SITTING, do_cast, 1, 0, -1},
	{"�����", POS_RESTING, do_not_here, 1, 0, 0},
	{"�����", POS_RESTING, do_map, 0, 0, 0},
	{"����", POS_RESTING, DoHouse, 0, 0, 0},
	{"����", POS_FIGHTING, do_warcry, 1, 0, -1},
	{"�����", POS_DEAD, DoBoard, 1, Boards::CODER_BOARD, -1},
	{"�������", POS_DEAD, do_commands, 0, SCMD_COMMANDS, 0},
	{"����", POS_SLEEPING, do_quit, 0, 0, 0},
	{"�����", POS_SLEEPING, do_quit, 0, SCMD_QUIT, 0},
	{"������", POS_STANDING, do_dig, 0, SKILL_DIG, -1},
	{"��������", POS_STANDING, do_hidemove, 1, 0, -2},
	{"�������", POS_RESTING, do_gen_comm, 0, SCMD_SHOUT, -1},
	{"���", POS_RESTING, do_who, 0, 0, 0},
	{"����������", POS_RESTING, DoWhoClan, 0, 0, 0},
	{"����", POS_DEAD, do_gen_ps, 0, SCMD_WHOAMI, 0},
	{"������", POS_STANDING, do_not_here, 0, 0, -1},

	{"���������", POS_RESTING, do_grab, 1, 0, 300},
	{"������", POS_STANDING, do_firstaid, 0, 0, -1},
	{"����", POS_STANDING, do_pour, 0, SCMD_POUR, 500},
	{"������", POS_STANDING, do_not_here, 1, 0, -1},
	{"������", POS_DEAD, DoBest, 0, 0, 0},

	{"����������", POS_RESTING, do_camouflage, 0, 0, 500},
	{"��������", POS_DEAD, do_shops_list, LVL_IMMORT, 0, 0},
	{"�������", POS_FIGHTING, do_throw, 0, 0, -1},
	{"������", POS_STANDING, do_not_here, 0, 0, -1},
	{"�����", POS_RESTING, do_revenge, 0, 0, 0},
	{"�����", POS_FIGHTING, do_mighthit, 0, 0, -1},
	{"��������", POS_STANDING, do_pray, 1, SCMD_PRAY, -1},
	{"�������������", POS_DEAD, do_mystat, 0, 0, 0},
	{"����", POS_DEAD, do_quit, 0, 0, 0},
	{"�����", POS_DEAD, report_on_board, 0, Boards::SUGGEST_BOARD, 0},

	{"������", POS_STANDING, do_not_here, 1, 0, -1},
	{"���������", POS_DEAD, DoBoard, 1, Boards::GODPUNISH_BOARD, -1},
	{"������", POS_STANDING, do_pour, 0, SCMD_FILL, 500},
	{"���������", POS_STANDING, do_pour, 0, SCMD_FILL, 500},
	{"�����", POS_STANDING, do_sense, 0, 0, 500},
	{"������", POS_STANDING, do_findhelpee, 0, SCMD_BUYHELPEE, -1},
	{"�������", POS_SLEEPING, do_gen_ps, 0, SCMD_INFO, 0},
	{"�������", POS_DEAD, DoBoard, 1, Boards::NEWS_BOARD, -1},
	{"������", POS_RESTING, do_wear, 0, 0, 500},
	{"����������", POS_RESTING, do_custom_label, 0, 0, 0},

	{"�����������", POS_FIGHTING, do_disarm, 0, 0, -1},
	{"����������", POS_STANDING, do_morph, 0, 0, -1},
	{"��������", POS_RESTING, do_wear, 0, 0, 500},
	{"�����", POS_STANDING, do_not_here, 0, 0, 0},
	{"��������", POS_STANDING, do_not_here, 0, 0, 0},
	{"����������", POS_RESTING, do_sides, 0, 0, 0},
	{"��������", POS_FIGHTING, do_stupor, 0, 0, -1},
	{"�����", POS_RESTING, do_wear, 0, 0, 500},
	{"��������", POS_RESTING, do_ident, 0, 0, 500},
	{"������������", POS_RESTING, do_drunkoff, 0, 0, -1},
	{"�������", POS_DEAD, do_quit, 0, 0, 0},
	{"��������", POS_DEAD, report_on_board, 0, Boards::MISPRINT_BOARD, 0},
	{"��������", POS_RESTING, do_put, 0, 0, 500},
	{"�����", POS_RESTING, do_gen_comm, 1, SCMD_HOLLER, -1},
	{"���������", POS_RESTING, do_examine, 0, 0, 0},
	{"��������", POS_STANDING, do_horsetake, 1, 0, -1},
	{"���������", POS_RESTING, do_insult, 0, 0, -1},
	{"�������", POS_RESTING, do_use, 0, SCMD_QUAFF, 300},
	{"����������", POS_STANDING, do_makefood, 0, 0, -1},
	{"��������", POS_RESTING, do_reply, 0, 0, -1},
	{"��������", POS_FIGHTING, do_multyparry, 0, 0, -1},
	{"��������", POS_DEAD, do_horseget, 0, 0, -1},
	{"���������", POS_RESTING, do_rest, 0, 0, -1},
	{"�������", POS_SITTING, do_gen_door, 0, SCMD_OPEN, 500},
	{"��������", POS_SITTING, do_gen_door, 0, SCMD_UNLOCK, 500},
	{"���������", POS_SITTING, do_stophorse, 0, 0, -1},
	{"��������", POS_FIGHTING, do_poisoned, 0, 0, -1},
	{"��������", POS_RESTING, do_antigods, 1, 0, -1},
	{"���������", POS_FIGHTING, do_stopfight, 1, 0, -1},
	{"���������", POS_STANDING, do_not_here, 1, 0, -1},
	{"������", POS_DEAD, do_offtop, 0, 0, -1},
	{"�������", POS_STANDING, do_not_here, 0, 0, 500},
	{"����", POS_DEAD, do_score, 0, 0, 0},
	{"��������", POS_DEAD, DoBoard, 1, Boards::MISPRINT_BOARD, 0},
	{"��������", POS_DEAD, do_not_here, 0, SCMD_CLEAR, -1},
	{"�����", POS_DEAD, do_quit, 0, 0, 0},
	{"������", POS_DEAD, report_on_board, 0, Boards::ERROR_BOARD, 0},

	{"����������", POS_FIGHTING, do_parry, 0, 0, -1},
	{"�����������", POS_FIGHTING, do_touch, 0, 0, -1},
	{"����������", POS_STANDING, do_transform_weapon, 0, SKILL_TRANSFORMWEAPON, -1},
	{"��������", POS_STANDING, do_givehorse, 0, 0, -1},
	{"���������", POS_STANDING, do_not_here, 1, 0, -1},
	{"�������������", POS_STANDING, do_relocate, 1, 0, 0},
	//python_off {"�������", POS_DEAD, do_email, LVL_IMPL, 0, 0},
	{"��������������", POS_STANDING, do_remort, 0, 0, -1},
	{"���������������", POS_STANDING, do_remort, 0, 1, -1},
	{"��������", POS_STANDING, do_pour, 0, SCMD_POUR, 500},
	{"��������", POS_RESTING, do_fit, 0, SCMD_MAKE_OVER, 500},
	{"����", POS_RESTING, do_drink, 0, SCMD_DRINK, 400},
	{"������", POS_STANDING, do_write, 1, 0, -1},
	{"������", POS_SLEEPING, DoClanPkList, 0, 0, 0},
	{"�����", POS_FIGHTING, do_kick, 1, 0, -1},
	{"������", POS_RESTING, do_weather, 0, 0, 0},
	{"�����������", POS_STANDING, do_sneak, 1, 0, 500},
	{"��������", POS_FIGHTING, do_chopoff, 0, 0, 500},
	{"���������", POS_RESTING, do_stand, 0, 0, -1},
	{"����������", POS_RESTING, do_bandage, 0, 0, 0},
	{"����������", POS_RESTING, do_fit, 0, SCMD_DO_ADAPT, 500},
	{"�����������", POS_RESTING, do_look, 0, SCMD_LOOK_HIDE, 0},
	{"��������", POS_RESTING, do_put, 0, 0, 400},
	{"��������", POS_STANDING, do_not_here, 1, 0, -1},
	{"��������", POS_SLEEPING, DoShowPolitics, 0, 0, 0},
	{"������", POS_FIGHTING, do_assist, 1, 0, -1},
	{"������", POS_DEAD, do_help, 0, 0, 0},
	{"��������", POS_DEAD, do_mark, LVL_IMPL, 0, 0},
	{"����������", POS_STANDING, do_not_here, 1, 0, -1},
	{"������", POS_STANDING, do_not_here, 1, 0, -1},
	{"�����", POS_STANDING, do_not_here, 1, 0, -1},
	{"���������", POS_RESTING, do_visible, 1, 0, -1},
	{"�������", POS_DEAD, do_gen_ps, 0, SCMD_POLICIES, 0},
	{"�����������", POS_STANDING, do_not_here, 1, 0, 500},
	{"������", POS_RESTING, do_order, 1, 0, -1},
	{"���������", POS_RESTING, do_horseput, 0, 0, 500},
	{"������������", POS_RESTING, do_looking, 0, 0, 250},
	{"��������", POS_FIGHTING, do_protect, 0, 0, -1},
	{"���������", POS_SITTING, do_use, 1, SCMD_USE, 400},
	{"��������", POS_RESTING, do_sit, 0, 0, -1},
	{"������������", POS_RESTING, do_hearing, 0, 0, 300},
	{"�������������", POS_RESTING, do_looking, 0, 0, 250},
	{"��������", POS_DEAD, DoBoard, 0, Boards::SUGGEST_BOARD, 0},
	{"����������", POS_SLEEPING, do_wake, 0, SCMD_WAKE, -1},
	{"��������", POS_RESTING, do_forgive, 0, 0, 0},
	{"���������", POS_RESTING, do_eat, 0, SCMD_TASTE, 300},
	{"�������", POS_RESTING, do_eat, 0, SCMD_DEVOUR, 300},
	{"�������", POS_STANDING, do_not_here, 0, 0, -1},
	{"������", POS_SLEEPING, do_goto, LVL_GOD, 0, 0},

	{"���������", POS_RESTING, do_wake, 0, SCMD_WAKEUP, -1},
	{"���������������", POS_DEAD, do_ungroup, 0, 0, 500},
	{"���������", POS_RESTING, do_split, 1, 0, 500},
	{"�������", POS_RESTING, do_help, 1, 0, 500},
	{"�������", POS_STANDING, do_fire, 0, 0, -1},
	{"����������", POS_DEAD, do_ungroup, 0, 0, 500},
	{"�����������", POS_STANDING, do_not_here, 0, 0, -1},
	{"����������", POS_RESTING, do_findhelpee, 0, SCMD_FREEHELPEE, -1},
	{"�����", POS_DEAD, do_mode, 0, 0, 0},
	{"������", POS_RESTING, do_repair, 0, 0, -1},
	{"�������", POS_RESTING, do_recipes, 0, 0, 0},
	{"�������", POS_DEAD, DoBest, 0, 0, 0},
	{"����", POS_FIGHTING, do_mixture, 0, SCMD_RUNES, -1},
	{"��������", POS_SITTING, do_fish, 0, 0, -1}, // prool

	{"�����", POS_FIGHTING, do_bash, 1, 0, -1},
	{"��������", POS_STANDING, do_not_here, 0, 0, -1},
	{"�������", POS_SLEEPING, do_gsay, 0, 0, -1},
	{"��������", POS_FIGHTING, do_manadrain, 0, 0, -1},
	{"�����", POS_RESTING, do_sit, 0, 0, -1},
	{"�������", POS_DEAD, do_alias, 0, 0, 0},
	{"�������", POS_RESTING, do_tell, 0, 0, -1},
	{"���������", POS_STANDING, do_lightwalk, 0, 0, 0},
	{"���������", POS_RESTING, do_follow, 0, 0, 500},
	{"�������", POS_FIGHTING, do_mixture, 0, SCMD_RUNES, -1},
	{"�����", POS_STANDING, Glory::do_spend_glory, 0, 0, 0},
	{"�����2", POS_STANDING, GloryConst::do_spend_glory, 0, 0, 0},
	{"��������", POS_RESTING, do_look, 0, SCMD_LOOK, 0},
	{"�������", POS_STANDING, do_mixture, 0, SCMD_ITEMS, -1},
//  { "����������",     POS_STANDING, do_transform_weapon, 0, SKILL_CREATEBOW, -1 },
	{"�����", POS_RESTING, do_remove, 0, 0, 500},
	{"�������", POS_SITTING, do_create, 0, 0, -1},
	{"���", POS_SLEEPING, do_sleep, 0, 0, -1},
	{"���������", POS_FIGHTING, do_horseoff, 0, 0, -1},
	{"������", POS_RESTING, do_create, 0, SCMD_RECIPE, 0},
	{"���������", POS_SLEEPING, do_save, 0, 0, 0},
	{"�������", POS_DEAD, do_commands, 0, SCMD_SOCIALS, 0},
	{"�����", POS_SLEEPING, do_sleep, 0, 0, -1},
	{"������", POS_FIGHTING, do_rescue, 1, 0, -1},
	{"�����������", POS_SLEEPING, do_features, 0, 0, 0},
	{"������", POS_STANDING, do_not_here, 0, 0, -1},
	{"�������", POS_DEAD, do_help, 0, 0, 0},
	{"��������", POS_RESTING, do_spec_comm, 0, SCMD_ASK, -1},
	{"����������", POS_STANDING, do_hide, 1, 0, 500},
	{"��������", POS_RESTING, do_consider, 0, 0, 500},
	{"������", POS_STANDING, do_not_here, 0, 0, -1},
	{"������", POS_DEAD, do_display, 0, 0, 0},
	{"����������", POS_DEAD, do_statistic, 0, 0, 0},
	{"�������", POS_DEAD, do_gen_ps, 0, SCMD_CLEAR, 0},
	{"�����", POS_RESTING, do_style, 0, 0, 0},
	{"������", POS_DEAD, do_display, 0, 0, 0},
	{"����", POS_DEAD, do_score, 0, 0, 0},

	{"�����", POS_DEAD, TitleSystem::do_title, 0, 0, 0},
	{"��������", POS_DEAD, do_wimpy, 0, 0, 0},

	{"�����", POS_FIGHTING, do_kill, 0, 0, -1},
	{"������", POS_RESTING, do_remove, 0, 0, 400},
	{"�������", POS_FIGHTING, do_hit, 0, SCMD_HIT, -1},
	{"�������", POS_FIGHTING, do_strangle, 0, 0, -1},
	{"����������", POS_FIGHTING, do_deviate, 1, 0, -1},
	{"�������", POS_STANDING, do_steal, 1, 0, 0},
	{"��������", POS_RESTING, do_armored, 0, 0, -1},
	{"������", POS_SLEEPING, do_skills, 0, 0, 0},
	{"�������", POS_DEAD, do_score, 0, 0, 0},
	{"������", POS_DEAD, do_levels, 0, 0, 0},
	{"�����", POS_STANDING, do_not_here, 0, 0, -1},

	{"���������", POS_DEAD, DoStoreHouse, 0, 0, 0},
	{"��������������", POS_STANDING, do_not_here, 0, 0, -1},

	{"����", POS_DEAD, do_color, 0, 0, 0},

	{"��������", POS_STANDING, do_clanstuff, 0, 0, 0},

	{"������", POS_STANDING, do_not_here, 0, 0, -1},
	{"������", POS_RESTING, do_look, 0, SCMD_READ, 200},

	{"�������", POS_RESTING, do_spec_comm, 0, SCMD_WHISPER, -1},

	{"����������", POS_SLEEPING, do_equipment, 0, 0, 0},
	{"������", POS_RESTING, do_echo, 1, SCMD_EMOTE, -1},
	{"���", POS_SLEEPING, do_echo, LVL_IMMORT, SCMD_ECHO, -1},

	{"������", POS_RESTING, do_courage, 0, 0, -1},

	// God commands for listing
	{"�������", POS_DEAD, do_liblist, LVL_GOD, SCMD_MLIST, 0},
	{"�������", POS_DEAD, do_liblist, LVL_GOD, SCMD_OLIST, 0},
	{"�������", POS_DEAD, do_liblist, LVL_GOD, SCMD_RLIST, 0},
	{"�������", POS_DEAD, do_liblist, LVL_GOD, SCMD_ZLIST, 0},
	{"�������", POS_DEAD, do_imlist, LVL_IMPL, 0, 0},

	{"'", POS_RESTING, do_say, 0, 0, -1},
	{":", POS_RESTING, do_echo, 1, SCMD_EMOTE, -1},
	{";", POS_DEAD, do_wiznet, LVL_IMMORT, 0, -1},
	{"advance", POS_DEAD, do_advance, LVL_IMPL, 0, 0},
	{"alias", POS_DEAD, do_alias, 0, 0, 0},
	{"alter", POS_RESTING, do_fit, 0, SCMD_MAKE_OVER, 500},
	{"ask", POS_RESTING, do_spec_comm, 0, SCMD_ASK, -1},
	{"assist", POS_FIGHTING, do_assist, 1, 0, -1},
	{"attack", POS_FIGHTING, do_hit, 0, SCMD_MURDER, -1},
	{"auction", POS_RESTING, do_gen_comm, 0, SCMD_AUCTION, -1},
	{"backstab", POS_STANDING, do_backstab, 1, 0, 1},
	{"balance", POS_STANDING, do_not_here, 1, 0, -1},
	{"ban", POS_DEAD, do_ban, LVL_GRGOD, 0, 0},
	{"bash", POS_FIGHTING, do_bash, 1, 0, -1},
	{"beep", POS_DEAD, do_beep, LVL_IMMORT, 0, 0},
	{"block", POS_FIGHTING, do_block, 0, 0, -1},
	{"build", POS_DEAD, do_build, LVL_IMPL, 0, -1},
	{"bug", POS_DEAD, report_on_board, 0, Boards::ERROR_BOARD, 0},
	{"buy", POS_STANDING, do_not_here, 0, 0, -1},
	{"best", POS_DEAD, DoBest, 0, 0, 0},
	{"cast", POS_SITTING, do_cast, 1, 0, -1},
	{"check", POS_STANDING, do_not_here, 1, 0, -1},
	{"chopoff", POS_FIGHTING, do_chopoff, 0, 0, 500},
	{"clear", POS_DEAD, do_gen_ps, 0, SCMD_CLEAR, 0},
	{"close", POS_SITTING, do_gen_door, 0, SCMD_CLOSE, 500},
	{"cls", POS_DEAD, do_gen_ps, 0, SCMD_CLEAR, 0},
	{"color", POS_DEAD, do_color, 0, 0, 0},
	{"commands", POS_DEAD, do_commands, 0, SCMD_COMMANDS, 0},
	{"consider", POS_RESTING, do_consider, 0, 0, 500},
	{"credits", POS_DEAD, do_gen_ps, 0, SCMD_CREDITS, 0},
	{"date", POS_DEAD, do_date, LVL_IMMORT, SCMD_DATE, 0},
	{"dc", POS_DEAD, do_dc, LVL_GRGOD, 0, 0},
	{"deposit", POS_STANDING, do_not_here, 1, 0, 500},
	{"deviate", POS_FIGHTING, do_deviate, 0, 0, -1},
	{"diagnose", POS_RESTING, do_diagnose, 0, 0, 500},
	{"dig", POS_STANDING, do_dig, 0, SKILL_DIG, -1},
	{"disarm", POS_FIGHTING, do_disarm, 0, 0, -1},
	{"display", POS_DEAD, do_display, 0, 0, 0},
	{"donate", POS_RESTING, do_drop, 0, 0 /*SCMD_DONATE */ , 500},
	{"drink", POS_RESTING, do_drink, 0, SCMD_DRINK, 500},
	{"drop", POS_RESTING, do_drop, 0, SCMD_DROP, 500},
	{"dumb", POS_DEAD, do_wizutil, LVL_IMMORT, SCMD_DUMB, 0},
	{"eat", POS_RESTING, do_eat, 0, SCMD_EAT, 500},
	{"devour", POS_RESTING, do_eat, 0, SCMD_DEVOUR, 300},
	{"echo", POS_SLEEPING, do_echo, LVL_IMMORT, SCMD_ECHO, 0},
	{"emote", POS_RESTING, do_echo, 1, SCMD_EMOTE, -1},
	//python_off {"email", POS_DEAD, do_email, LVL_IMPL, 0, 0},
	{"enter", POS_STANDING, do_enter, 0, 0, -2},
	{"equipment", POS_SLEEPING, do_equipment, 0, 0, 0},
	{"examine", POS_RESTING, do_examine, 0, 0, 500},
//F@N|
	{"exchange", POS_RESTING, do_exchange, 1, 0, -1},
	{"exits", POS_RESTING, do_exits, 0, 0, 500},
	{"featset", POS_SLEEPING, do_featset, LVL_IMPL, 0, 0},
	{"features", POS_SLEEPING, do_features, 0, 0, 0},
	{"fflush", POS_SLEEPING, do_fflush, 0, 0, 0}, // prool
	{"fill", POS_STANDING, do_pour, 0, SCMD_FILL, 500},
	{"fit", POS_RESTING, do_fit, 0, SCMD_DO_ADAPT, 500},
	{"flee", POS_FIGHTING, do_flee, 1, 0, -1},
	{"follow", POS_RESTING, do_follow, 0, 0, -1},
	{"force", POS_SLEEPING, do_force, LVL_GRGOD, 0, 0},
	{"forcetime", POS_DEAD, do_forcetime, LVL_IMPL, 0, 0},
	{"freeze", POS_DEAD, do_wizutil, LVL_FREEZE, SCMD_FREEZE, 0},
	{"gecho", POS_DEAD, do_gecho, LVL_GOD, 0, 0},
	{"get", POS_RESTING, do_get, 0, 0, 500},
	{"give", POS_RESTING, do_give, 0, 0, 500},
	{"godnews", POS_DEAD, DoBoard, 1, Boards::GODNEWS_BOARD, -1},
	{"gold", POS_RESTING, do_gold, 0, 0, 0},
	{"glide", POS_STANDING, do_lightwalk, 0, 0, 0},
	{"glory", POS_RESTING, do_glory, LVL_BUILDER, 0, 0},
	{"glory2", POS_RESTING, GloryConst::do_glory, LVL_IMPL, 0, 0},
	{"gossip", POS_RESTING, do_gen_comm, 0, SCMD_GOSSIP, -1},
	{"goto", POS_SLEEPING, do_goto, LVL_GOD, 0, 0},
	{"grab", POS_RESTING, do_grab, 0, 0, 500},
	{"group", POS_RESTING, do_group, 1, 0, 500},
	{"gsay", POS_SLEEPING, do_gsay, 0, 0, -1},
	{"gtell", POS_SLEEPING, do_gsay, 0, 0, -1},
	{"handbook", POS_DEAD, do_gen_ps, LVL_IMMORT, SCMD_HANDBOOK, 0},
	{"hcontrol", POS_DEAD, DoHcontrol, LVL_GRGOD, 0, 0},
	{"help", POS_DEAD, do_help, 0, 0, 0},
	{"hell", POS_DEAD, do_wizutil, LVL_GOD, SCMD_HELL, 0},
	{"hide", POS_STANDING, do_hide, 1, 0, 0},
	{"hit", POS_FIGHTING, do_hit, 0, SCMD_HIT, -1},
	{"hold", POS_RESTING, do_grab, 1, 0, 500},
	{"holler", POS_RESTING, do_gen_comm, 1, SCMD_HOLLER, -1},
	{"horse", POS_STANDING, do_not_here, 0, 0, -1},
	{"host", POS_RESTING, do_host, 0, 0, -1},
	{"house", POS_RESTING, DoHouse, 0, 0, 0},
	{"huk", POS_FIGHTING, do_mighthit, 0, 0, -1},
	{"idea", POS_DEAD, DoBoard, 1, Boards::IDEA_BOARD, 0},
	{"ignore", POS_DEAD, do_ignore, 0, 0, 0},
	{"immlist", POS_DEAD, do_gen_ps, 0, SCMD_IMMLIST, 0},
	{"index", POS_RESTING, do_help, 1, 0, 500},
	{"info", POS_SLEEPING, do_gen_ps, 0, SCMD_INFO, 0},
	{"insert", POS_STANDING, do_insertgem, 0, SKILL_INSERTGEM, -1},
	{"inspect", POS_DEAD, do_inspect, LVL_BUILDER, 0, 0},
	{"insult", POS_RESTING, do_insult, 0, 0, -1},
	{"inventory", POS_SLEEPING, do_inventory, 0, 0, 0},
	{"invis", POS_DEAD, do_invis, LVL_GOD, 0, -1},
	{"junk", POS_RESTING, do_drop, 0, 0 /*SCMD_JUNK */ , 500},
	{"kick", POS_FIGHTING, do_kick, 1, 0, -1},
	{"kill", POS_FIGHTING, do_kill, 0, 0, -1},
	{"last", POS_DEAD, do_last, LVL_GOD, 0, 0},
	{"levels", POS_DEAD, do_levels, 0, 0, 0},
	{"list", POS_STANDING, do_not_here, 0, 0, -1},
	{"load", POS_DEAD, do_load, LVL_BUILDER, 0, 0},
	{"look", POS_RESTING, do_look, 0, SCMD_LOOK, 200},
	{"lock", POS_SITTING, do_gen_door, 0, SCMD_LOCK, 500},
	{"map", POS_RESTING, do_map, 0, 0, 0},
	{"mail", POS_STANDING, do_not_here, 1, 0, -1},
	{"mode", POS_DEAD, do_mode, 0, 0, 0},
	{"mshout", POS_RESTING, do_mobshout, 0, 0, -1},
	{"motd", POS_DEAD, do_gen_ps, 0, SCMD_MOTD, 0},
	{"murder", POS_FIGHTING, do_hit, 0, SCMD_MURDER, -1},
	{"mute", POS_DEAD, do_wizutil, LVL_IMMORT, SCMD_MUTE, 0},
	{"medit", POS_DEAD, do_olc, LVL_BUILDER, SCMD_OLC_MEDIT, 0},
	{"name", POS_DEAD, do_wizutil, LVL_GOD, SCMD_NAME, 0},
	{"nedit", POS_RESTING, NamedStuff::do_named, LVL_BUILDER, SCMD_NAMED_EDIT, 0}, //������� ���� ��������������
	{"newpass", POS_DEAD, do_newpass, LVL_GRGOD, 0, 0}, // prool
	{"news", POS_DEAD, DoBoard, 1, Boards::NEWS_BOARD, -1},
	{"nlist", POS_RESTING, NamedStuff::do_named, LVL_BUILDER, SCMD_NAMED_LIST, 0}, //������� ���� ������
	{"notitle", POS_DEAD, do_wizutil, LVL_GRGOD, SCMD_NOTITLE, 0},
	{"nslookup", POS_DEAD, do_host, 1, 0, 0},
	{"oedit", POS_DEAD, do_olc, LVL_BUILDER, SCMD_OLC_OEDIT, 0},
	{"offer", POS_STANDING, do_not_here, 1, 0, 0},
	{"olc", POS_DEAD, do_olc, LVL_GOD, SCMD_OLC_SAVEINFO, 0},
	{"open", POS_SITTING, do_gen_door, 0, SCMD_OPEN, 500},
	{"order", POS_RESTING, do_order, 1, 0, -1},
	{"page", POS_DEAD, do_page, LVL_GOD, 0, 0},
	{"parry", POS_FIGHTING, do_parry, 0, 0, -1},
	{"pick", POS_STANDING, do_gen_door, 1, SCMD_PICK, -1},
	{"poisoned", POS_FIGHTING, do_poisoned, 0, 0, -1},
	{"policy", POS_DEAD, do_gen_ps, 0, SCMD_POLICIES, 0},
	{"poofin", POS_DEAD, do_poofset, LVL_GOD, SCMD_POOFIN, 0},
	{"poofout", POS_DEAD, do_poofset, LVL_GOD, SCMD_POOFOUT, 0},
	{"pour", POS_STANDING, do_pour, 0, SCMD_POUR, -1},
	{"practice", POS_STANDING, do_not_here, 0, 0, -1},
	{"prompt", POS_DEAD, do_display, 0, 0, 0},
	{"proxy", POS_DEAD, do_proxy, LVL_GRGOD, 0, 0},
	{"purge", POS_DEAD, do_purge, LVL_GOD, 0, 0},
	{"put", POS_RESTING, do_put, 0, 0, 500},
	//python_off {"python", POS_DEAD, do_console, LVL_GOD, 0, 0},
	{"quaff", POS_RESTING, do_use, 0, SCMD_QUAFF, 500},
	{"qui", POS_SLEEPING, do_quit, 0, 0, 0},
	{"quit", POS_SLEEPING, do_quit, 0, SCMD_QUIT, -1},
	{"read", POS_RESTING, do_look, 0, SCMD_READ, 200},
	{"receive", POS_STANDING, do_not_here, 1, 0, -1},
	{"recipes", POS_RESTING, do_recipes, 0, 0, 0},
	{"recite", POS_RESTING, do_use, 0, SCMD_RECITE, 500},
	{"redit", POS_DEAD, do_olc, LVL_BUILDER, SCMD_OLC_REDIT, 0},
	{"register", POS_DEAD, do_wizutil, LVL_IMMORT, SCMD_REGISTER, 0},
	{"roomdescr", POS_DEAD, do_room_descr, LVL_BUILDER, 0, 0},
	{"roomflag", POS_DEAD, do_room_flag, LVL_BUILDER, 0, 0},
	{"roomtitle", POS_DEAD, do_room_title, LVL_BUILDER, 0, 0},
	{"roomtype", POS_DEAD, do_room_type, LVL_BUILDER, 0, 0},
	{"unregister", POS_DEAD, do_wizutil, LVL_IMMORT, SCMD_UNREGISTER, 0},
	{"reload", POS_DEAD, do_reboot, LVL_IMPL, 0, 0},
	{"remove", POS_RESTING, do_remove, 0, 0, 500},
	{"rent", POS_STANDING, do_not_here, 1, 0, -1},
	{"reply", POS_RESTING, do_reply, 0, 0, -1},
	{"report", POS_RESTING, do_report, 0, 0, -1},
	{"reroll", POS_DEAD, do_wizutil, LVL_GRGOD, SCMD_REROLL, 0},
	{"rescue", POS_FIGHTING, do_rescue, 1, 0, -1},
	{"rest", POS_RESTING, do_rest, 0, 0, -1},
	{"restore", POS_DEAD, do_restore, LVL_GRGOD, SCMD_RESTORE_GOD, 0},
	{"return", POS_DEAD, do_return, 0, 0, -1},
	{"rset", POS_SLEEPING, do_rset, LVL_BUILDER, 0, 0},
	{"rules", POS_DEAD, do_gen_ps, LVL_IMMORT, SCMD_RULES, 0},
	{"runes", POS_FIGHTING, do_mixture, 0, SCMD_RUNES, -1},
	{"save", POS_SLEEPING, do_save, 0, 0, 0},
	{"say", POS_RESTING, do_say, 0, 0, -1},
	{"scan", POS_RESTING, do_sides, 0, 0, 500},
	{"score", POS_DEAD, do_score, 0, 0, 0},
	{"sell", POS_STANDING, do_not_here, 0, 0, -1},
	{"send", POS_SLEEPING, do_send, LVL_GRGOD, 0, 0},
	{"sense", POS_STANDING, do_sense, 0, 0, 500},
	{"set", POS_DEAD, do_set, LVL_IMMORT, 0, 0},
	{"settle", POS_STANDING, do_not_here, 1, 0, -1},
	{"shout", POS_RESTING, do_gen_comm, 0, SCMD_SHOUT, -1},
	{"show", POS_DEAD, do_show, LVL_IMMORT, 0, 0},
	{"shutdown", POS_DEAD, do_shutdown, LVL_IMPL, SCMD_SHUTDOWN, 0},
	{"sip", POS_RESTING, do_drink, 0, SCMD_SIP, 500},
	{"sit", POS_RESTING, do_sit, 0, 0, -1},
	{"skills", POS_RESTING, do_skills, 0, 0, 0},
	{"skillset", POS_SLEEPING, do_skillset, LVL_IMPL, 0, 0},
	{"system", POS_DEAD, do_system, LVL_GRGOD, 0, 0},
	{"morphset", POS_SLEEPING, do_morphset, LVL_IMPL, 0, 0},
	{"sleep", POS_SLEEPING, do_sleep, 0, 0, -1},
	{"sneak", POS_STANDING, do_sneak, 1, 0, -2},
	{"snoop", POS_DEAD, do_snoop, LVL_GRGOD, 0, 0},
	{"socials", POS_DEAD, do_commands, 0, SCMD_SOCIALS, 0},
	{"spells", POS_RESTING, do_spells, 0, 0, 0},
	{"split", POS_RESTING, do_split, 1, 0, 0},
	{"stand", POS_RESTING, do_stand, 0, 0, -1},
	{"stat", POS_DEAD, do_stat, LVL_GOD, 0, 0},
	{"steal", POS_STANDING, do_steal, 1, 0, 300},
	{"strangle", POS_FIGHTING, do_strangle, 0, 0, -1},
	{"stupor", POS_FIGHTING, do_stupor, 0, 0, -1},
	{"switch", POS_DEAD, do_switch, LVL_GRGOD, 0, 0},
	{"syslog", POS_DEAD, do_syslog, LVL_IMMORT, SYSLOG, 0},
	{"suggest", POS_DEAD, report_on_board, 0, Boards::SUGGEST_BOARD, 0},
	{"slist", POS_DEAD, do_slist, LVL_IMPL, 0, 0},
	{"sedit", POS_DEAD, do_sedit, LVL_IMPL, 0, 0},
	{"errlog", POS_DEAD, do_syslog, LVL_BUILDER, ERRLOG, 0},
	{"imlog", POS_DEAD, do_syslog, LVL_BUILDER, IMLOG, 0},
	{"take", POS_RESTING, do_get, 0, 0, 500},
	{"taste", POS_RESTING, do_eat, 0, SCMD_TASTE, 500},
	{"teleport", POS_DEAD, do_teleport, LVL_GRGOD, 0, -1},
	{"tell", POS_RESTING, do_tell, 0, 0, -1},
	{"time", POS_DEAD, do_time, 0, 0, 0},
	{"title", POS_DEAD, TitleSystem::do_title, 0, 0, 0},
	{"touch", POS_FIGHTING, do_touch, 0, 0, -1},
	{"track", POS_STANDING, do_track, 0, 0, -1},
	{"transfer", POS_STANDING, do_not_here, 1, 0, -1},
	{"trigedit", POS_DEAD, do_olc, LVL_BUILDER, SCMD_OLC_TRIGEDIT, 0},
	{"turn undead", POS_RESTING, do_turn_undead, 0, 0, -1},
	{"typo", POS_DEAD, report_on_board, 0, Boards::MISPRINT_BOARD, 0},
	{"unaffect", POS_DEAD, do_wizutil, LVL_GRGOD, SCMD_UNAFFECT, 0},
	{"unban", POS_DEAD, do_unban, LVL_GRGOD, 0, 0},
	{"ungroup", POS_DEAD, do_ungroup, 0, 0, -1},
	{"unlock", POS_SITTING, do_gen_door, 0, SCMD_UNLOCK, 500},
	{"uptime", POS_DEAD, do_date, 0, SCMD_UPTIME, 0},
	{"use", POS_SITTING, do_use, 1, SCMD_USE, 500},
	{"users", POS_DEAD, do_users, LVL_IMMORT, 0, 0},
	{"value", POS_STANDING, do_not_here, 0, 0, -1},
	{"version", POS_DEAD, do_gen_ps, 0, SCMD_VERSION, 0},
	{"virtustan", POS_DEAD, do_virtustan, 1, 0, -1},
	{"visible", POS_RESTING, do_visible, 1, 0, -1},
	{"vnum", POS_DEAD, do_vnum, LVL_GRGOD, 0, 0},
	{"vstat", POS_DEAD, do_vstat, LVL_GRGOD, 0, 0},
	{"wake", POS_SLEEPING, do_wake, 0, 0, -1},
	{"warcry", POS_FIGHTING, do_warcry, 1, 0, -1},
	{"wear", POS_RESTING, do_wear, 0, 0, 500},
	{"weather", POS_RESTING, do_weather, 0, 0, 0},
	{"where", POS_RESTING, do_where, LVL_IMMORT, 0, 0},
	{"whirl", POS_FIGHTING, do_iron_wind, 0, 0, -1},
	{"whisper", POS_RESTING, do_spec_comm, 0, SCMD_WHISPER, -1},
	{"who", POS_RESTING, do_who, 0, 0, 0},
	{"whoami", POS_DEAD, do_gen_ps, 0, SCMD_WHOAMI, 0},
	{"whois", POS_DEAD, do_whois, LVL_GRGOD, 0, 0},
	{"wield", POS_RESTING, do_wield, 0, 0, 500},
	{"wimpy", POS_DEAD, do_wimpy, 0, 0, 0},
	{"withdraw", POS_STANDING, do_not_here, 1, 0, -1},
	{"wizhelp", POS_SLEEPING, do_commands, LVL_IMMORT, SCMD_WIZHELP, 0},
	{"wizlock", POS_DEAD, do_wizlock, LVL_IMPL, 0, 0},
	{"wiznet", POS_DEAD, do_wiznet, LVL_IMMORT, 0, 0},
	{"wizat", POS_DEAD, do_at, LVL_GRGOD, 0, 0},
	{"write", POS_STANDING, do_write, 1, 0, -1},
	{"zedit", POS_DEAD, do_olc, LVL_BUILDER, SCMD_OLC_ZEDIT, 0},
	{"zone", POS_RESTING, do_zone, 0, 0, 0},
	{"zreset", POS_DEAD, do_zreset, LVL_GRGOD, 0, 0},

	// test command for gods
	{"godtest", POS_DEAD, do_godtest, LVL_IMPL, 0, 0},
	{"armor", POS_DEAD, do_print_armor, LVL_IMPL, 0, 0},

	// ������� ��������� - ��� ������� ���� ������ ����
	{"mrlist", POS_DEAD, do_list_make, LVL_BUILDER, 0, 0},
	{"mredit", POS_DEAD, do_edit_make, LVL_BUILDER, 0, 0},
	{"����", POS_STANDING, do_make_item, 0, MAKE_WEAR, 0},
	{"��������", POS_STANDING, do_make_item, 0, MAKE_METALL, 0},
	{"����������", POS_STANDING, do_make_item, 0, MAKE_CRAFT, 0},

	// God commands for listing
	{"mlist", POS_DEAD, do_liblist, LVL_GOD, SCMD_MLIST, 0},
	{"olist", POS_DEAD, do_liblist, LVL_GOD, SCMD_OLIST, 0},
	{"rlist", POS_DEAD, do_liblist, LVL_GOD, SCMD_RLIST, 0},
	{"zlist", POS_DEAD, do_liblist, LVL_GOD, SCMD_ZLIST, 0},

	// DG trigger commands
	{"attach", POS_DEAD, do_attach, LVL_IMPL, 0, 0},
	{"detach", POS_DEAD, do_detach, LVL_IMPL, 0, 0},
	{"tlist", POS_DEAD, do_tlist, LVL_GRGOD, 0, 0},
	{"tstat", POS_DEAD, do_tstat, LVL_GRGOD, 0, 0},
	{"masound", POS_DEAD, do_masound, -1, 0, -1},
	{"mkill", POS_STANDING, do_mkill, -1, 0, -1},
	{"mjunk", POS_SITTING, do_mjunk, -1, 0, -1},
	{"mdamage", POS_DEAD, do_mdamage, -1, 0, -1},
	{"mdoor", POS_DEAD, do_mdoor, -1, 0, -1},
	{"mecho", POS_DEAD, do_mecho, -1, 0, -1},
	{"mechoaround", POS_DEAD, do_mechoaround, -1, 0, -1},
	{"msend", POS_DEAD, do_msend, -1, 0, -1},
	{"mload", POS_DEAD, do_mload, -1, 0, -1},
	{"mpurge", POS_DEAD, do_mpurge, -1, 0, -1},
	{"mgoto", POS_DEAD, do_mgoto, -1, 0, -1},
	{"mat", POS_DEAD, do_mat, -1, 0, -1},
	{"mteleport", POS_DEAD, do_mteleport, -1, 0, -1},
	{"mforce", POS_DEAD, do_mforce, -1, 0, -1},
	{"mexp", POS_DEAD, do_mexp, -1, 0, -1},
	{"mgold", POS_DEAD, do_mgold, -1, 0, -1},
	{"mremember", POS_DEAD, do_mremember, -1, 0, -1},
	{"mforget", POS_DEAD, do_mforget, -1, 0, -1},
	{"mtransform", POS_DEAD, do_mtransform, -1, 0, -1},
	{"mfeatturn", POS_DEAD, do_mfeatturn, -1, 0, -1},
	{"mskillturn", POS_DEAD, do_mskillturn, -1, 0, -1},
	{"mskilladd", POS_DEAD, do_mskilladd, -1, 0, -1},
	{"mspellturn", POS_DEAD, do_mspellturn, -1, 0, -1},
	{"mspelladd", POS_DEAD, do_mspelladd, -1, 0, -1},
	{"mspellitem", POS_DEAD, do_mspellitem, -1, 0, -1},
	{"vdelete", POS_DEAD, do_vdelete, LVL_IMPL, 0, 0},

	{"konsole", POS_DEAD, do_konsole, 0, 0, 0}, // prool

	{"�����", POS_DEAD, do_kogda, 0, 0, 0}, // prool:�������, ��������� ��� �������, ����� �� ���� �������� ��� merge

	{"\n", 0, 0, 0, 0, 0}
};				// this must be last


const char *dir_fill[] = { "in",
						   "from",
						   "with",
						   "the",
						   "on",
						   "at",
						   "to",
						   "\n"
						 };

const char *reserved[] = { "a",
						   "an",
						   "self",
						   "me",
						   "all",
						   "room",
						   "someone",
						   "something",
						   "\n"
						 };


void check_hiding_cmd(CHAR_DATA * ch, int percent)
{
	int remove_hide = FALSE;
	if (affected_by_spell(ch, SPELL_HIDE))
	{
		if (percent == -2)
		{
			if (AFF_FLAGGED(ch, AFF_SNEAK))
				remove_hide = number(1, skill_info[SKILL_SNEAK].max_percent) >
							  calculate_skill(ch, SKILL_SNEAK, skill_info[SKILL_SNEAK].max_percent, 0);
			else
				percent = 500;
		}

		if (percent == -1)
			remove_hide = TRUE;
		else if (percent > 0)
			remove_hide = number(1, percent) > calculate_skill(ch, SKILL_HIDE, percent, 0);

		if (remove_hide)
		{
			affect_from_char(ch, SPELL_HIDE);
			if (!AFF_FLAGGED(ch, AFF_HIDE))
			{
				send_to_char("�� ���������� ���������.\r\n", ch);
				act("$n ���������$g ���������.", FALSE, ch, 0, 0, TO_ROOM);
			}
		}
	}
}

bool check_frozen_cmd(CHAR_DATA *ch, int cmd)
{
	if (!strcmp(cmd_info[cmd].command, "����")
		|| !strcmp(cmd_info[cmd].command, "�����")
		|| !strcmp(cmd_info[cmd].command, "qui")
		|| !strcmp(cmd_info[cmd].command, "quit")
		|| !strcmp(cmd_info[cmd].command, "�����������")
		|| !strcmp(cmd_info[cmd].command, "offer")
		|| !strcmp(cmd_info[cmd].command, "������")
		|| !strcmp(cmd_info[cmd].command, "rent")
		|| !strcmp(cmd_info[cmd].command, "����������")
		|| !strcmp(cmd_info[cmd].command, "settle")
		|| !strcmp(cmd_info[cmd].command, "����")
		|| !strcmp(cmd_info[cmd].command, "whoami")
		|| !strcmp(cmd_info[cmd].command, "�������")
		|| !strcmp(cmd_info[cmd].command, "help")
		|| !strcmp(cmd_info[cmd].command, "������")
		|| !strcmp(cmd_info[cmd].command, "�������")
		|| !strcmp(cmd_info[cmd].command, "����")
		|| !strcmp(cmd_info[cmd].command, "����")
		|| !strcmp(cmd_info[cmd].command, "�������")
		|| !strcmp(cmd_info[cmd].command, "score"))
	{
		return true;
	}
	return false;
}

/*
 * This is the actual command interpreter called from game_loop() in comm.c
 * It makes sure you are the proper level and position to execute the command,
 * then calls the appropriate function.
 */
void command_interpreter(CHAR_DATA * ch, char *argument)
{
	int cmd, length, social = FALSE, hardcopy = FALSE;
	char *line;
#ifdef PROOLDEBUG2
printf("proolfool. checkpoint #I0\n");
#endif

	// just drop to next line for hitting CR
	CHECK_AGRO(ch) = 0;
	skip_spaces(&argument);
#ifdef PROOLDEBUG2
printf("proolfool. checkpoint #I1\n");
#endif

	if (!*argument)
		return;
	if (!IS_NPC(ch))
	{
#ifdef PROOLDEBUG2
printf("proolfool. checkpoint #Ia\n");
#endif
#if 1 // proolfool: bug here
		log("<%s> {%5d} [%s]", GET_NAME(ch), GET_ROOM_VNUM(IN_ROOM(ch)), argument);
#endif
#ifdef PROOLDEBUG2
printf("proolfool. checkpoint #Ia2\n");
#endif
#if 1 // proolfool
		if (GET_LEVEL(ch) >= LVL_IMMORT || GET_GOD_FLAG(ch, GF_PERSLOG) || GET_GOD_FLAG(ch, GF_DEMIGOD))
			pers_log(ch, "<%s> {%5d} [%s]", GET_NAME(ch), GET_ROOM_VNUM(IN_ROOM(ch)), argument);
#endif
#ifdef PROOLDEBUG2
printf("proolfool. checkpoint #Ia3\n");
#endif
	}

	//Polud ������� ��� ��������� ������� ��������� ��������� �������� ����� ������ �������

	int fnum = get_number(&argument);

	/*
	 * special case to handle one-character, non-alphanumeric commands;
	 * requested by many people so "'hi" or ";godnet test" is possible.
	 * Patch sent by Eric Green and Stefan Wasilewski.
	 */
	if (!a_isalpha(*argument))
	{
		arg[0] = argument[0];
		arg[1] = '\0';
		line = argument + 1;
	}
	else
		line = any_one_arg(argument, arg);

#ifdef PROOLDEBUG
printf("proolfool. checkpoint #I4\n");
#endif
	if ((length = strlen(arg)) && length > 1 && *(arg + length - 1) == '!')
	{
		hardcopy = TRUE;
		*(arg + (--length)) = '\0';
		*(argument + length) = ' ';
	}

#ifdef PROOLDEBUG
printf("proolfool. checkpoint #50\n");
#endif
	if ((!GET_MOB_HOLD(ch) && !AFF_FLAGGED(ch, AFF_STOPFIGHT) && !AFF_FLAGGED(ch, AFF_MAGICSTOPFIGHT)))
	{
		int cont;	// continue the command checks
		cont = command_wtrigger(ch, arg, line);
		if (!cont)
			cont += command_mtrigger(ch, arg, line);
		if (!cont)
			cont = command_otrigger(ch, arg, line);
		if (cont)
		{
			check_hiding_cmd(ch, -1);
			return;	// command trigger took over
		}
	}

#ifdef PROOLDEBUG
printf("proolfool. checkpoint #I6\n");
#endif
	//python_off Try scripting
	//python_off if (scripting::execute_player_command(ch, arg, line))
		//python_off return;

	// otherwise, find the command
	for (cmd = 0; *cmd_info[cmd].command != '\n'; cmd++)
	{
		if (hardcopy)
		{
			if (!strcmp(cmd_info[cmd].command, arg))
				if (Privilege::can_do_priv(ch, std::string(cmd_info[cmd].command), cmd, 0))
					break;
		}
		else
		{
			if (!strncmp(cmd_info[cmd].command, arg, length))
				if (Privilege::can_do_priv(ch, std::string(cmd_info[cmd].command), cmd, 0))
					break;
		}
	}

#ifdef PROOLDEBUG
printf("proolfool. checkpoint #70\n");
#endif
	if (*cmd_info[cmd].command == '\n')
	{
		if (find_action(arg) >= 0)
			social = TRUE;
		else
		{
#ifdef I3
		char local_buf[PROOL_MAX_STRLEN];char *cc;
		cc=strchr(argument,' ');
		if (cc==NULL) local_buf[0]=0;
		else strcpy(local_buf,cc+1);
		//printf("i3 command hook label #1 arg='%s' argument='%s' local_buf='%s'\n",arg,argument,local_buf);
		if (i3_command_hook(ch, arg, local_buf)) return;
#endif
			send_to_char("Bad command\r\n", ch);
			return;
		}
	}

#ifdef PROOLDEBUG
printf("proolfool. checkpoint #I8\n");
#endif
	if (((!IS_NPC(ch) && (GET_FREEZE_LEV(ch) > GET_LEVEL(ch)) && (PLR_FLAGGED(ch, PLR_FROZEN)))
			|| GET_MOB_HOLD(ch) > 0
			|| AFF_FLAGGED(ch, AFF_STOPFIGHT)
			|| AFF_FLAGGED(ch, AFF_MAGICSTOPFIGHT))
		&& !check_frozen_cmd(ch, cmd))
	{
		send_to_char("�� ����������, �� �� ������ ���������� � �����...\r\n", ch);
		return;
	}

	if (!social && cmd_info[cmd].command_pointer == NULL)
	{
		send_to_char("��������, �� ���� ��������� �������.\r\n", ch);
		return;
	}

	if (!social && IS_NPC(ch) && cmd_info[cmd].minimum_level >= LVL_IMMORT)
	{
		send_to_char("�� ��� �� ���, ����� ������ ���.\r\n", ch);
		return;
	}

	if (!social && GET_POS(ch) < cmd_info[cmd].minimum_position)
	{
		switch (GET_POS(ch))
		{
		case POS_DEAD:
			send_to_char("����� ���� - �� ������!!! :-(\r\n", ch);
			break;
		case POS_INCAP:
		case POS_MORTALLYW:
			send_to_char("�� � ����������� ��������� � �� ������ ������ ������!\r\n", ch);
			break;
		case POS_STUNNED:
			send_to_char("�� ������� �����, ����� ������� ���!\r\n", ch);
			break;
		case POS_SLEEPING:
			send_to_char("������� ��� � ����� ����?\r\n", ch);
			break;
		case POS_RESTING:
			send_to_char("���... �� ������� �����������...\r\n", ch);
			break;
		case POS_SITTING:
			send_to_char("�������, ��� ����� ������ �� ����.\r\n", ch);
			break;
		case POS_FIGHTING:
			send_to_char("�� �� ���! �� ���������� �� ���� �����!\r\n", ch);
			break;
		}
		return;
	}

	if (social)
	{
		check_hiding_cmd(ch, -1);
		do_social(ch, argument);
	}
	else if (no_specials || !special(ch, cmd, line, fnum))
	{
		check_hiding_cmd(ch, cmd_info[cmd].unhide_percent);
		(*cmd_info[cmd].command_pointer)(ch, line, cmd, cmd_info[cmd].subcmd);
		if (ch->purged())
		{
			return;
		}
		if (!IS_NPC(ch) && IN_ROOM(ch) != NOWHERE && CHECK_AGRO(ch))
		{
			CHECK_AGRO(ch) = FALSE;
			do_aggressive_room(ch, FALSE);
			if (ch->purged())
			{
				return;
			}
		}
	}
}

// ************************************************************************
// * Routines to handle aliasing                                          *
// ************************************************************************


struct alias_data *find_alias(struct alias_data *alias_list, char *str)
{
	while (alias_list != NULL)
	{
		if (*str == *alias_list->alias)	// hey, every little bit counts :-)
			if (!strcmp(str, alias_list->alias))
				return (alias_list);

		alias_list = alias_list->next;
	}

	return (NULL);
}


void free_alias(struct alias_data *a)
{
	if (a->alias)
		free(a->alias);
	if (a->replacement)
		free(a->replacement);
	free(a);
}


// The interface to the outside world: do_alias
ACMD(do_alias)
{
	char *repl;
	struct alias_data *a, *temp;

	if (IS_NPC(ch))
		return;

	repl = any_one_arg(argument, arg);

	if (!*arg)  		// no argument specified -- list currently defined aliases
	{
		send_to_char("���������� ��������� ������:\r\n", ch);
		if ((a = GET_ALIASES(ch)) == NULL)
			send_to_char(" ��� �������.\r\n", ch);
		else
		{
			while (a != NULL)
			{
				sprintf(buf, "%-15s %s\r\n", a->alias, a->replacement);
				send_to_char(buf, ch);
				a = a->next;
			}
		}
	}
	else  		// otherwise, add or remove aliases
	{
		// is this an alias we've already defined?
		if ((a = find_alias(GET_ALIASES(ch), arg)) != NULL)
		{
			REMOVE_FROM_LIST(a, GET_ALIASES(ch), next);
			free_alias(a);
		}
		// if no replacement string is specified, assume we want to delete
		if (!*repl)
		{
			if (a == NULL)
				send_to_char("����� ����� �� ���������.\r\n", ch);
			else
				send_to_char("����� ������� ������.\r\n", ch);
		}
		else  	// otherwise, either add or redefine an alias
		{
			if (!str_cmp(arg, "alias"))
			{
				send_to_char("�� �� ������ ���������� ����� 'alias'.\r\n", ch);
				return;
			}
			CREATE(a, struct alias_data, 1);
			a->alias = str_dup(arg);
			delete_doubledollar(repl);
			a->replacement = str_dup(repl);
			if (strchr(repl, ALIAS_SEP_CHAR) || strchr(repl, ALIAS_VAR_CHAR))
				a->type = ALIAS_COMPLEX;
			else
				a->type = ALIAS_SIMPLE;
			a->next = GET_ALIASES(ch);
			GET_ALIASES(ch) = a;
			send_to_char("����� ������� ��������.\r\n", ch);
		}
	}
}

/*
 * Valid numeric replacements are only $1 .. $9 (makes parsing a little
 * easier, and it's not that much of a limitation anyway.)  Also valid
 * is "$*", which stands for the entire original line after the alias.
 * ";" is used to delimit commands.
 */
#define NUM_TOKENS       9

void perform_complex_alias(struct txt_q *input_q, char *orig, struct alias_data *a)
{
	struct txt_q temp_queue;
	char *tokens[NUM_TOKENS], *temp, *write_point;
	int num_of_tokens = 0, num;

	// First, parse the original string
	temp = strtok(strcpy(buf2, orig), " ");
	while (temp != NULL && num_of_tokens < NUM_TOKENS)
	{
		tokens[num_of_tokens++] = temp;
		temp = strtok(NULL, " ");
	}

	// initialize
	write_point = buf;
	temp_queue.head = temp_queue.tail = NULL;

	// now parse the alias
	for (temp = a->replacement; *temp; temp++)
	{
		if (*temp == ALIAS_SEP_CHAR)
		{
			*write_point = '\0';
			buf[MAX_INPUT_LENGTH - 1] = '\0';
			write_to_q(buf, &temp_queue, 1);
			write_point = buf;
		}
		else if (*temp == ALIAS_VAR_CHAR)
		{
			temp++;
			if ((num = *temp - '1') < num_of_tokens && num >= 0)
			{
				strcpy(write_point, tokens[num]);
				write_point += strlen(tokens[num]);
			}
			else if (*temp == ALIAS_GLOB_CHAR)
			{
				strcpy(write_point, orig);
				write_point += strlen(orig);
			}
			else if ((*(write_point++) = *temp) == '$')	// redouble $ for act safety
				*(write_point++) = '$';
		}
		else
			*(write_point++) = *temp;
	}

	*write_point = '\0';
	buf[MAX_INPUT_LENGTH - 1] = '\0';
	write_to_q(buf, &temp_queue, 1);

	// push our temp_queue on to the _front_ of the input queue
	if (input_q->head == NULL)
		*input_q = temp_queue;
	else
	{
		temp_queue.tail->next = input_q->head;
		input_q->head = temp_queue.head;
	}
}


/*
 * Given a character and a string, perform alias replacement on it.
 *
 * Return values:
 *   0: String was modified in place; call command_interpreter immediately.
 *   1: String was _not_ modified in place; rather, the expanded aliases
 *      have been placed at the front of the character's input queue.
 */
int perform_alias(DESCRIPTOR_DATA * d, char *orig)
{
	char first_arg[MAX_INPUT_LENGTH], *ptr;
	struct alias_data *a, *tmp;

	// Mobs don't have alaises. //
	if (IS_NPC(d->character))
		return (0);

	// bail out immediately if the guy doesn't have any aliases //
	if ((tmp = GET_ALIASES(d->character)) == NULL)
		return (0);

	// find the alias we're supposed to match //
	ptr = any_one_arg(orig, first_arg);

	// bail out if it's null //
	if (!*first_arg)
		return (0);

	// if the first arg is not an alias, return without doing anything //
	if ((a = find_alias(tmp, first_arg)) == NULL)
		return (0);

	if (a->type == ALIAS_SIMPLE)
	{
		strcpy(orig, a->replacement);
		return (0);
	}
	else
	{
		perform_complex_alias(&d->input, ptr, a);
		return (1);
	}
}



// ***************************************************************************
// * Various other parsing utilities                                         *
// ***************************************************************************

/*
 * searches an array of strings for a target string.  "exact" can be
 * 0 or non-0, depending on whether or not the match must be exact for
 * it to be returned.  Returns -1 if not found; 0..n otherwise.  Array
 * must be terminated with a '\n' so it knows to stop searching.
 */
int search_block(const char *arg, const char **list, int exact)
{
	register int i, l = strlen(arg);

	if (exact)
	{
		for (i = 0; **(list + i) != '\n'; i++)
			if (!str_cmp(arg, *(list + i)))
				return (i);
	}
	else
	{
		if (!l)
			l = 1;	// Avoid "" to match the first available string
		for (i = 0; **(list + i) != '\n'; i++)
			if (!strn_cmp(arg, *(list + i), l))
				return (i);
	}

	return (-1);
}

int search_block(const std::string &arg, const char **list, int exact)
{
	register int i;
	std::string::size_type l = arg.length();

	if (exact)
	{
		for (i = 0; **(list + i) != '\n'; i++)
			if (!str_cmp(arg, *(list + i)))
				return (i);
	}
	else
	{
		if (!l)
			l = 1;	// Avoid "" to match the first available string
		for (i = 0; **(list + i) != '\n'; i++)
			if (!strn_cmp(arg, *(list + i), l))
				return (i);
	}

	return (-1);
}

int is_number(const char *str)
{
	while (*str)
		if (!isdigit(*(str++)))
			return (0);

	return (1);
}

/*
 * Given a string, change all instances of double dollar signs ($$) to
 * single dollar signs ($).  When strings come in, all $'s are changed
 * to $$'s to avoid having users be able to crash the system if the
 * inputted string is eventually sent to act().  If you are using user
 * input to produce screen output AND YOU ARE SURE IT WILL NOT BE SENT
 * THROUGH THE act() FUNCTION (i.e., do_gecho, but NOT do_say),
 * you can call delete_doubledollar() to make the output look correct.
 *
 * Modifies the string in-place.
 */
char *delete_doubledollar(char *string)
{
	char *read, *write;

	// If the string has no dollar signs, return immediately //
	if ((write = strchr(string, '$')) == NULL)
		return (string);

	// Start from the location of the first dollar sign //
	read = write;


	while (*read)		// Until we reach the end of the string... //
		if ((*(write++) = *(read++)) == '$')	// copy one char //
			if (*read == '$')
				read++;	// skip if we saw 2 $'s in a row //

	*write = '\0';

	return (string);
}


int fill_word(const char *argument)
{
	return (search_block(argument, dir_fill, TRUE) >= 0);
}


int reserved_word(const char *argument)
{
	return (search_block(argument, reserved, TRUE) >= 0);
}

/*
 * determine if a given string is an abbreviation of another
 * (now works symmetrically -- JE 7/25/94)
 *
 * that was dumb.  it shouldn't be symmetrical.  JE 5/1/95
 *
 * returnss 1 if arg1 is an abbreviation of arg2
 */
int is_abbrev(const char *arg1, const char *arg2)
{
	if (!*arg1)
		return (0);

	for (; *arg1 && *arg2; arg1++, arg2++)
		if (LOWER(*arg1) != LOWER(*arg2))
			return (0);

	if (!*arg1)
		return (1);
	else
		return (0);
}

// return first space-delimited token in arg1; remainder of string in arg2 //
void half_chop(const char* string, char* arg1, char* arg2)
{
	const char* temp = any_one_arg(string, arg1);
	skip_spaces(&temp);
	strl_cpy(arg2, temp, MAX_INPUT_LENGTH);
}

// Used in specprocs, mostly.  (Exactly) matches "command" to cmd number //
int find_command(const char *command)
{
	int cmd;

	for (cmd = 0; *cmd_info[cmd].command != '\n'; cmd++)
		if (!strcmp(cmd_info[cmd].command, command))
			return (cmd);

	return (-1);
}

// int fnum - ����� ���������� � ������� �������-����, ��� ��������� ���������� �������-����� � ����� ������� //
int special(CHAR_DATA * ch, int cmd, char *arg, int fnum)
{
	if (ROOM_FLAGGED(ch->in_room, ROOM_HOUSE))
	{
		ClanListType::const_iterator it = Clan::IsClanRoom(ch->in_room);
		if (Clan::ClanList.end() == it)
		{
			return 0;
		}
	}

	register OBJ_DATA *i;
	register CHAR_DATA *k;
	int j;

	// special in room? //
	if (GET_ROOM_SPEC(ch->in_room) != NULL)
		if (GET_ROOM_SPEC(ch->in_room)(ch, world[ch->in_room], cmd, arg)) // prool: crash here
		{
			check_hiding_cmd(ch, -1);
			return (1);
		}

	// special in equipment list? //
	for (j = 0; j < NUM_WEARS; j++)
		if (GET_EQ(ch, j) && GET_OBJ_SPEC(GET_EQ(ch, j)) != NULL)
			if (GET_OBJ_SPEC(GET_EQ(ch, j))(ch, GET_EQ(ch, j), cmd, arg))
			{
				check_hiding_cmd(ch, -1);
				return (1);
			}

	// special in inventory? //
	for (i = ch->carrying; i; i = i->next_content)
		if (GET_OBJ_SPEC(i) != NULL)
			if (GET_OBJ_SPEC(i)(ch, i, cmd, arg))
			{
				check_hiding_cmd(ch, -1);
				return (1);
			}

	// special in mobile present? //
//Polud ����� �������� �� ������ ���� ����� � ����� �������, ������������ ����������� ��������� �� �� ������
	int specialNum = 1; //���� ����� �� ������ - �� ��������� ������� ������
	for (k = world[ch->in_room]->people; k; k = k->next_in_room)
	{
		if (GET_MOB_SPEC(k) != NULL && (fnum == 1 || fnum == specialNum++))
		{
			if (GET_MOB_SPEC(k)(ch, k, cmd, arg))
			{
				check_hiding_cmd(ch, -1);
				return (1);
			}
		}
	}

	// special in object present? //
	for (i = world[ch->in_room]->contents; i; i = i->next_content)
	{
		if (GET_OBJ_SPEC(i) != NULL)
		{
			if (GET_OBJ_SPEC(i)(ch, i, cmd, arg))
			{
				check_hiding_cmd(ch, -1);
				return (1);
			}
		}
	}

	return (0);
}



// **************************************************************************
// *  Stuff for controlling the non-playing sockets (get name, pwd etc)     *
// **************************************************************************


// locate entry in p_table with entry->name == name. -1 mrks failed search
int find_name(const char *name)
{
	int i;

	for (i = 0; i <= top_of_p_table; i++)
	{
		if (!str_cmp((player_table + i)->name, name))
			return (i);
	}

	return (-1);
}

int _parse_name(char *arg, char *name)
{
	int i;

	// skip whitespaces
	for (i = 0; (*name = (i ? LOWER(*arg) : UPPER(*arg))); arg++, i++, name++)
		if (*arg == '�' || *arg == '�' || !a_isalpha(*arg) || *arg > 0)
			return (1);

	if (!i)
		return (1);

	return (0);
}

/**
* ������ ��� ���� ������ ������� _parse_name ��� ��� ��������� ����� �����,
* ����� �� � ���� ������ �������, � ����� � �/� �������������� �����.
*/
int parse_exist_name(char *arg, char *name)
{
	int i;

	// skip whitespaces
	for (i = 0; (*name = (i ? LOWER(*arg) : UPPER(*arg))); arg++, i++, name++)
		if (!a_isalpha(*arg) || *arg > 0)
			return (1);

	if (!i)
		return (1);

	return (0);
}

#define RECON     1
#define USURP     2
#define UNSWITCH  3

/*
 * XXX: Make immortals 'return' instead of being disconnected when switched
 *      into person returns.  This function seems a bit over-extended too.
 */
int perform_dupe_check(DESCRIPTOR_DATA * d)
{
	DESCRIPTOR_DATA *k, *next_k;
	CHAR_DATA *target = NULL, *ch, *next_ch;
	int mode = 0;

	int id = GET_IDNUM(d->character);

	/*
	 * Now that this descriptor has successfully logged in, disconnect all
	 * other descriptors controlling a character with the same ID number.
	 */

	for (k = descriptor_list; k; k = next_k)
	{
		next_k = k->next;
		if (k == d)
			continue;

		if (k->original && (GET_IDNUM(k->original) == id))  	// switched char
		{
			if (str_cmp(d->host, k->host))
			{
				sprintf(buf,
						"��������� ���� !!! ID = %ld �������� = %s ���� = %s(��� %s)",
						GET_IDNUM(d->character), GET_NAME(d->character), d->host, k->host);
				mudlog(buf, BRF, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), SYSLOG, TRUE);
				//send_to_gods(buf);
			}
			SEND_TO_Q("\r\n������� ������� ����� - �����������.\r\n", k);
			STATE(k) = CON_CLOSE;
			if (!target)
			{
				target = k->original;
				mode = UNSWITCH;
			}
			if (k->character)
				k->character->desc = NULL;
			k->character = NULL;
			k->original = NULL;
		}
		else if (k->character && (GET_IDNUM(k->character) == id))
		{
			if (str_cmp(d->host, k->host))
			{
				sprintf(buf,
						"��������� ���� !!! ID = %ld Name = %s Host = %s(was %s)",
						GET_IDNUM(d->character), GET_NAME(d->character), d->host, k->host);
				mudlog(buf, BRF, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), SYSLOG, TRUE);
				//send_to_gods(buf);
			}

			if (!target && STATE(k) == CON_PLAYING)
			{
				SEND_TO_Q("\r\n���� ���� ��� ���-�� ������!\r\n", k);
				target = k->character;
				mode = USURP;
			}
			k->character->desc = NULL;
			k->character = NULL;
			k->original = NULL;
			SEND_TO_Q("\r\n������� ������� ����� - �����������.\r\n", k);
			STATE(k) = CON_CLOSE;
		}
	}

	/*
	 * now, go through the character list, deleting all characters that
	 * are not already marked for deletion from the above step (i.e., in the
	 * CON_HANGUP state), and have not already been selected as a target for
	 * switching into.  In addition, if we haven't already found a target,
	 * choose one if one is available (while still deleting the other
	 * duplicates, though theoretically none should be able to exist).
	 */

	for (ch = character_list; ch; ch = next_ch)
	{
		next_ch = ch->next;

		if (IS_NPC(ch))
			continue;
		if (GET_IDNUM(ch) != id)
			continue;

		// ignore chars with descriptors (already handled by above step) //
		if (ch->desc)
			continue;

		// don't extract the target char we've found one already //
		if (ch == target)
			continue;

		// we don't already have a target and found a candidate for switching //
		if (!target)
		{
			target = ch;
			mode = RECON;
			continue;
		}

		// we've found a duplicate - blow him away, dumping his eq in limbo. //
		if (ch->in_room != NOWHERE)
			char_from_room(ch);
		char_to_room(ch, STRANGE_ROOM);
		extract_char(ch, FALSE);
	}

	// no target for swicthing into was found - allow login to continue //
	if (!target)
		return (0);

	// Okay, we've found a target.  Connect d to target. //

	delete d->character;	// get rid of the old char //
	d->character = target;
	d->character->desc = d;
	d->original = NULL;
	d->character->char_specials.timer = 0;
	REMOVE_BIT(PLR_FLAGS(d->character, PLR_MAILING), PLR_MAILING);
	REMOVE_BIT(PLR_FLAGS(d->character, PLR_WRITING), PLR_WRITING);
	STATE(d) = CON_PLAYING;

	switch (mode)
	{
	case RECON:
//    toggle_compression(d);
		SEND_TO_Q("���������������.\r\n", d);
		check_light(d->character, LIGHT_NO, LIGHT_NO, LIGHT_NO, LIGHT_NO, 1);
		act("$n �����������$g �����.", TRUE, d->character, 0, 0, TO_ROOM);
		sprintf(buf, "%s [%s] has reconnected.", GET_NAME(d->character), d->host);
		mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), SYSLOG, TRUE);
		perslog("reconnected", GET_NAME(d->character)); // prool
		login_change_invoice(d->character);
		break;
	case USURP:
//    toggle_compression(d);
		SEND_TO_Q("���� ���� ����� ��������� � ����, ������� ��� ����� �� �����������!\r\n", d);
		act("$n ��������$u �� ����, ��������$w ����� �����...\r\n"
			"���� $s ���� ��������� ����� �����!", TRUE, d->character, 0, 0, TO_ROOM);
		sprintf(buf, "%s has re-logged in ... disconnecting old socket.", GET_NAME(d->character));
		mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), SYSLOG, TRUE);

		break;
	case UNSWITCH:
//    toggle_compression(d);
		SEND_TO_Q("��������������� ��� ������������� ������.", d);
		sprintf(buf, "%s [%s] has reconnected.", GET_NAME(d->character), d->host);
		mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), SYSLOG, TRUE);
		break;
	}
	add_logon_record(d);
	return (1);
}

int pre_help(CHAR_DATA * ch, char *arg)
{
	char command[MAX_INPUT_LENGTH], topic[MAX_INPUT_LENGTH];

	half_chop(arg, command, topic);

	if (!*command || strlen(command) < 2 || !*topic || strlen(topic) < 2)
		return (0);
	if (isname(command, "������ help �������"))
	{
		do_help(ch, topic, 0, 0);
		return (1);
	}
	return (0);
}

// ������ ������ ��� ���������� ��, ������ ��� ��� ��������� ������������, ���� ��������
// ����� ��������� � �� - ����� ��� ������, ��� �������� ���� �������, ����� ��� ���� ����� ��������
// � ������ ��� �� ����� �� ����������� ����� �������... ������ ����� ���, ��� �� ����� �)
int check_dupes_host(DESCRIPTOR_DATA * d, bool autocheck = 0)
{
	if (!d->character || IS_IMMORTAL(d->character))
		return 1;

	// � ������ ����������� ������ �������� ��� ��������� �� ����� � �������
	if (!autocheck)
	{
		if (RegisterSystem::is_registered(d->character))
			return 1;
		if (RegisterSystem::is_registered_email(GET_EMAIL(d->character)))
		{
			d->registered_email = 1;
			return 1;
		}
	}

	for (DESCRIPTOR_DATA* i = descriptor_list; i; i = i->next)
	{
		if (i != d
				&& i->ip == d->ip
				&& i->character
				&& !IS_IMMORTAL(i->character)
				&& (STATE(i) == CON_PLAYING || STATE(i) == CON_MENU))
		{
			switch (2/*CheckProxy(d)*/) // prool: multing enable!
			{
			case 0:
				// ���� ��� ����� � ������, �� ������ ������� ��������
				if (1/*IN_ROOM(d->character) == r_unreg_start_room || d->character->get_was_in_room() == r_unreg_start_room*/) // prool: �������� �����
					return 0;
				send_to_char(d->character,
							 "&R�� ����� � ������� %s � ������ IP(%s)!\r\n"
							 "��� ���������� ���������� � ����� ��� �����������.\r\n"
							 "���� �� ������ �������� � ������� ��� �������������������� �������.&n\r\n",
							 GET_PAD(i->character, 4), i->host);
				sprintf(buf,
						"! ���� � ������ IP ! ��������������������� ������.\r\n"
						"����� - %s, � ���� - %s, IP - %s.\r\n"
						"����� ������� � ������� �������������������� �������.",
						GET_NAME(d->character), GET_NAME(i->character), d->host);
				mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), SYSLOG, TRUE);
				return 0;
			case 1:
				if (autocheck) return 1;
				send_to_char("&R� ������ IP ������ ��������� ����������� ���������� ���������� �������.\r\n"
							 "���������� � ����� ��� ���������� ������ ������� � ������ ������.&n", d->character);
				return 0;
			default:
				return 1;
			}
		}
	}
	return 1;
}

int check_dupes_email(DESCRIPTOR_DATA * d)
{
	CHAR_DATA *ch;

	if (!d->character || IS_IMMORTAL(d->character))
		return (1);

	for (ch = character_list; ch; ch = ch->next)
	{
		if (ch == d->character)
			continue;
		if (IS_NPC(ch))
			continue;
		if (0/*!IS_IMMORTAL(ch) && (!str_cmp(GET_EMAIL(ch), GET_EMAIL(d->character)))*/) // prool: ������ ���� ��������
		{
			sprintf(buf, "�� �� ������ ����� ������������ � %s!\r\n"
					"���������� email �����!\r\n", GET_PAD(ch, 4));
			send_to_char(buf, d->character);
			return (0);
		}
	}
	return (1);
}
void add_logon_record(DESCRIPTOR_DATA * d)
{
	log("Enter logon list");
	// ��������� ������ � LOG_LIST
	if (LOGON_LIST(d->character) == 0)
	{
		LOGON_LIST(d->character) = new(struct logon_data);
		LOGON_LIST(d->character)->ip = str_dup(d->host);
		LOGON_LIST(d->character)->count = 1;
		LOGON_LIST(d->character)->lasttime = time(0);
		LOGON_LIST(d->character)->next = 0;
	}
	else
	{
		// ���� ���� �� ������ � logon-�
		struct logon_data * cur_log = LOGON_LIST(d->character);
		struct logon_data * last_log = cur_log;
		bool ipfound = false;
		while (cur_log)
		{
			if (!strcmp(cur_log->ip, d->host))
			{
				// �������
				cur_log->count++;
				cur_log->lasttime = time(0);
				ipfound = true;
				break;
			}
			last_log = cur_log;
			cur_log = cur_log->next;
		};
		if (!ipfound)
		{
			last_log->next = new(struct logon_data);
			last_log = last_log->next;
			last_log->ip = str_dup(d->host);
			last_log->count = 1;
			last_log->lasttime = time(0);
			last_log->next = 0;
		}
		//
	}
	int pos = get_ptable_by_unique(GET_UNIQUE(d->character));
	if (pos >= 0) {
		if (player_table[pos].last_ip)
			free(player_table[pos].last_ip);
		player_table[pos].last_ip = str_dup(d->host);
		player_table[pos].last_logon = LAST_LOGON(d->character);
	}
	log("Exit logon list");
}

// * �������� �� ��������� ������� ���������� ����� (�� ������� ��������� ����).
void check_religion(CHAR_DATA *ch)
{
	if (class_religion[ch->get_class()] == RELIGION_POLY && GET_RELIGION(ch) != RELIGION_POLY)
	{
		GET_RELIGION(ch) = RELIGION_POLY;
		log("Change religion to poly: %s", ch->get_name());
	}
	else if (class_religion[ch->get_class()] == RELIGION_MONO && GET_RELIGION(ch) != RELIGION_MONO)
	{
		GET_RELIGION(ch) = RELIGION_MONO;
		log("Change religion to mono: %s", ch->get_name());
	}
}

void do_entergame(DESCRIPTOR_DATA * d)
{
	int load_room, cmd, flag = 0;
	CHAR_DATA *ch;

	reset_char(d->character);
	read_aliases(d->character);

	if (GET_LEVEL(d->character) == LVL_IMMORT)
	{
		d->character->set_level(LVL_GOD);
	}
	if (GET_LEVEL(d->character) > LVL_IMPL)
	{
		d->character->set_level(1);
	}
	if (GET_INVIS_LEV(d->character) > LVL_IMPL || GET_INVIS_LEV(d->character) < 0)
		GET_INVIS_LEV(d->character) = 0;
	if (GET_LEVEL(d->character) > LVL_IMMORT
			&& GET_LEVEL(d->character) < LVL_BUILDER
			&& (d->character->get_gold() > 0 || d->character->get_bank() > 0))
	{
		d->character->set_gold(0);
		d->character->set_bank(0);
	}
	if (GET_LEVEL(d->character) >= LVL_IMMORT && GET_LEVEL(d->character) < LVL_IMPL)
	{
		for (cmd = 0; *cmd_info[cmd].command != '\n'; cmd++)
		{
			if (!strcmp(cmd_info[cmd].command, "syslog"))
				if (Privilege::can_do_priv(d->character, std::string(cmd_info[cmd].command), cmd, 0))
				{
					flag = 1;
					break;
				}
		}
		if (!flag)
			GET_LOGS(d->character)[0] = 0;
	}

	if (GET_LEVEL(d->character) < LVL_IMPL && !Privilege::check_flag(d->character, Privilege::KRODER))
	{
		if (PLR_FLAGGED(d->character, PLR_INVSTART))
			GET_INVIS_LEV(d->character) = LVL_IMMORT;
		if (GET_INVIS_LEV(d->character) > GET_LEVEL(d->character))
			GET_INVIS_LEV(d->character) = GET_LEVEL(d->character);

		if (PRF_FLAGGED(d->character, PRF_CODERINFO))
			REMOVE_BIT(PRF_FLAGS(d->character, PRF_CODERINFO), PRF_CODERINFO);
		if (GET_LEVEL(d->character) < LVL_GOD)
		{
			if (PRF_FLAGGED(d->character, PRF_HOLYLIGHT))
				REMOVE_BIT(PRF_FLAGS(d->character, PRF_HOLYLIGHT), PRF_HOLYLIGHT);
		}
		if (GET_LEVEL(d->character) < LVL_GRGOD)
		{
			if (PRF_FLAGGED(d->character, PRF_NOHASSLE))
				REMOVE_BIT(PRF_FLAGS(d->character, PRF_NOHASSLE), PRF_NOHASSLE);
			if (PRF_FLAGGED(d->character, PRF_ROOMFLAGS))
				REMOVE_BIT(PRF_FLAGS(d->character, PRF_ROOMFLAGS), PRF_ROOMFLAGS);
		}
		if (GET_INVIS_LEV(d->character) > 0 && GET_LEVEL(d->character) < LVL_IMMORT)
			GET_INVIS_LEV(d->character) = 0;
	}

	OfftopSystem::set_flag(d->character);
	// �������� ������������� ��, ���� �����
	check_max_hp(d->character);
	// �������� � ��� �������
	check_religion(d->character);

	/*
	 * We have to place the character in a room before equipping them
	 * or equip_char() will gripe about the person in NOWHERE.
	 */
	if (PLR_FLAGGED(d->character, PLR_HELLED))
		load_room = r_helled_start_room;
	else if (PLR_FLAGGED(d->character, PLR_NAMED))
		load_room = r_named_start_room;
	else if (PLR_FLAGGED(d->character, PLR_FROZEN))
		load_room = r_frozen_start_room;
	else if (0/*!check_dupes_host(d)*/) // prool: �������� �����
		load_room = r_unreg_start_room;
	else
	{
		if ((load_room = GET_LOADROOM(d->character)) == NOWHERE)
			load_room = calc_loadroom(d->character);
		load_room = real_room(load_room);

		if (!Clan::MayEnter(d->character, load_room, HCE_PORTAL))
			load_room = Clan::CloseRent(load_room);

		if (!is_rent(load_room))
			load_room = NOWHERE;
	}

	// If char was saved with NOWHERE, or real_room above failed...
	if (load_room == NOWHERE)
	{
		if (GET_LEVEL(d->character) >= LVL_IMMORT)
			load_room = r_immort_start_room;
		else
			load_room = r_mortal_start_room;
	}

	send_to_char(WELC_MESSG, d->character);

	for (ch = character_list; ch; ch = ch->next)
		if (ch == d->character)
			break;

	if (!ch)
	{
		d->character->next = character_list;
		character_list = d->character;
//		CharacterAlias::add(d->character);
	}
	else
	{
		REMOVE_BIT(MOB_FLAGS(ch, MOB_DELETE), MOB_DELETE);
		REMOVE_BIT(MOB_FLAGS(ch, MOB_FREE), MOB_FREE);
	}

	log("Player %s enter at room %d", GET_NAME(d->character), GET_ROOM_VNUM(load_room));
	char_to_room(d->character, load_room);

	// � ����� ��� �������� �� �����
	if (GET_LEVEL(d->character) != 0)
	{
		Crash_load(d->character);
		d->character->obj_bonus().update(d->character);
	}
	Depot::enter_char(d->character);
	Glory::check_freeze(d->character);
	Clan::clan_invoice(d->character, true);

	// ������ ����� ���� �� ����� ��
	if (IS_SET(PRF_FLAGS(d->character, PRF_PUNCTUAL), PRF_PUNCTUAL)
			&& !d->character->get_skill(SKILL_PUNCTUAL))
		REMOVE_BIT(PRF_FLAGS(d->character, PRF_PUNCTUAL), PRF_PUNCTUAL);

	if (IS_SET(PRF_FLAGS(d->character, PRF_AWAKE), PRF_AWAKE)
			&& !d->character->get_skill(SKILL_AWAKE))
		REMOVE_BIT(PRF_FLAGS(d->character, PRF_AWAKE), PRF_AWAKE);

	if (IS_SET(PRF_FLAGS(d->character, PRF_POWERATTACK), PRF_POWERATTACK)
			&& !can_use_feat(d->character, POWER_ATTACK_FEAT))
		REMOVE_BIT(PRF_FLAGS(d->character, PRF_POWERATTACK), PRF_POWERATTACK);

	if (IS_SET(PRF_FLAGS(d->character, PRF_GREATPOWERATTACK), PRF_GREATPOWERATTACK)
			&& !can_use_feat(d->character, GREAT_POWER_ATTACK_FEAT))
		REMOVE_BIT(PRF_FLAGS(d->character, PRF_GREATPOWERATTACK), PRF_GREATPOWERATTACK);

	if (IS_SET(PRF_FLAGS(d->character, PRF_AIMINGATTACK), PRF_AIMINGATTACK)
			&& !can_use_feat(d->character, AIMING_ATTACK_FEAT))
		REMOVE_BIT(PRF_FLAGS(d->character, PRF_AIMINGATTACK), PRF_AIMINGATTACK);

	if (IS_SET(PRF_FLAGS(d->character, PRF_GREATAIMINGATTACK), PRF_GREATAIMINGATTACK)
			&& !can_use_feat(d->character, GREAT_AIMING_ATTACK_FEAT))
		REMOVE_BIT(PRF_FLAGS(d->character, PRF_GREATAIMINGATTACK), PRF_GREATAIMINGATTACK);

	// Gorrah: ���������� ���� �� ������, ���� �� �����-�� ����� ����������
	if (IS_SET(PRF_FLAGS(d->character, PRF_IRON_WIND), PRF_IRON_WIND))
		REMOVE_BIT(PRF_FLAGS(d->character, PRF_IRON_WIND), PRF_IRON_WIND);

	// Check & remove/add natural, race & unavailable features
	for (int i = 1; i < MAX_FEATS; i++)
	{
		if(HAVE_FEAT(d->character,i) && !can_get_feat(d->character,i))
		{
			//UNSET_FEAT(d->character, i);
		} else {
			if (feat_info[i].natural_classfeat[(int) GET_CLASS(d->character)][(int) GET_KIN(d->character)])
				SET_FEAT(d->character, i);
		}
	};
	set_race_feats(d->character);

	//�������� ���� !�������������! �� �����������
	if (GET_SPELL_TYPE(d->character, SPELL_RELOCATE) == SPELL_KNOW && !IS_GOD(d->character))
	{
		GET_SPELL_TYPE(d->character, SPELL_RELOCATE) = 0;
		SET_FEAT(d->character, RELOCATE_FEAT);
	}

	// �������. ������ ����. ���������� ���� �� ������ �������.
	REMOVE_BIT(AFF_FLAGS(d->character, AFF_GROUP), AFF_GROUP);
	REMOVE_BIT(AFF_FLAGS(d->character, AFF_HORSE), AFF_HORSE);

	// �������� �������
	check_portals(d->character);

	// with the copyover patch, this next line goes in enter_player_game()
	GET_ID(d->character) = GET_IDNUM(d->character);
	GET_ACTIVITY(d->character) = number(0, PLAYER_SAVE_ACTIVITY - 1);
	d->character->set_last_logon(time(0));
	player_table[get_ptable_by_unique(GET_UNIQUE(d->character))].last_logon = LAST_LOGON(d->character);
	add_logon_record(d);
	// ����� �������������� ���� ����-�������� "���" �� ���, ����� ��� ������� �����
	// ����, ��� ������� �� �������; �����, ����� ���� ����� ��� ������ save_char()
	d->character->set_who_last(time(0));
	d->character->save_char();
	act("$n �������$g � ����.", TRUE, d->character, 0, 0, TO_ROOM);
	// with the copyover patch, this next line goes in enter_player_game()
	read_saved_vars(d->character);
	greet_mtrigger(d->character, -1);
	greet_otrigger(d->character, -1);
	greet_memory_mtrigger(d->character);
	STATE(d) = CON_PLAYING;

	const bool new_char = GET_LEVEL(d->character) <= 0 ? true : false;
	if (new_char)
	{
		SET_BIT(PRF_FLAGS(d->character, PRF_DRAW_MAP), PRF_DRAW_MAP);
		d->character->map_set_option(MapSystem::MAP_MODE_MOB_SPEC_SHOP);
		d->character->map_set_option(MapSystem::MAP_MODE_MOB_SPEC_RENT);
		d->character->map_set_option(MapSystem::MAP_MODE_MOB_SPEC_BANK);
		d->character->map_set_option(MapSystem::MAP_MODE_MOB_SPEC_TEACH);
		SET_BIT(PRF_FLAGS(d->character, PRF_ENTER_ZONE), PRF_ENTER_ZONE);
		SET_BIT(PRF_FLAGS(d->character, PRF_BOARD_MODE), PRF_BOARD_MODE);
		do_start(d->character, TRUE);
		GET_MANA_STORED(d->character) = 0;
		send_to_char(START_MESSG, d->character);
	}

	sprintf(buf, "%s ����� � ����.", GET_NAME(d->character));
	if (send_email && email_notif[0])
		{int i, silence;
		//printf("debug: send email? enter %s\n", (char *) GET_NAME(d->character));
		silence=0;
		for (i=0;i<MAX_NONAME;i++) if (!strcmp((char *) GET_NAME(d->character), noname[i])) silence=1;
		if (!silence) send_email2("VMUD", email_notif, "Login", (char *) GET_NAME(d->character));
		//else printf("no mail!\n");
		}
	perslog("login", GET_NAME(d->character)); // prool

	mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), SYSLOG, TRUE);
	look_at_room(d->character, 0);
	d->has_prompt = 0;
	login_change_invoice(d->character);
	check_light(d->character, LIGHT_NO, LIGHT_NO, LIGHT_NO, LIGHT_NO, 0);

	if (new_char)
	{
		send_to_char(
			"\r\n�������������� �������� ������� ��� ��������� ������� ���������� ������.\r\n������� ������ ����� ���� ��� ���� ������� �� �����, ��������� ����� ����\r\n",
			d->character);
		send_to_char(
			"������� ����� ��������������� ������ �����, �������� '������� �����' ��� ������������.\r\n"
			"���� �� ����������� � �� ������ �������������� ����� ������ ����� - �������� '������� �������'.\r\n",
			d->character);
	}

	Noob::check_help_message(d->character);
}

//�� ����� ��������� ������������ ����������
//��� ��� �������� � ���� ������� ��� ����
//����� � ������ �������������� ����� ���������� ����������
//��� ��� ���� ��������� ����������
bool ValidateStats(DESCRIPTOR_DATA * d)
{
	//��������� ����� ������
    if (!GloryMisc::check_stats(d->character))
		return false;
    //������������ ����� ����
    if (PlayerRace::GetKinNameByNum(GET_KIN(d->character),GET_SEX(d->character)) == KIN_NAME_UNDEFINED)
    {
        SEND_TO_Q("\r\n���-�� �������� ��, ������. ������ �������?\r\n�������� �����:\r\n", d);
        SEND_TO_Q(string(PlayerRace::ShowKinsMenu()).c_str(), d);
		SEND_TO_Q("\r\n�������� �����: ", d);
		STATE(d) = CON_RESET_KIN;
        return false;
    }
    //������������ ����� ����
    if (PlayerRace::GetRaceNameByNum(GET_KIN(d->character),GET_RACE(d->character),GET_SEX(d->character)) == RACE_NAME_UNDEFINED)
    {
		SEND_TO_Q("\r\n������ ����-������� �� ������?\r\n", d);
		SEND_TO_Q(string(PlayerRace::ShowRacesMenu(GET_KIN(d->character))).c_str(), d);
		SEND_TO_Q("\r\n�� ���� �� ������: ", d);
        STATE(d) = CON_RESET_RACE;
        return false;
    }
   return true;
}

void DoAfterPassword(DESCRIPTOR_DATA * d)
{
	int load_result;

	// Password was correct.
	load_result = GET_BAD_PWS(d->character);
	GET_BAD_PWS(d->character) = 0;
	d->bad_pws = 0;

	if (ban->is_banned(d->host) == BanList::BAN_SELECT && !PLR_FLAGGED(d->character, PLR_SITEOK))
	{
		SEND_TO_Q("��������, �� �� ������ ������� ����� ������ � ������� IP!\r\n", d);
		STATE(d) = CON_CLOSE;
		sprintf(buf, "Connection attempt for %s denied from %s", GET_NAME(d->character), d->host);
		mudlog(buf, NRM, LVL_GOD, SYSLOG, TRUE);
		return;
	}
	if (GET_LEVEL(d->character) < circle_restrict)
	{
		SEND_TO_Q("���� �������� ��������������.. ���� ��� ������� �����.\r\n", d);
		STATE(d) = CON_CLOSE;
		sprintf(buf, "Request for login denied for %s [%s] (wizlock)", GET_NAME(d->character), d->host);
		mudlog(buf, NRM, LVL_GOD, SYSLOG, TRUE);
		return;
	}

	// check and make sure no other copies of this player are logged in
	if (perform_dupe_check(d))
	{
		Clan::SetClanData(d->character);
		return;
	}

	// ��� ��������� ��������� ��� ��� ���������� � ��� ��������� ������� � ������ �������, ��� ����������� �� �������
	Clan::SetClanData(d->character);

	log("%s [%s] has connected.", GET_NAME(d->character), d->host);

	if (load_result)
	{
		sprintf(buf, "\r\n\r\n\007\007\007"
				"%s%d LOGIN FAILURE%s SINCE LAST SUCCESSFUL LOGIN.%s\r\n",
				CCRED(d->character, C_SPR), load_result,
				(load_result > 1) ? "S" : "", CCNRM(d->character, C_SPR));
		SEND_TO_Q(buf, d);
		GET_BAD_PWS(d->character) = 0;
	}
	time_t tmp_time = LAST_LOGON(d->character);
	sprintf(buf, "\r\n��������� ��� �� �������� � ��� � %s � ������ (%s).\r\n",
			rustime(localtime(&tmp_time)), GET_LASTIP(d->character));
	SEND_TO_Q(buf, d);

	//if (!GloryMisc::check_stats(d->character))
	if (!ValidateStats(d))
	{
		return;
	}

	SEND_TO_Q("\r\n* � ����� � ���������� �������� ����� ANYKEY ������� ENTER *", d);
	STATE(d) = CON_RMOTD;
}

void CreateChar(DESCRIPTOR_DATA * d)
{
	if (d->character != NULL) return;

	d->character = new Player;
	CREATE(d->character->player_specials, struct player_special_data, 1);
	memset(d->character->player_specials, 0, sizeof(struct player_special_data));
	d->character->desc = d;
}

// deal with newcomers and other non-playing sockets
void nanny(DESCRIPTOR_DATA * d, char *arg)
{
	char buf[MAX_STRING_LENGTH];
	int player_i, load_result;
	char tmp_name[MAX_INPUT_LENGTH], pwd_name[MAX_INPUT_LENGTH], pwd_pwd[MAX_INPUT_LENGTH];
	if (STATE(d) != CON_CONSOLE)
		skip_spaces(&arg);

	switch (STATE(d))
	{
		//. OLC states .
	case CON_OEDIT:
		oedit_parse(d, arg);
		break;
	case CON_REDIT:
		redit_parse(d, arg);
		break;
	case CON_ZEDIT:
		zedit_parse(d, arg);
		break;
	case CON_MEDIT:
		medit_parse(d, arg);
		break;
	case CON_TRIGEDIT:
		trigedit_parse(d, arg);
		break;
	case CON_MREDIT:
		mredit_parse(d, arg);
		break;
	case CON_CLANEDIT:
		d->clan_olc->clan->Manage(d, arg);
		break;
	case CON_SPEND_GLORY:
		if (!Glory::parse_spend_glory_menu(d->character, arg))
			Glory::spend_glory_menu(d->character);
		break;
	case CON_GLORY_CONST:
		if (!GloryConst::parse_spend_glory_menu(d->character, arg))
			GloryConst::spend_glory_menu(d->character);
		break;
	case CON_NAMED_STUFF:
		if (!NamedStuff::parse_nedit_menu(d->character, arg))
			NamedStuff::nedit_menu(d->character);
		break;
	case CON_MAP_MENU:
		d->map_options->parse_menu(d->character, arg);
		break;
	case CON_TORC_EXCH:
		ExtMoney::torc_exch_parse(d->character, arg);
		break;
	case CON_SEDIT:
	{
		try
		{
			obj_sets_olc::parse_input(d->character, arg);
		}
		catch (const std::out_of_range &e)
		{
			send_to_char(d->character, "�������������� ��������: %s", e.what());
			d->sedit.reset();
			STATE(d) = CON_PLAYING;
		}
		break;
	}
	//python_off case CON_CONSOLE:
		//python_off d->console->push(arg);
		//python_off break;
		//. End of OLC states .

	case CON_GET_KEYTABLE:
		if (strlen(arg) > 0)
			arg[0] = arg[strlen(arg) - 1];
		if (!*arg || *arg < '0' || *arg >= '0' + KT_LAST)
		{
			SEND_TO_Q("\r\nUnknown key table. Retry, please : ", d);
			return;
		};
		d->keytable = (ubyte) * arg - (ubyte) '0';
		ip_log(d->host);

#if 0 // prool
		char buf0 [512];
		sprintf(buf0,"\r\nVirtustan MUD. Online players = %i\r\n", total_players);
		SEND_TO_Q(buf0, d);
#endif

		SEND_TO_Q(GREETINGS, d);
		STATE(d) = CON_GET_NAME;
		break;
	case CON_GET_NAME:	// wait for input of name
		if (d->character == NULL)
			CreateChar(d);
		if (!*arg)
			STATE(d) = CON_CLOSE;
		else if (!str_cmp("�����", arg))
		{
			SEND_TO_Q(name_rules, d);
			SEND_TO_Q("������� ���: ", d);
			STATE(d) = CON_NEW_CHAR;
			return;
		}
		else
		{
			if (sscanf(arg, "%s %s", pwd_name, pwd_pwd) == 2)
			{
				if (parse_exist_name(pwd_name, tmp_name) ||
						(player_i = load_char(tmp_name, d->character)) < 0)
				{
					SEND_TO_Q("������������ ���. ���������, ����������.\r\n" "��� : ", d);
					return;
				}
				if (PLR_FLAGGED(d->character, PLR_DELETED) || !Password::compare_password(d->character, pwd_pwd))
				{
					SEND_TO_Q("������������ ���. ���������, ����������.\r\n" "��� : ", d);
					if (!PLR_FLAGGED(d->character, PLR_DELETED))
					{
						sprintf(buf, "Bad PW: %s [%s]", GET_NAME(d->character), d->host);
						mudlog(buf, BRF, LVL_IMMORT, SYSLOG, TRUE);
					}
					delete d->character;
					d->character = NULL;
					return;
				}
				REMOVE_BIT(PLR_FLAGS(d->character, PLR_MAILING), PLR_MAILING);
				REMOVE_BIT(PLR_FLAGS(d->character, PLR_WRITING), PLR_WRITING);
				REMOVE_BIT(PLR_FLAGS(d->character, PLR_CRYO), PLR_CRYO);
				d->character->set_pfilepos(player_i);
				GET_ID(d->character) = GET_IDNUM(d->character);
				DoAfterPassword(d);
				return;
			}
			else
				if (parse_exist_name(arg, tmp_name) ||
						strlen(tmp_name) < MIN_NAME_LENGTH ||
						strlen(tmp_name) > MAX_NAME_LENGTH ||
						!Is_Valid_Name(tmp_name) || fill_word(tmp_name) || reserved_word(tmp_name))
				{
					SEND_TO_Q("������������ ���. ���������, ����������.\r\n" "��� : ", d);
					return;
				}
				else if (!Is_Valid_Dc(tmp_name))
				{
					player_i = load_char(tmp_name, d->character);
					d->character->set_pfilepos(player_i);
					if (IS_IMMORTAL(d->character) || PRF_FLAGGED(d->character, PRF_CODERINFO))
					{
						SEND_TO_Q("����� � �������� ������ �������� ����������� � ����.\r\n", d);
					}
					else
					{
						SEND_TO_Q("����� � �������� ������ ��������� � ����.\r\n", d);
					}
					SEND_TO_Q("�� ��������� ������������� ������� ���� ��� ������.\r\n", d);
					SEND_TO_Q("��� � ������ ����� ������ : ", d);
					delete d->character;
					d->character = NULL;
					return;
				}
			if ((player_i = load_char(tmp_name, d->character)) > -1)
			{
				d->character->set_pfilepos(player_i);
				if (PLR_FLAGGED(d->character, PLR_DELETED))  	// We get a false positive from the original deleted character.
				{
					delete d->character;
					d->character = 0;
					// Check for multiple creations...
					if (!Valid_Name(tmp_name) || _parse_name(tmp_name, tmp_name))
					{
						SEND_TO_Q("������������ ���. ���������, ����������.\r\n" "��� : ", d);
						return;
					}
					CreateChar(d);
					d->character->set_pc_name(CAP(tmp_name));
					CREATE(GET_PAD(d->character, 0), char, strlen(tmp_name) + 1);
					strcpy(GET_PAD(d->character, 0), CAP(tmp_name));
					d->character->set_pfilepos(player_i);
					sprintf(buf, "�� ������������� ������� ��� %s [ Y(�) / N(�) ]? ", tmp_name);
					SEND_TO_Q(buf, d);
					STATE(d) = CON_NAME_CNFRM;
				}
				else  	// undo it just in case they are set
				{
					if (IS_IMMORTAL(d->character) || PRF_FLAGGED(d->character, PRF_CODERINFO))
					{
						SEND_TO_Q("����� � �������� ������ �������� ����������� � ����.\r\n", d);
						SEND_TO_Q("�� ��������� ������������� ������� ���� ��� ������.\r\n", d);
						SEND_TO_Q("��� � ������ ����� ������ : ", d);
						delete d->character;
						d->character = NULL;
						return;
					}
					REMOVE_BIT(PLR_FLAGS(d->character, PLR_MAILING), PLR_MAILING);
					REMOVE_BIT(PLR_FLAGS(d->character, PLR_WRITING), PLR_WRITING);
					REMOVE_BIT(PLR_FLAGS(d->character, PLR_CRYO), PLR_CRYO);
					SEND_TO_Q("�������� � ����� ������ ��� ����������. ������� ������ : ", d);
					d->idle_tics = 0;
					STATE(d) = CON_PASSWORD;
				}
			}
			else  	// player unknown -- make new character
			{


				// Check for multiple creations of a character.
				if (!Valid_Name(tmp_name) || _parse_name(tmp_name, tmp_name))
				{
					SEND_TO_Q("������������ ���. ���������, ����������.\r\n" "��� : ", d);
					return;
				}
				if (cmp_ptable_by_name(tmp_name, MIN_NAME_LENGTH + 1) >= 0)
				{
					SEND_TO_Q
					("������ ������� ������ ����� ��������� � ��� ������������ ����������.\r\n"
					 "��� ���������� ������ ������������� ��� ���������� ������� ������ ���.\r\n"
					 "���  : ", d);
					return;
				}
				d->character->set_pc_name(CAP(tmp_name));
				CREATE(GET_PAD(d->character, 0), char, strlen(tmp_name) + 1);
				strcpy(GET_PAD(d->character, 0), CAP(tmp_name));
				SEND_TO_Q(name_rules, d);
				sprintf(buf, "�� ������������� ������� ���  %s [ Y(�) / N(�) ]? ", tmp_name);
				SEND_TO_Q(buf, d);
				STATE(d) = CON_NAME_CNFRM;
			}
		}
		break;
	case CON_NAME_CNFRM:	// wait for conf. of new name
		if (UPPER(*arg) == 'Y' || UPPER(*arg) == '�')
		{
			if (ban->is_banned(d->host) >= BanList::BAN_NEW)
			{
				sprintf(buf,
						"������� �������� ��������� %s ��������� ��� [%s] (siteban)",
						GET_PC_NAME(d->character), d->host);
				mudlog(buf, NRM, LVL_GOD, SYSLOG, TRUE);
				SEND_TO_Q("��������, �������� ������ ��������� ��� ������ IP !!! ��������� !!!\r\n", d);
				STATE(d) = CON_CLOSE;
				return;
			}
			if (circle_restrict)
			{
				SEND_TO_Q("��������, �� �� ������ ������� ����� �������� � ��������� ������.\r\n", d);
				sprintf(buf,
						"������� �������� ������ ��������� %s ��������� ��� [%s] (wizlock)",
						GET_PC_NAME(d->character), d->host);
				mudlog(buf, NRM, LVL_GOD, SYSLOG, TRUE);
				STATE(d) = CON_CLOSE;
				return;
			}
			// Name auto-agreement by Alez see names.cpp //
			switch (process_auto_agreement(d))
			{
			case 0:	// Auto - agree
				sprintf(buf,
						"������� ������ ��� %s : ",
						GET_PAD(d->character, 1));
				SEND_TO_Q(buf, d);
				STATE(d) = CON_NEWPASSWD;
				return;
			case 1:	// Auto -disagree
				STATE(d) = CON_CLOSE;
				return;
			default:
				break;
			};
			SEND_TO_Q("��� ��� [ �(M)/�(F) ]? ", d);
			STATE(d) = CON_QSEX;
			return;

		}
		else if (UPPER(*arg) == 'N' || UPPER(*arg) == '�')
		{
			SEND_TO_Q("����, ���� ��������? ������, ������� ��� :)\r\n" "��� : ", d);
			d->character->set_pc_name(0);
			STATE(d) = CON_GET_NAME;
		}
		else
		{
			SEND_TO_Q("�������� Yes(��) or No(���) : ", d);
		}
		break;
	case CON_NEW_CHAR:
		if (!*arg)
		{
			STATE(d) = CON_CLOSE;
			return;
		}
		if (d->character == NULL)
			CreateChar(d);
		if (_parse_name(arg, tmp_name) ||
				strlen(tmp_name) < MIN_NAME_LENGTH ||
				strlen(tmp_name) > MAX_NAME_LENGTH ||
				!Is_Valid_Name(tmp_name) || fill_word(tmp_name) || reserved_word(tmp_name))
		{
			SEND_TO_Q("������������ ���. ���������, ����������.\r\n" "��� : ", d);
			return;
		}
		player_i = load_char(tmp_name, d->character);
		if (player_i > -1)
		{
			if (PLR_FLAGGED(d->character, PLR_DELETED))
			{
				delete d->character;
				d->character = 0;
				CreateChar(d);
			}
			else
			{
				SEND_TO_Q("����� �������� ��� ����������. �������� ������ ��� : ", d);
				delete d->character;
				d->character = 0;
				return;
			}
		}
		if (!Valid_Name(tmp_name))
		{
			SEND_TO_Q("������������ ���. ���������, ����������.\r\n" "��� : ", d);
			return;
		}
		if (cmp_ptable_by_name(tmp_name, MIN_NAME_LENGTH + 1) >= 0)
		{
			SEND_TO_Q("������ ������� ������ ����� ��������� � ��� ������������ ����������.\r\n"
					  "��� ���������� ������ ������������� ��� ���������� ������� ������ ���.\r\n"
					  "���  : ", d);
			return;
		}
		d->character->set_pc_name(CAP(tmp_name));
		CREATE(GET_PAD(d->character, 0), char, strlen(tmp_name) + 1);
		strcpy(GET_PAD(d->character, 0), CAP(tmp_name));
		if (ban->is_banned(d->host) >= BanList::BAN_NEW)
		{
			sprintf(buf, "������� �������� ��������� %s ��������� ��� [%s] (siteban)",
					GET_PC_NAME(d->character), d->host);
			mudlog(buf, NRM, LVL_GOD, SYSLOG, TRUE);
			SEND_TO_Q("��������, �������� ������ ��������� ��� ������ IP !!!���������!!!\r\n", d);
			STATE(d) = CON_CLOSE;
			return;
		}
		if (circle_restrict)
		{
			SEND_TO_Q("��������, �� �� ������ ������� ����� �������� � ��������� ������.\r\n", d);
			sprintf(buf,
					"������� �������� ������ ��������� %s ��������� ��� [%s] (wizlock)",
					GET_PC_NAME(d->character), d->host);
			mudlog(buf, NRM, LVL_GOD, SYSLOG, TRUE);
			STATE(d) = CON_CLOSE;
			return;
		}
		switch (process_auto_agreement(d))
		{
		case 0:	// Auto - agree
			sprintf(buf,
					"������� ������ ��� %s (�� ������� ������ ���� '123' ��� 'qwe', ����� ����� ���������� ����� �������) : ",
					GET_PAD(d->character, 1));
			SEND_TO_Q(buf, d);
			STATE(d) = CON_NEWPASSWD;
			return;
		case 1:	// Auto -disagree
			delete d->character;
			d->character = NULL;
			SEND_TO_Q("�������� ������ ��� : ", d);
			return;
		default:
			break;
		};
		SEND_TO_Q("��� ��� [ �(M)/�(F) ]? ", d);
		STATE(d) = CON_QSEX;
		return;
	case CON_PASSWORD:	// get pwd for known player
		/*
		 * To really prevent duping correctly, the player's record should
		 * be reloaded from disk at this point (after the password has been
		 * typed).  However I'm afraid that trying to load a character over
		 * an already loaded character is going to cause some problem down the
		 * road that I can't see at the moment.  So to compensate, I'm going to
		 * (1) add a 15 or 20-second time limit for entering a password, and (2)
		 * re-add the code to cut off duplicates when a player quits.  JE 6 Feb 96
		 */

		SEND_TO_Q("\r\n", d);

		if (!*arg)
			STATE(d) = CON_CLOSE;
		else
		{
			if (!Password::compare_password(d->character, arg))
			{
				sprintf(buf, "Bad PW: %s [%s]", GET_NAME(d->character), d->host);
				mudlog(buf, BRF, LVL_IMMORT, SYSLOG, TRUE);
				GET_BAD_PWS(d->character)++;
				d->character->save_char();
				if (++(d->bad_pws) >= max_bad_pws)  	// 3 strikes and you're out.
				{
					SEND_TO_Q("�������� ������... �������������.\r\n", d);
					STATE(d) = CON_CLOSE;
				}
				else
				{
					SEND_TO_Q("�������� ������.\r\n������ : ", d);
				}
				return;
			}
			DoAfterPassword(d);
		}
		break;

	case CON_NEWPASSWD:
	case CON_CHPWD_GETNEW:
		if (!Password::check_password(d->character, arg))
		{
			sprintf(buf, "\r\n%s\r\n", Password::BAD_PASSWORD);
			SEND_TO_Q(buf, d);
			SEND_TO_Q("������ : ", d);
			return;
		}
		Password::set_password(d->character, arg);

		SEND_TO_Q("\r\n��������� ������, ���������� : ", d);
		if (STATE(d) == CON_NEWPASSWD)
			STATE(d) = CON_CNFPASSWD;
		else
			STATE(d) = CON_CHPWD_VRFY;

		break;

	case CON_CNFPASSWD:
	case CON_CHPWD_VRFY:
		if (!Password::compare_password(d->character, arg))
		{
			SEND_TO_Q("\r\n������ �� �������������... ��������.\r\n", d);
			SEND_TO_Q("������: ", d);
			if (STATE(d) == CON_CNFPASSWD)
				STATE(d) = CON_NEWPASSWD;
			else
				STATE(d) = CON_CHPWD_GETNEW;
			return;
		}

		// commented by WorM: ����� ����� ���� ��� �� ������ �� ����� �� ���������� �� � ������� ����� ������� �������
		/*if (STATE(d) == CON_CNFPASSWD)
		{
            SEND_TO_Q("\r\n����� ����� ��� ����� �� ����:\r\n", d);
			SEND_TO_Q(string(PlayerRace::ShowKinsMenu()).c_str(), d);
			SEND_TO_Q
			("\r\n���� ����� (��� ����� ������ ���������� �� ������ �������"
			 " \r\n������� <������������ �����>): ", d);
			STATE(d) = CON_QKIN;
		}*/
		if (STATE(d) == CON_CNFPASSWD)
		{
			GET_KIN(d->character) = 0; // added by WorM: ���������� ���� � �����(������� ����)
        		SEND_TO_Q(class_menu, d);
			SEND_TO_Q("\r\n���� ��������� (��� ����� ������ ���������� �� ������ �������"
				  " \r\n������� <������������ ���������>): ", d);
			STATE(d) = CON_QCLASS;
		}
		else
		{
			d->character->save_char();
			SEND_TO_Q("\r\n������.\r\n", d);
			SEND_TO_Q(MENU, d);
			STATE(d) = CON_MENU;
		}

		break;

	case CON_QSEX:		// query sex of new user
		if (pre_help(d->character, arg))
		{
			SEND_TO_Q("\r\n��� ��� [ �(M)/�(F) ]? ", d);
			STATE(d) = CON_QSEX;
			return;
		}
		switch (UPPER(*arg))
		{
		case '�':
		case 'M':
			GET_SEX(d->character) = SEX_MALE;
			break;
		case '�':
		case 'F':
			GET_SEX(d->character) = SEX_FEMALE;
			break;
		default:
			SEND_TO_Q("��� ����� ���� � ���, �� ���� �� ��� :)\r\n" "� ����� � ��� ���? ", d);
			return;
		}
		SEND_TO_Q("��������� ������������ ��������� �����. � ������ ������ ������� ���� �������.\r\n", d);
		GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 1, tmp_name);
		sprintf(buf, "��� � ����������� ������ (��� ����?) [%s]: ", tmp_name);
		SEND_TO_Q(buf, d);
		STATE(d) = CON_NAME2;
		return;

	case CON_QKIN:		// query rass
		if (pre_help(d->character, arg))
		{
            SEND_TO_Q("\r\n����� ����� ��� ����� �� ����:\r\n", d);
            SEND_TO_Q(string(PlayerRace::ShowKinsMenu()).c_str(), d);
			SEND_TO_Q("\r\n�����: ", d);
			STATE(d) = CON_QKIN;
			return;
		}
        load_result = PlayerRace::CheckKin(arg);
		if (load_result == KIN_UNDEFINED)
		{
			SEND_TO_Q("������ �� ������� �������.\r\n"
					  "����� ����� ��� ����� �� ����? ", d);
			return;
		}
		GET_KIN(d->character) = load_result;
/*
������-����������!
���� ��� ������ ��� ������ �������� _�������_ ��� �������� �� ������� _���_.
���� �������� � ������ ������� ��������� ��� ��������.
�������� ���� ������, ��� ��� ��� �������� ����� ������ ���� ��� ��������� ������� "�������" ������.
������������ ��� ������� �����������, �� � ����������, ��� � ��������� ������� ���-�� ������ ���������� ����.
���� �� ����� �������� ��������, �� ��� ���� ��� ���� �������.
� ����� �������� ���� � �������� _���_ � ����� playerraces.xml
������ ������ ��������� ���� �������. � ��� �� ����� ������ ������� � ���������� � ������, � �� �������� ��� � 3 �����������
���� ������ ��� ���� �� ���� ����� � �� �������� �)
Sventovit
 */
        SEND_TO_Q(class_menu, d);
		SEND_TO_Q("\r\n���� ��������� (��� ����� ������ ���������� �� ������ �������"
				  " \r\n������� <������������ ���������>): ", d);
		STATE(d) = CON_QCLASS;
		break;

	case CON_RELIGION:	// query religion of new user
		if (pre_help(d->character, arg))
		{
			SEND_TO_Q(religion_menu, d);
			SEND_TO_Q("\n\r������� :", d);
			STATE(d) = CON_RELIGION;
			return;
		}

		switch (UPPER(*arg))
		{
		case '�':
		case '�':
		case 'P':
			if (class_religion[(int) GET_CLASS(d->character)] == RELIGION_MONO)
			{
				SEND_TO_Q
				("�������� ��������� ���� ��������� �� ������ ���� ���������!\r\n"
				 "��� ����� ����� �� ������ �������? ", d);
				return;
			}
			GET_RELIGION(d->character) = RELIGION_POLY;
			break;
		case '�':
		case 'C':
			if (class_religion[(int) GET_CLASS(d->character)] == RELIGION_POLY)
			{
				SEND_TO_Q
				("��������� ��������� ���� ��������� �������� ������������!\r\n"
				 "��� ����� ����� �� ������ �������? ", d);
				return;
			}
			GET_RELIGION(d->character) = RELIGION_MONO;
			break;
		default:
			SEND_TO_Q("������ ������ �� ����� :)\r\n" "��� ����� ����� �� ������ �������? ", d);
			return;
		}

		SEND_TO_Q("\r\n����� ��� ��� ����� ����� �� ����:\r\n", d);
		SEND_TO_Q(string(PlayerRace::ShowRacesMenu(GET_KIN(d->character))).c_str(), d);
		SEND_TO_Q("\r\n�� ���� �� ������ : ", d);
		STATE(d) = CON_RACE;
		break;

	case CON_QCLASS:
		if (pre_help(d->character, arg))
		{
			SEND_TO_Q(class_menu, d);
			SEND_TO_Q("\r\n���� ��������� : ", d);
			STATE(d) = CON_QCLASS;
			return;
		}
		load_result = parse_class(*arg);
		if (load_result == CLASS_UNDEFINED)
		{
			SEND_TO_Q("\r\n��� �� ���������.\r\n��������� : ", d);
			return;
		}
		else
		{
			d->character->set_class(load_result);
		}
		SEND_TO_Q(religion_menu, d);
		SEND_TO_Q("\n\r������� :", d);
		STATE(d) = CON_RELIGION;
		break;

	case CON_QCLASSS:
		if (pre_help(d->character, arg))
		{
			SEND_TO_Q(class_menu_step, d);
			SEND_TO_Q("\r\n���� ��������� : ", d);
			STATE(d) = CON_QCLASSS;
			return;
		}
		load_result = parse_class_step(*arg);
		if (load_result == CLASS_UNDEFINED)
		{
			SEND_TO_Q("\r\n��� �� ���������.\r\n��������� : ", d);
			return;
		}
		else
		{
			d->character->set_class(load_result);
		}
		SEND_TO_Q(religion_menu, d);
		SEND_TO_Q("\n\r������� :", d);
		STATE(d) = CON_RELIGION;
		break;

	case CON_QCLASSV:
		if (pre_help(d->character, arg))
		{
			SEND_TO_Q(class_menu_vik, d);
			SEND_TO_Q("\r\n���� ��������� : ", d);
			STATE(d) = CON_QCLASSV;
			return;
		}
		load_result = parse_class_vik(*arg);
		if (load_result == CLASS_UNDEFINED)
		{
			SEND_TO_Q("\r\n��� �� ���������.\r\n��������� : ", d);
			return;
		}
		else
		{
			d->character->set_class(load_result);
		}
		SEND_TO_Q(religion_menu, d);
		SEND_TO_Q("\n\r�������:", d);
		STATE(d) = CON_RELIGION;
		break;

	case CON_RACE:		// query race
		if (pre_help(d->character, arg))
		{
			SEND_TO_Q("����� ��� ��� ����� ����� �� ����:\r\n", d);
            SEND_TO_Q(string(PlayerRace::ShowRacesMenu(GET_KIN(d->character))).c_str(), d);
			SEND_TO_Q("\r\n���: ", d);
			STATE(d) = CON_RACE;
			return;
		}
        load_result = PlayerRace::CheckRace(GET_KIN(d->character), arg);
		if (load_result == RACE_UNDEFINED)
		{
			SEND_TO_Q("������ �� ������� �������.\r\n" "����� ��� ��� ����� �����? ", d);
			return;
		}
		GET_RACE(d->character) = load_result;
        SEND_TO_Q(string(BirthPlace::ShowMenu(PlayerRace::GetRaceBirthPlaces(GET_KIN(d->character),GET_RACE(d->character)))).c_str(), d);
        SEND_TO_Q("\r\n��� �� ������ ������ ���� �����������: ", d);
		STATE(d) = CON_BIRTHPLACE;
		break;

	case CON_BIRTHPLACE:
        if (pre_help(d->character, arg))
		{
			SEND_TO_Q(string(BirthPlace::ShowMenu(PlayerRace::GetRaceBirthPlaces(GET_KIN(d->character),GET_RACE(d->character)))).c_str(), d);
			SEND_TO_Q("\r\n��� �� ������ ������ ���� �����������: ", d);
			STATE(d) = CON_BIRTHPLACE;
			return;
		}
        load_result = PlayerRace::CheckBirthPlace(GET_KIN(d->character), GET_RACE(d->character), arg);
		if (!BirthPlace::CheckId(load_result))
		{
			SEND_TO_Q("�� �������? ������.\r\n"
					  "��������� ��� �����, � ��������:", d);
			return;
		}
		GET_LOADROOM(d->character) = calc_loadroom(d->character, load_result);
//		sprintf(buf, "\r\n���� ����������� �������: %5d\r\n", GET_LOADROOM(d->character));
//		SEND_TO_Q(buf, d);
		roll_real_abils(d->character);
		SEND_TO_Q(genchar_help, d);
		SEND_TO_Q("\r\n\r\n������� ����� �������.\r\n", d);
        STATE(d) = CON_ROLL_STATS;
        break;

	case CON_ROLL_STATS:
		if (pre_help(d->character, arg))
		{
			genchar_disp_menu(d->character);
			STATE(d) = CON_ROLL_STATS;
			return;
		}
		switch (genchar_parse(d->character, arg))
		{
		case GENCHAR_CONTINUE:
			genchar_disp_menu(d->character);
			break;
		default:
			// ���. ��������� ���������
			SEND_TO_Q(color_menu, d);
			SEND_TO_Q("\r\n����� :", d);
			STATE(d) = CON_COLOR;
		}
		break;

	case CON_COLOR:
		if (pre_help(d->character, arg))
		{
			SEND_TO_Q(color_menu, d);
			SEND_TO_Q("\n\r����� :", d);
			STATE(d) = CON_COLOR;
			return;
		}
		switch (UPPER(*arg))
		{
		case '0':
			snprintf(buf2, MAX_STRING_LENGTH, "����");
			break;
		case '1':
			snprintf(buf2, MAX_STRING_LENGTH, "�������");
			break;
		case '2':
			snprintf(buf2, MAX_STRING_LENGTH, "�������");
			break;
		case '3':
			snprintf(buf2, MAX_STRING_LENGTH, "������");
			break;
		default:
			SEND_TO_Q("����� ������� ���, �������� �� ��������������!", d);
			return;
		}
		do_color(d->character, buf2, 0, 0);
		SEND_TO_Q("\r\n������� ��� E-mail"
				  "\r\n(��� ���� ��������� ������ ����� ���������� E-mail): ", d);
		STATE(d) = CON_GET_EMAIL;
		break;


	case CON_GET_EMAIL:
		if (!*arg)
		{
			SEND_TO_Q("\r\n��� E-mail : ", d);
			return;
		}
		else if (!valid_email(arg))
		{
			SEND_TO_Q("\r\n������������ E-mail!" "\r\n��� E-mail :  ", d);
			return;
		}

		if (d->character->get_pfilepos() < 0)
			d->character->set_pfilepos(create_entry(GET_PC_NAME(d->character)));



		// Now GET_NAME() will work properly.
		init_char(d->character);
		strncpy(GET_EMAIL(d->character), arg, 127);
		*(GET_EMAIL(d->character) + 127) = '\0';
		lower_convert(GET_EMAIL(d->character));
		d->character->save_char();

		// ��������� � ������ ������ ���������
		if (!(int)NAME_FINE(d->character))
		{
			sprintf(buf, "%s - ����� �����. ������: %s/%s/%s/%s/%s/%s Email: %s ���: %s. ]\r\n"
					"[ %s ���� ��������� �����.",
					GET_NAME(d->character),	GET_PAD(d->character, 0),
					GET_PAD(d->character, 1), GET_PAD(d->character, 2),
					GET_PAD(d->character, 3), GET_PAD(d->character, 4),
					GET_PAD(d->character, 5), GET_EMAIL(d->character),
					genders[(int)GET_SEX(d->character)], GET_NAME(d->character));
			NewNameAdd(d->character);
		}

		SEND_TO_Q(motd, d);
		SEND_TO_Q("\r\n* � ����� � ���������� �������� ����� ANYKEY ������� ENTER *", d);
		STATE(d) = CON_RMOTD;
		d->character->set_who_mana(0);
		d->character->set_who_last(time(0));

		//sprintf(buf, "%s [%s] new player_data.", GET_NAME(d->character), d->host);
		//mudlog(buf, NRM, LVL_IMMORT, SYSLOG, TRUE);
		break;

	case CON_RMOTD:	// read CR after printing motd
		if (!check_dupes_email(d))
		{
			STATE(d) = CON_CLOSE;
			break;
		}
		do_entergame(d);
		// SEND_TO_Q(MENU, d);
		// STATE(d) = CON_MENU;
		break;

	case CON_MENU:		// get selection from main menu
		switch (*arg)
		{
		case '0':

			SEND_TO_Q("\r\n\r\n�� ������� �� ����� ��������.\r\n\r\nMUDQUIT\r\n\r\n", d);

			if (GET_REMORT(d->character) == 0 && GET_LEVEL(d->character) <= 25
					&& !IS_SET(PLR_FLAGS(d->character, PLR_NODELETE), PLR_NODELETE))
			{
				int timeout = -1;
				for (int ci = 0; GET_LEVEL(d->character) > pclean_criteria[ci].level; ci++)
				{
					//if (GET_LEVEL(d->character) == pclean_criteria[ci].level)
					timeout = pclean_criteria[ci + 1].days;
				}
				if (timeout > 0)
				{
					time_t deltime = time(NULL) + timeout * 60 * 60 * 24;
					sprintf(buf, "� ������ ������ ���������� �������� ����� ��������� �� %s ����� ��� :).\r\n",
							rustime(localtime(&deltime)));
					SEND_TO_Q(buf, d);
				}
			};

			STATE(d) = CON_CLOSE;
			break;

		case '1':
			if (!check_dupes_email(d))
			{
				STATE(d) = CON_CLOSE;
				break;
			}
			do_entergame(d);
			break;

		case '2':
			if (d->character->player_data.description)
			{
				SEND_TO_Q("���� ������� ��������:\r\n", d);
				SEND_TO_Q(d->character->player_data.description, d);
				/*
				 * Don't free this now... so that the old description gets loaded
				 * as the current buffer in the editor.  Do setup the ABORT buffer
				 * here, however.
				 *
				 * free(d->character->player_data.description);
				 * d->character->player_data.description = NULL;
				 */
				d->backstr = str_dup(d->character->player_data.description);
			}
			SEND_TO_Q
			("������� �������� ������ �����, ������� ����� ���������� �� ������� <���������>.\r\n", d);
			SEND_TO_Q("(/s ��������� /h ������)\r\n", d);
			d->str = &d->character->player_data.description;
			d->max_str = EXDSCR_LENGTH;
			STATE(d) = CON_EXDESC;
			break;

		case '3':
			page_string(d, background, 0);
			STATE(d) = CON_RMOTD;
			break;

		case '4':
			SEND_TO_Q("\r\n������� ������ ������ : ", d);
			STATE(d) = CON_CHPWD_GETOLD;
			break;

		case '5':
			if (IS_IMMORTAL(d->character))
			{
				SEND_TO_Q("\r\n���� ���������� (�) �������, ������� ���� ��������� :)))\r\n", d);
				SEND_TO_Q(MENU, d);
				break;
			}
			if (IS_SET(PLR_FLAGS(d->character, PLR_NODELETE), PLR_NODELETE))
			{
				SEND_TO_Q("\r\n���� ��������� ��� ������\r\n", d);
				SEND_TO_Q(MENU, d);
				break;
			}
			SEND_TO_Q("\r\n��� ������������� ������� ���� ������ : ", d);
			STATE(d) = CON_DELCNF1;
			break;

		case '6':
		{
			if (IS_IMMORTAL(d->character))
			{
				SEND_TO_Q("\r\n��� ��� �� � ����...\r\n", d);
				SEND_TO_Q(MENU, d);
				STATE(d) = CON_MENU;
			}
			else
			{
				ResetStats::print_menu(d);
				STATE(d) = CON_MENU_STATS;
			}
			break;
		}
		default:
			SEND_TO_Q("\r\n��� �� ���� ���������� �����!\r\n", d);
			SEND_TO_Q(MENU, d);
			break;
		}

		break;

	case CON_CHPWD_GETOLD:
		if (!Password::compare_password(d->character, arg))
		{
			SEND_TO_Q("\r\n�������� ������.\r\n", d);
			SEND_TO_Q(MENU, d);
			STATE(d) = CON_MENU;
		}
		else
		{
			SEND_TO_Q("\r\n������� ����� ������ : ", d);
			STATE(d) = CON_CHPWD_GETNEW;
		}
		return;

	case CON_DELCNF1:
		if (!Password::compare_password(d->character, arg))
		{
			SEND_TO_Q("\r\n�������� ������.\r\n", d);
			SEND_TO_Q(MENU, d);
			STATE(d) = CON_MENU;
		}
		else
		{
			SEND_TO_Q("\r\n!!! ��� �������� ����� ������ !!!\r\n"
					  "�� ��������� � ���� �������?\r\n\r\n"
					  "�������� \"YES / ��\" ��� �������������: ", d);
			STATE(d) = CON_DELCNF2;
		}
		break;

	case CON_DELCNF2:
		if (!strcmp(arg, "yes") || !strcmp(arg, "YES") || !strcmp(arg, "��") || !strcmp(arg, "��"))
		{
			if (PLR_FLAGGED(d->character, PLR_FROZEN))
			{
				SEND_TO_Q("�� �������� �� ������, �� ���� ���������� ���.\r\n", d);
				SEND_TO_Q("�������� �� ������.\r\n", d);
				STATE(d) = CON_CLOSE;
				return;
			}
			if (GET_LEVEL(d->character) >= LVL_GRGOD)
				return;
			delete_char(GET_NAME(d->character));
			sprintf(buf, "�������� '%s' ������!\r\n" "�� ��������.\r\n", GET_NAME(d->character));
			SEND_TO_Q(buf, d);
			sprintf(buf, "%s (lev %d) has self-deleted.", GET_NAME(d->character), GET_LEVEL(d->character));
			mudlog(buf, NRM, LVL_GOD, SYSLOG, TRUE);
			STATE(d) = CON_CLOSE;
			return;
		}
		else
		{
			SEND_TO_Q("\r\n�������� �� ������.\r\n", d);
			SEND_TO_Q(MENU, d);
			STATE(d) = CON_MENU;
		}
		break;
	case CON_NAME2:
		skip_spaces(&arg);
		if (strlen(arg) == 0)
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 1, arg);
		if (!_parse_name(arg, tmp_name) &&
				strlen(tmp_name) >= MIN_NAME_LENGTH && strlen(tmp_name) <= MAX_NAME_LENGTH &&
				!strn_cmp(tmp_name, GET_PC_NAME(d->character), MIN(MIN_NAME_LENGTH, strlen(GET_PC_NAME(d->character)) - 1))
		   )
		{
			CREATE(GET_PAD(d->character, 1), char, strlen(tmp_name) + 1);
			strcpy(GET_PAD(d->character, 1), CAP(tmp_name));
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 2, tmp_name);
			sprintf(buf, "��� � ��������� ������ (��������� ����?) [%s]: ", tmp_name);
			SEND_TO_Q(buf, d);
			STATE(d) = CON_NAME3;
		}
		else
		{
			SEND_TO_Q("�����������.\r\n", d);
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 1, tmp_name);
			sprintf(buf, "��� � ����������� ������ (��� ����?) [%s]: ", tmp_name);
			SEND_TO_Q(buf, d);
		};
		break;
	case CON_NAME3:
		skip_spaces(&arg);
		if (strlen(arg) == 0)
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 2, arg);
		if (!_parse_name(arg, tmp_name) &&
				strlen(tmp_name) >= MIN_NAME_LENGTH && strlen(tmp_name) <= MAX_NAME_LENGTH &&
				!strn_cmp(tmp_name, GET_PC_NAME(d->character), MIN(MIN_NAME_LENGTH, strlen(GET_PC_NAME(d->character)) - 1))
		   )
		{
			CREATE(GET_PAD(d->character, 2), char, strlen(tmp_name) + 1);
			strcpy(GET_PAD(d->character, 2), CAP(tmp_name));
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 3, tmp_name);
			sprintf(buf, "��� � ����������� ������ (������� ����?) [%s]: ", tmp_name);
			SEND_TO_Q(buf, d);
			STATE(d) = CON_NAME4;
		}
		else
		{
			SEND_TO_Q("�����������.\r\n", d);
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 2, tmp_name);
			sprintf(buf, "��� � ��������� ������ (��������� ����?) [%s]: ", tmp_name);
			SEND_TO_Q(buf, d);
		};
		break;
	case CON_NAME4:
		skip_spaces(&arg);
		if (strlen(arg) == 0)
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 3, arg);
		if (!_parse_name(arg, tmp_name) &&
				strlen(tmp_name) >= MIN_NAME_LENGTH && strlen(tmp_name) <= MAX_NAME_LENGTH &&
				!strn_cmp(tmp_name, GET_PC_NAME(d->character), MIN(MIN_NAME_LENGTH, strlen(GET_PC_NAME(d->character)) - 1))
		   )
		{
			CREATE(GET_PAD(d->character, 3), char, strlen(tmp_name) + 1);
			strcpy(GET_PAD(d->character, 3), CAP(tmp_name));
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 4, tmp_name);
			sprintf(buf, "��� � ������������ ������ (��������� � ���?) [%s]: ", tmp_name);
			SEND_TO_Q(buf, d);
			STATE(d) = CON_NAME5;
		}
		else
		{
			SEND_TO_Q("�����������.\n\r", d);
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 3, tmp_name);
			sprintf(buf, "��� � ����������� ������ (������� ����?) [%s]: ", tmp_name);
			SEND_TO_Q(buf, d);
		};
		break;
	case CON_NAME5:
		skip_spaces(&arg);
		if (strlen(arg) == 0)
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 4, arg);
		if (!_parse_name(arg, tmp_name) &&
				strlen(tmp_name) >= MIN_NAME_LENGTH && strlen(tmp_name) <= MAX_NAME_LENGTH &&
				!strn_cmp(tmp_name, GET_PC_NAME(d->character), MIN(MIN_NAME_LENGTH, strlen(GET_PC_NAME(d->character)) - 1))
		   )
		{
			CREATE(GET_PAD(d->character, 4), char, strlen(tmp_name) + 1);
			strcpy(GET_PAD(d->character, 4), CAP(tmp_name));
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 5, tmp_name);
			sprintf(buf, "��� � ���������� ������ (�������� � ���?) [%s]: ", tmp_name);
			SEND_TO_Q(buf, d);
			STATE(d) = CON_NAME6;
		}
		else
		{
			SEND_TO_Q("�����������.\n\r", d);
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 4, tmp_name);
			sprintf(buf, "��� � ������������ ������ (��������� � ���?) [%s]: ", tmp_name);
			SEND_TO_Q(buf, d);
		};
		break;
	case CON_NAME6:
		skip_spaces(&arg);
		if (strlen(arg) == 0)
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 5, arg);
		if (!_parse_name(arg, tmp_name) &&
				strlen(tmp_name) >= MIN_NAME_LENGTH && strlen(tmp_name) <= MAX_NAME_LENGTH &&
				!strn_cmp(tmp_name, GET_PC_NAME(d->character), MIN(MIN_NAME_LENGTH, strlen(GET_PC_NAME(d->character)) - 1))
		   )
		{
			CREATE(GET_PAD(d->character, 5), char, strlen(tmp_name) + 1);
			strcpy(GET_PAD(d->character, 5), CAP(tmp_name));
			sprintf(buf,
					"������� ������ ��� %s (�� ������� ������ ���� '12345' ��� 'qwe', ����� ����� ���������� ����� �������) : ",
					GET_PAD(d->character, 1));
			SEND_TO_Q(buf, d);
			STATE(d) = CON_NEWPASSWD;
		}
		else
		{
			SEND_TO_Q("�����������.\n\r", d);
			GetCase(GET_PC_NAME(d->character), GET_SEX(d->character), 5, tmp_name);
			sprintf(buf, "��� � ���������� ������ (�������� � ���?) [%s]: ", tmp_name);
			SEND_TO_Q(buf, d);
		};
		break;

	case CON_CLOSE:
		break;

	case CON_RESET_STATS:
		if (pre_help(d->character, arg))
		{
			return;
		}
		switch (genchar_parse(d->character, arg))
		{
		case GENCHAR_CONTINUE:
			genchar_disp_menu(d->character);
			break;
		default:
			// ����� ����������������� � ����� � genchar_parse ��������� ������ ���� ������ ����� � �����
			GloryMisc::recalculate_stats(d->character);
			// ����� ��������� � ����� �������
			sprintf(buf, "\r\n%s���������� �� ��������������. �)%s\r\n",
					CCIGRN(d->character, C_SPR), CCNRM(d->character, C_SPR));
			SEND_TO_Q(buf, d);
            // ��������� ������������ ������
            // ���� ���-�� �����������, ������� �������� ���� ������ ���� �� ���������.
            if (!ValidateStats(d))
                return;
            SEND_TO_Q("\r\n* � ����� � ���������� �������� ����� ANYKEY ������� ENTER *", d);
            STATE(d) = CON_RMOTD;
		}
		break;

    case CON_RESET_KIN:
		if (pre_help(d->character, arg))
		{
            SEND_TO_Q("\r\n����� ����� ��� ����� �� ����:\r\n", d);
            SEND_TO_Q(string(PlayerRace::ShowKinsMenu()).c_str(), d);
			SEND_TO_Q("\r\n�����: ", d);
			STATE(d) = CON_RESET_KIN;
			return;
		}
        load_result = PlayerRace::CheckKin(arg);
		if (load_result == KIN_UNDEFINED)
		{
			SEND_TO_Q("������ �� ������� �������.\r\n"
					  "����� ����� ��� ����� �� ����? ", d);
			return;
		}
		GET_KIN(d->character) = load_result;
        if (!ValidateStats(d))
            return;
        SEND_TO_Q("\r\n* � ����� � ���������� �������� ����� ANYKEY ������� ENTER *", d);
        STATE(d) = CON_RMOTD;
        break;

    case CON_RESET_RACE:
		if (pre_help(d->character, arg))
		{
            SEND_TO_Q("����� ��� ��� ����� ����� �� ����:\r\n", d);
            SEND_TO_Q(string(PlayerRace::ShowRacesMenu(GET_KIN(d->character))).c_str(), d);
			SEND_TO_Q("\r\n���: ", d);
			STATE(d) = CON_RESET_RACE;
			return;
		}
        load_result = PlayerRace::CheckRace(GET_KIN(d->character), arg);
		if (load_result == RACE_UNDEFINED)
		{
			SEND_TO_Q("������ �� ������� �������.\r\n" "����� ��� ��� ����� �����? ", d);
			return;
		}
		GET_RACE(d->character) = load_result;
        if (!ValidateStats(d))
            return;
		// ����������� ������ ���� ����������� ������ � do_entergame
        SEND_TO_Q("\r\n* � ����� � ���������� �������� ����� ANYKEY ������� ENTER *", d);
        STATE(d) = CON_RMOTD;
        break;

	case CON_MENU_STATS:
		ResetStats::parse_menu(d, arg);
		break;

	default:
		log("SYSERR: Nanny: illegal state of con'ness (%d) for '%s'; closing connection.",
			STATE(d), d->character ? GET_NAME(d->character) : "<unknown>");
		STATE(d) = CON_DISCONNECT;	// Safest to do.
		break;
	}
}

// ����� �� ������ ������ ���� ����� ��� ��������� � ��������, ��������� ��������� �� buffer
void GetOneParam(std::string & in_buffer, std::string & out_buffer)
{
	std::string::size_type beg_idx = 0, end_idx = 0;
	beg_idx = in_buffer.find_first_not_of(" ");

	if (beg_idx != std::string::npos)
	{
		// ������ � ���������
		if (in_buffer[beg_idx] == '\'')
		{
			if (std::string::npos != (beg_idx = in_buffer.find_first_not_of("\'", beg_idx)))
			{
				if (std::string::npos == (end_idx = in_buffer.find_first_of("\'", beg_idx)))
				{
					out_buffer = in_buffer.substr(beg_idx);
					in_buffer.clear();
				}
				else
				{
					out_buffer = in_buffer.substr(beg_idx, end_idx - beg_idx);
					in_buffer.erase(0, ++end_idx);
				}
			}
			// ������ � ����� ���������� ����� ������
		}
		else
		{
			if (std::string::npos != (beg_idx = in_buffer.find_first_not_of(" ", beg_idx)))
			{
				if (std::string::npos == (end_idx = in_buffer.find_first_of(" ", beg_idx)))
				{
					out_buffer = in_buffer.substr(beg_idx);
					in_buffer.clear();
				}
				else
				{
					out_buffer = in_buffer.substr(beg_idx, end_idx - beg_idx);
					in_buffer.erase(0, end_idx);
				}
			}
		}
		return;
	}

	in_buffer.clear();
	out_buffer.clear();
}

// ������������������� ��������� ���� ����� �� ����� ������, ���� - ��� ����� ����� ����� (�����������)
bool CompareParam(const std::string & buffer, const char *arg, bool full)
{
	if (!*arg || buffer.empty() || (full && buffer.length() != strlen(arg)))
		return 0;

	std::string::size_type i;
	for (i = 0; i != buffer.length() && *arg; ++i, ++arg)
		if (LOWER(buffer[i]) != LOWER(*arg))
			return (0);

	if (i == buffer.length())
		return (1);
	else
		return (0);
}

// ���� ����� � ������ ����������� ������
bool CompareParam(const std::string & buffer, const std::string & buffer2, bool full)
{
	if (buffer.empty() || buffer2.empty()
			|| (full && buffer.length() != buffer2.length()))
		return 0;

	std::string::size_type i;
	for (i = 0; i != buffer.length() && i != buffer2.length(); ++i)
		if (LOWER(buffer[i]) != LOWER(buffer2[i]))
			return (0);

	if (i == buffer.length())
		return (1);
	else
		return (0);
}

// ���� ���������� ������(������ ���������) �� ��� ����
DESCRIPTOR_DATA *DescByUID(int uid)
{
	DESCRIPTOR_DATA *d = 0;

	for (d = descriptor_list; d; d = d->next)
		if (d->character && GET_UNIQUE(d->character) == uid)
			break;
	return (d);
}

/**
* ���� ���������� ������ (������) �� �� ���� (� �������� ��� �����, �.�. ������ ���� �� ����������)
* \param id - ��, ������� ����
* \param playing - 0 ���� ���� ������ � ����� ���������, 1 (������) ���� ���� ������ ���������
*/
DESCRIPTOR_DATA* get_desc_by_id(long id, bool playing)
{
	DESCRIPTOR_DATA *d = 0;

	if (playing)
	{
		for (d = descriptor_list; d; d = d->next)
			if (d->character && STATE(d) == CON_PLAYING && GET_IDNUM(d->character) == id)
				break;
	}
	else
	{
		for (d = descriptor_list; d; d = d->next)
			if (d->character && GET_IDNUM(d->character) == id)
				break;
	}
	return d;
}

/**
* ���� ��� ������ �� ��� �����, ������ �������������� �������� - ��������� ��� ��� �����
* �������� ���� �� -1 ����� ������, ��� ��� ������ ���� (����� ���� ��������), ��� ���
* ��������� � �����-�����, �� ��� ��������� ��������� ����� -1
* TODO: �.�. �� ��� ��� ����� ������������ ������ ��� ���������� � ��� - ������ ������ ������ ���� ��� ��������...
* \param god �� ��������� = 0
* \return >0 - ��� ����, 0 - �� �����, -1 - �����, �� ��� �������� ��� (������ ��� god = true)
*/
long GetUniqueByName(const std::string & name, bool god)
{
	for (int i = 0; i <= top_of_p_table; ++i)
	{
		if (!str_cmp(player_table[i].name, name) && player_table[i].unique != -1)
		{
			if (!god)
				return player_table[i].unique;
			else
			{
				if (player_table[i].level < LVL_IMMORT)
					return player_table[i].unique;
				else
					return -1;
			}

		}
	}
	return 0;
}

// ���� ��� ������ �� ��� ����, ������ �������������� �������� - ��������� ��� ��� �����
std::string GetNameByUnique(long unique, bool god)
{
	std::string temp;
	for (int i = 0; i <= top_of_p_table; ++i)
		if (player_table[i].unique == unique)
		{
			if (!god)
				return (temp = player_table[i].name);
			else
			{
				if (player_table[i].level < LVL_IMMORT)
					return (temp = player_table[i].name);
				else
					return temp;
			}
		}
	return temp;
}

// ������ � name ������� �������� �� ���� � ������ �������� (��� ������)
void CreateFileName(std::string &name)
{
	for (unsigned i = 0; i != name.length(); ++i)
		name[i] = LOWER(AtoL(name[i]));
}

void ReadEndString(std::ifstream &file)
{
	char c;
	while (file.get(c))
		if (c == '\n')
			return;
}

// ������ ������� (� ������ ������ ����� ������) �� ���� ������, ��� ���������� ������� �������
void StringReplace(std::string & buffer, char s, std::string d)
{
	for (size_t index = 0; index = buffer.find(s, index), index != std::string::npos;)
	{
		buffer.replace(index, 1, d);
		index += d.length();
	}
}

// ����� ����� ��� ������
std::string ExpFormat(long long exp)
{
	std::string prefix;
	if (exp < 0)
	{
		exp = -exp;
		prefix = "-";
	}
	if (exp < 1000000)
		return (prefix + boost::lexical_cast<std::string>(exp));
	else if (exp < 1000000000)
		return (prefix + boost::lexical_cast<std::string>(exp / 1000) + " ���");
	else if (exp < 1000000000000LL)
		return (prefix + boost::lexical_cast<std::string>(exp / 1000000) + " ���");
	else
		return (prefix + boost::lexical_cast<std::string>(exp / 1000000000LL) + " ����");
}

// * ����������� ������� ������ � ������ �������
void lower_convert(std::string& text)
{
	for (std::string::iterator it = text.begin(); it != text.end(); ++it)
		*it = LOWER(*it);
}

// * ����������� ������� ������ � ������ �������
void lower_convert(char* text)
{
	while (*text)
	{
		*text = LOWER(*text);
		text++;
	}
}

// * ����������� ����� � ������ ������� + ������ ������ � ������� (��� �������������� ������ � �����������)
void name_convert(std::string& text)
{
	if (!text.empty())
	{
		lower_convert(text);
		*text.begin() = UPPER(*text.begin());
	}
}

// * ��������� ������ ������������ ������� � ���� � ����� �� ����
void single_god_invoice(CHAR_DATA* ch)
{
	TitleSystem::show_title_list(ch);
	NewNameShow(ch);
}

// * ����� ��������� ����� ������ ��� ������ �� ������������ ������� � ���� ��� � 5 �����
void god_work_invoice()
{
	for (DESCRIPTOR_DATA* d = descriptor_list; d; d = d->next)
		if (d->character && IS_IMMORTAL(d->character) && STATE(d) == CON_PLAYING)
			single_god_invoice(d->character);
}

// * ����� ���������� � ����� ���������� �� ������, �������, (������������ ���� � ������� ��� �����) ��� ������ � ��������
void login_change_invoice(CHAR_DATA* ch)
{
	Board::LoginInfo(ch);
	if (IS_IMMORTAL(ch))
	{
		single_god_invoice(ch);
	}
	if (mail::has_mail(ch->get_uid()))
	{
		send_to_char("&R\r\n��� ������� ������. ������� �� �����!&n\r\n", ch);
	}
	if (Parcel::has_parcel(ch))
	{
		send_to_char("&R\r\n��� ������� �������. ������� �� �����!&n\r\n", ch);
	}
	Depot::show_purged_message(ch);
	if (CLAN(ch))
	{
		CLAN(ch)->print_mod(ch);
	}
}

// ����-�������� ��� ������� ��� � ������ �� ��������
// �������� ���������� �������������� � ������������ ���� � �������
// ��������� ���� ���������� ����� #define � interpreter.h
// ���������� ������, ���� ������������ �������� � ������ �������� ���������
bool who_spamcontrol(CHAR_DATA *ch, unsigned short int mode = WHO_LISTALL)
{
	int cost = 0;
	time_t ctime;

	return false; // prool: we don't need no thought control!

	if (IS_IMMORTAL(ch))
		return false;

	ctime = time(0);

	switch (mode)
	{
		case WHO_LISTALL:
			cost = WHO_COST;
			break;
		case WHO_LISTNAME:
			cost = WHO_COST_NAME;
			break;
		case WHO_LISTCLAN:
			cost = WHO_COST_CLAN;
			break;
	}

	int mana = ch->get_who_mana();
	int last = ch->get_who_last();

#ifdef WHO_DEBUG
	send_to_char(boost::str(boost::format("\r\n����-��������:\r\n  ���� ����: %u, ������: %u\r\n") % ch->get_who_mana() % cost).c_str(), ch);
#endif

	// ������ ����, � �� �������� ����� ���� �����������
	mana = MIN(WHO_MANA_MAX, mana + (ctime - last) * WHO_MANA_REST_PER_SECOND + (ctime - last) * WHO_MANA_REST_PER_SECOND * (RENTABLE(ch) ? 1 : 0));

#ifdef WHO_DEBUG
	send_to_char(boost::str(boost::format("  ������ %u �, ������������ %u, ���� ����� ������: %u\r\n") %
	                                      (ctime - last) % (mana - ch->get_who_mana()) % mana).c_str(), ch);
#endif

	ch->set_who_mana(mana);
	ch->set_who_last(ctime);

	if (mana < cost)
	{
		send_to_char("������ ��������������, ��������...\r\n", ch);
		return true;
	}
	else
	{
		mana -= cost;
		ch->set_who_mana(mana);
	}
#ifdef WHO_DEBUG
	send_to_char(boost::str(boost::format("  �������� ����: %u\r\n") % mana).c_str(), ch);
#endif
	return false;
}

