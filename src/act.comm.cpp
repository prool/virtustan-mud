/*************************************************************************
*   File: act.comm.cpp                                  Part of Bylins    *
*  Usage: Player-level communication commands                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
*                                                                         *
*  $Author$                                                        *
*  $Date$                                           *
*  $Revision$                                                       *
************************************************************************ */

#include <sstream>
#include <list>
#include <string>
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "screen.h"
#include "dg_scripts.h"
#include "auction.h"
#include "privilege.hpp"
#include "char.hpp"
#include "char_player.hpp"
#include "remember.hpp"
#include "house.h"
#include "obj.hpp"
#include "room.hpp"
#include "spam.hpp"

// extern variables
extern DESCRIPTOR_DATA *descriptor_list;
extern CHAR_DATA *character_list;
extern TIME_INFO_DATA time_info;

// local functions
void perform_tell(CHAR_DATA * ch, CHAR_DATA * vict, char *arg);
int is_tell_ok(CHAR_DATA * ch, CHAR_DATA * vict);
bool tell_can_see(CHAR_DATA *ch, CHAR_DATA *vict);

// external functions
extern char *diag_timer_to_char(const OBJ_DATA * obj);
extern void set_wait(CHAR_DATA * ch, int waittime, int victim_in_room);

ACMD(do_say);
ACMD(do_gsay);
ACMD(do_tell);
ACMD(do_reply);
ACMD(do_spec_comm);
ACMD(do_write);
ACMD(do_page);
ACMD(do_gen_comm);
ACMD(do_pray_gods);
ACMD(do_remember_char);
// shapirus
ACMD(do_ignore);

#define SIELENCE ("�� ����, ��� ���� �� ���.\r\n")
#define SOUNDPROOF ("����� ��������� ���� �����.\r\n")


ACMD(do_say)
{
	skip_spaces(&argument);
	CHAR_DATA *to;

	if (AFF_FLAGGED(ch, AFF_SIELENCE) || AFF_FLAGGED(ch, AFF_STRANGLED))
	{
		send_to_char(SIELENCE, ch);
		return;
	}

	if (!IS_NPC(ch) && PLR_FLAGGED(ch, PLR_DUMB))
	{
		send_to_char("��� ��������� ���������� � ������ �������!\r\n", ch);
		return;
	}

	/* ��������� ������! ���� ������� ������� - ������� ��� ����� �����!
	if (ROOM_FLAGGED(ch->in_room, ROOM_ARENARECV))
	{
		send_to_char(SOUNDPROOF, ch);
		return;
	}
	*/

	if (!*argument)
		send_to_char("�� ����������: \"���� �� ������ �������?\"\r\n", ch);
	else
	{
		sprintf(buf, "$n ������$g : '%s'", argument);
//      act (buf, FALSE, ch, 0, 0, TO_ROOM | DG_NO_TRIG | CHECK_DEAF);
// shapirus; ��� ����������� ������������� ������ � ������
// �������� �������� act � ������ �� ������ �� ������
		for (to = world[ch->in_room]->people; to; to = to->next_in_room)
		{
			if (ch == to || ignores(to, ch, IGNORE_SAY))
				continue;
			act(buf, FALSE, ch, 0, to, TO_VICT | DG_NO_TRIG | CHECK_DEAF);
		}
		if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_NOREPEAT))
			send_to_char(OK, ch);
		else
		{
			delete_doubledollar(argument);
			sprintf(buf, "�� ������� : '%s'\r\n", argument);
			send_to_char(buf, ch);
		}
		speech_mtrigger(ch, argument);
		speech_wtrigger(ch, argument);
	}
}


ACMD(do_gsay)
{
	CHAR_DATA *k;
	struct follow_type *f;

	if (AFF_FLAGGED(ch, AFF_SIELENCE) || AFF_FLAGGED(ch, AFF_STRANGLED))
	{
		send_to_char(SIELENCE, ch);
		return;
	}

	/* ��������� ������ �����
	if (ROOM_FLAGGED(ch->in_room, ROOM_ARENARECV))
	{
		send_to_char(SOUNDPROOF, ch);
		return;
	}
	*/

	if (!IS_NPC(ch) && PLR_FLAGGED(ch, PLR_DUMB))
	{
		send_to_char("��� ��������� ���������� � ������ �������!\r\n", ch);
		return;
	}

	skip_spaces(&argument);

	if (!AFF_FLAGGED(ch, AFF_GROUP))
	{
		send_to_char("�� �� ��������� ������ ������!\r\n", ch);
		return;
	}
	if (!*argument)
		send_to_char("� ��� �� ������ �������� ����� ������?\r\n", ch);
	else
	{
		if (ch->master)
			k = ch->master;
		else
			k = ch;

		sprintf(buf, "$n �������$g ������ : '%s'", argument);

		if (AFF_FLAGGED(k, AFF_GROUP) && (k != ch) && !ignores(k, ch, IGNORE_GROUP))
		{
			act(buf, FALSE, ch, 0, k, TO_VICT | TO_SLEEP | CHECK_DEAF);
			// added by WorM  ��������� 2010.10.13
			if(!AFF_FLAGGED(k, AFF_DEAFNESS) && GET_POS(k) > POS_DEAD)
			{
				sprintf(buf1, "%s �������%s ������ : '%s'\r\n", tell_can_see(ch, k) ? GET_NAME(ch) : "���-��", GET_CH_VIS_SUF_1(ch, k), argument);
				k->remember_add(buf1, Remember::ALL);
				k->remember_add(buf1, Remember::GROUP);
			}
			//end by WorM
		}
		for (f = k->followers; f; f = f->next)
			if (AFF_FLAGGED(f->follower, AFF_GROUP) && (f->follower != ch) &&
					!ignores(f->follower, ch, IGNORE_GROUP))
				{
					act(buf, FALSE, ch, 0, f->follower, TO_VICT | TO_SLEEP | CHECK_DEAF);
					// added by WorM  ��������� 2010.10.13
					if(!AFF_FLAGGED(f->follower, AFF_DEAFNESS) && GET_POS(f->follower) > POS_DEAD)
					{
						sprintf(buf1, "%s �������%s ������ : '%s'\r\n", tell_can_see(ch, f->follower) ? GET_NAME(ch) : "���-��", GET_CH_VIS_SUF_1(ch, f->follower), argument);
						f->follower->remember_add(buf1, Remember::ALL);
						f->follower->remember_add(buf1, Remember::GROUP);
					}
					//end by WorM
				}

		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			send_to_char(OK, ch);
		else
		{
			sprintf(buf, "�� �������� ������ : '%s'\r\n", argument);
			send_to_char(buf, ch);
			// added by WorM  ��������� 2010.10.13
			ch->remember_add(buf, Remember::ALL);
			ch->remember_add(buf, Remember::GROUP);
			//end by WorM
		}
	}
}

