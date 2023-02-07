// zone generator 0.1 by prool@itl.ua

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"

#define NAMELEN 200
#define COPYLEFT "Created by Zone Generator by proolix@gmail.com"

#define SEVER 0
#define VOSTOK 1
#define YUG 2
#define ZAPAD 3
#define VVERH 4
#define VNIZ 5
#define CALC(i,j) j-1+(i-1)*10+ZONE*100
#define EXIT(DIREKT) fprintf(fw, "D%i\n~\n~\n%i %i %i\n", DIREKT, ROOM_EXIT_FLAG, ROOM_EXIT_KEY, exit_num(DIREKT, i, j));

char *ptime(void);  // ������������ ��������: ������ �� ��������� ������ � ������� ��������

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
int i, j;
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

// ��������� ���� ������
sprintf(namez,"%i.zon",ZONE);
sprintf(namem,"%i.mob",ZONE);
sprintf(nameo,"%i.obj",ZONE);
sprintf(namet,"%i.trg",ZONE);
sprintf(namew,"%i.wld",ZONE);

if ((fz=fopen (namez, "w"))==NULL) exit(1);
if ((fm=fopen (namem, "w"))==NULL) exit(1);
if ((fo=fopen (nameo, "w"))==NULL) exit(1);
if ((ft=fopen (namet, "w"))==NULL) exit(1);
if ((fw=fopen (namew, "w"))==NULL) exit(1);

// ������������ ������� trg �����
fprintf(ft,"* %s %s\n", COPYLEFT, ptime());
fputs("$\n$\n", ft);

// ������������ ������� obj �����
fprintf(fo,"* %s %s\n", COPYLEFT, ptime());
fputs("$\n$\n", fo);

// ������������ zon �����

fprintf(fz,"* %s %s\n", COPYLEFT, ptime());
fprintf(fz,"#%i\n",ZONE);
fprintf(fz,"%s~\n",ZONE_NAME);
fprintf(fz,"#1 0 0\n"); // for Zerkalo: ��� ������ ����� ��� �������. ��� ���������� �������� � � ��� � ��� �ţ
fprintf(fz,"%i99 %i %i\n",ZONE,REPOP_TIME,REPOP_TYPE);

// mobs to zonefile
for (i=0;i<90;i++)
    {
    mob_vnum=i;
    max_in_room=1;
    room_vnum=i;
    max_in_world=1;
    fprintf(fz,"M %s %i%02i %i %i%02i %i (mob number %i)\n",MOB_IFFLAG,
    ZONE, mob_vnum, max_in_room,
    ZONE, room_vnum, max_in_world, mob_vnum);
    }

fputs("S\n$\n$\n",fz);

// ������������ mob �����

fprintf(fm, "* %s %s\n", COPYLEFT, ptime());

for (i=0;i<90;i++)
{ // ��������� �����
mob_vnum=i;
fprintf(fm, "#%i%02i\n", ZONE, mob_vnum);
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
} // ��������� ����� end

fputs("$\n$\n",fm);

// ������������ ����� wld
fprintf(fw,"* %s\n", COPYLEFT);

for (i=1;i<=9; i++)
	for (j=1;j<=10; j++)
		{
		room_vnum=CALC(i,j);
		fprintf(fw, "#%i\n",room_vnum);
		fprintf(fw, "%s (%i,%i)~\n", ROOM_TITLE, i, j);
		fprintf(fw,"   %s\n", ROOM_DESCR);
		fputs("~\n",fw);
		fprintf(fw, "%i %i %i\n", ZONE, ROOM_FLAGS, ROOM_SECTOR_TYPE);
		// ������
		// D0 - D5 = ����� ������ �� ����� ����� ����
		//           D0    D1     D2 D3    D4    D5
		if (i==1)
			{// ������ ������
			if (j==1)
				{// ���� (1,1)
				// ������ � �
				EXIT(VOSTOK);
				EXIT(YUG);
				}
			else if (j==10)
				{// ���� (1,10)
				// ������ � �
				EXIT(YUG);
				EXIT(ZAPAD);
				}
			else	{// �������� 1-� ������
				// ������ � � �
				EXIT(YUG);
				EXIT(ZAPAD);
				EXIT(VOSTOK);
				}
			}
		else if (i==9)
			{// ��������� ������
			if (j==1)
				{// ���� (9,1)
				// ������ � �
				EXIT(SEVER);
				EXIT(VOSTOK);
				}
			else if (j==10)
				{// ���� (9,10)
				// ������ � �
				EXIT(SEVER);
				EXIT(ZAPAD);
				}
			else	{// �������� ��������� ������
				// ������ � � �
				EXIT(SEVER);
				EXIT(ZAPAD);
				EXIT(VOSTOK);
				}
			}
		else	{ // ������� ������
			if (j==1)
				{// ����� (��������) ����
				// ������ � � �
				EXIT(SEVER);
				EXIT(VOSTOK);
				EXIT(YUG);
				}
			else if (j==10)
				{// ������ (���������) ����
				// ������ � � �
				EXIT(SEVER);
				EXIT(YUG);
				EXIT(ZAPAD);
				}
			else	{// ����� ���������� ��������
				// ������ � � � �
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

// ��� �� Virtustan MUD
char *ptime(void) // ������������ ��������: ������ �� ��������� ������ � ������� ��������
	{
	char *tmstr;
	time_t mytime;

	mytime = time(0);

	tmstr = (char *) asctime(localtime(&mytime));
	*(tmstr + strlen(tmstr) - 1) = '\0';

	return tmstr;

	}
// ��������� ������ ���������
