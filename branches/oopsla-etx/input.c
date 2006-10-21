

/*
 * $Id: input.c,v 1.1.1.1 1998/02/23 15:22:27 lindig Exp $
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
 * Provide functions for parse tree build up */

#include "config.h"
#if defined(STDC_HEADERS) || defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#endif
#ifdef DBMALLOC
#  include <malloc.h>
#endif

#include "defines.h"
#include "panic.h"
#include "hash.h"
#include "list.h"
#include "set.h"
#include "relation.h"
#include "concept.h"
#include "context.h"
#include "input.h"

/* requires: ContextAddAttribute ContextAddObject ListDelete
   ListGetValue ListNew ListNewEntry ListSetValue free
   malloc panic */

/*
 * InputNew
 *
 * create a Input struct, initialize it and return a pointer to
 * it. Call InputDelete() to destroy it. Input holds a parse tree
 * where attributes are associated to objects */

Input *
InputNew ()
{
	Input *input;

	input = (Input*) malloc (sizeof (Input)) ;
	if (!input) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}
	InputInit (input);
	return input;
}

/*
 * InputInit
 *
 * initialize a Input struct */

VOID
InputInit (input)

	Input *input;

{
	input->objlist = ListNew();
}

/*
 * InputObjNew create a new object, associate a list of attribute to
 * it and return a pointer to it. The object is yet not added to the
 * parse tree but stays on its own */

InputObj *
InputObjNew (name,atrlist)
	
	char *name;		/* the objects's name */
	List *atrlist;		/* list of attributes */

{
	InputObj *obj;

	obj = (InputObj*) malloc (sizeof (InputObj)) ;
	if (!obj) {
		panic ("malloc failed in %s:%i",__FILE__,__LINE__) ;
	}
	/* initialize */

	obj->objstr = name;
	obj->atrlist = atrlist ;

	return obj;
}

/*
 * InputAddObj add an object to the parse tree.  */

VOID
InputAddObj(input,obj)
	
	Input *input;
	InputObj *obj;

{
	ListEntry *entry ;

	entry = ListNewEntry (input->objlist);
	ListSetValue(entry,(char*)obj);
}


/*
 * InputDelete remove a parse tree from memory - including the input
 * struct itself. Thus it's not possible to have an Input variable but
 * only an Input* variable */

VOID
InputDelete (input)

	Input *input;

{

	ListEntry *obj;		/* list entry - not the real stuff */
	ListEntry *atr;		/* list entry - client data is char* */
	
	InputObj *theobj ;	/* the real object */


	/* traverse all objects */

	if (input->objlist) {
		obj = input->objlist->first ;
		while (obj) {
			/* traverse all attributes */
			theobj = (InputObj*) ListGetValue (obj);
			/* there is a list in theobj */
			atr = theobj->atrlist->first ;
			while (atr) {
				/* free client data */
				free (ListGetValue(atr));
				atr = atr->next;
			}
			/* free data associated with theobj */
			ListDelete (theobj->atrlist);
			free ((char*)theobj->atrlist);
			free (theobj->objstr);
			free ((char*)theobj);
			obj = obj->next;
		}
	}
	
	/* free memory associated with objlist */

	ListDelete (input->objlist);
	free ((char*)input->objlist);

	/* free Input struct */

	free ((char*) input);
}
 
/*
 * InputToContext 
 * enter the contents of an Input parse tree to a
 * Context. The context should be craeted with ContextNew and
 * otherwise unused. On return the context is filled according to the
 * input. */

VOID
InputToContext (input,context)

	Input *input;
	Context *context;

{

	ListEntry *obj;		/* list entry - not the real stuff */
	ListEntry *atr;		/* list entry - client data is char* */
	InputObj *theobj ;	/* the real object */
	HashEntry *objhandle;	/* handle describing object */


	/* traverse all objects */

	if (input->objlist) {
		obj = input->objlist->first ;

		while (obj) {
			/* traverse all attributes */
			theobj = (InputObj*) ListGetValue (obj);
			/* enter object to context */
			objhandle = ContextAddObject(context,theobj->objstr);
			/* there is a list of attributes in theobj */
			atr = theobj->atrlist->first ;
			while (atr) {
				/* add attribute to context/obj */
				ContextAddAttribute(context,
						     objhandle,
						     ListGetValue(atr));
				atr = atr->next;
			}
			obj = obj->next;
		}
	}
}
 

 
 
