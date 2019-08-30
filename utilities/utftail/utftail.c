// tail -f syslog with recode koi8-r -> UTF-8
// by Prool
// GPL3

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iconv.h>

#define STRLEN 1024

void koi_to_utf8(char *str_i, char *str_o)
{
	iconv_t cd;
	size_t len_i, len_o = STRLEN * 6;
	size_t i;

	if ((cd = iconv_open("UTF-8","KOI8-RU")) == (iconv_t) - 1)
	{
		printf("koi_to_utf8: iconv_open error\n");
		return;
	}
	len_i = strlen(str_i);
	// const_cast at the next line is required for Linux, because there iconv has non-const input argument.
	if ((i = iconv(cd, &str_i, &len_i, &str_o, &len_o)) == (size_t) - 1)
	{
		printf("koi_to_utf8: iconv error\n");
		return;
	}
	*str_o = 0;
	if (iconv_close(cd) == -1)
	{
		printf("koi_to_utf8: iconv_close error\n");
		return;
	}
}

int main (void)
{
FILE *fp;
char str[STRLEN];
char str2[STRLEN*2];

printf("tail -f syslog with recode koi8-r->UTF-8. GPL3. By Prool\r\n");

fp=fopen("syslog","r");
if (fp==0) {printf("Cant open syslog\n"); return 1;}

// skip lines

while(1)
	{
	str[0]=0;
	fgets(str,STRLEN,fp);
	if (str[0]==0) break;
	//printf("%s",str);
	}

while(1)
	{
	str[0]=0;
	fgets(str,STRLEN,fp);
	if (str[0]==0) {sleep(1);continue;}
	koi_to_utf8(str,str2);
	printf("%s",str2);
	}
return 0;
}
