/****************************************************************************
 *                   ^     +----- |  / ^     ^ |     | +-\                  *
 *                  / \    |      | /  |\   /| |     | |  \                 *
 *                 /   \   +---   |<   | \ / | |     | |  |                 *
 *                /-----\  |      | \  |  v  | |     | |  /                 *
 *               /       \ |      |  \ |     | +-----+ +-/                  *
 ****************************************************************************
 * AFKMud Copyright 1997-2006 by Roger Libiez (Samson),                     *
 * Levi Beckerson (Whir), Michael Ward (Tarl), Erik Wolfe (Dwip),           *
 * Cameron Carroll (Cam), Cyberfox, Karangi, Rathian, Raine, and Adjani.    *
 * All Rights Reserved.                                                     *
 * Registered with the United States Copyright Office. TX 5-877-286         *
 *                                                                          *
 * External contributions from Xorith, Quixadhal, Zarius, and many others.  *
 *                                                                          *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,        *
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,        *
 * Grishnakh, Fireblade, and Nivek.                                         *
 *                                                                          *
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.                      *
 *                                                                          *
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,      *
 * Michael Seifert, and Sebastian Hammer.                                   *
 ****************************************************************************
 *                       Intermud-3 Network Module                          *
 ****************************************************************************/

#ifndef __I3_H__
#define __I3_H__

/*
 * Copyright (c) 2000 Fatal Dimensions
 *
 * See the file "LICENSE" or information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

/* Ported to Smaug 1.4a by Samson of Alsherok.
 * Consolidated for cross-codebase compatibility by Samson of Alsherok.
 * Modifications and enhancements to the code
 * Copyright (c)2001-2003 Roger Libiez ( Samson )
 * Registered with the United States Copyright Office
 * TX 5-562-404
 */

/* Number of messages to store in the channel history */
#define MAX_I3HISTORY 250
#define MAX_I3TELLHISTORY 250

/* Locations of the configuration files */
/* Remcon: Ask and ye shall receive. */
#define I3_DIR           "i3/"
#define PLAYER_DIR       "ply/"
#define HTTP_PLACEHOLDER        "http://mud.kharkov.org/"
#define MAXPLAYERS_PLACEHOLDER  0
#define NUMLOGINS_PLACEHOLDER   0
#define RFC1123FMT              "%a, %d %b %Y %H:%M:%S %Z"
#define CODETYPE                "DikuMUD"
#define CODEBASE                VERSION_BASE
#define CODEVERSION             VERSION_BUILD

#define I3_CHANNEL_FILE  I3_DIR "i3.channels"
#define I3_CONFIG_FILE   I3_DIR "i3.config"
#define I3_BAN_FILE      I3_DIR "i3.bans"
#define I3_UCACHE_FILE   I3_DIR "i3.ucache"
#define I3_COLOR_FILE    I3_DIR "i3.color"
#define I3_HELP_FILE     I3_DIR "i3.help"
#define I3_CMD_FILE      I3_DIR "i3.commands"
#define I3_PASSWORD_FILE I3_DIR "i3.password"
#define I3_MUDLIST_FILE  I3_DIR "i3.mudlist"
#define I3_CHANLIST_FILE I3_DIR "i3.chanlist"
#define I3_ROUTER_FILE   I3_DIR "i3.routers"

#define I3STRALLOC strdup
#define I3STRFREE DESTROY
#define I3DISPOSE DESTROY
#define I3LINK LINK
#define I3UNLINK UNLINK
#define I3INSERT INSERT
#define I3CREATE CREATE
#define I3FCLOSE FCLOSE

#ifndef __IMUD_KLUDGE
#define __IMUD_KLUDGE
typedef struct descriptor_data DESCRIPTOR_DATA;
#endif

#define first_descriptor descriptor_list
#define URANGE(a, b, c)         ((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define CH_I3DATA(ch)		((ch)->i3chardata)
#define CH_I3LEVEL(ch)          (GetMaxLevel((ch)))
#define CH_I3NAME(ch)           (SAFE_NAME((ch)))
#define CH_I3SEX(ch)            (GET_SEX((ch)))
#define CH_I3TITLE(ch)          (GET_TITLE((ch)))
#define CH_I3RANK(ch)           (GET_CLASS_TITLE((ch), BestClass((ch)), GetMaxLevel((ch))))

typedef enum
{
   I3PERM_NOTSET, I3PERM_NONE, I3PERM_MORT, I3PERM_IMM, I3PERM_ADMIN, I3PERM_IMP
} permissions;

