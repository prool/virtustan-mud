// convertor .obj files from VirtustanMUD format (new Byliny) to Byliny-0 format (old Byliny)
// delete "M" parameter

#include <stdio.h>

void main(void)
{
int status;
char str[255];

while(1)
	{
	str[0]=0;
	gets(str);
	if ((str[0]=='M')&&(str[1]==' ')) continue;
	printf("%s\n",str);
	if (str[0]==0) break;
	}
}
