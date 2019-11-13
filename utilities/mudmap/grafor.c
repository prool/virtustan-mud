#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
//#include <ncurses.h>

#define STRLEN 500
#define NAMELEN 50
#define MAXROOM 80000 // 66401 // 8000
#define ESC 0x1B
#define MAXINDEX 340000
#define MEGA_X 600 // 100 // 505
#define MEGA_Y 1000 // 100 // 780
#define MEGA_Z 110 // 102 // 15

// cyrillic comments in koi8-r codetable

int megamap [MEGA_X] [MEGA_Y] [MEGA_Z];

struct rooms
	{
	int vnum;
	int d[6];
	int coord;
	int x,y,z;
	int error;
	char name [NAMELEN];
	};

struct rooms room[MAXROOM];

int square_x=0;
int square_y=0;
char *room_color[] = {"gray", "yellow", "white", "red"};
int zerodir[6]={0,0,0,0,0,0};

int STARTZONE=99;

char *ptime(void) // Возвращаемое значение: ссылка на текстовую строку с текущим временем
	{
	char *tmstr;
	time_t mytime;

	mytime = time(0);

	tmstr = (char *) asctime(localtime(&mytime));
	*(tmstr + strlen(tmstr) - 1) = '\0';

	return tmstr;

	}

char title_buf [100];

char *room_title(int room_type, int vnum, char *name)
{
if (room_type) {
snprintf(title_buf,100,"<title>[%i] %s</title>",vnum,name); return title_buf;}
else return " ";
}

void square (int type, int dir[6], int vnum, char *name)
{
printf(
"<rect x=\"%i\" y=\"%i\" width=\"10\" height=\"10\" fill=\"%s\" stroke=\"black\" stroke-width=\"0\">%s</rect>\n",
square_x, square_y, room_color[type], room_title(type,vnum,name));

if (type)
	{
if (!dir[0]) printf("<rect x=\"%i\" y=\"%i\" width=\"10\" height=\"1\" fill=\"brown\" stroke=\"black\" stroke-width=\"0\"/>\n",
square_x, square_y); // гориз. линия

if (!dir[2]) printf("<rect x=\"%i\" y=\"%i\" width=\"10\" height=\"1\" fill=\"brown\" stroke=\"black\" stroke-width=\"0\"/>\n",
square_x, square_y+9); // гориз. линия 2

if (!dir[3]) printf("<rect x=\"%i\" y=\"%i\" width=\"1\" height=\"10\" fill=\"brown\" stroke=\"black\" stroke-width=\"0\"/>\n",
square_x, square_y); // вертик. линия

if (!dir[1]) printf("<rect x=\"%i\" y=\"%i\" width=\"1\" height=\"10\" fill=\"brown\" stroke=\"black\" stroke-width=\"0\"/>\n",
square_x+9, square_y); // вертик. линия 2

if (dir[4]) printf("<circle cx=\"%i\" cy=\"%i\" r=\"3\" fill=blue />", square_x+5, square_y+5);

if (dir[5])
		{
printf("<rect x=\"%i\" y=\"%i\" width=\"1\" height=\"7\" fill=\"black\" stroke=\"black\" stroke-width=\"0\"/>\n",
square_x+5, square_y+2);
printf("<rect x=\"%i\" y=\"%i\" width=\"7\" height=\"1\" fill=\"black\" stroke=\"black\" stroke-width=\"0\"/>\n",
square_x+2, square_y+5);
		}
	}

square_x+=10;
}

int active_room(int vnum) // заранее исключаем из рассмотрения зоны с известными ошибками в геометрии и просто мешающие зоны
{
if (vnum==0) return 0;
#if 0 // 1 for VMUD, 0 for others muds
if (vnum==1138) return 0;
if ((vnum>=100)&&(vnum<=198)) return 0; // Зона богов
if ((vnum>=9965)&&(vnum<=9971)) return 0;
if ((vnum>=1700)&&(vnum<1799)) return 0;
if ((vnum>=8800)&&(vnum<8899)) return 0;
if ((vnum>=28700)&&(vnum<28799)) return 0;
if ((vnum>=88400)&&(vnum<88499)) return 0;
#endif
return 1;
}

void help (void)
{
#if 0
clear();
printw("Help:\n\n\
x - increment X\n\
x - decrement X\n\
Y - increment Y\n\
y - decrement Y\n\
Z - increment Z\n\
z - decrement Z\n\
arrow keys or 8, 4, 6, 2 - move room cursor (hethack style move)\n\
h - help\n\
\n\
Prool. proolix@gmail.com http://mud.kharkov.ru\n\
");

getch();
#endif
}