bool tell_can_see(CHAR_DATA *ch, CHAR_DATA *vict)
{
	if (CAN_SEE_CHAR(vict, ch) || IS_IMMORTAL(ch) || GET_INVIS_LEV(ch))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void perform_tell(CHAR_DATA * ch, CHAR_DATA * vict, char *arg)
{
// shapirus: �� �������� ������, ���� ������ �� ����� � ��������
// ��������������� �����; ��������� ����� ������ ������
	if (PRF_FLAGGED(vict, PRF_NOINVISTELL)
			&& !CAN_SEE(vict, ch)
			&& GET_LEVEL(ch) < LVL_IMMORT
			&& !PRF_FLAGGED(ch, PRF_CODERINFO))
	{
		act("$N �� ����� ������������� � ����, ���� �� �����.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
		return;
	}

	// TODO: ���� � act() ��������� ����� �����, �� ��� � ��� ���� ���������� �� act()
	if (tell_can_see(ch, vict))
	{
		snprintf(buf, MAX_STRING_LENGTH, "%s ������%s ��� : '%s'", GET_NAME(ch), GET_CH_SUF_1(ch), arg);
	}
	else
	{
		snprintf(buf, MAX_STRING_LENGTH, "���-�� ������ ��� : '%s'", arg);
	}
	snprintf(buf1, MAX_STRING_LENGTH, "%s%s%s\r\n", CCICYN(vict, C_NRM), CAP(buf), CCNRM(vict, C_NRM));
	send_to_char(buf1, vict);
	if (!IS_NPC(vict))
	{
		vict->remember_add(buf1, Remember::ALL);
	}

	if (!IS_NPC(vict) && !IS_NPC(ch))
	{
		snprintf(buf, MAX_STRING_LENGTH, "%s%s : '%s'%s\r\n", CCICYN(vict, C_NRM),
				tell_can_see(ch, vict) ? GET_NAME(ch) : "���-��", arg, CCNRM(vict, C_NRM));
		vict->remember_add(buf, Remember::PERSONAL);
	}

	if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_NOREPEAT))
	{
		send_to_char(OK, ch);
	}
	else
	{
		snprintf(buf, MAX_STRING_LENGTH, "%s�� ������� %s : '%s'%s\r\n", CCICYN(ch, C_NRM),
				tell_can_see(vict, ch) ? vict->player_data.PNames[2] : "����-��", arg, CCNRM(ch, C_NRM));
		send_to_char(buf, ch);
		if (!IS_NPC(ch))
		{
			ch->remember_add(buf, Remember::ALL);
		}
	}

	if (!IS_NPC(vict) && !IS_NPC(ch))
	{
		vict->set_answer_id(GET_IDNUM(ch));
	}
}

