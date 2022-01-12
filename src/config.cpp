/* ************************************************************************
*   File: config.cpp                                    Part of Bylins    *
*  Usage: Configuration of various aspects of CircleMUD operation         *
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

#define __CONFIG_C__

#include <boost/version.hpp>
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "interpreter.h"	// alias_data definition for structs.h
#include "utils.h"
#include "constants.h"
#include "char.hpp"
#include "birth_places.hpp"

#define YES	    1
#define FALSE	0
#define NO	    0

/*
 * Below are several constants which you can change to alter certain aspects
 * of the way CircleMUD acts.  Since this is a .cpp file, all you have to do
 * to change one of the constants (assuming you keep your object files around)
 * is change the constant in this file and type 'make'.  Make will recompile
 * this file and relink; you don't have to wait for the whole thing to
 * recompile as you do if you change a header file.
 *
 * I realize that it would be slightly more efficient to have lots of
 * #defines strewn about, so that, for example, the autowiz code isn't
 * compiled at all if you don't want to use autowiz.  However, the actual
 * code for the various options is quite small, as is the computational time
 * in checking the option you've selected at run-time, so I've decided the
 * convenience of having all your options in this one file outweighs the
 * efficency of doing it the other way.
 *
 */

int level_exp(CHAR_DATA * ch, int level);

// GAME PLAY OPTIONS

// exp change limits
int max_exp_gain_npc = 100000;	// max gainable per kill

// number of tics (usually 75 seconds) before PC/NPC corpses decompose
int max_npc_corpse_time = 5;
int max_pc_corpse_time = 30;

// How many ticks before a player is sent to the void or idle-rented.
int idle_void = 20; // 10; // prool
int idle_rent_time = 70; // 40; // prool

// This level and up is immune to idling, LVL_IMPL+1 will disable it.
int idle_max_level = LVL_IMMORT;

// should items in death traps automatically be junked?
int dts_are_dumps = YES;

/*
 * Whether you want items that immortals load to appear on the ground or not.
 * It is most likely best to set this to 'YES' so that something else doesn't
 * grab the item before the immortal does, but that also means people will be
 * able to carry around things like boards.  That's not necessarily a bad
 * thing, but this will be left at a default of 'NO' for historic reasons.
 */
int load_into_inventory = YES;

// "okay" etc.
const char *OK = "�������.\r\n";
const char *NOPERSON = "��� ������ �������� � ���� ����.\r\n";
const char *NOEFFECT = "���� ������ ��������� ����������.\r\n";

/*
 * You can define or not define TRACK_THOUGH_DOORS, depending on whether
 * or not you want track to find paths which lead through closed or
 * hidden doors. A setting of 'NO' means to not go through the doors
 * while 'YES' will pass through doors to find the target.
 */
//int track_through_doors = YES;

// RENT/CRASHSAVE OPTIONS

/*
 * Should the MUD allow you to 'rent' for free?  (i.e. if you just quit,
 * your objects are saved at no cost, as in Merc-type MUDs.)
 */
int free_rent = YES; // prool: free rent!

// maximum number of items players are allowed to rent
//int max_obj_save = 120;

// receptionist's surcharge on top of item costs
int min_rent_cost(CHAR_DATA * ch)
{
	if (1/*(GET_LEVEL(ch) < 15) && (GET_REMORT(ch) == 0)*/) // prool: min_rent_cost = 0
		return (0);
	else
		return ((GET_LEVEL(ch) + 30 * GET_REMORT(ch)) * 2);
}

/*
 * Should the game automatically save people?  (i.e., save player data
 * every 4 kills (on average), and Crash-save as defined below.  This
 * option has an added meaning past bpl13.  If auto_save is YES, then
 * the 'save' command will be disabled to prevent item duplication via
 * game crashes.
 */
int auto_save = YES;

/*
 * if auto_save (above) is yes, how often (in minutes) should the MUD
 * Crash-save people's objects?   Also, this number indicates how often
 * the MUD will Crash-save players' houses.
 */
