
/*
 * $Id: panic.c,v 1.1.1.1 1998/02/23 15:22:26 lindig Exp $
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
 */

#include "config.h"
#include <stdio.h>
#if defined(STDC_HEADERS) || defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#endif

#include "defines.h"
#include "panic.h"


/* requires: exit fflush fprintf */

/*
 * in panic.h panic is declared as panic () so that arguments are not
 * checked. This is a dirty trick but makes it easy to use a single
 * fprintf-call to do all the formatting. Otherwise we had to fiddle
 * with va_start, va_list, va_end and va_arg macros for handling
 * variable number of arguments.  */

VOID
panic(format,arg1,arg2,arg3,arg4,arg5,arg6)

	char *format;		/* fprintf format string */
	char *arg1,*arg2,*arg3;
	char *arg4,*arg5,*arg6;

{
	fprintf (stderr,format,arg1, arg2, arg3, arg4, arg5, arg6);
	fflush(stderr);
	exit (1);
}
