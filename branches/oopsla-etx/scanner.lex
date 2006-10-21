
%{	

/*
 * $Id: scanner.lex,v 1.1.1.1 1998/02/23 15:22:25 lindig Exp $
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

#if STDC_HEADERS || defined(HAVE_STRING_H)
#  include <string.h>
#else
#  ifndef HAVE_MEMCPY
#    define memcpy(d, s, n) bcopy ((s), (d), (n))
#    define memmove(d, s, n) bcopy ((s), (d), (n))
#  endif
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
#include "parser.h"

#ifndef HAVE_STRDUP
#  include "lib.h"
#endif


/* 
 * current line number 
 * number of errors
 */

int line = 1;
int errors = 0 ;

%}

/* white space and newline */

WS    		[ \t\f\r]
NL    		\n

/* identifier */

DIGIT 		[0-9]
LETTER		[a-zA-Z]
MISC		[\$\-\_\.\&\|()!\[\]\~\\\<\>\,\?]
ID		({LETTER}|{DIGIT}|{MISC})({LETTER}|{DIGIT}|{MISC})*

/* ID		{LETTER}({LETTER}|{DIGIT}|{MISC})* */

/* special character */

SEMICOLON	";"
COLON		":"
COMMENT		"%"

%%

{COMMENT}.*	; /* skip - eat up to end of line */


{COLON}		{ return COLON ; }
{SEMICOLON}	{ return SEMICOLON ; }
{NL} 		{ line++ ;}
{ID}		{ yylval.ident = strdup(yytext) ; 
		  if (!yylval.ident) 
			panic ("strdup failed in %s:%i",__FILE__,__LINE__);
		  return IDENT; 
                }
{WS}+		; /* skip */

.		{ yylval.chr = *yytext ; return UNKNOWN;}






