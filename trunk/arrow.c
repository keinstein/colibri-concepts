/*
 * $Id: arrow.c,v 1.1.1.1 1998/02/23 15:22:27 lindig Exp $
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
 * Provides cocnept lattice operations as join and meet and determies
 * if object and attributes are arrow related.  */

#include "config.h"

#if defined(STDC_HEADERS) || defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#endif

#ifdef DBMALLOC
#  include <malloc.h>
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
#include "list.h"
#include "hash.h"
#include "set.h"
#include "relation.h"
#include "concept.h"
#include "context.h"
#include "arrow.h"

/*
 * internal prototypes
 */

static VOID ArrowMaps _ANSI_ARGS_ ((ContextLattice * lattice, 
				    int **objmap, int **atrmap)); 
static int ArrowFindMeet _ANSI_ARGS_ ((Set * key, Concept ** concept));
static int ArrowFindJoin _ANSI_ARGS_ ((Set * key, Concept ** concept));
static int ArrowCmpAtrConcepts _ANSI_ARGS_ ((Concept ** x, Concept ** y));
static Concept *ArrowMeet _ANSI_ARGS_ ((Concept ** concepts, int n, 
					Concept * x, Concept * y));
static Concept *ArrowJoin _ANSI_ARGS_ ((Concept ** concepts, int n, 
					Concept * x, Concept * y));
static Concept *ArrowMeetConcepts _ANSI_ARGS_ ((Concept ** set, int m, 
						Concept ** concepts, int n));
static Concept *ArrowJoinConcepts _ANSI_ARGS_ ((Concept ** set, int m, 
						Concept ** concepts, int n));
static Concept **ArrowSortConcepts _ANSI_ARGS_ ((Concept ** concepts, int n));
static Concept **ArrowCreateArray _ANSI_ARGS_ ((Concept ** concepts, 
						Set * set, int *n));
static Concept *ArrowJoinSubconcepts _ANSI_ARGS_ ((Concept ** concepts, 
						   Concept ** sorted, 
						   int n, int concept));
static Concept *ArrowMeetSuperconcepts _ANSI_ARGS_ ((Concept ** concepts, 
						     int n, int concept));

/*
 * ArrowMaps
 *
 * calculate maps from attributes to concepts and objects to
 * concepts. Each map is an array of integers and will be stored in
 * <objmap> and <atrmap>; the x-th integer is the concept which
 * introduced attribute x and object x resp. The caller is responsible
 * for free()'ing the maps. The atrmap consists of lattice->atr
 * entries, the objmap of lattice->obj entries.
 * */

static VOID
ArrowMaps (lattice, objmap, atrmap) 

	ContextLattice *lattice; 
	int **objmap, **atrmap;	/* place maps here */

{
	int attribute ;		/* attribute from concept */
	int object;		/* object introduced by concept */
	int c;			/* index of actual concept */

	*atrmap = (int*) malloc (sizeof (int) * lattice->atr);
	if (!*atrmap) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__);
	}
	*objmap = (int*) malloc (sizeof (int) * lattice->obj);
	if (!*objmap) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__);
	}

	/* traverse all concepts. Member atr/obj of a Concept contains all
	   attributes/objects introduced by that concept. */

	for (c=0;c < lattice->conc; c++) {
		attribute = SetFirst (&lattice->concepts[c]->atr);
		/* enter all attributs to map */
		while (attribute != -1) {
			(*atrmap)[attribute] = c;
			attribute = SetNext(&lattice->concepts[c]->atr, 
					    attribute); 
		}
		object = SetFirst (&lattice->concepts[c]->obj);
		/* enter all objects to map */
		while (object != -1) {
			(*objmap)[object] = c;
			object = SetNext(&lattice->concepts[c]->obj, 
					    object); 
		}
	}
}

/*
 * ArrowFindMeet
 */

static int 
ArrowFindMeet (key,concept)

	Set *key;
	Concept **concept;

{
	return -SetLectical(key,&(*concept)->objects);
}

/*
 * ArrowFindJoin
 */

static int 
ArrowFindJoin (key,concept)

	Set *key;
	Concept **concept;

{
	return -SetLectical(key,&(*concept)->attributes);
}

#if 0				/* not needed - lattice->objects is in
				 the right order */
/*
 * ArrowCmpObjConcepts
 */

