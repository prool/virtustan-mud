// #define DEBUG

#define SEND_EMAIL 0

/* ************************************************************************
*   File: virtustan.cpp                          Part of Virtustan MUD    *
*  Usage: prool subprograms for Virtustan MUD                             *
*                                                                         *
*  Copyleft 2007-2023, Prool                                              *
*                                                                         *
*  Author: Prool, proolix@gmail.com, http://mud.kharkov.org               *
************************************************************************ */

#include "interpreter.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "utils.h"
#include "structs.h"
#include "conf.h"
#include "sysdep.h"
#include "dg_scripts.h"
//#include "prool.h"
#include "char.hpp"
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef HAVE_ICONV
#include <iconv.h>
#endif

#include "telnet.h"

#include "virtustan.h"

extern int total_players;
extern int boot_time;
extern int console_codetable;
extern char mudname[];
extern obj_rnum top_of_objt;

void system_(char *cmd)
{
if (system(cmd)==-1)
	{
	printf("system_(): error, maybe system() fork() error, RAM overflow\n");
	}
}

void prool_log(char *str)
{
FILE *fp;
fp=fopen("proolmud.log", "a");
fprintf(fp,"%s %s\n",ptime(),str);
fclose(fp);
}

void perslog (char *verb, const char *pers)
{
FILE *fp; char buffer [PROOL_MAX_STRLEN];
char *ident;

if (mudname[0]) ident=mudname;
else ident = (char *)"VMUD";

fp=fopen(PERSLOG_FILE, "a");
fprintf(fp,"%s %s %s\n",ptime(),pers,verb);
if (console_codetable==T_UTF)
	{
	koi_to_utf8((char*)pers,buffer);
	printf("%s %s %s %s\n",ident, ptime(),buffer,verb);
	}
else if (console_codetable==T_LAT)
	{
	koi_to_lat((char*)pers,buffer);
	printf("%s %s %s %s\n",ident, ptime(),buffer,verb);
	}
else
	{
	if (console_codetable!=T_EMPTY) printf("%s %s %s %s\n",ident, ptime(),pers,verb);
	}
fclose(fp);
}

char *ptime(void) // Возвращаемое значение: ссылка на текстовую строку с текущим временем
	{
	char *tmstr;
	time_t mytime;

	mytime = time(0);

	tmstr = (char *) asctime(localtime(&mytime));
	*(tmstr + strlen(tmstr) - 1) = '\0';

	return tmstr;

	}
	
ACMD(do_omol)
{
ch->player_data.time.birth=time(0);
}

ACMD(do_konsole)
{
printf("do_konsole: %s\r\n",argument);
prool_log(argument);
}

ACMD(do_fflush)
{
send_to_char("fflush!\n", ch);
fflush(0);
}

#define PUT_OBJ(obj_number) {r_num = real_object(obj_number); if (r_num==-1) {send_to_char("&RЭтого предмета почему-то не существует в мире и я не могу его найти!&n :(\r\n",ch); return;} obj = read_object(r_num, REAL); GET_OBJ_MAKER(obj) = GET_UNIQUE(ch); obj_to_char(obj, ch); act("$n получил$g от духа мада $o3!", FALSE, ch, obj, 0, TO_ROOM); act("Вы получили от духа мада $o3.", FALSE, ch, obj, 0, TO_CHAR); /* load_otrigger(obj); obj_decay(obj); */ olc_log("Духмада: %s load obj %s #%d", GET_NAME(ch), GET_OBJ_ALIAS(obj), obj_number);}

#define DUH_INSTR "Список вещей, которые вам может принести дух мада: хлеб, фляга, меч, нож, лук, палица, лампа, шарик, доспех, руны, сума, кинжал, мизерикорд, стилет, бочка с синим колдовским зельем\r\n\r\nПример вызова:\r\nдухмада хлеб\r\n"

ACMD(do_duhmada)
{
	mob_rnum r_num;
	OBJ_DATA *obj;

// char str[100];
// sprintf(str,"`%s'",argument);
// send_to_char(str, ch);

if (*argument==0)
	{
	send_to_char(DUH_INSTR, ch);
	}
else
	{
	argument++;
	if (!strcmp(argument,"хлеб")) PUT_OBJ(2200 /*125*/)
//	else if (!strcmp(argument,"рекол")) PUT_OBJ(2201/*115*/)
	else if (!strcmp(argument,"меч")) PUT_OBJ(2202/*112*/)
	else if (!strcmp(argument,"палица")) PUT_OBJ(2203/*121*/)
	else if (!strcmp(argument,"нож")) PUT_OBJ(2204/*122*/)
	else if (!strcmp(argument,"шарик")) PUT_OBJ(2205/*126*/)
	else if (!strcmp(argument,"лампа")) PUT_OBJ(2206/*127*/)
	else if (!strcmp(argument,"доспех")) PUT_OBJ(2207/*113*/)
	else if (!strcmp(argument,"фляга")) PUT_OBJ(2208/*114*/)
	else if (!strcmp(argument,"лук")) PUT_OBJ(2209/*109*/)
	else if (!strcmp(argument,"сума")) PUT_OBJ(/*2214*/2045)
	else if (!strcmp(argument,"кинжал")) PUT_OBJ(9908)
	else if (!strcmp(argument,"мизерикорд")) PUT_OBJ(9921)
	else if (!strcmp(argument,"стилет")) PUT_OBJ(2215)
	else if (!strcmp(argument,"бочка с синим колдовским зельем")) PUT_OBJ(500023)
	else if (!strcmp(argument,"руны")) {PUT_OBJ(222/*693*/); PUT_OBJ(223/*692*/); PUT_OBJ(224/*695*/); PUT_OBJ(225/*694*/);}
	else
		{
		send_to_char("Этого нет в списке моих возможностей! Набирайте названия полностью.\r\n\r\n",ch);
		send_to_char(DUH_INSTR, ch);
		}
	}

}