int is_tell_ok(CHAR_DATA * ch, CHAR_DATA * vict)
{
	if (ch == vict)
	{
		send_to_char("�� ������ ���������� ������������� � ����� �����.\r\n", ch);
		return (FALSE);
	}
	else if (!IS_NPC(ch) && PLR_FLAGGED(ch, PLR_DUMB))
	{
		send_to_char("��� ��������� ���������� � ������ �������.\r\n", ch);
		return (FALSE);
	}
	else if (!IS_NPC(vict) && !vict->desc)  	// linkless
	{
		act("$N �������$G ����� � ���� ������.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
		return (FALSE);
	}
	else if (PLR_FLAGGED(vict, PLR_WRITING))
	{
		act("$N ����� ��������� - ��������� �������.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
		return (FALSE);
	}

	if (IS_GOD(ch) || PRF_FLAGGED(ch, PRF_CODERINFO))
		return (TRUE);

	if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF))
		send_to_char(SOUNDPROOF, ch);
	else if ((!IS_NPC(vict) &&
			  (PRF_FLAGGED(vict, PRF_NOTELL) || ignores(vict, ch, IGNORE_TELL))) ||
			 ROOM_FLAGGED(vict->in_room, ROOM_SOUNDPROOF))
		act("$N �� ������ ��� ��������.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else if (GET_POS(vict) < POS_RESTING || AFF_FLAGGED(vict, AFF_DEAFNESS))
		act("$N ��� �� �������.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else
		return (TRUE);

	return (FALSE);
}

/*
 * Yes, do_tell probably could be combined with whisper and ask, but
 * called frequently, and should IMHO be kept as tight as possible.
 */
ACMD(do_tell)
{
	CHAR_DATA *vict = NULL;

	if (AFF_FLAGGED(ch, AFF_CHARM))
		return;

	if (AFF_FLAGGED(ch, AFF_SIELENCE) || AFF_FLAGGED(ch, AFF_STRANGLED))
	{
		send_to_char(SIELENCE, ch);
		return;
	}

	/* ��������� ������ �����
	if (ROOM_FLAGGED(ch->in_room, ROOM_ARENARECV))
	{
		send_to_char(SOUNDPROOF, ch);
		return;
	}
	*/

	half_chop(argument, buf, buf2);

	if (!*buf || !*buf2)
	{
		send_to_char("��� � ���� �� ������ �������?\r\n", ch);
	}
	else if (!(vict = get_player_vis(ch, buf, FIND_CHAR_WORLD)))
	{
		send_to_char(NOPERSON, ch);
	}
	else if (IS_NPC(vict))
		send_to_char(NOPERSON, ch);
	else if (is_tell_ok(ch, vict))
	{
		if (PRF_FLAGGED(ch, PRF_NOTELL))
			send_to_char("�������� ��� �� ������!\r\n", ch);
		perform_tell(ch, vict, buf2);
	}
}


ACMD(do_reply)
{
	CHAR_DATA *tch = character_list;

	if (IS_NPC(ch))
		return;

	if (AFF_FLAGGED(ch, AFF_SIELENCE) || AFF_FLAGGED(ch, AFF_STRANGLED))
	{
		send_to_char(SIELENCE, ch);
		return;
	}

	/* � ��� �� ���� ����� ����
	if (ROOM_FLAGGED(ch->in_room, ROOM_ARENARECV))
	{
		send_to_char(SOUNDPROOF, ch);
		return;
	}
	*/

	if (!IS_NPC(ch) && PLR_FLAGGED(ch, PLR_DUMB))
	{
		send_to_char("��� ��������� ���������� � ������ �������!\r\n", ch);
		return;
	}

	skip_spaces(&argument);

	if (ch->get_answer_id() == NOBODY)
		send_to_char("��� ������ ��������!\r\n", ch);
	else if (!*argument)
		send_to_char("��� �� ����������� ��������?\r\n", ch);
	else
	{			/*
				 * Make sure the person you're replying to is still playing by searching
				 * for them.  Note, now last tell is stored as player IDnum instead of
				 * a pointer, which is much better because it's safer, plus will still
				 * work if someone logs out and back in again.
				 */

		/*
		 * XXX: A descriptor list based search would be faster although
		 *      we could not find link dead people.  Not that they can
		 *      hear tells anyway. :) -gg 2/24/98
		 */
		while (tch != NULL && (IS_NPC(tch) || GET_IDNUM(tch) != ch->get_answer_id()))
			tch = tch->next;

		if (tch == NULL)
			send_to_char("����� ������ ��� ��� � ����.", ch);
		else if (is_tell_ok(ch, tch))
			perform_tell(ch, tch, argument);
	}
}


ACMD(do_spec_comm)
{
	CHAR_DATA *vict;
	const char *action_sing, *action_plur, *action_others, *vict1, *vict2;
	char vict3[MAX_INPUT_LENGTH];

	if (AFF_FLAGGED(ch, AFF_SIELENCE) || AFF_FLAGGED(ch, AFF_STRANGLED))
	{
		send_to_char(SIELENCE, ch);
		return;
	}

	if (!IS_NPC(ch) && PLR_FLAGGED(ch, PLR_DUMB))
	{
		send_to_char("��� ��������� ���������� � ������ �������!\r\n", ch);
		return;
	}

	if (subcmd == SCMD_WHISPER)
	{
		action_sing = "�������";
		vict1 = "����";
		vict2 = "���";
		action_plur = "���������";
		action_others = "$n ���-�� ���������$g $N2.";
	}
	else
	{
		action_sing = "��������";
		vict1 = "� ����";
		vict2 = "� ���";
		action_plur = "�������";
		action_others = "$n �����$g $N2 ������.";
	}

	half_chop(argument, buf, buf2);

	if (!*buf || !*buf2)
	{
		sprintf(buf, "��� �� ������ %s.. � %s?\r\n", action_sing, vict1);
		send_to_char(buf, ch);
	}
	else if (!(vict = get_char_vis(ch, buf, FIND_CHAR_ROOM)))
		send_to_char(NOPERSON, ch);
	else if (vict == ch)
		send_to_char("�� ����� ��� �� ���� - ����� ���� ������...\r\n", ch);
	else if (ignores(vict, ch, subcmd == SCMD_WHISPER ? IGNORE_WHISPER : IGNORE_ASK))
	{
		sprintf(buf, "%s �� ������ ��� �������.\r\n", GET_NAME(vict));
		send_to_char(buf, ch);
	}
	else
	{
		if (subcmd == SCMD_WHISPER)
			sprintf(vict3, "%s", GET_PAD(vict, 2));
		else
			sprintf(vict3, "� %s", GET_PAD(vict, 1));

		sprintf(buf, "$n %s$g %s : '%s'", action_plur, vict2, buf2);
		act(buf, FALSE, ch, 0, vict, TO_VICT | CHECK_DEAF);

		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			send_to_char(OK, ch);
		else
		{
			sprintf(buf, "�� %s� %s : '%s'\r\n", action_plur, vict3, buf2);
			send_to_char(buf, ch);
		}

		act(action_others, FALSE, ch, 0, vict, TO_NOTVICT);
	}
}



#define MAX_NOTE_LENGTH 1000	// arbitrary

ACMD(do_write)
{
	OBJ_DATA *paper, *pen = NULL;
	char *papername, *penname;

	papername = buf1;
	penname = buf2;

	two_arguments(argument, papername, penname);

	if (!ch->desc)
		return;

	if (!*papername)  	// nothing was delivered
	{
		send_to_char("��������?  ���?  � �� ���?\r\n", ch);
		return;
	}
	if (*penname)  		// there were two arguments
	{
		if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying)))
		{
			sprintf(buf, "� ��� ��� %s.\r\n", papername);
			send_to_char(buf, ch);
			return;
		}
		if (!(pen = get_obj_in_list_vis(ch, penname, ch->carrying)))
		{
			sprintf(buf, "� ��� ��� %s.\r\n", penname);
			send_to_char(buf, ch);
			return;
		}
	}
	else  		// there was one arg.. let's see what we can find
	{
		if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying)))
		{
			sprintf(buf, "�� �� ������ %s � ���������.\r\n", papername);
			send_to_char(buf, ch);
			return;
		}
		if (GET_OBJ_TYPE(paper) == ITEM_PEN)  	// oops, a pen..
		{
			pen = paper;
			paper = NULL;
		}
		else if (GET_OBJ_TYPE(paper) != ITEM_NOTE)
		{
			send_to_char("�� �� ������ �� ���� ������.\r\n", ch);
			return;
		}
		// One object was found.. now for the other one.
		if (!GET_EQ(ch, WEAR_HOLD))
		{
			sprintf(buf, "�� ����� ������!\r\n");
			send_to_char(buf, ch);
			return;
		}
		if (!CAN_SEE_OBJ(ch, GET_EQ(ch, WEAR_HOLD)))
		{
			send_to_char("�� ������� ���-�� ���������!  ����, �� ������ ���� ������!!\r\n", ch);
			return;
		}
		if (pen)
			paper = GET_EQ(ch, WEAR_HOLD);
		else
			pen = GET_EQ(ch, WEAR_HOLD);
	}


	// ok.. now let's see what kind of stuff we've found
	if (GET_OBJ_TYPE(pen) != ITEM_PEN)
		act("�� �� ������ ������ $o4.", FALSE, ch, pen, 0, TO_CHAR);
	else if (GET_OBJ_TYPE(paper) != ITEM_NOTE)
		act("�� �� ������ ������ �� $o5.", FALSE, ch, paper, 0, TO_CHAR);
	else if (paper->action_description)
		send_to_char("��� ��� ���-�� ��������.\r\n", ch);
	else  			// we can write - hooray!
	{
		/* this is the PERFECT code example of how to set up:
		 * a) the text editor with a message already loaed
		 * b) the abort buffer if the player aborts the message
		 */
		ch->desc->backstr = NULL;
		send_to_char("������ ������.  (/s ��������� ������  /h ������)\r\n", ch);
		// ok, here we check for a message ALREADY on the paper
		if (paper->action_description)  	// we str_dup the original text to the descriptors->backstr
		{
			ch->desc->backstr = str_dup(paper->action_description);
			// send to the player what was on the paper (cause this is already
			// loaded into the editor)
			send_to_char(paper->action_description, ch);
		}
		act("$n �����$g ������.", TRUE, ch, 0, 0, TO_ROOM);
		// assign the descriptor's->str the value of the pointer to the text
		// pointer so that we can reallocate as needed (hopefully that made
		// sense :>)
		string_write(ch->desc, &paper->action_description, MAX_NOTE_LENGTH, 0, NULL);
	}
}