static int 
ArrowCmpObjConcepts  (x,y)

	Concept **x,**y;

{
	return -SetLectical(&(*x)->objects,&(*y)->objects);
}

#endif

/*
 * ArrowCmpAtrConcepts
 */

static int 
ArrowCmpAtrConcepts (x,y)

	Concept **x,**y;

{
	return -SetLectical(&(*x)->attributes,&(*y)->attributes);
}


/*
 * ArrowMeet meet two concepts and return the the meet. The list of
 * concepts supplied by <concepts> must be lectically ordered by the
 * concepts object sets */

static Concept *
ArrowMeet (concepts,n,x,y)
	Concept **concepts;	/* ordered concepts */
	int n;			/* number of concepts */
	Concept *x,*y;		/* concepts to meet */

{

	/* The intersection of two object sets is unique within the
	   lattice and determines the meet. We intersect the object
	   sets of a and y and bsearch the result within lattice */

	Set objects;		/* intersect */
	Concept **meet;		/* meet */
	
	if (n == 0) { /* just paranoia */
		panic ("Empty Lattice %s:%i",__FILE__,__LINE__);
	}
	
	/* get the right set size from concept 0 */
	SetNew (&objects, concepts[0]->objects.size);
	SetIntersect (&x->objects, &y->objects, &objects);
	/* objects is unique - we have to find it */

	meet = (Concept**) 
		bsearch ((char*)&objects,
			 (char*)concepts,
			 n,
			 sizeof(Concept*),
			 (int (*)_ANSI_ARGS_((const void *,const void *))) 
			 ArrowFindMeet);

	if (!meet) {
		panic ("Meet not found %s:%i:",__FILE__,__LINE__);
	}

	return *meet;
}

/*
 * ArrowJoin join two concepts and return the the join. The list of
 * concepts supplied by <concepts> must be lectically ordered by the
 * concepts attribute sets */

static Concept *
ArrowJoin (concepts,n,x,y)
	Concept **concepts;	/* ordered concepts */
	int n;			/* number of concepts */
	Concept *x,*y;		/* concepts to join */

{

	/* The intersection of two attribute sets is unique within the
	   lattice and determines the join. We intersect the attribute
	   sets of a and y and bsearch the result within lattice */

	Set attributes;		/* intersect */
	Concept **join;		/* join */

	if (n == 0) { /* just paranoia */
		panic ("Empty Lattice %s:%i",__FILE__,__LINE__);
	}
	
	/* get the right set size from concept 0 */
	SetNew (&attributes, concepts[0]->attributes.size);
	SetIntersect (&x->attributes, &y->attributes, &attributes);
	/* attributes is unique - we have to find it */

	join = (Concept**) 
		bsearch ((char*)&attributes,
			 (char*)concepts,
			 n,
			 sizeof(Concept*),
			 (int (*)_ANSI_ARGS_((const void *,const void *))) 
			 ArrowFindJoin);

	if (!join) {
		panic ("Join not found %s:%i:",__FILE__,__LINE__);
	}

	return *join;
}


/*
 * ArrowMeetConcepts meet a set of concepts and return the meet from a
 * the set of all concepts. The set of all concepts must be lectically
 * ordered by the concepts object sets.
 */
 

static Concept*
ArrowMeetConcepts (set,m,concepts,n)

	Concept **set;		/* concepts to meet */
	int m;			/* size of set */
	Concept **concepts;	/* all concepts */
	int n;			/* number of all concepts */

{
	Concept **meet;		/* actual meet */
	int c;			/* concept from concepts Set */
	Set objects;		/* intersect */

	
	if (m == 0) { /* top */
		return concepts[0]; /* == *(concepts[0]) */
	}
	
	SetCopy (&set[0]->objects,&objects); /* creates objects! */
	for (c=1;c<m;c++) {
		SetIntersect (&set[c]->objects, &objects, &objects);
	}
	/* objects is unique - we have to find it */
	
	meet = (Concept**) 
		bsearch ((char*)&objects,
			 (char*)concepts,
			 n,
			 sizeof(Concept*),
			 (int (*)_ANSI_ARGS_((const void *,const void *))) 
			 ArrowFindMeet);

	if (!meet) {
		panic ("Meet not found %s:%i:",__FILE__,__LINE__);
	}
	
	SetDelete(&objects);
	return *meet;
}