char *nslookup(const char *ip) // prool
// example: input "217.12.192.65", returned output "www.itl.ua"
{
struct in_addr ip0;
struct hostent *hp;

if (!inet_aton(ip, &ip0))
	{/* printf("can't parse IP address %s", ip) */; }

if ((hp = gethostbyaddr((const char *)&ip0, sizeof ip0, AF_INET)) == NULL)
	{/* printf("no name associated with %s", ip) */; }

// printf("name associated with %s is %s\n", ip, hp->h_name);

#ifdef CYGWIN
return (char *)((hp!=NULL)?hp->h_name:(char *)"*");
#else
return (hp!=NULL)?hp->h_name:(char *)"*";
#endif
}

void send_email2 (char *from, char *to, char *subj, char *text)
	{char buf [80*25];
#if 1

	sprintf(buf,"echo \"Subject: %s\r\nContent-Type: text/plain; charset=koi8-r\r\n\r\n%s\"|/usr/sbin/sendmail -F\"%s\" %s\r\n",
		subj,text,from,to);

	system_(buf);
#endif
	}

void mssp_start(DESCRIPTOR_DATA * t)
{char buf[1024];

#if 0
const char mssp_str[] = {IAC,SB,MSSP,
	MSSP_VAR,'P','L','A','Y','E','R','S',MSSP_VAL,'0',
	MSSP_VAR,'N','A','M','E',MSSP_VAL,'V','i','r','t','u','s','t','a','n',' ','M','U','D',
	IAC,SE,'\0'};
#endif

sprintf(buf,
"%c%c%c%cPLAYERS%c%i%cNAME%cVirtustan MUD%cUPTIME%c%li%cCRAWL DELAY%c-1\
%cHOSTNAME%cmud.kharkov.org\
%cPORT%c3000\
%cPORT%c8888\
%cCODEBASE%cCircleMUD/Byliny\
%cCONTACT%cproolix@gmail.com\
%cCREATED%c2007\
%cIP%c195.123.245.173\
%cLANGUAGE%cRussian\
%cLOCATION%cUkraine\
%cMINIMUM AGE%c0\
%cWEBSITE%chttp://mud.kharkov.org\
%cFAMILY%cDikuMUD\
%cAREAS%c%i\
%cMOBILES%c%i\
%cOBJECTS%c%i\
%cROOMS%c%i\
%cCLASSES%c15\
%cRACES%c6\
%cANSI%c1\
%cMCCP%c1\
%cMCP%c0\
%cMSP%c0\
%cMXP%c0\
%cPUEBLO%c0\
%cGMCP%c0\
%cMSDP%c0\
%cHIRING BUILDERS%c1\
%cHIRING CODERS%c1\
%cPLAYER CLANS%c1\
%cWORLD ORIGINALITY%c1\
%cGENRE%cFantasy\
%cGAMESYSTEM%cCustom\
%cLEVELS%c30\
%cVT100%c0\
%cPAY TO PLAY%c0\
%cPAY FOR PERKS%c0\
%cINTERMUD%c0\
%cXTERM 256 COLORS%c0\
%cXTERM TRUE COLORS%c0\
%cUTF-8%c1\
%cCHARSET%cUTF-8%ckoi8-r%ccp1251\
%cREFERRAL%clolamud.net:6969%ctbamud.com:9091\
%c%c",
IAC,SB,MSSP,MSSP_VAR,MSSP_VAL,total_players,MSSP_VAR,MSSP_VAL,MSSP_VAR,MSSP_VAL,(long int)boot_time,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,top_of_zone_table + 1 /*statistic_zones*/,
MSSP_VAR,MSSP_VAL,top_of_mobt + 1 /*statistic_mobs*/,
MSSP_VAR,MSSP_VAL,top_of_objt + 1 /*statistic_objs*/,
MSSP_VAR,MSSP_VAL,top_of_world + 1 /*statistic_rooms*/,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR,MSSP_VAL,
MSSP_VAR, MSSP_VAL, MSSP_VAL, MSSP_VAL,
MSSP_VAR, MSSP_VAL, MSSP_VAL,
IAC,SE);

// printf("MSSP total_players %i\n",total_players);

write_to_descriptor(t->descriptor, buf, strlen(buf));
}

