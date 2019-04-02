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
 *
 * I condensed the 14 or so Fatal Dimensions source code files into this
 * one file, because I for one find it far easier to maintain when all of
 * the functions are right here in one file.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fnmatch.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <pcre.h>
#include <signal.h>
//#include "global.h" // prool
//#include "bug.h" // prool
#include "utils.h"
//#include "multiclass.h"
#include "comm.h"
#include "constants.h"
#include "modify.h"
#include "interpreter.h"
//#include "version.h"
#include "i3.h"
#include "i3_2.h" // prool
#include "virtustan.h" // prool
#include "char.hpp" // prool

#define log_error printf // prool

/* Global variables for I3 */
char                                    I3_input_buffer[IPS];
char                                    I3_output_buffer[OPS];
char                                    I3_currentpacket[IPS];
bool                                    packetdebug = FALSE;   /* Packet debugging toggle, can be turned on to check
							        * outgoing packets */
long                                    I3_input_pointer = 0;
long                                    I3_output_pointer = 4;

#define I3_THISMUD (this_i3mud->name)
//char *I3_THISMUD = NULL;
char                                   *I3_ROUTER_NAME;
const char                             *manual_router;
int                                     I3_socket;
int                                     i3wait;		       /* Number of game loops to wait before attempting to
							        * reconnect when a socket dies */
int                                     i3timeout;	       /* Number of loops to wait before giving up on an
							        * initial router connection */
int                                     i3justconnected = 0;    // So we can say something for the logs.
time_t                                  ucache_clock;	       /* Timer for pruning the ucache */
long                                    bytes_received;
long                                    bytes_sent;
time_t                                  i3_time;	       /* Current clock time for the client */

I3_MUD                                 *this_i3mud = NULL;
I3_MUD                                 *first_mud;
I3_MUD                                 *last_mud;

I3_CHANNEL                             *first_I3chan;
I3_CHANNEL                             *last_I3chan;
I3_BAN                                 *first_i3ban;
I3_BAN                                 *last_i3ban;
UCACHE_DATA                            *first_ucache;
UCACHE_DATA                            *last_ucache;
ROUTER_DATA                            *first_router;
ROUTER_DATA                            *last_router;
I3_COLOR                               *first_i3_color;
I3_COLOR                               *last_i3_color;
I3_CMD_DATA                            *first_i3_command;
I3_CMD_DATA                            *last_i3_command;
I3_HELP_DATA                           *first_i3_help;
I3_HELP_DATA                           *last_i3_help;

#define TAUNT_DELAY                     PULSE_PER_SECOND * 60 * 30; /* 30 minutes worth */
int                                     tics_since_last_message = TAUNT_DELAY;

void                                    i3_printf(CHAR_DATA *ch, const char *fmt, ...)
    __attribute__ ((format(printf, 2, 3)));
void                                    i3page_printf(CHAR_DATA *ch, const char *fmt, ...)
    __attribute__ ((format(printf, 2, 3)));
void                                    i3bug(const char *format, ...)
    __attribute__ ((format(printf, 1, 2)));
void                                    i3log(const char *format, ...)
    __attribute__ ((format(printf, 1, 2)));
I3_HEADER                              *I3_get_header(char **pps);
void                                    I3_send_channel_listen(I3_CHANNEL *channel,
							       bool lconnect);
void                                    I3_write_channel_config(void);
const char                             *i3_funcname(I3_FUN *func);
I3_FUN                                 *i3_function(const char *func);
void                                    I3_saveconfig(void);
void                                    to_channel(const char *argument, char *xchannel,
						   int level);
void                                    I3_connection_close(bool reconnect);
char                                   *i3rankbuffer(CHAR_DATA *ch);
char                                   *I3_nameescape(const char *ps);
char                                   *I3_nameremap(const char *ps);
void                                    i3_npc_chat(const char *chan_name, const char *actor, const char *message);
void                                    i3_npc_speak(const char *chan_name, const char *actor, const char *message);
void                                    i3_nuke_url_file(void);
void                                    i3_check_urls(void);

#define I3KEY( literal, field, value ) \
if( !strcasecmp( word, literal ) )     \
{                                      \
   field = value;                      \
   fMatch = TRUE;                      \
   break;                              \
}

const char                             *perm_names[] = {
    "Notset", "None", "Mort", "Imm", "Admin", "Imp"
};

/* creates a random number in interval [from;to] */
int i3number(int from, int to)
{
#if 0 // prool
    if (DEBUG > 3)
	log_info("called %s with %d, %d", __PRETTY_FUNCTION__, from, to);
#endif

    if (to - from + 1)
	return ((random() % (to - from + 1)) + from);
    else
	return from;
}

/*******************************************
 * String buffering and logging functions. *
 ******************************************/

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 *
 * Renamed so it can play itself system independent.
 * Samson 10-12-03
 */
size_t i3strlcpy(char *dst, const char *src, size_t siz)
{
    register char                          *d = dst;
    register const char                    *s = src;
    register size_t                         n = siz;

    /*
     * Copy as many bytes as will fit 
     */
    if (n != 0 && --n != 0) {
	do {
	    if ((*d++ = *s++) == 0)
		break;
	}
	while (--n != 0);
    }

    /*
     * Not enough room in dst, add NUL and traverse rest of src 
     */
    if (n == 0) {
	if (siz != 0)
	    *d = '\0';					       /* NUL-terminate dst */
	while (*s++);
    }
    return (s - src - 1);				       /* count does not include NUL */
}

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(initial dst) + strlen(src); if retval >= siz,
 * truncation occurred.
 *
 * Renamed so it can play itself system independent.
 * Samson 10-12-03
 */
size_t i3strlcat(char *dst, const char *src, size_t siz)
{
    register char                          *d = dst;
    register const char                    *s = src;
    register size_t                         n = siz;
    size_t                                  dlen;

    /*
     * Find the end of dst and adjust bytes left but don't go past end 
     */
    while (n-- != 0 && *d != '\0')
	d++;
    dlen = d - dst;
    n = siz - dlen;

    if (n == 0)
	return (dlen + strlen(s));
    while (*s != '\0') {
	if (n != 1) {
	    *d++ = *s;
	    n--;
	}
	s++;
    }
    *d = '\0';
    return (dlen + (s - src));				       /* count does not include NUL */
}

const char                             *i3one_argument(const char *argument, char *arg_first)
{
    char                                    cEnd;
    int                                     count;

    count = 0;

    if (arg_first)
	arg_first[0] = '\0';

    if (!argument || argument[0] == '\0')
	return NULL;

    while (isspace(*argument))
	argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
	cEnd = *argument++;

    while (*argument != '\0' && ++count <= MAX_INPUT_LENGTH - 1) {
	if (*argument == cEnd) {
	    argument++;
	    break;
	}

	if (arg_first)
	    *arg_first++ = *argument++;
	else
	    argument++;
    }

    if (arg_first)
	*arg_first = '\0';

    while (isspace(*argument))
	argument++;

    return argument;
}

/* Generic log function which will route the log messages to the appropriate system logging function */
void i3log(const char *format, ...)
{
    char                                    buf[MAX_STRING_LENGTH],
                                            buf2[MAX_STRING_LENGTH];
    //char                                   *strtime;
    va_list                                 ap;

    va_start(ap, format);
    vsnprintf(buf, MAX_STRING_LENGTH, format, ap);
    va_end(ap);

    snprintf(buf2, MAX_STRING_LENGTH, "I3: %s", buf);

    //strtime = ctime(&i3_time);
    //fprintf(stderr, "%s :: %s\n", strtime, buf2);

    //log_info("%s", buf2); // prool
    return;
}

/* Generic bug logging function which will route the message to the appropriate function that handles bug logs */
void i3bug(const char *format, ...)
{
    char                                    buf[MAX_STRING_LENGTH];
    va_list                                 ap;

    va_start(ap, format);
    vsnprintf(buf, MAX_STRING_LENGTH, format, ap);
    va_end(ap);

    log_error("%s", buf);
    return;
}

/*
   Original Code from SW:FotE 1.1
   Reworked strrep function. 
   Fixed a few glaring errors. It also will not overrun the bounds of a string.
   -- Xorith
*/
char                                   *i3strrep(const char *src, const char *sch,
						 const char *rep)
{
    int                                     src_len = strlen(src),
	sch_len = strlen(sch),
	rep_len = strlen(rep),
	src_p,
	offset,
	dest_p;
    static char                             dest[MAX_STRING_LENGTH];
    bool                                    searching = FALSE;

    dest[0] = '\0';
    for (src_p = 0, dest_p = 0; src_p < src_len; src_p++, dest_p++) {
	if (src[src_p] == sch[0]) {
	    searching = TRUE;
	    for (offset = 0; offset < sch_len; offset++)
		if (src[src_p + offset] != sch[offset])
		    searching = FALSE;

	    if (searching) {
		for (offset = 0; offset < rep_len; offset++, dest_p++) {
		    if (dest_p == (MAX_STRING_LENGTH - 1)) {
			dest[dest_p] = '\0';
			return dest;
		    }
#if 0
		    if (src[src_p - 1] == sch[0]) {
			if (rep[0] == '\033') {
			    if (offset < sch_len) {
				if (offset == 0)
				    dest[dest_p - 1] = sch[offset];
				else
				    dest[dest_p] = sch[offset];
			    } else
				offset = rep_len;
			} else {
			    if (offset == 0)
				dest[dest_p - 1] = rep[offset];
			    dest[dest_p] = rep[offset];
			}
		    } else
#endif
			dest[dest_p] = rep[offset];
		}
		src_p += sch_len - 1;
		dest_p--;
		searching = FALSE;
		continue;
	    }
	}
	if (dest_p == (MAX_STRING_LENGTH - 1)) {
	    dest[dest_p] = '\0';
	    return dest;
	}
	dest[dest_p] = src[src_p];
    }
    dest[dest_p] = '\0';
    return dest;
}

char                                   *i3_strip_colors(const char *txt)
{
    I3_COLOR                               *color;
    static char                             tbuf[MAX_STRING_LENGTH];

    i3strlcpy(tbuf, txt, MAX_STRING_LENGTH);

    for (color = first_i3_color; color; color = color->next)
	i3strlcpy(tbuf, i3strrep(tbuf, color->i3tag, ""), MAX_STRING_LENGTH);

#ifdef IMC
//    for (color = first_i3_color; color; color = color->next)
//	i3strlcpy(tbuf, i3strrep(tbuf, color->imctag, ""), MAX_STRING_LENGTH);
#endif

    for (color = first_i3_color; color; color = color->next)
	i3strlcpy(tbuf, i3strrep(tbuf, color->mudtag, ""), MAX_STRING_LENGTH);

    return tbuf;
}

char                                   *I3_mudtag_to_i3tag(const char *txt)
{
    I3_COLOR                               *color;
    static char                             tbuf[MAX_STRING_LENGTH];

    *tbuf = '\0';
    if (!txt || *txt == '\0')
	return tbuf;

    i3strlcpy(tbuf, txt, MAX_STRING_LENGTH);
    for (color = first_i3_color; color; color = color->next)
	i3strlcpy(tbuf, i3strrep(tbuf, color->mudtag, color->i3tag), MAX_STRING_LENGTH);

    return tbuf;
}

char                                   *I3_imctag_to_i3tag(const char *txt)
{
    I3_COLOR                               *color;
    static char                             tbuf[MAX_STRING_LENGTH];

    *tbuf = '\0';
    if (!txt || *txt == '\0')
	return tbuf;

    i3strlcpy(tbuf, txt, MAX_STRING_LENGTH);
    for (color = first_i3_color; color; color = color->next)
	i3strlcpy(tbuf, i3strrep(tbuf, color->imctag, color->i3tag), MAX_STRING_LENGTH);

    return tbuf;
}

char                                   *I3_imctag_to_mudtag(CHAR_DATA *ch, const char *txt)
{
    I3_COLOR                               *color;
    static char                             tbuf[MAX_STRING_LENGTH];

    *tbuf = '\0';
    if (!txt || *txt == '\0')
	return tbuf;

    if (0/*I3IS_SET(I3FLAG(ch), I3_COLORFLAG)*/) { // prool fool
	i3strlcpy(tbuf, txt, MAX_STRING_LENGTH);
	for (color = first_i3_color; color; color = color->next)
	    i3strlcpy(tbuf, i3strrep(tbuf, color->imctag, color->mudtag), MAX_STRING_LENGTH);
    } else
	i3strlcpy(tbuf, i3_strip_colors(txt), MAX_STRING_LENGTH);

    return tbuf;
}

char                                   *I3_i3tag_to_mudtag(CHAR_DATA *ch, const char *txt)
{
    I3_COLOR                               *color;
    static char                             tbuf[MAX_STRING_LENGTH];

    *tbuf = '\0';
    if (!txt || *txt == '\0')
	return tbuf;

    if (0/*I3IS_SET(I3FLAG(ch), I3_COLORFLAG)*/) { // prool fool
	i3strlcpy(tbuf, txt, MAX_STRING_LENGTH);
	for (color = first_i3_color; color; color = color->next)
	    i3strlcpy(tbuf, i3strrep(tbuf, color->i3tag, color->mudtag), MAX_STRING_LENGTH);
    } else
	i3strlcpy(tbuf, i3_strip_colors(txt), MAX_STRING_LENGTH);

    return tbuf;
}

/********************************
 * User level output functions. *
 *******************************/

/* Generic substitute for cprintf that has color support */
void i3_printf(CHAR_DATA *ch, const char *fmt, ...)
{
    char                                    buf[MAX_STRING_LENGTH];
    char                                    buf2[MAX_STRING_LENGTH];
//#ifdef IMC
//    char                                    buf3[MAX_STRING_LENGTH];
//#endif
    va_list                                 args;

    va_start(args, fmt);
    vsnprintf(buf, MAX_STRING_LENGTH, fmt, args);
    va_end(args);

    i3strlcpy(buf2, I3_i3tag_to_mudtag(ch, buf), MAX_STRING_LENGTH);
//#ifdef IMC
//    i3strlcpy(buf3, I3_imctag_to_mudtag(ch, buf2), MAX_STRING_LENGTH);
//    cprintf(ch, "%s", buf3);
//#else
    //cprintf(ch, "%s", buf2); // prool
    send_to_char(ch, buf2); // prool
//#endif
    return;
}

/* Generic send_to_pager type function to send to the proper code for each codebase */
void i3send_to_pager(const char *txt, CHAR_DATA *ch)
{
    char                                    buf[MAX_STRING_LENGTH];
    char                                    buf2[MAX_STRING_LENGTH];

    i3strlcpy(buf, I3_i3tag_to_mudtag(ch, txt), MAX_STRING_LENGTH);
#ifdef IMC
//    i3strlcpy(buf2, I3_imctag_to_mudtag(ch, buf), MAX_STRING_LENGTH);
//    page_printf(ch, "%s\033[0m", buf2);
#else
    //page_printf(ch, "%s", buf); // prool
    send_to_char(ch, buf2); // prool
#endif
    return;
}

/* Generic page_printf type function */
void i3page_printf(CHAR_DATA *ch, const char *fmt, ...)
{
    char                                    buf[MAX_STRING_LENGTH];
    va_list                                 args;

    va_start(args, fmt);
    vsnprintf(buf, MAX_STRING_LENGTH, fmt, args);
    va_end(args);

    i3send_to_pager(buf, ch);
    return;
}

/********************************
 * Low level utility functions. *
 ********************************/

int i3todikugender(int gender)
{
    int                                     sex = 0;

    if (gender == 0)
	sex = SEX_MALE;

    if (gender == 1)
	sex = SEX_FEMALE;

    if (gender > 1)
	sex = SEX_NEUTRAL;

    return sex;
}

int dikutoi3gender(int gender)
{
    int                                     sex = 0;

    if (gender > 2 || gender < 0)
	sex = 2;					       /* I3 neuter */

    if (gender == SEX_MALE)
	sex = 0;					       /* I3 Male */

    if (gender == SEX_FEMALE)
	sex = 1;					       /* I3 Female */

    return sex;
}

int get_permvalue(const char *flag)
{
    unsigned int                            x;

    for (x = 0; x < (sizeof(perm_names) / sizeof(perm_names[0])); x++)
	if (!strcasecmp(flag, perm_names[x]))
	    return x;
    return -1;
}

/*  I3_getarg: extract a single argument (with given max length) from
 *  argument to arg; if arg==NULL, just skip an arg, don't copy it out
 */
char                                   *I3_getarg(char *argument, char *arg, int maxlen)
{
    int                                     len = 0;

    if (!argument || argument[0] == '\0') {
	if (arg)
	    arg[0] = '\0';

	return argument;
    }

    while (*argument && isspace(*argument))
	argument++;

    if (arg)
	while (*argument && !isspace(*argument) && len < maxlen - 1)
	    *arg++ = *argument++, len++;
    else
	while (*argument && !isspace(*argument))
	    argument++;

    while (*argument && !isspace(*argument))
	argument++;

    while (*argument && isspace(*argument))
	argument++;

    if (arg)
	*arg = '\0';

    return argument;
}

/* Check for a name in a list */
bool I3_hasname(char *list, const char *name)
{
    char                                   *p;
    char                                    arg[MAX_INPUT_LENGTH];

    if (!list)
	return FALSE;

    p = I3_getarg(list, arg, MAX_INPUT_LENGTH);
    while (arg[0]) {
	if (!strcasecmp(name, arg))
	    return TRUE;
	p = I3_getarg(p, arg, MAX_INPUT_LENGTH);
    }
    return FALSE;
}

/* Add a name to a list */
void I3_flagchan(char **list, const char *name)
{
    char                                    buf[MAX_STRING_LENGTH];

    if (I3_hasname(*list, name))
	return;

    if (*list && *list[0] != '\0')
	snprintf(buf, MAX_STRING_LENGTH, "%s %s", *list, name);
    else
	i3strlcpy(buf, name, MAX_STRING_LENGTH);

    I3STRFREE(*list);
    *list = I3STRALLOC(buf);
}

/* Remove a name from a list */
void I3_unflagchan(char **list, const char *name)
{
    char                                    buf[MAX_STRING_LENGTH],
                                            arg[MAX_INPUT_LENGTH];
    char                                   *p;

    buf[0] = '\0';
    p = I3_getarg(*list, arg, MAX_INPUT_LENGTH);
    while (arg[0]) {
	if (strcasecmp(arg, name)) {
	    if (buf[0])
		i3strlcat(buf, " ", MAX_STRING_LENGTH);
	    i3strlcat(buf, arg, MAX_STRING_LENGTH);
	}
	p = I3_getarg(p, arg, MAX_INPUT_LENGTH);
    }
    I3STRFREE(*list);
    *list = I3STRALLOC(buf);
}

bool i3_str_prefix(const char *astr, const char *bstr)
{
    if (!astr) {
	i3bug("Strn_cmp: null astr.");
	return TRUE;
    }

    if (!bstr) {
	i3bug("Strn_cmp: null bstr.");
	return TRUE;
    }

    for (; *astr; astr++, bstr++) {
	if (LOWER(*astr) != LOWER(*bstr))
	    return TRUE;
    }
    return FALSE;
}

/*
 * Returns an initial-capped string.
 */
char                                   *i3capitalize(const char *str)
{
    static char                             strcap[MAX_STRING_LENGTH];
    int                                     i;

    for (i = 0; str[i] != '\0'; i++)
	strcap[i] = tolower(str[i]);
    strcap[i] = '\0';
    strcap[0] = toupper(strcap[0]);
    return strcap;
}

/* Borrowed from Samson's new_auth snippet - checks to see if a particular player exists in the mud.
 * This is called from i3locate and i3finger to report on offline characters.
 */
bool i3exists_player(char *name)
{
    struct stat                             fst;
    char                                    buf[MAX_INPUT_LENGTH];

    /*
     * Stands to reason that if there ain't a name to look at, they damn well don't exist! 
     */
    if (!name || !strcasecmp(name, ""))
	return FALSE;

    snprintf(buf, MAX_INPUT_LENGTH, "%s%c/%s", PLAYER_DIR, tolower(name[0]),
	     i3capitalize(name));

    if (stat(buf, &fst) != -1)
	return TRUE;
    else
	return FALSE;
}

bool verify_i3layout(const char *fmt, int num)
{
    const char                             *c;
    int                                     i = 0;

    c = fmt;
    while ((c = strchr(c, '%')) != NULL) {
	if (*(c + 1) == '%') {				       /* %% */
	    c += 2;
	    continue;
	}

	if (*(c + 1) != 's')				       /* not %s */
	    return FALSE;

	c++;
	i++;
    }

    if (i != num)
	return FALSE;

    return TRUE;
}

/* Fixed this function yet again. If the socket is negative or 0, then it will return
 * a FALSE. Used to just check to see if the socket was positive, and that just wasn't
 * working for the way some places checked for this. Any negative value is an indication
 * that the socket never existed.
 */
bool i3_is_connected(void)
{
    if (I3_socket < 1)
	return FALSE;

    return TRUE;
}

/*
 * Add backslashes in front of the " and \'s
 */
char                                   *I3_escape(const char *ps)
{
    static char                             xnew[MAX_STRING_LENGTH];
    char                                   *pnew = xnew;

    while (ps[0]) {
	if (ps[0] == '"') {
	    pnew[0] = '\\';
	    pnew++;
	}
	if (ps[0] == '\\') {
	    pnew[0] = '\\';
	    pnew++;
	}
	if (ps[0] == '\r' || ps[0] == '\n') {
	    ps++;
	    continue;
	}
	pnew[0] = ps[0];
	pnew++;
	ps++;
    }
    pnew[0] = '\0';
    return xnew;
}

/*
 * Remove "'s at begin/end of string
 * If a character is prefixed by \'s it also will be unescaped
 */
void I3_remove_quotes(char **ps)
{
    char                                   *ps1,
                                           *ps2;

    if (*ps[0] == '"')
	(*ps)++;
    if ((*ps)[strlen(*ps) - 1] == '"')
	(*ps)[strlen(*ps) - 1] = 0;

    ps1 = ps2 = *ps;
    while (ps2[0]) {
	if (ps2[0] == '\\') {
	    ps2++;
	}
	ps1[0] = ps2[0];
	ps1++;
	ps2++;
    }
    ps1[0] = '\0';
}

/*
 * Adds "'s at begin/end of string.
 * Also escapes any " or ' characters inside the string.
 */
char *I3_add_quotes(char *s)
{
    static char          buf[MAX_STRING_LENGTH];
    char                *ps = buf;
    int                  i;

    if(s[0] != '"') {
        *ps++ = '"';
    }
    for(i = 0; i < strlen(s); i++) {
        switch( s[i] ) {
            case '"':
            case '\'': 
                *ps++ = '\\';
                *ps++ = s[i];
                break;
            default:
                *ps++ = s[i];
                break;
        }
    }
    if(s[strlen(s)-1] != '"') {
        *ps++ = '"';
    }
    *ps = '\0';
    return buf;
}

/* Searches through the channel list to see if one exists with the localname supplied to it. */
I3_CHANNEL                             *find_I3_channel_by_localname(const char *name)
{
    I3_CHANNEL                             *channel = NULL;

    for (channel = first_I3chan; channel; channel = channel->next) {
	if (!channel->local_name)
	    continue;

	if (!strcasecmp(channel->local_name, name))
	    return channel;
    }
    return NULL;
}

/* Searches through the channel list to see if one exists with the I3 channel name supplied to it.*/
I3_CHANNEL                             *find_I3_channel_by_name(const char *name)
{
    I3_CHANNEL                             *channel = NULL;

    for (channel = first_I3chan; channel; channel = channel->next) {
	if (!strcasecmp(channel->I3_name, name))
	    return channel;
    }
    return NULL;
}

/* Sets up a channel on the mud for the first time, configuring its default layout.
 * If you don't like the default layout of channels, this is where you should edit it to your liking.
 */
I3_CHANNEL                             *new_I3_channel(void)
{
    I3_CHANNEL                             *cnew;

    I3CREATE(cnew, I3_CHANNEL, 1);

    I3LINK(cnew, first_I3chan, last_I3chan, next, prev);
    return cnew;
}

/* Deletes a channel's information from the mud. */
void destroy_I3_channel(I3_CHANNEL *channel)
{
    int                                     x;

    if (channel == NULL) {
	i3bug("%s", "destroy_I3_channel: Null parameter");
	return;
    }

    I3STRFREE(channel->local_name);
    I3STRFREE(channel->host_mud);
    I3STRFREE(channel->I3_name);
    I3STRFREE(channel->layout_m);
    I3STRFREE(channel->layout_e);

    for (x = 0; x < MAX_I3HISTORY; x++) {
	if (channel->history[x] && channel->history[x] != 0/*'\0'*/) // prool: adaptation for osx/clang
	    I3STRFREE(channel->history[x]);
    }

    I3UNLINK(channel, first_I3chan, last_I3chan, next, prev);
    I3DISPOSE(channel);
}

/* Finds a mud with the name supplied on the mudlist */
I3_MUD                                 *find_I3_mud_by_name(const char *name)
{
    I3_MUD                                 *mud;

    for (mud = first_mud; mud; mud = mud->next) {
	if (!strcasecmp(mud->name, name))
	    return mud;
    }
    return NULL;
}

I3_MUD                                 *new_I3_mud(char *name)
{
    I3_MUD                                 *cnew,
                                           *mud_prev;

    I3CREATE(cnew, I3_MUD, 1);

    cnew->name = I3STRALLOC(name);

    for (mud_prev = first_mud; mud_prev; mud_prev = mud_prev->next)
	if (strcasecmp(mud_prev->name, name) >= 0)
	    break;

    if (!mud_prev)
	I3LINK(cnew, first_mud, last_mud, next, prev);
    else
	I3INSERT(cnew, mud_prev, first_mud, next, prev);

    return cnew;
}

I3_MUD *create_I3_mud()
{
    I3_MUD *mud = NULL;

    I3CREATE(mud, I3_MUD, 1);

    /* make sure string pointers are NULL */
    mud->name = NULL;
    mud->ipaddress = NULL;
    mud->mudlib = NULL;
    mud->base_mudlib = NULL;
    mud->driver = NULL;
    mud->mud_type = NULL;
    mud->open_status = NULL;
    mud->admin_email = NULL;
    mud->telnet = NULL;
    mud->web_wrong = NULL;

    mud->banner = NULL;
    mud->web = NULL;
    mud->time = NULL;
    mud->daemon = NULL;

    mud->routerName = NULL;

    /* default values */
    mud->status = -1;
    mud->player_port = 0;
    mud->imud_tcp_port = 0;
    mud->imud_udp_port = 0;

    mud->tell = FALSE;
    mud->beep = FALSE;
    mud->emoteto = FALSE;
    mud->who = FALSE;
    mud->finger = FALSE;
    mud->locate = FALSE;
    mud->channel = FALSE;
    mud->news = FALSE;
    mud->mail = FALSE;
    mud->file = FALSE;
    mud->auth = FALSE;
    mud->ucache = FALSE;

    mud->smtp = 0;
    mud->ftp = 0;
    mud->nntp = 0;
    mud->http = 0;
    mud->pop3 = 0;
    mud->rcp = 0;
    mud->amrcp = 0;

    mud->jeamland = 0;

    mud->autoconnect = FALSE;
    mud->password = 0;
    mud->mudlist_id = 0;
    mud->chanlist_id = 0;
    mud->minlevel = 1;      /* Minimum default level before I3 will acknowledge you exist */
    mud->immlevel = 2;      /* Default immortal level */
    mud->adminlevel = 51;   /* Default administration level */
    mud->implevel = 60;     /* Default implementor level */

    return mud;
}

void destroy_I3_mud(I3_MUD *mud)
{
    if (mud == NULL) {
	i3bug("%s", "destroy_I3_mud: Null parameter");
	return;
    }

    I3STRFREE(mud->name);
    I3STRFREE(mud->ipaddress);
    I3STRFREE(mud->mudlib);
    I3STRFREE(mud->base_mudlib);
    I3STRFREE(mud->driver);
    I3STRFREE(mud->mud_type);
    I3STRFREE(mud->open_status);
    I3STRFREE(mud->admin_email);
    I3STRFREE(mud->telnet);
    I3STRFREE(mud->web_wrong);

    I3STRFREE(mud->banner);
    I3STRFREE(mud->web);
    I3STRFREE(mud->time);
    I3STRFREE(mud->daemon);

    I3STRFREE(mud->routerName);
    if (mud != this_i3mud)
	I3UNLINK(mud, first_mud, last_mud, next, prev);
    I3DISPOSE(mud);
}

/*
 * Returns a CHAR_DATA class which matches the string
 *
 */
CHAR_DATA                              *I3_find_user(const char *name)
{
    DESCRIPTOR_DATA                        *d;
    CHAR_DATA                              *vch = NULL;

    for (d = first_descriptor; d; d = d->next) {
	if ((vch = d->character ? d->character : d->original) != NULL
	    && !strcasecmp(CH_I3NAME(vch), name)
	    && d->connected == CON_PLAYING)
	    return vch;
    }
    return NULL;
}

/* Beefed up to include wildcard ignores and user-level IP ignores.
 * Be careful when setting IP based ignores - Last resort measure, etc.
 */
bool i3ignoring(CHAR_DATA *ch, const char *ignore)
{
    I3_IGNORE                              *temp;
    I3_MUD                                 *mud;
    char                                   *ps;
    char                                    ipbuf[MAX_INPUT_LENGTH],
                                            mudname[MAX_INPUT_LENGTH];

return FALSE; // prool fool
#if 0 // prool fool 
    /*
     * Wildcard support thanks to Xorith 
     */
    for (temp = FIRST_I3IGNORE(ch); temp; temp = temp->next) {
	if (!fnmatch(temp->name, ignore, 0))
	    return TRUE;
    }

    /*
     * In theory, getting this far should be the result of an IP:Port ban 
     */
    ps = (char *)strchr(ignore, '@'); // prool

    if (ignore[0] == '\0' || ps == NULL)
	return FALSE;

    ps[0] = '\0';
    i3strlcpy(mudname, ps + 1, MAX_INPUT_LENGTH);

    for (mud = first_mud; mud; mud = mud->next) {
	if (!strcasecmp(mud->name, mudname)) {
	    snprintf(ipbuf, MAX_INPUT_LENGTH, "%s:%d", mud->ipaddress, mud->player_port);
	    for (temp = FIRST_I3IGNORE(ch); temp; temp = temp->next) {
		if (!strcasecmp(temp->name, ipbuf))
		    return TRUE;
	    }
	}
    }
    return FALSE;
#endif
}

/* Be careful with an IP ban - Last resort measure, etc. */
bool i3banned(const char *ignore)
{
    I3_BAN                                 *temp;
    I3_MUD                                 *mud;
    char                                   *ps;
    char                                    mudname[MAX_INPUT_LENGTH],
                                            ipbuf[MAX_INPUT_LENGTH];

    /*
     * Wildcard support thanks to Xorith 
     */
    for (temp = first_i3ban; temp; temp = temp->next) {
	if (!fnmatch(temp->name, ignore, 0))
	    return TRUE;
    }

    /*
     * In theory, getting this far should be the result of an IP:Port ban 
     */
    ps = (char *)strchr(ignore, '@'); // prool

    if (!ignore || ignore[0] == '\0' || ps == NULL)
	return FALSE;

    ps[0] = '\0';
    i3strlcpy(mudname, ps + 1, MAX_INPUT_LENGTH);

    for (mud = first_mud; mud; mud = mud->next) {
	if (!strcasecmp(mud->name, mudname)) {
	    snprintf(ipbuf, MAX_INPUT_LENGTH, "%s:%d", mud->ipaddress, mud->player_port);
	    for (temp = first_i3ban; temp; temp = temp->next) {
		if (!strcasecmp(temp->name, ipbuf))
		    return TRUE;
	    }
	}
    }
    return FALSE;
}

bool i3check_permissions(CHAR_DATA *ch, int checkvalue, int targetvalue, bool enforceequal)
{
#if 0 // prool fool
    if (checkvalue < 0 || checkvalue > I3PERM_IMP) {
	i3_printf(ch, "Invalid permission setting.\r\n");
	return FALSE;
    }

    if (checkvalue > I3PERM(ch)) {
	i3_printf(ch, "You cannot set permissions higher than your own.\r\n");
	return FALSE;
    }

    if (checkvalue == I3PERM(ch) && I3PERM(ch) != I3PERM_IMP && enforceequal) {
	i3_printf(ch, "You cannot set permissions equal to your own. Someone higher up must do this.\r\n");
	return FALSE;
    }

    if (I3PERM(ch) < targetvalue) {
	i3_printf(ch, "You cannot alter the permissions of someone or something above your own.\r\n");
	return FALSE;
    }
#endif
    return TRUE;
}

/*
 * Read a number from a file. [Taken from Smaug's fread_number]
 */
int i3fread_number(FILE * fp)
{
    int                                     num;
    bool                                    sign;
    char                                    c;

    do {
	if (feof(fp)) {
	    i3log("%s", "i3fread_number: EOF encountered on read.");
	    return 0;
	}
	c = getc(fp);
    }
    while (isspace(c));

    num = 0;

    sign = FALSE;
    if (c == '+') {
	c = getc(fp);
    } else if (c == '-') {
	sign = TRUE;
	c = getc(fp);
    }

    if (!isdigit(c)) {
	i3log("i3fread_number: bad format. (%c)", c);
	return 0;
    }

    while (isdigit(c)) {
	if (feof(fp)) {
	    i3log("%s", "i3fread_number: EOF encountered on read.");
	    return num;
	}
	num = num * 10 + c - '0';
	c = getc(fp);
    }

    if (sign)
	num = 0 - num;

    if (c == '|')
	num += i3fread_number(fp);
    else if (c != ' ')
	ungetc(c, fp);

    return num;
}

/*
 * Read to end of line into static buffer [Taken from Smaug's fread_line]
 */
char                                   *i3fread_line(FILE * fp)
{
    char                                    line[MAX_STRING_LENGTH];
    char                                   *pline;
    char                                    c;
    int                                     ln;

    pline = line;
    line[0] = '\0';
    ln = 0;

    /*
     * Skip blanks.
     * Read first char.
     */
    do {
	if (feof(fp)) {
	    i3bug("%s", "i3fread_line: EOF encountered on read.");
	    i3strlcpy(line, "", MAX_STRING_LENGTH);
	    return I3STRALLOC(line);
	}
	c = getc(fp);
    }
    while (isspace(c));

    ungetc(c, fp);

    do {
	if (feof(fp)) {
	    i3bug("%s", "i3fread_line: EOF encountered on read.");
	    *pline = '\0';
	    return I3STRALLOC(line);
	}
	c = getc(fp);
	*pline++ = c;
	ln++;
	if (ln >= (MAX_STRING_LENGTH - 1)) {
	    i3bug("%s", "i3fread_line: line too long");
	    break;
	}
    }
    while (c != '\n' && c != '\r');

    do {
	c = getc(fp);
    }
    while (c == '\n' || c == '\r');

    ungetc(c, fp);
    pline--;
    *pline = '\0';

    /*
     * Since tildes generally aren't found at the end of lines, this seems workable. Will enable reading old configs. 
     */
    if (line[strlen(line) - 1] == '~')
	line[strlen(line) - 1] = '\0';
    return I3STRALLOC(line);
}

/*
 * Read one word (into static buffer). [Taken from Smaug's fread_word]
 */
char                                   *i3fread_word(FILE * fp)
{
    static char                             word[MAX_INPUT_LENGTH];
    char                                   *pword;
    char                                    cEnd;

    do {
	if (feof(fp)) {
	    i3log("%s", "i3fread_word: EOF encountered on read.");
	    word[0] = '\0';
	    return word;
	}
	cEnd = getc(fp);
    }
    while (isspace(cEnd));

    if (cEnd == '\'' || cEnd == '"') {
	pword = word;
    } else {
	word[0] = cEnd;
	pword = word + 1;
	cEnd = ' ';
    }

    for (; pword < word + MAX_INPUT_LENGTH; pword++) {
	if (feof(fp)) {
	    i3log("%s", "i3fread_word: EOF encountered on read.");
	    *pword = '\0';
	    return word;
	}
	*pword = getc(fp);
	if (cEnd == ' ' ? isspace(*pword) : *pword == cEnd) {
	    if (cEnd == ' ')
		ungetc(*pword, fp);
	    *pword = '\0';
	    return word;
	}
    }

    i3log("%s", "i3fread_word: word too long");
    return NULL;
}

char                                   *i3fread_rest_of_line(FILE * fp)
{
    static char                             word[MAX_STRING_LENGTH];
    char                                   *pword;
    char                                    c;

    do {
	if (feof(fp)) {
	    i3log("%s", "i3fread_rest_of_line: EOF encountered on read.");
	    word[0] = '\0';
	    return word;
	}
	c = getc(fp);
    } while (isspace(c));

    word[0] = c;
    pword = word + 1;

    for (; pword < word + MAX_STRING_LENGTH; pword++) {
	if (feof(fp)) {
	    i3log("%s", "i3fread_rest_of_line: EOF encountered on read.");
	    *pword = '\0';
	    return word;
	}
        c = getc(fp);
        if(c == '\n' || c == '\r') {
            do {
                c = getc(fp);
            } while (c == '\n' || c == '\r');
	    if (!feof(fp))
                ungetc(c, fp);
            *pword = '\0';
            return word;
        } else {
            *pword = c;
        }
    }

    i3log("%s", "i3fread_rest_of_line: line too long");
    return NULL;
}

/*
 * Read a letter from a file. [Taken from Smaug's fread_letter]
 */
char i3fread_letter(FILE * fp)
{
    char                                    c;

    do {
	if (feof(fp)) {
	    i3log("%s", "i3fread_letter: EOF encountered on read.");
	    return '\0';
	}
	c = getc(fp);
    }
    while (isspace(c));

    return c;
}

/*
 * Read to end of line (for comments). [Taken from Smaug's fread_to_eol]
 */
void i3fread_to_eol(FILE * fp)
{
    char                                    c;

    do {
	if (feof(fp)) {
	    i3log("%s", "i3fread_to_eol: EOF encountered on read.");
	    return;
	}
	c = getc(fp);
    }
    while (c != '\n' && c != '\r');

    do {
	c = getc(fp);
    }
    while (c == '\n' || c == '\r');

    ungetc(c, fp);
    return;
}

/*
 * Read and allocate space for a string from a file.
 */
char                                   *i3fread_string(FILE * fp)
{
    static char                             buf[MAX_STRING_LENGTH] = "\0\0\0\0\0\0\0";
    char                                   *ack = NULL;
    int                                     flag = FALSE;
    int                                     c = 0;
    static char                             Empty[1] = "";

    bzero(buf, MAX_STRING_LENGTH);
    ack = buf;
    flag = 0;
    do {
	c = getc(fp);
    } while (isspace(c));

    if ((*ack++ = c) == '~')
	return Empty;
    if (((int)(*ack++ = c)) == '\xA2') // prool: del 0
	return Empty;

    for (;;) {
	if (ack > &buf[MAX_STRING_LENGTH - 1]) {
	    log_error("new_fread_string: MAX_STRING %d exceeded, truncating.", MAX_STRING_LENGTH);
	    return buf;
	}
	switch ((int)(*ack = getc(fp))) {
	    default:
		flag = 0;
		ack++;
		break;
	    case EOF:
		log_error("Fread_string: EOF");
		return buf;
	    case '\r':
		break;
	    case '~':
	    case '\xA2': // prool
		ack++;
		flag = 1;
		break;
	    case '\n':
		if (flag) {
		    if (ack > buf) {
			ack--;
			*ack = '\0';
		    }
		    return buf;
		} else {
		    flag = 0;
		    ack++;
		    *ack++ = '\r';
		}
		break;
	}
    }
}

/******************************************
 * Packet handling and routing functions. *
 ******************************************/

/*
 * Write a string into the send-buffer. Does not yet send it.
 */
void I3_write_buffer(const char *msg)
{
    long                                    newsize = I3_output_pointer + strlen(msg);

    if (newsize > OPS - 1) {
	i3bug("I3_write_buffer: buffer too large (would become %ld)", newsize);
	return;
    }
    i3strlcpy(I3_output_buffer + I3_output_pointer, msg, newsize);
    I3_output_pointer = newsize;
}

/* Use this function in place of I3_write_buffer ONLY if the text to be sent could 
 * contain color tags to parse into Pinkfish codes. Otherwise it will mess up the packet.
 */
void send_to_i3(const char *text)
{
    char                                    buf[MAX_STRING_LENGTH];

    snprintf(buf, MAX_STRING_LENGTH, "%s", I3_mudtag_to_i3tag(text));
    I3_write_buffer(buf);
}

/*
 * Put a I3-header in the send-buffer. If a field is NULL it will
 * be replaced by a 0 (zero).
 */
void I3_write_header(const char *identifier, const char *originator_mudname,
		     const char *originator_username, const char *target_mudname,
		     const char *target_username)
{
    I3_write_buffer("({\"");
    I3_write_buffer(identifier);
    I3_write_buffer("\",5,");
    if (originator_mudname) {
	I3_write_buffer("\"");
	I3_write_buffer(originator_mudname);
	I3_write_buffer("\",");
    } else
	I3_write_buffer("0,");

    if (originator_username) {
	I3_write_buffer("\"");
	I3_write_buffer(I3_nameescape(originator_username));
	I3_write_buffer("\",");
    } else
	I3_write_buffer("0,");

    if (target_mudname) {
	I3_write_buffer("\"");
	I3_write_buffer(target_mudname);
	I3_write_buffer("\",");
    } else
	I3_write_buffer("0,");

    if (target_username) {
	I3_write_buffer("\"");
	I3_write_buffer(target_username);
	I3_write_buffer("\",");
    } else
	I3_write_buffer("0,");
}

/*
 * Gets the next I3 field, that is when the amount of {[("'s and
 * ")]}'s match each other when a , is read. It's not foolproof, it
 * should honestly be some kind of statemachine, which does error-
 * checking. Right now I trust the I3-router to send proper packets
 * only. How naive :-) [Indeed Edwin, but I suppose we have little choice :P - Samson]
 *
 * ps will point to the beginning of the next field.
 *
 */
char                                   *I3_get_field(char *packet, char **ps)
{
    int                                     count[MAX_INPUT_LENGTH];
    char                                    has_apostrophe = 0,
	has_backslash = 0;
    char                                    foundit = 0;

    bzero(count, sizeof(count));

    *ps = packet;
    while (1) {
	switch (*ps[0]) {
	    case '{':
		if (!has_apostrophe)
		    count[(int)'{']++;
		break;
	    case '}':
		if (!has_apostrophe)
		    count[(int)'}']++;
		break;
	    case '[':
		if (!has_apostrophe)
		    count[(int)'[']++;
		break;
	    case ']':
		if (!has_apostrophe)
		    count[(int)']']++;
		break;
	    case '(':
		if (!has_apostrophe)
		    count[(int)'(']++;
		break;
	    case ')':
		if (!has_apostrophe)
		    count[(int)')']++;
		break;
	    case '\\':
		if (has_backslash)
		    has_backslash = 0;
		else
		    has_backslash = 1;
		break;
	    case '"':
		if (has_backslash) {
		    has_backslash = 0;
		} else {
		    if (has_apostrophe)
			has_apostrophe = 0;
		    else
			has_apostrophe = 1;
		}
		break;
	    case ',':
	    case ':':
		if (has_apostrophe)
		    break;
		if (has_backslash)
		    break;
		if (count[(int)'{'] != count[(int)'}'])
		    break;
		if (count[(int)'['] != count[(int)']'])
		    break;
		if (count[(int)'('] != count[(int)')'])
		    break;
		foundit = 1;
		break;
	}
	if (foundit)
	    break;
	(*ps)++;
    }
    *ps[0] = '\0';
    (*ps)++;
    return *ps;
}

/*
 * Writes the string into the socket, prefixed by the size.
 */
bool I3_write_packet(char *msg)
{
    int                                     oldsize,
                                            size,
                                            check,
                                            x;
    char                                   *s = I3_output_buffer;

    oldsize = size = strlen(msg + 4);
    s[3] = size % 256;
    size >>= 8;
    s[2] = size % 256;
    size >>= 8;
    s[1] = size % 256;
    size >>= 8;
    s[0] = size % 256;

    /*
     * Scan for \r used in Diku client packets and change to NULL 
     */
    for (x = 0; x < oldsize + 4; x++)
	if (msg[x] == '\r' && x > 3)
	    msg[x] = '\0';

    check = send(I3_socket, msg, oldsize + 4, 0);

    if (!check || (check < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
	if (check < 0)
	    i3log("%s", "Write error on socket.");
	else
	    i3log("%s", "EOF encountered on socket write.");
	I3_connection_close(TRUE);
	return FALSE;
    }

    if (check < 0)					       /* EAGAIN */
	return TRUE;

    bytes_sent += check;
    if (packetdebug) {
	i3log("Size: %d. Bytes Sent: %d.", oldsize, check);
	i3log("Packet Sent: %s", msg + 4);
    }
    I3_output_pointer = 4;
    return TRUE;
}

void I3_send_packet(void)
{
    I3_write_packet(I3_output_buffer);
    return;
}

/* The all important startup packet. This is what will be initially sent upon trying to connect
 * to the I3 router. It is therefore quite important that the information here be exactly correct.
 * If anything is wrong, your packet will be dropped by the router as invalid and your mud simply
 * won't connect to I3. DO NOT USE COLOR TAGS FOR ANY OF THIS INFORMATION!!!
 * Bugs fixed in this on 8-31-03 for improperly sent tags. Streamlined for less packet bloat.
 */
void I3_startup_packet(void)
{
    char                                    s[MAX_INPUT_LENGTH];
    char                                   *strtime;

    if (!i3_is_connected())
	return;

    I3_output_pointer = 4;
    I3_output_buffer[0] = '\0';

    printf("I3: Sending startup_packet to %s\n", I3_ROUTER_NAME); // prool

    I3_write_header("startup-req-3", this_i3mud->name, NULL, I3_ROUTER_NAME, NULL);

    snprintf(s, MAX_INPUT_LENGTH, "%d", this_i3mud->password);
    I3_write_buffer(s);
    I3_write_buffer(",");
    snprintf(s, MAX_INPUT_LENGTH, "%d", this_i3mud->mudlist_id);
    I3_write_buffer(s);
    I3_write_buffer(",");
    snprintf(s, MAX_INPUT_LENGTH, "%d", this_i3mud->chanlist_id);
    I3_write_buffer(s);
    I3_write_buffer(",");
    snprintf(s, MAX_INPUT_LENGTH, "%d", this_i3mud->player_port);
    I3_write_buffer(s);
    I3_write_buffer(",0,0,\"");

    I3_write_buffer(this_i3mud->mudlib);
    I3_write_buffer("\",\"");
    I3_write_buffer(this_i3mud->base_mudlib);
    I3_write_buffer("\",\"");
    I3_write_buffer(this_i3mud->driver);
    I3_write_buffer("\",\"");
    I3_write_buffer(this_i3mud->mud_type);
    I3_write_buffer("\",\"");
    I3_write_buffer(this_i3mud->open_status);
    I3_write_buffer("\",\"");
    I3_write_buffer(this_i3mud->admin_email);
    I3_write_buffer("\",");

    /*
     * Begin first mapping set 
     */
    I3_write_buffer("([");
    if (this_i3mud->emoteto)
	I3_write_buffer("\"emoteto\":1,");
    if (this_i3mud->news)
	I3_write_buffer("\"news\":1,");
    if (this_i3mud->ucache)
	I3_write_buffer("\"ucache\":1,");
    if (this_i3mud->auth)
	I3_write_buffer("\"auth\":1,");
    if (this_i3mud->locate)
	I3_write_buffer("\"locate\":1,");
    if (this_i3mud->finger)
	I3_write_buffer("\"finger\":1,");
    if (this_i3mud->channel)
	I3_write_buffer("\"channel\":1,");
    if (this_i3mud->who)
	I3_write_buffer("\"who\":1,");
    if (this_i3mud->tell)
	I3_write_buffer("\"tell\":1,");
    if (this_i3mud->beep)
	I3_write_buffer("\"beep\":1,");
    if (this_i3mud->mail)
	I3_write_buffer("\"mail\":1,");
    if (this_i3mud->file)
	I3_write_buffer("\"file\":1,");
    if (this_i3mud->http) {
	snprintf(s, MAX_INPUT_LENGTH, "\"http\":%d,", this_i3mud->http);
	I3_write_buffer(s);
    }
    if (this_i3mud->smtp) {
	snprintf(s, MAX_INPUT_LENGTH, "\"smtp\":%d,", this_i3mud->smtp);
	I3_write_buffer(s);
    }
    if (this_i3mud->pop3) {
	snprintf(s, MAX_INPUT_LENGTH, "\"pop3\":%d,", this_i3mud->pop3);
	I3_write_buffer(s);
    }
    if (this_i3mud->ftp) {
	snprintf(s, MAX_INPUT_LENGTH, "\"ftp\":%d,", this_i3mud->ftp);
	I3_write_buffer(s);
    }
    if (this_i3mud->nntp) {
	snprintf(s, MAX_INPUT_LENGTH, "\"nntp\":%d,", this_i3mud->nntp);
	I3_write_buffer(s);
    }
    if (this_i3mud->rcp) {
	snprintf(s, MAX_INPUT_LENGTH, "\"rcp\":%d,", this_i3mud->rcp);
	I3_write_buffer(s);
    }
    if (this_i3mud->amrcp) {
	snprintf(s, MAX_INPUT_LENGTH, "\"amrcp\":%d,", this_i3mud->amrcp);
	I3_write_buffer(s);
    }
    I3_write_buffer("]),([");

    /*
     * END first set of "mappings", start of second set 
     */
    if (this_i3mud->web && this_i3mud->web[0] != '\0') {
	snprintf(s, MAX_INPUT_LENGTH, "\"url\":\"%s\",", this_i3mud->web);
	I3_write_buffer(s);
    }
#if 0 // 0 - prool 1 - orig
    strtime = ctime(&i3_time);
#else
    i3_time=time(0);
    strtime = asctime(localtime(&i3_time));
#endif
    strtime[strlen(strtime) - 1] = '\0';
printf("i3 strtime='%s'\n", strtime);
    snprintf(s, MAX_INPUT_LENGTH, "\"time\":\"%s\",", strtime);
    I3_write_buffer(s);

    I3_write_buffer("]),})\r");
    I3_send_packet();
}

/* This function saves the password, mudlist ID, and chanlist ID that are used by the mud.
 * The password value is returned from the I3 router upon your initial connection.
 * The mudlist and chanlist ID values are updated as needed while your mud is connected.
 * Do not modify the file it generates because doing so may prevent your mud from reconnecting
 * to the router in the future. This file will be rewritten each time the i3_shutdown function
 * is called, or any of the id values change.
 */
void I3_save_id(void)
{
    FILE                                   *fp;

    if (!(fp = fopen(I3_PASSWORD_FILE, "w"))) {
	i3log("%s", "Couldn't write to I3 password file.");
	return;
    }

    fprintf(fp, "%s", "#PASSWORD\n");
    fprintf(fp, "%d %d %d\n", this_i3mud->password, this_i3mud->mudlist_id,
	    this_i3mud->chanlist_id);
    I3FCLOSE(fp);
}

/* The second most important packet your mud will deal with. If you never get this
 * coming back from the I3 router, something was wrong with your startup packet
 * or the router may be jammed up. Whatever the case, if you don't get a reply back
 * your mud won't be acknowledged as connected.
 */
void I3_process_startup_reply(I3_HEADER *header, char *s)
{
    ROUTER_DATA                            *router;
    I3_CHANNEL                             *channel;
    char                                   *ps = s,
	*next_ps;

    /*
     * Recevies the router list. Nothing much to do here until there's more than 1 router. 
     */
    I3_get_field(ps, &next_ps);
    i3log("%s", ps);					       /* Just checking for now */
    ps = next_ps;

    /*
     * Receives your mud's updated password, which may or may not be the same as what it sent out before 
     */
    I3_get_field(ps, &next_ps);
    this_i3mud->password = atoi(ps);

    i3log("Received startup_reply from %s", header->originator_mudname);

    I3_save_id();

    for (router = first_router; router; router = router->next) {
	if (!strcasecmp(router->name, header->originator_mudname)) {
	    router->reconattempts = 0;
	    I3_ROUTER_NAME = router->name;
	    break;
	}
    }
    i3wait = 0;
    i3timeout = 0;
    i3justconnected = 1;
    i3log("%s", "Intermud-3 Network connection complete.");

    for (channel = first_I3chan; channel; channel = channel->next) {
	if (channel->local_name && channel->local_name[0] != '\0') {
	    i3log("Subscribing to %s", channel->local_name);
	    I3_send_channel_listen(channel, TRUE);
	}
    }
    return;
}

void I3_process_chanack(I3_HEADER *header, char *s)
{
    CHAR_DATA                              *ch;
    char                                   *next_ps,
                                           *ps = s;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    if (!(ch = I3_find_user(header->target_username)))
	i3log("%s", ps);
    else
	i3_printf(ch, "%%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", ps);
    return;
}

void I3_send_error(const char *mud, const char *user, const char *code, const char *message)
{
    if (!i3_is_connected())
	return;

    I3_write_header("error", this_i3mud->name, 0, mud, user);
    I3_write_buffer("\"");
    I3_write_buffer(code);
    I3_write_buffer("\",\"");
    I3_write_buffer(I3_escape(message));
    I3_write_buffer("\",0,})\r");
    I3_send_packet();
}

void I3_process_error(I3_HEADER *header, char *s)
{
    CHAR_DATA                              *ch;
    char                                   *next_ps,
                                           *ps = s;
    char                                    type[MAX_INPUT_LENGTH],
                                            error[MAX_STRING_LENGTH];

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(type, ps, MAX_INPUT_LENGTH);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    /*
     * Since VargonMUD likes to spew errors for no good reason.... 
     */
    if (!strcasecmp(header->originator_mudname, "VargonMUD"))
	return;

    snprintf(error, MAX_STRING_LENGTH, "Error: from %s to %s@%s\r\n%s: %s",
	     header->originator_mudname, header->target_username, header->target_mudname, type,
	     ps);

    if (!(ch = I3_find_user(header->target_username)))
	i3log("%s", error);
    else
	i3_printf(ch, "%%^RED%%^%%^BOLD%%^%s%%^RESET%%^\r\n", error);
}

int I3_get_ucache_gender(char *name)
{
    UCACHE_DATA                            *user;

    for (user = first_ucache; user; user = user->next) {
	if (!strcasecmp(user->name, name))
	    return user->gender;
    }

    /*
     * -1 means you aren't in the list and need to be put there. 
     */
    return -1;
}

/* Saves the ucache info to disk because it would just be spamcity otherwise */
void I3_save_ucache(void)
{
    FILE                                   *fp;
    UCACHE_DATA                            *user;

    if (!(fp = fopen(I3_UCACHE_FILE, "w"))) {
	i3log("%s", "Couldn't write to I3 ucache file.");
	return;
    }

    for (user = first_ucache; user; user = user->next) {
	fprintf(fp, "%s", "#UCACHE\n");
	fprintf(fp, "Name %s\n", user->name);
	fprintf(fp, "Sex  %d\n", user->gender);
	fprintf(fp, "Time %ld\n", (long int)user->time);
	fprintf(fp, "%s", "End\n\n");
    }
    fprintf(fp, "%s", "#END\n");
    I3FCLOSE(fp);
    return;
}

void I3_prune_ucache(void)
{
    UCACHE_DATA                            *ucache,
                                           *next_ucache;

    for (ucache = first_ucache; ucache; ucache = next_ucache) {
	next_ucache = ucache->next;

	/*
	 * Info older than 30 days is removed since this person likely hasn't logged in at all 
	 */
	if (i3_time - ucache->time >= 2592000) {
	    I3STRFREE(ucache->name);
	    I3UNLINK(ucache, first_ucache, last_ucache, next, prev);
	    I3DISPOSE(ucache);
	}
    }
    I3_save_ucache();
    return;
}

/* Updates user info if they exist, adds them if they don't. */
void I3_ucache_update(char *name, int gender)
{
    UCACHE_DATA                            *user;

    for (user = first_ucache; user; user = user->next) {
	if (!strcasecmp(user->name, name)) {
	    user->gender = gender;
	    user->time = i3_time;
	    return;
	}
    }
    I3CREATE(user, UCACHE_DATA, 1);
    user->name = I3STRALLOC(name);
    user->gender = gender;
    user->time = i3_time;
    I3LINK(user, first_ucache, last_ucache, next, prev);

    I3_save_ucache();
    return;
}

void I3_send_ucache_update(const char *visname, int gender)
{
    char                                    buf[10];

    if (!i3_is_connected())
	return;

    I3_write_header("ucache-update", this_i3mud->name, NULL, NULL, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(visname);
    I3_write_buffer("\",\"");
    I3_write_buffer(visname);
    I3_write_buffer("\",");
    snprintf(buf, 10, "%d", gender);
    I3_write_buffer(buf);
    I3_write_buffer(",})\r");
    I3_send_packet();

    return;
}

void I3_process_ucache_update(I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    char                                    username[MAX_INPUT_LENGTH],
                                            visname[MAX_INPUT_LENGTH],
                                            buf[MAX_STRING_LENGTH];
    int                                     sex,
                                            gender;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(username, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(visname, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    gender = atoi(ps);

    snprintf(buf, MAX_STRING_LENGTH, "%s@%s", visname, header->originator_mudname);

    sex = I3_get_ucache_gender(buf);

    if (sex == gender)
	return;

    I3_ucache_update(buf, gender);
    return;
}

void I3_send_chan_user_req(char *targetmud, char *targetuser)
{
    if (!i3_is_connected())
	return;

    I3_write_header("chan-user-req", this_i3mud->name, NULL, targetmud, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(targetuser);
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

void I3_process_chan_user_req(I3_HEADER *header, char *s)
{
    char                                    buf[MAX_STRING_LENGTH];
    char                                   *ps = s,
	*next_ps;
    int                                     gender;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    snprintf(buf, MAX_STRING_LENGTH, "%s@%s", header->target_username, this_i3mud->name);
    gender = I3_get_ucache_gender(buf);

    /*
     * Gender of -1 means they aren't in the mud's ucache table, don't waste a packet on a reply 
     */
    if (gender == -1)
	return;

    I3_write_header("chan-user-reply", this_i3mud->name, NULL, header->originator_mudname, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(ps);
    I3_write_buffer("\",\"");
    I3_write_buffer(ps);
    I3_write_buffer("\",");
    snprintf(buf, MAX_STRING_LENGTH, "%d", gender);
    I3_write_buffer(buf);
    I3_write_buffer(",})\r");
    I3_send_packet();

    return;
}

void I3_process_chan_user_reply(I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    char                                    username[MAX_INPUT_LENGTH],
                                            visname[MAX_INPUT_LENGTH],
                                            buf[MAX_STRING_LENGTH];
    int                                     sex,
                                            gender;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(username, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(visname, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    gender = atoi(ps);

    snprintf(buf, MAX_STRING_LENGTH, "%s@%s", visname, header->originator_mudname);

    sex = I3_get_ucache_gender(buf);

    if (sex == gender)
	return;

    I3_ucache_update(buf, gender);
    return;
}

void I3_process_mudlist(I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    I3_MUD                                 *mud = NULL;
    char                                    mud_name[MAX_INPUT_LENGTH];

    I3_get_field(ps, &next_ps);
    this_i3mud->mudlist_id = atoi(ps);
    I3_save_id();

    ps = next_ps;
    ps += 2;

    while (1) {
	char                                   *next_ps2;

	I3_get_field(ps, &next_ps);
	I3_remove_quotes(&ps);
	i3strlcpy(mud_name, ps, MAX_INPUT_LENGTH);

	ps = next_ps;
	I3_get_field(ps, &next_ps2);

	if (ps[0] != '0') {
	    mud = find_I3_mud_by_name(mud_name);
	    if (!mud)
		mud = new_I3_mud(mud_name);

	    ps += 2;
	    I3_get_field(ps, &next_ps);
	    mud->status = atoi(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    I3_remove_quotes(&ps);
	    I3STRFREE(mud->ipaddress);
	    mud->ipaddress = I3STRALLOC(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    mud->player_port = atoi(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    mud->imud_tcp_port = atoi(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    mud->imud_udp_port = atoi(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    I3_remove_quotes(&ps);
	    I3STRFREE(mud->mudlib);
	    mud->mudlib = I3STRALLOC(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    I3_remove_quotes(&ps);
	    I3STRFREE(mud->base_mudlib);
	    mud->base_mudlib = I3STRALLOC(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    I3_remove_quotes(&ps);
	    I3STRFREE(mud->driver);
	    mud->driver = I3STRALLOC(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    I3_remove_quotes(&ps);
	    I3STRFREE(mud->mud_type);
	    mud->mud_type = I3STRALLOC(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    I3_remove_quotes(&ps);
	    I3STRFREE(mud->open_status);
	    mud->open_status = I3STRALLOC(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    I3_remove_quotes(&ps);
	    I3STRFREE(mud->admin_email);
	    mud->admin_email = I3STRALLOC(ps);
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);

	    ps += 2;
	    while (1) {
		char                                   *next_ps3;
		char                                    key[MAX_INPUT_LENGTH];

		if (ps[0] == ']')
		    break;

		I3_get_field(ps, &next_ps3);
		I3_remove_quotes(&ps);
		i3strlcpy(key, ps, MAX_INPUT_LENGTH);
		ps = next_ps3;
		I3_get_field(ps, &next_ps3);

		switch (key[0]) {
		    case 'a':
			if (!strcasecmp(key, "auth")) {
			    mud->auth = ps[0] == '0' ? 0 : 1;
			    break;
			}
			if (!strcasecmp(key, "amrcp")) {
			    mud->amrcp = atoi(ps);
			    break;
			}
			break;
		    case 'b':
			if (!strcasecmp(key, "beep")) {
			    mud->beep = ps[0] == '0' ? 0 : 1;
			    break;
			}
			break;
		    case 'c':
			if (!strcasecmp(key, "channel")) {
			    mud->channel = ps[0] == '0' ? 0 : 1;
			    break;
			}
			break;
		    case 'e':
			if (!strcasecmp(key, "emoteto")) {
			    mud->emoteto = ps[0] == '0' ? 0 : 1;
			    break;
			}
			break;
		    case 'f':
			if (!strcasecmp(key, "file")) {
			    mud->file = ps[0] == '0' ? 0 : 1;
			    break;
			}
			if (!strcasecmp(key, "finger")) {
			    mud->finger = ps[0] == '0' ? 0 : 1;
			    break;
			}
			if (!strcasecmp(key, "ftp")) {
			    mud->ftp = atoi(ps);
			    break;
			}
			break;
		    case 'h':
			if (!strcasecmp(key, "http")) {
			    mud->http = atoi(ps);
			    break;
			}
			break;
		    case 'l':
			if (!strcasecmp(key, "locate")) {
			    mud->locate = ps[0] == '0' ? 0 : 1;
			    break;
			}
			break;
		    case 'm':
			if (!strcasecmp(key, "mail")) {
			    mud->mail = ps[0] == '0' ? 0 : 1;
			    break;
			}
			break;
		    case 'n':
			if (!strcasecmp(key, "news")) {
			    mud->news = ps[0] == '0' ? 0 : 1;
			    break;
			}
			if (!strcasecmp(key, "nntp")) {
			    mud->nntp = atoi(ps);
			    break;
			}
			break;
		    case 'p':
			if (!strcasecmp(key, "pop3")) {
			    mud->pop3 = atoi(ps);
			    break;
			}
			break;
		    case 'r':
			if (!strcasecmp(key, "rcp")) {
			    mud->rcp = atoi(ps);
			    break;
			}
			break;
		    case 's':
			if (!strcasecmp(key, "smtp")) {
			    mud->smtp = atoi(ps);
			    break;
			}
			break;
		    case 't':
			if (!strcasecmp(key, "tell")) {
			    mud->tell = ps[0] == '0' ? 0 : 1;
			    break;
			}
			break;
		    case 'u':
			if (!strcasecmp(key, "ucache")) {
			    mud->ucache = ps[0] == '0' ? 0 : 1;
			    break;
			}
			if (!strcasecmp(key, "url")) {
			    I3_remove_quotes(&ps);
			    I3STRFREE(mud->web_wrong);
			    mud->web_wrong = I3STRALLOC(ps);
			    break;
			}
			break;
		    case 'w':
			if (!strcasecmp(key, "who")) {
			    mud->who = ps[0] == '0' ? 0 : 1;
			    break;
			}
			break;
		    default:
			break;
		}

		ps = next_ps3;
		if (ps[0] == ']')
		    break;
	    }
	    ps = next_ps;

	    I3_get_field(ps, &next_ps);
	    ps = next_ps;

	} else {
	    if ((mud = find_I3_mud_by_name(mud_name)) != NULL)
		destroy_I3_mud(mud);
	}
	ps = next_ps2;
	if (ps[0] == ']')
	    break;
    }
    return;
}

void I3_process_chanlist_reply(I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    I3_CHANNEL                             *channel;
    char                                    chan[MAX_INPUT_LENGTH];

    i3log("I3_process_chanlist_reply: %s", "Got chanlist-reply packet!");

    I3_get_field(ps, &next_ps);
    this_i3mud->chanlist_id = atoi(ps);
    I3_save_id();

    ps = next_ps;
    ps += 2;

    while (1) {
	char                                   *next_ps2;

	I3_get_field(ps, &next_ps);
	I3_remove_quotes(&ps);
	i3strlcpy(chan, ps, MAX_INPUT_LENGTH);

	ps = next_ps;
	I3_get_field(ps, &next_ps2);
	if (ps[0] != '0') {
	    if (!(channel = find_I3_channel_by_name(chan))) {
		channel = new_I3_channel();
		channel->I3_name = I3STRALLOC(chan);
		i3log("New channel %s has been added from router %s", channel->I3_name,
		      I3_ROUTER_NAME);
	    } else {
		i3log("Channel %s has been updated from router %s", channel->I3_name,
		      I3_ROUTER_NAME);
	    }

	    ps += 2;
	    I3_get_field(ps, &next_ps);
	    I3_remove_quotes(&ps);
	    I3STRFREE(channel->host_mud);
	    channel->host_mud = I3STRALLOC(ps);
	    ps = next_ps;
	    I3_get_field(ps, &next_ps);
	    channel->status = atoi(ps);
	} else {
	    if ((channel = find_I3_channel_by_name(chan)) != NULL) {
		if (channel->local_name && channel->local_name[0] != '\0')
		    i3log("Locally configured channel %s has been purged from router %s",
			  channel->local_name, I3_ROUTER_NAME);
		destroy_I3_channel(channel);
		I3_write_channel_config();
	    }
	}
	ps = next_ps2;
	if (ps[0] == ']')
	    break;
    }
    i3log("I3_process_chanlist_reply: %s", "Saving channel config data.");
    I3_write_channel_config();
    return;
}

void I3_send_channel_message(I3_CHANNEL *channel, const char *name, const char *message)
{
    char                                    buf[MAX_STRING_LENGTH];

    if (!i3_is_connected())
	return;

    i3strlcpy(buf, message, MAX_STRING_LENGTH);

    //log_info("I3_send_channel(%s@%s, %s, %s)", channel->I3_name, channel->host_mud, name, message);
    I3_write_header("channel-m", this_i3mud->name, name, NULL, NULL);
    //log_info("I3_send_channel() header setup.");
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",\"");
    I3_write_buffer(I3_nameremap(name));
    //log_info("I3_send_channel() name remap %s to %s.", name, I3_nameremap(name));
    I3_write_buffer("\",\"");
    send_to_i3(I3_escape(buf));
    //log_info("I3_send_channel() escaped buffer.");
    I3_write_buffer("\",})\r");
    I3_send_packet();
    //log_info("I3_send_channel() done.");

    return;
}

void I3_send_channel_emote(I3_CHANNEL *channel, const char *name, const char *message)
{
    char                                    buf[MAX_STRING_LENGTH];

    if (!i3_is_connected())
	return;

    if (strstr(message, "$N") == NULL)
	snprintf(buf, MAX_STRING_LENGTH, "$N %s", message);
    else
	i3strlcpy(buf, message, MAX_STRING_LENGTH);

    I3_write_header("channel-e", this_i3mud->name, name, NULL, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",\"");
    I3_write_buffer(I3_nameremap(name));
    I3_write_buffer("\",\"");
    send_to_i3(I3_escape(buf));
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

void I3_send_channel_t(I3_CHANNEL *channel, const char *name, char *tmud, char *tuser, char *msg_o,
		       char *msg_t, char *tvis)
{
    if (!i3_is_connected())
	return;

    I3_write_header("channel-t", this_i3mud->name, name, NULL, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",\"");
    I3_write_buffer(tmud);
    I3_write_buffer("\",\"");
    I3_write_buffer(tuser);
    I3_write_buffer("\",\"");
    send_to_i3(I3_escape(msg_o));
    I3_write_buffer("\",\"");
    send_to_i3(I3_escape(msg_t));
    I3_write_buffer("\",\"");
    I3_write_buffer(name);
    I3_write_buffer("\",\"");
    I3_write_buffer(tvis);
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

int I3_token(char type, char *string, char *oname, char *tname)
{
    char                                    code[50];
    char                                   *p;

    switch (type) {
	default:
	    code[0] = type;
	    code[1] = '\0';
	    return 1;
	case '$':
	    i3strlcpy(code, "$", 50);
	    break;
	case ' ':
	    i3strlcpy(code, " ", 50);
	    break;
	case 'N':					       /* Originator's name */
	    i3strlcpy(code, oname, 50);
	    break;
	case 'O':					       /* Target's name */
	    i3strlcpy(code, tname, 50);
	    break;
    }
    p = code;
    while (*p != '\0') {
	*string = *p++;
	*++string = '\0';
    }
    return (strlen(code));
}

void I3_message_convert(char *buffer, const char *txt, char *oname, char *tname)
{
    const char                             *point;
    int                                     skip = 0;

    for (point = txt; *point; point++) {
	if (*point == '$') {
	    point++;
	    if (*point == '\0')
		point--;
	    else
		skip = I3_token(*point, buffer, oname, tname);
	    while (skip-- > 0)
		++buffer;
	    continue;
	}
	*buffer = *point;
	*++buffer = '\0';
    }
    *buffer = '\0';
    return;
}

char                                   *I3_convert_channel_message(const char *message,
								   char *sname, char *tname)
{
    static char                             msgbuf[MAX_STRING_LENGTH];

    strcpy(msgbuf, "ERROR");
    /*
     * Sanity checks - if any of these are NULL, bad things will happen - Samson 6-29-01 
     */
    if (!message) {
	i3bug("%s", "I3_convert_channel_message: NULL message!");
	return msgbuf;
    }

    if (!sname) {
	i3bug("%s", "I3_convert_channel_message: NULL sname!");
	return msgbuf;
    }

    if (!tname) {
	i3bug("%s", "I3_convert_channel_message: NULL tname!");
	return msgbuf;
    }

    I3_message_convert(msgbuf, message, sname, tname);
    return msgbuf;
}

void update_chanhistory(I3_CHANNEL *channel, char *message)
{
    char                                    msg[MAX_STRING_LENGTH],
                                            buf[MAX_STRING_LENGTH];
    struct tm                              *local;
    time_t                                  t;
    int                                     x;

    if (!channel) {
	i3bug("%s", "update_chanhistory: NULL channel received!");
	return;
    }

    if (!message || message[0] == '\0') {
	i3bug("%s", "update_chanhistory: NULL message received!");
	return;
    }

    i3strlcpy(msg, message, MAX_STRING_LENGTH);
    for (x = 0; x < MAX_I3HISTORY; x++) {
	if (channel->history[x] == NULL) {
	    t = time(NULL);
	    local = localtime(&t);
	    snprintf(buf, MAX_STRING_LENGTH, "%%^RED%%^%%^BOLD%%^[%-4.4d-%-2.2d-%-2.2d %-2.2d:%-2.2d]%%^RESET%%^ %%^GREEN%%^%%^BOLD%%^%s",
		     local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, msg);
	    channel->history[x] = I3STRALLOC(buf);

	    if (I3IS_SET(channel->flags, I3CHAN_LOG)) {
		FILE                                   *fp;

		snprintf(buf, MAX_STRING_LENGTH, "%s%s.log", I3_DIR, channel->local_name);
		if (!(fp = fopen(buf, "a"))) {
		    perror(buf);
		    i3bug("Could not open file %s!", buf);
		} else {
		    fprintf(fp, "%s\n", i3_strip_colors(channel->history[x]));
		    I3FCLOSE(fp);
		}
	    }
	    break;
	}

	if (x == MAX_I3HISTORY - 1) {
	    int                                     y;

	    for (y = 1; y < MAX_I3HISTORY; y++) {
		int                                     z = y - 1;

		if (channel->history[z] != NULL) {
		    I3STRFREE(channel->history[z]);
		    channel->history[z] = I3STRALLOC(channel->history[y]);
		}
	    }

	    t = time(NULL);
	    local = localtime(&t);
	    snprintf(buf, MAX_STRING_LENGTH, "%%^RED%%^%%^BOLD%%^[%-4.4d-%-2.2d-%-2.2d %-2.2d:%-2.2d]%%^RESET%%^ %%^GREEN%%^%%^BOLD%%^%s",
		     local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, msg);
	    I3STRFREE(channel->history[x]);
	    channel->history[x] = I3STRALLOC(buf);

	    if (I3IS_SET(channel->flags, I3CHAN_LOG)) {
		FILE                                   *fp;

		snprintf(buf, MAX_STRING_LENGTH, "%s%s.log", I3_DIR, channel->local_name);
		if (!(fp = fopen(buf, "a"))) {
		    perror(buf);
		    i3bug("Could not open file %s!", buf);
		} else {
		    fprintf(fp, "%s\n", i3_strip_colors(channel->history[x]));
		    I3FCLOSE(fp);
		}
	    }
	}
    }
    return;
}

/* Handles the support for channel filtering.
 * Pretty basic right now. Any truly useful filtering would have to be at the discretion of the channel owner anyway.
 */
void I3_chan_filter_m(I3_CHANNEL *channel, I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    char                                    visname[MAX_INPUT_LENGTH],
                                            newmsg[MAX_STRING_LENGTH];

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(visname, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(newmsg, ps, MAX_STRING_LENGTH);
    snprintf(newmsg, MAX_STRING_LENGTH, "%s%s", ps, " (filtered M)");

    I3_write_header("chan-filter-reply", this_i3mud->name, NULL, I3_ROUTER_NAME, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",({\"channel-m\",5,\"");
    I3_write_buffer(header->originator_mudname);
    I3_write_buffer("\",\"");
    I3_write_buffer(header->originator_username);
    I3_write_buffer("\",0,0,\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",\"");
    I3_write_buffer(visname);
    I3_write_buffer("\",\"");
    I3_write_buffer(newmsg);
    I3_write_buffer("\",}),})\r");

    I3_send_packet();
    return;
}

/* Handles the support for channel filtering.
 * Pretty basic right now. Any truly useful filtering would have to be at the discretion of the channel owner anyway.
 */
void I3_chan_filter_e(I3_CHANNEL *channel, I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    char                                    visname[MAX_INPUT_LENGTH],
                                            newmsg[MAX_STRING_LENGTH];

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(visname, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    snprintf(newmsg, MAX_STRING_LENGTH, "%s%s", ps, " (filtered E)");

    I3_write_header("chan-filter-reply", this_i3mud->name, NULL, I3_ROUTER_NAME, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",({\"channel-e\",5,\"");
    I3_write_buffer(header->originator_mudname);
    I3_write_buffer("\",\"");
    I3_write_buffer(header->originator_username);
    I3_write_buffer("\",0,0,\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",\"");
    I3_write_buffer(visname);
    I3_write_buffer("\",\"");
    I3_write_buffer(newmsg);
    I3_write_buffer("\",}),})\r");

    I3_send_packet();
    return;
}

/* Handles the support for channel filtering.
 * Pretty basic right now. Any truly useful filtering would have to be at the discretion of the channel owner anyway.
 */
void I3_chan_filter_t(I3_CHANNEL *channel, I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    char                                    targetmud[MAX_INPUT_LENGTH],
                                            targetuser[MAX_INPUT_LENGTH],
                                            message_o[MAX_STRING_LENGTH],
                                            message_t[MAX_STRING_LENGTH];
    char                                    visname_o[MAX_INPUT_LENGTH];

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(targetmud, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(targetuser, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    snprintf(message_o, MAX_STRING_LENGTH, "%s%s", ps, " (filtered T)");

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    snprintf(message_t, MAX_STRING_LENGTH, "%s%s", ps, " (filtered T)");

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(visname_o, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    I3_write_header("chan-filter-reply", this_i3mud->name, NULL, I3_ROUTER_NAME, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",({\"channel-t\",5,\"");
    I3_write_buffer(header->originator_mudname);
    I3_write_buffer("\",\"");
    I3_write_buffer(header->originator_username);
    I3_write_buffer("\",0,0,\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",\"");
    I3_write_buffer(targetmud);
    I3_write_buffer("\",\"");
    I3_write_buffer(targetuser);
    I3_write_buffer("\",\"");
    send_to_i3(I3_escape(message_o));
    I3_write_buffer("\",\"");
    send_to_i3(I3_escape(message_t));
    I3_write_buffer("\",\"");
    I3_write_buffer(visname_o);
    I3_write_buffer("\",\"");
    I3_write_buffer(ps);
    I3_write_buffer("\",}),})\r");

    I3_send_packet();
    return;
}

void I3_process_channel_filter(I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    char                                    ptype[MAX_INPUT_LENGTH];
    I3_CHANNEL                             *channel = NULL;
    I3_HEADER                              *second_header;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    if (!(channel = find_I3_channel_by_name(ps))) {
	i3log("I3_process_channel_filter: received unknown channel (%s)", ps);
	return;
    }

    if (!channel->local_name)
	return;

    ps = next_ps;
    ps += 2;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(ptype, ps, MAX_INPUT_LENGTH);

    second_header = I3_get_header(&ps);

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    if (!strcasecmp(ptype, "channel-m"))
	I3_chan_filter_m(channel, second_header, next_ps);
    if (!strcasecmp(ptype, "channel-e"))
	I3_chan_filter_e(channel, second_header, next_ps);
    if (!strcasecmp(ptype, "channel-t"))
	I3_chan_filter_t(channel, second_header, next_ps);

    I3DISPOSE(second_header);
    return;
}

#define I3_ALLCHAN_LOG  I3_DIR "i3.allchan.log"

char *color_time( struct tm *local )
{
    static char                             timestamp[MAX_INPUT_LENGTH] = "\0\0\0\0\0\0\0\0";
    char                                   *hours[] = {
        "%^BLACK%^%^BOLD%^",
        "%^BLACK%^%^BOLD%^",
        "%^BLACK%^%^BOLD%^",
        "%^BLACK%^%^BOLD%^",
        "%^RED%^",
        "%^RED%^",
        "%^ORANGE%^",
        "%^ORANGE%^",
        "%^YELLOW%^",
        "%^YELLOW%^",
        "%^GREEN%^",
        "%^GREEN%^",
        "%^GREEN%^%^BOLD%^",
        "%^GREEN%^%^BOLD%^",
        "%^WHITE%^",
        "%^WHITE%^",
        "%^CYAN%^%^BOLD%^",
        "%^CYAN%^%^BOLD%^",
        "%^CYAN%^",
        "%^CYAN%^",
        "%^BLUE%^%^BOLD%^",
        "%^BLUE%^%^BOLD%^",
        "%^BLUE%^",
        "%^BLUE%^"
    };

    snprintf(timestamp, MAX_INPUT_LENGTH,
            "%s%-2.2d:%s%-2.2d%%^RESET%%^",
            hours[local->tm_hour], local->tm_hour, hours[local->tm_hour], local->tm_min);
    return timestamp;
}

void I3_process_channel_t(I3_HEADER *header, char *s)
{
    char                                   *ps = s;
    char                                   *next_ps;
    DESCRIPTOR_DATA                        *d;
    CHAR_DATA                              *vch = NULL;
    char                                    targetmud[MAX_INPUT_LENGTH],
                                            targetuser[MAX_INPUT_LENGTH],
                                            message_o[MAX_STRING_LENGTH],
                                            message_t[MAX_STRING_LENGTH],
                                            buf[MAX_STRING_LENGTH];
    char                                    visname_o[MAX_INPUT_LENGTH],
                                            sname[MAX_INPUT_LENGTH],
                                            tname[MAX_INPUT_LENGTH],
                                            lname[MAX_INPUT_LENGTH],
                                            layout[MAX_INPUT_LENGTH],
                                            tmsg[MAX_STRING_LENGTH],
                                            omsg[MAX_STRING_LENGTH];
    I3_CHANNEL                             *channel = NULL;
    struct tm                              *local = localtime(&i3_time);

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    if (!(channel = find_I3_channel_by_name(ps))) {
	i3log("I3_process_channel_t: received unknown channel (%s)", ps);
	return;
    }

    if (!channel->local_name)
	return;

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(targetmud, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(targetuser, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(message_o, ps, MAX_STRING_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(message_t, ps, MAX_STRING_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(visname_o, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    snprintf(sname, MAX_INPUT_LENGTH, "%s@%s", visname_o, header->originator_mudname);
    snprintf(tname, MAX_INPUT_LENGTH, "%s@%s", ps, targetmud);

    snprintf(omsg, MAX_STRING_LENGTH, "%s",
	     I3_convert_channel_message(message_o, sname, tname));
    snprintf(tmsg, MAX_STRING_LENGTH, "%s",
	     I3_convert_channel_message(message_t, sname, tname));

    strcpy(layout, "%s ");
    strcat(layout, channel->layout_e);

    for (d = first_descriptor; d; d = d->next) {
	vch = d->original ? d->original : d->character;

	if (!vch)
	    continue;

#if 0 // prool fool
	if (!I3_hasname(I3LISTEN(vch), channel->local_name)
	    || I3_hasname(I3DENY(vch), channel->local_name))
	    continue;
#endif

	snprintf(lname, MAX_INPUT_LENGTH, "%s@%s", CH_I3NAME(vch), this_i3mud->name);

	if (d->connected == CON_PLAYING && !i3ignoring(vch, sname)) {
	    if (!strcasecmp(lname, tname)) {
		sprintf(buf, layout, color_time(local), channel->local_name, tmsg);
		i3_printf(vch, "%s%%^RESET%%^\r\n", buf);
	    } else {
		sprintf(buf, layout, color_time(local), channel->local_name, omsg);
		i3_printf(vch, "%s%%^RESET%%^\r\n", buf);
	    }
	}
    }
    update_chanhistory(channel, omsg);
    tics_since_last_message = TAUNT_DELAY;
    return;
}

#define UNTINY      "../bin/untiny.pl"
#define PERL        "/usr/bin/perl"
#define I3_URL_DUMP I3_DIR "i3.urldump"

void I3_process_channel_m(I3_HEADER *header, char *s)
{
    char                                   *ps = s;
    char                                   *next_ps;
    DESCRIPTOR_DATA                        *d;
    CHAR_DATA                              *vch = NULL;
    char                                    visname[MAX_INPUT_LENGTH],
                                            buf[MAX_STRING_LENGTH],
                                            tps[MAX_STRING_LENGTH],
                                            format[MAX_INPUT_LENGTH];
    I3_CHANNEL                             *channel;
    struct tm                              *local = localtime(&i3_time);
    FILE                                   *fp = NULL;
    int                                     len;

    const char          *regexp_pattern     = "(https?\\:\\/\\/[\\w.-]+(?:\\.[\\w\\.-]+)+[\\w\\-\\._~:/?#[\\]@!\\$&'\\(\\)\\*\\+,;=.]+)";
    int                  regexp_opts        = PCRE_CASELESS|PCRE_MULTILINE;
    static pcre         *regexp_compiled    = NULL;
    static pcre_extra   *regexp_studied     = NULL;
    const char          *regexp_error       = NULL;
    int                  regexp_err_offset  = 0;
    const char          *regexp_match       = NULL;
    static int           regexp_broken      = 0;
    int                  regexp_rv;
    int                  regexp_matchpos[30];
    int                  regexp_pid;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    if (!(channel = find_I3_channel_by_name(ps))) {
	i3log("channel_m: received unknown channel (%s)", ps);
	return;
    }

    if (!channel->local_name)
	return;

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(visname, ps, MAX_INPUT_LENGTH);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    /* Try to squash multiple trailing newlines in the packet message */
    i3strlcpy(tps, ps, MAX_STRING_LENGTH);
    len = strlen(tps);
    while(ISNEWL(tps[len-1]) && len > 1) {
        tps[len-1] = '\0';
        len--;
    }

    strcpy(format, "%s ");
    strcat(format, channel->layout_m);
    snprintf(buf, MAX_STRING_LENGTH, format, color_time(local), channel->local_name, visname, header->originator_mudname, tps);

    if(!(fp = fopen(I3_ALLCHAN_LOG, "a"))) {
        perror(buf);
        i3bug("Could not open file %s!", I3_ALLCHAN_LOG);
    } else {
        fprintf(fp, "%04d.%02d.%02d-%02d.%02d,%02d%03d\t%s\t%s@%s\t%s\n",
                local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
                local->tm_hour, local->tm_min, local->tm_sec,
                0,
                channel->local_name,
                visname,
                header->originator_mudname,
                tps
               );
        I3FCLOSE(fp);
    }

    if(strcasecmp(channel->local_name, "url")) {
        // Never process url's from the url channel, recursion lurks here!

        //i3log("I3 regexp checking starts...");
        if(regexp_broken) {
            i3log("I3 regexp checking skipped");
            goto skip_regexp;
        }

        if(!strcasecmp(visname, "cron") && !strcasecmp(header->originator_mudname, "wileymud")) {
            // Also don't process things from Cron@WileyMUD...
            goto skip_regexp;
        }

        if(!regexp_compiled) { // Haven't compiled yet
            regexp_compiled = pcre_compile(regexp_pattern, regexp_opts, &regexp_error, &regexp_err_offset, NULL);
            i3log("I3 regexp: /%s/", regexp_pattern);
            if(!regexp_compiled) {
                i3bug("regexp failed to compile");
                regexp_broken = 1;
                goto skip_regexp;
            }
            regexp_studied = pcre_study(regexp_compiled, 0, &regexp_error);
            if(regexp_error != NULL) {
                i3bug("regexp study failed");
                regexp_broken = 1;
                goto skip_regexp;
            }
        }

        regexp_rv = pcre_exec(regexp_compiled, regexp_studied,
                              tps, strlen(tps),
                              0, 0, // START POS, OPTIONS
                              regexp_matchpos, 30);

        if(regexp_rv < 0) {
            switch(regexp_rv) {
                case PCRE_ERROR_NOMATCH:
                    //i3log("No match");
                    break;
                case PCRE_ERROR_NULL:
                    i3bug("NULL Error in regexp handling");
                    break;
                default:
                    i3bug("Error in regexp handling");
                    break;
            }
        } else {
            while( regexp_rv > 0 ) {
                if (pcre_get_substring(tps, regexp_matchpos, regexp_rv, 0, &regexp_match) >= 0) {
                    i3log("Found URL ( %s ) in channel ( %s )", regexp_match, channel->local_name);
                    regexp_pid = fork();
                    if( regexp_pid == 0 ) {
                        // We are the new kid, so go run our perl script.
                        //i3log("Launching %s -w %s %s %s", PERL, UNTINY, regexp_match, channel->local_name);
                        freopen(I3_URL_DUMP, "a", stdout);
                        execl(PERL, PERL, "-w", UNTINY, regexp_match, channel->local_name, (char *)NULL);
                        i3bug("It is not possible to be here!");
                    } else {
                        // Normally, we need to track the child pid, but we're already
                        // ignoring SIGCHLD in signals.c, and doing so prevents zombies.
                    }
                }
                regexp_rv = pcre_exec(regexp_compiled, regexp_studied,
                                      tps, strlen(tps),
                                      regexp_matchpos[1], 0,
                                      regexp_matchpos, 30);
            }
        }
        //i3log("I3 regexp checking ends.");
    }

skip_regexp:
    for (d = first_descriptor; d; d = d->next) {
	vch = d->original ? d->original : d->character;

	if (!vch)
	    continue;

#if 0 // prool fool
	if (!I3_hasname(I3LISTEN(vch), channel->local_name)
	    || I3_hasname(I3DENY(vch), channel->local_name))
	    continue;
#endif

	if (d->connected == CON_PLAYING && !i3ignoring(vch, visname)) {
	    i3_printf(vch, "%s%%^RESET%%^\r\n", buf);
        }
    }
    update_chanhistory(channel, buf);
    tics_since_last_message = TAUNT_DELAY;
    return;
}

void I3_process_channel_e(I3_HEADER *header, char *s)
{
    char                                   *ps = s;
    char                                   *next_ps;
    DESCRIPTOR_DATA                        *d;
    CHAR_DATA                              *vch = NULL;
    char                                    visname[MAX_INPUT_LENGTH],
                                            msg[MAX_STRING_LENGTH],
                                            buf[MAX_STRING_LENGTH],
                                            tps[MAX_STRING_LENGTH],
                                            format[MAX_INPUT_LENGTH];
    I3_CHANNEL                             *channel;
    struct tm                              *local = localtime(&i3_time);
    FILE                                   *fp = NULL;
    int                                     len;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    if (!(channel = find_I3_channel_by_name(ps))) {
	i3log("channel_e: received unknown channel (%s)", ps);
	return;
    }

    if (!channel->local_name)
	return;

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    snprintf(visname, MAX_INPUT_LENGTH, "%s@%s", ps, header->originator_mudname);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    /* Try to squash multiple trailing newlines in the packet message */
    i3strlcpy(tps, ps, MAX_STRING_LENGTH);
    len = strlen(tps);
    while(ISNEWL(tps[len-1]) && len > 1) {
        tps[len-1] = '\0';
        len--;
    }

    snprintf(msg, MAX_STRING_LENGTH, "%s", I3_convert_channel_message(tps, visname, visname));
    //strcpy(format, "%%^GREEN%%^%%^BOLD%%^%-2.2d%%^WHITE%%^%%^BOLD%%^:%%^GREEN%%^%%^BOLD%%^%-2.2d%%^RESET%%^ ");
    //strcat(format, channel->layout_e);
    //snprintf(buf, MAX_STRING_LENGTH, format, local->tm_hour, local->tm_min, channel->local_name, msg);

    strcpy(format, "%s ");
    strcat(format, channel->layout_e);
    snprintf(buf, MAX_STRING_LENGTH, format, color_time(local), channel->local_name, msg);

    if(!(fp = fopen(I3_ALLCHAN_LOG, "a"))) {
        perror(buf);
        i3bug("Could not open file %s!", I3_ALLCHAN_LOG);
    } else {
        fprintf(fp, "%04d.%02d.%02d-%02d.%02d,%02d%03d\t%s\t%s@%s\t%s\n",
                local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
                local->tm_hour, local->tm_min, local->tm_sec,
                0,
                channel->local_name,
                visname,
                header->originator_mudname,
                msg
               );
        I3FCLOSE(fp);
    }

    for (d = first_descriptor; d; d = d->next) {
	vch = d->original ? d->original : d->character;

	if (!vch)
	    continue;

#if 0 // proolfool
	if (!I3_hasname(I3LISTEN(vch), channel->local_name)
	    || I3_hasname(I3DENY(vch), channel->local_name))
	    continue;
#endif

	if (d->connected == CON_PLAYING && !i3ignoring(vch, visname))
	    i3_printf(vch, "%s%%^RESET%%^\r\n", buf);
    }
    update_chanhistory(channel, buf);
    tics_since_last_message = TAUNT_DELAY;
    return;
}

void I3_process_chan_who_req(I3_HEADER *header, char *s)
{
    CHAR_DATA                              *vch;
    DESCRIPTOR_DATA                        *d;
    char                                   *ps = s,
	*next_ps;
    char                                    buf[MAX_STRING_LENGTH],
                                            ibuf[MAX_INPUT_LENGTH];
    I3_CHANNEL                             *channel;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    snprintf(ibuf, MAX_INPUT_LENGTH, "%s@%s", header->originator_username,
	     header->originator_mudname);

    if (!(channel = find_I3_channel_by_name(ps))) {
	snprintf(buf, MAX_STRING_LENGTH, "The channel you specified (%s) is unknown at %s", ps,
		 this_i3mud->name);
	I3_send_error(header->originator_mudname, header->originator_username, "unk-channel",
		      buf);
	i3log("chan_who_req: received unknown channel (%s)", ps);
	return;
    }

    if (!channel->local_name) {
	snprintf(buf, MAX_STRING_LENGTH,
		 "The channel you specified (%s) is not registered at %s", ps, this_i3mud->name);
	I3_send_error(header->originator_mudname, header->originator_username, "unk-channel",
		      buf);
	return;
    }

    I3_write_header("chan-who-reply", this_i3mud->name, NULL, header->originator_mudname,
		    header->originator_username);
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",({");

    for (d = first_descriptor; d; d = d->next) {
	vch = d->original ? d->original : d->character;

	if (!vch)
	    continue;

#if 0 // prool fool
	if (I3ISINVIS(vch))
	    continue;
#endif

	if (1/*I3_hasname(I3LISTEN(vch), channel->local_name) && !i3ignoring(vch, ibuf)
	    && !I3_hasname(I3DENY(vch), channel->local_name)*/) { // proolfool
	    I3_write_buffer("\"");
	    I3_write_buffer(CH_I3NAME(vch));
	    I3_write_buffer("\",");
	}
    }
    I3_write_buffer("}),})\r");
    I3_send_packet();
    return;
}

void I3_process_chan_who_reply(I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    CHAR_DATA                              *ch;

    if (!(ch = I3_find_user(header->target_username))) {
	i3bug("I3_process_chan_who_reply(): user %s not found.", header->target_username);
	return;
    }

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3_printf(ch, "%%^WHITE%%^%%^BOLD%%^Users listening to %s on %s:%%^RESET%%^\r\n\r\n", ps, header->originator_mudname);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    ps += 2;
    while (1) {
	if (ps[0] == '}') {
	    i3_printf(ch, "%%^CYAN%%^No information returned or no people listening.%%^RESET%%^\r\n");
	    return;
	}
	I3_get_field(ps, &next_ps);
	I3_remove_quotes(&ps);
	i3_printf(ch, "%%^CYAN%%^%s%%^RESET%%^\r\n", ps);

	ps = next_ps;
	if (ps[0] == '}')
	    break;
    }
    return;
}

void I3_send_chan_who(CHAR_DATA *ch, I3_CHANNEL *channel, I3_MUD *mud)
{
    if (!i3_is_connected())
	return;

    I3_write_header("chan-who-req", this_i3mud->name, CH_I3NAME(ch), mud->name, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

void I3_send_beep(CHAR_DATA *ch, const char *to, I3_MUD *mud)
{
    if (!i3_is_connected())
	return;

    I3_escape(to);
    I3_write_header("beep", this_i3mud->name, CH_I3NAME(ch), mud->name, to);
    I3_write_buffer("\"");
    I3_write_buffer(CH_I3NAME(ch));
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

void I3_process_beep(I3_HEADER *header, char *s)
{
    char                                    buf[MAX_INPUT_LENGTH];
    char                                   *ps = s,
	*next_ps;
    CHAR_DATA                              *ch;

    snprintf(buf, MAX_INPUT_LENGTH, "%s@%s", header->originator_username,
	     header->originator_mudname);

    if (!(ch = I3_find_user(header->target_username))) {
	if (!i3exists_player(header->target_username))
	    I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
			  "No such player.");
	else
	    I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
			  "That player is offline.");
	return;
    }

#if 0 // prool fool
    if (I3PERM(ch) < I3PERM_MORT) {
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
		      "No such player.");
	return;
    }

    if (I3ISINVIS(ch) || i3ignoring(ch, buf)) {
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
		      "That player is offline.");
	return;
    }

    if (I3IS_SET(I3FLAG(ch), I3_BEEP)) {
	snprintf(buf, MAX_INPUT_LENGTH, "%s is not accepting beeps.", CH_I3NAME(ch));
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user", buf);
	return;
    }
#endif

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    i3_printf(ch, "%%^YELLOW%%^\a%s@%s i3beeps you.%%^RESET%%^\r\n", ps, header->originator_mudname);
    return;
}

void I3_send_tell(CHAR_DATA *ch, const char *to, const char *mud, const char *message)
{
    if (!i3_is_connected())
	return;

    I3_escape(to);
    I3_write_header("tell", this_i3mud->name, CH_I3NAME(ch), mud, to);
    I3_write_buffer("\"");
    I3_write_buffer(CH_I3NAME(ch));
    I3_write_buffer("\",\"");
    send_to_i3(I3_escape(message));
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

void i3_update_tellhistory(CHAR_DATA *ch, const char *msg)
{
    char                                    new_msg[MAX_STRING_LENGTH];
    time_t                                  t = time(NULL);
    struct tm                              *local = localtime(&t);
    int                                     x;

    snprintf(new_msg, MAX_STRING_LENGTH, "%%^RED%%^%%^BOLD%%^[%-2.2d:%-2.2d] %s", local->tm_hour, local->tm_min,
	     msg);
#if 0 // prool fool
    for (x = 0; x < MAX_I3TELLHISTORY; x++) {
	if (I3TELLHISTORY(ch, x) == '\0') {
	    I3TELLHISTORY(ch, x) = I3STRALLOC(new_msg);
	    break;
	}

	if (x == MAX_I3TELLHISTORY - 1) {
	    int                                     i;

	    for (i = 1; i < MAX_I3TELLHISTORY; i++) {
		I3STRFREE(I3TELLHISTORY(ch, i - 1));
		I3TELLHISTORY(ch, i - 1) = I3STRALLOC(I3TELLHISTORY(ch, i));
	    }
	    I3STRFREE(I3TELLHISTORY(ch, x));
	    I3TELLHISTORY(ch, x) = I3STRALLOC(new_msg);
	}
    }
#endif
    return;
}

void I3_process_tell(I3_HEADER *header, char *s)
{
    char                                    buf[MAX_INPUT_LENGTH],
                                            usr[MAX_INPUT_LENGTH];
    char                                   *ps = s,
	*next_ps;
    CHAR_DATA                              *ch;
    struct tm                              *local = localtime(&i3_time);

    snprintf(buf, MAX_INPUT_LENGTH, "%s@%s", header->originator_username,
	     header->originator_mudname);

    if (!(ch = I3_find_user(header->target_username))) {
	if (!i3exists_player(header->target_username))
	    I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
			  "No such player.");
	else
	    I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
			  "That player is offline.");
	return;
    }
#if 0 // prool fool
    if (I3PERM(ch) < I3PERM_MORT) {
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
		      "No such player.");
	return;
    }

    if (I3ISINVIS(ch) || i3ignoring(ch, buf)) {
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
		      "That player is offline.");
	return;
    }

    if (I3IS_SET(I3FLAG(ch), I3_TELL)) {
	snprintf(buf, MAX_INPUT_LENGTH, "%s is not accepting tells.", CH_I3NAME(ch));
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user", buf);
	return;
    }

    if (CH_I3AFK(ch)) {
	snprintf(buf, MAX_INPUT_LENGTH, "%s is currently AFK. Try back later.", CH_I3NAME(ch));
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user", buf);
	return;
    }
#endif
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    snprintf(usr, MAX_INPUT_LENGTH, "%s@%s", ps, header->originator_mudname);
    //snprintf(buf, MAX_INPUT_LENGTH, "'%s@%s'", ps, header->originator_mudname);
    //snprintf(buf, MAX_INPUT_LENGTH, "%s@%s", header->originator_username, header->originator_mudname);

#if 0 // prool fool unknown!
    I3STRFREE(I3REPLYNAME(ch));
    I3STRFREE(I3REPLYMUD(ch));
    I3REPLYNAME(ch) = I3STRALLOC(header->originator_username);
    I3REPLYMUD(ch) = I3STRALLOC(header->originator_mudname);
#endif

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    snprintf(buf, MAX_INPUT_LENGTH, "%s %%^CYAN%%^%%^BOLD%%^%s%%^RESET%%^ %%^YELLOW%%^i3tells you: %%^RESET%%^%s", color_time(local), usr, ps);
    i3_printf(ch, "%s%%^RESET%%^\r\n", buf);
    i3_update_tellhistory(ch, buf);
    tics_since_last_message = TAUNT_DELAY;
    return;
}

void I3_send_who(CHAR_DATA *ch, char *mud)
{
    if (!i3_is_connected())
	return;

    I3_escape(mud);
    I3_write_header("who-req", this_i3mud->name, CH_I3NAME(ch), mud, NULL);
    I3_write_buffer("})\r");
    I3_send_packet();

    return;
}

char                                   *i3centerline(const char *string, int len)
{
    char                                    stripped[300];
    static char                             outbuf[400];
    int                                     amount;

    i3strlcpy(stripped, i3_strip_colors(string), 300);
    amount = len - strlen(stripped);			       /* Determine amount to put in front of line */

    if (amount < 1)
	amount = 1;

    /*
     * Justice, you are the String God! 
     */
    snprintf(outbuf, 400, "%*s%s%*s", (amount / 2), "", string,
	     ((amount / 2) * 2) == amount ? (amount / 2) : ((amount / 2) + 1), "");

    return outbuf;
}

char                                   *i3rankbuffer(CHAR_DATA *ch)
{
    static char                             rbuf[MAX_INPUT_LENGTH];
#if 0 // prool fool
    if (I3PERM(ch) >= I3PERM_IMM) {
	i3strlcpy(rbuf, "~YStaff", MAX_INPUT_LENGTH);

	if (CH_I3RANK(ch) && CH_I3RANK(ch)[0] != '\0')
	    snprintf(rbuf, MAX_INPUT_LENGTH, "~Y%s", I3_mudtag_to_i3tag(CH_I3RANK(ch)));
    } else {
	i3strlcpy(rbuf, "~BPlayer", MAX_INPUT_LENGTH);

	if (CH_I3RANK(ch) && CH_I3RANK(ch)[0] != '\0')
	    snprintf(rbuf, MAX_INPUT_LENGTH, "~B%s", I3_mudtag_to_i3tag(CH_I3RANK(ch)));
    }
#else
	i3strlcpy(rbuf, "~BPlayer", MAX_INPUT_LENGTH);
#endif
    return rbuf;
}

void I3_process_who_req(I3_HEADER *header, char *s)
{
    DESCRIPTOR_DATA                        *d;
    CHAR_DATA                              *person;
    char                                    ibuf[MAX_INPUT_LENGTH],
                                            personbuf[MAX_STRING_LENGTH],
                                            tailbuf[MAX_STRING_LENGTH];
    char                                    smallbuf[50],
                                            buf[300],
                                            outbuf[400],
                                            stats[200],
                                            rank[200];
    int                                     pcount = 0,
	xx,
	yy;
    long int                                bogusidle = 9999;
    char                                    boottimebuf[100];

#if 0 // 0 - prool 1 -orig
    strftime(boottimebuf, sizeof(boottimebuf), RFC1123FMT, localtime((time_t *) & Uptime));
#else
time_t	uptime;
		uptime=time(0) - boot_time;
    strftime(boottimebuf, sizeof(boottimebuf), RFC1123FMT, localtime(&uptime));
#endif
    snprintf(ibuf, MAX_INPUT_LENGTH, "%s@%s", header->originator_username,
	     header->originator_mudname);

    I3_write_header("who-reply", this_i3mud->name, NULL, header->originator_mudname,
		    header->originator_username);
    I3_write_buffer("({");

    I3_write_buffer("({\"");
    snprintf(buf, 300, "%%^RED%%^%%^BOLD%%^-=[ %%^WHITE%%^%%^BOLD%%^Players on %s %%^RED%%^%%^BOLD%%^]=-", this_i3mud->name);
    i3strlcpy(outbuf, i3centerline(buf, 78), 400);
    send_to_i3(I3_escape(outbuf));

    I3_write_buffer("\",");
    snprintf(smallbuf, 50, "%ld", -1l);
    I3_write_buffer(smallbuf);

    I3_write_buffer(",\" \",}),({\"");
    snprintf(buf, 300, "%%^YELLOW%%^-=[ %%^WHITE%%^%%^BOLD%%^telnet://%s:%d %%^YELLOW%%^]=-", this_i3mud->telnet,
	     this_i3mud->player_port);
    i3strlcpy(outbuf, i3centerline(buf, 78), 400);
    send_to_i3(I3_escape(outbuf));

    I3_write_buffer("\",");
    snprintf(smallbuf, 50, "%ld", bogusidle);
    I3_write_buffer(smallbuf);

    I3_write_buffer(",\" \",}),");

    xx = 0;
    for (d = first_descriptor; d; d = d->next) {
	person = d->original ? d->original : d->character;

	if (person && d->connected >= CON_PLAYING) {
#if 0 // prool fool
	    if (I3PERM(person) < I3PERM_MORT || I3PERM(person) >= I3PERM_IMM
		|| I3ISINVIS(person) || i3ignoring(person, ibuf))
		continue;
#endif

	    pcount++;

	    if (xx == 0) {
		I3_write_buffer("({\"");
		send_to_i3(I3_escape
			   ("%%^BLUE%%^%%^BOLD%%^--------------------------------=[ %%^WHITE%%^%%^BOLD%%^Players %%^BLUE%%^%%^BOLD%%^]=---------------------------------"));
		I3_write_buffer("\",");
		snprintf(smallbuf, 50, "%ld", bogusidle);
		I3_write_buffer(smallbuf);
		I3_write_buffer(",\" \",}),");
	    }

	    I3_write_buffer("({\"");

	    i3strlcpy(rank, i3rankbuffer(person), 200);
	    i3strlcpy(outbuf, i3centerline(rank, 20), 400);
	    send_to_i3(I3_escape(outbuf));

	    I3_write_buffer("\",");
	    snprintf(smallbuf, 50, "%ld", -1l);
	    I3_write_buffer(smallbuf);
	    I3_write_buffer(",\"");

	    i3strlcpy(stats, "&z[", 200);
	    if (0/*CH_I3AFK(person)*/) // prool fool
		i3strlcat(stats, "AFK", 200);
	    else
		i3strlcat(stats, "---", 200);
	    i3strlcat(stats, "]%%^GREEN%%^%%^BOLD%%^", 200);

	    snprintf(personbuf, MAX_STRING_LENGTH, "%s %s%s", stats, CH_I3NAME(person),
		     CH_I3TITLE(person));
	    send_to_i3(I3_escape(personbuf));
	    I3_write_buffer("\",}),");
	    xx++;
	}
    }

    yy = 0;
    for (d = first_descriptor; d; d = d->next) {
	person = d->original ? d->original : d->character;

	if (person && d->connected >= CON_PLAYING) {
#if 0 // prool fool
	    if (I3PERM(person) < I3PERM_IMM || I3ISINVIS(person) || i3ignoring(person, ibuf))
		continue;
#endif

	    pcount++;

	    if (yy == 0) {
		I3_write_buffer("({\"");
		send_to_i3(I3_escape
			   ("%%^RED%%^%%^BOLD%%^-------------------------------=[ %%^WHITE%%^%%^BOLD%%^Immortals %%^RED%%^%%^BOLD%%^]=--------------------------------"));
		I3_write_buffer("\",");
		if (xx > 0)
		    snprintf(smallbuf, 50, "%ld", bogusidle * 3);
		else
		    snprintf(smallbuf, 50, "%ld", bogusidle);
		I3_write_buffer(smallbuf);
		I3_write_buffer(",\" \",}),");
	    }
	    I3_write_buffer("({\"");

	    i3strlcpy(rank, i3rankbuffer(person), 200);
	    i3strlcpy(outbuf, i3centerline(rank, 20), 400);
	    send_to_i3(I3_escape(outbuf));

	    I3_write_buffer("\",");
	    snprintf(smallbuf, 50, "%ld", -1l);
	    I3_write_buffer(smallbuf);
	    I3_write_buffer(",\"");

	    i3strlcpy(stats, "&z[", 200);
	    if (0/*CH_I3AFK(person)*/) // prool fool
		i3strlcat(stats, "AFK", 200);
	    else
		i3strlcat(stats, "---", 200);
	    i3strlcat(stats, "]%%^GREEN%%^%%^BOLD%%^", 200);

	    snprintf(personbuf, MAX_STRING_LENGTH, "%s %s%s", stats, CH_I3NAME(person),
		     CH_I3TITLE(person));
	    send_to_i3(I3_escape(personbuf));
	    I3_write_buffer("\",}),");
	    yy++;
	}
    }

    I3_write_buffer("({\"");
    snprintf(tailbuf, MAX_STRING_LENGTH, "%%^YELLOW%%^[%%^WHITE%%^%%^BOLD%%^%d Player%s%%^YELLOW%%^]", pcount, pcount == 1 ? "" : "s");
    send_to_i3(I3_escape(tailbuf));
    I3_write_buffer("\",");
    snprintf(smallbuf, 50, "%ld", bogusidle * 2);
    I3_write_buffer(smallbuf);
    I3_write_buffer(",\"");
    snprintf(tailbuf, MAX_STRING_LENGTH, "%%^YELLOW%%^[%%^WHITE%%^%%^BOLD%%^Homepage: %s%%^YELLOW%%^] [%%^WHITE%%^%%^BOLD%%^%d Max Since Reboot%%^YELLOW%%^]",
	     HTTP_PLACEHOLDER, MAXPLAYERS_PLACEHOLDER);
    send_to_i3(I3_escape(tailbuf));
    I3_write_buffer("\",}),");

    I3_write_buffer("({\"");
    snprintf(tailbuf, MAX_STRING_LENGTH, "%%^YELLOW%%^[%%^WHITE%%^%%^BOLD%%^%d logins since last reboot on %s%%^YELLOW%%^]",
	     NUMLOGINS_PLACEHOLDER, boottimebuf);
    send_to_i3(I3_escape(tailbuf));
    I3_write_buffer("\",");
    snprintf(smallbuf, 50, "%ld", bogusidle);
    I3_write_buffer(smallbuf);
    I3_write_buffer(",\" \",}),}),})\r");
    I3_send_packet();

    return;
}

/* This is where the incoming results of a who-reply packet are processed.
 * Note that rather than just spit the names out, I've copied the packet fields into
 * buffers to be output later. Also note that if it receives an idle value of 9999
 * the normal 30 space output will be bypassed. This is so that muds who want to
 * customize the listing headers in their who-reply packets can do so and the results
 * won't get chopped off after the 30th character. If for some reason a person on
 * the target mud just happens to have been idling for 9999 cycles, their data may
 * be displayed strangely compared to the rest. But I don't expect that 9999 is a very
 * common length of time to be idle either :P
 * Receving an idle value of 19998 may also cause odd results since this is used
 * to indicate receipt of the last line of a who, which is typically the number of
 * visible players found.
 */
void I3_process_who_reply(I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps,
	*next_ps2;
    CHAR_DATA                              *ch;
    char                                    person[MAX_STRING_LENGTH],
                                            title[MAX_INPUT_LENGTH];
    int                                     idle;

    if (!(ch = I3_find_user(header->target_username)))
	return;

    ps += 2;

    while (1) {
	if (ps[0] == '}') {
	    i3_printf(ch, "%%^WHITE%%^%%^BOLD%%^No information returned.%%^RESET%%^\r\n");
	    return;
	}

	I3_get_field(ps, &next_ps);

	ps += 2;
	I3_get_field(ps, &next_ps2);
	I3_remove_quotes(&ps);
	i3strlcpy(person, ps, MAX_STRING_LENGTH);
	ps = next_ps2;
	I3_get_field(ps, &next_ps2);
	idle = atoi(ps);
	ps = next_ps2;
	I3_get_field(ps, &next_ps2);
	I3_remove_quotes(&ps);
	i3strlcpy(title, ps, MAX_INPUT_LENGTH);
	ps = next_ps2;

	if (idle == 9999)
	    i3_printf(ch, "%s %s\r\n\r\n", person, title);
	else if (idle == 19998)
	    i3_printf(ch, "\r\n%s %s\r\n", person, title);
	else if (idle == 29997)
	    i3_printf(ch, "\r\n%s %s\r\n\r\n", person, title);
	else
	    i3_printf(ch, "%s %s\r\n", person, title);

	ps = next_ps;
	if (ps[0] == '}')
	    break;
    }
    return;
}

void I3_send_emoteto(CHAR_DATA *ch, const char *to, I3_MUD *mud, const char *message)
{
    char                                    buf[MAX_STRING_LENGTH];

    if (!i3_is_connected())
	return;

    if (strstr(message, "$N") == NULL)
	snprintf(buf, MAX_STRING_LENGTH, "$N %s", message);
    else
	i3strlcpy(buf, message, MAX_STRING_LENGTH);

    I3_escape(to);
    I3_write_header("emoteto", this_i3mud->name, CH_I3NAME(ch), mud->name, to);
    I3_write_buffer("\"");
    I3_write_buffer(CH_I3NAME(ch));
    I3_write_buffer("\",\"");
    send_to_i3(I3_escape(buf));
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

void I3_process_emoteto(I3_HEADER *header, char *s)
{
    CHAR_DATA                              *ch;
    char                                   *ps = s,
	*next_ps;
    char                                    visname[MAX_INPUT_LENGTH],
                                            buf[MAX_INPUT_LENGTH];

    snprintf(buf, MAX_INPUT_LENGTH, "%s@%s", header->originator_username,
	     header->originator_mudname);

    if (!(ch = I3_find_user(header->target_username))) {
	if (!i3exists_player(header->target_username))
	    I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
			  "No such player.");
	else
	    I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
			  "That player is offline.");
	return;
    }
#if 0 // prool fool
    if (I3PERM(ch) < I3PERM_MORT) {
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
		      "No such player.");
	return;
    }

    if (I3ISINVIS(ch) || i3ignoring(ch, buf) || !ch->desc) {
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
		      "That player is offline.");
	return;
    }
#endif

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    snprintf(visname, MAX_INPUT_LENGTH, "%s@%s", ps, header->originator_mudname);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    i3_printf(ch, "%%^CYAN%%^%s%%^RESET%%^\r\n",
            I3_convert_channel_message(ps, visname, visname));
    return;
}

void I3_send_finger(CHAR_DATA *ch, char *user, char *mud)
{
    if (!i3_is_connected())
	return;

    I3_escape(mud);

    I3_write_header("finger-req", this_i3mud->name, CH_I3NAME(ch), mud, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(I3_escape(user));
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

/* The output of this was slightly modified to resemble the Finger snippet */
void I3_process_finger_reply(I3_HEADER *header, char *s)
{
    CHAR_DATA                              *ch;
    char                                   *ps = s,
	*next_ps;
    char                                    title[MAX_INPUT_LENGTH],
                                            email[MAX_INPUT_LENGTH],
                                            last[MAX_INPUT_LENGTH],
                                            level[MAX_INPUT_LENGTH];

    if (!(ch = I3_find_user(header->target_username)))
	return;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3_printf(ch, "%%^WHITE%%^I3FINGER information for %%^GREEN%%^%%^BOLD%%^%s@%s%%^RESET%%^\r\n", ps, header->originator_mudname);
    i3_printf(ch, "%%^WHITE%%^-------------------------------------------------%%^RESET%%^\r\n");
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(title, ps, MAX_INPUT_LENGTH);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(email, ps, MAX_INPUT_LENGTH);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(last, ps, MAX_INPUT_LENGTH);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(level, ps, MAX_INPUT_LENGTH);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    i3_printf(ch, "%%^WHITE%%^Title: %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", title);
    i3_printf(ch, "%%^WHITE%%^Level: %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", level);
    i3_printf(ch, "%%^WHITE%%^Email: %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", email);
    i3_printf(ch, "%%^WHITE%%^HTTP : %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", ps);
    i3_printf(ch, "%%^WHITE%%^Last on: %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", last);

    return;
}

void I3_process_finger_req(I3_HEADER *header, char *s)
{
    CHAR_DATA                              *ch;
    char                                   *ps = s,
	*next_ps;
    char                                    smallbuf[200],
                                            buf[MAX_INPUT_LENGTH];

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    snprintf(buf, MAX_INPUT_LENGTH, "%s@%s", header->originator_username,
	     header->originator_mudname);

    if (!(ch = I3_find_user(ps))) {
	if (!i3exists_player(ps))
	    I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
			  "No such player.");
	else
	    I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
			  "That player is offline.");
	return;
    }
#if 0 // prool fool
    if (I3PERM(ch) < I3PERM_MORT) {
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
		      "No such player.");
	return;
    }

    if (I3ISINVIS(ch) || i3ignoring(ch, buf)) {
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user",
		      "That player is offline.");
	return;
    }

    if (I3IS_SET(I3FLAG(ch), I3_DENYFINGER) || I3IS_SET(I3FLAG(ch), I3_PRIVACY)) {
	snprintf(buf, MAX_INPUT_LENGTH, "%s is not accepting fingers.", CH_I3NAME(ch));
	I3_send_error(header->originator_mudname, header->originator_username, "unk-user", buf);
	return;
    }
#endif

    i3_printf(ch, "%s@%s has requested your i3finger information.\r\n", // prool fool
	      header->originator_username, header->originator_mudname);

    I3_write_header("finger-reply", this_i3mud->name, NULL, header->originator_mudname,
		    header->originator_username);
    I3_write_buffer("\"");
    I3_write_buffer(I3_escape(CH_I3NAME(ch)));
    I3_write_buffer("\",\"");
    I3_write_buffer(I3_escape(CH_I3NAME(ch)));
    send_to_i3(I3_escape(CH_I3TITLE(ch)));
    I3_write_buffer("\",\"\",\"");
#if 0
    if (ch->pcdata->email) {
	if (!IS_SET(ch->pcdata->flags, PCFLAG_PRIVACY))
	    I3_write_buffer(ch->pcdata->email);
	else
	    I3_write_buffer("[Private]");
    }
#endif
    I3_write_buffer("[Private]");
    I3_write_buffer("\",\"");
    i3strlcpy(smallbuf, "-1", 200);			       /* online since */
    I3_write_buffer(smallbuf);
    I3_write_buffer("\",");
    snprintf(smallbuf, 200, "%ld", -1l);
    I3_write_buffer(smallbuf);
    I3_write_buffer(",\"");
    I3_write_buffer("[PRIVATE]");
    I3_write_buffer("\",\"");
    snprintf(buf, MAX_INPUT_LENGTH, "%s", i3rankbuffer(ch));
    send_to_i3(buf);
    I3_write_buffer("\",\"");
#if 0
    if (ch->pcdata->homepage)
	I3_write_buffer(I3_escape(ch->pcdata->homepage));
    else
	I3_write_buffer("Not Provided");
#endif
    I3_write_buffer("[Private]");
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

void I3_send_locate(CHAR_DATA *ch, const char *user)
{
    if (!i3_is_connected())
	return;

    I3_write_header("locate-req", this_i3mud->name, CH_I3NAME(ch), NULL, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(I3_escape(user));
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

void I3_process_locate_reply(I3_HEADER *header, char *s)
{
    char                                    mud_name[MAX_INPUT_LENGTH],
                                            user_name[MAX_INPUT_LENGTH],
                                            status[MAX_INPUT_LENGTH];
    char                                   *ps = s,
	*next_ps;
    CHAR_DATA                              *ch;

    if (!(ch = I3_find_user(header->target_username)))
	return;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(mud_name, ps, MAX_INPUT_LENGTH);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(user_name, ps, MAX_INPUT_LENGTH);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    ps = next_ps;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(status, ps, MAX_INPUT_LENGTH);

    if (!strcasecmp(status, "active"))
	i3strlcpy(status, "Online", MAX_INPUT_LENGTH);

    if (!strcasecmp(status, "exists, but not logged on"))
	i3strlcpy(status, "Offline", MAX_INPUT_LENGTH);

    i3_printf(ch, "%%^RED%%^%%^BOLD%%^I3 Locate: %%^YELLOW%%^%s@%s: %%^CYAN%%^%s.%%^RESET%%^\r\n", user_name, mud_name, status);
    return;
}

void I3_process_locate_req(I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    char                                    smallbuf[50],
                                            buf[MAX_INPUT_LENGTH];
    CHAR_DATA                              *ch;
    bool                                    choffline = FALSE;

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);

    snprintf(buf, MAX_INPUT_LENGTH, "%s@%s", header->originator_username,
	     header->originator_mudname);

    if (!(ch = I3_find_user(ps))) {
	if (i3exists_player(ps))
	    choffline = TRUE;
	else
	    return;
    }

#if 0 // prool fool
    if (ch) {
	if (I3PERM(ch) < I3PERM_MORT)
	    return;

	if (I3ISINVIS(ch))
	    choffline = TRUE;

	if (i3ignoring(ch, buf))
	    choffline = TRUE;
    }
#endif

    I3_write_header("locate-reply", this_i3mud->name, NULL, header->originator_mudname,
		    header->originator_username);
    I3_write_buffer("\"");
    I3_write_buffer(this_i3mud->name);
    I3_write_buffer("\",\"");
    if (!choffline)
	I3_write_buffer(CH_I3NAME(ch));
    else
	I3_write_buffer(i3capitalize(ps));
    I3_write_buffer("\",");
    snprintf(smallbuf, 50, "%ld", -1l);
    I3_write_buffer(smallbuf);
    if (!choffline)
	I3_write_buffer(",\"Online\",})\r");
    else
	I3_write_buffer(",\"Offline\",})\r");
    I3_send_packet();

    return;
}

void I3_send_channel_listen(I3_CHANNEL *channel, bool lconnect)
{
    if (!i3_is_connected())
	return;

    I3_write_header("channel-listen", this_i3mud->name, NULL, I3_ROUTER_NAME, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",");
    if (lconnect)
	I3_write_buffer("1,})\r");
    else
	I3_write_buffer("0,})\r");
    I3_send_packet();

    return;
}

void I3_process_channel_adminlist_reply(I3_HEADER *header, char *s)
{
    char                                   *ps = s,
	*next_ps;
    I3_CHANNEL                             *channel;
    CHAR_DATA                              *ch;

    if ((ch = I3_find_user(header->target_username)) == NULL) {
	i3bug("I3_process_channel_adminlist_reply(): user %s not found.",
	      header->target_username);
	return;
    }

    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    if (!(channel = find_I3_channel_by_name(ps))) {
	i3bug("I3_process_channel_adminlist_reply(): Invalid local channel %s reply received.",
	      ps);
	return;
    }
    i3_printf(ch, "%%^RED%%^%%^BOLD%%^The following muds are %s %s:%%^RESET%%^\r\n\r\n",
	      channel->status == 0 ? "banned from" : "invited to", channel->local_name);

    ps = next_ps;
    I3_get_field(ps, &next_ps);
    ps += 2;
    while (1) {
	if (ps[0] == '}') {
	    i3_printf(ch, "%%^YELLOW%%^No entries found.%%^RESET%%^\r\n");
	    return;
	}

	I3_get_field(ps, &next_ps);
	I3_remove_quotes(&ps);
	i3_printf(ch, "%%^YELLOW%%^%s%%^RESET%%^\r\n", ps);

	ps = next_ps;
	if (ps[0] == '}')
	    break;
    }
    return;
}

void I3_send_channel_adminlist(CHAR_DATA *ch, char *chan_name)
{
    if (!i3_is_connected())
	return;

    I3_write_header("chan-adminlist", this_i3mud->name, CH_I3NAME(ch), I3_ROUTER_NAME, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(chan_name);
    I3_write_buffer("\",})\r");
    I3_send_packet();

    return;
}

void I3_send_channel_admin(CHAR_DATA *ch, char *chan_name, char *list)
{
    if (!i3_is_connected())
	return;

    I3_write_header("channel-admin", this_i3mud->name, CH_I3NAME(ch), I3_ROUTER_NAME, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(chan_name);
    I3_write_buffer("\",");
    I3_write_buffer(list);
    I3_write_buffer("})\r");
    I3_send_packet();

    return;
}

void I3_send_channel_add(CHAR_DATA *ch, char *arg, int type)
{
    if (!i3_is_connected())
	return;

    I3_write_header("channel-add", this_i3mud->name, CH_I3NAME(ch), I3_ROUTER_NAME, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(arg);
    I3_write_buffer("\",");
    switch (type) {
	default:
	    i3bug("%s", "I3_send_channel_add: Illegal channel type!");
	    return;
	case 0:
	    I3_write_buffer("0,})\r");
	    break;
	case 1:
	    I3_write_buffer("1,})\r");
	    break;
	case 2:
	    I3_write_buffer("2,})\r");
	    break;
    }
    I3_send_packet();
    return;
}

void I3_send_channel_remove(CHAR_DATA *ch, I3_CHANNEL *channel)
{
    if (!i3_is_connected())
	return;

    I3_write_header("channel-remove", this_i3mud->name, CH_I3NAME(ch), I3_ROUTER_NAME, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",})\r");
    I3_send_packet();
    return;
}

void I3_send_shutdown(int delay)
{
    I3_CHANNEL                             *channel;
    char                                    s[50];

    if (!i3_is_connected())
	return;

    for (channel = first_I3chan; channel; channel = channel->next) {
	if (channel->local_name && channel->local_name[0] != '\0')
	    I3_send_channel_listen(channel, FALSE);
    }

    I3_write_header("shutdown", this_i3mud->name, NULL, I3_ROUTER_NAME, NULL);
    snprintf(s, 50, "%d", delay);
    I3_write_buffer(s);
    I3_write_buffer(",})\r");

    if (!I3_write_packet(I3_output_buffer))
	I3_connection_close(FALSE);

    return;
}

/*
 * Read the header of an I3 packet. pps will point to the next field
 * of the packet.
 */
I3_HEADER                              *I3_get_header(char **pps)
{
    I3_HEADER                              *header;
    char                                   *ps = *pps,
	*next_ps;

    I3CREATE(header, I3_HEADER, 1);

    I3_get_field(ps, &next_ps);
    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(header->originator_mudname, ps, MAX_INPUT_LENGTH);
    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(header->originator_username, ps, MAX_INPUT_LENGTH);
    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(header->target_mudname, ps, MAX_INPUT_LENGTH);
    ps = next_ps;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(header->target_username, ps, MAX_INPUT_LENGTH);

    *pps = next_ps;
    return header;
}

/*
 * Read the first field of an I3 packet and call the proper function to
 * process it. Afterwards the original I3 packet is completly messed up.
 *
 * Reworked on 9-5-03 by Samson to be made more efficient with regard to banned muds.
 * Also only need to gather the header information once this way instead of in multiple places.
 */
void I3_parse_packet(void)
{
    I3_HEADER                              *header = NULL;
    char                                   *ps,
                                           *next_ps;
    char                                    ptype[MAX_INPUT_LENGTH];

    ps = I3_currentpacket;
    if (ps[0] != '(' || ps[1] != '{')
	return;

    if (packetdebug)
	i3log("Packet received: %s", ps);

    ps += 2;
    I3_get_field(ps, &next_ps);
    I3_remove_quotes(&ps);
    i3strlcpy(ptype, ps, MAX_INPUT_LENGTH);

    header = I3_get_header(&ps);

    /*
     * There. Nice and simple, no? 
     */
    if (i3banned(header->originator_mudname))
	return;

    if (!strcasecmp(ptype, "tell"))
	I3_process_tell(header, ps);

    if (!strcasecmp(ptype, "beep"))
	I3_process_beep(header, ps);

    if (!strcasecmp(ptype, "emoteto"))
	I3_process_emoteto(header, ps);

    if (!strcasecmp(ptype, "channel-m"))
	I3_process_channel_m(header, ps);

    if (!strcasecmp(ptype, "channel-e"))
	I3_process_channel_e(header, ps);

    if (!strcasecmp(ptype, "chan-filter-req"))
	I3_process_channel_filter(header, ps);

    if (!strcasecmp(ptype, "finger-req"))
	I3_process_finger_req(header, ps);

    if (!strcasecmp(ptype, "finger-reply"))
	I3_process_finger_reply(header, ps);

    if (!strcasecmp(ptype, "locate-req"))
	I3_process_locate_req(header, ps);

    if (!strcasecmp(ptype, "locate-reply"))
	I3_process_locate_reply(header, ps);

    if (!strcasecmp(ptype, "chan-who-req"))
	I3_process_chan_who_req(header, ps);

    if (!strcasecmp(ptype, "chan-who-reply"))
	I3_process_chan_who_reply(header, ps);

    if (!strcasecmp(ptype, "chan-adminlist-reply"))
	I3_process_channel_adminlist_reply(header, ps);

    if (!strcasecmp(ptype, "ucache-update") && this_i3mud->ucache == TRUE)
	I3_process_ucache_update(header, ps);

    if (!strcasecmp(ptype, "who-req"))
	I3_process_who_req(header, ps);

    if (!strcasecmp(ptype, "who-reply"))
	I3_process_who_reply(header, ps);

    if (!strcasecmp(ptype, "chanlist-reply"))
	I3_process_chanlist_reply(header, ps);

    if (!strcasecmp(ptype, "startup-reply"))
	I3_process_startup_reply(header, ps);

    if (!strcasecmp(ptype, "mudlist"))
	I3_process_mudlist(header, ps);

    if (!strcasecmp(ptype, "error"))
	I3_process_error(header, ps);

    if (!strcasecmp(ptype, "chan-ack"))
	I3_process_chanack(header, ps);

    if (!strcasecmp(ptype, "channel-t"))
	I3_process_channel_t(header, ps);

    if (!strcasecmp(ptype, "chan-user-req"))
	I3_process_chan_user_req(header, ps);

    if (!strcasecmp(ptype, "chan-user-reply") && this_i3mud->ucache == TRUE)
	I3_process_chan_user_reply(header, ps);

    if (!strcasecmp(ptype, "router-shutdown")) {
	int                                     delay;

	I3_get_field(ps, &next_ps);
	delay = atoi(ps);

	if (delay == 0) {
	    i3log("Router %s is shutting down.", I3_ROUTER_NAME);
	    I3_connection_close(FALSE);
	} else {
	    i3log("Router %s is rebooting and will be back in %d second%s.", I3_ROUTER_NAME,
		  delay, delay == 1 ? "" : "s");
	    I3_connection_close(TRUE);
	}
    }
    I3DISPOSE(header);
    return;
}

/*
 * Read one I3 packet into the I3_input_buffer
 */
void I3_read_packet(void)
{
    long                                    size;

    memmove(&size, I3_input_buffer, 4);
    size = ntohl(size);

    memmove(I3_currentpacket, I3_input_buffer + 4, size);

    if (I3_currentpacket[size - 1] != ')')
	I3_currentpacket[size - 1] = 0;
    I3_currentpacket[size] = 0;

    memmove(I3_input_buffer, I3_input_buffer + size + 4, I3_input_pointer - size - 4);
    I3_input_pointer -= size + 4;
    return;
}

/************************************
 * User login and logout functions. *
 ************************************/

void i3_initchar(CHAR_DATA *ch)
{
    //log_info("Setting up I3 data"); // prool
    if (IS_NPC(ch))
	return;

#if 0 // prool fool
    I3CREATE(CH_I3DATA(ch), I3_CHARDATA, 1);
    I3LISTEN(ch) = NULL;
    I3DENY(ch) = NULL;
    I3REPLYNAME(ch) = NULL;
    I3REPLYMUD(ch) = NULL;
    I3FLAG(ch) = 0;
    I3SET_BIT(I3FLAG(ch), I3_COLORFLAG);		       /* Default color to on. People can turn this off if they 
							        * hate it. */
    FIRST_I3IGNORE(ch) = NULL;
    LAST_I3IGNORE(ch) = NULL;
    I3PERM(ch) = I3PERM_MORT;
#endif

    //log_info("Done"); // prool
    return;
}

void i3_freechardata(CHAR_DATA *ch)
{
    I3_IGNORE                              *temp,
                                           *next;
    int                                     x;

    if (IS_NPC(ch))
	return;
#if 0 // prool fool
    if (!CH_I3DATA(ch))
	return;

    I3STRFREE(I3LISTEN(ch));
    I3STRFREE(I3DENY(ch));
    I3STRFREE(I3REPLYNAME(ch));
    I3STRFREE(I3REPLYMUD(ch));

    if (FIRST_I3IGNORE(ch)) {
	for (temp = FIRST_I3IGNORE(ch); temp; temp = next) {
	    next = temp->next;
	    I3STRFREE(temp->name);
	    I3UNLINK(temp, FIRST_I3IGNORE(ch), LAST_I3IGNORE(ch), next, prev);
	    I3DISPOSE(temp);
	}
    }
    for (x = 0; x < MAX_I3TELLHISTORY; x++)
	I3STRFREE(I3TELLHISTORY(ch, x));

    I3DISPOSE(CH_I3DATA(ch));
#endif
    return;
}

void I3_adjust_perms(CHAR_DATA *ch)
{
    if (!this_i3mud)
	return;

    /*
     * Ugly hack to let the permission system adapt freely, but retains the ability to override that adaptation
     * * in the event you need to restrict someone to a lower level, or grant someone a higher level. This of
     * * course comes at the cost of forgetting you may have done so and caused the override flag to be set, but hey.
     * * This isn't a perfect system and never will be. Samson 2-8-04.
     */
#if 0 // prool fool
    if (!I3IS_SET(I3FLAG(ch), I3_PERMOVERRIDE)) {
	if (CH_I3LEVEL(ch) < this_i3mud->minlevel)
	    I3PERM(ch) = I3PERM_NONE;
	else if (CH_I3LEVEL(ch) >= this_i3mud->minlevel
		 && CH_I3LEVEL(ch) < this_i3mud->immlevel)
	    I3PERM(ch) = I3PERM_MORT;
	else if (CH_I3LEVEL(ch) >= this_i3mud->immlevel
		 && CH_I3LEVEL(ch) < this_i3mud->adminlevel)
	    I3PERM(ch) = I3PERM_IMM;
	else if (CH_I3LEVEL(ch) >= this_i3mud->adminlevel
		 && CH_I3LEVEL(ch) < this_i3mud->implevel)
	    I3PERM(ch) = I3PERM_ADMIN;
	else if (CH_I3LEVEL(ch) >= this_i3mud->implevel)
	    I3PERM(ch) = I3PERM_IMP;
    }
#endif
    return;
}

void I3_char_login(CHAR_DATA *ch)
{
    int                                     gender,
                                            sex;
    char                                    buf[MAX_INPUT_LENGTH];

    if (!this_i3mud)
	return;

    I3_adjust_perms(ch);

    if (!i3_is_connected()) {
	if (/*I3PERM(ch) >= I3PERM_IMM && */ i3wait == -2) // prool fool
	    i3_printf(ch, "%%^RED%%^%%^BOLD%%^The Intermud-3 connection is down. Attempts to reconnect were abandoned due to excessive failures.%%^RESET%%^\r\n");
	return;
    }

#if 0 // prool fool
    if (I3PERM(ch) < I3PERM_MORT)
	return;
#endif

    if (this_i3mud->ucache == TRUE) {
	snprintf(buf, MAX_INPUT_LENGTH, "%s@%s", CH_I3NAME(ch), this_i3mud->name);
	gender = I3_get_ucache_gender(buf);
	sex = dikutoi3gender(CH_I3SEX(ch));

	if (gender == sex)
	    return;

	I3_ucache_update(buf, sex);
#if 0 // prool fool
	if (!I3IS_SET(I3FLAG(ch), I3_INVIS))
	    I3_send_ucache_update(CH_I3NAME(ch), sex);
#endif
    }
    return;
}

bool i3_loadchar(CHAR_DATA *ch, FILE * fp, const char *word)
{
    bool                                    fMatch = FALSE;

    if (IS_NPC(ch))
	return FALSE;
#if 0 // prool fool
    if (I3PERM(ch) == I3PERM_NOTSET)
	I3_adjust_perms(ch);
#endif

    switch (UPPER(word[0])) {
	case 'I':
	    //I3KEY("i3perm", I3PERM(ch), i3fread_number(fp)); // prool fool
	    if (!strcasecmp(word, "i3flags")) {
		//I3FLAG(ch) = i3fread_number(fp); // prool fool
		I3_char_login(ch);
		fMatch = TRUE;
		break;
	    }

	    if (!strcasecmp(word, "i3listen")) {
		//I3LISTEN(ch) = i3fread_line(fp); // prool fool
		if (/*I3LISTEN(ch) != NULL && */i3_is_connected()) { // prool fool
		    I3_CHANNEL                             *channel = NULL;
		    const char                             *channels = 0/*I3LISTEN(ch)*/; // prool fool
		    char                                    arg[MAX_INPUT_LENGTH];

		    while (1) {
			if (channels[0] == '\0')
			    break;
			channels = i3one_argument(channels, arg);
#if 0 // prool fool
			if (!(channel = find_I3_channel_by_localname(arg)))
			    I3_unflagchan(&I3LISTEN(ch), arg);
			if (channel && I3PERM(ch) < channel->i3perm)
			    I3_unflagchan(&I3LISTEN(ch), arg);
#endif
		    }
		}
		fMatch = TRUE;
		break;
	    }

	    if (!strcasecmp(word, "i3deny")) {
		//I3DENY(ch) = i3fread_line(fp); // proolfool
		if (/*I3DENY(ch) != NULL && */i3_is_connected()) { // prool fool
		    I3_CHANNEL                             *channel = NULL;
		    const char                             *channels = 0/*I3DENY(ch)*/; // prool fool
		    char                                    arg[MAX_INPUT_LENGTH];

		    while (1) {
			if (channels[0] == '\0')
			    break;
			channels = i3one_argument(channels, arg);
#if 0 // prool fool
			if (!(channel = find_I3_channel_by_localname(arg)))
			    I3_unflagchan(&I3DENY(ch), arg);
			if (channel && I3PERM(ch) < channel->i3perm)
			    I3_unflagchan(&I3DENY(ch), arg);
#endif
		    }
		}
		fMatch = TRUE;
		break;
	    }
	    if (!strcasecmp(word, "i3ignore")) {
		I3_IGNORE                              *temp;

		I3CREATE(temp, I3_IGNORE, 1);

		temp->name = i3fread_line(fp);
		//I3LINK(temp, FIRST_I3IGNORE(ch), LAST_I3IGNORE(ch), next, prev); // prool fool
		fMatch = TRUE;
		break;
	    }
	    break;
    }
    return fMatch;
}

void i3_savechar(CHAR_DATA *ch, FILE * fp)
{
    I3_IGNORE                              *temp;

    if (IS_NPC(ch))
	return;
#if 0 // prool fool
    fprintf(fp, "i3perm       %d\n", I3PERM(ch));
    fprintf(fp, "i3flags      %d\n", I3FLAG(ch));
    if (I3LISTEN(ch) && I3LISTEN(ch)[0] != '\0')
	fprintf(fp, "i3listen     %s\n", I3LISTEN(ch));
    if (I3DENY(ch) && I3DENY(ch)[0] != '\0')
	fprintf(fp, "i3deny       %s\n", I3DENY(ch));
    for (temp = FIRST_I3IGNORE(ch); temp; temp = temp->next)
	fprintf(fp, "i3ignore     %s\n", temp->name);
#endif
    return;
}

/*******************************************
 * Network Startup and Shutdown functions. *
 *******************************************/

void I3_savecolor(void)
{
    FILE                                   *fp;
    I3_COLOR                               *color;

    if ((fp = fopen(I3_COLOR_FILE, "w")) == NULL) {
	i3log("%s", "Couldn't write to I3 color file.");
	return;
    }

    for (color = first_i3_color; color; color = color->next) {
	fprintf(fp, "%s", "#COLOR\n");
	fprintf(fp, "Name   %s\n", color->name);
	fprintf(fp, "Mudtag %s\n", color->mudtag);
	fprintf(fp, "IMCtag %s\n", color->imctag);
	fprintf(fp, "I3tag  %s\n", color->i3tag);
	fprintf(fp, "%s", "End\n\n");
    }
    fprintf(fp, "%s", "#END\n");
    I3FCLOSE(fp);
    return;
}

void I3_readcolor(I3_COLOR *color, FILE * fp)
{
    const char                             *word;
    bool                                    fMatch;

    for (;;) {
	word = feof(fp) ? "End" : i3fread_word(fp);
	fMatch = FALSE;

	switch (word[0]) {
	    case '*':
		fMatch = TRUE;
		i3fread_to_eol(fp);
		break;

	    case 'E':
		if (!strcasecmp(word, "End"))
		    return;
		break;

	    case 'I':
		I3KEY("IMCtag", color->imctag, i3fread_line(fp));
		I3KEY("I3tag", color->i3tag, i3fread_line(fp));
		break;

	    case 'M':
		I3KEY("Mudtag", color->mudtag, i3fread_line(fp));
		break;

	    case 'N':
		I3KEY("Name", color->name, i3fread_line(fp));
		break;
	}
	if (!fMatch)
	    i3bug("i3_readcolor: no match: %s", word);
    }
}

void I3_load_color_table(void)
{
    FILE                                   *fp;
    I3_COLOR                               *color;

    first_i3_color = last_i3_color = NULL;

    i3log("%s", "Loading color table...");

    if (!(fp = fopen(I3_COLOR_FILE, "r"))) {
	i3log("%s", "No color table found.");
	return;
    }

    for (;;) {
	char                                    letter;
	char                                   *word;

	letter = i3fread_letter(fp);
	if (letter == '*') {
	    i3fread_to_eol(fp);
	    continue;
	}

	if (letter != '#') {
	    i3bug("%s", "i3_load_color_table: # not found.");
	    break;
	}

	word = i3fread_word(fp);
	if (!strcasecmp(word, "COLOR")) {
	    I3CREATE(color, I3_COLOR, 1);

	    I3_readcolor(color, fp);
	    I3LINK(color, first_i3_color, last_i3_color, next, prev);
	    continue;
	} else if (!strcasecmp(word, "END"))
	    break;
	else {
	    i3bug("i3_load_color_table: bad section: %s.", word);
	    continue;
	}
    }
    I3FCLOSE(fp);
    return;
}

void I3_savehelps(void)
{
    FILE                                   *fp;
    I3_HELP_DATA                           *help;

    if ((fp = fopen(I3_HELP_FILE, "w")) == NULL) {
	i3log("%s", "Couldn't write to I3 help file.");
	return;
    }

    for (help = first_i3_help; help; help = help->next) {
	fprintf(fp, "%s", "#HELP\n");
	fprintf(fp, "Name %s\n", help->name);
	fprintf(fp, "Perm %s\n", perm_names[help->level]);
	//fprintf(fp, "Text %s¢\n", help->text);
	fprintf(fp, "Text %s%c\n", help->text, '\xA2');
	fprintf(fp, "%s", "End\n\n");
    }
    fprintf(fp, "%s", "#END\n");
    I3FCLOSE(fp);
    return;
}

void I3_readhelp(I3_HELP_DATA *help, FILE * fp)
{
    const char                             *word;
    //char                                    hbuf[MAX_STRING_LENGTH];
    int                                     permvalue;
    bool                                    fMatch;

    for (;;) {
	word = feof(fp) ? "End" : i3fread_word(fp);
	fMatch = FALSE;

	switch (word[0]) {
	    case '*':
		fMatch = TRUE;
		i3fread_to_eol(fp);
		break;

	    case 'E':
		if (!strcasecmp(word, "End"))
		    return;
		break;

	    case 'N':
		I3KEY("Name", help->name, i3fread_line(fp));
		break;

	    case 'P':
		if (!strcasecmp(word, "Perm")) {
		    word = i3fread_word(fp);
		    permvalue = get_permvalue(word);

		    if (permvalue < 0 || permvalue > I3PERM_IMP) {
			i3bug
			    ("i3_readhelp: Command %s loaded with invalid permission. Set to Imp.",
			     help->name);
			help->level = I3PERM_IMP;
		    } else
			help->level = permvalue;
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'T':
		if (!strcasecmp(word, "Text")) {
		    help->text = I3STRALLOC(i3fread_string(fp));
		    fMatch = TRUE;
		    break;
                }
                /*
		if (!strcasecmp(word, "Text")) {
		    int                                     num = 0;

		    //while ((hbuf[num] = fgetc(fp)) != EOF && hbuf[num] != '¢'
		    while ((hbuf[num] = fgetc(fp)) != EOF && (int) hbuf[num] != (int) '\0xA2'
			   && num < (MAX_STRING_LENGTH - 2))
			num++;
		    hbuf[num] = '\0';
		    help->text = I3STRALLOC(hbuf);
		    fMatch = TRUE;
		    break;
		}
		I3KEY("Text", help->text, i3fread_line(fp));
                */
		break;
	}
	if (!fMatch)
	    i3bug("i3_readhelp: no match: %s", word);
    }
}

void I3_load_helps(void)
{
    FILE                                   *fp;
    I3_HELP_DATA                           *help;

    first_i3_help = last_i3_help = NULL;

    i3log("%s", "Loading I3 help file...");

    if (!(fp = fopen(I3_HELP_FILE, "r"))) {
	i3log("%s", "No help file found.");
	return;
    }

    for (;;) {
	char                                    letter;
	char                                   *word;

	letter = i3fread_letter(fp);
	if (letter == '*') {
	    i3fread_to_eol(fp);
	    continue;
	}

	if (letter != '#') {
	    i3bug("%s", "i3_load_helps: # not found.");
	    break;
	}

	word = i3fread_word(fp);
	if (!strcasecmp(word, "HELP")) {
	    I3CREATE(help, I3_HELP_DATA, 1);

	    I3_readhelp(help, fp);
	    I3LINK(help, first_i3_help, last_i3_help, next, prev);
	    continue;
	} else if (!strcasecmp(word, "END"))
	    break;
	else {
	    i3bug("i3_load_helps: bad section: %s.", word);
	    continue;
	}
    }
    I3FCLOSE(fp);
    return;
}

void I3_savecommands(void)
{
    FILE                                   *fp;
    I3_CMD_DATA                            *cmd;
    I3_ALIAS                               *alias;

    if (!(fp = fopen(I3_CMD_FILE, "w"))) {
	i3log("%s", "Couldn't write to I3 command file.");
	return;
    }

    for (cmd = first_i3_command; cmd; cmd = cmd->next) {
	fprintf(fp, "%s", "#COMMAND\n");
	fprintf(fp, "Name      %s\n", cmd->name);
	if (cmd->function != NULL)
	    fprintf(fp, "Code      %s\n", i3_funcname(cmd->function));
	else
	    fprintf(fp, "%s", "Code      NULL\n");
	fprintf(fp, "Perm      %s\n", perm_names[cmd->level]);
	fprintf(fp, "Connected %d\n", cmd->connected);
	for (alias = cmd->first_alias; alias; alias = alias->next)
	    fprintf(fp, "Alias     %s\n", alias->name);
	fprintf(fp, "%s", "End\n\n");
    }
    fprintf(fp, "%s", "#END\n");
    I3FCLOSE(fp);
    return;
}

void I3_readcommand(I3_CMD_DATA *cmd, FILE * fp)
{
    I3_ALIAS                               *alias;
    const char                             *word;
    int                                     permvalue;
    bool                                    fMatch;

    for (;;) {
	word = feof(fp) ? "End" : i3fread_word(fp);
	fMatch = FALSE;

	switch (word[0]) {
	    case '*':
		fMatch = TRUE;
		i3fread_to_eol(fp);
		break;

	    case 'E':
		if (!strcasecmp(word, "End"))
		    return;
		break;

	    case 'A':
		if (!strcasecmp(word, "Alias")) {
		    I3CREATE(alias, I3_ALIAS, 1);

		    alias->name = i3fread_line(fp);
		    I3LINK(alias, cmd->first_alias, cmd->last_alias, next, prev);
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'C':
		I3KEY("Connected", cmd->connected, i3fread_number(fp));
		if (!strcasecmp(word, "Code")) {
		    word = i3fread_word(fp);
		    cmd->function = i3_function(word);
		    if (cmd->function == NULL)
			i3bug
			    ("i3_readcommand: Command %s loaded with invalid function. Set to NULL.",
			     cmd->name);
		    fMatch = TRUE;
		    break;
		}
		break;

	    case 'N':
		I3KEY("Name", cmd->name, i3fread_line(fp));
		break;

	    case 'P':
		if (!strcasecmp(word, "Perm")) {
		    word = i3fread_word(fp);
		    permvalue = get_permvalue(word);

		    if (permvalue < 0 || permvalue > I3PERM_IMP) {
			i3bug
			    ("i3_readcommand: Command %s loaded with invalid permission. Set to Imp.",
			     cmd->name);
			cmd->level = I3PERM_IMP;
		    } else
			cmd->level = permvalue;
		    fMatch = TRUE;
		    break;
		}
		break;
	}
	if (!fMatch)
	    i3bug("i3_readcommand: no match: %s", word);
    }
}

bool I3_load_commands(void)
{
    FILE                                   *fp;
    I3_CMD_DATA                            *cmd;

    first_i3_command = last_i3_command = NULL;

    i3log("%s", "Loading I3 command table...");

    if (!(fp = fopen(I3_CMD_FILE, "r"))) {
	i3log("%s", "No command table found.");
	return FALSE;
    }

    for (;;) {
	char                                    letter;
	char                                   *word;

	letter = i3fread_letter(fp);
	if (letter == '*') {
	    i3fread_to_eol(fp);
	    continue;
	}

	if (letter != '#') {
	    i3bug("%s", "i3_load_commands: # not found.");
	    break;
	}

	word = i3fread_word(fp);
	if (!strcasecmp(word, "COMMAND")) {
	    I3CREATE(cmd, I3_CMD_DATA, 1);

	    I3_readcommand(cmd, fp);
	    I3LINK(cmd, first_i3_command, last_i3_command, next, prev);
	    continue;
	} else if (!strcasecmp(word, "END"))
	    break;
	else {
	    i3bug("i3_load_commands: bad section: %s.", word);
	    continue;
	}
    }
    I3FCLOSE(fp);
    return TRUE;
}

void I3_saverouters(void)
{
    FILE                                   *fp;
    ROUTER_DATA                            *router;

    if (!(fp = fopen(I3_ROUTER_FILE, "w"))) {
	i3log("%s", "Couldn't write to I3 router file.");
	return;
    }

    for (router = first_router; router; router = router->next) {
	fprintf(fp, "%s", "#ROUTER\n");
	fprintf(fp, "Name %s\n", router->name);
	fprintf(fp, "IP   %s\n", router->ip);
	fprintf(fp, "Port %d\n", router->port);
	fprintf(fp, "%s", "End\n\n");
    }
    fprintf(fp, "%s", "#END\n");
    I3FCLOSE(fp);
    return;
}

void I3_readrouter(ROUTER_DATA *router, FILE * fp)
{
    const char                             *word;
    bool                                    fMatch;

    for (;;) {
	word = feof(fp) ? "End" : i3fread_word(fp);
	fMatch = FALSE;

	switch (word[0]) {
	    case '*':
		fMatch = TRUE;
		i3fread_to_eol(fp);
		break;

	    case 'E':
		if (!strcasecmp(word, "End"))
		    return;
		break;

	    case 'I':
		I3KEY("IP", router->ip, i3fread_line(fp));
		break;

	    case 'N':
		I3KEY("Name", router->name, i3fread_line(fp));
		break;

	    case 'P':
		I3KEY("Port", router->port, i3fread_number(fp));
		break;
	}
	if (!fMatch)
	    i3bug("i3_readrouter: no match: %s", word);
    }
}

bool I3_load_routers(void)
{
    FILE                                   *fp;
    ROUTER_DATA                            *router;

    first_router = last_router = NULL;

    i3log("%s", "Loading I3 router data...");

    if (!(fp = fopen(I3_ROUTER_FILE, "r"))) {
	i3log("%s", "No router data found.");
	return FALSE;
    }

    for (;;) {
	char                                    letter;
	char                                   *word;

	letter = i3fread_letter(fp);
	if (letter == '*') {
	    i3fread_to_eol(fp);
	    continue;
	}

	if (letter != '#') {
	    i3bug("%s", "i3_load_routers: # not found.");
	    break;
	}

	word = i3fread_word(fp);
	if (!strcasecmp(word, "ROUTER")) {
	    I3CREATE(router, ROUTER_DATA, 1);

	    I3_readrouter(router, fp);
	    if (!router->name || router->name[0] == '\0' || !router->ip || router->ip[0] == '\0'
		|| router->port <= 0) {
		I3STRFREE(router->name);
		I3STRFREE(router->ip);
		I3DISPOSE(router);
	    } else
		I3LINK(router, first_router, last_router, next, prev);
	    continue;
	} else if (!strcasecmp(word, "END"))
	    break;
	else {
	    i3bug("i3_load_routers: bad section: %s.", word);
	    continue;
	}
    }
    I3FCLOSE(fp);

    if (!first_router || !last_router)
	return FALSE;

    return TRUE;
}

ROUTER_DATA                            *i3_find_router(const char *name)
{
    ROUTER_DATA                            *router;

    for (router = first_router; router; router = router->next) {
	if (!strcasecmp(router->name, name))
	    return router;
    }
    return NULL;
}

void I3_readucache(UCACHE_DATA *user, FILE * fp)
{
    const char                             *word;
    bool                                    fMatch;

    for (;;) {
	word = feof(fp) ? "End" : i3fread_word(fp);
	fMatch = FALSE;

	switch (UPPER(word[0])) {
	    case '*':
		fMatch = TRUE;
		i3fread_to_eol(fp);
		break;

	    case 'N':
		I3KEY("Name", user->name, i3fread_line(fp));
		break;

	    case 'S':
		I3KEY("Sex", user->gender, i3fread_number(fp));
		break;

	    case 'T':
		I3KEY("Time", user->time, i3fread_number(fp));
		break;

	    case 'E':
		if (!strcasecmp(word, "End"))
		    return;
		break;
	}
	if (!fMatch)
	    i3bug("I3_readucache: no match: %s", word);
    }
}

void I3_load_ucache(void)
{
    FILE                                   *fp;
    UCACHE_DATA                            *user;

    first_ucache = last_ucache = NULL;

    i3log("%s", "Loading ucache data...");

    if (!(fp = fopen(I3_UCACHE_FILE, "r"))) {
	i3log("%s", "No ucache data found.");
	return;
    }

    for (;;) {
	char                                    letter;
	char                                   *word;

	letter = i3fread_letter(fp);
	if (letter == '*') {
	    i3fread_to_eol(fp);
	    continue;
	}

	if (letter != '#') {
	    i3bug("%s", "I3_load_ucahe: # not found.");
	    break;
	}

	word = i3fread_word(fp);
	if (!strcasecmp(word, "UCACHE")) {
	    I3CREATE(user, UCACHE_DATA, 1);

	    I3_readucache(user, fp);
	    I3LINK(user, first_ucache, last_ucache, next, prev);
	    continue;
	} else if (!strcasecmp(word, "END"))
	    break;
	else {
	    i3bug("I3_load_ucache: bad section: %s.", word);
	    continue;
	}
    }
    I3FCLOSE(fp);
    return;
}

void I3_saveconfig(void)
{
    FILE                                   *fp;

    if (!(fp = fopen(I3_CONFIG_FILE, "w"))) {
	i3log("%s", "Couldn't write to i3.config file.");
	return;
    }

    fprintf(fp, "%s", "$I3CONFIG\n\n");
    fprintf(fp, "%s", "# This file will now allow you to use tildes.\n");
    fprintf(fp, "%s", "# Set autoconnect to 1 to automatically connect at bootup.\n");
    fprintf(fp, "%s", "# This information can be edited online using 'i3config'\n");
    fprintf(fp, "thismud      %s\n", this_i3mud->name);
    fprintf(fp, "autoconnect  %d\n", this_i3mud->autoconnect);
    fprintf(fp, "telnet       %s\n", this_i3mud->telnet);
    fprintf(fp, "web          %s\n", this_i3mud->web);
    fprintf(fp, "adminemail   %s\n", this_i3mud->admin_email);
    fprintf(fp, "openstatus   %s\n", this_i3mud->open_status);
    if (this_i3mud->mud_type)
	fprintf(fp, "mudtype      %s\n", this_i3mud->mud_type);
    if (this_i3mud->mudlib && strcasecmp(this_i3mud->mudlib, this_i3mud->base_mudlib))
	fprintf(fp, "mudlib       %s\n", this_i3mud->mudlib);
    fprintf(fp, "minlevel     %d\n", this_i3mud->minlevel);
    fprintf(fp, "immlevel     %d\n", this_i3mud->immlevel);
    fprintf(fp, "adminlevel   %d\n", this_i3mud->adminlevel);
    fprintf(fp, "implevel     %d\n\n", this_i3mud->implevel);

    fprintf(fp, "%s", "\n# Information below this point cannot be edited online.\n");
    fprintf(fp, "%s", "# The services provided by your mud.\n");
    fprintf(fp, "%s",
	    "# Do not turn things on unless you KNOW your mud properly supports them!\n");
    fprintf(fp, "%s",
	    "# Refer to http://cie.imaginary.com/protocols/intermud3.html for public packet specifications.\n");
    fprintf(fp, "tell         %d\n", this_i3mud->tell);
    fprintf(fp, "beep         %d\n", this_i3mud->beep);
    fprintf(fp, "emoteto      %d\n", this_i3mud->emoteto);
    fprintf(fp, "who          %d\n", this_i3mud->who);
    fprintf(fp, "finger       %d\n", this_i3mud->finger);
    fprintf(fp, "locate       %d\n", this_i3mud->locate);
    fprintf(fp, "channel      %d\n", this_i3mud->channel);
    fprintf(fp, "news         %d\n", this_i3mud->news);
    fprintf(fp, "mail         %d\n", this_i3mud->mail);
    fprintf(fp, "file         %d\n", this_i3mud->file);
    fprintf(fp, "auth         %d\n", this_i3mud->auth);
    fprintf(fp, "ucache       %d\n\n", this_i3mud->ucache);
    fprintf(fp, "%s",
	    "# Port numbers for OOB services. Leave as 0 if your mud does not support these.\n");
    fprintf(fp, "smtp         %d\n", this_i3mud->smtp);
    fprintf(fp, "ftp          %d\n", this_i3mud->ftp);
    fprintf(fp, "nntp         %d\n", this_i3mud->nntp);
    fprintf(fp, "http         %d\n", this_i3mud->http);
    fprintf(fp, "pop3         %d\n", this_i3mud->pop3);
    fprintf(fp, "rcp          %d\n", this_i3mud->rcp);
    fprintf(fp, "amrcp        %d\n", this_i3mud->amrcp);
    fprintf(fp, "%s", "end\n");
    fprintf(fp, "%s", "$END\n");
    I3FCLOSE(fp);
    return;
}

void I3_fread_config_file(FILE * fin)
{
    const char                             *word;
    bool                                    fMatch;

    for (;;) {
	word = feof(fin) ? "end" : i3fread_word(fin);
	fMatch = FALSE;

	switch (word[0]) {
	    case '#':
		fMatch = TRUE;
		i3fread_to_eol(fin);
		break;

	    case 'a':
		I3KEY("adminemail", this_i3mud->admin_email, i3fread_line(fin));
		I3KEY("adminlevel", this_i3mud->adminlevel, i3fread_number(fin));
		I3KEY("amrcp", this_i3mud->amrcp, i3fread_number(fin));
		I3KEY("auth", this_i3mud->auth, i3fread_number(fin));
		I3KEY("autoconnect", this_i3mud->autoconnect, i3fread_number(fin));
		break;

	    case 'b':
		I3KEY("beep", this_i3mud->beep, i3fread_number(fin));
		break;

	    case 'c':
		I3KEY("channel", this_i3mud->channel, i3fread_number(fin));
		break;

	    case 'e':
		I3KEY("emoteto", this_i3mud->emoteto, i3fread_number(fin));
		if (!strcasecmp(word, "end")) {
		    char                                    lib_buf[MAX_STRING_LENGTH];

		    /*
		     * Adjust base_mudlib information based on already supplied info (mud.h). -Orion
		     * Modified for AFKMud use by Samson.
		     */
		    I3STRFREE(this_i3mud->mud_type);
		    this_i3mud->mud_type = I3STRALLOC(CODETYPE);

		    //snprintf(lib_buf, MAX_STRING_LENGTH, "%s %s", CODEBASE, CODEVERSION); // prool fool
		    snprintf(lib_buf, MAX_STRING_LENGTH, "CircleRussian"); // prool fool
		    I3STRFREE(this_i3mud->base_mudlib);
		    this_i3mud->base_mudlib = I3STRALLOC(lib_buf);

		    if (!this_i3mud->mudlib
			|| strcasecmp(this_i3mud->mudlib, this_i3mud->base_mudlib)) {
			if (this_i3mud->mudlib)
			    I3STRFREE(this_i3mud->mudlib);
			this_i3mud->mudlib = I3STRALLOC(lib_buf);
		    }

		    I3STRFREE(this_i3mud->driver);
		    this_i3mud->driver = I3STRALLOC(I3DRIVER);

		    /*
		     * Convert to new router file 
		     */
		    if (first_router) {
			I3_saverouters();
			I3_saveconfig();
		    }
		    return;
		}
		break;

	    case 'f':
		I3KEY("file", this_i3mud->file, i3fread_number(fin));
		I3KEY("finger", this_i3mud->finger, i3fread_number(fin));
		I3KEY("ftp", this_i3mud->ftp, i3fread_number(fin));
		break;

	    case 'h':
		I3KEY("http", this_i3mud->http, i3fread_number(fin));
		break;

	    case 'i':
		I3KEY("immlevel", this_i3mud->immlevel, i3fread_number(fin));
		I3KEY("implevel", this_i3mud->implevel, i3fread_number(fin));
		break;

	    case 'l':
		I3KEY("locate", this_i3mud->locate, i3fread_number(fin));
		break;

	    case 'm':
		I3KEY("mail", this_i3mud->mail, i3fread_number(fin));
		I3KEY("minlevel", this_i3mud->minlevel, i3fread_number(fin));
		I3KEY("mudlib", this_i3mud->mudlib, i3fread_line(fin));
		I3KEY("mudtype", this_i3mud->mud_type, i3fread_line(fin));
		break;

	    case 'n':
		I3KEY("news", this_i3mud->news, i3fread_number(fin));
		I3KEY("nntp", this_i3mud->nntp, i3fread_number(fin));
		break;

	    case 'o':
		I3KEY("openstatus", this_i3mud->open_status, i3fread_line(fin));
		break;

	    case 'p':
		I3KEY("pop3", this_i3mud->pop3, i3fread_number(fin));
		break;

	    case 'r':
		I3KEY("rcp", this_i3mud->rcp, i3fread_number(fin));
		/*
		 * Router config loading is legacy support here - routers are configured in their own file now. 
		 */
		if (!strcasecmp(word, "router")) {
		    ROUTER_DATA                            *router;
		    char                                    rname[MAX_INPUT_LENGTH],
		                                            rip[MAX_INPUT_LENGTH],
		                                           *ln;
		    int                                     rport;

		    ln = i3fread_line(fin);
		    sscanf(ln, "%s %s %d", rname, rip, &rport);

		    I3CREATE(router, ROUTER_DATA, 1);

		    router->name = I3STRALLOC(rname);
		    router->ip = I3STRALLOC(rip);
		    router->port = rport;
		    router->reconattempts = 0;
		    I3LINK(router, first_router, last_router, next, prev);
		    fMatch = TRUE;
		    I3DISPOSE(ln);
		    break;
		}
		break;

	    case 's':
		I3KEY("smtp", this_i3mud->smtp, i3fread_number(fin));
		break;

	    case 't':
		I3KEY("tell", this_i3mud->tell, i3fread_number(fin));
		I3KEY("telnet", this_i3mud->telnet, i3fread_line(fin));
		I3KEY("thismud", this_i3mud->name, i3fread_line(fin));
		break;

	    case 'u':
		I3KEY("ucache", this_i3mud->ucache, i3fread_number(fin));
		break;

	    case 'w':
		I3KEY("web", this_i3mud->web, i3fread_line(fin));
		I3KEY("who", this_i3mud->who, i3fread_number(fin));
		break;
	}
	if (!fMatch)
	    i3bug("I3_fread_config_file: Bad keyword: %s\r\n", word);
    }
}

bool I3_read_config(int mudport)
{
    FILE                                   *fin,
                                           *fp;

    if (this_i3mud != NULL)
	destroy_I3_mud(this_i3mud);
    this_i3mud = NULL;

    i3log("%s", "Loading Intermud-3 network data...");

    if (!(fin = fopen(I3_CONFIG_FILE, "r"))) {
	i3log("%s", "Can't open configuration file: i3.config");
	i3log("%s", "Network configuration aborted.");
	return FALSE;
    }

    for (;;) {
	char                                    letter;
	char                                   *word;

	letter = i3fread_letter(fin);

	if (letter == '#') {
	    i3fread_to_eol(fin);
	    continue;
	}

	if (letter != '$') {
	    i3bug("%s", "I3_read_config: $ not found");
	    break;
	}

	word = i3fread_word(fin);
	if (!strcasecmp(word, "I3CONFIG") && this_i3mud == NULL) {
            this_i3mud = create_I3_mud();
	    this_i3mud->player_port = mudport;		       /* Passed in from the mud's startup script */
	    I3_fread_config_file(fin);
	    continue;
	} else if (!strcasecmp(word, "END"))
	    break;
	else {
	    i3bug("I3_read_config: Bad section in config file: %s", word);
	    continue;
	}
    }
    I3FCLOSE(fin);

    if (!this_i3mud) {
	i3log("%s", "Error during configuration load.");
	i3log("%s", "Network configuration aborted.");
	return FALSE;
    }

    if ((fp = fopen(I3_PASSWORD_FILE, "r")) != NULL && this_i3mud != NULL) {
	char                                   *word;

	word = i3fread_word(fp);

	if (!strcasecmp(word, "#PASSWORD")) {
	    char                                   *ln = i3fread_line(fp);
	    int                                     pass,
	                                            mud,
	                                            chan;

	    pass = mud = chan = 0;
	    sscanf(ln, "%d %d %d", &pass, &mud, &chan);
	    this_i3mud->password = pass;
	    this_i3mud->mudlist_id = mud;
	    this_i3mud->chanlist_id = chan;
	    I3DISPOSE(ln);
	}
	I3FCLOSE(fp);
    }

    if (!this_i3mud->name || this_i3mud->name[0] == '\0') {
	i3log("%s", "Mud name not loaded in configuration file.");
	i3log("%s", "Network configuration aborted.");
	destroy_I3_mud(this_i3mud);
	return FALSE;
    }

    if (!this_i3mud->telnet || this_i3mud->telnet[0] == '\0')
	this_i3mud->telnet = I3STRALLOC("Address not configured");

    if (!this_i3mud->web || this_i3mud->web[0] == '\0')
	this_i3mud->web = I3STRALLOC("Address not configured");

    // I3_THISMUD = this_i3mud->name;
    return TRUE;
}

void I3_readban(I3_BAN *ban, FILE * fin)
{
    const char                             *word;
    bool                                    fMatch;

    for (;;) {
	word = feof(fin) ? "End" : i3fread_word(fin);
	fMatch = FALSE;

	switch (UPPER(word[0])) {
	    case '*':
		fMatch = TRUE;
		i3fread_to_eol(fin);
		break;

	    case 'N':
		I3KEY("Name", ban->name, i3fread_line(fin));
		break;

	    case 'E':
		if (!strcasecmp(word, "End"))
		    return;
		break;
	}
	if (!fMatch)
	    i3bug("I3_readban: no match: %s", word);
    }
}

void I3_loadbans(void)
{
    FILE                                   *fin;
    I3_BAN                                 *ban;

    first_i3ban = NULL;
    last_i3ban = NULL;

    i3log("%s", "Loading ban list...");

    if ((fin = fopen(I3_BAN_FILE, "r")) == NULL) {
	i3log("%s", "No ban list defined.");
	return;
    }

    for (;;) {
	char                                    letter;
	char                                   *word;

	letter = i3fread_letter(fin);
	if (letter == '*') {
	    i3fread_to_eol(fin);
	    continue;
	}

	if (letter != '#') {
	    i3bug("%s", "I3_loadbans: # not found.");
	    break;
	}

	word = i3fread_word(fin);
	if (!strcasecmp(word, "I3BAN")) {
	    I3CREATE(ban, I3_BAN, 1);

	    I3_readban(ban, fin);
	    if (!ban->name)
		I3DISPOSE(ban);
	    else
		I3LINK(ban, first_i3ban, last_i3ban, next, prev);
	    continue;
	} else if (!strcasecmp(word, "END"))
	    break;
	else {
	    i3bug("I3_loadbans: bad section: %s.", word);
	    continue;
	}
    }
    I3FCLOSE(fin);
    return;
}

void I3_write_bans(void)
{
    FILE                                   *fout;
    I3_BAN                                 *ban;

    if ((fout = fopen(I3_BAN_FILE, "w")) == NULL) {
	i3log("%s", "Couldn't write to ban list file.");
	return;
    }

    for (ban = first_i3ban; ban; ban = ban->next) {
	fprintf(fout, "%s", "#I3BAN\n");
	fprintf(fout, "Name   %s\n", ban->name);
	fprintf(fout, "%s", "End\n\n");
    }
    fprintf(fout, "%s", "#END\n");
    I3FCLOSE(fout);
}

void I3_readchannel(I3_CHANNEL *channel, FILE * fin)
{
    const char                             *word;
    bool                                    fMatch;

    for (;;) {
	word = feof(fin) ? "End" : i3fread_word(fin);
	fMatch = FALSE;

	switch (UPPER(word[0])) {
	    case '*':
		fMatch = TRUE;
		i3fread_to_eol(fin);
		break;

	    case 'C':
		I3KEY("ChanName", channel->I3_name, i3fread_line(fin));
		I3KEY("ChanMud", channel->host_mud, i3fread_line(fin));
		I3KEY("ChanLocal", channel->local_name, i3fread_line(fin));
		I3KEY("ChanLayM", channel->layout_m, i3fread_line(fin));
		I3KEY("ChanLayE", channel->layout_e, i3fread_line(fin));
		I3KEY("ChanLevel", channel->i3perm, i3fread_number(fin));
		I3KEY("ChanStatus", channel->status, i3fread_number(fin));
		I3KEY("ChanFlags", channel->flags, i3fread_number(fin));
		break;

	    case 'E':
		if (!strcasecmp(word, "End")) {
		    /*
		     * Legacy support to convert channel permissions 
		     */
		    if (channel->i3perm > I3PERM_IMP) {
			/*
			 * The I3PERM_NONE condition should realistically never happen.... 
			 */
			if (channel->i3perm < this_i3mud->minlevel)
			    channel->i3perm = I3PERM_NONE;
			else if (channel->i3perm >= this_i3mud->minlevel
				 && channel->i3perm < this_i3mud->immlevel)
			    channel->i3perm = I3PERM_MORT;
			else if (channel->i3perm >= this_i3mud->immlevel
				 && channel->i3perm < this_i3mud->adminlevel)
			    channel->i3perm = I3PERM_IMM;
			else if (channel->i3perm >= this_i3mud->adminlevel
				 && channel->i3perm < this_i3mud->implevel)
			    channel->i3perm = I3PERM_ADMIN;
			else if (channel->i3perm >= this_i3mud->implevel)
			    channel->i3perm = I3PERM_IMP;
		    }
		    return;
		}
		break;
	}
	if (!fMatch)
	    i3bug("I3_readchannel: no match: %s", word);
    }
}

void I3_loadchannels(void)
{
    FILE                                   *fin;
    I3_CHANNEL                             *channel;

    first_I3chan = last_I3chan = NULL;

    i3log("%s", "Loading channels...");

    if (!(fin = fopen(I3_CHANNEL_FILE, "r"))) {
	i3log("%s", "No channel config file found.");
	return;
    }

    for (;;) {
	char                                    letter;
	char                                   *word;

	letter = i3fread_letter(fin);
	if (letter == '*') {
	    i3fread_to_eol(fin);
	    continue;
	}

	if (letter != '#') {
	    i3bug("%s", "I3_loadchannels: # not found.");
	    break;
	}

	word = i3fread_word(fin);
	if (!strcasecmp(word, "I3CHAN")) {
	    int                                     x;

	    I3CREATE(channel, I3_CHANNEL, 1);

	    I3_readchannel(channel, fin);

	    if (channel->local_name && !strcmp(channel->local_name, "(null)")) {
		I3STRFREE(channel->local_name);
		channel->local_name = NULL;
	    }
	    if (channel->layout_m && !strcmp(channel->layout_m, "(null)")) {
		I3STRFREE(channel->layout_m);
		channel->layout_m = I3STRALLOC("%%^RED%%^%%^BOLD%%^[%%^WHITE%%^%%^BOLD%%^%s%%^RED%%^%%^BOLD%%^] %%^CYAN%%^%%^BOLD%%^%s@%s: %%^RESET%%^%s");
	    }
	    if (channel->layout_e && !strcmp(channel->layout_e, "(null)")) {
		I3STRFREE(channel->layout_e);
		channel->layout_e = I3STRALLOC("%%^RED%%^%%^BOLD%%^[%%^WHITE%%^%%^BOLD%%^%s%%^RED%%^%%^BOLD%%^] %%^RESET%%^%s");
	    }
	    for (x = 0; x < MAX_I3HISTORY; x++)
		channel->history[x] = NULL;
	    I3LINK(channel, first_I3chan, last_I3chan, next, prev);
	    continue;
	} else if (!strcasecmp(word, "END"))
	    break;
	else {
	    i3bug("I3_loadchannels: bad section: %s.", word);
	    continue;
	}
    }
    I3FCLOSE(fin);
    return;
}

void I3_write_channel_config(void)
{
    FILE                                   *fout;
    I3_CHANNEL                             *channel;

    if ((fout = fopen(I3_CHANNEL_FILE, "w")) == NULL) {
	i3log("%s", "Couldn't write to channel config file.");
	return;
    }

    for (channel = first_I3chan; channel; channel = channel->next) {
	// if( channel->local_name )
	// {
	fprintf(fout, "%s", "#I3CHAN\n");
	fprintf(fout, "ChanName   %s\n", channel->I3_name);
	fprintf(fout, "ChanMud    %s\n", channel->host_mud);
	fprintf(fout, "ChanLocal  %s\n", channel->local_name);
	fprintf(fout, "ChanLayM   %s\n", channel->layout_m);
	fprintf(fout, "ChanLayE   %s\n", channel->layout_e);
	fprintf(fout, "ChanLevel  %d\n", channel->i3perm);
	fprintf(fout, "ChanStatus %d\n", channel->status);
	fprintf(fout, "ChanFlags  %ld\n", (long int)channel->flags);
	fprintf(fout, "%s", "End\n\n");
	// }
    }
    fprintf(fout, "%s", "#END\n");
    I3FCLOSE(fout);
}

/* Used only during copyovers */
void fread_mudlist(FILE * fin, I3_MUD *mud)
{
    const char                             *word;
    char                                   *ln;
    bool                                    fMatch;
    int                                     x1,
                                            x2,
                                            x3,
                                            x4,
                                            x5,
                                            x6,
                                            x7,
                                            x8,
                                            x9,
                                            x10,
                                            x11,
                                            x12;

    for (;;) {
	word = feof(fin) ? "End" : i3fread_word(fin);
	fMatch = FALSE;

	switch (UPPER(word[0])) {
	    case '*':
		fMatch = TRUE;
		i3fread_to_eol(fin);
		break;

	    case 'B':
		I3KEY("Banner", mud->banner, i3fread_line(fin));
		I3KEY("Baselib", mud->base_mudlib, i3fread_line(fin));
		break;

	    case 'D':
		I3KEY("Daemon", mud->daemon, i3fread_line(fin));
		I3KEY("Driver", mud->driver, i3fread_line(fin));
		break;

	    case 'E':
		I3KEY("Email", mud->admin_email, i3fread_line(fin));
		if (!strcasecmp(word, "End")) {
		    return;
		}

	    case 'I':
		I3KEY("IP", mud->ipaddress, i3fread_line(fin));
		break;

	    case 'M':
		I3KEY("Mudlib", mud->mudlib, i3fread_line(fin));
		break;

	    case 'O':
		I3KEY("Openstatus", mud->open_status, i3fread_line(fin));
		if (!strcasecmp(word, "OOBPorts")) {
		    ln = i3fread_line(fin);
		    x1 = x2 = x3 = x4 = x5 = x6 = x7 = 0;

		    sscanf(ln, "%d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6, &x7);
		    mud->smtp = x1;
		    mud->ftp = x2;
		    mud->nntp = x3;
		    mud->http = x4;
		    mud->pop3 = x5;
		    mud->rcp = x6;
		    mud->amrcp = x7;
		    fMatch = TRUE;
		    I3DISPOSE(ln);
		    break;
		}
		break;

	    case 'P':
		if (!strcasecmp(word, "Ports")) {
		    ln = i3fread_line(fin);
		    x1 = x2 = x3 = 0;

		    sscanf(ln, "%d %d %d ", &x1, &x2, &x3);
		    mud->player_port = x1;
		    mud->imud_tcp_port = x2;
		    mud->imud_udp_port = x3;
		    fMatch = TRUE;
		    I3DISPOSE(ln);
		    break;
		}
		break;

	    case 'S':
		I3KEY("Status", mud->status, i3fread_number(fin));
		if (!strcasecmp(word, "Services")) {
		    ln = i3fread_line(fin);
		    x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 = x9 = x10 = x11 = x12 = 0;

		    sscanf(ln, "%d %d %d %d %d %d %d %d %d %d %d %d",
			   &x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8, &x9, &x10, &x11, &x12);
		    mud->tell = x1;
		    mud->beep = x2;
		    mud->emoteto = x3;
		    mud->who = x4;
		    mud->finger = x5;
		    mud->locate = x6;
		    mud->channel = x7;
		    mud->news = x8;
		    mud->mail = x9;
		    mud->file = x10;
		    mud->auth = x11;
		    mud->ucache = x12;
		    fMatch = TRUE;
		    I3DISPOSE(ln);
		    break;
		}
		break;

	    case 'T':
		I3KEY("Telnet", mud->telnet, i3fread_line(fin));
		I3KEY("Time", mud->time, i3fread_line(fin));
		I3KEY("Type", mud->mud_type, i3fread_line(fin));
		break;

	    case 'W':
		I3KEY("Web", mud->web, i3fread_line(fin));
		break;
	}

	if (!fMatch)
	    i3bug("I3_readmudlist: no match: %s", word);
    }
}

/* Called only during copyovers */
void I3_loadmudlist(void)
{
    FILE                                   *fin;
    I3_MUD                                 *mud;

    if (!(fin = fopen(I3_MUDLIST_FILE, "r")))
	return;

    for (;;) {
	char                                    letter;
	const char                             *word;

	letter = i3fread_letter(fin);
	if (letter == '*') {
	    i3fread_to_eol(fin);
	    continue;
	}

	if (letter != '#') {
	    i3bug("%s", "I3_loadmudlist: # not found.");
	    break;
	}

	word = i3fread_word(fin);
	if (!strcasecmp(word, "ROUTER")) {
	    I3STRFREE(this_i3mud->routerName);
	    this_i3mud->routerName = i3fread_line(fin);
	    I3_ROUTER_NAME = this_i3mud->routerName;
	    continue;
	}
	if (!strcasecmp(word, "MUDLIST")) {
	    word = i3fread_word(fin);
	    if (!strcasecmp(word, "Name")) {
		char                                   *tmpname;

		tmpname = i3fread_line(fin);
		mud = new_I3_mud(tmpname);
		fread_mudlist(fin, mud);
		I3STRFREE(tmpname);
	    } else {
		i3bug("%s", "fread_mudlist: No mudname saved, skipping entry.");
		i3fread_to_eol(fin);
		for (;;) {
		    word = feof(fin) ? "End" : i3fread_word(fin);
		    if (strcasecmp(word, "End"))
			i3fread_to_eol(fin);
		    else
			break;
		}
	    }
	    continue;
	} else if (!strcasecmp(word, "END"))
	    break;
	else {
	    i3bug("I3_loadmudlist: bad section: %s.", word);
	    continue;
	}
    }
    I3FCLOSE(fin);
    unlink(I3_MUDLIST_FILE);
    return;
}

/* Called only during copyovers */
void I3_loadchanlist(void)
{
    FILE                                   *fin;
    I3_CHANNEL                             *channel;

    if (!(fin = fopen(I3_CHANLIST_FILE, "r")))
	return;

    for (;;) {
	char                                    letter;
	char                                   *word;

	letter = i3fread_letter(fin);
	if (letter == '*') {
	    i3fread_to_eol(fin);
	    continue;
	}

	if (letter != '#') {
	    i3bug("%s", "I3_loadchanlist: # not found.");
	    break;
	}

	word = i3fread_word(fin);
	if (!strcasecmp(word, "I3CHAN")) {
	    int                                     x;
	    I3CREATE(channel, I3_CHANNEL, 1);

	    I3_readchannel(channel, fin);

	    for (x = 0; x < MAX_I3HISTORY; x++)
		channel->history[x] = NULL;
	    I3LINK(channel, first_I3chan, last_I3chan, next, prev);
	    continue;
	} else if (!strcasecmp(word, "END"))
	    break;
	else {
	    i3bug("I3_loadchanlist: bad section: %s.", word);
	    continue;
	}
    }
    I3FCLOSE(fin);
    unlink(I3_CHANLIST_FILE);
    return;
}

/* Called only during copyovers */
void I3_savemudlist(void)
{
    FILE                                   *fp;
    I3_MUD                                 *mud;

    if (!(fp = fopen(I3_MUDLIST_FILE, "w"))) {
	i3bug("%s", "I3_savemudlist: Unable to write to mudlist file.");
	return;
    }

    fprintf(fp, "#ROUTER %s\n", I3_ROUTER_NAME);
    for (mud = first_mud; mud; mud = mud->next) {
	/*
	 * Don't store muds that are down, who cares? They'll update themselves anyway 
	 */
	if (mud->status == 0)
	    continue;

	fprintf(fp, "%s", "#MUDLIST\n");
	fprintf(fp, "Name		%s\n", mud->name);
	fprintf(fp, "Status		%d\n", mud->status);
	fprintf(fp, "IP			%s\n", mud->ipaddress);
	fprintf(fp, "Mudlib		%s\n", mud->mudlib);
	fprintf(fp, "Baselib		%s\n", mud->base_mudlib);
	fprintf(fp, "Driver		%s\n", mud->driver);
	fprintf(fp, "Type		%s\n", mud->mud_type);
	fprintf(fp, "Openstatus	%s\n", mud->open_status);
	fprintf(fp, "Email		%s\n", mud->admin_email);
	if (mud->telnet)
	    fprintf(fp, "Telnet		%s\n", mud->telnet);
	if (mud->web)
	    fprintf(fp, "Web		%s\n", mud->web);
	if (mud->banner)
	    fprintf(fp, "Banner		%s\n", mud->banner);
	if (mud->daemon)
	    fprintf(fp, "Dameon		%s\n", mud->daemon);
	if (mud->time)
	    fprintf(fp, "Time		%s\n", mud->time);
	fprintf(fp, "Ports %d %d %d\n", mud->player_port, mud->imud_tcp_port,
		mud->imud_udp_port);
	fprintf(fp, "Services %d %d %d %d %d %d %d %d %d %d %d %d\n", mud->tell, mud->beep,
		mud->emoteto, mud->who, mud->finger, mud->locate, mud->channel, mud->news,
		mud->mail, mud->file, mud->auth, mud->ucache);
	fprintf(fp, "OOBports %d %d %d %d %d %d %d\n", mud->smtp, mud->ftp, mud->nntp,
		mud->http, mud->pop3, mud->rcp, mud->amrcp);
	fprintf(fp, "%s", "End\n\n");
    }
    fprintf(fp, "%s", "#END\n");
    I3FCLOSE(fp);
    return;
}

/* Called only during copyovers */
void I3_savechanlist(void)
{
    FILE                                   *fp;
    I3_CHANNEL                             *channel;

    if (!(fp = fopen(I3_CHANLIST_FILE, "w"))) {
	i3bug("%s", "I3_savechanlist: Unable to write to chanlist file.");
	return;
    }

    for (channel = first_I3chan; channel; channel = channel->next) {
	/*
	 * Don't save local channels, they are stored elsewhere 
	 */
	if (channel->local_name)
	    continue;

	fprintf(fp, "%s", "#I3CHAN\n");
	fprintf(fp, "ChanMud		%s\n", channel->host_mud);
	fprintf(fp, "ChanName		%s\n", channel->I3_name);
	fprintf(fp, "ChanStatus	%d\n", channel->status);
	fprintf(fp, "%s", "End\n\n");
    }
    fprintf(fp, "%s", "#END\n");
    I3FCLOSE(fp);
    return;
}

/* Used during copyovers */
void I3_loadhistory(void)
{
    char                                    filename[MAX_INPUT_LENGTH];
    FILE                                   *tempfile;
    I3_CHANNEL                             *tempchan = NULL;
    int                                     x;

    for (tempchan = first_I3chan; tempchan; tempchan = tempchan->next) {
	if (!tempchan->local_name)
	    continue;

	snprintf(filename, MAX_INPUT_LENGTH, "%s%s.hist", I3_DIR, tempchan->local_name);

	if (!(tempfile = fopen(filename, "r")))
	    continue;

	for (x = 0; x < MAX_I3HISTORY; x++) {
	    if (feof(tempfile))
		tempchan->history[x] = NULL;
	    else
		tempchan->history[x] = i3fread_line(tempfile);
	}
	I3FCLOSE(tempfile);
	unlink(filename);
    }
}

/* Used during copyovers */
void I3_savehistory(void)
{
    char                                    filename[MAX_INPUT_LENGTH];
    FILE                                   *tempfile;
    I3_CHANNEL                             *tempchan = NULL;
    int                                     x;

    for (tempchan = first_I3chan; tempchan; tempchan = tempchan->next) {
	if (!tempchan->local_name)
	    continue;

	if (!tempchan->history[0])
	    continue;

	snprintf(filename, MAX_INPUT_LENGTH, "%s%s.hist", I3_DIR, tempchan->local_name);

	if (!(tempfile = fopen(filename, "w")))
	    continue;

	for (x = 0; x < MAX_I3HISTORY; x++) {
	    if (tempchan->history[x] != NULL)
		fprintf(tempfile, "%s\n", tempchan->history[x]);
	}
	I3FCLOSE(tempfile);
    }
}

/*
 * Setup a TCP session to the router. Returns socket or <0 if failed.
 *
 */
int I3_connection_open(ROUTER_DATA *router)
{
    struct sockaddr_in                      sa;
    struct hostent                         *hostp;
    int                                     x = 1;

    i3log("Attempting connect to %s on port %d", router->ip, router->port);

    I3_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (I3_socket < 0) {
	i3log("%s", "Cannot create socket!");
	I3_connection_close(TRUE);
	return -1;
    }

    if ((x = fcntl(I3_socket, F_GETFL, 0)) < 0) {
	i3log("%s", "I3_connection_open: fcntl(F_GETFL)");
	I3_connection_close(TRUE);
	return -1;
    }

    if (fcntl(I3_socket, F_SETFL, x | O_NONBLOCK) < 0) {
	i3log("%s", "I3_connection_open: fcntl(F_SETFL)");
	I3_connection_close(TRUE);
	return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;

    if (!inet_aton(router->ip, &sa.sin_addr)) {
	hostp = gethostbyname(router->ip);
	if (!hostp) {
	    i3log("%s", "I3_connection_open: Cannot resolve router hostname.");
	    I3_connection_close(TRUE);
	    return -1;
	}
	memcpy(&sa.sin_addr, hostp->h_addr, hostp->h_length);
    }

    sa.sin_port = htons(router->port);

    if (connect(I3_socket, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
	if (errno != EINPROGRESS) {
	    i3log("I3_connection_open: Unable to connect to router %s", router->name);
	    I3_connection_close(TRUE);
	    return -1;
	}
    }
    I3_ROUTER_NAME = router->name;
    i3log("Connected to Intermud-3 router %s", router->name);
    return I3_socket;
}

/*
 * Close the socket to the router.
 */
void I3_connection_close(bool reconnect)
{
    ROUTER_DATA                            *router = NULL;
    bool                                    rfound = FALSE;

    I3_input_pointer = 0;
    I3_output_pointer = 4;
    bzero(I3_input_buffer, IPS);
    bzero(I3_output_buffer, OPS);
    bzero(I3_currentpacket, IPS);

    for (router = first_router; router; router = router->next)
	if (!strcasecmp(router->name, I3_ROUTER_NAME)) {
	    rfound = TRUE;
	    break;
	}

    if (!rfound) {
	i3log("%s", "I3_connection_close: Disconnecting from router.");
	if (I3_socket > 0) {
	    close(I3_socket);
	    I3_socket = -1;
	}
	return;
    }

    i3log("Closing connection to Intermud-3 router %s", router->name);
    if (I3_socket > 0) {
	close(I3_socket);
	I3_socket = -1;
    }
    if (reconnect) {
	if (router->reconattempts <= 3) {
	    i3wait = 100;				       /* Wait for 100 game loops */
	    i3log("%s", "Will attempt to reconnect in approximately 15 seconds.");
	} else if (router->next != NULL) {
	    i3log("Unable to reach %s. Abandoning connection.", router->name);
	    i3log("Bytes sent: %ld. Bytes received: %ld.", bytes_sent, bytes_received);
	    bytes_sent = 0;
	    bytes_received = 0;
	    i3wait = 100;
	    i3log("Will attempt new connection to %s in approximately 15 seconds.",
		  router->next->name);
	} else {
	    bytes_sent = 0;
	    bytes_received = 0;
	    i3wait = -2;
	    i3log("%s", "Unable to reconnect. No routers responding.");
	    return;
	}
    }
    i3log("Bytes sent: %ld. Bytes received: %ld.", bytes_sent, bytes_received);
    bytes_sent = 0;
    bytes_received = 0;
    return;
}

/* Free up all the data lists once the connection is down. No sense in wasting memory on it. */
void free_i3data(bool complete)
{
    I3_MUD                                 *mud,
                                           *next_mud;
    I3_CHANNEL                             *channel,
                                           *next_chan;
    I3_BAN                                 *ban,
                                           *next_ban;
    UCACHE_DATA                            *ucache,
                                           *next_ucache;
    ROUTER_DATA                            *router,
                                           *router_next;
    I3_CMD_DATA                            *cmd,
                                           *cmd_next;
    I3_ALIAS                               *alias,
                                           *alias_next;
    I3_HELP_DATA                           *help,
                                           *help_next;
    I3_COLOR                               *color,
                                           *color_next;

    if (first_i3ban) {
	for (ban = first_i3ban; ban; ban = next_ban) {
	    next_ban = ban->next;
	    I3STRFREE(ban->name);
	    I3UNLINK(ban, first_i3ban, last_i3ban, next, prev);
	    I3DISPOSE(ban);
	}
    }

    if (first_I3chan) {
	for (channel = first_I3chan; channel; channel = next_chan) {
	    next_chan = channel->next;
	    destroy_I3_channel(channel);
	}
    }

    if (first_mud) {
	for (mud = first_mud; mud; mud = next_mud) {
	    next_mud = mud->next;
	    destroy_I3_mud(mud);
	}
    }

    if (first_ucache) {
	for (ucache = first_ucache; ucache; ucache = next_ucache) {
	    next_ucache = ucache->next;
	    I3STRFREE(ucache->name);
	    I3UNLINK(ucache, first_ucache, last_ucache, next, prev);
	    I3DISPOSE(ucache);
	}
    }

    if (complete == TRUE) {
	for (router = first_router; router; router = router_next) {
	    router_next = router->next;
	    I3STRFREE(router->name);
	    I3STRFREE(router->ip);
	    I3UNLINK(router, first_router, last_router, next, prev);
	    I3DISPOSE(router);
	}

	for (cmd = first_i3_command; cmd; cmd = cmd_next) {
	    cmd_next = cmd->next;

	    for (alias = cmd->first_alias; alias; alias = alias_next) {
		alias_next = alias->next;

		I3STRFREE(alias->name);
		I3UNLINK(alias, cmd->first_alias, cmd->last_alias, next, prev);
		I3DISPOSE(alias);
	    }
	    I3STRFREE(cmd->name);
	    I3UNLINK(cmd, first_i3_command, last_i3_command, next, prev);
	    I3DISPOSE(cmd);
	}

	for (help = first_i3_help; help; help = help_next) {
	    help_next = help->next;
	    I3STRFREE(help->name);
	    I3STRFREE(help->text);
	    I3UNLINK(help, first_i3_help, last_i3_help, next, prev);
	    I3DISPOSE(help);
	}

	for (color = first_i3_color; color; color = color_next) {
	    color_next = color->next;
	    I3STRFREE(color->name);
	    I3STRFREE(color->mudtag);
	    I3STRFREE(color->imctag);
	    I3STRFREE(color->i3tag);
	    I3UNLINK(color, first_i3_color, last_i3_color, next, prev);
	    I3DISPOSE(color);
	}
    }
    return;
}

/*
 * Shutdown the connection to the router.
 */
void i3_shutdown(int delay, CHAR_DATA *ch)
{
    if (I3_socket < 1)
	return;

    I3_savehistory();
    free_i3data(FALSE);

    /*
     * Flush the outgoing buffer 
     */
    if (I3_output_pointer != 4)
	I3_write_packet(I3_output_buffer);

    I3_send_shutdown(delay);
    I3_connection_close(FALSE);
    I3_input_pointer = 0;
    I3_output_pointer = 4;
    I3_save_id();
    sleep(2);						       /* Short delay to allow the socket to close */
    if(ch)
        i3_printf(ch, "Intermud-3 router connection closed.\r\n");
}

/*
 * Connect to the router and send the startup-packet.
 * Mud port is passed in from main() so that the information passed along to the I3
 * network regarding the mud's operational port is now determined by the mud's own
 * startup script instead of the I3 config file.
 */
void router_connect(const char *router_name, bool forced, int mudport, bool isconnected)
{
    ROUTER_DATA                            *router;
    bool                                    rfound = FALSE;

    i3wait = 0;
    i3timeout = 0;
    bytes_sent = 0;
    bytes_received = 0;

    manual_router = router_name;

    /*
     * The Command table is required for operation. Like.... duh? 
     */
    if (first_i3_command == NULL) {
	if (!I3_load_commands()) {
	    i3log("%s", "router_connect: Unable to load command table!");
	    I3_socket = -1;
	    return;
	}
    }

    if (!I3_read_config(mudport)) {
	I3_socket = -1;
	return;
    }

    if (first_router == NULL) {
	if (!I3_load_routers()) {
	    i3log("%s", "router_connect: No router configurations were found!");
	    I3_socket = -1;
	    return;
	}
	I3_ROUTER_NAME = first_router->name;
    }

    /*
     * Help information should persist even when the network is not connected... 
     */
    if (first_i3_help == NULL)
	I3_load_helps();

    /*
     * ... as should the color table. 
     */
    if (first_i3_color == NULL)
	I3_load_color_table();

    if ((!this_i3mud->autoconnect && !forced && !isconnected) || (isconnected && I3_socket < 1)) {
	i3log("%s",
	      "Intermud-3 network data loaded. Autoconnect not set. Will need to connect manually.");
	I3_socket = -1;
	return;
    } else
	i3log("%s", "Intermud-3 network data loaded. Initialiazing network connection...");

    I3_loadchannels();
    I3_loadbans();

    if (this_i3mud->ucache == TRUE) {
	I3_load_ucache();
	I3_prune_ucache();
	ucache_clock = i3_time + 86400;
    }

    if (I3_socket < 1) {
	for (router = first_router; router; router = router->next) {
	    if (router_name && strcasecmp(router_name, router->name))
		continue;

	    if (router->reconattempts <= 3) {
		rfound = TRUE;
		I3_socket = I3_connection_open(router);
		break;
	    }
	}
    }

    if (!rfound && !isconnected) {
	i3log("%s", "Unable to connect. No available routers found.");
	I3_socket = -1;
	return;
    }

    if (I3_socket < 1) {
	i3wait = 100;
	return;
    }

    sleep(1);

    i3log("%s", "Intermud-3 Network initialized.");

    if (!isconnected) {
	I3_startup_packet();
	i3timeout = 100;
    } else {
	I3_loadmudlist();
	I3_loadchanlist();
    }
    I3_loadhistory();
}

/* Wrapper for router_connect now - so we don't need to force older client installs to adjust. */
void i3_startup(bool forced, int mudport, bool isconnected)
{
    i3_nuke_url_file();
    if (I3_read_config(mudport))
	router_connect(NULL, forced, mudport, isconnected);
    else
	i3bug("i3_startup: %s", "Configuration failed!");
}

void do_taunt_from_log(void)
{
    FILE                                   *fp = NULL;
    int                                     taunt_count = 0;
    char                                    line[MAX_STRING_LENGTH] = "\0\0\0\0\0\0\0\0";
    char                                    taunt[MAX_STRING_LENGTH] = "\0\0\0\0\0\0\0\0";
    int                                     taunt_pid = 0;
    int                                     i = 0;
    int                                     taunt_selection = 0;
    char                                    year[5] = "\0\0\0\0";
    char                                    month[3] = "\0\0";
    char                                    day[3] = "\0\0";
    char                                    hour[3] = "\0\0";
    char                                    minute[3] = "\0\0";
    char                                    second[3] = "\0\0";
    char                                    speaker[MAX_STRING_LENGTH] = "\0\0\0\0\0\0\0\0";
    char                                    message[MAX_STRING_LENGTH] = "\0\0\0\0\0\0\0\0";
    char                                   *s = NULL;
    char                                   *t = NULL;

    taunt_pid = fork();
    if( taunt_pid == 0 ) {
        // We're up!  Do the needful!
        if(!(fp = fopen(I3_ALLCHAN_LOG, "r"))) {
            log_error("Cannot open I3 log file: %s!", I3_ALLCHAN_LOG);
            exit(1);
        } else {
            while( fgets(line, MAX_STRING_LENGTH-2, fp) ) {
                taunt_count++;
            }
            rewind(fp);
            taunt_selection = random() % taunt_count;
            for( i = taunt_selection; i > 0; i-- ) {
                fgets(line, MAX_STRING_LENGTH-2, fp);
            }
            fclose(fp);
/* 2009.09.21-12.10,28000  imud_gossip     Sinistrad@Dead Souls Dev        Not out yet tho */
            strncpy(year, &line[0], 4);
            strncpy(month, &line[5], 2);
            strncpy(day, &line[8], 2);
            strncpy(hour, &line[11], 2);
            strncpy(minute, &line[14], 2);
            strncpy(second, &line[17], 2);
            s = strchr(line, '\t');
            if(!s || !*s) {
                exit(1);
            }
            s++;
            s = strchr(s, '\t');
            if(!s || !*s) {
                exit(1);
            }
            s++;
            t = strchr(s, '\t');
            if(!t || !*t) {
                exit(1);
            }
            bzero(speaker, MAX_STRING_LENGTH);
            strncpy(speaker, s, (t-s));
            t++;
            bzero(message, MAX_STRING_LENGTH);
            strncpy(message, t, MAX_STRING_LENGTH);
            snprintf(taunt, MAX_STRING_LENGTH, "%%^RED%%^%%^BOLD%%^[%-s-%s-%s %s:%s]%%^RESET%%^ %%^YELLOW%%^%s%%^RESET%%^ once said %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^",
                     year, month, day, hour, minute, speaker, message);
            //i3_npc_speak("wiley", "Cron", taunt); // prool
            exit(0);
        }
    } else {
        // Zombie patrol should be handled by ignoring SIGCHLD
    }
}


#define I3_TAUNT_FILE   I3_DIR "i3.taunts"

char *i3_taunt_line()
{
    FILE            *fp = NULL;
    struct stat      fst;
    static int       taunt_count = 0;
    static char    **taunt_list = NULL;
    static int       last_changed = 0;
    static int       already_using_defaults = 0;
    int              i = 0;
    char             line[MAX_STRING_LENGTH] = "\0\0\0\0\0\0\0\0";

    if (stat(I3_TAUNT_FILE, &fst) != -1) {
        if( fst.st_mtime > last_changed ) {
            /* File has been updated, so reload it */
            last_changed = fst.st_mtime;

            if (taunt_list) {
                for (i = 0; i < taunt_count; i++) {
                    if (taunt_list[i]) {
                        free(taunt_list[i]);
                        taunt_list[i] = NULL;
                    }
                }
                free(taunt_list);
                taunt_list = NULL;
                taunt_count = 0;
            }
            if(!(fp = fopen(I3_TAUNT_FILE, "r"))) {
                log_error("Cannot open I3 taunt file: %s!", I3_TAUNT_FILE);
                already_using_defaults = 0;
                goto no_taunt_file;
            } else {
                while( fgets(line, MAX_STRING_LENGTH-2, fp) ) {
                    taunt_count++;
                }
                rewind(fp);
                taunt_list = (char **) (unsigned long int) calloc(taunt_count, sizeof(char *)); // prool fool
                for( i = 0; i < taunt_count; i++ ) {
                    taunt_list[i] = strdup( fgets(line, MAX_STRING_LENGTH-2, fp) );
                }
                fclose(fp);
                already_using_defaults = 0;
            }
        }
    } else {
no_taunt_file:
        /* No file, so use a small set of built-in taunts. */
        if( !already_using_defaults ) {
            if (taunt_list) {
                for (i = 0; i < taunt_count; i++) {
                    if (taunt_list[i]) {
                        free(taunt_list[i]);
                        taunt_list[i] = NULL;
                    }
                }
                free(taunt_list);
                taunt_list = NULL;
                taunt_count = 0;
            }

            taunt_count = 10;
            taunt_list = (char **)(unsigned long int)calloc(taunt_count, sizeof(char *)); // prool fool
            taunt_list[0] = strdup("Ummmm.. go away, we already got one.");
            taunt_list[1] = strdup("Connection closed by foreign host");
            taunt_list[2] = strdup("NO CARRIER");
            taunt_list[3] = strdup("I wish this connection would stay open.");
            taunt_list[4] = strdup("I hate you!");
            taunt_list[5] = strdup("Why will you not die?");
            taunt_list[6] = strdup("WTF are you still doing here?");
            taunt_list[7] = strdup("I hate ALL of you!");
            taunt_list[8] = strdup("When I am dictator, things will run smoothly...");
            taunt_list[9] = strdup("SUFFER!  You will all SUFFER!");
            already_using_defaults = 1;
        }
    }

    /* We should have taunt data now */
    return (taunt_list[i3number(0, taunt_count-1)]);
}

#define I3_USERMAP_FILE   I3_DIR "i3.usermap"

char *I3_nameremap(const char *ps)
{
    FILE            *fp = NULL;
    struct stat      fst;
    static int       map_count = 0;
    static char    **key_list = NULL;
    static char    **value_list = NULL;
    static int       last_changed = 0;
    int              i = 0;
    char             line[MAX_STRING_LENGTH] = "\0\0\0\0\0\0\0\0";
    static char      remapped[MAX_STRING_LENGTH];
    char            *s;

    strcpy(remapped, ps);

    if (stat(I3_USERMAP_FILE, &fst) != -1) {
        if( fst.st_mtime > last_changed ) {
            /* File has been updated, so reload it */
            last_changed = fst.st_mtime;

            if (key_list) {
                for (i = 0; i < map_count; i++) {
                    if (key_list[i]) {
                        free(key_list[i]);
                        key_list[i] = NULL;
                    }
                }
                free(key_list);
                key_list = NULL;
            }
            if (value_list) {
                for (i = 0; i < map_count; i++) {
                    if (value_list[i]) {
                        free(value_list[i]);
                        value_list[i] = NULL;
                    }
                }
                free(value_list);
                value_list = NULL;
            }
            map_count = 0;

            if(!(fp = fopen(I3_USERMAP_FILE, "r"))) {
                log_error("Cannot open I3 usermap file: %s!", I3_USERMAP_FILE);
                return remapped;
            } else {
                while( fgets(line, MAX_STRING_LENGTH-2, fp) ) {
                    map_count++;
                }
                rewind(fp);
                key_list = (char **)(unsigned long int)calloc(map_count, sizeof(char *)); // prool fool
                value_list = (char **)(unsigned long int)calloc(map_count, sizeof(char *)); // prool fool
                for( i = 0; i < map_count; i++ ) {
                    s = i3fread_word(fp);
                    if( s && *s) {
                        key_list[i] = strdup(s);
                        s = i3fread_rest_of_line(fp);
                        if( s && *s) {
                            value_list[i] = strdup(s);
                        } else {
                            value_list[i] = strdup(key_list[i]);
                        }
                    } else {
                        // We have to put something here to avoid NULL pointers.
                        key_list[i] = strdup("INVALID_NAME");
                        // If the key wasn't valid, the value probably can't be either.
                        // I guess just skip to the next and hope.
                        value_list[i] = strdup("INVALID_NAME");
                    }
                }
                fclose(fp);
            }
        }
    }

    if(map_count > 0) {
        for( i = 0; i < map_count; i++ ) {
            if(strcasecmp(key_list[i], ps) == 0) {
                strcpy(remapped, value_list[i]);
                break;
            }
        }
    }

    return remapped;
}

void i3_nuke_url_file() {
    struct stat      fst;

    if(stat(I3_URL_DUMP, &fst) != -1) {
        truncate(I3_URL_DUMP, 0);
    }
}

void i3_check_urls() {
    FILE            *fp = NULL;
    struct stat      fst;
    char             line[MAX_STRING_LENGTH] = "\0\0\0\0\0\0\0\0";
    static int       last_changed = 0;
    int              i = 0;
    int              j = 0;
    //int              x = 0;

    if(stat(I3_URL_DUMP, &fst) != -1) {
        if( fst.st_mtime > last_changed ) {
            i3log("URL file is ready to process!");
            /* File has been updated, so reload it */
            last_changed = fst.st_mtime;

            if(!(fp = fopen(I3_URL_DUMP, "r"))) {
                log_error("No URL DUMP file: %s!", I3_URL_DUMP);
            } else {
                while( fgets(line, MAX_STRING_LENGTH-2, fp) ) {
                    /*
                    // Remove trailing newlines, if any
                    while(*line && ((x = strlen(line)) > 0)) {
                        if(ISNEWL(line[x-1])) {
                            line[x-1] = '\0';
                        }
                    }
                    // If anything is left, add a proper newline
                    if(*line && x > 0 && !ISNEWL(line[x-1])) {
                        line[x-1] = '\r';
                        line[x] = '\n';
                        line[x+1] = '\0';
                    }
                    if(*line || strlen(line) < 3) {
                        // Nothing but the newline is here, skip it
                        continue;
                    }
                    */
                    i++;
                    if(*line) {
                        i3_npc_speak("url", "URLbot", line);
                        j++;
                    }
                }
                fclose(fp);
                fp = NULL;
                truncate(I3_URL_DUMP, 0);
                i3log("%d lines read, %d results sent, from URL file %s.", i, j, I3_URL_DUMP);
            }
        }
    }
}

/*
 * Check for a packet and if one available read it and parse it.
 * Also checks to see if the mud should attempt to reconnect to the router.
 * This is an input only loop. Attempting to use it to send buffered output
 * just wasn't working out, so output has gone back to sending packets to the
 * router as soon as they're assembled.
 */
void i3_loop(void)
{
    ROUTER_DATA                            *router;
    int                                     ret;
    long                                    size;
    fd_set                                  in_set,
                                            out_set,
                                            exc_set;
    static struct timeval                   last_time,
                                            null_time;
    bool                                    rfound = FALSE;

    struct tm                              *tm_info = NULL;
    time_t                                  tc = (time_t) 0;
    char                                    taunt[MAX_STRING_LENGTH];
    /*
    int                                     chan_choice = 0;
    const char                             *chan_list[] = {
        "wiley",
        "wiley",
        "wiley",
        "wiley",
        "wiley",
        "wiley",
        "intergossip",
        "wiley",
        "wiley",
        "wiley",
        "wiley",
        "wiley",
        "wiley",
        "dwchat"
        "wiley",
        "wiley",
        "wiley",
        "wiley",
        "wiley",
        "wiley"
    };
    */

#if 0 // 0 - prool 1 - orig
    gettimeofday(&last_time, NULL);
    i3_time = (time_t) last_time.tv_sec;
#else
    i3_time = time(0);
#endif

    FD_ZERO(&in_set);
    FD_ZERO(&out_set);
    FD_ZERO(&exc_set);

    if (i3wait > 0)
	i3wait--;

    if (i3timeout > 0) {
	i3timeout--;
	if (i3timeout == 0) {				       /* Time's up baby! */
	    i3log("I3 Client timeout.");
	    I3_connection_close(TRUE);
	    return;
	}
    }

    tc = time(0);
    tm_info = localtime(&tc);

    /* We reboot our router every Monday, Wedensday, and Friday, at 4:45AM.  This makes the I3
     * connection die, but we can't seem to recognize this, so bounce I3 at 5AM on those days.
     */
    if ((tm_info->tm_wday == 1) || (tm_info->tm_wday == 3) || (tm_info->tm_wday == 5)) {
        if ((tm_info->tm_hour == 5) && (tm_info->tm_min == 0) && (tm_info->tm_sec == 0)) {
	    i3log("I3 Client is rebooting for weekly router reboot.");
            I3_connection_close(TRUE);
            return;
        }
    }

    /*
     * This condition can only occur if you were previously connected and the socket was closed.
     * * Tries 3 times, then attempts connection to an alternate router, if it has one.
     */
    if (i3wait == 1) {
	for (router = first_router; router; router = router->next) {
	    if (manual_router && strcasecmp(router->name, manual_router))
		continue;

	    if (router->reconattempts <= 3) {
		rfound = TRUE;
		break;
	    }
	}

	if (!rfound) {
	    i3wait = -2;
	    i3log("%s", "I3 Unable to reconnect. No routers responding.");
	    return;
	}
	I3_socket = I3_connection_open(router);
	if (I3_socket < 1) {
	    if (router->reconattempts <= 3)
		i3wait = 100;
	    return;
	}

	sleep(1);

	i3log("Connection to Intermud-3 router %s %s.",
	      router->name, router->reconattempts > 0 ? "reestablished" : "established");
	router->reconattempts++;
	I3_startup_packet();
	i3timeout = 100;
	return;
    }

    if (!i3_is_connected())
	return;

    // A version of keepalive...
    if (i3justconnected) {
        i3justconnected = 0;
        snprintf(taunt, MAX_STRING_LENGTH, "%%^RED%%^%%^BOLD%%^[%-4.4d-%-2.2d-%-2.2d %-2.2d:%-2.2d]%%^RESET%%^ %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^ %%^YELLOW%%^%s%%^RESET%%^",
                tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday, tm_info->tm_hour, tm_info->tm_min, "It's ALIVE!\r\n", "prool v.0"/*VERSION_STR*/);
        //i3_npc_speak("wiley", "Cron", taunt); // prool
        tics_since_last_message = TAUNT_DELAY;
    }

    tics_since_last_message--;

    if ( tics_since_last_message <= 0 ) {
        tics_since_last_message = TAUNT_DELAY;

        do_taunt_from_log();

        /*
        snprintf(taunt, MAX_STRING_LENGTH, "%%^RED%%^%%^BOLD%%^[%-4.4d-%-2.2d-%-2.2d %-2.2d:%-2.2d]%%^RESET%%^ %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^",
                tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday, tm_info->tm_hour, tm_info->tm_min, i3_taunt_line());

        chan_choice = i3number(0, (sizeof(chan_list) / sizeof(chan_list[0])) - 1);
        i3_npc_speak(chan_list[chan_choice], "Cron", taunt);
        */
    }

    // Check for urls that our external process prepared for us.
    i3_check_urls();

    /*
     * Will prune the cache once every 24hrs after bootup time 
     */
    if (ucache_clock <= i3_time) {
	ucache_clock = i3_time + 86400;
	I3_prune_ucache();
    }

    FD_SET(I3_socket, &in_set);
    FD_SET(I3_socket, &out_set);
    FD_SET(I3_socket, &exc_set);

    if (select(I3_socket + 1, &in_set, &out_set, &exc_set, &null_time) < 0) {
	perror("i3_loop: select: Unable to poll I3_socket!");
	I3_connection_close(TRUE);
	return;
    }

    if (FD_ISSET(I3_socket, &exc_set)) {
	FD_CLR(I3_socket, &in_set);
	FD_CLR(I3_socket, &out_set);
	i3log("%s", "Exception raised on I3 socket.");
	I3_connection_close(TRUE);
	return;
    }

    if (FD_ISSET(I3_socket, &in_set)) {
	ret = read(I3_socket, I3_input_buffer + I3_input_pointer, MAX_STRING_LENGTH);
	if (!ret || (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
	    FD_CLR(I3_socket, &out_set);
	    if (ret < 0)
		i3log("%s", "Read error on I3 socket.");
	    else
		i3log("%s", "EOF encountered on I3 socket read.");
	    I3_connection_close(TRUE);
	    return;
	}
	if (ret < 0)					       /* EAGAIN */
	    return;
	if (ret == MAX_STRING_LENGTH)
	    i3log("%s", "String overflow in I3 socket read!");

	I3_input_pointer += ret;
	bytes_received += ret;
	if (packetdebug)
	    i3log("Bytes received: %d", ret);
    }

    memcpy(&size, I3_input_buffer, 4);
    size = ntohl(size);

    if (size <= I3_input_pointer - 4) {
	I3_read_packet();
	I3_parse_packet();
    }
    return;
}

/*****************************************
 * User level commands and social hooks. *
 *****************************************/

/* This is very possibly going to be spammy as hell */
I3_CMD(I3_show_ucache_contents)
{
    UCACHE_DATA                            *user;
    int                                     users = 0;

    i3send_to_pager("%%^WHITE%%^Cached user information%%^RESET%%^\r\n", ch);
    i3send_to_pager
	("%%^WHITE%%^User                          | Gender ( 0 = Male, 1 = Female, 2 = Neuter )%%^RESET%%^\r\n",
	 ch);
    i3send_to_pager
	("%%^WHITE%%^---------------------------------------------------------------------------%%^RESET%%^\r\n",
	 ch);
    for (user = first_ucache; user; user = user->next) {
	i3page_printf(ch, "%%^WHITE%%^%-30s %d%%^RESET%%^\r\n", user->name, user->gender);
	users++;
    }
    i3page_printf(ch, "%%^WHITE%%^%d users being cached.%%^RESET%%^\r\n", users);
    return;
}

I3_CMD(I3_beep)
{
    char                                   *ps;
    char                                    mud[MAX_INPUT_LENGTH];
    I3_MUD                                 *pmud;

#if 0 // prool fool
    if (I3IS_SET(I3FLAG(ch), I3_DENYBEEP)) {
	i3_printf(ch, "You are not allowed to use i3beeps.\r\n");
	return;
    }
#endif

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3beep user@mud%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Usage: i3beep [on]/[off]%%^RESET%%^\r\n");
	return;
    }

    if (!strcasecmp(argument, "on")) {
	//I3REMOVE_BIT(I3FLAG(ch), I3_BEEP); // prool fool
	i3_printf(ch, "You now send and receive i3beeps.\r\n");
	return;
    }

    if (!strcasecmp(argument, "off")) {
	//I3SET_BIT(I3FLAG(ch), I3_BEEP); // prool fool
	i3_printf(ch, "You no longer send and receive i3beeps.\r\n");
	return;
    }

#if 0 // prool fool
    if (I3IS_SET(I3FLAG(ch), I3_BEEP)) {
	i3_printf(ch, "Your i3beeps are turned off.\r\n");
	return;
    }

    if (I3ISINVIS(ch)) {
	i3_printf(ch, "You are invisible.\r\n");
	return;
    }
#endif

    ps = (char *)strchr(argument, '@');

    if (!argument || argument[0] == '\0' || ps == NULL) {
	i3_printf(ch, "%%^YELLOW%%^You should specify a person@mud.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    ps[0] = '\0';
    ps++;
    i3strlcpy(mud, ps, MAX_INPUT_LENGTH);

    if (!(pmud = find_I3_mud_by_name(mud))) {
	i3_printf(ch, "%%^YELLOW%%^No such mud known.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    if (!strcasecmp(this_i3mud->name, pmud->name)) {
	i3_printf(ch, "Use your mud's own internal system for that.\r\n");
	return;
    }

    if (pmud->status >= 0) {
	i3_printf(ch, "%s is marked as down.\r\n", pmud->name);
	return;
    }

    if (pmud->beep == 0)
	i3_printf(ch, "%s does not support the 'beep' command. Sending anyway.\r\n",
		  pmud->name);

    I3_send_beep(ch, argument, pmud);
    i3_printf(ch, "%%^YELLOW%%^You i3beep %s@%s.%%^RESET%%^\r\n", i3capitalize(argument), pmud->name);
}

I3_CMD(I3_tell)
{
    char                                    to[MAX_INPUT_LENGTH],
                                           *ps;
    char                                    mud[MAX_INPUT_LENGTH];
    I3_MUD                                 *pmud;
    struct tm                              *local = localtime(&i3_time);
#if 0 // prool fool
    if (I3IS_SET(I3FLAG(ch), I3_DENYTELL)) {
	i3_printf(ch, "You are not allowed to use i3tells.\r\n");
	return;
    }
#endif

    if (!argument || argument[0] == '\0') {
	int                                     x;

	i3_printf(ch, "%%^WHITE%%^Usage: i3tell <user@mud> <message>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Usage: i3tell [on]/[off]%%^RESET%%^\r\n\r\n");
	i3_printf(ch, "%%^CYAN%%^The last %d things you were told over I3:%%^RESET%%^\r\n", MAX_I3TELLHISTORY);
#if 0 // proolfool
	for (x = 0; x < MAX_I3TELLHISTORY; x++) {
	    if (I3TELLHISTORY(ch, x) == NULL)
		break;
	    i3_printf(ch, "%s\r\n", I3TELLHISTORY(ch, x));
	}
#endif
	return;
    }
#if 0 // prool fool
    if (!strcasecmp(argument, "on")) {
	I3REMOVE_BIT(I3FLAG(ch), I3_TELL);
	i3_printf(ch, "You now send and receive i3tells.\r\n");
	return;
    }

    if (!strcasecmp(argument, "off")) {
	I3SET_BIT(I3FLAG(ch), I3_TELL);
	i3_printf(ch, "You no longer send and receive i3tells.\r\n");
	return;
    }

    if (I3IS_SET(I3FLAG(ch), I3_TELL)) {
	i3_printf(ch, "Your i3tells are turned off.\r\n");
	return;
    }

    if (I3ISINVIS(ch)) {
	i3_printf(ch, "You are invisible.\r\n");
	return;
    }
#endif
    argument = i3one_argument(argument, to);
    ps = strchr(to, '@');

    if (to[0] == '\0' || argument[0] == '\0' || ps == NULL) {
	i3_printf(ch, "%%^YELLOW%%^You should specify a person and a mud.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    ps[0] = '\0';
    ps++;
    i3strlcpy(mud, ps, MAX_INPUT_LENGTH);

    if (!(pmud = find_I3_mud_by_name(mud))) {
	i3_printf(ch, "%%^YELLOW%%^No such mud known.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    if (!strcasecmp(this_i3mud->name, pmud->name)) {
	i3_printf(ch, "Use your mud's own internal system for that.\r\n");
	return;
    }

    if (pmud->status >= 0) {
	i3_printf(ch, "%s is marked as down.\r\n", pmud->name);
	return;
    }

    if (pmud->tell == 0) {
	i3_printf(ch, "%s does not support the 'tell' command.\r\n", pmud->name);
	return;
    }

    I3_send_tell(ch, to, pmud->name, argument);
    snprintf(mud, MAX_INPUT_LENGTH, "%s %%^YELLOW%%^You i3tell %%^CYAN%%^%%^BOLD%%^%s@%s%%^RESET%%^%%^YELLOW%%^: %%^RESET%%^%s", color_time(local), i3capitalize(to), pmud->name, argument);
    i3_printf(ch, "%s%%^RESET%%^\r\n", mud);
    i3_update_tellhistory(ch, mud);
}

I3_CMD(I3_reply)
{
    char                                     buf[MAX_STRING_LENGTH];
    char                                     to[MAX_INPUT_LENGTH];
    char                                     mud[MAX_INPUT_LENGTH];
    char                                    *ps;
    I3_MUD                                  *pmud;
    struct tm                               *local = localtime(&i3_time);
#if 0 // prool fool
    if (I3IS_SET(I3FLAG(ch), I3_DENYTELL)) {
	i3_printf(ch, "You are not allowed to use i3tells.\r\n");
	return;
    }

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "Usage: i3reply <message>\r\n");
	return;
    }

    if (I3IS_SET(I3FLAG(ch), I3_TELL)) {
	i3_printf(ch, "Your i3tells are turned off.\r\n");
	return;
    }

    if (I3ISINVIS(ch)) {
	i3_printf(ch, "You are invisible.\r\n");
	return;
    }
#endif
    if (!i3_str_prefix("set ", argument)) {
        argument += 4;
        argument = i3one_argument(argument, to);

        i3_printf(ch, "%%^GREEN%%^DEBUG: argument == %s\r\n", argument);
        i3_printf(ch, "%%^GREEN%%^DEBUG: to == %s\r\n", to);

        ps = strchr(to, '@');
        if (to[0] == '\0' || ps == NULL) {
            i3_printf(ch, "%%^YELLOW%%^You should specify a person and a mud.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
            return;
        }
        ps[0] = '\0';
        ps++;
        i3strlcpy(mud, ps, MAX_INPUT_LENGTH);

        i3_printf(ch, "%%^GREEN%%^DEBUG: to == %s\r\n", to);
        i3_printf(ch, "%%^GREEN%%^DEBUG: mud == %s\r\n", mud);

        if (!(pmud = find_I3_mud_by_name(mud))) {
            i3_printf(ch, "%%^YELLOW%%^No such mud known.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
            return;
        }

        if (!strcasecmp(this_i3mud->name, pmud->name)) {
            i3_printf(ch, "Use your mud's own internal system for that.\r\n");
            return;
        }

        if (pmud->status >= 0) {
            i3_printf(ch, "%s is marked as down.\r\n", pmud->name);
            return;
        }

        if (pmud->tell == 0) {
            i3_printf(ch, "%s does not support the 'tell' command.\r\n", pmud->name);
            return;
        }

        snprintf(buf, MAX_INPUT_LENGTH, "%s@%s", to, pmud->name);
#if 0 // prool fool
        I3STRFREE(I3REPLYNAME(ch));
        I3STRFREE(I3REPLYMUD(ch));
        I3REPLYNAME(ch) = I3STRALLOC(to);
        I3REPLYMUD(ch) = I3STRALLOC(pmud->name);
        i3_printf(ch, "i3reply target set to %s@%s.\r\n", I3REPLYNAME(ch), I3REPLYMUD(ch));
#endif
        return;
    }
#if 0 // prool fool
    if (!I3REPLYNAME(ch) || !I3REPLYMUD(ch)) {
	i3_printf(ch, "You have not yet received an i3tell?!?\r\n");
	return;
    }

    if (!strcasecmp(argument, "show")) {
	i3_printf(ch, "The last i3tell you received was from %s@%s.\r\n", I3REPLYNAME(ch), I3REPLYMUD(ch));
        return;
    }

    I3_send_tell(ch, I3REPLYNAME(ch), I3REPLYMUD(ch), argument);
    snprintf(buf, MAX_STRING_LENGTH, "%s %%^YELLOW%%^You i3reply to %%^CYAN%%^%%^BOLD%%^%s@%s%%^RESET%%^%%^YELLOW%%^: %%^RESET%%^%s", color_time(local), i3capitalize(I3REPLYNAME(ch)), I3REPLYMUD(ch), argument);
#endif
    i3_printf(ch, "%s%%^RESET%%^\r\n", buf);
    i3_update_tellhistory(ch, buf);

    /*
    snprintf(buf, MAX_STRING_LENGTH, "%s %s", I3REPLY(ch), argument);
    I3_tell(ch, buf);
    */
    return;
}

I3_CMD(I3_mudlisten)
{
    I3_CHANNEL                             *channel;
    char                                    arg[MAX_INPUT_LENGTH];

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3mudlisten [all/none]%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Usage: i3mudlisten <localchannel> [on/off]%%^RESET%%^\r\n");
	return;
    }

    if (!strcasecmp(argument, "all")) {
	for (channel = first_I3chan; channel; channel = channel->next) {
	    if (!channel->local_name || channel->local_name[0] == '\0')
		continue;

	    i3_printf(ch, "Subscribing to %s.\r\n", channel->local_name);
	    I3_send_channel_listen(channel, TRUE);
	}
	i3_printf(ch, "%%^YELLOW%%^The mud is now subscribed to all available local I3 channels.%%^RESET%%^\r\n");
	return;
    }

    if (!strcasecmp(argument, "none")) {
	for (channel = first_I3chan; channel; channel = channel->next) {
	    if (!channel->local_name || channel->local_name[0] == '\0')
		continue;

	    i3_printf(ch, "Unsubscribing from %s.\r\n", channel->local_name);
	    I3_send_channel_listen(channel, FALSE);
	}
	i3_printf(ch, "%%^YELLOW%%^The mud is now unsubscribed from all available local I3 channels.%%^RESET%%^\r\n");
	return;
    }

    argument = i3one_argument(argument, arg);
    if (!(channel = find_I3_channel_by_localname(arg))) {
	i3_printf(ch, "No such channel configured locally.\r\n");
	return;
    }

    if (!strcasecmp(argument, "on")) {
	i3_printf(ch, "Turning %s channel on.\r\n", channel->local_name);
	I3_send_channel_listen(channel, TRUE);
	return;
    }

    if (!strcasecmp(argument, "off")) {
	i3_printf(ch, "Turning %s channel off.\r\n", channel->local_name);
	I3_send_channel_listen(channel, FALSE);
	return;
    }
    I3_mudlisten(ch, "");
    return;
}

I3_CMD(I3_mudlist)
{
    I3_MUD                                 *mud;
    char                                    filter[MAX_INPUT_LENGTH];
    int                                     mudcount = 0;
    bool                                    all = FALSE;

    argument = i3one_argument(argument, filter);

    if (!strcasecmp(filter, "all")) {
	all = TRUE;
	argument = i3one_argument(argument, filter);
    }

    if (first_mud == NULL) {
	i3_printf(ch, "There are no muds to list!?\r\n");
	return;
    }

    i3page_printf(ch, "%%^WHITE%%^%%^BOLD%%^%-30s%-10.10s%-25.25s%-15.15s %s%%^RESET%%^\r\n", "Name", "Type", "Mudlib",
		  "Address", "Port");
    for (mud = first_mud; mud; mud = mud->next) {
	if (mud == NULL) {
	    i3bug("%s", "I3_mudlist: NULL mud found in listing!");
	    continue;
	}

	if (mud->name == NULL) {
	    i3bug("%s", "I3_mudlist: NULL mud name found in listing!");
	    continue;
	}

	if (filter[0] && i3_str_prefix(filter, mud->name) &&
	    (mud->mud_type && i3_str_prefix(filter, mud->mud_type)) && (mud->mudlib
									&& i3_str_prefix(filter,
											 mud->mudlib)))
	    continue;

	if (!all && mud->status == 0)
	    continue;

	mudcount++;

	switch (mud->status) {
	    case -1:
		i3page_printf(ch, "%%^CYAN%%^%-30s%-10.10s%-25.25s%-15.15s %d%%^RESET%%^\r\n",
			      mud->name, mud->mud_type, mud->mudlib, mud->ipaddress,
			      mud->player_port);
		break;
	    case 0:
		i3page_printf(ch, "%%^RED%%^%%^BOLD%%^%-26s(down)%%^RESET%%^\r\n", mud->name);
		break;
	    default:
		i3page_printf(ch, "%%^YELLOW%%^%-26s(rebooting, back in %d seconds)%%^RESET%%^\r\n", mud->name,
			      mud->status);
		break;
	}
    }
    i3page_printf(ch, "%%^WHITE%%^%%^BOLD%%^%d total muds listed.%%^RESET%%^\r\n", mudcount);
    return;
}

I3_CMD(I3_chanlist)
{char local_buf[PROOL_MAX_STRLEN];
    I3_CHANNEL                             *channel;
    bool                                    all = FALSE,
	found = FALSE;
    char                                    filter[MAX_INPUT_LENGTH];

    *filter = '\0';
    argument = i3one_argument(argument, filter);

    if (!strcasecmp(filter, "all") && i3_is_connected()) {
	all = TRUE;
	argument = i3one_argument(argument, filter);
	//printf("Showing ALL known channels.\r\n\r\n", ch);
    }

    sprintf(local_buf,"%s", "Local name          Perm    I3 Name             Hosted at           Status\r\n");
    send_to_char(ch, local_buf);
    sprintf(local_buf,"%s", "-------------------------------------------------------------------------------\r\n");
    send_to_char(ch, local_buf);
    for (channel = first_I3chan; channel; channel = channel->next) {
	found = FALSE;

	if (!all && !channel->local_name && (filter[0] == '\0'))
	    continue;
#if 0 // prool fool
	if (I3PERM(ch) < I3PERM_ADMIN && !channel->local_name)
	    continue;

	if (I3PERM(ch) < channel->i3perm)
	    continue;
#endif
	if (!all && filter[0] != '\0' && i3_str_prefix(filter, channel->I3_name)
	    && i3_str_prefix(filter, channel->host_mud))
	    continue;

#if 1 // prool fool
	if (channel->local_name /*&& I3_hasname(I3LISTEN(ch), channel->local_name)*/)
	    found = TRUE;
#endif
// prool: i3page_printf to printf:
	sprintf(local_buf,"%c %-18s%-8s%-20s%-20s%-8s\r\n",
		      found ? '*' : ' ',
		      channel->local_name ? channel->local_name : "Not configured",
		      perm_names[channel->i3perm], channel->I3_name, channel->host_mud,
		      channel->status == 0 ? "Public" : "Private");
    	send_to_char(ch, local_buf);
    }
    sprintf(local_buf,"%s", "*: You are listening to these channels.\r\n");
    send_to_char(ch, local_buf);
    return;
}

I3_CMD(I3_setup_channel)
{
    CHAR_DATA                              *vch;
    DESCRIPTOR_DATA                        *d;
    char                                    localname[MAX_INPUT_LENGTH],
                                            I3_name[MAX_INPUT_LENGTH];
    I3_CHANNEL                             *channel,
                                           *channel2;
    int                                     permvalue = I3PERM_MORT;

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "Usage: i3setchan <i3channelname> <localname> [permission]\r\n");
	return;
    }

    argument = i3one_argument(argument, I3_name);
    argument = i3one_argument(argument, localname);

    if (!(channel = find_I3_channel_by_name(I3_name))) {
	i3_printf(ch, "%%^YELLOW%%^Unknown channel%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3chanlist%%^YELLOW%%^ to get an overview of the channels available)%%^RESET%%^\r\n");
	return;
    }

    if (localname[0] == '\0') {
	if (!channel->local_name) {
	    i3_printf(ch, "Channel %s@%s isn't configured.%%^RESET%%^\r\n", channel->I3_name,
		      channel->host_mud);
	    return;
	}
#if 0 // prool fool
	if (channel->i3perm > I3PERM(ch)) {
	    i3_printf(ch, "You do not have sufficient permission to remove the %s channel.\r\n",
		      channel->local_name);
	    return;
	}
#endif

	for (d = first_descriptor; d; d = d->next) {
	    vch = d->original ? d->original : d->character;

	    if (!vch)
		continue;
#if 0 // prool fool
	    if (I3_hasname(I3LISTEN(vch), channel->local_name))
		I3_unflagchan(&I3LISTEN(vch), channel->local_name);
	    if (I3_hasname(I3DENY(vch), channel->local_name))
		I3_unflagchan(&I3DENY(vch), channel->local_name);
#endif
	}
	i3log("setup_channel: removing %s as %s@%s", channel->local_name, channel->I3_name,
	      channel->host_mud);
	I3_send_channel_listen(channel, FALSE);
	I3STRFREE(channel->local_name);
	I3_write_channel_config();
    } else {
	if (channel->local_name) {
	    i3_printf(ch, "Channel %s@%s is already known as %s.\r\n", channel->I3_name,
		      channel->host_mud, channel->local_name);
	    return;
	}
	if ((channel2 = find_I3_channel_by_localname(localname))) {
	    i3_printf(ch, "Channel %s@%s is already known as %s.\r\n", channel2->I3_name,
		      channel2->host_mud, channel2->local_name);
	    return;
	}

	if (argument && argument[0] != '\0') {
	    permvalue = get_permvalue(argument);
	    if (permvalue < 0 || permvalue > I3PERM_IMP) {
		i3_printf(ch, "Invalid permission setting.\r\n");
		return;
	    }
#if 0 // prool fool
	    if (permvalue > I3PERM(ch)) {
		i3_printf(ch, "You cannot assign a permission value above your own.\r\n");
		return;
	    }
#endif
	}
	channel->local_name = I3STRALLOC(localname);
	channel->i3perm = permvalue;
	I3STRFREE(channel->layout_m);
	I3STRFREE(channel->layout_e);
	channel->layout_m = I3STRALLOC("%%^RED%%^%%^BOLD%%^[%%^WHITE%%^%%^BOLD%%^%s%%^RED%%^%%^BOLD%%^] %%^CYAN%%^%%^BOLD%%^%s@%s: %%^CYAN%%^%s");
	channel->layout_e = I3STRALLOC("%%^RED%%^%%^BOLD%%^[%%^WHITE%%^%%^BOLD%%^%s%%^RED%%^%%^BOLD%%^] %%^CYAN%%^%s");
	i3_printf(ch, "%s@%s is now locally known as %s\r\n", channel->I3_name,
		  channel->host_mud, channel->local_name);
	i3log("setup_channel: setting up %s@%s as %s", channel->I3_name, channel->host_mud,
	      channel->local_name);
	I3_send_channel_listen(channel, TRUE);
	I3_write_channel_config();
    }
}

I3_CMD(I3_edit_channel)
{
    char                                    localname[MAX_INPUT_LENGTH];
    char                                    arg2[MAX_INPUT_LENGTH];
    I3_CHANNEL                             *channel;

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3 editchan <localname> localname <new localname>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Usage: i3 editchan <localname> perm <type>%%^RESET%%^\r\n");
	return;
    }

    argument = i3one_argument(argument, localname);

    if ((channel = find_I3_channel_by_localname(localname)) == NULL) {
	i3_printf(ch, "%%^YELLOW%%^Unknown local channel%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3chanlist%%^YELLOW%%^ to get an overview of the channels available)%%^RESET%%^\r\n");
	return;
    }

    argument = i3one_argument(argument, arg2);
#if 0 // prool fool
    if (channel->i3perm > I3PERM(ch)) {
	i3_printf(ch, "You do not have sufficient permissions to edit this channel.\r\n");
	return;
    }
#endif

    if (!strcasecmp(arg2, "localname")) {
	i3_printf(ch, "Local channel %s renamed to %s.\r\n", channel->local_name, argument);
	I3STRFREE(channel->local_name);
	channel->local_name = I3STRALLOC(argument);
	I3_write_channel_config();
	return;
    }

    if (!strcasecmp(arg2, "perm") || !strcasecmp(arg2, "permission")) {
	int                                     permvalue = get_permvalue(argument);

	if (permvalue < 0 || permvalue > I3PERM_IMP) {
	    i3_printf(ch, "Invalid permission setting.\r\n");
	    return;
	}
#if 0 // prool fool
	if (permvalue > I3PERM(ch)) {
	    i3_printf(ch, "You cannot set a permission higher than your own.\r\n");
	    return;
	}
	if (channel->i3perm > I3PERM(ch)) {
	    i3_printf(ch, "You cannot edit a channel above your permission level.\r\n");
	    return;
	}
#endif
	channel->i3perm = permvalue;
	i3_printf(ch, "Local channel %s permission changed to %s.\r\n", channel->local_name,
		  argument);
	I3_write_channel_config();
	return;
    }
    I3_edit_channel(ch, "");
    return;
}

I3_CMD(I3_chan_who)
{
    char                                    channel_name[MAX_INPUT_LENGTH];
    I3_CHANNEL                             *channel;
    I3_MUD                                 *mud;

    argument = i3one_argument(argument, channel_name);

    if (channel_name[0] == '\0' || !argument || argument[0] == '\0') {
	i3_printf(ch, "Usage: i3chanwho <local channel> <mud>\r\n");
	return;
    }

    if ((channel = find_I3_channel_by_localname(channel_name)) == NULL) {
	i3_printf(ch, "%%^YELLOW%%^Unknown channel.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3chanlist%%^YELLOW%%^ to get an overview of the channels available)%%^RESET%%^\r\n");
	return;
    }

    if (!(mud = find_I3_mud_by_name(argument))) {
	i3_printf(ch, "%%^YELLOW%%^Unknown mud.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    if (mud->status >= 0) {
	i3_printf(ch, "%s is marked as down.\r\n", mud->name);
	return;
    }

    I3_send_chan_who(ch, channel, mud);
}

I3_CMD(I3_listen_channel)
{
    I3_CHANNEL                             *channel;

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^CYAN%%^Currently tuned into:%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n",
		  /*(I3LISTEN(ch) && I3LISTEN(ch)[0] != '\0') ? I3LISTEN(ch) : */"None"); // prool fool
	return;
    }

    if (!strcasecmp(argument, "all")) {
	for (channel = first_I3chan; channel; channel = channel->next) {
	    if (!channel->local_name || channel->local_name[0] == '\0')
		continue;
#if 0 // prool fool
	    if (I3PERM(ch) >= channel->i3perm && !I3_hasname(I3LISTEN(ch), channel->local_name))
		I3_flagchan(&I3LISTEN(ch), channel->local_name);
#endif
	}
	i3_printf(ch, "%%^YELLOW%%^You are now listening to all available I3 channels.%%^RESET%%^\r\n");
	return;
    }

    if (!strcasecmp(argument, "none")) {
	for (channel = first_I3chan; channel; channel = channel->next) {
	    if (!channel->local_name || channel->local_name[0] == '\0')
		continue;
#if 0 // prool fool
	    if (I3_hasname(I3LISTEN(ch), channel->local_name))
		I3_unflagchan(&I3LISTEN(ch), channel->local_name);
#endif
	}
	i3_printf(ch, "%%^YELLOW%%^You no longer listen to any available I3 channels.%%^RESET%%^\r\n");
	return;
    }

    if ((channel = find_I3_channel_by_localname(argument)) == NULL) {
	i3_printf(ch, "%%^YELLOW%%^Unknown channel.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3chanlist%%^YELLOW%%^ to get an overview of the channels available)%%^RESET%%^\r\n");
	return;
    }
#if 0 // prool fool
    if (I3_hasname(I3LISTEN(ch), channel->local_name)) {
	i3_printf(ch, "You no longer listen to %s\r\n", channel->local_name);
	I3_unflagchan(&I3LISTEN(ch), channel->local_name);
    } else {
	if (I3PERM(ch) < channel->i3perm) {
	    i3_printf(ch, "Channel %s is above your permission level.\r\n",
		      channel->local_name);
	    return;
	}
	i3_printf(ch, "You now listen to %s\r\n", channel->local_name);
	I3_flagchan(&I3LISTEN(ch), channel->local_name);
    }
    return;
#endif
}

I3_CMD(I3_deny_channel)
{
    char                                    vic_name[MAX_INPUT_LENGTH];
    CHAR_DATA                              *victim;
    I3_CHANNEL                             *channel;

    argument = i3one_argument(argument, vic_name);

    if (vic_name[0] == '\0' || !argument || argument[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3deny <person> <local channel name>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Usage: i3deny <person> [tell/beep/finger]%%^RESET%%^\r\n");
	return;
    }

    if (!(victim = I3_find_user(vic_name))) {
	i3_printf(ch, "No such person is currently online.\r\n");
	return;
    }
#if 0 // prool fool
    if (I3PERM(ch) <= I3PERM(victim)) {
	i3_printf(ch, "You cannot alter their settings.\r\n");
	return;
    }

    if (!strcasecmp(argument, "tell")) {
	if (!I3IS_SET(I3FLAG(victim), I3_DENYTELL)) {
	    I3SET_BIT(I3FLAG(victim), I3_DENYTELL);
	    i3_printf(ch, "%s can no longer use i3tells.\r\n", CH_I3NAME(victim));
	    return;
	}
	I3REMOVE_BIT(I3FLAG(victim), I3_DENYTELL);
	i3_printf(ch, "%s can use i3tells again.\r\n", CH_I3NAME(victim));
	return;
    }

    if (!strcasecmp(argument, "beep")) {
	if (!I3IS_SET(I3FLAG(victim), I3_DENYBEEP)) {
	    I3SET_BIT(I3FLAG(victim), I3_DENYBEEP);
	    i3_printf(ch, "%s can no longer use i3beeps.\r\n", CH_I3NAME(victim));
	    return;
	}
	I3REMOVE_BIT(I3FLAG(victim), I3_DENYBEEP);
	i3_printf(ch, "%s can use i3beeps again.\r\n", CH_I3NAME(victim));
	return;
    }

    if (!strcasecmp(argument, "finger")) {
	if (!I3IS_SET(I3FLAG(victim), I3_DENYFINGER)) {
	    I3SET_BIT(I3FLAG(victim), I3_DENYFINGER);
	    i3_printf(ch, "%s can no longer use i3fingers.\r\n", CH_I3NAME(victim));
	    return;
	}
	I3REMOVE_BIT(I3FLAG(victim), I3_DENYFINGER);
	i3_printf(ch, "%s can use i3fingers again.\r\n", CH_I3NAME(victim));
	return;
    }
#endif

    if (!(channel = find_I3_channel_by_localname(argument))) {
	i3_printf(ch, "%%^YELLOW%%^Unknown channel.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3chanlist%%^YELLOW%%^ to get an overview of the channels available)%%^RESET%%^\r\n");
	return;
    }
#if 0 // prool fool
    if (I3_hasname(I3DENY(ch), channel->local_name)) {
	i3_printf(ch, "%s can now listen to %s\r\n", CH_I3NAME(victim), channel->local_name);
	I3_unflagchan(&I3DENY(ch), channel->local_name);
    } else {
	i3_printf(ch, "%s can no longer listen to %s\r\n", CH_I3NAME(victim),
		  channel->local_name);
	I3_flagchan(&I3DENY(ch), channel->local_name);
    }
    return;
#endif
}

I3_CMD(I3_mudinfo)
{
    I3_MUD                                 *mud;

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "Usage: i3mudinfo <mudname>\r\n");
	return;
    }

    if (!(mud = find_I3_mud_by_name(argument))) {
	i3_printf(ch, "%%^YELLOW%%^Unknown mud.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    i3_printf(ch, "%%^WHITE%%^%%^BOLD%%^Information about %s%%^RESET%%^\r\n\r\n", mud->name);
    if (mud->status == 0)
	i3_printf(ch, "%%^WHITE%%^Status     : Currently down%%^RESET%%^\r\n");
    else if (mud->status > 0)
	i3_printf(ch, "%%^WHITE%%^Status     : Currently rebooting, back in %d seconds%%^RESET%%^\r\n",
		  mud->status);
    i3_printf(ch, "%%^WHITE%%^MUD port   : %s %d%%^RESET%%^\r\n", mud->ipaddress, mud->player_port);
    i3_printf(ch, "%%^WHITE%%^Base mudlib: %s%%^RESET%%^\r\n", mud->base_mudlib);
    i3_printf(ch, "%%^WHITE%%^Mudlib     : %s%%^RESET%%^\r\n", mud->mudlib);
    i3_printf(ch, "%%^WHITE%%^Driver     : %s%%^RESET%%^\r\n", mud->driver);
    i3_printf(ch, "%%^WHITE%%^Type       : %s%%^RESET%%^\r\n", mud->mud_type);
    i3_printf(ch, "%%^WHITE%%^Open status: %s%%^RESET%%^\r\n", mud->open_status);
    i3_printf(ch, "%%^WHITE%%^Admin      : %s%%^RESET%%^\r\n", mud->admin_email);
    if (mud->web)
	i3_printf(ch, "%%^WHITE%%^URL        : %s%%^RESET%%^\r\n", mud->web);
    if (mud->web_wrong && !mud->web)
	i3_printf(ch, "%%^WHITE%%^URL        : %s%%^RESET%%^\r\n", mud->web_wrong);
    if (mud->daemon)
	i3_printf(ch, "%%^WHITE%%^Daemon     : %s%%^RESET%%^\r\n", mud->daemon);
    if (mud->time)
	i3_printf(ch, "%%^WHITE%%^Time       : %s%%^RESET%%^\r\n", mud->time);
    if (mud->banner)
	i3_printf(ch, "%%^WHITE%%^Banner:%%^RESET%%^\r\n%s\r\n", mud->banner);

    i3_printf(ch, "%%^WHITE%%^Supports   : ");
    if (mud->tell)
	i3_printf(ch, "%%^WHITE%%^tell, ");
    if (mud->beep)
	i3_printf(ch, "%%^WHITE%%^beep, ");
    if (mud->emoteto)
	i3_printf(ch, "%%^WHITE%%^emoteto, ");
    if (mud->who)
	i3_printf(ch, "%%^WHITE%%^who, ");
    if (mud->finger)
	i3_printf(ch, "%%^WHITE%%^finger, ");
    if (mud->locate)
	i3_printf(ch, "%%^WHITE%%^locate, ");
    if (mud->channel)
	i3_printf(ch, "%%^WHITE%%^channel, ");
    if (mud->news)
	i3_printf(ch, "%%^WHITE%%^news, ");
    if (mud->mail)
	i3_printf(ch, "%%^WHITE%%^mail, ");
    if (mud->file)
	i3_printf(ch, "%%^WHITE%%^file, ");
    if (mud->auth)
	i3_printf(ch, "%%^WHITE%%^auth, ");
    if (mud->ucache)
	i3_printf(ch, "%%^WHITE%%^ucache, ");
    i3_printf(ch, "%%^RESET%%^\r\n");

    i3_printf(ch, "%%^WHITE%%^Supports   : ");
    if (mud->smtp)
	i3_printf(ch, "%%^WHITE%%^smtp (port %d), ", mud->smtp);
    if (mud->http)
	i3_printf(ch, "%%^WHITE%%^http (port %d), ", mud->http);
    if (mud->ftp)
	i3_printf(ch, "%%^WHITE%%^ftp  (port %d), ", mud->ftp);
    if (mud->pop3)
	i3_printf(ch, "%%^WHITE%%^pop3 (port %d), ", mud->pop3);
    if (mud->nntp)
	i3_printf(ch, "%%^WHITE%%^nntp (port %d), ", mud->nntp);
    if (mud->rcp)
	i3_printf(ch, "%%^WHITE%%^rcp  (port %d), ", mud->rcp);
    if (mud->amrcp)
	i3_printf(ch, "%%^WHITE%%^amrcp (port %d), ", mud->amrcp);
    i3_printf(ch, "%%^RESET%%^\r\n");
}

I3_CMD(I3_chanlayout)
{
    I3_CHANNEL                             *channel = NULL;
    char                                    arg1[MAX_INPUT_LENGTH],
                                            arg2[MAX_INPUT_LENGTH];

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3chanlayout <localchannel|all> <layout> <format...>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Layout can be one of these: layout_e layout_m%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Format can be any way you want it to look, provided you have the proper number of %%s tags in it.%%^RESET%%^\r\n");
	return;
    }

    argument = i3one_argument(argument, arg1);
    argument = i3one_argument(argument, arg2);

    if (arg1[0] == '\0') {
	I3_chanlayout(ch, "");
	return;
    }
    if (arg2[0] == '\0') {
	I3_chanlayout(ch, "");
	return;
    }
    if (!argument || argument[0] == '\0') {
	I3_chanlayout(ch, "");
	return;
    }

    if (!(channel = find_I3_channel_by_localname(arg1))) {
	i3_printf(ch, "%%^YELLOW%%^Unknown channel.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3chanlist%%^YELLOW%%^ to get an overview of the channels available)%%^RESET%%^\r\n");
	return;
    }

    if (!strcasecmp(arg2, "layout_e")) {
	if (!verify_i3layout(argument, 2)) {
	    i3_printf(ch, "Incorrect format for layout_e. You need exactly 2 %%s's.\r\n");
	    return;
	}
	I3STRFREE(channel->layout_e);
	channel->layout_e = I3STRALLOC(argument);
	i3_printf(ch, "Channel layout_e changed.\r\n");
	I3_write_channel_config();
	return;
    }

    if (!strcasecmp(arg2, "layout_m")) {
	if (!verify_i3layout(argument, 4)) {
	    i3_printf(ch, "Incorrect format for layout_m. You need exactly 4 %%s's.\r\n");
	    return;
	}
	I3STRFREE(channel->layout_m);
	channel->layout_m = I3STRALLOC(argument);
	i3_printf(ch, "Channel layout_m changed.\r\n");
	I3_write_channel_config();
	return;
    }
    I3_chanlayout(ch, "");
    return;
}

I3_CMD(I3_bancmd)
{
    I3_BAN                                 *temp;

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^GREEN%%^%%^BOLD%%^The mud currently has the following ban list:%%^RESET%%^\r\n\r\n");

	if (!first_i3ban)
	    i3_printf(ch, "%%^YELLOW%%^Nothing%%^RESET%%^\r\n");
	else {
	    for (temp = first_i3ban; temp; temp = temp->next)
		i3_printf(ch, "%%^YELLOW%%^\t  - %s%%^RESET%%^\r\n", temp->name);
	}
	i3_printf(ch, "\r\n%%^YELLOW%%^To add a ban, just specify a target. Suggested targets being user@mud or IP:Port\r\n");
	i3_printf(ch, "%%^YELLOW%%^User@mud bans can also have wildcard specifiers, such as *@Mud or User@*%%^RESET%%^\r\n");
	return;
    }

    if (!fnmatch(argument, this_i3mud->name, 0)) {
	i3_printf(ch, "%%^YELLOW%%^You don't really want to do that....%%^RESET%%^\r\n");
	return;
    }

    for (temp = first_i3ban; temp; temp = temp->next) {
	if (!strcasecmp(temp->name, argument)) {
	    I3STRFREE(temp->name);
	    I3UNLINK(temp, first_i3ban, last_i3ban, next, prev);
	    I3DISPOSE(temp);
	    I3_write_bans();
	    i3_printf(ch, "%%^YELLOW%%^The mud no longer bans %s.%%^RESET%%^\r\n", argument);
	    return;
	}
    }
    I3CREATE(temp, I3_BAN, 1);
    temp->name = I3STRALLOC(argument);
    I3LINK(temp, first_i3ban, last_i3ban, next, prev);
    I3_write_bans();
    i3_printf(ch, "%%^YELLOW%%^The mud now bans all incoming traffic from %s.%%^RESET%%^\r\n", temp->name);
}

I3_CMD(I3_ignorecmd)
{
    I3_IGNORE                              *temp;
    char                                    buf[MAX_INPUT_LENGTH];

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^GREEN%%^%%^BOLD%%^You are currently ignoring the following:%%^RESET%%^\r\n\r\n");
#if 0 // prool fool
	if (!FIRST_I3IGNORE(ch)) {
	    i3_printf(ch, "%%^YELLOW%%^Nobody%%^RESET%%^\r\n\r\n");
	    i3_printf(ch, "%%^YELLOW%%^To add an ignore, just specify a target. Suggested targets being user@mud or IP:Port%%^RESET%%^\r\n");
	    i3_printf(ch, "%%^YELLOW%%^User@mud ignores can also have wildcard specifiers, such as *@Mud or User@*%%^RESET%%^\r\n");
	    return;
	}
	for (temp = FIRST_I3IGNORE(ch); temp; temp = temp->next)
	    i3_printf(ch, "%%^YELLOW%%^\t  - %s%%^RESET%%^\r\n", temp->name);
#endif
	return;
    }

    snprintf(buf, MAX_INPUT_LENGTH, "%s@%s", CH_I3NAME(ch), this_i3mud->name);
    if (!strcasecmp(buf, argument)) {
	i3_printf(ch, "%%^YELLOW%%^You don't really want to do that....%%^RESET%%^\r\n");
	return;
    }

    if (!fnmatch(argument, this_i3mud->name, 0)) {
	i3_printf(ch, "%%^YELLOW%%^Ignoring your own mud would be silly.%%^RESET%%^\r\n");
	return;
    }
#if 0 // prool fool	
    for (temp = FIRST_I3IGNORE(ch); temp; temp = temp->next) {
	if (!strcasecmp(temp->name, argument)) {
	    I3STRFREE(temp->name);
	    I3UNLINK(temp, FIRST_I3IGNORE(ch), LAST_I3IGNORE(ch), next, prev);
	    I3DISPOSE(temp);
	    i3_printf(ch, "%%^YELLOW%%^You are no longer ignoring %s.%%^RESET%%^\r\n", argument);
	    return;
	}
    }

    I3CREATE(temp, I3_IGNORE, 1);
    temp->name = I3STRALLOC(argument);
    I3LINK(temp, FIRST_I3IGNORE(ch), LAST_I3IGNORE(ch), next, prev);
    i3_printf(ch, "%%^YELLOW%%^You now ignore %s.%%^RESET%%^\r\n", temp->name);
#endif
}

I3_CMD(I3_invis)
{
#if 0 // prool fool
    if (I3INVIS(ch)) {
	I3REMOVE_BIT(I3FLAG(ch), I3_INVIS);
	i3_printf(ch, "You are now i3visible.\r\n");
    } else {
	I3SET_BIT(I3FLAG(ch), I3_INVIS);
	i3_printf(ch, "You are now i3invisible.\r\n");
    }
    return;
#endif
}

I3_CMD(I3_debug)
{
    packetdebug = !packetdebug;

    if (packetdebug)
	i3_printf(ch, "Packet debugging enabled.\r\n");
    else
	i3_printf(ch, "Packet debugging disabled.\r\n");

    return;
}

I3_CMD(I3_send_user_req)
{
    char                                    user[MAX_INPUT_LENGTH],
                                            mud[MAX_INPUT_LENGTH];
    char                                   *ps;
    I3_MUD                                 *pmud;

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^YELLOW%%^Query who at which mud?%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }
    if (!(ps = (char *)strchr(argument, '@'))) { // prool
	i3_printf(ch, "%%^YELLOW%%^You should specify a person and a mud.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    ps[0] = '\0';
    i3strlcpy(user, argument, MAX_INPUT_LENGTH);
    i3strlcpy(mud, ps + 1, MAX_INPUT_LENGTH);

    if (user[0] == '\0' || mud[0] == '\0') {
	i3_printf(ch, "%%^YELLOW%%^You should specify a person and a mud.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    if (!(pmud = find_I3_mud_by_name(mud))) {
	i3_printf(ch, "%%^YELLOW%%^No such mud known.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    if (pmud->status >= 0) {
	i3_printf(ch, "%s is marked as down.\r\n", pmud->name);
	return;
    }

    I3_send_chan_user_req(pmud->name, user);
    return;
}

I3_CMD(I3_admin_channel)
{
    I3_CHANNEL                             *channel = NULL;
    char                                    arg1[MAX_INPUT_LENGTH],
                                            arg2[MAX_INPUT_LENGTH],
                                            buf[MAX_STRING_LENGTH];

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3adminchan <localchannel> <add|remove> <mudname>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Usage: i3adminchan <localchannel> list%%^RESET%%^\r\n");
	return;
    }
    argument = i3one_argument(argument, arg1);
    argument = i3one_argument(argument, arg2);

    if (arg1[0] == '\0') {
	I3_admin_channel(ch, "");
	return;
    }

    if (!(channel = find_I3_channel_by_localname(arg1))) {
	i3_printf(ch, "No such channel with that name here.\r\n");
	return;
    }

    if (arg2[0] == '\0') {
	I3_admin_channel(ch, "");
	return;
    }

    if (!strcasecmp(arg2, "list")) {
	I3_send_channel_adminlist(ch, channel->I3_name);
	i3_printf(ch, "Sending request for administrative list.\r\n");
	return;
    }

    if (!argument || argument[0] == '\0') {
	I3_admin_channel(ch, "");
	return;
    }

    if (!strcasecmp(arg2, "add")) {
	snprintf(buf, MAX_STRING_LENGTH, "({\"%s\",}),({}),", argument);
	I3_send_channel_admin(ch, channel->I3_name, buf);
	i3_printf(ch, "Sending administrative list addition.\r\n");
	return;
    }

    if (!strcasecmp(arg2, "remove")) {
	snprintf(buf, MAX_STRING_LENGTH, "({}),({\"%s\",}),", argument);
	I3_send_channel_admin(ch, channel->I3_name, buf);
	i3_printf(ch, "Sending administrative list removal.\r\n");
	return;
    }
    I3_admin_channel(ch, "");
    return;
}

I3_CMD(I3_disconnect)
{
    if (!i3_is_connected()) {
	i3_printf(ch, "The MUD isn't connected to the Intermud-3 router.\r\n");
	return;
    }

    i3_printf(ch, "Disconnecting from Intermud-3 router.\r\n");

    i3_shutdown(0, ch);
    return;
}

I3_CMD(I3_connect)
{
    ROUTER_DATA                            *router;

    if (i3_is_connected()) {
	i3_printf(ch, "The MUD is already connected to Intermud-3 router %s\r\n",
		  I3_ROUTER_NAME);
	return;
    }

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "Connecting to Intermud-3\r\n");
	router_connect(NULL, TRUE, this_i3mud->player_port, FALSE);
	return;
    }

    for (router = first_router; router; router = router->next) {
	if (!strcasecmp(router->name, argument)) {
	    router->reconattempts = 0;
	    i3_printf(ch, "Connecting to Intermud-3 router %s\r\n", argument);
	    router_connect(argument, TRUE, this_i3mud->player_port, FALSE);
	    return;
	}
    }

    i3_printf(ch, "%s is not configured as a router for this mud.\r\n", argument);
    i3_printf(ch, "If you wish to add it, use the i3router command to provide its information.\r\n");
    return;
}

I3_CMD(I3_reload)
{
    int                                     mudport = this_i3mud->player_port;

    if (i3_is_connected()) {
	i3_printf(ch, "Disconnecting from I3 router...\r\n");
	i3_shutdown(0, ch);
    }
    i3_printf(ch, "Reloading I3 configuration...\r\n");
    if (I3_read_config(mudport)) {
	i3_printf(ch, "Reconnecting to I3 router...\r\n");
	router_connect(NULL, FALSE, this_i3mud->player_port, FALSE);
    }
    i3_printf(ch, "Done!\r\n");
    return;
}

I3_CMD(I3_addchan)
{
    I3_CHANNEL                             *channel;
    char                                    arg[MAX_INPUT_LENGTH],
                                            arg2[MAX_INPUT_LENGTH],
                                            buf[MAX_STRING_LENGTH];
    int                                     type,
                                            x;

    argument = i3one_argument(argument, arg);
    argument = i3one_argument(argument, arg2);

    if (!argument || argument[0] == '\0' || arg[0] == '\0' || arg2[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3addchan <channelname> <localname> <type>%%^RESET%%^\r\n\r\n");
	i3_printf(ch, "%%^WHITE%%^Channelname should be the name seen on 'chanlist all'%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Localname should be the local name you want it listed as.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Type can be one of the following:%%^RESET%%^\r\n\r\n");
	i3_printf(ch, "%%^WHITE%%^0: selectively banned%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^1: selectively admitted%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^2: filtered - valid for selectively admitted ONLY%%^RESET%%^\r\n");
	return;
    }

    if ((channel = find_I3_channel_by_name(arg)) != NULL) {
	i3_printf(ch, "%%^RED%%^%%^BOLD%%^%s is already hosted by %s.%%^RESET%%^\r\n", channel->I3_name, channel->host_mud);
	return;
    }

    if ((channel = find_I3_channel_by_localname(arg2)) != NULL) {
	i3_printf(ch, "%%^RED%%^%%^BOLD%%^Channel %s@%s is already locally configured as %s.%%^RESET%%^\r\n",
		  channel->I3_name, channel->host_mud, channel->local_name);
	return;
    }

    if (!isdigit(argument[0])) {
	i3_printf(ch, "%%^RED%%^%%^BOLD%%^Invalid type. Must be numerical.%%^RESET%%^\r\n");
	I3_addchan(ch, "");
	return;
    }

    type = atoi(argument);
    if (type < 0 || type > 2) {
	i3_printf(ch, "%%^RED%%^%%^BOLD%%^Invalid channel type.%%^RESET%%^\r\n");
	I3_addchan(ch, "");
	return;
    }

    i3_printf(ch, "%%^GREEN%%^%%^BOLD%%^Adding channel to router: %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n", arg);
    I3_send_channel_add(ch, arg, type);

    I3CREATE(channel, I3_CHANNEL, 1);
    channel->I3_name = I3STRALLOC(arg);
    channel->host_mud = I3STRALLOC(this_i3mud->name);
    channel->local_name = I3STRALLOC(arg2);
    channel->i3perm = I3PERM_ADMIN;
    channel->layout_m = I3STRALLOC("%%^RED%%^%%^BOLD%%^[%%^WHITE%%^%%^BOLD%%^%s%%^RED%%^%%^BOLD%%^] %%^CYAN%%^%%^BOLD%%^%s@%s: %%^CYAN%%^%s");
    channel->layout_e = I3STRALLOC("%%^RED%%^%%^BOLD%%^[%%^WHITE%%^%%^BOLD%%^%s%%^RED%%^%%^BOLD%%^] %%^CYAN%%^%s");
    for (x = 0; x < MAX_I3HISTORY; x++)
	channel->history[x] = NULL;
    I3LINK(channel, first_I3chan, last_I3chan, next, prev);

    if (type != 0) {
	snprintf(buf, MAX_STRING_LENGTH, "({\"%s\",}),({}),", this_i3mud->name);
	I3_send_channel_admin(ch, channel->I3_name, buf);
	i3_printf(ch, "%%^GREEN%%^%%^BOLD%%^Sending command to add %s to the invite list.%%^RESET%%^\r\n", this_i3mud->name);
    }

    i3_printf(ch, "%%^YELLOW%%^%s@%s %%^WHITE%%^%%^BOLD%%^is now locally known as %%^YELLOW%%^%s%%^RESET%%^\r\n", channel->I3_name,
	      channel->host_mud, channel->local_name);
    I3_send_channel_listen(channel, TRUE);
    I3_write_channel_config();

    return;
}

I3_CMD(I3_removechan)
{
    I3_CHANNEL                             *channel = NULL;

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3removechan <channel>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Channelname should be the name seen on 'chanlist all'%%^RESET%%^\r\n");
	return;
    }

    if ((channel = find_I3_channel_by_name(argument)) == NULL) {
	i3_printf(ch, "%%^RED%%^%%^BOLD%%^No channel by that name exists.%%^RESET%%^\r\n");
	return;
    }

    if (strcasecmp(channel->host_mud, this_i3mud->name)) {
	i3_printf(ch, "%%^RED%%^%%^BOLD%%^%s does not host this channel and cannot remove it.%%^RESET%%^\r\n", this_i3mud->name);
	return;
    }

    i3_printf(ch, "%%^YELLOW%%^Removing channel from router: %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n", channel->I3_name);
    I3_send_channel_remove(ch, channel);

    i3_printf(ch, "%%^RED%%^%%^BOLD%%^Destroying local channel entry for %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n", channel->I3_name);
    destroy_I3_channel(channel);
    I3_write_channel_config();

    return;
}

I3_CMD(I3_setconfig)
{
    char                                    arg[MAX_INPUT_LENGTH];

    argument = i3one_argument(argument, arg);

    if (arg[0] == '\0') {
	i3_printf(ch, "%%^GREEN%%^%%^BOLD%%^Configuration info for your mud. Changes save when edited.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^GREEN%%^%%^BOLD%%^You can set the following:%%^RESET%%^\r\n\r\n");
	i3_printf(ch, "%%^WHITE%%^Show       : %%^GREEN%%^%%^BOLD%%^Displays your current congfiguration.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Autoconnect: %%^GREEN%%^%%^BOLD%%^A toggle. Either on or off. Your mud will connect automatically with it on.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Mudname    : %%^GREEN%%^%%^BOLD%%^The name you want displayed on I3 for your mud.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Telnet     : %%^GREEN%%^%%^BOLD%%^The telnet address for your mud. Do not include the port number.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Web        : %%^GREEN%%^%%^BOLD%%^The website address for your mud. In the form of: www.address.com%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Email      : %%^GREEN%%^%%^BOLD%%^The email address of your mud's administrator. Needs to be valid!!%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Status     : %%^GREEN%%^%%^BOLD%%^The open status of your mud. IE: Public, Development, etc.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Mudtype    : %%^GREEN%%^%%^BOLD%%^What you call the basic type of your codebase.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Mudlib     : %%^GREEN%%^%%^BOLD%%^What you call the current version of your codebase.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Minlevel   : %%^GREEN%%^%%^BOLD%%^Minimum level at which I3 will recognize your players.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Immlevel   : %%^GREEN%%^%%^BOLD%%^The level at which immortal commands become available.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Adminlevel : %%^GREEN%%^%%^BOLD%%^The level at which administrative commands become available.%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Implevel   : %%^GREEN%%^%%^BOLD%%^The level at which implementor commands become available.%%^RESET%%^\r\n");
	return;
    }

    if (!strcasecmp(arg, "show")) {
	i3_printf(ch, "%%^WHITE%%^Mudname       : %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", this_i3mud->name);
	i3_printf(ch, "%%^WHITE%%^Autoconnect   : %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", this_i3mud->autoconnect == TRUE ? "Enabled" : "Disabled");
	i3_printf(ch, "%%^WHITE%%^Telnet        : %%^GREEN%%^%%^BOLD%%^%s:%d%%^RESET%%^\r\n", this_i3mud->telnet, this_i3mud->player_port);
	i3_printf(ch, "%%^WHITE%%^Web           : %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", this_i3mud->web);
	i3_printf(ch, "%%^WHITE%%^Email         : %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", this_i3mud->admin_email);
	i3_printf(ch, "%%^WHITE%%^Status        : %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", this_i3mud->open_status);
	i3_printf(ch, "%%^WHITE%%^Mudtype       : %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", this_i3mud->mud_type);
	i3_printf(ch, "%%^WHITE%%^Mudlib        : %%^GREEN%%^%%^BOLD%%^%s%%^RESET%%^\r\n", this_i3mud->mudlib);
	i3_printf(ch, "%%^WHITE%%^Minlevel      : %%^GREEN%%^%%^BOLD%%^%d%%^RESET%%^\r\n", this_i3mud->minlevel);
	i3_printf(ch, "%%^WHITE%%^Immlevel      : %%^GREEN%%^%%^BOLD%%^%d%%^RESET%%^\r\n", this_i3mud->immlevel);
	i3_printf(ch, "%%^WHITE%%^Adminlevel    : %%^GREEN%%^%%^BOLD%%^%d%%^RESET%%^\r\n", this_i3mud->adminlevel);
	i3_printf(ch, "%%^WHITE%%^Implevel      : %%^GREEN%%^%%^BOLD%%^%d%%^RESET%%^\r\n", this_i3mud->implevel);
	return;
    }

    if (!strcasecmp(arg, "autoconnect")) {
	this_i3mud->autoconnect = !this_i3mud->autoconnect;

	if (this_i3mud->autoconnect)
	    i3_printf(ch, "Autoconnect enabled.\r\n");
	else
	    i3_printf(ch, "Autoconnect disabled.\r\n");
	I3_saveconfig();
	return;
    }

    if (!argument || argument[0] == '\0') {
	I3_setconfig(ch, "");
	return;
    }
#if 0 // prool fool
    if (!strcasecmp(arg, "implevel") && I3PERM(ch) == I3PERM_IMP) {
	int                                     value = atoi(argument);

	this_i3mud->implevel = value;
	I3_saveconfig();
	i3_printf(ch, "Implementor level changed to %d\r\n", value);
	return;
    }
#endif

    if (!strcasecmp(arg, "adminlevel")) {
	int                                     value = atoi(argument);

	this_i3mud->adminlevel = value;
	I3_saveconfig();
	i3_printf(ch, "Admin level changed to %d\r\n", value);
	return;
    }

    if (!strcasecmp(arg, "immlevel")) {
	int                                     value = atoi(argument);

	this_i3mud->immlevel = value;
	I3_saveconfig();
	i3_printf(ch, "Immortal level changed to %d\r\n", value);
	return;
    }

    if (!strcasecmp(arg, "minlevel")) {
	int                                     value = atoi(argument);

	this_i3mud->minlevel = value;
	I3_saveconfig();
	i3_printf(ch, "Minimum level changed to %d\r\n", value);
	return;
    }

    if (i3_is_connected()) {
	i3_printf(ch, "%s may not be changed while the mud is connected.\r\n", arg);
	return;
    }

    if (!strcasecmp(arg, "mudname")) {
	I3STRFREE(this_i3mud->name);
	this_i3mud->name = I3STRALLOC(argument);
	// I3_THISMUD = argument;
	unlink(I3_PASSWORD_FILE);
	I3_saveconfig();
	i3_printf(ch, "Mud name changed to %s\r\n", argument);
	return;
    }

    if (!strcasecmp(arg, "telnet")) {
	I3STRFREE(this_i3mud->telnet);
	this_i3mud->telnet = I3STRALLOC(argument);
	I3_saveconfig();
	i3_printf(ch, "Telnet address changed to %s:%d\r\n", argument, this_i3mud->player_port);
	return;
    }

    if (!strcasecmp(arg, "web")) {
	I3STRFREE(this_i3mud->web);
	this_i3mud->web = I3STRALLOC(argument);
	I3_saveconfig();
	i3_printf(ch, "Website changed to %s\r\n", argument);
	return;
    }

    if (!strcasecmp(arg, "email")) {
	I3STRFREE(this_i3mud->admin_email);
	this_i3mud->admin_email = I3STRALLOC(argument);
	I3_saveconfig();
	i3_printf(ch, "Admin email changed to %s\r\n", argument);
	return;
    }

    if (!strcasecmp(arg, "status")) {
	I3STRFREE(this_i3mud->open_status);
	this_i3mud->open_status = I3STRALLOC(argument);
	I3_saveconfig();
	i3_printf(ch, "Status changed to %s\r\n", argument);
	return;
    }

    if (!strcasecmp(arg, "mudlib")) {
	I3STRFREE(this_i3mud->mudlib);
	this_i3mud->mudlib = I3STRALLOC(argument);
	I3_saveconfig();
	i3_printf(ch, "Mudlib changed to %s\r\n", argument);
	return;
    }
    if (!strcasecmp(arg, "mudtype")) {
	I3STRFREE(this_i3mud->mud_type);
	this_i3mud->mud_type = I3STRALLOC(argument);
	I3_saveconfig();
	i3_printf(ch, "Mudtype changed to %s\r\n", argument);
	return;
    }

    I3_setconfig(ch, "");
    return;
}

I3_CMD(I3_permstats)
{
    CHAR_DATA                              *victim;

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "Usage: i3perms <user>\r\n");
	return;
    }

    if (!(victim = I3_find_user(argument))) {
	i3_printf(ch, "No such person is currently online.\r\n");
	return;
    }
#if 0 // prool fool
    if (I3PERM(victim) < 0 || I3PERM(victim) > I3PERM_IMP) {
	i3_printf(ch, "%%^RED%%^%%^BOLD%%^%s has an invalid permission setting!%%^RESET%%^\r\n", CH_I3NAME(victim));
	return;
    }

    i3_printf(ch, "%%^GREEN%%^%%^BOLD%%^Permissions for %s: %s%%^RESET%%^\r\n", CH_I3NAME(victim),
	      perm_names[I3PERM(victim)]);
    i3_printf(ch, "%%^GREEN%%^These permissions were obtained %s.%%^RESET%%^\r\n",
	      I3IS_SET(I3FLAG(victim),
		       I3_PERMOVERRIDE) ? "manually via i3permset" : "automatically by level");
    return;
#endif
}

I3_CMD(I3_permset)
{
    CHAR_DATA                              *victim;
    char                                    arg[MAX_INPUT_LENGTH];
    int                                     permvalue;

    argument = i3one_argument(argument, arg);

    if (arg[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3permset <user> <permission>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Permission can be one of: None, Mort, Imm, Admin, Imp%%^RESET%%^\r\n");
	return;
    }

    if (!(victim = I3_find_user(arg))) {
	i3_printf(ch, "No such person is currently online.\r\n");
	return;
    }

    if (!strcasecmp(argument, "override"))
	permvalue = -1;
    else {
	permvalue = get_permvalue(argument);
#if 0 // prool fool
	if (!i3check_permissions(ch, permvalue, I3PERM(victim), TRUE))
	    return;
#endif
    }

    /*
     * Just something to avoid looping through the channel clean-up --Xorith 
     */
#if 0 // prool fool
    if (I3PERM(victim) == permvalue) {
	i3_printf(ch, "%s already has a permission level of %s.\r\n", CH_I3NAME(victim),
		  perm_names[permvalue]);
	return;
    }

    if (permvalue == -1) {
	I3REMOVE_BIT(I3FLAG(victim), I3_PERMOVERRIDE);
	i3_printf(ch, "%%^YELLOW%%^Permission flag override has been removed from %s%%^RESET%%^\r\n",
		  CH_I3NAME(victim));
	return;
    }

    I3PERM(victim) = permvalue;
    I3SET_BIT(I3FLAG(victim), I3_PERMOVERRIDE);
#endif
    i3_printf(ch, "%%^YELLOW%%^Permission level for %s has been changed to %s%%^RESET%%^\r\n", CH_I3NAME(victim),
	      perm_names[permvalue]);
    /*
     * Channel Clean-Up added by Xorith 9-24-03 
     */
    /*
     * Note: Let's not clean up I3_DENY for a player. Never know... 
     */
#if 0 // prool fool
    if (I3LISTEN(victim) != NULL) {
	I3_CHANNEL                             *channel = NULL;
	const char                             *channels = I3LISTEN(victim);

	while (1) {
	    if (channels[0] == '\0')
		break;
	    channels = i3one_argument(channels, arg);

	    if (!(channel = find_I3_channel_by_localname(arg)))
		I3_unflagchan(&I3LISTEN(victim), arg);
	    if (channel && I3PERM(victim) < channel->i3perm) {
		I3_unflagchan(&I3LISTEN(victim), arg);
		i3_printf(ch,
			  "%%^WHITE%%^%%^BOLD%%^Removing '%s' level channel: '%s', exceeding new permission of '%s'%%^RESET%%^\r\n",
			  perm_names[channel->i3perm], channel->local_name,
			  perm_names[I3PERM(victim)]);
	    }
	}
    }
#endif
    return;
}

I3_CMD(I3_who)
{
    I3_MUD                                 *mud;

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "Usage: i3who <mudname>\r\n");
	return;
    }

    if (!(mud = find_I3_mud_by_name(argument))) {
	i3_printf(ch, "%%^YELLOW%%^No such mud known.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    if (mud->status >= 0) {
	i3_printf(ch, "%s is marked as down.\r\n", mud->name);
	return;
    }

    if (mud->who == 0)
	i3_printf(ch, "%s does not support the 'who' command. Sending anyway.\r\n", mud->name);

    I3_send_who(ch, mud->name);
}

I3_CMD(I3_locate)
{
    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "Usage: i3locate <person>\r\n");
	return;
    }
    I3_send_locate(ch, argument);
}

I3_CMD(I3_finger)
{
    char                                    user[MAX_INPUT_LENGTH],
                                            mud[MAX_INPUT_LENGTH];
    char                                   *ps;
    I3_MUD                                 *pmud;
#if 0 // prool fool
    if (I3IS_SET(I3FLAG(ch), I3_DENYFINGER)) {
	i3_printf(ch, "You are not allowed to use i3finger.\r\n");
	return;
    }
#endif

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3finger <user@mud>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Usage: i3finger privacy%%^RESET%%^\r\n");
	return;
    }
#if 0 // prool fool
    if (!strcasecmp(argument, "privacy")) {
	if (!I3IS_SET(I3FLAG(ch), I3_PRIVACY)) {
	    I3SET_BIT(I3FLAG(ch), I3_PRIVACY);
	    i3_printf(ch, "I3 finger privacy flag set.\r\n");
	    return;
	}
	I3REMOVE_BIT(I3FLAG(ch), I3_PRIVACY);
	i3_printf(ch, "I3 finger privacy flag removed.\r\n");
	return;
    }

    if (I3ISINVIS(ch)) {
	i3_printf(ch, "You are invisible.\r\n");
	return;
    }
#endif
    if ((ps = (char *)strchr(argument, '@')) == NULL) { // prool
	i3_printf(ch, "%%^YELLOW%%^You should specify a person and a mud.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    ps[0] = '\0';
    i3strlcpy(user, argument, MAX_INPUT_LENGTH);
    i3strlcpy(mud, ps + 1, MAX_INPUT_LENGTH);

    if (user[0] == '\0' || mud[0] == '\0') {
	i3_printf(ch, "%%^YELLOW%%^You should specify a person and a mud.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    if (!(pmud = find_I3_mud_by_name(mud))) {
	i3_printf(ch, "%%^YELLOW%%^No such mud known.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    if (!strcasecmp(this_i3mud->name, pmud->name)) {
	i3_printf(ch, "Use your mud's own internal system for that.\r\n");
	return;
    }

    if (pmud->status >= 0) {
	i3_printf(ch, "%s is marked as down.\r\n", pmud->name);
	return;
    }

    if (pmud->finger == 0)
	i3_printf(ch, "%s does not support the 'finger' command. Sending anyway.\r\n",
		  pmud->name);

    I3_send_finger(ch, user, pmud->name);
}

I3_CMD(I3_emote)
{
    char                                    to[MAX_INPUT_LENGTH],
                                           *ps;
    char                                    mud[MAX_INPUT_LENGTH];
    I3_MUD                                 *pmud;
#if 0 // prool fool
    if (I3ISINVIS(ch)) {
	i3_printf(ch, "You are invisible.\r\n");
	return;
    }
#endif

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "Usage: i3emoteto <person@mud> <emote message>\r\n");
	return;
    }

    argument = i3one_argument(argument, to);
    ps = strchr(to, '@');

    if (to[0] == '\0' || argument[0] == '\0' || ps == NULL) {
	i3_printf(ch, "%%^YELLOW%%^You should specify a person and a mud.%%^RESET%%^\r\n" "(use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    ps[0] = '\0';
    ps++;
    i3strlcpy(mud, ps, MAX_INPUT_LENGTH);

    if (!(pmud = find_I3_mud_by_name(mud))) {
	i3_printf(ch, "%%^YELLOW%%^No such mud known.%%^RESET%%^\r\n" "( use %%^WHITE%%^%%^BOLD%%^i3mudlist%%^YELLOW%%^ to get an overview of the muds available)%%^RESET%%^\r\n");
	return;
    }

    if (pmud->status >= 0) {
	i3_printf(ch, "%s is marked as down.\r\n", pmud->name);
	return;
    }

    if (pmud->emoteto == 0)
	i3_printf(ch, "%s does not support the 'emoteto' command. Sending anyway.\r\n",
		  pmud->name);

    I3_send_emoteto(ch, to, pmud, argument);
}

I3_CMD(I3_router)
{
    ROUTER_DATA                            *router;
    char                                    cmd[MAX_INPUT_LENGTH];

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3router add <router_name> <router_ip> <router_port>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Usage: i3router remove <router_name>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Usage: i3router list%%^RESET%%^\r\n");
	return;
    }
    argument = i3one_argument(argument, cmd);

    if (!strcasecmp(cmd, "list")) {
	i3_printf(ch, "%%^RED%%^%%^BOLD%%^The mud has the following routers configured:%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^%%^BOLD%%^Router Name     Router IP/DNS                  Router Port%%^RESET%%^\r\n");
	for (router = first_router; router; router = router->next)
	    i3_printf(ch, "%%^CYAN%%^%-15.15s %%^CYAN%%^%-30.30s %d%%^RESET%%^\r\n", router->name, router->ip,
		      router->port);
	return;
    }

    if (!argument || argument[0] == '\0') {
	I3_router(ch, "");
	return;
    }

    if (!strcasecmp(cmd, "remove")) {
	for (router = first_router; router; router = router->next) {
	    if (!strcasecmp(router->name, argument) || !strcasecmp(router->ip, argument)) {
		I3STRFREE(router->name);
		I3STRFREE(router->ip);
		I3UNLINK(router, first_router, last_router, next, prev);
		I3DISPOSE(router);
		i3_printf(ch, "%%^YELLOW%%^Router %%^WHITE%%^%%^BOLD%%^%s%%^YELLOW%%^ has been removed from your configuration.%%^RESET%%^\r\n",
			  argument);
		I3_saverouters();
		return;
	    }
	}
	i3_printf(ch, "%%^YELLOW%%^No router named %%^WHITE%%^%%^BOLD%%^%s%%^YELLOW%%^ exists in your configuration.%%^RESET%%^\r\n", argument);
	return;
    }

    if (!strcasecmp(cmd, "add")) {
	ROUTER_DATA                            *temp;
	char                                    rtname[MAX_INPUT_LENGTH];
	char                                    rtip[MAX_INPUT_LENGTH];
	int                                     rtport;

	argument = i3one_argument(argument, rtname);
	argument = i3one_argument(argument, rtip);

	if (rtname[0] == '\0' || rtip[0] == '\0' || !argument || argument[0] == '\0') {
	    I3_router(ch, "");
	    return;
	}

	if (rtname[0] != '*') {
	    i3_printf(ch, "%%^YELLOW%%^A router name must begin with a %%^WHITE%%^%%^BOLD%%^*%%^YELLOW%%^ to be valid.%%^RESET%%^\r\n");
	    return;
	}

	for (temp = first_router; temp; temp = temp->next) {
	    if (!strcasecmp(temp->name, rtname)) {
		i3_printf(ch, "%%^YELLOW%%^A router named %%^WHITE%%^%%^BOLD%%^%s%%^YELLOW%%^ is already in your configuration.%%^RESET%%^\r\n",
			  rtname);
		return;
	    }
	}

	if (!is_number(argument)) {
	    i3_printf(ch, "%%^YELLOW%%^Port must be a numerical value.%%^RESET%%^\r\n");
	    return;
	}

	rtport = atoi(argument);
	if (rtport < 1 || rtport > 65535) {
	    i3_printf(ch, "%%^YELLOW%%^Invalid port value specified.%%^RESET%%^\r\n");
	    return;
	}

	I3CREATE(router, ROUTER_DATA, 1);
	router->name = I3STRALLOC(rtname);
	router->ip = I3STRALLOC(rtip);
	router->port = rtport;
	router->reconattempts = 0;
	I3LINK(router, first_router, last_router, next, prev);

	i3_printf(ch, "%%^YELLOW%%^Router: %%^WHITE%%^%%^BOLD%%^%s %s %d%%^YELLOW%%^ has been added to your configuration.%%^RESET%%^\r\n",
		  router->name, router->ip, router->port);
	I3_saverouters();
	return;
    }
    I3_router(ch, "");
    return;
}

I3_CMD(I3_stats)
{
    I3_MUD                                 *mud;
    I3_CHANNEL                             *channel;
    int                                     mud_count = 0,
	chan_count = 0;

    for (mud = first_mud; mud; mud = mud->next)
	mud_count++;

    for (channel = first_I3chan; channel; channel = channel->next)
	chan_count++;

    i3_printf(ch, "%%^CYAN%%^General Statistics:%%^RESET%%^\r\n\r\n");
    i3_printf(ch, "%%^CYAN%%^Currently connected to: %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n",
	      i3_is_connected()? I3_ROUTER_NAME : "Nowhere!");
    if (i3_is_connected())
	i3_printf(ch, "%%^CYAN%%^Connected on descriptor: %%^WHITE%%^%%^BOLD%%^%d%%^RESET%%^\r\n", I3_socket);
    i3_printf(ch, "%%^CYAN%%^Bytes sent    : %%^WHITE%%^%%^BOLD%%^%ld%%^RESET%%^\r\n", bytes_sent);
    i3_printf(ch, "%%^CYAN%%^Bytes received: %%^WHITE%%^%%^BOLD%%^%ld%%^RESET%%^\r\n", bytes_received);
    i3_printf(ch, "%%^CYAN%%^Known muds    : %%^WHITE%%^%%^BOLD%%^%d%%^RESET%%^\r\n", mud_count);
    i3_printf(ch, "%%^CYAN%%^Known channels: %%^WHITE%%^%%^BOLD%%^%d%%^RESET%%^\r\n", chan_count);
    return;
}

I3_CMD(i3_help)
{
    I3_HELP_DATA                           *help;
    char                                    buf[MAX_STRING_LENGTH];
    int                                     col,
                                            perm;

    if (!argument || argument[0] == '\0') {
	i3strlcpy(buf, "%^GREEN%^Help is available for the following commands:%^RESET%^\r\n",
		  MAX_STRING_LENGTH);
	i3strlcat(buf, "%^GREEN%^%^BOLD%^---------------------------------------------%^RESET%^\r\n",
		  MAX_STRING_LENGTH);
#if 0 // prool fool
	for (perm = I3PERM_MORT; perm <= I3PERM(ch); perm++) {
	    col = 0;
	    snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf),
		     "%%^RESET%%^\r\n%%^GREEN%%^%s helps:%%^RESET%%^\r\n", perm_names[perm]);
	    for (help = first_i3_help; help; help = help->next) {
		if (help->level != perm)
		    continue;

		snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf), "%-15s",
			 help->name);
		if (++col % 6 == 0)
		    i3strlcat(buf, "%^RESET%^\r\n", MAX_STRING_LENGTH);
	    }
	    if (col % 6 != 0)
		i3strlcat(buf, "%^RESET%^\r\n", MAX_STRING_LENGTH);
	}
#endif
	i3send_to_pager(buf, ch);
	return;
    }

    for (help = first_i3_help; help; help = help->next) {
	if (!strcasecmp(help->name, argument)) {
	    if (!help->text || help->text[0] == '\0')
		i3_printf(ch, "%%^GREEN%%^No inforation available for topic %%^WHITE%%^%%^BOLD%%^%s%%^GREEN%%^.%%^RESET%%^\r\n", help->name);
	    else
		i3_printf(ch, "%%^GREEN%%^%s%%^RESET%%^\r\n", help->text);
	    return;
	}
    }
    i3_printf(ch, "%%^GREEN%%^No help exists for topic %%^WHITE%%^%%^BOLD%%^%s%%^GREEN%%^.%%^RESET%%^\r\n", argument);
    return;
}

I3_CMD(i3_hedit)
{
    I3_HELP_DATA                           *help;
    char                                    name[MAX_INPUT_LENGTH],
                                            cmd[MAX_INPUT_LENGTH];
    bool                                    found = FALSE;

    argument = i3one_argument(argument, name);
    argument = i3one_argument(argument, cmd);

    if (name[0] == '\0' || cmd[0] == '\0' || !argument || argument[0] == '\0') {
	i3_printf(ch, "%%^WHITE%%^Usage: i3hedit <topic> [name|perm] <field>%%^RESET%%^\r\n");
	i3_printf(ch, "%%^WHITE%%^Where <field> can be either name, or permission level.%%^RESET%%^\r\n");
	return;
    }

    for (help = first_i3_help; help; help = help->next) {
	if (!strcasecmp(help->name, name)) {
	    found = TRUE;
	    break;
	}
    }

    if (!found) {
	i3_printf(ch,
		  "%%^GREEN%%^No help exists for topic %%^WHITE%%^%%^BOLD%%^%s%%^GREEN%%^. You will need to add it to the helpfile manually.%%^RESET%%^\r\n",
		  name);
	return;
    }

    if (!strcasecmp(cmd, "name")) {
	i3_printf(ch, "%%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^has been renamed to %%^WHITE%%^%%^BOLD%%^%s.%%^RESET%%^\r\n", help->name, argument);
	I3STRFREE(help->name);
	help->name = I3STRALLOC(argument);
	I3_savehelps();
	return;
    }

    if (!strcasecmp(cmd, "perm")) {
	int                                     permvalue = get_permvalue(argument);

	if (!i3check_permissions(ch, permvalue, help->level, FALSE))
	    return;

	i3_printf(ch, "%%^GREEN%%^Permission level for %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^has been changed to %%^WHITE%%^%%^BOLD%%^%s.%%^RESET%%^\r\n", help->name,
		  perm_names[permvalue]);
	help->level = permvalue;
	I3_savehelps();
	return;
    }
    i3_hedit(ch, "");
    return;
}

I3_CMD(I3_other)
{
    I3_CMD_DATA                            *cmd;
    char                                    buf[MAX_STRING_LENGTH];
    int                                     col,
                                            perm;

    i3strlcpy(buf, "%%^GREEN%%^The following commands are available:%%^RESET%%^\r\n", MAX_STRING_LENGTH);
    i3strlcat(buf, "%%^GREEN%%^%%^BOLD%%^-------------------------------------%%^RESET%%^\r\n", MAX_STRING_LENGTH);
#if 0 // prool fool
    for (perm = I3PERM_MORT; perm <= I3PERM(ch); perm++) {
	col = 0;
	snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf), "%%^RESET%%^\r\n%%^GREEN%%^%s commands:%%^GREEN%%^%%^BOLD%%^\r\n",
		 perm_names[perm]);
	for (cmd = first_i3_command; cmd; cmd = cmd->next) {
	    if (cmd->level != perm)
		continue;

	    snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf), "%-15s", cmd->name);
	    if (++col % 6 == 0)
		i3strlcat(buf, "%%^RESET%%^\r\n%%^GREEN%%^%%^BOLD%%^", MAX_STRING_LENGTH);
	}
	if (col % 6 != 0)
            i3strlcat(buf, "%%^RESET%%^\r\n%%^GREEN%%^%%^BOLD%%^", MAX_STRING_LENGTH);
    }
#endif
    i3send_to_pager(buf, ch);
    i3send_to_pager
	("%%^RESET%%^\r\n%%^GREEN%%^For information about a specific command, see %%^WHITE%%^%%^BOLD%%^i3help <command>%%^GREEN%%^.%%^RESET%%^\r\n", ch);
    return;
}

I3_CMD(I3_afk)
{
#if 0 // prool fool
    if (I3IS_SET(I3FLAG(ch), I3_AFK)) {
	I3REMOVE_BIT(I3FLAG(ch), I3_AFK);
	i3_printf(ch, "You are no longer AFK to I3.\r\n");
    } else {
	I3SET_BIT(I3FLAG(ch), I3_AFK);
	i3_printf(ch, "You are now AFK to I3.\r\n");
    }
#endif
    return;
}

I3_CMD(I3_color)
{
#if 0 // prool fool
    if (I3IS_SET(I3FLAG(ch), I3_COLORFLAG)) {
	I3REMOVE_BIT(I3FLAG(ch), I3_COLORFLAG);
	i3_printf(ch, "I3 color is now off.\r\n");
    } else {
	I3SET_BIT(I3FLAG(ch), I3_COLORFLAG);
	i3_printf(ch, "%%^RED%%^%%^BOLD%%^I3 c%%^YELLOW%%^o%%^GREEN%%^%%^BOLD%%^l%%^BLUE%%^%%^BOLD%%^o%%^MAGENTA%%^%%^BOLD%%^r %%^RED%%^%%^BOLD%%^is now on. Enjoy :)%%^RESET%%^\r\n");
    }
#endif
    return;
}

I3_CMD(i3_cedit)
{
    I3_CMD_DATA                            *cmd,
                                           *tmp;
    I3_ALIAS                               *alias,
                                           *alias_next;
    char                                    name[MAX_INPUT_LENGTH],
                                            option[MAX_INPUT_LENGTH];
    bool                                    found = FALSE,
	aliasfound = FALSE;

    argument = i3one_argument(argument, name);
    argument = i3one_argument(argument, option);

    if (name[0] == '\0' || option[0] == '\0') {
	i3_printf(ch, "Usage: i3cedit <command> <create|delete|alias|rename|code|permission|connected> <field>.\r\n");
	return;
    }

    for (cmd = first_i3_command; cmd; cmd = cmd->next) {
	if (!strcasecmp(cmd->name, name)) {
	    found = TRUE;
	    break;
	}
	for (alias = cmd->first_alias; alias; alias = alias->next) {
	    if (!strcasecmp(alias->name, name))
		aliasfound = TRUE;
	}
    }

    if (!strcasecmp(option, "create")) {
	if (found) {
	    i3_printf(ch, "%%^GREEN%%^A command named %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^already exists.%%^RESET%%^\r\n", name);
	    return;
	}

	if (aliasfound) {
	    i3_printf(ch, "%%^GREEN%%^%s already exists as an alias for another command.%%^RESET%%^\r\n", name);
	    return;
	}

	I3CREATE(cmd, I3_CMD_DATA, 1);
	cmd->name = I3STRALLOC(name);
	//cmd->level = I3PERM(ch); // prool fool
	cmd->connected = FALSE;
	i3_printf(ch, "%%^GREEN%%^Command %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^created.%%^RESET%%^\r\n", cmd->name);
	if (argument && argument[0] != '\0') {
	    cmd->function = i3_function(argument);
	    if (cmd->function == NULL)
		i3_printf(ch, "%%^GREEN%%^Function %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^does not exist - set to NULL.%%^RESET%%^\r\n", argument);
	} else {
	    i3_printf(ch, "%%^GREEN%%^Function set to NULL.%%^RESET%%^\r\n");
	    cmd->function = NULL;
	}
	I3LINK(cmd, first_i3_command, last_i3_command, next, prev);
	I3_savecommands();
	return;
    }

    if (!found) {
	i3_printf(ch, "%%^GREEN%%^No command named %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^exists.%%^RESET%%^\r\n", name);
	return;
    }

    if (!i3check_permissions(ch, cmd->level, cmd->level, FALSE))
	return;

    if (!strcasecmp(option, "delete")) {
	i3_printf(ch, "%%^GREEN%%^Command %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^has been deleted.%%^RESET%%^\r\n", cmd->name);
	for (alias = cmd->first_alias; alias; alias = alias_next) {
	    alias_next = alias->next;

	    I3UNLINK(alias, cmd->first_alias, cmd->last_alias, next, prev);
	    I3STRFREE(alias->name);
	    I3DISPOSE(alias);
	}
	I3UNLINK(cmd, first_i3_command, last_i3_command, next, prev);
	I3STRFREE(cmd->name);
	I3DISPOSE(cmd);
	I3_savecommands();
	return;
    }

    /*
     * MY GOD! What an inefficient mess you've made Samson! 
     */
    if (!strcasecmp(option, "alias")) {
	for (alias = cmd->first_alias; alias; alias = alias_next) {
	    alias_next = alias->next;

	    if (!strcasecmp(alias->name, argument)) {
		i3_printf(ch, "%%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^has been removed as an alias for %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n", argument,
			  cmd->name);
		I3UNLINK(alias, cmd->first_alias, cmd->last_alias, next, prev);
		I3STRFREE(alias->name);
		I3DISPOSE(alias);
		I3_savecommands();
		return;
	    }
	}

	for (tmp = first_i3_command; tmp; tmp = tmp->next) {
	    if (!strcasecmp(tmp->name, argument)) {
		i3_printf(ch, "%%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^is already a command name.%%^RESET%%^\r\n", argument);
		return;
	    }
	    for (alias = tmp->first_alias; alias; alias = alias->next) {
		if (!strcasecmp(argument, alias->name)) {
		    i3_printf(ch, "%%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^is already an alias for %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n", argument,
			      tmp->name);
		    return;
		}
	    }
	}

	I3CREATE(alias, I3_ALIAS, 1);
	alias->name = I3STRALLOC(argument);
	I3LINK(alias, cmd->first_alias, cmd->last_alias, next, prev);
	i3_printf(ch, "%%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^has been added as an alias for %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n", alias->name, cmd->name);
	I3_savecommands();
	return;
    }

    if (!strcasecmp(option, "connected")) {
	cmd->connected = !cmd->connected;

	if (cmd->connected)
	    i3_printf(ch, "%%^GREEN%%^Command %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^will now require a connection to I3 to use.%%^RESET%%^\r\n",
		      cmd->name);
	else
	    i3_printf(ch,
		      "%%^GREEN%%^Command %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^will no longer require a connection to I3 to use.%%^RESET%%^\r\n",
		      cmd->name);
	I3_savecommands();
	return;
    }

    if (!strcasecmp(option, "show")) {
	char                                    buf[MAX_STRING_LENGTH];

	i3_printf(ch, "%%^GREEN%%^Command       : %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n", cmd->name);
	i3_printf(ch, "%%^GREEN%%^Permission    : %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n", perm_names[cmd->level]);
	i3_printf(ch, "%%^GREEN%%^Function      : %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n", i3_funcname(cmd->function));
	i3_printf(ch, "%%^GREEN%%^Connection Req: %%^WHITE%%^%%^BOLD%%^%s%%^RESET%%^\r\n", cmd->connected ? "Yes" : "No");
	if (cmd->first_alias) {
	    int                                     col = 0;

	    i3strlcpy(buf, "%^GREEN%^Aliases       : %^WHITE%^%^BOLD%^", MAX_STRING_LENGTH);
	    for (alias = cmd->first_alias; alias; alias = alias->next) {
		snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf), "%s ",
			 alias->name);
		if (++col % 10 == 0)
		    i3strlcat(buf, "\r\n", MAX_STRING_LENGTH);
	    }
	    if (col % 10 != 0)
		i3strlcat(buf, "\r\n", MAX_STRING_LENGTH);
	    i3_printf(ch, "%s%%^RESET%%^", buf);
	}
	return;
    }

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "Required argument missing.\r\n");
	i3_cedit(ch, "");
	return;
    }

    if (!strcasecmp(option, "rename")) {
	i3_printf(ch, "%%^GREEN%%^Command %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^has been renamed to %%^WHITE%%^%%^BOLD%%^%s.%%^RESET%%^\r\n", cmd->name, argument);
	I3STRFREE(cmd->name);
	cmd->name = I3STRALLOC(argument);
	I3_savecommands();
	return;
    }

    if (!strcasecmp(option, "code")) {
	cmd->function = i3_function(argument);
	if (cmd->function == NULL)
	    i3_printf(ch, "%%^GREEN%%^Function %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^does not exist - set to NULL.%%^RESET%%^\r\n", argument);
	else
	    i3_printf(ch, "%%^GREEN%%^Function set to %%^WHITE%%^%%^BOLD%%^%s.%%^RESET%%^\r\n", argument);
	I3_savecommands();
	return;
    }

    if (!strcasecmp(option, "perm") || !strcasecmp(option, "permission")) {
	int                                     permvalue = get_permvalue(argument);

	if (!i3check_permissions(ch, permvalue, cmd->level, FALSE))
	    return;

	cmd->level = permvalue;
	i3_printf(ch, "%%^GREEN%%^Command %%^WHITE%%^%%^BOLD%%^%s %%^GREEN%%^permission level has been changed to %%^WHITE%%^%%^BOLD%%^%s.%%^RESET%%^\r\n",
		  cmd->name, perm_names[permvalue]);
	I3_savecommands();
	return;
    }
    i3_cedit(ch, "");
    return;
}

char                                   *I3_find_social(CHAR_DATA *ch, char *sname, char *person,
						       char *mud, bool victim)
{
    static char                             socname[MAX_STRING_LENGTH];

    socname[0] = '\0';
    i3_printf(ch, "~YSocial ~W%s~Y does not exist on this mud.~!\r\n", sname);

#if 0
    SOCIALTYPE                             *social;
    char                                   *c;

    socname[0] = '\0';

    for (c = sname; *c; *c = tolower(*c), c++);

    if (!(social = find_social(sname))) {
	i3_printf(ch, "%%^YELLOW%%^Social %%^WHITE%%^%%^BOLD%%^%s%%^YELLOW%%^ does not exist on this mud.%%^RESET%%^\r\n", sname);
	return socname;
    }

    if (person && person[0] != '\0' && mud && mud[0] != '\0') {
	if (person && person[0] != '\0' && !strcasecmp(person, CH_I3NAME(ch))
	    && mud && mud[0] != '\0' && !strcasecmp(mud, this_i3mud->name)) {
	    if (!social->others_auto) {
		i3_printf(ch, "%%^YELLOW%%^Social %%^WHITE%%^%%^BOLD%%^%s%%^YELLOW%%^: Missing others_auto.%%^RESET%%^\r\n", social->name);
		return socname;
	    }
	    i3strlcpy(socname, social->others_auto, MAX_STRING_LENGTH);
	} else {
	    if (!victim) {
		if (!social->others_found) {
		    i3_printf(ch, "%%^YELLOW%%^Social %%^WHITE%%^%%^BOLD%%^%s%%^YELLOW%%^: Missing others_found.%%^RESET%%^\r\n", social->name);
		    return socname;
		}
		i3strlcpy(socname, social->others_found, MAX_STRING_LENGTH);
	    } else {
		if (!social->vict_found) {
		    i3_printf(ch, "%%^YELLOW%%^Social %%^WHITE%%^%%^BOLD%%^%s%%^YELLOW%%^: Missing vict_found.%%^RESET%%^\r\n", social->name);
		    return socname;
		}
		i3strlcpy(socname, social->vict_found, MAX_STRING_LENGTH);
	    }
	}
    } else {
	if (!social->others_no_arg) {
	    i3_printf(ch, "%%^YELLOW%%^Social %%^WHITE%%^%%^BOLD%%^%s%%^YELLOW%%^: Missing others_no_arg.%%^RESET%%^\r\n", social->name);
	    return socname;
	}
	i3strlcpy(socname, social->others_no_arg, MAX_STRING_LENGTH);
    }
#endif
    return socname;
}

/* Revised 10/10/03 by Xorith: Recognize the need to capitalize for a new sentence. */
char                                   *i3act_string(const char *format, CHAR_DATA *ch,
						     CHAR_DATA *vic)
{
    static const char                      *he_she[] = { "it", "he", "she" };
    static const char                      *him_her[] = { "it", "him", "her" };
    static const char                      *his_her[] = { "its", "his", "her" };
    static char                             buf[MAX_STRING_LENGTH];
    char                                    tmp_str[MAX_STRING_LENGTH];
    const char                             *i = "";
    char                                   *point;
    bool                                    should_upper = FALSE;

    if (!format || format[0] == '\0' || !ch)
	return NULL;

    point = buf;

    while (*format != '\0') {
	if (*format == '.' || *format == '?' || *format == '!')
	    should_upper = TRUE;
	else if (should_upper == TRUE && !isspace(*format) && *format != '$')
	    should_upper = FALSE;

	if (*format != '$') {
	    *point++ = *format++;
	    continue;
	}
	++format;

	if ((!vic)
	    && (*format == 'N' || *format == 'E' || *format == 'M' || *format == 'S'
		|| *format == 'K'))
	    i = " !!!!! ";
	else {
	    switch (*format) {
		default:
		    i = " !!!!! ";
		    break;
		case 'n':
		    i = "$N";
		    break;
		case 'N':
		    i = "$O";
		    break;
		case 'e':
		    i = should_upper ?
			i3capitalize(he_she[URANGE(0, CH_I3SEX(ch), 2)]) :
			he_she[URANGE(0, CH_I3SEX(ch), 2)];
		    break;

		case 'E':
		    i = should_upper ?
			i3capitalize(he_she[URANGE(0, CH_I3SEX(vic), 2)]) :
			he_she[URANGE(0, CH_I3SEX(vic), 2)];
		    break;

		case 'm':
		    i = should_upper ?
			i3capitalize(him_her[URANGE(0, CH_I3SEX(ch), 2)]) :
			him_her[URANGE(0, CH_I3SEX(ch), 2)];
		    break;

		case 'M':
		    i = should_upper ?
			i3capitalize(him_her[URANGE(0, CH_I3SEX(vic), 2)]) :
			him_her[URANGE(0, CH_I3SEX(vic), 2)];
		    break;

		case 's':
		    i = should_upper ?
			i3capitalize(his_her[URANGE(0, CH_I3SEX(ch), 2)]) :
			his_her[URANGE(0, CH_I3SEX(ch), 2)];
		    break;

		case 'S':
		    i = should_upper ?
			i3capitalize(his_her[URANGE(0, CH_I3SEX(vic), 2)]) :
			his_her[URANGE(0, CH_I3SEX(vic), 2)];
		    break;

		case 'k':
		    i3one_argument(CH_I3NAME(ch), tmp_str);
		    i = (char *)tmp_str;
		    break;
		case 'K':
		    i3one_argument(CH_I3NAME(vic), tmp_str);
		    i = (char *)tmp_str;
		    break;
		    break;
	    }
	}
	++format;
	while ((*point = *i) != '\0')
	    ++point, ++i;
    }
    *point = 0;
    point++;
    *point = '\0';

    buf[0] = UPPER(buf[0]);
    return buf;
}

CHAR_DATA                              *I3_make_skeleton(char *name)
{
    CHAR_DATA                              *skeleton;

    CREATE(skeleton, CHAR_DATA, 1);
#if 0 // prool fool
    skeleton->player.name = I3STRALLOC(name);
    skeleton->player.short_descr = I3STRALLOC(name);
#endif
    skeleton->in_room = 1;				       /* LIMBO */

    return skeleton;
}

void I3_purge_skeleton(CHAR_DATA *skeleton)
{
    if (!skeleton)
	return;
#if 0 // prool fool
    I3STRFREE(skeleton->player.name);
    I3STRFREE(skeleton->player.short_descr);
    I3DISPOSE(skeleton);
#endif
    return;
}

void I3_send_social(I3_CHANNEL *channel, CHAR_DATA *ch, const char *argument)
{
    CHAR_DATA                              *skeleton = NULL;
    char                                   *ps;
    char                                    socbuf_o[MAX_STRING_LENGTH],
                                            socbuf_t[MAX_STRING_LENGTH],
                                            msg_o[MAX_STRING_LENGTH],
                                            msg_t[MAX_STRING_LENGTH];
    char                                    arg1[MAX_INPUT_LENGTH],
                                            person[MAX_INPUT_LENGTH],
                                            mud[MAX_INPUT_LENGTH],
                                            user[MAX_INPUT_LENGTH],
                                            buf[MAX_STRING_LENGTH];
    unsigned int                            x;

    person[0] = '\0';
    mud[0] = '\0';

    /*
     * Name of social, remainder of argument is assumed to hold the target 
     */
    argument = i3one_argument(argument, arg1);

    snprintf(user, MAX_INPUT_LENGTH, "%s@%s", CH_I3NAME(ch), this_i3mud->name);
    if (!strcasecmp(user, argument)) {
	i3_printf(ch, "Cannot target yourself due to the nature of I3 socials.\r\n");
	return;
    }

    if (argument && argument[0] != '\0') {
	if (!(ps = (char *)strchr(argument, '@'))) { // prool
	    i3_printf(ch, "You need to specify a person@mud for a target.\r\n");
	    return;
	} else {
	    for (x = 0; x < strlen(argument); x++) {
		person[x] = argument[x];
		if (person[x] == '@')
		    break;
	    }
	    person[x] = '\0';

	    ps[0] = '\0';
	    i3strlcpy(mud, ps + 1, MAX_INPUT_LENGTH);
	}
    }

    snprintf(socbuf_o, MAX_STRING_LENGTH, "%s", I3_find_social(ch, arg1, person, mud, FALSE));

    if (socbuf_o[0] != '\0')
	snprintf(socbuf_t, MAX_STRING_LENGTH, "%s",
		 I3_find_social(ch, arg1, person, mud, TRUE));

    if ((socbuf_o[0] != '\0') && (socbuf_t[0] != '\0')) {
	if (argument && argument[0] != '\0') {
	    int                                     sex;

	    snprintf(buf, MAX_STRING_LENGTH, "%s@%s", person, mud);
	    sex = I3_get_ucache_gender(buf);
	    if (sex == -1) {
		/*
		 * Greg said to "just punt and call them all males".
		 * * I decided to meet him halfway and at least request data before punting :)
		 */
		I3_send_chan_user_req(mud, person);
		sex = SEX_MALE;
	    } else
		sex = i3todikugender(sex);

	    skeleton = I3_make_skeleton(buf);
	    CH_I3SEX(skeleton) = sex;
	}

	i3strlcpy(msg_o, (char *)i3act_string(socbuf_o, ch, skeleton), MAX_STRING_LENGTH);
	i3strlcpy(msg_t, (char *)i3act_string(socbuf_t, ch, skeleton), MAX_STRING_LENGTH);

	if (!skeleton)
	    I3_send_channel_emote(channel, CH_I3NAME(ch), msg_o);
	else {
	    i3strlcpy(buf, person, MAX_STRING_LENGTH);
	    buf[0] = tolower(buf[0]);
	    I3_send_channel_t(channel, CH_I3NAME(ch), mud, buf, msg_o, msg_t, person);
	}
	if (skeleton)
	    I3_purge_skeleton(skeleton);
    }
    return;
}

I3_CMD(I3_taunt)
{
    do_taunt_from_log();
    return;
}

const char                             *i3_funcname(I3_FUN *func)
{
    if (func == I3_other)
	return ("I3_other");
    if (func == I3_listen_channel)
	return ("I3_listen_channel");
    if (func == I3_chanlist)
	return ("I3_chanlist");
    if (func == I3_mudlist)
	return ("I3_mudlist");
    if (func == I3_invis)
	return ("I3_invis");
    if (func == I3_who)
	return ("I3_who");
    if (func == I3_locate)
	return ("I3_locate");
    if (func == I3_tell)
	return ("I3_tell");
    if (func == I3_reply)
	return ("I3_reply");
    if (func == I3_emote)
	return ("I3_emote");
    if (func == I3_beep)
	return ("I3_beep");
    if (func == I3_ignorecmd)
	return ("I3_ignorecmd");
    if (func == I3_finger)
	return ("I3_finger");
    if (func == I3_mudinfo)
	return ("I3_mudinfo");
    if (func == I3_color)
	return ("I3_color");
    if (func == I3_afk)
	return ("I3_afk");
    if (func == I3_chan_who)
	return ("I3_chan_who");
    if (func == I3_connect)
	return ("I3_connect");
    if (func == I3_disconnect)
	return ("I3_disconnect");
    if (func == I3_reload)
	return ("I3_reload");
    if (func == I3_send_user_req)
	return ("I3_send_user_req");
    if (func == I3_permstats)
	return ("I3_permstats");
    if (func == I3_deny_channel)
	return ("I3_deny_channel");
    if (func == I3_permset)
	return ("I3_permset");
    if (func == I3_chanlayout)
	return ("I3_chanlayout");
    if (func == I3_admin_channel)
	return ("I3_admin_channel");
    if (func == I3_addchan)
	return ("I3_addchan");
    if (func == I3_removechan)
	return ("I3_removechan");
    if (func == I3_edit_channel)
	return ("I3_edit_channel");
    if (func == I3_mudlisten)
	return ("I3_mudlisten");
    if (func == I3_router)
	return ("I3_router");
    if (func == I3_bancmd)
	return ("I3_bancmd");
    if (func == I3_setconfig)
	return ("I3_setconfig");
    if (func == I3_setup_channel)
	return ("I3_setup_channel");
    if (func == I3_stats)
	return ("I3_stats");
    if (func == I3_show_ucache_contents)
	return ("I3_show_ucache_contents");
    if (func == I3_debug)
	return ("I3_debug");
    if (func == i3_hedit)
	return ("i3_hedit");
    if (func == i3_help)
	return ("i3_help");
    if (func == i3_cedit)
	return ("i3_cedit");
    if (func == I3_taunt)
	return ("I3_taunt");

    return "";
}

I3_FUN                                 *i3_function(const char *func)
{
    if (!strcasecmp(func, "I3_other"))
	return I3_other;
    if (!strcasecmp(func, "I3_listen_channel"))
	return I3_listen_channel;
    if (!strcasecmp(func, "I3_chanlist"))
	return I3_chanlist;
    if (!strcasecmp(func, "I3_mudlist"))
	return I3_mudlist;
    if (!strcasecmp(func, "I3_invis"))
	return I3_invis;
    if (!strcasecmp(func, "I3_who"))
	return I3_who;
    if (!strcasecmp(func, "I3_locate"))
	return I3_locate;
    if (!strcasecmp(func, "I3_tell"))
	return I3_tell;
    if (!strcasecmp(func, "I3_reply"))
	return I3_reply;
    if (!strcasecmp(func, "I3_emote"))
	return I3_emote;
    if (!strcasecmp(func, "I3_beep"))
	return I3_beep;
    if (!strcasecmp(func, "I3_ignorecmd"))
	return I3_ignorecmd;
    if (!strcasecmp(func, "I3_finger"))
	return I3_finger;
    if (!strcasecmp(func, "I3_mudinfo"))
	return I3_mudinfo;
    if (!strcasecmp(func, "I3_color"))
	return I3_color;
    if (!strcasecmp(func, "I3_afk"))
	return I3_afk;
    if (!strcasecmp(func, "I3_chan_who"))
	return I3_chan_who;
    if (!strcasecmp(func, "I3_connect"))
	return I3_connect;
    if (!strcasecmp(func, "I3_disconnect"))
	return I3_disconnect;
    if (!strcasecmp(func, "I3_reload"))
	return I3_reload;
    if (!strcasecmp(func, "I3_send_user_req"))
	return I3_send_user_req;
    if (!strcasecmp(func, "I3_permstats"))
	return I3_permstats;
    if (!strcasecmp(func, "I3_deny_channel"))
	return I3_deny_channel;
    if (!strcasecmp(func, "I3_permset"))
	return I3_permset;
    if (!strcasecmp(func, "I3_admin_channel"))
	return I3_admin_channel;
    if (!strcasecmp(func, "I3_bancmd"))
	return I3_bancmd;
    if (!strcasecmp(func, "I3_setconfig"))
	return I3_setconfig;
    if (!strcasecmp(func, "I3_setup_channel"))
	return I3_setup_channel;
    if (!strcasecmp(func, "I3_chanlayout"))
	return I3_chanlayout;
    if (!strcasecmp(func, "I3_addchan"))
	return I3_addchan;
    if (!strcasecmp(func, "I3_removechan"))
	return I3_removechan;
    if (!strcasecmp(func, "I3_edit_channel"))
	return I3_edit_channel;
    if (!strcasecmp(func, "I3_mudlisten"))
	return I3_mudlisten;
    if (!strcasecmp(func, "I3_router"))
	return I3_router;
    if (!strcasecmp(func, "I3_stats"))
	return I3_stats;
    if (!strcasecmp(func, "I3_show_ucache_contents"))
	return I3_show_ucache_contents;
    if (!strcasecmp(func, "I3_debug"))
	return I3_debug;
    if (!strcasecmp(func, "i3_help"))
	return i3_help;
    if (!strcasecmp(func, "i3_cedit"))
	return i3_cedit;
    if (!strcasecmp(func, "i3_hedit"))
	return i3_hedit;
    if (!strcasecmp(func, "I3_taunt"))
	return I3_taunt;

    return NULL;
}

/*
 * This is how channels are interpreted. If they are not commands
 * or socials, this function will go through the list of channels
 * and send it to it if the name matches the local channel name.
 */
bool i3_command_hook(CHAR_DATA *ch, const char *lcommand, const char *argument)
{
    I3_CMD_DATA                            *cmd;
    I3_ALIAS                               *alias;
    I3_CHANNEL                             *channel;
    int                                     x = 0;
    char                                    buffer[MAX_STRING_LENGTH];
    const char                             *s = NULL;
    char                                   *b = buffer;
    int                                     token_count = 15;
    const char                             *token[] = {
        "%^RESET%^%^RED%^",
        "%^RESET%^%^RED%^%^BOLD%^",
        "%^RESET%^%^ORANGE%^",
        "%^RESET%^%^YELLOW%^",
        "%^RESET%^%^GREEN%^",
        "%^RESET%^%^GREEN%^%^BOLD%^",
        "%^RESET%^%^WHITE%^",
        "%^RESET%^%^WHITE%^%^BOLD%^",
        "%^RESET%^%^CYAN%^%^BOLD%^",
        "%^RESET%^%^CYAN%^",
        "%^RESET%^%^BLUE%^%^BOLD%^",
        "%^RESET%^%^BLUE%^",
        "%^RESET%^%^BLACK%^%^BOLD%^",
        "%^RESET%^%^MAGENTA%^",
        "%^RESET%^%^MAGENTA%^%^BOLD%^",
    };
    int                                     color = 0;

    if (IS_NPC(ch))
	return FALSE;

    if (!ch->desc)
	return FALSE;

    if (!this_i3mud) {
	i3log("%s", "Ooops. I3 called with missing configuration!");
	return FALSE;
    }

    if (first_i3_command == NULL) {
	i3log("%s", "Dammit! No command data is loaded!");
	return FALSE;
    }
#if 0 // prool fool
    if (I3PERM(ch) <= I3PERM_NONE) {
	i3bug("Permission %d vs. %d", I3PERM(ch), I3PERM_NONE);
	return FALSE;
    }
#endif

    /*
     * Simple command interpreter menu. Nothing overly fancy etc, but it beats trying to tie directly into the mud's
     * * own internal structures. Especially with the differences in codebases.
     */
    for (cmd = first_i3_command; cmd; cmd = cmd->next) {
#if 0 // prool fool
	if (I3PERM(ch) < cmd->level)
	    continue;
#endif

	for (alias = cmd->first_alias; alias; alias = alias->next) {
	    if (!strcasecmp(lcommand, alias->name)) {
		lcommand = cmd->name;
		break;
	    }
	}

	if (!strcasecmp(lcommand, cmd->name)) {
	    if (cmd->connected == TRUE && !i3_is_connected()) {
		i3_printf(ch, "The mud is not currently connected to I3.\r\n");
		return TRUE;
	    }

	    if (cmd->function == NULL) {
		i3_printf(ch, "That command has no code set. Inform the administration.\r\n");
		i3bug("i3_command_hook: Command %s has no code set!", cmd->name);
		return TRUE;
	    }

	    (*cmd->function) (ch, argument);
	    return TRUE;
	}
    }

    /*
     * Assumed to be going for a channel if it gets this far 
     */

    if (!(channel = find_I3_channel_by_localname(lcommand)))
	return FALSE;
#if 0 // prool fool
    if (I3PERM(ch) < channel->i3perm)
	return FALSE;

    if (I3_hasname(I3DENY(ch), channel->local_name)) {
	i3_printf(ch, "You have been denied the use of %s by the administration.\r\n",
		  channel->local_name);
	return TRUE;
    }
#endif

    if (!argument || argument[0] == '\0') {
	i3_printf(ch, "%%^CYAN%%^The last %d %s messages:%%^RESET%%^\r\n", MAX_I3HISTORY, channel->local_name);
	for (x = 0; x < MAX_I3HISTORY; x++) {
	    if (channel->history[x] != NULL)
		i3_printf(ch, "%s%%^RESET%%^\r\n", channel->history[x]);
	    else
		break;
	}
	return TRUE;
    }

    if (!i3_is_connected()) {
	i3_printf(ch, "The mud is not currently connected to I3.\r\n");
	return TRUE;
    }
#if 0 // prool fool
    if (I3PERM(ch) >= I3PERM_ADMIN && !strcasecmp(argument, "log")) {
	if (!I3IS_SET(channel->flags, I3CHAN_LOG)) {
	    I3SET_BIT(channel->flags, I3CHAN_LOG);
	    i3_printf(ch,
		      "%%^RED%%^%%^BOLD%%^File logging enabled for %s, PLEASE don't forget to undo this when it isn't needed!%%^RESET%%^\r\n",
		      channel->local_name);
	} else {
	    I3REMOVE_BIT(channel->flags, I3CHAN_LOG);
	    i3_printf(ch, "%%^GREEN%%^%%^BOLD%%^File logging disabled for %s.%%^RESET%%^\r\n", channel->local_name);
	}
	I3_write_channel_config();
	return TRUE;
    }

    if (!I3_hasname(I3LISTEN(ch), channel->local_name)) {
	i3_printf(ch, "%%^YELLOW%%^You were trying to send something to an I3 "
		  "channel but you're not listening to it.%%^RESET%%^\r\nPlease use the command "
		  "'%%^WHITE%%^%%^BOLD%%^i3listen %s%%^YELLOW%%^' to listen to it.%%^RESET%%^\r\n", channel->local_name);
	return TRUE;
    }
#endif

    switch (argument[0]) {
	case ',':
	case ':':
	    /*
	     * Strip the , and then extra spaces - Remcon 6-28-03 
	     */
	    argument++;
	    while (isspace(*argument))
		argument++;
	    I3_send_channel_emote(channel, CH_I3NAME(ch), argument);
	    break;
	case '@':
	    /*
	     * Strip the @ and then extra spaces - Remcon 6-28-03 
	     */
	    argument++;
	    while (isspace(*argument))
		argument++;
	    I3_send_social(channel, ch, argument);
	    break;
        case '!':
            argument++;
	    while (isspace(*argument))
		argument++;
            if(!strncasecmp(argument, "colorize", 8)) {
                argument += 8;
                while (isspace(*argument))
                    argument++;
                bzero(buffer, MAX_STRING_LENGTH);
                color = random() % token_count;
                strncpy(b, token[color], strlen(token[color]));
                b += strlen(token[color]);
                for(s = argument, b = buffer; *s && strlen(b) < (MAX_STRING_LENGTH - 10); s++) {
                    if(isspace(*s) || ispunct(*s)) {
                        *b = *s;
                        b++;
                        color = random() % token_count;
                        strncpy(b, token[color], strlen(token[color]));
                        b += strlen(token[color]);
                    } else {
                        *b = *s;
                        b++;
                    }
                }
                strcpy(b, "%^RESET%^");
	        I3_send_channel_message(channel, CH_I3NAME(ch), buffer);
            } else if(!strncasecmp(argument, "rainbow", 7)) {
                argument += 7;
                while (isspace(*argument))
                    argument++;
                bzero(buffer, MAX_STRING_LENGTH);
                color = 0;
                strncpy(b, token[color], strlen(token[color]));
                b += strlen(token[color]);
                for(s = argument, b = buffer; *s && strlen(b) < (MAX_STRING_LENGTH - 10); s++) {
                    if(isspace(*s)) {
                        *b = *s;
                        b++;
                    } else {
                        *b = *s;
                        b++;
                        color++;
                        color = color % token_count;
                        strncpy(b, token[color], strlen(token[color]));
                        b += strlen(token[color]);
                    }
                }
                strcpy(b, "%^RESET%^");
	        I3_send_channel_message(channel, CH_I3NAME(ch), buffer);
            } else {
	        I3_send_channel_message(channel, CH_I3NAME(ch), argument);
            }
            break;
	default:
	    I3_send_channel_message(channel, CH_I3NAME(ch), argument);
	    break;
    }
    return TRUE;
}

void i3_npc_speak(const char *chan_name, const char *actor, const char *message)
{
    I3_CHANNEL                             *channel;

    // char buf[MAX_STRING_LENGTH];

    if (!i3_is_connected()) {
	i3log("Not connected!");
	return;
    }
    if (!(channel = find_I3_channel_by_localname(chan_name))) {
	i3log("Can't find local channel %s.", chan_name);
	return;
    }

    while (isspace(*message))
	message++;
    i3log("Sending [%s] from %s to %s.", message, actor, chan_name);
    I3_send_channel_message(channel, actor, message);
}

void i3_npc_chat(const char *chan_name, const char *actor, const char *message)
{
    I3_CHANNEL                             *channel;

    // char buf[MAX_STRING_LENGTH];

    if (!i3_is_connected()) {
	i3log("Not connected!");
	return;
    }
    if (!(channel = find_I3_channel_by_localname(chan_name))) {
	i3log("Can't find local channel %s.", chan_name);
	return;
    }

    while (isspace(*message))
	message++;
    i3log("Sending [%s] from %s to %s.", message, actor, chan_name);
    I3_send_channel_emote(channel, actor, message);

#if 0
    if (strstr(message, "$N") == NULL)
	snprintf(buf, MAX_STRING_LENGTH, "$N %s", message);
    else
	i3strlcpy(buf, message, MAX_STRING_LENGTH);

    I3_write_header("channel-e", this_i3mud->name, actor, NULL, NULL);
    I3_write_buffer("\"");
    I3_write_buffer(channel->I3_name);
    I3_write_buffer("\",\"");
    I3_write_buffer(actor);
    I3_write_buffer("\",\"");
    send_to_i3(I3_escape(buf));
    I3_write_buffer("\",})\r");
    I3_send_packet();
    i3log("Sending [%s] from %s to %s.", buf, actor, channel->I3_name);
#endif
}

char                                   *I3_nameescape(const char *ps)
{
    static char                             xnew[MAX_STRING_LENGTH];
    char                                   *pnew = xnew;
    char                                    c;

    while (ps[0]) {
	c = (char)tolower((int)*ps);
	ps++;
	if (c < 'a' || c > 'z')
	    continue;
	pnew[0] = c;
	pnew++;
    }
    pnew[0] = '\0';
    return xnew;
}

char                                   *I3_old_nameremap(const char *ps)
{
    static char                             xnew[MAX_STRING_LENGTH];

    if(!strcasecmp(ps, "quixadhal")) {
        /* strcpy(xnew, "Quixadhal, the Lost"); */
        /* strcpy(xnew, "きけさだる"); */
        /* strcpy(xnew, "Dread Lord Quixadhal"); */
        /* strcpy(xnew, "Off-Topic Quixadhal"); */
        strcpy(xnew, "Cromulent Quixadhal");
    } else {
        strcpy(xnew, ps);
    }
    return xnew;
}

