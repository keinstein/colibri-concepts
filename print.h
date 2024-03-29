
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
 * Handle all output aspects 
 */


/* new.c */
extern VOID PrintLattice _ANSI_ARGS_((FILE *f, char *fmt, 
				      ContextLattice *lattice));
extern VOID PrintGraph _ANSI_ARGS_((ContextLattice *lattice, 
				    int print_names,
				    FILE *out));
extern VOID PrintDot _ANSI_ARGS_((ContextLattice *lattice, char *fmt,
				  FILE *out));
extern VOID PrintStat _ANSI_ARGS_((Context *context, FILE *out));
extern VOID PrintArrows _ANSI_ARGS_((ContextLattice *lattice, 
				   Context *context, FILE *out));
extern const char *PrintCheck _ANSI_ARGS_((char *fmt));
extern VOID PrintRelation _ANSI_ARGS_((FILE *f, char *fmt, 
				       Relation *rel, char **obj, 
				       char **atr));
