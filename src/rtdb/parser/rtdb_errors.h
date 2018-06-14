/****************************************************************
*@Program: xrtdb -> Cambada soccer robots configuration files	*
*		automatic generator				*
*@Authors: Tiago Nunes Mec 37161				*
*	   Milton Gregório Mec 36275				*
*	   David Ferreira Mec 36129				*
*@Year: 2007							*
*@Univ: Universidade de Aveiro					*
*@File: rtdb_errors.h						*
*@Description: Error handling module with defition of error	*
*		messages and error handling functions		*
*****************************************************************/
#ifndef _RTDB_ERRORS_H_
#define _RTDB_ERRORS_H_

/* Parsing Error messages definition */
#define _ERR_LEX_ "Erro Lexical! Sequência de caracteres inválida!"
#define _ERR_INITIAL_ "À espera de uma declaração de um tipo válido!"
#define _ERR_AGENTS_ "Agentes mal declarados! À espera de uma lista de agntes válida!"
#define _ERR_ITEMOPEN_ "Item mal declarado! À espera de \"{\""
#define _ERR_ITEMFIELD_ "Item mal declarado! À espera de \"datatype =\" id, \"period =\" num, \"headerfile =\" ficheiro.h ou \"}\""
#define _ERR_ITEMAFTERFIELD_ "Item mal declarado! À espera de \";\", fim de linha ou \"}\""
#define _ERR_SCHEMAOPEN_ "Esquema mal declarado! À espera de \"{\""
#define _ERR_SCHEMAFIELD_ "Esquema mal declarado! À espera de \"shared =\" ListaItems, \"local =\" ListaItems ou \"}\""
#define _ERR_ITEMSLIST_ "Esquema mal declarado! À espera de uma lista de items válida!"
#define _ERR_ASSIGNMENTOPEN_ "Atribuição mal declarada! À espera de \"{\""
#define _ERR_ASSIGNMENT_ "Atribuição mal declarada! À espera de \"schema =\", \"agents =\" ou \"}\""
#define _ERR_AGENTSLIST_ "Atribuição mal declarada! À espera de uma lista de agentes válida!"


/* Parsing Errors handling function */
void raiseError (unsigned, const char*);

/* Structure definitions Errors handling function */
void abortOnError(char*);

#endif

/* EOF: rtdb_errors.h */
