

/*
 * $Id: relation.h,v 1.1.1.1 1998/02/23 15:22:27 lindig Exp $
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
 * Implements a relation between intergers of known maximum.
 *
 */ 


#ifndef __RELATION_H__
#define __RELATION_H__

/*
 * a relation is modeled using arrays of Sets (with some redundancy).
 * obj points to an array of objmax Sets, each holding maxatr
 * elements. atr points to an array of atrmax Sets, each holding
 * objmax elements. 
 *
 * obj points to an array of [0..<maxobj-1>] Sets. Each Set holds up
 * to maxatr attributes [0..<maxtr-1>].  atr points to an array of
 * [0..<maxatr-1>] Sets. Each Set holds up to maxobj objects
 * [0..<maxobj-1>]. */

typedef struct {
	int maxatr, maxobj ;
	Set *obj;
	Set *atr;
} Relation ;

/* DO NOT DELETE THIS LINE */
/* relation.c */
extern VOID RelNew _ANSI_ARGS_((Relation *rel, int atr, int obj));
extern VOID RelDelete _ANSI_ARGS_((Relation *rel));
extern VOID RelRelate _ANSI_ARGS_((Relation *rel, int atr, int obj));
extern VOID RelUnrelate _ANSI_ARGS_((Relation *rel, int atr, int obj));
extern int RelRelated _ANSI_ARGS_((Relation *rel, int atr, int obj));
extern Set *RelObj _ANSI_ARGS_((Relation *rel, int obj));
extern Set *RelAtr _ANSI_ARGS_((Relation *rel, int atr));
extern VOID RelPrint _ANSI_ARGS_((Relation *rel));

#endif /* ! __RELATION_H__ */
