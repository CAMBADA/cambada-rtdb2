/****************************************************************
*@Program: xrtdb -> Cambada soccer robots configuration files	*
*		automatic generator				*
*@Authors: Tiago Nunes Mec 37161				*
*	   Milton Gregório Mec 36275				*
*	   David Ferreira Mec 36129				*
*@Year: 2007							*
*@Univ: Universidade de Aveiro					*
*@File: xrtdb.lex 						*
*@Description: Lexical analizer module    			*
*****************************************************************/
%{
#include <string.h>
#include "rtdb_errors.h"
#include "xrtdb.tab.h"
#define YY_DECL \
        int yylex(char **p)

//Variable to track the line number for error messages
unsigned lineNumber= 1;
%}

%option noyywrap never-interactive pointer nounput noinput

%x COMMENT

id [_a-zA-Z][_a-zA-Z0-9]*
headerfl {id}\.h
blanks [ \t]*
digit [0-9]
number {digit}+

%%
<INITIAL>{
	{blanks}        /* Ignore white space */
        \n	        { lineNumber++; return eol; }
	#	        { BEGIN(COMMENT); }
        =               { return equal; }
        ;               { return semicomma; }
        ,               { return comma; }
        \{              { return openbrace; }
        \}              { return closebrace; }
	AGENTS	        { return agentsDECL; }
        ITEM            { return itemDECL; }
	SCHEMA	        { return schemaDECL; }
	ASSIGNMENT	{ return assignmentDECL; }
        datatype        { return datatypeFIELD; }
        period          { return periodFIELD; }
        headerfile      { return headerfileFIELD; }
        shared          { return sharedFIELD; }
        local           { return localFIELD; }
        schema          { return schemaFIELD; }
        agents          { return agentsFIELD; }
        {id}            { *p= strdup(yytext); return identifier; }
        {headerfl}      { *p= strdup(yytext); return headerfl; }
        {number}        { *p= strdup(yytext); return integer; }
	.		{ raiseError(lineNumber, _ERR_LEX_); }
}
<COMMENT>{
	\n	{ lineNumber++; BEGIN(INITIAL); return eol; }
	.	/* Ignoring all chars except \n */
}

<*><<EOF>>	{ return eof; }
%%

/* EOF: xrtdb.lex */
