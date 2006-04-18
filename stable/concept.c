
/*
 * $Id: concept.c,v 1.1.1.1 1998/02/23 15:22:26 lindig Exp $
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
 * Functions for calculating concepts from a relation.
 */ 

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
#include "list.h"
#include "set.h"
#include "relation.h"
#include "concept.h"

/* requires: ListDelete ListGetValue ListInit ListNewEntry
   ListSetValue RelAtr RelObj SetAdd SetCopy SetDelete SetFirst
   SetFull SetInit SetIntersect SetJoin SetLe SetMember SetMinus
   SetNew SetNext SetPrint SetRemove SetSubset free malloc panic
   printf */
 

/*
 * ConceptExtent
 *
 * from a Set of attributes calculate the extent - a Set of objects -
 * with respect to a given relation of objects and attributes.
 * <result> must be of the right size, i.e. context->maxobj and must
 * be initalized (using SetNew()). The extent of a set of attributes
 * is the set of objects they have in common. A better name would have
 * been ConceptCommonAtr or similar */

static VOID
ConceptExtent (result,attrib,context)

	Set *result,*attrib;
	Relation *context;
{
	int a ;			/* attribute */
	Set *obj ;		/* objects related to given attrib */

	if (context->maxobj != result->size ||
	    context->maxatr != attrib->size) {
		panic ("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}
	
	SetFull (result) ;	/* all obj present before intersect */
	
	/* visit all attributes and intersect the related object sets */
	  
	a = SetFirst (attrib) ;
	while (a != -1) {
		/* intersect the object sets from the attributes given 
		   in attrib */
		obj = RelAtr(context,a) ; /* obj related to attrib a */
		SetIntersect (result,obj,result); /* intersect sets */
		a = SetNext(attrib,a) ;	/* visit next attrib */

	}
}

/*
 * ConceptIntent
 * 
 * From a Set of objects calculate the intent - a Set of attributes -
 * with respect to a given relation. The resulting Set of attributes
 * must be oft the right size and must be initialized (using
 * SetNew())! The intent is the set of attributes the objects have in
 * common. The name is misleading */

VOID
ConceptIntent (result,obj,context)

	Set *result,*obj;
	Relation *context;
{	
	int o ;			/* object of intreset */
	Set *atr ;		/* set of attributes */

	if (context->maxatr != result->size ||
	    context->maxobj != obj->size) {
		panic ("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}
	
	SetFull (result) ;	/* all attributes present */

	o = SetFirst (obj) ;	/* visit all objects and intersect attrs */
	while (o != -1) {
		
		atr = RelObj(context,o) ; /* attr related to obj o */
		SetIntersect (result,atr,result);
		o = SetNext(obj,o) ; /* next object */
	}
}

/*
 * ConceptCExtent
 * 
 * From a set of objects calculate the closed extent:
 * close_extent=extent(intent(obj)) - a set of objects with respect
 * to a given relation. The result will be stored in <result>. To a
 * closed extent there also exists a set of attributes which is stored
 * in <atr>. <result> and <atr> together form a concept. The Sets
 * <result> and <atr> must be of the right size and must be existent
 * (SetNew()). */

static VOID
ConceptCExtent (result,obj,atr,context)

	Set *result,*obj,*atr;
	Relation *context ;
{

	if (obj->size != result->size ||
	    obj->size != context->maxobj ||
	    atr->size != context->maxatr) {
		panic ("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}
	ConceptIntent (atr,obj,context) ;
	ConceptExtent (result,atr,context);
}


/*
 * ConceptFirstExtent
 *
 * Calculate the first concept (closed extent) from with respect to a
 * given relation <context> and store it to <result> - a Set of
 * objects. The corresponding Set of attributes will be stored in
 * <atr>. returns 1 (for convienience) because there allways exists a
 * first concept */

static int
ConceptFirstExtent (result,atr,context)
	
	Set *result,*atr;
	Relation *context;

{
	Set objects ;

	if (result->size != context->maxobj ||
	    atr->size != context->maxatr) {
		panic ("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}
	
	SetNew (&objects,context->maxobj) ;
	/*
	 * object is empty - the first concept has an empty object. 
	 */
	ConceptCExtent (result,&objects,atr,context);
	SetDelete(&objects);
	return 1;
}

/*
 * ConceptNextExtent
 *
 * from a set of given objects <old> calculate the next concept
 * consisting of a set of objects <new> and a set of attributes <atr>
 * with respect to <context>. <new> and <atr> must be of the right size.
 * 
 * This function implements Ganter's algorithm to compute the next
 * intent as presented in : B. Ganther, R.  Wille, K.E.  Wolff (Eds.),
 * Beitraege zur Begriffsanalyse (Contributions to Concept Analysis),
 * 1987,BI-Wissenschafts-Verlag, Mannheim, Germany
 *
 * 
 * Returns 1 if a next <new>,<atr> pair was found, 0 otherwise */

static int 
ConceptNextExtent (old,new,atr,context)

	Set *old,*new,*atr;
	Relation *context ;

{
	Set objects;		/* holds candidate for next concept */
	int found ;		/* candidate found? */
	int o ;			/* object */

	if (old->size != new->size ||
	    old->size != context->maxobj ||
	    atr->size != context->maxatr) {
		panic ("Size mismatch in %s:%i",__FILE__,__LINE__) ;
	}

	found = 0 ;
	o = context->maxobj - 1 ; /* start with highest object */

	SetInit(&objects);	/* remember <old> in objects */
	SetCopy(old,&objects);
	
	while (!found && (o>=0)) {
		if (!SetMember(&objects,o)) {
			SetAdd(&objects,o);
			ConceptCExtent(new,&objects,atr,context);
			found = SetLe (&objects,new,o);
		}
		SetRemove(&objects,o);
		o-- ;		/* check next object */
	}
	SetDelete (&objects);	/* delete candidate */
	return found ;
}

/*
 * ConceptInit
 *
 * initialize a Concept structure which will be derived from the
 * relation <rel>. */

VOID
ConceptInit (c,rel)

	Concept *c;
	Relation *rel;

{
	SetNew(&c->objects, rel->maxobj);
	SetNew(&c->attributes, rel->maxatr);
	
	SetInit (&c->obj);
	SetInit (&c->atr);

	SetInit(&c->subconcepts); /* size not known yet */
	SetInit(&c->closure);	/* size not known yet */

	SetInit(&c->superclos);	/* size unknown */
}

/*
 * ConceptDelete
 *
 * Remove a concept from memory. Does not free the struct <c> itself. */

VOID
ConceptDelete (c) 

	Concept *c;
{
	SetDelete(&c->objects);
	SetDelete(&c->attributes);
	
	SetDelete(&c->obj);
	SetDelete(&c->atr);

	SetDelete(&c->subconcepts);
	SetDelete(&c->closure);
	SetDelete(&c->superclos);
}


/*
 * ConceptPrint
 * prints out a concept - just for debugging
 */

VOID 
ConceptPrint (c) 

	Concept *c;
{
	printf ("\nconcept-objects:\n");
	SetPrint (&c->objects);
	printf ("\nconcept-attributes:\n");
	SetPrint (&c->attributes);

	printf ("\nconcept-obj:\n");
	SetPrint (&c->obj);
	printf ("\nconcept-atr:\n");
	SetPrint (&c->atr);

	printf ("\nconcept-subconcepts:\n");
	SetPrint (&c->subconcepts);
	printf ("\nconcept-closure:\n");
	SetPrint (&c->closure);

	printf ("\nconcept-super-closure:\n");
	SetPrint (&c->superclos);

	printf ("\n");
}

/*
 * ConceptArrayInit
 *
 * set an ConceptArray to defined values. Once all concepts have been
 * calculated it's possible to store them in a ConceptArray */

VOID
ConceptArrayInit (array) 

	ConceptArray *array ;

{
	array->size = 0;
	array->concepts = (Concept**) NULL ;

}

/*
 * ConceptArrayDelete
 *
 * free all memory associated with an ConceptArray */

VOID 
ConceptArrayDelete (array)
	
	ConceptArray *array ;

{
	int i;

	if (array->size == 0 || !array->concepts) {
		panic ("empty array in %s:%i",__FILE__,__LINE__) ;
	}
	for (i=0;i<array->size;i++) {
		ConceptDelete(array->concepts[i]);
		free ((char*)array->concepts[i]); /* free Concept struct */
	}
	free ((char*) array->concepts);	/* free pointer array */
	array->concepts = (Concept**) NULL ;

}
 
/*
 * ConceptLattice
 *
 * calculate a concept lattice from a given relation. All concepts are
 * stored in a ConceptArray <lattice>. Because the cumber of concepts
 * is not known in advance they are internally stored in a list before
 * the array is built.
 *
 * The concept lattice is returned as an array of concepts which are
 * ordered in this way: the top has index 0, the bottom the greates
 * index.  In general a subconcept has a higher index than all its
 * superconcepts */

VOID
ConceptLattice (rel,lattice) 

	Relation *rel;
	ConceptArray *lattice;

{
	int more ;		/* flag */
	List list ;		/* will be list of Concepts */
	ListEntry *entry;	/* list entry to hold concept */
	Concept *prev,*new;	/* previos and actual concept */
	Concept **array, **tmp ;
		
	ListInit (&list);
	prev = (Concept*) malloc (sizeof(Concept)); 
	if (!prev) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}

	ConceptInit (prev,rel);	/* initialize */
	ConceptFirstExtent (&prev->objects,&prev->attributes,rel);
	do {
		/* enter to list */	
		entry = ListNewEntry (&list);
		ListSetValue(entry,(char*)prev); 

		/* calculate next concept */

		new = (Concept*) malloc (sizeof(Concept));
		if (!new) {
			panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
		}
		ConceptInit (new,rel);
		more = ConceptNextExtent(&prev->objects, &new->objects,
					&new->attributes, rel);
		prev = new ;
	} while (more) ;

	/* we malloced one Concept too often - the data computed by the 
	   last ConceptNextExtent call is garbage */

	ConceptDelete (new);	/* not needed - stuff from ConceptInit()*/
	free ((char*)new);	/* not needed - Concept struct */

	/* now all concepts are stored in list - we will now build up
           an array of concepts and can throw away the list (not its
           contents) */
	
	/* new will point now to a an array of Concept pointers */

	array = (Concept**) malloc (sizeof (Concept*) * list.entries);
	if (!array) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}

	/* enter all concept pointers to the array - the list may be
           freed afterwards */

	tmp = array ;
	entry = list.first ;
	while (entry) {
		/* lint complains: warning: possible pointer alignment
		 * problem, but I can't see why */

		*(tmp++) = (Concept*) ListGetValue (entry); 
		entry = entry->next ; 
	}
	
	lattice->size = list.entries ;
	lattice->concepts = array ;
	
	ListDelete(&list);	/* removes list, but not client data */

	/* all raw data is now stored in <lattice>, we just need to
	   calculate the sub-concept relation. This is done in the
	   next call. */

	ConceptSubconcepts (lattice);
	
}

/*
 * ConceptNumber
 *
 * calculate the number of concepts from a context (really a relation)
 * and return it */

int
ConceptNumber (rel) 

	Relation *rel;

{
	int more ;		/* flag */
	int count ;		/* number of concepts */
	Concept c1,c2;	        /* previos and actual concept */
	Concept *prev,*new;
		
	count = 0;		/* number of concepts */

	ConceptInit (&c1,rel);	/* initialize */
	ConceptInit (&c2,rel);
	prev = &c1;
	new = &c2;

	ConceptFirstExtent (&prev->objects,&prev->attributes,rel);
	count++ ;

	do {
		more = ConceptNextExtent(&prev->objects, &new->objects,
					&new->attributes, rel);
		count++;
		/* new becomes old */
		if (prev == &c1) {
			prev = &c2 ;
			new = &c1 ;
		} else {
			prev = &c1;
			new = &c2;
		}
	} while (more) ;

	/*  data computed by the last ConceptNextExtent call is garbage */

	count--;
	ConceptDelete (&c1);	/* not needed - stuff from ConceptInit()*/
	ConceptDelete (&c2);

	return count;
}

/*
 * ConceptSubconcepts
 *
 * takes a ConceptArray struct with raw concept data and computes the
 * sub-concept relation on it. The result is also filled into the
 * ConceptArray. */

VOID 
ConceptSubconcepts (array)

	ConceptArray *array ;

{
	int i,top,actual;

	/*
	 * first create for each Concept and array for closure and
	 * subconcepts (of size array->size) */

	for (i=0;i<array->size;i++) {

		SetNew(&array->concepts[i]->subconcepts, array->size); 
		SetNew(&array->concepts[i]->closure, array->size);
		SetNew(&array->concepts[i]->superclos, array->size);

		SetCopy(&array->concepts[i]->objects,
			&array->concepts[i]->obj);
		SetCopy(&array->concepts[i]->attributes,
			&array->concepts[i]->atr);
			
	}

	for (top=array->size-1;top>=0;top--) {
		
		/* each concepts is subconcept of itself (reflexsive) */
		SetAdd(&array->concepts[top]->closure,top);
		/* this is also true for super concepts */
		SetAdd(&array->concepts[top]->superclos, top);
		
		for(actual=top+1;actual<array->size;actual++) {

			if(SetSubset(&array->concepts[top]->objects,
					  &array->concepts[actual]->objects)) {
				
				/* actual is a subconcept of top but
				   necessary not a direct subconcept -
				   so top belongs to the closure
				   of superconcepts of actual */

				SetAdd(&array->concepts[actual]->superclos,
				       top);

				/* remove the objects of actual from top */

				SetMinus(&array->concepts[top]->obj,
					 &array->concepts[actual]->objects,
					 &array->concepts[top]->obj);

				/* remove the attributes of top from actual */

				SetMinus(&array->concepts[actual]->atr,
					 &array->concepts[top]->attributes,
					 &array->concepts[actual]->atr);

				if (!SetMember(&array->concepts[top]->closure,
					       actual)) {

					/* actual is direct(!) subconcept 
					   of top */
				
					SetAdd(&array->concepts[top]
					       ->subconcepts,actual);
					SetAdd(&array->concepts[top]
					       ->closure,actual);
					SetJoin(&array->concepts[top]
						->closure,
						&array->concepts[actual]
						->closure,
						&array->concepts[top]
						->closure);
				} /* if (!SetMember()) */
			} /* if (SetSubset()) */
		}
	}
}


