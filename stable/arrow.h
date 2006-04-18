/*
 * $Id: arrow.h,v 1.1.1.1 1998/02/23 15:22:27 lindig Exp $
 * 
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
 * Provides lattice operations as join and meet and determies if 
 * object and attributes are arrow related.
 */

extern VOID ArrowUpDown _ANSI_ARGS_ ((Context * context, 
				      ContextLattice * lattice, 
				      Relation * uprel, Relation * downrel));