/*
 * ArrowJoinConcepts join a set of concepts and return the join from a
 * the set of all concepts. The set of all concepts must be lectically
 * ordered by the concepts attribute sets.
 */
 

static Concept*
ArrowJoinConcepts (set,m,concepts,n)

	Concept **set;		/* concepts to join */
	int m;			/* size of set */
	Concept **concepts;	/* all concepts */
	int n;			/* number of all concepts */

{
	Concept **join;		/* actual join */
	int c;			/* concept from concepts Set */
	Set attributes;		/* intersect */

	
	if (m == 0) { /* bottom */
		return concepts[n-1]; /* == *(concepts[n-1]) */
	}
	
	SetCopy (&set[0]->attributes,&attributes); /* creates attributes! */
	for (c=1;c<m;c++) {
		SetIntersect (&set[c]->attributes, &attributes, &attributes);
	}
	/* attributes is unique - we have to find it */
	
	join = (Concept**) bsearch 
		((char*)&attributes,
		 (char*)concepts,
		 n,
		 sizeof(Concept*),
		 (int (*)_ANSI_ARGS_((const void *,const void *))) 
		 ArrowFindJoin);

	if (!join) {
		panic ("Join not found %s:%i:",__FILE__,__LINE__);
	}

	SetDelete(&attributes);
	return *join;
}

/*
 * ArrowSortConcepts creates a sorted list of concepts. The order is
 * determined by the lectically ordering of the concept's attribute
 * sets. The initial table is first copied and sorted afterwards. */

static Concept **
ArrowSortConcepts (concepts,n)
	Concept **concepts;	/* concept array */
	int n;			/* size */

{
	Concept **tab;
	tab = (Concept**) malloc (sizeof(Concept*) * n);
	if (!tab) {
		panic ("Malloc failed in %s:%i",__FILE__,__LINE__);
	}
	/* copy table and sort it */
	memcpy ((char*)tab,(char*)concepts,sizeof(Concept*) * n);
	qsort(tab,n,sizeof(Concept*),
	      (int(*)_ANSI_ARGS_((const void *, const void *)))
	      ArrowCmpAtrConcepts);
	return tab;
}

/*
 * ArrowCreateArray given an array of Concept pointers and a set of
 * indices create a new array of pointers containing only pointers of
 * concepts mentioned in the set. The size of the array is returned in
 * n. if n==0 no array will be created and NULL returned instead*/

static Concept **
ArrowCreateArray (concepts,set,n)
	Concept **concepts;
	Set *set;
	int *n;

{
	Concept **tab;
	int s,c;

	if (0 == set->size) {
		return (Concept**) NULL;
	}
	tab = (Concept**) malloc (sizeof(Concept*) * set->size);
	if (!tab) {
		panic ("Malloc failed in %s:%i",__FILE__,__LINE__);
	}

	c = 0;
	s = SetFirst (set);
	while (-1 != s) {
		tab[c++] = concepts[s];
		s = SetNext(set,s);
	}
	*n = c;
	return tab;
}

/*
 * ArrowJoinSubconcepts
 * determines the join of all true subconcepts from a given concept
 */

static Concept *
ArrowJoinSubconcepts (concepts,sorted,n,concept)

	Concept **concepts;	/* array of concepts */
	Concept **sorted;	/* sorted array of concepts */
	int n;			/* size of array */
	int concept;		/* index into concepts */

{
	Set subconcepts;
	Concept **set;		/* array of concepts to join*/
	int setsize;		/* size of array */
	Concept *join;		/* computed join */

	/* determine subconcepts - remove concept from the
	   set of subconcepts */

	SetCopy (&concepts[concept]->closure, &subconcepts);
	SetRemove(&subconcepts,concept);

	set = ArrowCreateArray(concepts,&subconcepts,&setsize); 
	/* ArrowJoinConcepts needs the sorted array of concepts!! */
	join = ArrowJoinConcepts(set,setsize,sorted,n);

	if (!join) {
		panic ("Join not found %s:%i:",__FILE__,__LINE__);
	}

	free ((char*) set);
	SetDelete (&subconcepts);

	return join;

}

/*
 * ArrowMeetSuperconcepts
 * determines the meet of all true superconcepts of a given concepts
 */