ACMD(do_page)
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *vict;

	half_chop(argument, arg, buf2);

	if (IS_NPC(ch))
		send_to_char("��������-��-��������� ����� �� �����.. ��������.\r\n", ch);
	else if (!*arg)
		send_to_char("Whom do you wish to page?\r\n", ch);
	else
	{
		sprintf(buf, "\007\007*$n* %s", buf2);
		if (!str_cmp(arg, "all") || !str_cmp(arg, "���"))
		{
			if (IS_GRGOD(ch))
			{
				for (d = descriptor_list; d; d = d->next)
					if (STATE(d) == CON_PLAYING && d->character)
						act(buf, FALSE, ch, 0, d->character, TO_VICT);
			}
			else
				send_to_char("��� �������� ������ �����!\r\n", ch);
			return;
		}
		if ((vict = get_char_vis(ch, arg, FIND_CHAR_WORLD)) != NULL)
		{
			act(buf, FALSE, ch, 0, vict, TO_VICT);
			if (PRF_FLAGGED(ch, PRF_NOREPEAT))
				send_to_char(OK, ch);
			else
				act(buf, FALSE, ch, 0, vict, TO_CHAR);
		}
		else
			send_to_char("����� ����� �����������!\r\n", ch);
	}
}


/**********************************************************************
 * generalized communication func, originally by Fred C. Merkel (Torg) *
  *********************************************************************/

struct communication_type
{
	const char *muted_msg;
	const char *action;
	const char *no_channel;
	const char *color;
	const char *you_action;
	const char *hi_action;
	int min_lev;
	int move_cost;
	int noflag;
};

