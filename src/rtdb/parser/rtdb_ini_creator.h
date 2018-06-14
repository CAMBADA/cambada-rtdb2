/****************************************************************
*@Program: xrtdb -> Cambada soccer robots configuration files	*
*		automatic generator				*
*@Authors: Tiago Nunes Mec 37161				*
*	   Milton Gregório Mec 36275				*
*	   David Ferreira Mec 36129				*
*@Year: 2007							*
*@Univ: Universidade de Aveiro					*
*@File: rtdb_ini_creator.h 					*
*@Description: rtdb.ini DB initialization file generation	*
*		module headerfile with function	prototypes	*
*		definition					*
*****************************************************************/
#ifndef _RTDB_INI_CREATOR_H_
#define _RTDB_USER_CREATOR_H_

#include "rtdb_structs.h"
#include "rtdb_configuration.h"

/* 
Function to write, compile and execute a rtdb_sizeof_tmp.c file
   that has included the headerfile where the datatype is defined
   and write a rtdb_size.tmp with the size of the datatype.
After that, the rtdb_size.tmp file is opened, the size of the
   datatype is read and returned 
*/
int getSizeof(char*, char*);

/*
Function to read the Global list of agents and the Global list
   of assignments and automatically generate the file for the
   RtDB2 temporarily.
 */
int printRtDB2File(rtdb_AgentList, rtdb_AssignmentList);

#endif

/* EOF: rtdb_ini_creator.h */