int main (int argc, char *argv[], char **env)

{FILE *f1, *html;
char str[STRLEN];
char string[STRLEN];
int r, i, j, k;
int count;
int direct, i1, i2, i3;
int room0;
int iter;
int linked;
int max_x, max_y, max_z, min_x, min_y, min_z;
char c;
int x, y, x0, y0, z0, dx, dy;
int q;
char key;
int color;
int newx, newy, newz;
int cursor_x, cursor_y, cursor_room;
int cursor_i;
int status=0;
int processed=1;
int max_room_vnum=0;
int x1, y1, z1;
char param[STRLEN];
char var[STRLEN];
char MUDname [STRLEN];
int value;
char *pp, *pp2;
char *cc;

int index [MAXINDEX];
FILE *fconfig;

z0=0;
x0=-41;
y0=-33;

strcpy(MUDname,"Mud");

// process config file

fconfig=fopen("grafor.cfg","r");
if (fconfig)
	{
	while (!feof(fconfig))
		{char *pp;
		string[0]=0;
		fgets(string,STRLEN,fconfig);
		pp=strchr(string,'\n');
		if (pp) *pp=0;
		// printf("`%s'\n", string); // debug print
		if (!strcmp(string,"test")) printf("<!-- config file test ok -->\n");
		else if (!memcmp(string,"startzone ",strlen("startzone ")))
			{
			cc=string;
			i=atoi(cc+strlen("startzone "));
			if (i) STARTZONE=i;
			}
		else if (!memcmp(string,"mudname ",strlen("mudname ")))
			{
			cc=strchr(string,' ');
			if (cc) cc++;
			if (*cc) strcpy(MUDname,cc);
			}
		}
	fclose(fconfig);
	}
else
	{
	//printf("grafor.cfg not found\n");
	}
// end of process config file

printf("<!-- grafor (prool's mud map maker) - http://mud.kharkov.org -->\n");
// process command line
//printf("<!-- argc = %i -->", argc);
if (argc>=2)
	{
	//printf("<!-- argv[1]=%s -->",argv[1]);
	i=atoi(argv[1]);
	if (i!=0) STARTZONE=i;
	}

for(i=1;i<argc;i++)
	{
	printf("<!-- argv[i]=%s -->\n",argv[i]);
	j=atoi(argv[i]);
	if (j!=0)
		switch (i)
		{
		case 1: STARTZONE=j; break;
		case 2: x0=j; break;
		case 3: y0=j; break;
		case 4: z0=j; break;
		}
	}

#if 1 // BIG IF
count=-1;

for (i=0; i<MEGA_X; i++)
	for (j=0; j<MEGA_Y; j++)
		for (k=0; k<MEGA_Z; k++)
			megamap[i][j][k]=-1;

for (i=0;i<MAXINDEX;i++) index[i]=-1;

for (i=0;i<MAXROOM;i++)
	{
	room[i].vnum=0;
	room[i].coord=0;
	room[i].x=0;
	room[i].y=0;
	room[i].z=0;
	room[i].error=0;
	for (j=0;j<6;j++) room[i].d[j]=0;
	for (j=0; j<NAMELEN; j++) room[i].name[j]=0;
	}

dx=101; //dx=38+58+1;
dy=67; //dy=32+34+1;
        
cursor_x=0;
cursor_y=0;

i=0;
#define Q_STR "QUERY_STRING"
while(env[i])
        {
        if (!memcmp(env[i],Q_STR,strlen(Q_STR)))
                {
                strcpy(param,env[i]+strlen(Q_STR));
                break;
                }
        i++;
        }

//printf("param %s ", param);

// parse param
pp=param;
if (*pp=='=') pp++;
while(*pp)
	{
	pp2=strchr(pp,'=');
	if (!pp2) break;
	for(i=0;i<STRLEN;i++)var[i]=0;
	memcpy(var,pp,pp2-pp);
	//printf("var=`%s' ",var);
	pp=pp2+1;
	if (!*pp) break;
	value=atoi(pp);
	//printf("value=%i ",value);
	// присваивание
	if (!strcmp(var,"x0")) x0=value;
	else if (!strcmp(var,"y0")) y0=value;
	else if (!strcmp(var,"z0")) z0=value;
	else if (!strcmp(var,"dx")) dx=value;
	else if (!strcmp(var,"dy")) dy=value;
	else if (!strcmp(var,"startzone")) STARTZONE=value;
	pp2=strchr(pp,'&');
	if (!pp2) break;
	pp=pp2+1;
	}

// Первый проход
f1=fopen("rooms.lst","r");
if (f1==NULL) {printf("File rooms.lst not found!\n"); return 1;}
while(!feof(f1))
	{
	fgets(str,STRLEN,f1);
	//printf("%s",str);
	if (str[0]=='#')
		{char *cc;
		r=atoi(str+1);
		//printf("\nRoom #%i ", r);
		count++;
		if (count>=MAXROOM)
			{printf("Error:Too many rooms: Maxroom=%i\n",(int)MAXROOM);
			exit(1);
			}
		room[count].vnum=r;
		if (r>max_room_vnum) max_room_vnum=r;
		for (j=0; j<NAMELEN; j++) room[count].name[j]=0;
		fgets(room[count].name,NAMELEN,f1);
		//room[count].name[strlen(room[count].name)-2]=0;
		cc=strchr(room[count].name,'~');
		if (cc) *cc=0;
		else room[count].name[NAMELEN-1]=0;
		}
	else if (str[0]=='D')
		{
		direct=atoi(str+1);
		fgets(str,STRLEN,f1);
		fgets(str,STRLEN,f1);
		fgets(str,STRLEN,f1);
		sscanf(str,"%i %i %i", &i1, &i2, &i3);
		//printf("dir=%i i1=%i i2=%i i3=%i\n", direct, i1, i2, i3);
		if ((direct>=0)&&(direct<=5))
			{
			room[count].d[direct]=i3;
			}
		}
	}
printf("<!-- Rooms: %i Max room number %i --> \n", count+1, max_room_vnum);
fclose(f1);

#if 0
// контрольный вывод
for (i=0; i<MAXROOM; i++)
	{
	if (room[i].vnum)
		{
		printf("Room %i ",room[i].vnum);
		for (j=0;j<6;j++)
			if (room[i].d[j])
				printf("D%i=%i ", j, room[i].d[j]);
		printf("\n");
		}
	}
#endif

// второй проход
// формирование индексной таблицы
for (i=0;i<MAXROOM;i++)
	if (room[i].vnum)
		index[room[i].vnum]=i;

room0=STARTZONE*100;
cursor_room=room0;
i=index[room0];
if (i==-1)
	{
	printf("Start room not found\n");
	exit(2);
	}
room[i].coord=1;
room[i].x=0;
room[i].y=0;
room[i].z=0;

// итерации
linked=1;
while (linked) //for (iter=0;iter<ITER;iter++)
	{
	//printf("I %i ", processed);
	fflush(NULL);
	linked=0;
	for (i=0;i<MAXROOM;i++)
		if (active_room(room[i].vnum))
			if (room[i].coord)
				{
				for (j=0;j<6;j++)
					if (room[i].d[j])
						{
						k=index[room[i].d[j]];
						if (k==-1)
								{
								room[i].error=1;
								printf("<!-- error 1-->\n");
								}
						else
								{
								if (room[k].coord==0)
								{
								room[k].coord=1; linked=1; processed++;
								switch(j)
									{
									case 0: //sever
									room[k].x=room[i].x; room[k].y=room[i].y+1;
									room[k].z=room[i].z; break;
									case 1: // vost
									room[k].x=room[i].x+1; room[k].y=room[i].y;
									room[k].z=room[i].z; break;
									case 2: // yug
									room[k].x=room[i].x; room[k].y=room[i].y-1;
									room[k].z=room[i].z; break;
									case 3: // zapad
									room[k].x=room[i].x-1; room[k].y=room[i].y;
									room[k].z=room[i].z; break;
									case 4: // verh
									room[k].x=room[i].x; room[k].y=room[i].y;
									room[k].z=room[i].z+1; break;
									case 5: // vniz
									room[k].x=room[i].x; room[k].y=room[i].y;
									room[k].z=room[i].z-1; break;
									}
								}
								else
								{ // room[k].coord == 1
								switch(j)
									{
									case 0: //sever
									newx=room[i].x; newy=room[i].y+1;
									newz=room[i].z; break;
									case 1: // vost
									newx=room[i].x+1; newy=room[i].y;
									newz=room[i].z; break;
									case 2: // yug
									newx=room[i].x; newy=room[i].y-1;
									newz=room[i].z; break;
									case 3: // zapad
									newx=room[i].x-1; newy=room[i].y;
									newz=room[i].z; break;
									case 4: // verh
									newx=room[i].x; newy=room[i].y;
									newz=room[i].z+1; break;
									case 5: // vniz
									newx=room[i].x; newy=room[i].y;
									newz=room[i].z-1; break;
									}
								if (!((newx==room[k].x)&&(newy==room[k].y)&&(newz==room[k].z)))
									{
									//printf("<!-- error 2 %i->%i -->\n",room[i].vnum,room[k].vnum);
									//fflush(NULL);
									room[i].error=1;
									}
								}
								}
						}
				}
	}
//printf("\n");
// подсчет координат
max_x=room[0].x;
max_y=room[0].y;
max_z=room[0].z;
min_x=room[0].x;
min_y=room[0].y;
min_z=room[0].z;
for (i=0; i<MAXROOM; i++)
	{
	if ((room[i].vnum) && (room[i].coord))
		{
		if (max_x<room[i].x) max_x=room[i].x;
		if (max_y<room[i].y) max_y=room[i].y;
		if (max_z<room[i].z) max_z=room[i].z;
		if (min_x>room[i].x) min_x=room[i].x;
		if (min_y>room[i].y) min_y=room[i].y;
		if (min_z>room[i].z) min_z=room[i].z;
		}
	}
//printf("X %i %i Y %i %i Z %i %i\n", min_x, max_x, min_y, max_y, min_z, max_z);

if ( max_x-min_x+1>MEGA_X)
	{
	printf("MEGA_X is small!\n");
	exit(3);
	}

if ( max_y-min_y+1>MEGA_Y)
	{
	printf("MEGA_Y is small!\n");
	exit(3);
	}

if ( max_z-min_z+1>MEGA_Z)
	{
	printf("MEGA_Z is small! max_z=%i min_z=%i \n", max_z, min_z );
	exit(3);
	}

//printf("press any enter"); getchar();

// формирование мегакарты
for (i=0; i<MAXROOM; i++)
	if ((room[i].vnum)&&(room[i].coord))
		{
		megamap[room[i].x-min_x][room[i].y-min_y][room[i].z-min_z]=room[i].vnum;
		}

#if 0
// вывод мегакарты в файл
html=fopen("map0.html","w");
fprintf(html,"\n Mega map <br><br>\n\n");
for (y=max_y-min_y+1/*MEGA_Y-1*/; y>=0; y--)
	{
	for (x=0; x<=(max_x-min_x+1); x++)
		{
		c='.';
		if (megamap[x][y][-min_z]!=-1) c='*';
		if ((megamap[x][y][-min_z]>=4000)&&(megamap[x][y][-min_z]<4099)) c='#';
		//fprintf(html,"%c ",c);
		switch(c)
			{
			case '*':
			case '#':
				fprintf(html,"<img src=r.gif title=\"%s\">",room[index[megamap[x][y][-min_z]]].name); break;
			default: 
				fprintf(html,"<img src=z.gif>"); break;
			}
		}
	fprintf(html,"<br>\n");
	}
fprintf(html,"\n<br><br>\n--- end of megamap\n");
fclose(html);
#endif

// вывод карты

q=1;

key=0;

#endif // BIG IF

// вывод фрагмента мегакарты
printf("<html>\n"
"<head>\n"
"<title>MUD map</title>\n");

printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=koi8-r\">"
"</head>\n"
"<body>\n\
\n");