ACMD(do_gen_comm)
{
	DESCRIPTOR_DATA *i;
	char color_on[24];
	int ign_flag;
	/*
	 * com_msgs: Message if you can't perform the action because of mute
	 *           name of the action
	 *           message if you're not on the channel
	 *           a color string.
	 *           �� ....
	 *           ��(�) ....
	 *           min access level.
	 *           mov cost.
	 */

	struct communication_type com_msgs[] =
	{
		{"�� �� ������ �����.\r\n",	// holler
		 "�����",
		 "�� ��� ��������� ������.",
		 KIYEL,
		 "�������",
		 "������$g",
		 4,
		 25,
		 PRF_NOHOLLER},

		{"��� ��������� �������.\r\n",	// shout
		 "�������",
		 "�� ��� ��������� ������.\r\n",
		 KIYEL,
		 "���������",
		 "��������$g",
		 2,
		 10,
		 PRF_NOSHOUT},

		{"��� ����������� �������.\r\n",	// gossip
		 "�������",
		 "�� ��� ��������� ������.\r\n",
		 KYEL,
		 "��������",
		 "�������$g",
		 1, // boltat s 1 urovnya! prool
		 15,
		 PRF_NOGOSS},

		{"��� �� � ���� �����������.\r\n",	// auction
		 "���������",
		 "�� ��� ��������� ������.\r\n",
		 KIYEL,
		 "����������� �������������",
		 "�������$g � ����",
		 2,
		 0,
		 PRF_NOAUCT},
	};

	// to keep pets, etc from being ordered to shout
//  if (!ch->desc)
	if (AFF_FLAGGED(ch, AFF_CHARM))
		return;

	if (AFF_FLAGGED(ch, AFF_SIELENCE) || AFF_FLAGGED(ch, AFF_STRANGLED))
	{
		send_to_char(SIELENCE, ch);
		return;
	}

	if (!IS_NPC(ch) && PLR_FLAGGED(ch, PLR_DUMB))
	{
		send_to_char("��� ��������� ���������� � ������ �������!\r\n", ch);
		return;
	}

	if (PLR_FLAGGED(ch, PLR_MUTE) && subcmd != SCMD_AUCTION)
	{
		send_to_char(com_msgs[subcmd].muted_msg, ch);
		return;
	}
	//if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF) || ROOM_FLAGGED(ch->in_room, ROOM_ARENARECV))
	if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF))
	{
		send_to_char(SOUNDPROOF, ch);
		return;
	}

	if (GET_LEVEL(ch) < com_msgs[subcmd].min_lev && !GET_REMORT(ch))
	{
		sprintf(buf1,
				"��� ����� ������� ���� �� %d ������, ����� �� ����� %s.\r\n",
				com_msgs[subcmd].min_lev, com_msgs[subcmd].action);
		send_to_char(buf1, ch);
		return;
	}

	// make sure the char is on the channel
	if (PRF_FLAGGED(ch, com_msgs[subcmd].noflag))
	{
		send_to_char(com_msgs[subcmd].no_channel, ch);
		return;
	}

	// skip leading spaces
	skip_spaces(&argument);

	// make sure that there is something there to say!
	if (!*argument && subcmd != SCMD_AUCTION)
	{
		sprintf(buf1, "�����! ��, ����� ��� ������, ��� %s???\r\n", com_msgs[subcmd].action);
		send_to_char(buf1, ch);
		return;
	}

	// set up the color on code
	strcpy(color_on, com_msgs[subcmd].color);

	// ����-������� :coded by �����

#define MAX_UPPERS_CHAR_PRC 30
#define MAX_UPPERS_SEQ_CHAR 3

	if ((subcmd != SCMD_AUCTION) && (!IS_IMMORTAL(ch)) && (!IS_NPC(ch)))
	{
		unsigned int bad_smb_procent = MAX_UPPERS_CHAR_PRC;
		int bad_simb_cnt = 0, bad_seq_cnt = 0;

		// ��������� ������� �������
		for (int k = 0; argument[k] != '\0'; k++)
		{
			if (a_isupper(argument[k]))
			{
				bad_simb_cnt++;
				bad_seq_cnt++;
			}
			else
				bad_seq_cnt = 0;

			if ((bad_seq_cnt > 1) &&
					(((bad_simb_cnt * 100 / strlen(argument)) > bad_smb_procent) ||
					 (bad_seq_cnt > MAX_UPPERS_SEQ_CHAR)))
				argument[k] = a_lcc(argument[k]);
		}
		// ��������� ���������� ��������� � �����
		if (!str_cmp(ch->get_last_tell().c_str(), argument))
		{
			send_to_char("�� �� �� ������� �������� ���� �����?!\r\n", ch);
			return;
		}
		ch->set_last_tell(argument);
	}

	// � ���� �������� ������ ����������� ���� �� �����, ������� ��� ������ ���� ���������
	if (!check_moves(ch, com_msgs[subcmd].move_cost))
		return;

	char out_str[MAX_STRING_LENGTH];

	// first, set up strings to be given to the communicator
	if (subcmd == SCMD_AUCTION)
	{
		*buf = '\0';
		auction_drive(ch, argument);
		return;
	}
	else
	{
		/* ���������� ������
		if (ROOM_FLAGGED(ch->in_room, ROOM_ARENARECV))
		{
			send_to_char(SOUNDPROOF, ch);
			return;
		}
		*/
		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			send_to_char(OK, ch);
		else
		{
			if (COLOR_LEV(ch) >= C_CMP)
			{
				snprintf(buf1, MAX_STRING_LENGTH, "%s�� %s : '%s'%s", color_on,
						com_msgs[subcmd].you_action, argument, KNRM);
			}
			else
			{
				snprintf(buf1, MAX_STRING_LENGTH, "�� %s : '%s'",
						com_msgs[subcmd].you_action, argument);
			}
			act(buf1, FALSE, ch, 0, 0, TO_CHAR | TO_SLEEP);

			if (!IS_NPC(ch))
			{
				snprintf(buf1 + strlen(buf1), MAX_STRING_LENGTH, "\r\n");
				ch->remember_add(buf1, Remember::ALL);
			}
		}
		snprintf(out_str, MAX_STRING_LENGTH, "$n %s : '%s'", com_msgs[subcmd].hi_action, argument);

		if (!IS_NPC(ch) && (subcmd == SCMD_GOSSIP || subcmd == SCMD_HOLLER))
		{
			snprintf(buf1, MAX_STRING_LENGTH, "%s'%s'%s\r\n", color_on, argument, KNRM);
			ch->remember_add(buf1, Remember::GOSSIP);
		}
	}

	switch (subcmd)
	{
	case SCMD_SHOUT:
		ign_flag = IGNORE_SHOUT;
		break;
	case SCMD_GOSSIP:
		ign_flag = IGNORE_GOSSIP;
		break;
	case SCMD_HOLLER:
		ign_flag = IGNORE_HOLLER;
		break;
	default:
		ign_flag = 0;
	}

	// now send all the strings out
	for (i = descriptor_list; i; i = i->next)
	{
		if (STATE(i) == CON_PLAYING && i != ch->desc && i->character &&
				!PRF_FLAGGED(i->character, com_msgs[subcmd].noflag) &&
				!PLR_FLAGGED(i->character, PLR_WRITING) &&
				!ROOM_FLAGGED(i->character->in_room, ROOM_SOUNDPROOF) && GET_POS(i->character) > POS_SLEEPING)
		{
			if (ignores(i->character, ch, ign_flag))
				continue;
			if (subcmd == SCMD_SHOUT &&
					((world[ch->in_room]->zone != world[i->character->in_room]->zone) || !AWAKE(i->character)))
				continue;

			if (COLOR_LEV(i->character) >= C_NRM)
				send_to_char(color_on, i->character);
			act(out_str, FALSE, ch, 0, i->character, TO_VICT | TO_SLEEP | CHECK_DEAF);
			if (COLOR_LEV(i->character) >= C_NRM)
				send_to_char(KNRM, i->character);

			std::string text = Remember::format_gossip(ch, i->character, subcmd, argument);
			//���� ������ ��� ����, ��� � ��� �� ���������� ������� �����������
			//if (!IS_NPC(ch) && (subcmd == SCMD_GOSSIP || subcmd == SCMD_HOLLER))
			//{
				//	i->character->remember_add(text, Remember::GOSSIP);
			//}
			i->character->remember_add(text, Remember::ALL);
		}
	}
}