int autosave_time = 5;

// Lifetime of crashfiles, forced-rent and idlesave files in days
int crash_file_timeout = 30;

// Lifetime of normal rent files in days
int rent_file_timeout = 30;

// The period of free rent after crash or forced-rent in hours
int free_crashrent_period = 2;

/* ������� ������������
   � ���� ��������� �������� ���������� ������� ���������� ������ ������
   ������������ ��� ������������ ����. ������ ������ ��������� �
   ������������ ������� � ���������� ����� ������� � ����� �������.
   ������� -1 ���������� ��������� ������� - �.�. ����� ������� �������
   ��� ��������� ����������.
   ���� ���������� ���� ����� -1, �� �� ������� �������
*/
struct pclean_criteria_data pclean_criteria[] =
{
	//     �������           ���
	{ -1, 0},		// ��������� ���� - ������� �����
	{0, 0},			// ���� 0�� ������ ������� �� ������ � ����, ��� ��� ����� ������� �����
	{1, 7},
	{2, 14},
	{3, 21},
	{4, 28},
	{5, 35},
	{6, 42},
	{7, 49},
	{8, 56},
	{9, 63},
	{10, 70},
	{11, 77},
	{12, 84},
	{13, 91},
	{14, 98},
	{15, 105},
	{16, 112},
	{17, 119},
	{18, 126},
	{19, 133},
	{20, 140},
	{21, 147},
	{22, 154},
	{23, 161},
	{24, 168},
	{25, 360},
	{LVL_IMPL, -1},		// c 25�� � ������ ����� �����
	{ -2, 0}			// ��������� ������������ ������
};


// ROOM NUMBERS

// virtual number of room that mortals should enter at
room_vnum mortal_start_room = 9900;	// tavern in village

// virtual number of room that immorts should enter at by default
room_vnum immort_start_room = 100;	// place  in castle

// virtual number of room that frozen players should enter at
room_vnum frozen_start_room = 121;	// something in castle

// virtual number of room that helled players should enter at
room_vnum helled_start_room = 102;	// something in castle
room_vnum named_start_room = 122;
room_vnum unreg_start_room = 123;


// GAME OPERATION OPTIONS

/*
 * This is the default port on which the game should run if no port is
 * given on the command-line.  NOTE WELL: If you're using the
 * 'autorun' script, the port number there will override this setting.
 * Change the PORT= line in autorun instead of (or in addition to)
 * changing this.
 */
ush_int DFLT_PORT = 4000;

/*
 * IP address to which the MUD should bind.  This is only useful if
 * you're running Circle on a host that host more than one IP interface,
 * and you only want to bind to *one* of them instead of all of them.
 * Setting this to NULL (the default) causes Circle to bind to all
 * interfaces on the host.  Otherwise, specify a numeric IP address in
 * dotted quad format, and Circle will only bind to that IP address.  (Of
 * course, that IP address must be one of your host's interfaces, or it
 * won't work.)
 */
const char *DFLT_IP = NULL;	// bind to all interfaces
// const char *DFLT_IP = "192.168.1.1";  -- bind only to one interface

// default directory to use as data directory
const char *DFLT_DIR = "lib";

/*
 * What file to log messages to (ex: "log/syslog").  Setting this to NULL
 * means you want to log to stderr, which was the default in earlier
 * versions of Circle.  If you specify a file, you don't get messages to
 * the screen. (Hint: Try 'tail -f' if you have a UNIX machine.)
 */
const char *LOGNAME = NULL;
// const char *LOGNAME = "log/syslog";  -- useful for Windows users

// maximum number of players allowed before game starts to turn people away
int max_playing = 300;

// maximum size of bug, typo and idea files in bytes (to prevent bombing)
int max_filesize = 500000;

// maximum number of password attempts before disconnection
int max_bad_pws = 3;

