/*
 * $Id: hash.h,v 1.1.1.1 1998/02/23 15:22:27 lindig Exp $
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
 * Hash tables mapping strings to client data. Each entry in a
 * hashtable has also a unique id. This id is later used to build an
 * array mapping ids to client data */



#ifndef __HASH_H__
#define __HASH_H__

/*
 * A hash table has HASH_SIZE bucket lists */

#define HASH_SIZE 31

/*
 * bucket list
 */

typedef struct hashentry {
	char *key;		/* key within table */
	int id;			/* unique id within hashtable */
	char *clientdata ;	/* may be anything */
	struct hashentry *next;	/* next entry in bucket chain */

	struct hashtable *tab ;	/* points to table we are in */
	struct hashentry **bucket ;	/* points to origin of bucket list */
} HashEntry ;

#define HASH_NULL ((struct hashentry*)0)

/*
 * hashtable */

typedef struct hashtable {
	HashEntry *buckets[HASH_SIZE] ;
	int entries ;		/* number of entries */
	int maxid ;		/* highest id ever returned */
} HashTable ;

/*
 * For visiting all members of a HashTable we need to remember where
 * we are - this information is kept inside a HashSearch */ 

typedef struct {
	int bucket ;		/* index of bucket we are in */
	HashEntry *actual ;	/* last visited entry */
} HashSearch ;


/*
 * Array mapping id to client data. This may be used when the
 * structure of the hash table is fixed. */

typedef struct  {
	int size ;		/* number of entries in map */
	HashEntry **map ;	/* points to pointer array of Entries*/
} HashMap ;

/* DO NOT DELETE THIS LINE */
/* hash.c */
extern HashTable *HashTabNew _ANSI_ARGS_((void));
extern VOID HashTabInit _ANSI_ARGS_((HashTable *tab));
extern VOID HashTabDelete _ANSI_ARGS_((HashTable *tab));
extern HashEntry *HashCreateEntry _ANSI_ARGS_((HashTable *tab, 
					       char *key, int *new));
extern VOID HashDeleteEntry _ANSI_ARGS_((HashEntry *entry));
extern char *HashGetValue _ANSI_ARGS_((HashEntry *entry));
extern int HashGetId _ANSI_ARGS_((HashEntry *entry));
extern VOID HashSetValue _ANSI_ARGS_((HashEntry *entry, char *data));
extern char *HashGetKey _ANSI_ARGS_((HashEntry *entry));
extern VOID HashToMap _ANSI_ARGS_((HashTable *tab, HashMap *map));
extern HashEntry *HashFindEntry _ANSI_ARGS_((HashTable *tab, char *key));
extern HashEntry *HashFirstEntry _ANSI_ARGS_((HashTable *tab, 
					      HashSearch *search));
extern HashEntry *HashNextEntry _ANSI_ARGS_((HashTable *tab, 
					     HashSearch *search));
extern int HashEqEntry _ANSI_ARGS_((HashEntry *x, HashEntry *y));

#endif /* ! __HASH_H__ */
