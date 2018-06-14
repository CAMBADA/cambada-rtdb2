/****************************************************************
*@Program: xrtdb -> Cambada soccer robots configuration files	*
*		automatic generator				*
*@Authors: Tiago Nunes Mec 37161				*
*	   Milton Gregório Mec 36275				*
*	   David Ferreira Mec 36129				*
*@Year: 2007							*
*@Univ: Universidade de Aveiro					*
*@File: rtdb_errors.c						*
*@Description: Error handling module with the definition of	*
*		error handling functions			*
*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "rtdb_errors.h"

/* Parsing Errors handling function */
void raiseError (unsigned numln, const char* msg)
{
	/* Output to the standard error stream (normally stdout) a message with an error message
	informing the error line number and an error description message */ 
	fprintf(stderr, "\n\e[33mErro\e[0m no ficheiro de configuração na \e[33mlinha %u\e[0m: \n\e[32m%s\e[0m\n", numln, msg);
	/* Abort the program execution */
	exit(2);
}

/* Structure definitions Errors handling function */
void abortOnError(char* errMsg)
{
	/* Output to the standard error stream (normally stdout) a message with an error message */
	fprintf(stderr, "\n\e[33mA abortar! Erro\e[0m: \e[32m%s\e[0m\n", errMsg);
	/* Abort the program execution */
	exit(3);
}

/* EOF: rtdb_errors.c */
