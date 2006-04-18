
%{
/* 
 *
 * $Id: parser.yacc,v 1.1.1.1 1998/02/23 15:22:25 lindig Exp $
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
#define YYDEBUG 1			/* include debugging code */

#include <stdio.h>

#include "defines.h"
#include "list.h"
#include "set.h"
#include "hash.h"
#include "relation.h"
#include "concept.h"
#include "context.h"
#include "input.h"

/* prototypes */

VOID yyerror _ANSI_ARGS_((char *str));  /* prototype for yyerror below */
extern int yylex _ANSI_ARGS_((void));   /* the lexer */

extern int line;			/* from scanner */
extern Input *maininput;            	/* from main */ 
%}


/*
 * token
 */

%union {
	char *chr ;		/* strings from scanner */
	char *ident ;		/* character from scanner */
	Input *tree;		/* parse tree - list of objects */
	InputObj *obj;		/* object with attributes */
	List *atrseq;		/* attr sequence */
} ;

%token <ident> IDENT 
%token <char> UNKNOWN 

%token COLON 
%token SEMICOLON

%type <tree> context
%type <obj> object
%type <atrseq> seqAttribute
%type <tree> start

%%

start:			context
			{
				maininput = $1 ;
			}

context:		object 
			{ 
				Input *tree;
	
				tree = InputNew();
				InputAddObj(tree,$1);
				$$ = tree;
			}
		|	context object
			{
				InputAddObj($1,$2);
				$$ = $1 ;
			}

object: 		IDENT COLON seqAttribute SEMICOLON
			{
				$$ = InputObjNew ($1,$3);	
			}

object: 		IDENT COLON SEMICOLON
			{
				$$ = InputObjNew ($1,ListNew());	
			}

seqAttribute :		IDENT 
			{
				List *list ;
				ListEntry *entry ;
				
				list = ListNew();
				entry = ListNewEntry(list);
				ListSetValue(entry,$1);
				$$ = list ;
			}
				
		|       seqAttribute IDENT
			{
				ListEntry *entry = ListNewEntry($1);
				ListSetValue(entry,$2);
				$$ = $1;
			}
			
		|	seqAttribute error 
			{
				$$ = $1;
			}
%%

/*
 * yyerror
 *
 * report parsing error to stdout */

VOID
yyerror (str) 
	char *str ;

{
        fprintf (stderr, "error in line %i: %s\n",line, str);
}
