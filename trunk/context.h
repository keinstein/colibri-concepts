

/*
 * $Id: context.h,v 1.1.1.1 1998/02/23 15:22:27 lindig Exp $
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


#ifndef __CONTEXT_H__
#define __CONTEXT_H__

typedef struct {
	HashTable objects;
	HashTable attributes;
} Context ;

typedef struct {
	int obj;		/* number of objects */
	int atr;		/* number of attributes */
	int conc;		/* number of concepts */
	char **objects ;	/* array of all objects (strings) */
	char **attributes ;	/* array of all attributes */
	Concept **concepts ;	/* array of concepts */
} ContextLattice ;

/* context.c */
extern Context *ContextNew _ANSI_ARGS_((void));
extern VOID ContextDelete _ANSI_ARGS_((Context *context));
extern HashEntry *ContextAddObject _ANSI_ARGS_((Context *context, char *name));
extern ContextLattice *ContextConcepts _ANSI_ARGS_((Context *context));
extern VOID ContextLatticeDelete _ANSI_ARGS_((ContextLattice *lattice));
extern VOID ContextToRel _ANSI_ARGS_((Context *context, 
				      Relation *rel, char ***objstr, 
				      char ***atrstr, int *objs, int *atrs));
extern VOID ContextAddAttribute _ANSI_ARGS_((Context *context, 
					     HashEntry *obj, char *name));
extern int ContextNumber _ANSI_ARGS_((Context *context));
extern int ContextRelated _ANSI_ARGS_((Context *context, 
				       char *obj, char *atr));
#endif /* ! __CONTEXT_H__ */