static Concept *
ArrowMeetSuperconcepts (concepts,n,concept)
	Concept **concepts;	/* sorted list of all concepts */
	int n;			/* number of all concepts */
	int concept;		

{

	Set superconcepts;
	Concept **set;		/* array of concepts to meet*/
	int setsize;		/* size of array */
	Concept *meet;		/* computed join */

	/* determine subconcepts - remove concept from the
	   set of subconcepts */

	SetCopy (&concepts[concept]->superclos, &superconcepts);
	SetRemove(&superconcepts,concept);

	set = ArrowCreateArray(concepts,&superconcepts,&setsize); 
	/* ArrowMeetConcepts needs the sorted array of concepts!! */
	meet = ArrowMeetConcepts(set,setsize,concepts,n);

	if (!meet) {
		panic ("Meet not found %s:%i:",__FILE__,__LINE__);
	}

	free ((char*) set);
	SetDelete (&superconcepts);

	return meet;
}


/*
 * ArrowUpDown the arrow-up and arrow-down relation from a context by
 * utilizing the concept lattice of that context. Creates new
 * relations up and down to store the result; old contents will be
 * lost. */

/* ATTENTION: the Relation functions usually take arguments in the 
   order ATTRIBUTE, OBJECT ! */

VOID
ArrowUpDown (context, lattice, uprel, downrel)

	Context *context;
	ContextLattice *lattice;
	Relation *uprel,*downrel;	/* results stored here */

{
	int *objmap, *atrmap;	/* maps from obj/atr -> introducing conc. */
	int obj,atr;		/* indexes */
	Concept *join,*meet;
	Concept **concepts;	/* sorted list of concepts */
	int up,down;

	/* create relations of the appropriate size */
	RelNew (downrel,lattice->atr, lattice->obj);
	RelNew (uprel,lattice->atr,lattice->obj);
	
	/* calculate maps for introducing concepts */
	ArrowMaps (lattice,&objmap,&atrmap);

	/* create sorted array of concepts for join */
	concepts = ArrowSortConcepts(lattice->concepts,lattice->conc);

	/* compute down array relation */

	for (obj=0;obj<lattice->obj;obj++) {
		/* join all subconcepts of objmap[obj] */
		join = ArrowJoinSubconcepts(lattice->concepts,
					    concepts,
					    lattice->conc,
					    objmap[obj]);

		for(atr=0;atr<lattice->atr;atr++) {
			/* only unrelated pairs can be arrow related */
			if (ContextRelated(context,lattice->objects[obj],
					   lattice->attributes[atr])) {
				RelUnrelate(downrel,atr,obj);
				continue ;
			} else if (join == lattice->concepts[objmap[obj]]) {
				RelUnrelate(downrel,atr,obj);
				continue ;
			}

			meet = ArrowMeet(lattice->concepts,
					 lattice->conc,
					 lattice->concepts[objmap[obj]],
					 lattice->concepts[atrmap[atr]]);
			/* join != lattice->concepts[objmap[obj]] 
			   checked above */
			down = ( meet == join ) ;
			if (down) 
				RelRelate(downrel,atr,obj);
			else
				RelUnrelate(downrel,atr,obj);
		}
	}

	/* compute up array relation */

	for (atr=0;atr < lattice->atr;atr++) {
		/* meet all superconcepts of atrmap[atr] */
		meet = ArrowMeetSuperconcepts (lattice->concepts,
					       lattice->conc,atrmap[atr]);
		for (obj=0;obj<lattice->obj;obj++) {
			/* only unrelated pairs can be arrow related */
			if (ContextRelated(context,lattice->objects[obj],
					   lattice->attributes[atr])) {
				RelUnrelate(uprel,atr,obj);
				continue ;
			} else if (meet == lattice->concepts[atrmap[atr]]) {
				RelUnrelate(uprel,atr,obj);
				continue ;
			}
			/* join need differently sorted concepts! */
			join = ArrowJoin(concepts, lattice->conc,
					 lattice->concepts[atrmap[atr]],
					 lattice->concepts[objmap[obj]]);
			up = ( meet == join );
			if (up) 
				RelRelate(uprel,atr,obj);
			else
				RelUnrelate(uprel,atr,obj);
		}
	}
	
	free((char*)concepts);
	free((char*)atrmap);
	free((char*)objmap);
}
	
	

