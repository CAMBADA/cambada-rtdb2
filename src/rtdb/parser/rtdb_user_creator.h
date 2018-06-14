/****************************************************************
*@Program: xrtdb -> Cambada soccer robots configuration files	*
*		automatic generator				*
*@Authors: Tiago Nunes Mec 37161				*
*	   Milton Gregório Mec 36275				*
*	   David Ferreira Mec 36129				*
*@Year: 2007							*
*@Univ: Universidade de Aveiro					*
*@File: rtdb_user_creator.h					*
*@Description: rtdb_user.h Agent and Item definition file	*
*		automatic generation module function prototype	*
*		definition					*
*****************************************************************/
#ifndef _RTDB_USER_CREATOR_H_
#define _RTDB_USER_CREATOR_H_

#include "rtdb_structs.h"
#include "rtdb_configuration.h"

/*
Function to write the rtdb_user.h file using the Global agents list
  and the Global Items list
*/
int printUserFile(rtdb_ItemList, rtdb_AgentList);

#endif

/* EOF: rtdb_user_creator.h */