#if 1 // BIG IF 2

printf("%s map. Generated by Proolgenerator %s. Start zone=%i x0=%i y0=%i z0=%i\n", MUDname, ptime(), STARTZONE, x0, y0, z0);

#if 0
printf("startzone=%i X=%03i \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[&lArr;]</a> \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[&rArr;]</a> \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[&larr;]</a> \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[&rarr;]</a> \
Y=%03i \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[&uArr;]</a> \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[&dArr;]</a> \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[&uarr;]</a> \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[&darr;]</a> \
Z=%02i \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[&uarr;]</a> \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[&darr;]</a> \
dx=%02i \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[+]</a> \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[-]</a> \
dy=%02i \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[+]</a> \
<a href=script.cgi?x0=%i&y0=%i&z0=%i&dx=%i&dy=%i&startzone=%i>[-]</a> \
<br>\n",
STARTZONE,
x0,
x0+10, y0, z0, dx, dy, STARTZONE,
x0-10, y0, z0, dx, dy, STARTZONE,
x0+ 1, y0, z0, dx, dy, STARTZONE,
x0- 1, y0, z0, dx, dy, STARTZONE,
y0,
x0, y0-10, z0, dx, dy, STARTZONE,
x0, y0+10, z0, dx, dy, STARTZONE,
x0, y0- 1, z0, dx, dy, STARTZONE,
x0, y0+ 1, z0, dx, dy, STARTZONE,
z0,
x0, y0, z0+1, dx, dy, STARTZONE,
x0, y0, z0-1, dx, dy, STARTZONE,
dx,
x0, y0, z0, dx+1, dy, STARTZONE,
x0, y0, z0, dx-1, dy, STARTZONE,
dy,
x0, y0, z0, dx, dy+1, STARTZONE,
x0, y0, z0, dx, dy-1, STARTZONE );
#endif

