

/*
 * $Id: context.c,v 1.1.1.1 1998/02/23 15:22:26 lindig Exp $
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
 * provides high level functions to build up a context (which is really a 
 * relation) and to calculate its concepts. */

/* requires: ConceptDelete ConceptLattice ConceptNumber
   HashCreateEntry HashEqEntry HashFindEntry HashFirstEntry HashGetId
   HashGetKey HashGetValue HashNextEntry HashSetValue HashTabDelete
   HashTabInit HashToMap ListDelete ListFindEntry ListGetValue
   ListInit ListNewEntry ListSetValue RelDelete RelNew RelRelate free
   malloc panic strdup */

#include "config.h"
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
#include "list.h"
#include "hash.h"
#include "set.h"
#include "relation.h"
#include "concept.h"
#include "context.h"

#ifndef HAVE_STRDUP
# include "lib.h"
#endif


/*
 * ContextNew create a new context and initialize it. A Context holds
 * objects and attributes (both as strings) in hash tables. Attributes
 * are related to objects by calling ContextAddAttribute for a given
 * object. To relate objects to attributes is not possible. Each
 * object contains a list as client, which holds pointers to
 * HashEntries (attributes). */

Context *
ContextNew ()

{
	Context *context ;

	context = (Context*) malloc (sizeof (Context));
	if (!context) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}
	HashTabInit(&context->objects);
	HashTabInit(&context->attributes);
	
	return (context);
}


/*
 * ContextDelete
 * remove an existing context from memory. */

VOID
ContextDelete (context) 

	Context *context ;

{
	HashSearch search;
	HashEntry *entry ;
	List *list;

	
	/* traverse all objects. Each object contains a list as
           client, which holds pointers to HashEntries
           (attributes). */

	entry = HashFirstEntry (&context->objects,&search);
	while (entry) {
		list = (List*) HashGetValue(entry);
		ListDelete (list); /* list contents */
		free((char*)list); /* list struct */
		entry = HashNextEntry(&context->objects,&search);
	}
	HashTabDelete(&context->objects);

	/* traverse all attributes */

	entry = HashFirstEntry (&context->attributes,&search);
	while (entry) {
		list = (List*) HashGetValue(entry);
		ListDelete (list); /* list contents */
		free((char*)list); /* list struct */
		entry = HashNextEntry(&context->attributes,&search);
	}
	HashTabDelete(&context->attributes);

	/* free context itself */

	free ((char*) context);
}

/*
 * ContextAddObject
 *
 * adds an objects <name> to a given context. Returns a handle which is
 * needed to add attributes to that object. */

HashEntry *
ContextAddObject (context,name)
	
	Context *context;
	char *name;

{
	int new ;
	HashEntry *obj ;
	List *list ;

	obj = HashCreateEntry(&context->objects,name,&new);
	if(new) {
		/*
		 * create a list as client data
		 */
		list = (List*) malloc (sizeof(List));
		if (!list) {
			panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
		}
		/* initialize list */
		ListInit (list);
		HashSetValue(obj,(char*)list);
	} /* else nothing has to be done - there allready exists a
	     list - the object has been previously added*/
	
	return obj;
}       

/*
 * ContextAddAttribute
 *
 * add an attribute to an object of a given context. Nothing is returned */

VOID
ContextAddAttribute (context,obj,name)

	Context *context;
	HashEntry *obj;
	char *name;

{

	List *list;
	ListEntry *entry;	/* list entry at obj, pointing at attr*/
	HashEntry *attr;	/* real location of attribute */
	int new ;

	/* each attribute is stored in the hashtable and has an
           associated list. The list contains pointers to the objects
           the attribut is related to. Also each object has a list
           with pointers to its attributes */

	attr = HashCreateEntry (&context->attributes,name,&new);
	if(new) {
		/*
		 * create a list as client data
		 */
		list = (List*) malloc (sizeof(List));
		if (!list) {
			panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
		}
		/* initialize list */
		ListInit (list);
		HashSetValue(attr,(char*)list);
	} else {
		/* get list from client data */

		list = (List*) HashGetValue(attr);
	}

	/* add a pointer to obj at the list at atr */
	/* check for duplicate */

	entry = ListFindEntry(list,(char*)obj,
			      (int (*)_ANSI_ARGS_((char *x, char *y)))
			      HashEqEntry);
	if (!entry) {
		/* no entry found - create one */
		entry = ListNewEntry(list);
		ListSetValue(entry,(char*)obj);
	} else 
		/* else: this attribute has been previously related to the 
		   object - nothing to do */

		return ;

	
	/* add this attr to the list of attributes at obj */

	list = (List*) HashGetValue (obj); /* get list at obj */
	entry = ListFindEntry(list,(char*)attr,
			      (int (*)_ANSI_ARGS_((char *x, char *y)))
			      HashEqEntry);
	if (!entry) {
		/* not member yet - create new entry */
		entry = ListNewEntry(list); /* create new entry in that list */
		ListSetValue (entry,(char*)attr); /* add attr to list at obj */
	} else {

		/* because the attribute was previously unrelated to
                   obj, there can't be an entry - we have found one,
                   so it's an internal error */

		panic ("internal error in %s:%i",__FILE__,__LINE__);
	} 
}


/*
 * ContextConcepts
 *
 * calculate all concepts from a given context and return it in 
 * a ContextLattice struct
 */

ContextLattice *
ContextConcepts (context)

	Context *context ;