void test_color(void)
{
const char *const green = "\033[0;40;32m";
const char *const normal = "\033[0m";
int i;

char ESC = '\033';
for (i=1;i<8;i++)
	{
	printf("%c[0;40;%imcolor %i ", ESC, 30+i, i);
	}

for (i=0;i<8;i++)
	{
	printf("%c[0;40;%i;1mlight color %i ", ESC, 30+i, i);
	}

printf("%s Hello World%s\n", green, normal);
}

#ifdef HAVE_ICONV
void koi_to_utf8(char *str_i, char *str_o)
{
	iconv_t cd;
	size_t len_i, len_o = MAX_SOCK_BUF * 6;
	size_t i;
	char *str_i_orig=str_i;
	char *str_o_orig=str_o;

	if ((cd = iconv_open("UTF-8","KOI8-RU")) == (iconv_t) - 1)
	{
		printf("koi_to_utf8: iconv_open error\n");
		*str_o = 0;
		return;
	}
	len_i = strlen(str_i);
	if ((i = iconv(cd, &str_i, &len_i, &str_o, &len_o)) == (size_t) - 1)
	{
		printf("koi_to_utf8: iconv error ");
		printf("'%s' [", str_i_orig);
		while (*str_i_orig) printf("%02X ", *str_i_orig++);
		printf("] -> '%s' [", str_o_orig);
		while (*str_o_orig) printf("%02X ", *str_o_orig++);
		printf("]\n");

		*str_o = 0;
		return;
	}
	*str_o = 0;
	if (iconv_close(cd) == -1)
	{
		printf("koi_to_utf8: iconv_close error\n");
		return;
	}
}

void telnet_prool_scanner(char *str)
{
char local_str [MAX_SOCK_BUF*6];
int i;
char *cc;

cc=str; i=0;

printf("prooldebug telnet_scanner 1 '%s'\n", str);

while (*cc)
	{
	if ((*cc&0xFF)==0xFF)
		{
			printf("prooldebug: label 0xFF\n");
		cc++;
			switch (*cc&0xFF)
			{
				case 0xF4:
					printf("prooldebug: label 0xF4\n");
					local_str[i++]=0x83;
					break;
				case 0xF5:
					printf("prooldebug: label 0xF5\n");
					local_str[i++]=0x8f;
					break;
				case 0xFD: ;
					printf("prooldebug: label 0xFD\n");
				default: ;
			}
		}
	else
		local_str[i++]=*cc;
	cc++;
	if (i==(MAX_SOCK_BUF*6-1)) break;
	}
local_str[i]=0;

strcpy(str, local_str);
printf("prooldebug telnet_scanner 2 '%s'\n", str);
}

void utf8_to_koi(char *str_i, char *str_o)
{
	iconv_t cd;
	size_t len_i, len_o = MAX_SOCK_BUF * 6;
	size_t i;
	char *str_i_orig=str_i;
	char *str_o_orig=str_o;

//	telnet_prool_scanner(str_i);
//	printf("prooldebug telnet_scanner 3 '%s'\n", str_i);

#if 0 // debug print
		printf("I '%s' [", str_i_orig);
		while (*str_i_orig) printf("%02X ", *str_i_orig++);
		printf("]\n");
#endif

	if ((cd = iconv_open("KOI8-RU", "UTF-8")) == (iconv_t) - 1)
	{
		printf("utf8_to_koi: iconv_open error\n");
		return;
	}
	len_i = strlen(str_i);
	if ((i=iconv(cd, &str_i, &len_i, &str_o, &len_o)) == (size_t) - 1)
	{
		printf("utf8_to_koi: iconv error ");
		printf("'%s' [", str_i_orig);
		while (*str_i_orig) printf("%02X ", *str_i_orig++);
		printf("] -> '%s' [", str_o_orig);
		while (*str_o_orig) printf("%02X ", *str_o_orig++);
		printf("]\n");
		// return;
	}
	*str_o = 0;
	if (iconv_close(cd) == -1)
	{
		printf("utf8_to_koi: iconv_close error\n");
		return;
	}
}
#endif // HAVE_ICONV

ACMD (do_virtustan)
{
int i;
char buf [PROOL_MAX_STRLEN];

//printf("do_virtustan: parameter=%i\n", ch->virtustan);


if (argument==0) goto l_fin;
if (*argument==0) goto l_fin;
if (*argument==' ') argument++;
if (*argument==0) goto l_fin;
i=atoi(argument);
snprintf(buf, PROOL_MAX_STRLEN, "do_virtustan: i=%i\r\n", i);
send_to_char(buf,ch);
ch->virtustan=i;
return;

l_fin: ;
snprintf(buf, PROOL_MAX_STRLEN, "do_virtustan: virtustan=%i\r\n", ch->virtustan);
send_to_char(buf,ch);
}

int prool_miw (int miw)
{
	if (miw==0) return 100;
	else return miw;
}
