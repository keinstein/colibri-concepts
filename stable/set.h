/*
 * $Id: set.h,v 1.3 1998/02/25 16:22:09 lindig Exp $
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
 * This file implements sets based on bit arrays. When a set is 
 * created its  maximum size must be specified. 
 */

#ifndef __SET_H__
#define __SET_H__

/*
 * ulong arrays are used to implement bit arrays. We define them here 
 * - other architectures may require a different definition.
 */
 
typedef unsigned long setbase ;

#define SET_BITS 	(sizeof (setbase) * 8)
#define SET_EMPTY 	0L
#define SET_FULL 	ULONG_MAX
#define SET_ONE 	1L

/* 
 * a set consists if a size and a pointer to <size> setbases
 */

typedef struct {
	int size ;
	setbase *set ;		/* points to <size> setbases - not just one! */
} Set ;

/* DO NOT DELETE THIS LINE */ 
/* set.c */
extern VOID SetInit _ANSI_ARGS_((Set *set));
extern VOID SetEmpty _ANSI_ARGS_((Set *set));
extern VOID SetNew _ANSI_ARGS_((Set *set, int size));
extern VOID SetCopy _ANSI_ARGS_((Set *source, Set *dest));
extern VOID SetDelete _ANSI_ARGS_((Set *set));
extern VOID SetFull _ANSI_ARGS_((Set *set));
extern VOID SetAdd _ANSI_ARGS_((Set *set, int elem));
extern VOID SetRemove _ANSI_ARGS_((Set *set, int elem));
extern VOID SetJoin _ANSI_ARGS_((Set *a, Set *b, Set *result));
extern VOID SetMinus _ANSI_ARGS_((Set *a, Set *b, Set *result));
extern VOID SetIntersect _ANSI_ARGS_((Set *a, Set *b, Set *result));
extern int SetMember _ANSI_ARGS_((Set *set, int elem));
extern int SetFirst _ANSI_ARGS_((Set *set));
extern int SetNext _ANSI_ARGS_((Set *set, int prev));
extern int SetSubset _ANSI_ARGS_((Set *a, Set *b));
extern int SetEqual _ANSI_ARGS_((Set *a, Set *b));
extern int SetLe _ANSI_ARGS_((Set *a, Set *b, int e));
extern VOID SetPrint _ANSI_ARGS_((Set *set));
extern int SetSize _ANSI_ARGS_((Set *set));
extern int SetLectical _ANSI_ARGS_((Set *a, Set *b));
#endif /* ! __SET_H__ */
