

/*
 * $Id: concept.h,v 1.1.1.1 1998/02/23 15:22:27 lindig Exp $
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
 */ 


#ifndef __CONCEPT_H__
#define __CONCEPT_H__

/*
 * a concept consists of a set of objetcs (belonging to that concept),
 * a set of subconcepts and the closure of all subconcepts.
 */

typedef struct  {
	Set objects ;		/* objects belonging to concept */
	Set attributes ;	/* attributes */
	
	Set obj;		/* obj introduced by this conc */
	Set atr;		/* ditto for atr */

	Set subconcepts ;	/* set of direct subconcepts */
	Set closure ;		/* closure of subconcepts */
	
	Set superclos ;		/* closure of superconcepts */
} Concept ;

typedef struct {
	int size ;		/* number of concepts */
	Concept **concepts;	/* array of concepts */
} ConceptArray ;


/* DO NOT DELETE THIS LINE */
/* concept.c */
extern VOID ConceptIntent _ANSI_ARGS_((Set *result, Set *obj, 
				       Relation *context));
extern VOID ConceptInit _ANSI_ARGS_((Concept *c, Relation *rel));
extern VOID ConceptDelete _ANSI_ARGS_((Concept *c));
extern VOID ConceptPrint _ANSI_ARGS_((Concept *c));
extern VOID ConceptArrayInit _ANSI_ARGS_((ConceptArray *array));
extern VOID ConceptArrayDelete _ANSI_ARGS_((ConceptArray *array));
extern VOID ConceptLattice _ANSI_ARGS_((Relation *rel, ConceptArray *lattice));
extern VOID ConceptSubconcepts _ANSI_ARGS_((ConceptArray *array));
extern int ConceptNumber _ANSI_ARGS_((Relation *rel));

#endif /* ! __CONCEPT_H__ */
