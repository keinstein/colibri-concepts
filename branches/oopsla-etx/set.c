
/*
 * $Id: set.c,v 1.3 1998/02/25 16:22:08 lindig Exp $
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
 * Implements fixed size bit arrays.
 */

/* requires: bcopy ULONG_MAX fflush printf malloc free */

#include "config.h"
#include <stdio.h>
#if defined(STDC_HEADERS) || defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#endif

#if STDC_HEADERS || defined(HAVE_STRING_H)
#  include <string.h>
#else
#  ifndef HAVE_MEMCPY
#    define memcpy(d, s, n) bcopy ((s), (d), (n))
#    define memmove(d, s, n) bcopy ((s), (d), (n))
#  endif
#endif


#ifdef DBMALLOC
#  include <malloc.h>
#endif

#include "defines.h"
#include "panic.h"
#include <limits.h>		/* ATTENTION - portable? */
#include "set.h"


/*
 * SetInit
 *
 * initialize a Set to defined values. This should be used if a Set is
 * defined but its size isn't known so SetNew can't be called. */

VOID 
SetInit (set) 

	Set *set;
{
	set->set = NULL ;	/* no array yet */
	set->size = 0;
}

/*
 * SetEmpty
 *
 * make a set the empty set: all elements will be marked not present */

VOID
SetEmpty (set) 

	Set *set;

{
	int words ;
	int i;
	setbase *p ;

	words = set->size / SET_BITS ;
	if (set->size % SET_BITS) 
		words++ ;

	for (i=0, p=set->set ;i < words; i++) {
		*p++ = SET_EMPTY ;	/* all bits to zero */
	}
}


/*
 * SetNew
 *
 * Create a new Set of a given, fixed size and initialize it to the
 * empty set. */

VOID
SetNew (set,size) 

	Set *set;
	int size;
{
	int words ;

	set->size = size ;

	/*
	 * calculate the number of setbases we need to hold all elements
	 */

	words = size / SET_BITS ;
	if (size % SET_BITS) 
		words++ ;
	
	set->set = (setbase*) malloc (words * sizeof(setbase)) ;
	if (!set->set) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}
	
	/* clear the set */

	SetEmpty (set);
}

/*
 * SetCopy 
 *
 * create a copy of <source> set at <dest>. All old contents of <dest>
 * will be lost. The caller is responsible for free()-ing memory
 * pointed to by <dest> before calling SetCopy(). */

VOID 
SetCopy (source,dest)

	Set *source;
	Set *dest;

{
	int words ;

	words = source->size / SET_BITS ;
	if (source->size % SET_BITS) 
		words++ ;

	dest->size = source->size ;
	dest->set = (setbase*) malloc (words * sizeof(setbase)) ;
	if (!dest->set) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}
	
	memcpy((char*)dest->set, (char*)source->set,
	       words * sizeof(setbase));

}

/* 
 * SetDelete 
 *
 * Frees all memory associated with a Set, but does not free the Set
 * structure itself */

VOID
SetDelete (set) 
	Set *set;

{
	if (set->set == NULL) {
		/* nothing to free */
		return ;
	}
	free ((char*)set->set);
	set->set = NULL ;
	set->size = 0;
}


/*
 * SetFull
 * 
 * Mark all members of the Set present
 */

VOID
SetFull (set) 

	Set *set;

{
	int words;		/* number of setbase's */
	int bits;		/* significant bits in most sig. setbase */
	int i;
	setbase *p ;		/* points to actual setbase */
	setbase w ;		/* bit mask for most sig setbase */

	words = set->size / SET_BITS ;
	for (i=0, p=set->set ;i < words; i++) {
		*p++ = SET_FULL ;
	}

	/* the last setbase requires special handling because not all bits
	   are valid */

	bits = set->size % SET_BITS ;
	w = SET_ONE ;
	for (i=0; i < bits; i++) {
		*p = *p | w ;	/* set the bits one by one */
		w <<= 1 ;
	}
	
}
		
/*
 * SetAdd 
 *
 * Marks one member as present (regardless if it allready was allreday
 * marked) */

VOID
SetAdd (set,elem) 

	Set *set;
	int elem ;

