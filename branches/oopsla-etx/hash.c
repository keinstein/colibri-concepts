/*
 * $Id: hash.c,v 1.1.1.1 1998/02/23 15:22:26 lindig Exp $
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
 * Hash tables mapping strings to client data. Each entry in a
 * hashtable has also a unique id. This id is later used to build an
 * array mapping ids to client data */

/* requires strdup, strcmp, panic, malloc, free */

#include "config.h"
#if defined(STDC_HEADERS) || defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#endif
#ifdef DBMALLOC
  #include <malloc.h>
#endif

#if STDC_HEADERS || defined(HAVE_STRING_H)
#  include <string.h>
#else
#  ifndef HAVE_MEMCPY
#    define memcpy(d, s, n) bcopy ((s), (d), (n))
#    define memmove(d, s, n) bcopy ((s), (d), (n))
#  endif
#endif

#include "defines.h"
#include "panic.h"
#include "hash.h"

#ifndef HAVE_STRDUP
#  include "lib.h"
#endif

/*
 * HashString
 *
 * compute hash value (0..HASH_SIZE) for a string. The code is
 * borrowed from John Ousterhout's Tcl.  */

static int 
HashString (str)

	char *str ;

{
	unsigned int result;
	int c;

	result = 0;
	while (1) {
		c = *str;
		str++;
		if (c == 0) {
			break;
		}
		result += (result<<3) + c;
	}
	/* make sure the result is positive */
	return (((int)result % HASH_SIZE) + HASH_SIZE) % HASH_SIZE ;

}

/*
 * HashTabNew
 *
 * create a HashTable, initialize it and return a pointer to it. Must
 * be freed by the caller. */

HashTable *
HashTabNew ()

{
	HashTable *tab ;

	tab = (HashTable*) malloc (sizeof (HashTable)) ;
	if (!tab) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}
	HashTabInit (tab);
	return tab;
}


/*
 * HashTabInit
 * 
 * Initializes a HashTab - should be called before any other operation
 * is perfomed on that table */

VOID
HashTabInit (tab)
	
	HashTable *tab;
{

	int i ;

	tab->entries = 0 ;	/* no entries */
	tab->maxid = 0;		/* initial value for id generator */
	for (i=0; i < HASH_SIZE;i++) {
		tab->buckets[i] = HASH_NULL ; /* chains are empty */
	}
}

/*
 * HashTabDelete 
 *
 * remove a HashTable from memory - does not free HashTab structure
 * nor the client data */

VOID
HashTabDelete (tab)

	HashTable *tab;
{
	int i;
	HashEntry *entry ;
	HashEntry *old ;

	for (i=0;i<HASH_SIZE;i++) {
		entry = tab->buckets[i] ;
		while (entry) {
			old = entry ;
			free (old->key); /* free key string */
			free ((char*)old); /* free HashEntry */
			entry = entry->next ;
			
		}
	}
}

/*
 * HashCreateEntry
 *
 * add a new entry to a HashTab. *new !=0 indicates if there was allready
 * such an entry (no new entry will be created in that case) */

HashEntry *
HashCreateEntry (tab,key,new) 

	HashTable *tab;
	char *key;
	int *new;
{

	int buck ;
	HashEntry *entry ;

	buck = HashString(key); /* which bucket chain */
	entry = tab->buckets[buck] ;
	*new = 1;

	/* look for key in chain */
	while (entry && (*new = strcmp(key,entry->key))) {
		entry = entry->next ;
	}

	if (entry) {
		/* found entry - new == 1 */
		return entry ;
	}
	
	/* not found - create a new entry and insert it into chain */

	entry = (HashEntry*) malloc (sizeof (HashEntry)) ;
	if (!entry) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}
	/* insert new entry at head of bucket chain */
	entry->next = tab->buckets[buck] ;
	entry->key = strdup (key); /* make private copy of key string */
	if (!entry->key) 
		panic ("strdup failed in %s:%i",__FILE__,__LINE__);
	entry->id = tab->maxid ; /* assign id */
	entry->clientdata = (char*) NULL ;

	tab->buckets[buck] = entry ;
	tab->entries++ ;
	tab->maxid++ ;

	entry->bucket = &tab->buckets[buck] ;
	entry->tab = tab ;

	return entry ;

}

/*
 * HashDeleteEntry
 *
 * remove one entry from a hash table - does not free client data! */

VOID
HashDeleteEntry (entry)

	HashEntry *entry ;
	