ACMD(do_mobshout)
{
	DESCRIPTOR_DATA *i;

	// to keep pets, etc from being ordered to shout
	if (!(IS_NPC(ch) || WAITLESS(ch)))
		return;
	if (AFF_FLAGGED(ch, AFF_CHARM))
		return;

	skip_spaces(&argument); //������� ������ � ������ ���������
	sprintf(buf, "$n ������$g : '%s'", argument);

	// now send all the strings out
	for (i = descriptor_list; i; i = i->next)
	{
		if (STATE(i) == CON_PLAYING && i->character &&
				!PLR_FLAGGED(i->character, PLR_WRITING) && GET_POS(i->character) > POS_SLEEPING)
		{
			if (COLOR_LEV(i->character) >= C_NRM)
				send_to_char(KIYEL, i->character);
			act(buf, FALSE, ch, 0, i->character, TO_VICT | TO_SLEEP | CHECK_DEAF);
			if (COLOR_LEV(i->character) >= C_NRM)
				send_to_char(KNRM, i->character);
		}
	}
}

ACMD(do_pray_gods)
{
	char arg1[MAX_INPUT_LENGTH];
	DESCRIPTOR_DATA *i;
	CHAR_DATA *victim = NULL;

	skip_spaces(&argument);

	if (!IS_NPC(ch) && PLR_FLAGGED(ch, PLR_DUMB))
	{
		send_to_char("��� ��������� ���������� � �����, ��������, �� �� ��������...\r\n", ch);
		return;
	}

	if (IS_IMMORTAL(ch))
	{
		// �������� ���� ���� �������� ����
		argument = one_argument(argument, arg1);
		skip_spaces(&argument);
		if (!*arg1)
		{
			send_to_char("������ ��������� �� ����������� ��������?\r\n", ch);
			return;
		}
		victim = get_player_vis(ch, arg1, FIND_CHAR_WORLD);
		if (victim == NULL)
		{
			send_to_char("������ ��� � ����!\r\n", ch);
			return;
		}
	}

	if (!*argument)
	{
		sprintf(buf, "� ��� �� ������ ���������� � �����?\r\n");
		send_to_char(buf, ch);
		return;
	}
	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		send_to_char(OK, ch);
	else
	{
		if (IS_NPC(ch))
			return;
		if (IS_IMMORTAL(ch))
		{
			sprintf(buf, "&R�� ������� ������ %s : '%s'&n\r\n", GET_PAD(victim, 3), argument);
		}
		else
		{
			sprintf(buf, "&R�� �������� � ����� � ���������� : '%s'&n\r\n", argument);
			set_wait(ch, 3, FALSE);
		}
		send_to_char(ch, buf);
		ch->remember_add(buf, Remember::PRAY_PERSONAL);
	}

	if (IS_IMMORTAL(ch))
	{
		sprintf(buf, "&R%s �������%s ��� : '%s'&n\r\n", GET_NAME(ch), GET_CH_SUF_1(ch), argument);
		send_to_char(buf, victim);
		victim->remember_add(buf, Remember::PRAY_PERSONAL);

		snprintf(buf1, MAX_STRING_LENGTH, "&R%s �������%s %s : '%s&n\r\n",
				GET_NAME(ch), GET_CH_SUF_1(ch), GET_PAD(victim, 2), argument);
		ch->remember_add(buf1, Remember::PRAY);

		snprintf(buf, MAX_STRING_LENGTH, "&R%s �������%s �� ��������� %s : '%s'&n\r\n",
				GET_NAME(ch), GET_CH_SUF_1(ch), GET_PAD(victim, 1), argument);
	}
	else
	{
		snprintf(buf1, MAX_STRING_LENGTH, "&R%s �������%s � ����� : '%s&n\r\n",
				 GET_NAME(ch), GET_CH_SUF_1(ch), argument);
		ch->remember_add(buf1, Remember::PRAY);

		snprintf(buf, MAX_STRING_LENGTH, "&R%s �������%s � ����� � ���������� : '%s'&n\r\n",
				GET_NAME(ch), GET_CH_SUF_1(ch), argument);
	}

	for (i = descriptor_list; i; i = i->next)
	{
		if (STATE(i) == CON_PLAYING
				&& IS_IMMORTAL(i->character)
//			&& Privilege::god_list_check(GET_NAME(i->character), GET_UNIQUE(i->character))
				&& i->character != ch)
		{
			send_to_char(buf, i->character);
			i->character->remember_add(buf, Remember::ALL);
		}
	}
}