{
	setbase w ;
	int word ;
	int bit ;

	if (elem >= set->size) {
		panic ("Can't fit %i in %i sized set: %s:%i",
		       elem,set->size,__FILE__,__LINE__) ;
	}

	word = elem / SET_BITS ;
	bit = elem % SET_BITS ;

	w = SET_ONE << bit ;

	set->set[word] = set->set[word] | w ;
}

/*
 * SetRemove
 *
 * Removes one member from the set (marks it absent) */

VOID
SetRemove (set,elem) 

	Set *set;
	int elem ;
{
	setbase w ;
	int word ;		/* where to find elem */
	int bit ;

	if (elem >= set->size) {
		panic ("Can't remove %i in %i sized set: %s:%i",
		       elem,set->size,__FILE__,__LINE__) ;
	}

	word = elem / SET_BITS ;
	bit = elem % SET_BITS ;

	w = SET_ONE << bit ;
	w = ~w ;

	set->set[word] = set->set[word] & w ;
}

/*
 * SetJoin 
 *
 * Join two gives Sets to a resulting Set. The resulting Set is
 * explictly permitted to be one of the joining Sets */


VOID
SetJoin (a,b,result) 

	Set *a,*b,*result;

{
	
	int words,i ;
	setbase *x,*y,*z ;	/* walking pointers to a,b,result */

	words = a->size / SET_BITS ;
	if (a->size % SET_BITS) 
		words++ ;

	if (a->size != b->size || b->size != result->size) {
		panic("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}

	x = a->set ;
	y = b->set ;
	z = result->set ;

	for (i = 0 ; i < words ; i++) {
		*z++ = *x++ | *y++ ;
	}
}

/*
 * SetMinus 
 *
 * Calculate (Set a) - (Set b). The resulting Set is
 * explictly permitted to be one of a,b.*/


VOID
SetMinus (a,b,result) 

	Set *a,*b,*result;

{
	int words,i ;
	setbase *x,*y,*z ;	/* walking pointers to a,b,result */

	words = a->size / SET_BITS ;
	if (a->size % SET_BITS) 
		words++ ;

	if (a->size != b->size || b->size != result->size) {
		panic("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}

	x = a->set ;
	y = b->set ;
	z = result->set ;

	for (i = 0 ; i < words ; i++) {
		*z++ = *x++ & (~*y++) ;
	}
}

/*
 * SetIntersect
 *
 * Intersects two Sets <a>,<b> giving Set <result>. Set <result> ==
 * <a> == <b> is permitted */

VOID
SetIntersect (a,b,result)

	Set *a,*b,*result;
{
	
	int words,i ;
	setbase *x,*y,*z ;	/* pointers to a,b,result resp. */

	words = a->size / SET_BITS ;
	if (a->size % SET_BITS) 
		words++ ;

	if (a->size != b->size || b->size != result->size) {
		panic("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}

	x = a->set ;
	y = b->set ;
	z = result->set ;

	for (i = 0 ; i < words ; i++) {
		*z++ = *x++ & *y++ ;
	}
}

/*
 * SetMember
 *
 * Checks for presence of a given member and returns 1 resp. 0 */

int SetMember (set,elem) 

	Set *set;
	int elem;

{
	setbase w ;		/* bit mask */
	int word ;
	int bit ;

	if (elem >= set->size) {
		panic ("Can't check %i in %i sized set: %s:%i",
		       elem,set->size,__FILE__,__LINE__) ;
	}

	word = elem / SET_BITS ;
	bit = elem % SET_BITS ;

	w = SET_ONE << bit ;

	if ((set->set[word] & w) == 0)
		return 0 ;
	else
		return 1 ;
}


/*
 * SetFirst
 * return the first element of a set. This is used in a first/next-scheme 
 * to examine all members of a set */

int
SetFirst (set)

	Set *set;

{
	int max;		/* number of setbase in set */
	int bit;		/* actual member */
	int word;
	int found;		/* flag */
	setbase w;		/* acutal word  */

	max = set->size / SET_BITS ;
	if (set->size % SET_BITS)
		max++ ;

	word = 0;
	
	/* skip to the first non empty word */
	while (word < max && set->set[word] == 0)
		word++;
	
	if (word == max) {
		/* all words are zero - set is all empty */
		return -1 ;
	}

	/* found non empty word - examine it */

	bit = word * SET_BITS ;
	found = 0;
	w = set->set[word];	/* get the contents for shifting */
	
	while (bit < set->size && !found) {
		found = ( w & SET_ONE );
		bit++ ;
		w >>=1 ;	/* shift word right */
	}
	
	if (found) 
		return (--bit);	/* we did bit++ one too often */
	else
		return -1;	/* nothing found */
}


/*
 * SetNext 
 * return the next member of a set starting from the last
 * found member by the prevoius call to SetNext or SetFirst */

int
SetNext (set,prev)
	Set *set;
	int prev;

{
	
	int max;		/* number of setbase in set */
	int bit;		/* actual member */
	int bit_of_word ;	/* bit % SET_BITS */
	int word;
	int found;		/* flag */
	setbase w;		/* acutal word  */

	max = set->size / SET_BITS ;
	if (set->size % SET_BITS)
		max++ ;

	word = (prev+1) / SET_BITS ; /* first word to examine */
	bit_of_word = (prev+1) % SET_BITS ; /* first bit to examine */
	bit = prev+1 ;

	w = set->set[word];	/* get contents */
	w >>= bit_of_word ;	/* shift bit of intrest to zero pos. */

	/* right shift does not necessarily fill with 0 bits on the
	   right so we mask out all the bits we have shifted in; we
	   check for bit !=0 because otherwise it may not work */
	
	if (bit_of_word != 0)
		w &= ~ ( SET_FULL << (SET_BITS - bit_of_word)) ;
	 
	if (w == 0) {
		/* word empty - find non empty word */
		
		word++ ;
		while (word < max && set->set[word] == 0)
			word++;
	
		if (word == max) {
			/* all words are zero - set is all empty */
			return -1 ;
		}

		/* found non empty word - examine it */
		bit = word * SET_BITS ;	/* correct our position */
		w = set->set[word];	/* get the contents for shifting */
	}
	
	found = 0;
	while (bit < set->size && !found) {
		found = ( w & SET_ONE );
		bit++ ;
		w >>=1 ;	/* shift word right */
	}
		
	if (found) 
		return (--bit) ;
	else
		return -1 ;

}

/*
 * SetSubset
 * returns 1 if <b> is subset of <a>, 0 otherwise
 */

int
SetSubset (a,b) 
	
	Set *a,*b;

{
	int words, bits ;
	int i;
	int lesseq ;
	setbase mask;

	if (a->size != b->size) {
		panic("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}

	bits = a->size % SET_BITS ;
	words = a->size / SET_BITS ;

	/*
	 * x >> SET_BITS is undefined in C
	 * x >> SET_BITS == 0 here
	 * build a mask for the word e is in
	 */

	if (bits != 0) {
		mask = SET_FULL & ~(SET_FULL << bits) ;
	} else {
		mask = SET_EMPTY ;
	}

	lesseq = ((a->set[words] & b->set[words] & mask) 
		== (b->set[words] & mask)) ;


	/* check all other words */
	 
	i = words-1;
	while (i>=0 && lesseq) {
		lesseq = lesseq && ( (a->set[i] & b->set[i]) == b->set[i] ) ;
		i-- ;
	}
	
	return lesseq ;
}

/*
 * SetEqual
 * returns 1 is the two sets a,b are equal
 */

int SetEqual (a,b)
	
	Set *a,*b;

{
	int words, bits ;
	int i;
	int equal ;
	setbase mask ;

	if (a->size != b->size) {
		panic("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}

	bits = a->size % SET_BITS ;
	words = a->size / SET_BITS ;

	/*
	 * x >> SET_BITS is undefined in C
	 * x >> SET_BITS == 0 here
	 * build a mask for the word e is in
	 */

	if (bits != 0) {
		mask = SET_FULL & ~(SET_FULL << bits) ;
	} else {
		mask = SET_EMPTY ;
	}

	equal = ((a->set[words] & mask) == (b->set[words] & mask)) ;


	/* check all other words */
	 
	i = words-1;
	while (i>=0 && equal) {
		equal = equal && ( a->set[i] == b->set[i] ) ;
		i-- ;
	}
	return equal;
}


/*
 * SetLectical defines a total order on Sets. A < B holds if the
 * smallest member in which A and B differ is member of B. This
 * defines a lectical order on Sets. Returns -1 if A < B, 0 if A == B
 * and 1 otherwise */

int
SetLectical (a,b)
	Set *a, *b;		/* sets to be compared */

{
	int words;		/* words occupied by a set */
	int sb;			/* significant bits the highest word */
	int w,bit;		/* index on words, bits */
	int cmp;
	setbase aa,bb;		/*  */

	/* sets must be of equal size */
	if (a->size != b->size) {
		panic("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}

	words = a->size / SET_BITS ;
	sb = a->size % SET_BITS ;

	w = 0;
	while ( (w < words) && (a->set[w] == b->set[w])) {
		w++ ;
	}
	/* we encountered a difference or we have reached the 
	   last word. In any case we have to check the actual
	   word bit by bit. Copy the words to aa and bb resp. */

	aa = a->set[w];
	bb = b->set[w];
	bit = ( w < words ? SET_BITS : sb ); /* bits to check */
	
	cmp = 0;
	while (bit && !cmp) { 
		cmp = (aa & SET_ONE) - (bb & SET_ONE);
		bit-- ;
		aa >>= 1; /* shift right */
		bb >>= 1; /* shift right */
	}
	return cmp;
}

/*
 * SetLe
 *
 * checks if for all elements below e every element member of set b is
 * also member of set a (b subset of a for elements < e) */

int 
SetLe (a,b,e) 
	
	Set *a,*b;
	int e;

{
	int words ;
	int bits ;
	int i;
	int lesseq ;
	setbase mask ;

	if ( (a->size != b->size) || (e >= a->size) ) {
		panic("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}

	words = e / SET_BITS ;
	bits = e % SET_BITS ;

	/*
	 * x >> SET_BITS is undefined in C
	 * x >> SET_BITS == 0 here
	 * build a mask for the word e is in
	 */

	if (bits != 0) {
		mask = SET_FULL & ~(SET_FULL << bits) ;
	} else {
		mask = SET_EMPTY ;
	}

	lesseq = (((a->set[words] & mask) 
		   & (b->set[words] & mask)) 
		  == (b->set[words] & mask));

	/* check all other words below from the one e is in */
	 
	i = words-1;
	while (i>=0 && lesseq) {
		lesseq = lesseq && ( (a->set[i] & b->set[i]) == b->set[i] ) ;
		i-- ;
	}
	
	return lesseq ;
}


/*
 * SetSize
 * returns the number of elements in the given set.
 */

int
SetSize (set)
	
	Set *set;

{
	int words,i,j,k;
	int length ;
	setbase *p;
	setbase w;

	words = set->size / SET_BITS ;
	if (set->size % SET_BITS) 
		words++ ;

	p = set->set ;
	j = 0;
	length = 0;
	/* count members - check bitwise */
	for (i=0;i<words;i++) {
		w = *p++ ;
		for (k=0;k<SET_BITS;k++,j++) {
			if ( w & SET_ONE ) {
				length++;
			}
			w = w >> 1 ;
		}
		
	}
	return length;
}
    

/*
 * SetPrint
 *
 * prints out a Set to stdout - just for debugging
 * just for debugging 
 */
	
VOID
SetPrint (set)

	Set *set;

 {
	int words,i,j,k;
	setbase *p;
	setbase w;

	words = set->size / SET_BITS ;
	if (set->size % SET_BITS) 
		words++ ;

	p = set->set ;
	j = 0;
	/* print all present members - check bitwise */
	for (i=0;i<words;i++) {
		w = *p++ ;
		for (k=0;k<SET_BITS;k++,j++) {
			if ( w & SET_ONE ) {
				printf ("%i ",j);
			}
			w = w >> 1 ;
		}
		
	}
	fflush(stdout) ;
}
			
