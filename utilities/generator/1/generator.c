// zone generator by prool <proolix@gmail.com> http://mud.kharkov.org
// cyrillic letters in codetable koi8-r
// кириллица в кодировке koi8-r

#define VERSION "0.2"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NAMELEN 200
#define COPYLEFT "Created by Zone Generator by proolix@gmail.com"

#define SEVER 0
#define VOSTOK 1
#define YUG 2
#define ZAPAD 3
#define VVERH 4
#define VNIZ 5
#define CALC(i,j) j-1+(i-1)*10+zone_num*100
#define EXIT(DIREKT) fprintf(fw, "D%i\n~\n~\n%s %s %i\n", DIREKT, ROOM_EXIT_FLAG, ROOM_EXIT_KEY, exit_num(DIREKT, i, j));

#define MAX_STR 512
#define MAX_PARAM 20

int zone_num;

char *ptime(void);  // Возвращаемое значение: ссылка на текстовую строку с текущим временем

int exit_num (int direkt, int i, int j)
{
switch (direkt)
	{
	case SEVER: return CALC(i-1,j);
	case YUG: return CALC(i+1,j);
	case ZAPAD: return CALC(i,j-1);
	case VOSTOK: return CALC(i,j+1);
	default: return 0;
	}
}

int main (void)
{
int i, ii, j, param_num;
int mob_vnum, room_vnum, max_in_room, max_in_world;

FILE *fz;
FILE *fm;
FILE *fo;
FILE *ft;
FILE *fw;

char namez[NAMELEN];
char namem[NAMELEN];
char nameo[NAMELEN];
char namet[NAMELEN];
char namew[NAMELEN];

char buf[MAX_STR];

char ZONE [MAX_STR];
char ZONE_NAME [MAX_STR];
char REPOP_TIME [MAX_STR];
char REPOP_TYPE [MAX_STR];
char MOB_IFFLAG [MAX_STR];
char ALIAS [MAX_STR];
char PADEZH1 [MAX_STR];
char PADEZH2 [MAX_STR];
char PADEZH3 [MAX_STR];
char PADEZH4 [MAX_STR];
char PADEZH5 [MAX_STR];
char PADEZH6 [MAX_STR];
char DESCR [MAX_STR];
char LONG_DESCR [MAX_STR];
char MOB_FLAG [MAX_STR];
char MOB_AFF [MAX_STR];
char ALIGN [MAX_STR];
char MOB_LVL [MAX_STR];
char MOB_HITROLL [MAX_STR];
char MOB_AC [MAX_STR];
char MOB_HIT [MAX_STR];
char MOB_DAMAGE [MAX_STR];
char MOB_MONEY [MAX_STR];
char MOB_EXPERIENSE [MAX_STR];
char MOB_POS_BOOT [MAX_STR];
char MOB_POS_DEFAULT [MAX_STR];
char MOB_SEX [MAX_STR];
char ROOM_TITLE [MAX_STR];
char ROOM_DESCR [MAX_STR];
char ROOM_FLAGS [MAX_STR];
char ROOM_SECTOR_TYPE [MAX_STR];
char ROOM_EXIT_FLAG [MAX_STR];
char ROOM_EXIT_KEY [MAX_STR];

char MOB_PARAMETERS [MAX_STR][MAX_PARAM];

char *cc;
FILE *fp;

printf("Zone generator for MUD by Prool %s\n",VERSION);

for (i=0;i<MAX_PARAM;i++) MOB_PARAMETERS[0][i]=0;

ZONE [0]=0;
ZONE_NAME [0]=0;
REPOP_TIME [0]=0;
REPOP_TYPE [0]=0;
MOB_IFFLAG [0]=0;
ALIAS [0]=0;
PADEZH1 [0]=0;
PADEZH2 [0]=0;
PADEZH3 [0]=0;
PADEZH4 [0]=0;
PADEZH5 [0]=0;
PADEZH6 [0]=0;
DESCR [0]=0;
LONG_DESCR [0]=0;
MOB_FLAG [0]=0;
MOB_AFF [0]=0;
ALIGN [0]=0;
MOB_LVL [0]=0;
MOB_HITROLL [0]=0;
MOB_AC [0]=0;
MOB_HIT [0]=0;
MOB_DAMAGE [0]=0;
MOB_MONEY [0]=0;
MOB_EXPERIENSE [0]=0;
MOB_POS_BOOT [0]=0;
MOB_POS_DEFAULT [0]=0;
MOB_SEX [0]=0;
ROOM_TITLE [0]=0;
ROOM_DESCR [0]=0;
ROOM_FLAGS [0]=0;
ROOM_SECTOR_TYPE [0]=0;
ROOM_EXIT_FLAG [0]=0;
ROOM_EXIT_KEY [0]=0;

// read config
fp=fopen("generator.cfg","r");
if (fp==NULL) {
printf("File generator.cfg not found\n");
return -1;
}

while(1) {
buf[0]=0;
fgets(buf, MAX_STR, fp);
if (buf[0]==0) break;
cc=strchr(buf,'\r'); if (cc) *cc=0;
cc=strchr(buf,'\n'); if (cc) *cc=0;
if (buf[0]==0) continue;
if (buf[0]=='#') continue;
cc=strchr(buf,' ');
if (cc==0) continue;
*cc=0;
if (!strcmp(buf,"ZONE")) strcpy(ZONE,cc+1);
else if (!strcmp(buf,"ZONE_NAME")) strcpy(ZONE_NAME,cc+1);
else if (!strcmp(buf,"REPOP_TIME")) strcpy(REPOP_TIME,cc+1);
else if (!strcmp(buf,"REPOP_TYPE")) strcpy(REPOP_TYPE,cc+1);
else if (!strcmp(buf,"MOB_IFFLAG")) strcpy(MOB_IFFLAG,cc+1);
else if (!strcmp(buf,"ALIAS")) strcpy(ALIAS,cc+1);
else if (!strcmp(buf,"PADEZH1")) strcpy(PADEZH1,cc+1);
else if (!strcmp(buf,"PADEZH2")) strcpy(PADEZH2,cc+1);
else if (!strcmp(buf,"PADEZH3")) strcpy(PADEZH3,cc+1);
else if (!strcmp(buf,"PADEZH4")) strcpy(PADEZH4,cc+1);
else if (!strcmp(buf,"PADEZH5")) strcpy(PADEZH5,cc+1);
else if (!strcmp(buf,"PADEZH6")) strcpy(PADEZH6,cc+1);
else if (!strcmp(buf,"DESCR")) strcpy(DESCR,cc+1);
else if (!strcmp(buf,"LONG_DESCR")) strcpy(LONG_DESCR,cc+1);
else if (!strcmp(buf,"MOB_FLAG")) strcpy(MOB_FLAG,cc+1);
else if (!strcmp(buf,"MOB_AFF")) strcpy(MOB_AFF,cc+1);
else if (!strcmp(buf,"ALIGN")) strcpy(ALIGN,cc+1);
else if (!strcmp(buf,"MOB_LVL")) strcpy(MOB_LVL,cc+1);
else if (!strcmp(buf,"MOB_HITROLL")) strcpy(MOB_HITROLL,cc+1);
else if (!strcmp(buf,"MOB_AC")) strcpy(MOB_AC,cc+1);
else if (!strcmp(buf,"MOB_HIT")) strcpy(MOB_HIT,cc+1);
else if (!strcmp(buf,"MOB_DAMAGE")) strcpy(MOB_DAMAGE,cc+1);
else if (!strcmp(buf,"MOB_MONEY")) strcpy(MOB_MONEY,cc+1);
else if (!strcmp(buf,"MOB_EXPERIENSE")) strcpy(MOB_EXPERIENSE,cc+1);
else if (!strcmp(buf,"MOB_POS_BOOT")) strcpy(MOB_POS_BOOT,cc+1);
else if (!strcmp(buf,"MOB_POS_DEFAULT")) strcpy(MOB_POS_DEFAULT,cc+1);
else if (!strcmp(buf,"MOB_SEX")) strcpy(MOB_SEX,cc+1);
else if (!strcmp(buf,"ROOM_TITLE")) strcpy(ROOM_TITLE,cc+1);
else if (!strcmp(buf,"ROOM_DESCR")) strcpy(ROOM_DESCR,cc+1);
else if (!strcmp(buf,"ROOM_FLAGS")) strcpy(ROOM_FLAGS,cc+1);
else if (!strcmp(buf,"ROOM_SECTOR_TYPE")) strcpy(ROOM_SECTOR_TYPE,cc+1);
else if (!strcmp(buf,"ROOM_EXIT_FLAG")) strcpy(ROOM_EXIT_FLAG,cc+1);
else if (!strcmp(buf,"ROOM_EXIT_KEY")) strcpy(ROOM_EXIT_KEY,cc+1);
else if (!strcmp(buf,"MOB_PARAMETERS")) {
	if (param_num>=MAX_PARAM) {
		printf("Too many MOB_PARAMETERS\n");
		continue;
		}
	else	{
		for(i=0;i<=MAX_STR;i++) {
			MOB_PARAMETERS[i][param_num]=*(cc+1+i);
			if (*(cc+1+i)==0) break;
			}
		param_num++;
		}
	}
else printf("debug: read '%s'\n", buf);
}

fclose(fp);

zone_num=atoi(ZONE);
if (zone_num<=0) {
printf("zone_num error. zone_num=%i\n", zone_num);
return -1;
}

// генерация имен файлов
sprintf(namez,"%i.zon",zone_num);
sprintf(namem,"%i.mob",zone_num);
sprintf(nameo,"%i.obj",zone_num);
sprintf(namet,"%i.trg",zone_num);
sprintf(namew,"%i.wld",zone_num);

if ((fz=fopen (namez, "w"))==NULL) exit(1);
if ((fm=fopen (namem, "w"))==NULL) exit(1);
if ((fo=fopen (nameo, "w"))==NULL) exit(1);
if ((ft=fopen (namet, "w"))==NULL) exit(1);
if ((fw=fopen (namew, "w"))==NULL) exit(1);

// формирование пустого trg файла
fprintf(ft,"* %s %s\n", COPYLEFT, ptime());
fputs("$\n$\n", ft);

// формирование пустого obj файла
fprintf(fo,"* %s %s\n", COPYLEFT, ptime());
fputs("$\n$\n", fo);

// формирование zon файла

fprintf(fz,"* %s %s\n", COPYLEFT, ptime());
fprintf(fz,"#%i\n",zone_num);
fprintf(fz,"%s~\n",ZONE_NAME);
fprintf(fz,"#1 0 0\n"); // for Zerkalo: эта строка нужна для Зеркала. код Виртустана работает и с ней и без неё
fprintf(fz,"%i99 %s %s\n",zone_num,REPOP_TIME,REPOP_TYPE);

// mobs to zonefile
for (i=0;i<90;i++)
    {
    mob_vnum=i;
    max_in_room=1;
    room_vnum=i;
    max_in_world=1;
    fprintf(fz,"M %s %i%02i %i %i%02i %i (mob number %i)\n",MOB_IFFLAG,
    zone_num, mob_vnum, max_in_room,
    zone_num, room_vnum, max_in_world, mob_vnum);
    }

fputs("S\n$\n$\n",fz);

// формирование mob файла

fprintf(fm, "* %s %s\n", COPYLEFT, ptime());

for (i=0;i<90;i++)
{ // генерация зверя
mob_vnum=i;
fprintf(fm, "#%i%02i\n", zone_num, mob_vnum);
fprintf(fm, "%s~\n", ALIAS);

fprintf(fm, "%s %i~\n", PADEZH1,i);
fprintf(fm, "%s %i~\n", PADEZH2,i);
fprintf(fm, "%s %i~\n", PADEZH3,i);
fprintf(fm, "%s %i~\n", PADEZH4,i);
fprintf(fm, "%s %i~\n", PADEZH5,i);
fprintf(fm, "%s %i~\n", PADEZH6,i);

fprintf(fm, "%s %i\n", DESCR, i);
fputs("~\n", fm);

fprintf(fm, "%s\n", LONG_DESCR);
fputs("~\n", fm);
fprintf(fm, "%s %s %s E\n", MOB_FLAG, MOB_AFF, ALIGN);
fprintf(fm, "%s %s %s %s %s\n", MOB_LVL, MOB_HITROLL, MOB_AC, MOB_HIT,
MOB_DAMAGE);
fprintf(fm, "%s %s\n", MOB_MONEY, MOB_EXPERIENSE);
fprintf(fm, "%s %s %s\n", MOB_POS_BOOT, MOB_POS_DEFAULT, MOB_SEX);

//fprintf(fm, "%s\n", MOB_PARAMETERS):

#if 1
for (j=0;j<param_num;j++) {
		for(ii=0;ii<=MAX_STR;ii++) {
			if (MOB_PARAMETERS[ii][j]==0) break;
			fprintf(fm,"%c",MOB_PARAMETERS[ii][j]);
			}
		fprintf(fm,"\n");
}
#endif

fputs("E\n",fm);
} // генерация зверя end

fputs("$\n$\n",fm);

// формирование файла wld
fprintf(fw,"* %s %s\n", COPYLEFT, ptime());

for (i=1;i<=9; i++)
	for (j=1;j<=10; j++)
		{
		room_vnum=CALC(i,j);
		fprintf(fw, "#%i\n",room_vnum);
		fprintf(fw, "%s (%i,%i)~\n", ROOM_TITLE, i, j);
		fprintf(fw,"   %s\n", ROOM_DESCR);
		fputs("~\n",fw);
		fprintf(fw, "%i %s %s\n", zone_num, ROOM_FLAGS, ROOM_SECTOR_TYPE);
		// выходы
		// D0 - D5 = север восток юг запад вверх вниз
		//           D0    D1     D2 D3    D4    D5
		if (i==1)
			{// первая строка
			if (j==1)
				{// угол (1,1)
				// выходы в ю
				EXIT(VOSTOK);
				EXIT(YUG);
				}
			else if (j==10)
				{// угол (1,10)
				// выходы з ю
				EXIT(YUG);
				EXIT(ZAPAD);
				}
			else	{// середина 1-й строки
				// выходы ю з в
				EXIT(YUG);
				EXIT(ZAPAD);
				EXIT(VOSTOK);
				}
			}
		else if (i==9)
			{// последняя строка
			if (j==1)
				{// угол (9,1)
				// выходы с в
				EXIT(SEVER);
				EXIT(VOSTOK);
				}
			else if (j==10)
				{// угол (9,10)
				// выходы с з
				EXIT(SEVER);
				EXIT(ZAPAD);
				}
			else	{// середина последней строки
				// выходы с з в
				EXIT(SEVER);
				EXIT(ZAPAD);
				EXIT(VOSTOK);
				}
			}
		else	{ // средняя строка
			if (j==1)
				{// левый (западный) край
				// выходы с ю в
				EXIT(SEVER);
				EXIT(VOSTOK);
				EXIT(YUG);
				}
			else if (j==10)
				{// правый (восточный) край
				// выходы с ю з
				EXIT(SEVER);
				EXIT(YUG);
				EXIT(ZAPAD);
				}
			else	{// самая серединная середина
				// выходы с в ю з
				EXIT(SEVER);
				EXIT(VOSTOK);
				EXIT(YUG);
				EXIT(ZAPAD);
				}
			}
		fputs("S\n",fw);
		}

fputs("$\n$\n", fw);

fclose(fz);
fclose(fm);
fclose(fo);
fclose(ft);
fclose(fw);

return 0;
}

// код из Virtustan MUD
char *ptime(void) // Возвращаемое значение: ссылка на текстовую строку с текущим временем
	{
	char *tmstr;
	time_t mytime;

	mytime = time(0);

	tmstr = (char *) asctime(localtime(&mytime));
	*(tmstr + strlen(tmstr) - 1) = '\0';

	return tmstr;

	}
// последняя строка исходника