{
	ContextLattice *lattice;
	Relation rel;
	char **objstr,**atrstr;	/* all names of obj and atr */
	int obj,atr;
	ConceptArray array;	/* all concepts stored here */
	
	lattice = (ContextLattice*) malloc (sizeof (ContextLattice));
	if (!lattice) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}

	/* create Relation from Context */

	ContextToRel(context,&rel,&objstr,&atrstr,&obj,&atr);

	/* assign strings and sizes to lattice */

	lattice->obj = obj ;
	lattice->atr = atr ;
	lattice->objects = objstr ;
	lattice->attributes = atrstr ;
	
	/* calculate concepts */

	ConceptLattice(&rel,&array);
	lattice->concepts = array.concepts ;
	lattice->conc = array.size ;

	/* free memory */
	
	RelDelete (&rel);

	return lattice ;
}

/*
 * ContextNumber
 *
 * calculate the number of concepts from a given context and return
 * that number */

int 
ContextNumber (context)

	Context *context;

{
	Relation rel;
	char **objstr,**atrstr;	/* all names of obj and atr */
	int obj,atr;
	int count ;
	int i;

	/* create Relation from Context */

	ContextToRel(context,&rel,&objstr,&atrstr,&obj,&atr);
	
	/* calculate concepts */

	count = ConceptNumber(&rel);

	/* free memory */
	
	RelDelete (&rel);

	for (i=0;i<obj;i++) {
		free(objstr[i]);
	}
	free ((char*)objstr);

	for (i=0;i<atr;i++) {
		free(atrstr[i]);
	}
	free ((char*)atrstr);

	return count;

}

/*
 * ContextToRel
 *
 * create a Relation from a Context and return it. The mapping from
 * intergers (in Relation) and names (in Context) for objects and
 * attributes are also returned */

VOID 
ContextToRel (context,rel,objstr,atrstr,objs,atrs)

	Context *context;
	Relation *rel;
	char ***objstr,***atrstr;
	int *objs,*atrs;	/* number of obj and atr */

{

	int obj,atr;		/* number of ... */
	HashMap objmap,atrmap;	/* maps for .. */
	int i;			/* general purpose index */
	char **objarray,**atrarray;
	List *list;
	ListEntry *entry;


	/* use mappings instead of the raw HashTables */

	HashToMap(&context->objects,&objmap);
	HashToMap(&context->attributes,&atrmap);


	/* create space for all strings */
	 
	obj = objmap.size ;
	atr = atrmap.size ;

	/* return these numbers */

	*objs = obj ;
	*atrs = atr ;

	objarray = (char**) malloc (sizeof (char*) * obj);
	if (!objarray) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}

	atrarray = (char**) malloc (sizeof (char*) * atr);
	if (!atrarray) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}

	/* initialize relation for (obj,atr) relation */

	RelNew(rel,atr,obj);

	/* traverse mappings and copy all names, store them in
           objstr,atrstr */

	for (i=0;i<objmap.size;i++) {
		objarray[i] = strdup (HashGetKey(objmap.map[i]));
		if (!objarray[i]) 
			panic ("strdup failed in %s:%i",__FILE__,__LINE__);
	}
	for (i=0;i<atrmap.size;i++) {
		atrarray[i] = strdup (HashGetKey(atrmap.map[i]));
		if (!atrarray[i]) 
			panic ("strdup failed in %s:%i",__FILE__,__LINE__);
	}

	/* set atrstr,objstr */

	*atrstr = atrarray;
	*objstr = objarray;

	/* enter relation to rel */

	for (i=0;i<objmap.size;i++) {
		if (!objmap.map[i])
			break ;
		list = (List*) HashGetValue (objmap.map[i]);
		entry = list->first ;
		while (entry) {
			RelRelate(rel,
				  HashGetId((HashEntry*)ListGetValue(entry)),
				  i);
			entry = entry->next ;

		}
		
	}

	/* free space malloc()'ed by HashToMap */
	 
	free ((char*)objmap.map);
	free ((char*)atrmap.map);

}

/*
 * ContextRelated
 * is a given object related to a given attribute?
 */


int
ContextRelated (context,obj,atr)

	Context *context;
	char *obj;
	char *atr;

{
	HashEntry *object,*attribute;
	List *list;
	ListEntry *entry;

	object = HashFindEntry (&context->objects,obj);
	if (!object) {
		/* no such object! */
		panic ("unknown object: %s in %s:%i",obj,__FILE__,__LINE__);
	}
	attribute = HashFindEntry (&context->attributes,atr);
	if (!attribute) {
		/* no such object! */
		panic ("unknown attribute: %s in %s:%i",atr,__FILE__,__LINE__);
	}

	/* obj and atr are known - are they related? */

	list = (List*) HashGetValue (object);
	entry = ListFindEntry(list,(char*)attribute,
			      (int (*)_ANSI_ARGS_((char *x, char *y)))
			      HashEqEntry);

	return (entry != (ListEntry*)0);
	
}



/*
 * ContextLatticeDelete
 * 
 * free all memory associated with a ContextLattice */

VOID
ContextLatticeDelete (lattice)

	ContextLattice *lattice ;

{

	int i;

	/* get rid of all strings */

	for (i=0;i<lattice->obj;i++) {
		if (!lattice->objects[i]) 
			break ;
		free (lattice->objects[i]);
	}
	free ((char*)lattice->objects);

	for (i=0;i<lattice->atr;i++) {
		if (!lattice->attributes[i]) 
			break ;
		free (lattice->attributes[i]);
	}
	free ((char*)lattice->attributes);

	/* get rid of concepts */

	for (i=0;i<lattice->conc;i++) {
		ConceptDelete(lattice->concepts[i]);
		free ((char*)lattice->concepts[i]); /* free Concept struct */
	}
	free ((char*) lattice->concepts);	/* free pointer array */

	/* free ContextLattice */

	free ((char*) lattice);
 
}
 
