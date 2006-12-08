
/*
 * $Id$
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
 * Relation between objects and attributes, both represented by
 * integers but with diffrent maximum values */

#include "config.h"
#include <stdio.h>
#if defined(STDC_HEADERS) || defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#endif

#ifdef DBMALLOC
#  include <malloc.h>
#endif

#include "defines.h"
#include "panic.h"
#include "set.h"
#include "relation.h"

/* requires: SetAdd SetDelete SetMember SetNew SetPrint
   SetRemove free malloc panic printf */

/*
 * RelNew
 *
 * create a relation between [0..<atr>] x [0..<obj>] and initialize
 * it to the empty relation */

VOID
RelNew (rel,atr,obj) 

	Relation *rel;
	int atr,obj;

{
	int i;

	rel->maxatr = atr ;
	rel->maxobj = obj ;

	rel->atr = (Set*) malloc (sizeof(Set) * atr) ;
	if (rel->atr == NULL) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}
	rel->obj = (Set*) malloc (sizeof(Set) * obj) ;
	if (rel->obj == NULL) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}
	for (i=0;i<obj;i++) {
		SetNew(&rel->obj[i],atr);
	}
	for (i=0;i<atr;i++) {
		SetNew(&rel->atr[i],obj);
	}
}

/*
 * RelDelete
 *
 * free all memory associated to a Relation - does not free the
 * Relation struct itself */

VOID 
RelDelete (rel) 
	
	Relation *rel;
{
	int i,obj,atr ;

	/* delete the sets */

	atr = rel->maxatr ;
	obj = rel->maxobj ;

	for (i=0;i<obj;i++) {
		SetDelete(&rel->obj[i]);
	}

	for (i=0;i<atr;i++) {
		SetDelete(&rel->atr[i]);
	}

	/* free the array containing the pointers to the sets */

	free((char*)rel->atr);
	free((char*)rel->obj);
	
	rel->atr = NULL ;
	rel->obj = NULL ;
	rel->maxatr = 0 ;
	rel->maxobj = 0 ;
}


/*
 * RelRelate
 *
 * enter a relation (<atr>,<obj>) to a given relation */
 
VOID
RelRelate (rel, atr, obj) 

	Relation *rel;
	int atr,obj;
{

	if ( (atr >= rel->maxatr) || (obj >= rel->maxobj)) {
		panic("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}
	/* enter (atr,obj) in both Set arrays */
	SetAdd(&rel->obj[obj],atr);
	SetAdd(&rel->atr[atr],obj);
}

/*
 * RelUnrelate
 *
 * remove a (<atr>,<obj>) from a relation (regardless if it exists) */


VOID
RelUnrelate (rel, atr, obj) 

	Relation *rel;
	int atr,obj;
 {
	if ( (atr >= rel->maxatr) || (obj >= rel->maxobj)) {
		panic("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}
	SetRemove(&rel->obj[obj],atr);
	SetRemove(&rel->atr[atr],obj);
}

/*
 * RelRelated check if (<atr>,<obj>) are related */

int 
RelRelated (rel, atr, obj) 

	Relation *rel;
	int atr,obj;

{
	return (SetMember(&rel->obj[obj],atr)) ;
}


/*
 * RelObj
 *
 * return a pointer to a set containing all <atr> which are related
 * to <obj>  */

Set *
RelObj (rel,obj)

	Relation *rel;
	int obj ;
{
	if (obj >= rel->maxobj) {
		panic ("Can't find object %i within %i objects: %s:%i",
		       obj,rel->maxobj,__FILE__,__LINE__) ;
	}

	return (&rel->obj[obj]);
}

/*
 * RelAtr
 *
 * return a pointer to a set containing all <obj> which are related
 * to <atr>  */

Set *
RelAtr (rel,atr)

	Relation *rel;
	int atr ;
{

	if (atr >= rel->maxatr) {
		panic ("Can't find atribute %i within %i atributes: %s:%i",
		       atr,rel->maxatr,__FILE__,__LINE__) ;
	}

	return (&rel->atr[atr]);
}

/* 
 * RelSize
 * return the size of the relation. This is the number of pairs (x,y)
 * that are related.
 */

int
RelSize (rel)
        Relation *rel;
{
        int i;
        int pairs;

        pairs = 0;
        for (i=0;i<rel->maxatr;i++) {
            pairs += SetSize(&rel->atr[i]);
        }
        return pairs;
}

/*
 * RelPrint
 *
 * prints out a relation - just for debugging */

VOID
RelPrint (rel) 
	
	Relation *rel;
{
	int r,c;

	printf ("atr: %i\n",rel->maxatr);
	printf ("obj: %i\n",rel->maxobj);

	for (r=0;r<rel->maxatr;r++) {
		printf ("%i: ",r);
		SetPrint(&rel->atr[r]);
		printf ("\n");
	}
	printf ("\n\n");
	for (c=0;c<rel->maxobj;c++) {
		printf ("%i: ",c);
		SetPrint(&rel->obj[c]);
		printf ("\n");
	}
}
