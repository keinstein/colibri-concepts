
/*
 * $Id: lib.c,v 1.1.1.1 1998/02/23 15:22:27 lindig Exp $
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
 * Define missing library functions.
 */

#include "config.h"
#if defined(STDC_HEADERS) || defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#endif
#ifdef DBMALLOC
#  include <malloc.h>
#endif

#include "defines.h"
#include "lib.h"

#ifndef HAVE_STRDUP
/*
 * strdup
 * mallocs memory for a string, copies a given string to the
 * newly allocated memory and returns a pointer to it. Returns 
 * NULL is malloc fails
 */

char *
strdup (string)
	char *string;

{
	char *result = (char*)malloc(strlen(string) + 1);
	if (result == (char*)0)
		return (char*)0;
	strcpy(result, string);
	return result;
}

#endif
