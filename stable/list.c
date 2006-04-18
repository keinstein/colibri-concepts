

/*
 * $Id: list.c,v 1.1.1.1 1998/02/23 15:22:26 lindig Exp $
 *
 * CONCEPTS
 * Copyright (C) 1994 Technical University of Braunschweig, Germany
 * Written by Christian Lindig (lindig@ips.cs.tu-bs.de)
 * 
 * This program is free software; you can redistrbute it and/or modify
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
 * A generic list package - the content is totally managed by the caller
 */

/* requires: panic, free, malloc */

#include "config.h"
#if defined(STDC_HEADERS) || defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#endif

#ifdef DBMALLOC
#  include <malloc.h>
#endif

#include "defines.h"
#include "panic.h"
#include "list.h"

/*
 * ListNew
 * 
 * creates a new List, initializes it and returns a pointer to
 * it. Later on LeistDelete can be used to delete the member of the
 * lists but this will not free the List structure itself. The user
 * must call free() explicitly */

List *
ListNew ()

{
	List *list;
	list = (List*) malloc (sizeof(List));
	if (!list) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}
	ListInit (list);	/* initialize list */
	return list;
}

/*
 * ListInit sets everything in an just created List to a defined state
 * - should be called whenever a List is created */

VOID
ListInit (list)

	List *list;

{
	list->entries = 0;	/* no members */
	list->first = LIST_NULL ;
}

/*
 * ListDelete remove memory associated to a List from memory but DOES
 * NOT remove memory associated with the client data - this should be
 * freed before ListDelete is called. ListDelete does not free the
 * List struct! */

VOID 
ListDelete (list) 
	List *list;
{
	ListEntry *entry,*old ;

	/* free all ListEntries one by one */
	entry = list->first ;
	while (entry) {
		old = entry ;
		entry = entry->next ;
		free ((char*) old);
	}
}

/*
 * ListNewEntry creates a new ListElement - i.e. a new entry to hold
 * client data and retuns a pointer to the new entry. */

ListEntry *
ListNewEntry (list) 
	List *list;
{

	ListEntry *entry ;

	entry = (ListEntry*) malloc (sizeof(ListEntry));
	if (!entry) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}

	entry->clientdata = (char*) NULL ; /* no client data yet */
	entry->list = list;
	entry->next = list->first ;
	list->first = entry ;
	list->entries++ ;

	return entry ;
}

/*
 * ListDeleteEntry
 *
 * remove a ListEntry from a chain - does not free client data. From
 * entry the list it is in can be deduced so the List structure does
 * not to be passed seperatetly to ListDeleteEntry. */

VOID
ListDeleteEntry (entry) 
	ListEntry *entry;
{

	ListEntry *prev ;

	if (entry == entry->list->first) {
		/* first entry in chain */
		entry->list->first = entry->next ;
	} else {
		for (prev = entry->list->first ;; prev = prev->next) {
			if (prev == LIST_NULL) {
				panic ("malformed List in %s:%i"
				       ,__FILE__,__LINE__) ;
			}
			if (prev->next == entry) {
				/* found - remove from chain */
				prev->next = entry->next ;
				break ;
			}
		}
	}
	entry->list->entries--;	/* decrease number of entries */
	free((char*)entry);	/* free ListEntry - assumes that data
				   associated with entry is allready
				   free()'ed */

}

/*
 * ListGetValue
 * returns the client data from a given element */
 
char *
ListGetValue (entry) 
	ListEntry *entry ;
{
	return (entry->clientdata) ;
}
		
/*
 * ListFindEntry
 * returns a pointer to the entry which client data matches data. The
 * user supplied function eq() is called for matching */

ListEntry *
ListFindEntry (list,data,eq)
	
	List *list;
	char *data;
	int (*eq)_ANSI_ARGS_((char*,char*));

{

	ListEntry *entry;

	entry = list->first;
	while (entry && !(*eq)(data,entry->clientdata)) {
		entry = entry->next;
	}
	return entry ;
}


/*
 * ListSetValue
 * set the client data of a given entry */

VOID
ListSetValue (entry,data) 

	ListEntry *entry;
	char *data;
{
	entry->clientdata = data ;
}
		
	

	


