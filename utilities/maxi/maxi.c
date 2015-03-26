// Если в obj файле не указано максимальное количество предметов в мире (последняя строка описания объекта
// M 200
// то такой предмет в мире не появится и в магазине не появится (почему такое могло произойти: ошибка билдера
// или глюк olc при смене кодовой базы или формата obj файла)
//
// Эта прога это исправляет, добавляет M 200
//
// Обработка всего мира делается примерно так: ls -1 *.obj | awk '{print "./maxi "$1"; mv maxi.tmp "$1}' | sh
//
// prool, http://prool.in.ua
// 27.11.2013
//
#include<stdio.h>
#include<string.h>

#define MAXSTR 255

int main(int argc, char **argv)
{
FILE *f1, *f2;
char str[MAXSTR];
char pred[MAXSTR];

pred[0]='$';

//printf("argc %i\n", argc);
if (argc!=2) {printf("usage maxi file.obj\n"); return -1;}
//printf("hello world: `%s'\n\n", argv[1]);

f1=fopen(argv[1],"r");
if (!f1) {printf("Can't open %s\n", argv[1]); return -1;}

f2=fopen("maxi.tmp","w");
if (!f2) {printf("Can't open tmp file\n"); return -1;}

while(!feof(f1))
    {
    str[0]=0;
    fgets(str,MAXSTR,f1);
    //printf("%s",str);
    if ((str[0]=='#')||(str[0]=='$'))
	{
	if ((pred[0]!='M')&&(pred[0]!='$')&&(pred[0]!='*'))
	    {
	    printf("max in world not found\n");
	    fputs("M 200\n", f2);
	    }
	}
    fputs(str,f2);
    strcpy(pred,str);
    }
return 0;
}