/*
 * Rationale for enabling this, as explained by naved@bird.taponline.com.
 *
 * Usually, when you select ban a site, it is because one or two people are
 * causing troubles while there are still many people from that site who you
 * want to still log on.  Right now if I want to add a new select ban, I need
 * to first add the ban, then SITEOK all the players from that site except for
 * the one or two who I don't want logging on.  Wouldn't it be more convenient
 * to just have to remove the SITEOK flags from those people I want to ban
 * rather than what is currently done?
 */
int siteok_everyone = TRUE;

/*
 * Some nameservers are very slow and cause the game to lag terribly every
 * time someone logs in.  The lag is caused by the gethostbyaddr() function
 * which is responsible for resolving numeric IP addresses to alphabetic names.
 * Sometimes, nameservers can be so slow that the incredible lag caused by
 * gethostbyaddr() isn't worth the luxury of having names instead of numbers
 * for players' sitenames.
 *
 * If your nameserver is fast, set the variable below to NO.  If your
 * nameserver is slow, of it you would simply prefer to have numbers
 * instead of names for some other reason, set the variable to YES.
 *
 * You can experiment with the setting of nameserver_is_slow on-line using
 * the SLOWNS command from within the MUD.
 */

int nameserver_is_slow = YES;


const char *MENU = "\r\n"
				   "0) �������������.\r\n"
				   "1) ������ ����.\r\n"
				   "2) ������ �������� ������ ���������.\r\n"
				   "3) ������ �������.\r\n"
				   "4) �������� ������.\r\n"
				   "5) ������� ���������.\r\n"
				   "6) �������� ��������� ���������.\r\n"
				   "\r\n"
				   "   ���� ���� ���� ������? ";

const char *WELC_MESSG =
	"\r\n"
	"  ����� ���������� � �����, ������� �������� � ������ ������������\r\n"
	"�������������. ��������, ��� ��� ����������, � �� ������� � ���� ��� � �������\r\n" "����� �������� ����.\r\n\r\n";

const char *START_MESSG =
	" ���� ������, ��������.\r\n"
	" ��� � �� ���� �� ����� ������������� �����������, �������, ��������, ����\r\n"
	"���� � ����� ����.\r\n"
	" ���� ������ ��������, �� ��������, ��� �� ������� �������� ������ ��.\r\n"
	" � ������ ���, ������, � �� ����� ��������� ���� ������...\r\n" "\r\n";

int max_exp_gain_pc(CHAR_DATA * ch)
{
	int result = 1;
	if (!IS_NPC(ch))
	{
		int max_per_lev =
			level_exp(ch, GET_LEVEL(ch) + 1) - level_exp(ch, GET_LEVEL(ch) + 0);
		result = max_per_lev / (10 + GET_REMORT(ch));
	}
	return result;
}

int max_exp_loss_pc(CHAR_DATA * ch)
{
	return (IS_NPC(ch) ? 1 : (level_exp(ch, GET_LEVEL(ch) + 1) - level_exp(ch, GET_LEVEL(ch) + 0)) / 3);
}

int calc_loadroom(CHAR_DATA * ch, int bplace_mode = BIRTH_PLACE_UNDEFINED)
{
	int loadroom;
    if (IS_IMMORTAL(ch))
		return (immort_start_room);
	else if (PLR_FLAGGED(ch, PLR_FROZEN))
		return (frozen_start_room);
	else
	{
        loadroom = BirthPlace::GetLoadRoom(bplace_mode);
        if (loadroom != BIRTH_PLACE_UNDEFINED)
            return loadroom;
	}
	return (mortal_start_room);
}

#if defined(BOOST_ENABLE_ASSERT_HANDLER)
void boost::assertion_failed(char const * expr, char const * function, char const * file, long line)
{
	log("Assert: expr='%s', funct='%s', file='%s', line=%ld",
		expr, function, file, line);
}

#if BOOST_VERSION >= 104600
void boost::assertion_failed_msg(char const * expr, char const * msg, char const * function, char const * file, long line)
{
	log("Assert: expr='%s', msg='%s', funct='%s', file='%s', line=%ld",
		expr, msg, function, file, line);
}
#endif
#endif
