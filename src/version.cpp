// $RCSfile$     $Date$     $Revision$
// Copyright (c) 2007 Krodo
// Part of Bylins http://www.mud.ru

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "comm.h"
#include "utils.h"

extern char mudname[];

// * Весь файл - исключительно как попытка автоматической вставки в код нормальной даты сборки.

void show_code_date(CHAR_DATA *ch)
{
	send_to_char(ch, "Virtustan MUD, ver %s %s\r\nBy Prool http://mud.kharkov.org\r\n", __DATE__, __TIME__);
	if (mudname[0])
		send_to_char(ch, "&R%s&n\r\n",mudname);
}

void log_code_date()
{
	log("Code version %s %s", __DATE__, __TIME__);
}
