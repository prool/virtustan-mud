// zone generator by proolix@gmail.com

#include "config.h"

#define NAMELEN 200
#define COPYLEFT "Created by Zone Generator by proolix@gmail.com"

#define SEVER 0
#define VOSTOK 1
#define YUG 2
#define ZAPAD 3
#define VVERH 4
#define VNIZ 5
#define CALC(i,j) j-1+(i-1)*10+zone*100
#define EXIT(DIREKT) fprintf(fw, "D%i\n~\n~\n%i %i %i\n", DIREKT, ROOM_EXIT_FLAG, ROOM_EXIT_KEY, exit_num(DIREKT, i, j, zone))

#include <stdio.h>

int exit_num (int direkt, int i, int j, int zone)
{
switch (direkt)
	{
	case SEVER: return CALC(i-1,j);
	case YUG: return CALC(i+1,j);
	case ZAPAD: return CALC(i,j-1);
	case VOSTOK: return CALC(i,j+1);
	case VVERH: return CALC(i,j)+100;
	case VNIZ: return CALC(i,j)-100;
	default: return 0;
	}
}

int generate_plain (int zone)
{
int i, j;
int mob_vnum, room_vnum, max_in_room, max_in_world;

FILE *fz;
FILE *fm;
FILE *fo;
FILE *ft;
FILE *fw;
FILE *fs;

char namez[NAMELEN];
char namem[NAMELEN];
char nameo[NAMELEN];
char namet[NAMELEN];
char namew[NAMELEN];
char names[NAMELEN];

// генерация имен файлов
sprintf(namez,"%i.zon",zone);
sprintf(namem,"%i.mob",zone);
sprintf(nameo,"%i.obj",zone);
sprintf(namet,"%i.trg",zone);
sprintf(namew,"%i.wld",zone);
sprintf(names,"%i.shp",zone);

if ((fz=fopen (namez, "w"))==NULL) exit(1);
if ((fm=fopen (namem, "w"))==NULL) exit(1);
if ((fo=fopen (nameo, "w"))==NULL) exit(1);
if ((ft=fopen (namet, "w"))==NULL) exit(1);
if ((fw=fopen (namew, "w"))==NULL) exit(1);
if ((fs=fopen (names, "w"))==NULL) exit(1);

// формирование пустого trg файла
fprintf(ft,"* %s\n", COPYLEFT);
fputs("$\n$\n", ft);

// формирование пустого obj файла
fprintf(fo,"* %s\n", COPYLEFT);
fputs("$\n$\n", fo);

// формирование пустого shp файла

fputs("CircleMUD v3.0 Shop File~\n$\n$\n~\n", fs);

// формирование zon файла

fprintf(fz,"* %s\n", COPYLEFT);
fprintf(fz,"#%i\n",zone);
fprintf(fz,"%s~\n",ZONE_NAME);
fprintf(fz,"%i99 %i %i\n",zone,REPOP_TIME,REPOP_TYPE);

// mobs to zonefile
for (i=0;i<90;i++)
    {
    mob_vnum=i;
    max_in_room=1;
    room_vnum=i;
    max_in_world=1;
    fprintf(fz,"M %s %i%02i %i %i%02i %i (mob number %i)\n",MOB_IFFLAG,
    zone, mob_vnum, max_in_room,
    zone, room_vnum, max_in_world, mob_vnum);
    // objects to mob
    fprintf(fz,"G %i %i %i %i %i (add object to mob)\n", OBJ_IFFLAG, OBJ_VNUM, OBJ_MAX_IN_WORLD,
    OBJ_VEROYATN, OBJ_NADET);
    }

fputs("S\n$\n$\n",fz);

// формирование mob файла

fprintf(fm, "* %s\n", COPYLEFT);

for (i=0;i<90;i++)
{ // генерация зверя
mob_vnum=i;
fprintf(fm, "#%i%02i\n", zone, mob_vnum);
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
fprintf(fm, "%s %s %i E\n", MOB_FLAG, MOB_AFF, ALIGN);
fprintf(fm, "%i %i %i %s %s\n", MOB_LVL, MOB_HITROLL, MOB_AC, MOB_HIT,
MOB_DAMAGE);
fprintf(fm, "%s %i\n", MOB_MONEY, MOB_EXPERIENSE);
fprintf(fm, "%i %i %i\n", MOB_POS_BOOT, MOB_POS_DEFAULT, MOB_SEX);

fprintf(fm, "%s\n", MOB_PARAMETERS);

fputs("E\n",fm);
} // генерация зверя end

fputs("$\n$\n",fm);

// формирование файла wld
fprintf(fw,"* %s\n", COPYLEFT);

for (i=1;i<=9; i++)
	for (j=1;j<=10; j++)
		{
		room_vnum=CALC(i,j);
		fprintf(fw, "#%i\n",room_vnum);
		fprintf(fw, "%s (%i,%i,%i)~\n", ROOM_TITLE, i, j,zone-ZONE_START);
		if (zone==ZONE_START) fprintf(fw,"   %s\n", ROOM_DESCR1);
		else fprintf(fw,ROOM_DESCR2,(zone-ZONE_START)*10);
		fputs("~\n",fw);
		fprintf(fw, "%i %i %i\n", zone, ROOM_FLAGS, ROOM_SECTOR_TYPE);
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
				// выходы ю
				EXIT(VOSTOK);
				EXIT(YUG);
				EXIT(ZAPAD);
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
				// выходы с
				EXIT(SEVER);
				EXIT(VOSTOK);
				EXIT(ZAPAD);
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
		if (zone==ZONE_START) EXIT(VVERH);
		else if (zone==ZONE_END) EXIT(VNIZ);
		else	{
			EXIT(VVERH);
			EXIT(VNIZ);
			}
		fputs("S\n",fw);
		}

fputs("$\n$\n", fw);

fclose(fz);
fclose(fm);
fclose(fo);
fclose(ft);
fclose(fw);
fclose(fs);

return 0;
}

int main (void)
{int i;

for (i=ZONE_START;i<=ZONE_END;i++)
    generate_plain(i);
    
printf("\n\nGeneration done.\n\n");

return 0;
}