printf("\
<svg version = \"1.1\"\n\
baseProfile=\"full\"\n\
xmlns = \"http://www.w3.org/2000/svg\"\n\
xmlns:xlink = \"http://www.w3.org/1999/xlink\"\n\
xmlns:ev = \"http://www.w3.org/2001/xml-events\"\n\
height = \"600px\"  width = \"1350\">\n\
\n");

for (y=y0+dy; y>y0; y--)
	{
	for (x=x0; x<(x0+dx); x++)
		{
		c=' '; color=1;
		x1=x-min_x;
		y1=y-min_y;
		z1=z0-min_z;
		if ((x1<0)||(y1<0)||(z1<0)||(x1>=MEGA_X)||(y1>=MEGA_Y)||(z1>=MEGA_Z)) c='.';
		else if (megamap[x1][y1][z1]!=-1)
			{
			c='*';
					if ((megamap[x1][y1][z1]>=STARTZONE*100) && (megamap[x1][y1][z1]<STARTZONE*100+99))
						{c='#';}
					if ((x==cursor_x)&&(y==cursor_y))
						{c='*'; color=5; cursor_room=megamap[x1][y1][z1];}
					if (room[index[megamap[x1][y1][z1]]].error) {c='E'; color=3;}
			}
#if 0
		if (c=='*') printf("<img src=r.gif title=\"%s\">",room[index[megamap[x1][y1][z1]]].name);
		else printf("<img src=z.gif>");
#endif


		switch(c)
			{
			case '*': square(1, room[index[megamap[x1][y1][z1]]].d,room[index[megamap[x1][y1][z1]]].vnum,room[index[megamap[x1][y1][z1]]].name); break;
			case '#': square(2, room[index[megamap[x1][y1][z1]]].d,room[index[megamap[x1][y1][z1]]].vnum,room[index[megamap[x1][y1][z1]]].name); break;
			case 'E': square(3, room[index[megamap[x1][y1][z1]]].d,room[index[megamap[x1][y1][z1]]].vnum,room[index[megamap[x1][y1][z1]]].name); break;
			default: if ((x1>=0)&&(y1>=0)&&(z1>=0)) square(0, room[index[megamap[x1][y1][z1]]].d,room[index[megamap[x1][y1][z1]]].vnum,room[index[megamap[x1][y1][z1]]].name);
				 else square(0, zerodir,0," ");
			}
		}
	square_y+=10; square_x=0; //printf("<br>\n");
	}