/**
* ����� ������. �� ����� �����, ������ ����� ��� �������, ���/���� ����� ������.
*/
ACMD(do_offtop)
{
	if (IS_NPC(ch) || GET_LEVEL(ch) >= LVL_IMMORT || PRF_FLAGGED(ch, PRF_IGVA_PRONA))
	{
		send_to_char("����?\r\n", ch);
		return;
	}

	if (PLR_FLAGGED(ch, PLR_DUMB))
	{
		send_to_char("��� ��������� ���������� � ������ �������!\r\n", ch);
		return;
	}
	//if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF) || ROOM_FLAGGED(ch->in_room, ROOM_ARENARECV))
	if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF))
	{
		send_to_char(SOUNDPROOF, ch);
		return;
	}
	if (GET_LEVEL(ch) < SpamSystem::MIN_OFFTOP_LVL && !GET_REMORT(ch))
	{
		send_to_char(ch, "��� ����� ������� ���� �� %d ������, ����� �� ����� ���������.\r\n",
				SpamSystem::MIN_OFFTOP_LVL);
		return;
	}
	if (!PRF_FLAGGED(ch, PRF_OFFTOP_MODE))
	{
		send_to_char("�� ��� ��������� ������.\r\n", ch);
		return;
	}
	skip_spaces(&argument);
	if (!*argument)
	{
		send_to_char(ch, "��� ������ �������, �� ����� �� ���������.");
		return;
	}
	lower_convert(argument);
	if (!strcmp(ch->get_last_tell().c_str(), argument))
	{
		send_to_char("�� �� �� ������� �������� ���� �����!?!\r\n", ch);
		return;
	}
	// ��� �������� ������ ���� ��������� � ������ ��� ������� �������� ���� � ����
	if (!SpamSystem::check(ch, SpamSystem::OFFTOP_MODE))
	{
		return;
	}
	ch->set_last_tell(argument);

	snprintf(buf, MAX_STRING_LENGTH, "[������] %s : '%s'\r\n", GET_NAME(ch), argument);
	snprintf(buf1, MAX_STRING_LENGTH, "&c%s&n", buf);

	for (DESCRIPTOR_DATA *i = descriptor_list; i; i = i->next)
	{
		// �������� ��� �������� ���������� ���� �� ���� ����� ����� ���� �����...
		// � �� ��, �� ����? ����� ������ 34-��! ���� �� �������� - ��� ������...
		if (STATE(i) == CON_PLAYING
			&& i->character
			&& (GET_LEVEL(i->character) < LVL_IMMORT || IS_IMPL(i->character))
			&& PRF_FLAGGED(i->character, PRF_OFFTOP_MODE)
			&& !PRF_FLAGGED(i->character, PRF_IGVA_PRONA)
			&& !ignores(i->character, ch, IGNORE_OFFTOP))
		{
			send_to_char(i->character, "%s%s%s", CCCYN(i->character, C_NRM), buf, CCNRM(i->character, C_NRM));
			i->character->remember_add(buf1, Remember::ALL);
		}
	}
	ch->remember_add(buf1, Remember::OFFTOP);
}

// shapirus
void ignore_usage(CHAR_DATA * ch)
{
	send_to_char("������ �������: ������������ <���|���> <�����|���> <��������|������>\r\n"
				 "��������� ������:\r\n"
				 "  ������� �������� ������� �������� ������ �������\r\n"
				 "  ������� ����� ������ ������� ��������\r\n", ch);
}

int ign_find_id(char *name, long *id)
{
	extern struct player_index_element *player_table;
	extern int top_of_p_table;
	int i;

	for (i = 0; i <= top_of_p_table; i++)
	{
		if (!str_cmp(name, player_table[i].name))
		{
			if (player_table[i].level >= LVL_IMMORT)
				return 0;
			*id = player_table[i].id;
			return 1;
		}
	}
	return -1;
}

const char * ign_find_name(long id)
{
	extern struct player_index_element *player_table;
	extern int top_of_p_table;
	int i;

	for (i = 0; i <= top_of_p_table; i++)
		if (id == player_table[i].id)
			return player_table[i].name;
	return "���-��";
}

char *text_ignore_modes(unsigned long mode, char *buf)
{
	buf[0] = 0;

	if (IS_SET(mode, IGNORE_TELL))
		strcat(buf, " �������");
	if (IS_SET(mode, IGNORE_SAY))
		strcat(buf, " ��������");
	if (IS_SET(mode, IGNORE_WHISPER))
		strcat(buf, " �������");
	if (IS_SET(mode, IGNORE_ASK))
		strcat(buf, " ��������");
	if (IS_SET(mode, IGNORE_EMOTE))
		strcat(buf, " ������");
	if (IS_SET(mode, IGNORE_SHOUT))
		strcat(buf, " �������");
	if (IS_SET(mode, IGNORE_GOSSIP))
		strcat(buf, " �������");
	if (IS_SET(mode, IGNORE_HOLLER))
		strcat(buf, " �����");
	if (IS_SET(mode, IGNORE_GROUP))
		strcat(buf, " ������");
	if (IS_SET(mode, IGNORE_CLAN))
		strcat(buf, " �������");
	if (IS_SET(mode, IGNORE_ALLIANCE))
		strcat(buf, " ��������");
	if (IS_SET(mode, IGNORE_OFFTOP))
		strcat(buf, " ��������");
	return buf;
}

ACMD(do_ignore)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	unsigned int mode = 0, list_empty = 1, all = 0, flag = 0;
	long vict_id;
	struct ignore_data *ignore, *cur;
	char buf[256], buf1[256], name[50];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

// ����� ��������� -- ������� �������
	if (arg1[0] && (!arg2[0] || !arg3[0]))
	{
		ignore_usage(ch);
		return;
	}
