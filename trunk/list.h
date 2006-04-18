
/*
 * $Id: list.h,v 1.1.1.1 1998/02/23 15:22:27 lindig Exp $
 *
 * CONCEPTS
 * Copyright (C) 1994 Technical University of Braunschweig, Germany
 * Written by Christian Lindig (lindig@ips.cs.tu-bs.de)
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Generic lists.
 */

#ifndef __LIST_H__
#define __LIST_H__

typedef struct listentry {
	char *clientdata ;		/* may be anything */
	struct listentry *next ; 	/* next element in chain */
	struct list *list;		/* point to origin */
} ListEntry ;

#define LIST_NULL ((struct listentry*)0)

typedef struct list {
	int entries ;			/* number of entries */
	struct listentry *first; 	/* points to first element */
 } List ;


extern List *ListNew _ANSI_ARGS_((void));
extern VOID ListInit _ANSI_ARGS_((List *list));
extern VOID ListDelete _ANSI_ARGS_((List *list));
extern ListEntry *ListNewEntry _ANSI_ARGS_((List *list));
extern VOID ListDeleteEntry _ANSI_ARGS_((ListEntry *entry));
extern char *ListGetValue _ANSI_ARGS_((ListEntry *entry));
extern VOID ListSetValue _ANSI_ARGS_((ListEntry *entry, char *data));
extern ListEntry* ListFindEntry _ANSI_ARGS_((List *list,
					     char *data, 
					     int (*eq)(char *x,char *y)));
#endif /* ! __LIST_H__ */