{
	HashEntry *prev ;
	
	if (*entry->bucket == entry) {
		/* first entry in bucket */
		*entry->bucket = entry->next ;
	} else {
		/* search through chain */
		for (prev = *entry->bucket ;; prev = prev->next) {
			if (prev == HASH_NULL) {
				panic ("malformed chain in %s:%i",
				       __FILE__,__LINE__) ;
			}
			if (prev->next == entry) {
				prev->next = entry->next;
				break ;
			}
		}
	}
	entry->tab->entries-- ;	/* decrease number of entries */
	free(entry->key);	/* free private key string */
	free((char*)entry);	/* free entry itself */
}

/*
 * HashGetValue
 *
 * return the client data of a HashEntry
 */

char *
HashGetValue (entry) 

	HashEntry *entry ;
{
	return (entry->clientdata);
}

/*
 * HashGetId
 *
 * return the unique ID of a HashEntry
 */

int 
HashGetId (entry)

	HashEntry *entry ;
{
	return (entry->id) ;
}

/*
 * HashSetValue
 * 
 * assigns a new value to the client data of an entry - does not free
 * the old client data! */

VOID
HashSetValue (entry,data)

	HashEntry *entry;
	char *data;
{
	entry->clientdata = data ;
}

/*
 * HashGetKey
 *
 * returns a pointer to the key string
 */

char *
HashGetKey (entry)

	HashEntry *entry;
{
	return entry->key ;
}

/*
 * HashToMap
 *
 * create an array of pointers to all HashEntries currently in a
 * HashTable.  ATTENTION: the size of the array is NOT determined by
 * map->entries (current number of entries) but by maxid. Right now I
 * can't remember the reason for this :(. The resulting map therefore
 * can contain empty slots. */

VOID
HashToMap (tab,map)

	HashTable *tab;
	HashMap *map ;

 {

	HashEntry **array ;
	HashEntry *entry ;
	HashSearch search ;
	int i;
	
	/*
	 * malloc space for pointer array of tab->maxid entries */

	array = (HashEntry**) (malloc (sizeof(HashEntry*) * tab->maxid));
	if (!array) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}

	/* init all pointer to NULL */

	for (i=0;i<tab->maxid;i++) {
		array[i] = HASH_NULL ;
	}

	/* traverse hashtab and enter all pointer to array */

	entry = HashFirstEntry(tab,&search);
	while (entry) {
		array[entry->id] = entry ;
		entry = HashNextEntry (tab,&search);
	}

	map->size = tab->maxid ;
	map->map = array ;
}


/*
 * HashFindEntry 
 *
 * returns a pointer to the HashEntry whichs key is equal to the given
 * key. Returns HASH_NULL if no such entry exists. */
 
HashEntry *
HashFindEntry (tab,key)

	HashTable *tab;
	char *key;
{
	int buck ;
	HashEntry *entry ;

	buck = HashString(key); /* locate which bucket chain to use */
	entry = tab->buckets[buck] ;

	while (entry && strcmp(key,entry->key)) {
		entry = entry->next ;
	}

	return entry;		/* NULL if not found */
}

/*
 * HashFirstEntry
 *
 * Returns a starting HashEntry for sequential access to all
 * entries. HashSearch must be provided by the caller. Returns
 * HASH_NULL if no entry found. */

HashEntry *
HashFirstEntry (tab,search) 

	HashTable *tab;
	HashSearch *search;

{

	search->bucket = 0;	/* we start at bucket chain 0 */

	/* find a non empty bucket */
	while (search->bucket < HASH_SIZE && !tab->buckets[search->bucket]) {
		search->bucket++ ;
	}

	if (search->bucket == HASH_SIZE) {
		return HASH_NULL ; /* all empty */
	}

	/* return the first element of the found chain and mark it in
           the search struct */
	search->actual = tab->buckets[search->bucket];
	return search->actual ;
}


/*
 * HashNextEntry
 *
 * return the next HashEntry for sequntial access. Returns HASH_NULL
 * if no such is available */

HashEntry *
HashNextEntry (tab,search)

	HashTable *tab;
	HashSearch *search ;

{
	HashEntry *entry ;

	entry = search->actual->next ;
	if (entry) {
		/* the next from the actual chain */
		search->actual = entry ;
		return entry ;
	}

	/* look for next non empty chain */
	search->bucket++ ;
	while (search->bucket < HASH_SIZE && !tab->buckets[search->bucket]) {
		search->bucket++ ;
	}

	if (search->bucket == HASH_SIZE) {
		return HASH_NULL ; /* all chains empty */
	}

	/* return the first entry from the found chain */
	search->actual = tab->buckets[search->bucket];
	return search->actual ;
}
			

/*
 * HashEqEntry
 *
 * checks to given HashEntries for equality - simply compares two
 * pointers, becuase a HashTable never contains duplicates. Returns 0
 * if x == y, 1 otherwise */

int
HashEqEntry (x,y)
	
	HashEntry *x,*y;

{
	return (x == y);
}