// ��� ������ ��� ���������� ������� ���� ������
	if (!arg1[0] && !arg2[0] && !arg3[0])
	{
		sprintf(buf, "%s�� ����������� ��������� ����������:%s\r\n", CCWHT(ch, C_NRM), CCNRM(ch, C_NRM));
		send_to_char(buf, ch);
		for (ignore = IGNORE_LIST(ch); ignore; ignore = ignore->next)
		{
			if (!ignore->id)
				continue;
			if (ignore->id == -1)
			{
				strcpy(name, "���");
			}
			else
			{
				strcpy(name, ign_find_name(ignore->id));
				name[0] = UPPER(name[0]);
			}
			sprintf(buf, "  %s: ", name);
			send_to_char(buf, ch);
			mode = ignore->mode;
			send_to_char(text_ignore_modes(mode, buf), ch);
			send_to_char("\r\n", ch);
			list_empty = 0;
		}
		if (list_empty)
			send_to_char("  ������ ����.\r\n", ch);
		return;
	}

	if (is_abbrev(arg2, "���"))
		all = 1;
	else if (is_abbrev(arg2, "�������"))
		flag = IGNORE_TELL;
	else if (is_abbrev(arg2, "��������"))
		flag = IGNORE_SAY;
	else if (is_abbrev(arg2, "�������"))
		flag = IGNORE_WHISPER;
	else if (is_abbrev(arg2, "��������"))
		flag = IGNORE_ASK;
	else if (is_abbrev(arg2, "������"))
		flag = IGNORE_EMOTE;
	else if (is_abbrev(arg2, "�������"))
		flag = IGNORE_SHOUT;
	else if (is_abbrev(arg2, "�������"))
		flag = IGNORE_GOSSIP;
	else if (is_abbrev(arg2, "�����"))
		flag = IGNORE_HOLLER;
	else if (is_abbrev(arg2, "������"))
		flag = IGNORE_GROUP;
	else if (is_abbrev(arg2, "�������"))
		flag = IGNORE_CLAN;
	else if (is_abbrev(arg2, "��������"))
		flag = IGNORE_ALLIANCE;
	else if (is_abbrev(arg2, "������"))
		flag = IGNORE_OFFTOP;
	else
	{
		ignore_usage(ch);
		return;
	}

// ����� "���" ������������� id -1
	if (is_abbrev(arg1, "���"))
	{
		vict_id = -1;
	}
	else
	{
		// ��������, ��� ����������� ��� �� ������ ������ ����������
		// � �� �� ���, � ������ ������� ��� id
		switch (ign_find_id(arg1, &vict_id))
		{
		case 0:
			send_to_char("������ ������������ �����, ��� ����� ��������.\r\n", ch);
			return;
		case -1:
			send_to_char("��� ������ ���������, �������� ���.\r\n", ch);
			return;
		}
	}

// ���� ������ � ������
	for (ignore = IGNORE_LIST(ch); ignore; ignore = ignore->next)
	{
		if (ignore->id == vict_id)
			break;
		if (!ignore->next)
			break;
	}

	if (is_abbrev(arg3, "��������"))
	{
// ������� ����� ������� ������ � ������, ���� �� �����
		if (!ignore || ignore->id != vict_id)
		{
			CREATE(cur, struct ignore_data, 1);
			cur->next = NULL;
			if (!ignore)	// ������� ������ ����� ������, ���� ��� ���
				IGNORE_LIST(ch) = cur;
			else
				ignore->next = cur;
			ignore = cur;
			ignore->id = vict_id;
			ignore->mode = 0;
		}
		mode = ignore->mode;
		if (all)
		{
			SET_BIT(mode, IGNORE_TELL);
			SET_BIT(mode, IGNORE_SAY);
			SET_BIT(mode, IGNORE_WHISPER);
			SET_BIT(mode, IGNORE_ASK);
			SET_BIT(mode, IGNORE_EMOTE);
			SET_BIT(mode, IGNORE_SHOUT);
			SET_BIT(mode, IGNORE_GOSSIP);
			SET_BIT(mode, IGNORE_HOLLER);
			SET_BIT(mode, IGNORE_GROUP);
			SET_BIT(mode, IGNORE_CLAN);
			SET_BIT(mode, IGNORE_ALLIANCE);
			SET_BIT(mode, IGNORE_OFFTOP);
		}
		else
		{
			SET_BIT(mode, flag);
		}
		ignore->mode = mode;
	}
	else if (is_abbrev(arg3, "������"))
	{
		if (!ignore || ignore->id != vict_id)
		{
			if (vict_id == -1)
			{
				send_to_char("�� � ��� �� ����������� ���� �����.\r\n", ch);
			}
			else
			{
				strcpy(name, ign_find_name(vict_id));
				name[0] = UPPER(name[0]);
				sprintf(buf,
						"�� � ��� �� ����������� "
						"��������� %s%s%s.\r\n", CCWHT(ch, C_NRM), name, CCNRM(ch, C_NRM));
				send_to_char(buf, ch);
			}
			return;
		}
		mode = ignore->mode;
		if (all)
		{
			mode = 0;
		}
		else
		{
			REMOVE_BIT(mode, flag);
		}
		ignore->mode = mode;
		if (!mode)
			ignore->id = 0;
	}
	else
	{
		ignore_usage(ch);
		return;
	}
	if (mode)
	{
		if (ignore->id == -1)
		{
			sprintf(buf, "��� ���� ����� �� �����������:%s.\r\n", text_ignore_modes(ignore->mode, buf1));
			send_to_char(buf, ch);
		}
		else
		{
			strcpy(name, ign_find_name(ignore->id));
			name[0] = UPPER(name[0]);
			sprintf(buf, "��� ��������� %s%s%s �� �����������:%s.\r\n",
					CCWHT(ch, C_NRM), name, CCNRM(ch, C_NRM), text_ignore_modes(ignore->mode, buf1));
			send_to_char(buf, ch);
		}
	}
	else
	{
		if (vict_id == -1)
		{
			send_to_char("�� ������ �� ����������� ���� �����.\r\n", ch);
		}
		else
		{
			strcpy(name, ign_find_name(vict_id));
			name[0] = UPPER(name[0]);
			sprintf(buf, "�� ������ �� ����������� ��������� %s%s%s.\r\n",
					CCWHT(ch, C_NRM), name, CCNRM(ch, C_NRM));
			send_to_char(buf, ch);
		}
	}
}