/* Flag macros */
#define I3IS_SET(flag, bit)	((flag) & (bit))
#define I3SET_BIT(var, bit)	((var) |= (bit))
#define I3REMOVE_BIT(var, bit)((var) &= ~(bit))

/* Channel flags, only one so far, but you never know when more might be useful */
#define I3CHAN_LOG      (1 <<  0)

/* Player flags */
#define I3_TELL         (1 <<  0)
#define I3_DENYTELL     (1 <<  1)
#define I3_BEEP         (1 <<  2)
#define I3_DENYBEEP     (1 <<  3)
#define I3_INVIS        (1 <<  4)
#define I3_PRIVACY      (1 <<  5)
#define I3_DENYFINGER   (1 <<  6)
#define I3_AFK          (1 <<  7)
#define I3_COLORFLAG    (1 <<  8)
#define I3_PERMOVERRIDE (1 <<  9)

/* You should not need to edit anything below this line if I've done this all correctly. */

/* The current revision of the I3 code */
#define I3DRIVER "I3 Driver 2.40b-prool"

/* IPS: Inbound packet size.
 * OPS: Outbound packet size.
 * You would be well advised not to mess with these things or Bad Things(tm) will happen to you.
 */
#define IPS 131072
#define OPS 65536

#define I3PERM(ch)            (CH_I3DATA((ch))->i3perm)
#define I3FLAG(ch)            (CH_I3DATA((ch))->i3flags)
#define FIRST_I3IGNORE(ch)    (CH_I3DATA((ch))->i3first_ignore)
#define LAST_I3IGNORE(ch)     (CH_I3DATA((ch))->i3last_ignore)
#define I3LISTEN(ch)          (CH_I3DATA((ch))->i3_listen)
#define I3DENY(ch)            (CH_I3DATA((ch))->i3_denied)
#define I3REPLYNAME(ch)       (CH_I3DATA((ch))->i3_replyname)
#define I3REPLYMUD(ch)        (CH_I3DATA((ch))->i3_replymud)
#define I3TELLHISTORY(ch,x)   (CH_I3DATA((ch))->i3_tellhistory[(x)])
#define I3INVIS(ch)           ( I3IS_SET( I3FLAG((ch)), I3_INVIS ) )
#define CH_I3AFK(ch)          ( I3IS_SET( I3FLAG((ch)), I3_AFK ) )
#define I3ISINVIS(ch)         ( I3INVIS((ch)) )

typedef struct I3_channel I3_CHANNEL;
typedef struct I3_mud I3_MUD;
typedef struct I3_header I3_HEADER;
typedef struct I3_ignore I3_IGNORE;
typedef struct I3_ban I3_BAN;
typedef struct ucache_data UCACHE_DATA;
typedef struct i3_chardata I3_CHARDATA;
typedef struct router_data ROUTER_DATA;
typedef struct i3_color_table I3_COLOR;   /* The Color config */
typedef struct i3_command_table I3_CMD_DATA; /* Command table */
typedef struct i3_help_table I3_HELP_DATA;   /* Help table */
typedef struct i3_cmd_alias I3_ALIAS;  /* Big, bad, bloated command alias thing */

typedef void I3_FUN( CHAR_DATA * ch, const char *argument );
#define I3_CMD( name ) void (name)( CHAR_DATA *ch, const char *argument )

extern int I3_socket;

extern I3_MUD *first_mud;
extern I3_MUD *this_i3mud;
extern char *I3_ROUTER_NAME;

/* Oh yeah, baby, that raunchy looking Merc structure just got the facelift of the century.
 * Thanks to Thoric and friends for the slick idea.
 */
struct i3_cmd_alias
{
   I3_ALIAS *next;
   I3_ALIAS *prev;
   char *name;
};

struct i3_command_table
{
   I3_CMD_DATA *next;
   I3_CMD_DATA *prev;
   I3_ALIAS *first_alias;
   I3_ALIAS *last_alias;
   I3_FUN *function;
   char *name;
   int level;
   bool connected;
};

struct i3_help_table
{
   I3_HELP_DATA *next;
   I3_HELP_DATA *prev;
   char *name;
   char *text;
   int level;
};

