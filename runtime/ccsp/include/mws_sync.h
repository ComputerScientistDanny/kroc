/*
 *	mws_sync.h -- new multi-way synchronisation structures for run-time (mirror those generated by NOCC)
 *	Copyright (C) 2006 Fred Barnes <frmb@kent.ac.uk>
 *	Adapted from algorithm by Peter Welch
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef __MWS_SYNC_H
#define __MWS_SYNC_H

struct TAG_mws_parbarrier;
struct TAG_mws_procbarrier;

typedef struct TAG_mws_barrier {
	struct TAG_mws_procbarrier *parent;		/* for deep trees (created by interleaving) */
	int sets_enrolled;				/* number of parbarriers syncing */
	int sets_downcount;				/* number of parbarriers left before sync */
	struct TAG_mws_parbarrier *set_fptr;		/* queue of parbarriers attached to this	*/
	struct TAG_mws_parbarrier *set_bptr;		/* procbarrier -- maybe more than sets_enrolled	*/
} mws_barrier_t;

typedef struct TAG_mws_parbarrier {
	struct TAG_mws_parbarrier *next_set;		/* next set in list attached to barrier */
	struct TAG_mws_parbarrier *prev_set;		/* prev set ditto */
	struct TAG_mws_parbarrier *parent_set;		/* parent parbarrier we resigned from / re-enroll to */
	mws_barrier_t *barrier_link;			/* link to barrier */

	int enroll_count;				/* processes (procbarriers) enrolled */
	int sync_count;					/* processes actively synchronising */
	int down_count;					/* number of processes left to synch (may go < 0 for surplus) */

	struct TAG_mws_procbarrier *q_fptr;		/* queue of blocked processes (procbarriers)	*/
	struct TAG_mws_procbarrier *q_bptr;		/* waiting to sync				*/
} mws_parbarrier_t;

typedef enum ENUM_mwspflags {
	MWS_NONE = 0x0000,
	MWS_ALT = 0x0001,				/* process is waiting in an ALT */
	MWS_ALT_SELECTED = 0x0002,			/* process has been selected in an ALT (this choice forced) */
	MWS_INTERLEAVE = 0x0004,			/* process is part of an interleave, affects sub-par handling */
	MWS_SUBPAR = 0x0008				/* not a real process, actually a sub-barrier at wptr */
} mwspflags_e;

typedef struct TAG_mws_procbarrier {
	struct TAG_mws_procbarrier *q_next;		/* next blocked process (procbarrier) */
	struct TAG_mws_procbarrier *q_prev;		/* prev blocked process */
	mws_parbarrier_t *parbarrier_link;		/* link to parbarrier */
	unsigned int *wptr;				/* NotProcess, blocked-process (sync/alt) or pointer to sub-barrier */
	mwspflags_e flags;				/* alt/interleave/sub-par flags */
} mws_procbarrier_t;


#endif	/* !__MWS_SYNC_H */