//printf("Room %i %s", cursor_room, room[index[cursor_room]].name);

#if 0
while ((key=getch())==ERR) {}
printf("key=%i\n", key);
if (status==0)
switch(key)
	{
	case 'q': q=0;
		break;
	case 'z': z0--; break;
	case 'Z': z0++; break;
	case 'x': x0--; break;
	case 'X': x0++; break;
	case 'y': y0--; break;
	case 'Y': y0++; break;
	case '8': cursor_y++; break;
	case '2': cursor_y--; break;
	case '4': cursor_x--; break;
	case '6': cursor_x++; break;
	case 'h': help(); break;
	case ESC: status=1;
	default: ;
	}
else
{
status=0;
switch(getch())
	{
	case 'A': cursor_y++; break;
	case 'B': cursor_y--; break;
	case 'D': cursor_x--; break;
	case 'C': cursor_x++; break;
	default: ;
	}
}
#endif

#endif // BIG IF 2

#if 0 // BIG IF 3
#define MAX_X 135
#define MAX_Y 60

for (j=0; j<MAX_Y; j++)
	{

	for (i=0; i<MAX_X; i++) // вывод строки
		{
		x=i*10+1;
		y=j*10+1;
		printf("<rect x=\"%i\" y=\"%i\" width=\"10\" height=\"10\" fill=\"%s\" stroke=\"black\" stroke-width=\"0\"/>\n",
		x, y, (i==0)||(i==MAX_X-1)||(j==MAX_Y-1)?"brown":"yellow");
		}

	}
#endif // BIG IF 3

printf("\n\
</svg>\n");

printf("</body>\n");
printf("</html>\n");

return 0;
} // end of main()