struct i3_color_table
{
   I3_COLOR *next;
   I3_COLOR *prev;
   char *name; /* the name of the color */
   char *mudtag;  /* What the mud uses for the raw tag */
   char *imctag;  /* This client's internal code that represents the mudtag to the network */
   char *i3tag;   /* The Pinkfish code for this color - bleh at having to do this twice! */
};

struct router_data
{
   ROUTER_DATA *next;
   ROUTER_DATA *prev;
   char *name;
   char *ip;
   int port;
   int reconattempts;
};

struct ucache_data
{
   UCACHE_DATA *next;
   UCACHE_DATA *prev;
   char *name;
   int gender;
   time_t time;
};

struct I3_ignore
{
   I3_IGNORE *next;
   I3_IGNORE *prev;
   char *name;
};

struct I3_ban
{
   I3_BAN *next;
   I3_BAN *prev;
   char *name;
};

struct i3_chardata
{
   I3_IGNORE *i3first_ignore; /* List of people to ignore stuff from - Samson 2-7-01 */
   I3_IGNORE *i3last_ignore;
   char *i3_replyname;  /* Target for reply - Samson 1-23-01 */
   char *i3_replymud;  /* Target for reply - Quixadhal 3-31-17 */
   char *i3_listen;  /* The I3 channels someone is listening to - Samson 1-30-01 */
   char *i3_denied;  /* The I3 channels someone is forbidden to use - Samson 6-16-03 */
   char *i3_tellhistory[MAX_I3TELLHISTORY];  /* History of received i3tells - Samson 1-21-04 */
   int i3flags;   /* Flag settings such as invis, tell on/off, beep on/off, etc. - Samson 6-30-03 */
   int i3perm; /* Your permission setting. None, All, Imm, Admin, Imp - Samson 6-25-03 */
};

struct I3_header
{
   char originator_mudname[MAX_INPUT_LENGTH];
   char originator_username[MAX_INPUT_LENGTH];
   char target_mudname[MAX_INPUT_LENGTH];
   char target_username[MAX_INPUT_LENGTH];
};

struct I3_channel
{
   I3_CHANNEL *next;
   I3_CHANNEL *prev;
   char *local_name;
   char *host_mud;
   char *I3_name;
   char *layout_m;
   char *layout_e;
   char *history[MAX_I3HISTORY];
   int status;
   int i3perm;
   long flags;
};

struct I3_mud
{
   I3_MUD *next;
   I3_MUD *prev;

   /*
    * Stuff for the first mapping set 
    */
   int status;
   char *name;
   char *ipaddress;
   char *mudlib;
   char *base_mudlib;
   char *driver;
   char *mud_type;
   char *open_status;
   char *admin_email;
   char *telnet;
   char *web_wrong;  /* This tag shows up in the wrong location on several implementations, including previous AFKMud versions */

   int player_port;
   int imud_tcp_port;
   int imud_udp_port;

   bool tell;
   bool beep;
   bool emoteto;
   bool who;
   bool finger;
   bool locate;
   bool channel;
   bool news;
   bool mail;
   bool file;
   bool auth;
   bool ucache;

   int smtp;
   int ftp;
   int nntp;
   int http;
   int pop3;
   int rcp;
   int amrcp;

   /*
    * Stuff for the second mapping set - can be added to as indicated by i3log messages for missing keys 
    */
   char *banner;
   char *web;
   char *time;
   char *daemon;
   int jeamland;

   /*
    * only used for this mud 
    */
   char *routerName;
   bool autoconnect;
   int password;
   int mudlist_id;
   int chanlist_id;
   int minlevel;
   int immlevel;
   int adminlevel;
   int implevel;
};

bool i3_is_connected( void );
size_t i3strlcpy( char *dst, const char *src, size_t siz );
size_t i3strlcat( char *dst, const char *src, size_t siz );
const char *i3one_argument( const char *argument, char *arg_first );
void i3_loop( void );
bool i3_loadchar( CHAR_DATA * ch, FILE * fp, const char *word );
void i3_savechar( CHAR_DATA * ch, FILE * fp );
void i3_freechardata( CHAR_DATA * ch );
void i3_initchar( CHAR_DATA * ch );
bool i3_command_hook( CHAR_DATA * ch, const char *command, const char *argument );
void i3_startup( bool forced, int mudport, bool isconnected );
void i3_shutdown( int delay, CHAR_DATA *ch );
void i3_npc_chat( const char *chan_name, const char *actor, const char *message );
void I3_listen_channel( CHAR_DATA *ch, const char *argument);
bool I3_hasname( char *list, const char *name );
#endif
