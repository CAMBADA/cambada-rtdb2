/****************************************************************
*@Program: xrtdb -> Cambada soccer robots configuration files	*
*		automatic generator				*
*@Authors: Tiago Nunes Mec 37161				*
*	   Milton Gregório Mec 36275				*
*	   David Ferreira Mec 36129				*
*@Year: 2007							*
*@Univ: Universidade de Aveiro					*
*@File: rtdb_user_creator.c					*
*@Description: rtdb_user.h Agent and Item definition file	*
*		automatic generation module			*
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
//#include <malloc.h>
#include "rtdb_user_creator.h"

/*
Function to write the rtdb_user.h file using the Global agents list
  and the Global Items list
*/
int printUserFile(rtdb_ItemList it, rtdb_AgentList ag)
{
	unsigned i;
	FILE *f;
	char* command;

	//Open the file rtdb_user.h for reading to check if it exists
	f= fopen(RTDB_USER_H, "r");
 	if (f != NULL)
	{
		//If it exists close it and ask the user permission to overwrite it
		fclose(f);
		char op = '\0';
		while ((op != 'y') && (op != 'n'))
		{
			printf("\nO ficheiro \e[33m%s\e[0m ja existe!\nDeseja substitui-lo? (y/n): ", RTDB_USER_H);;
			assert(scanf("%c", &op) == 1);
			//Clean stdin
			purge();
		}
		if (op == 'n')
		{
			return 1;
 		}
	}


	//Remove the rtdb_user.h file in case it exists, if we have permission from the user
//	command= strdup(RM_COMMAND);
	command= malloc ((2 + strlen(RM_COMMAND)+strlen(RTDB_USER_H)) * sizeof(char));
	sprintf(command, "%s %s", RM_COMMAND, RTDB_USER_H);
	assert(system(command) != -1);
	free(command);

	//If for some reason we can't create the file, abort
	if ((f= fopen(RTDB_USER_H, "w")) == NULL)
	{
		return 2;
	}

	//Write generic information to the file
	fprintf(f, "/* AUTOGEN FILE : rtdb_user.h */\n\n");
	fprintf(f, "#ifndef _CAMBADA_RTDB_USER_\n#define _CAMBADA_RTDB_USER_\n\n");
	fprintf(f, "/* agents section */\n\n");
 
	//Run through the agents list and write all the defines
	for(i= 0; i < ag.numAg; i++)
	{
		fprintf(f, "#define %s\t%d\n", ag.agents[i].id, ag.agents[i].num);
	}

	//Write the total number of agents
	fprintf(f, "\n#define N_AGENTS\t%d\n\n", ag.numAg);

	//Write a generic comment
	fprintf(f, "/* items section */\n\n");

	//Run through the item list and write all the defines
	for(i= 0; i < it.numIt; i++)
	{
		fprintf(f, "#define %s\t%d\n", it.items[i].id, it.items[i].num);
	}

	//Write the total number of items
	fprintf(f, "\n#define N_ITEMS\t%d\n\n", it.numIt);
	
	//Write generic information to the file
	fprintf(f, "#endif\n\n");
	fprintf(f, "/* EOF : rtdb_user.h */\n");

	//Close the rtdb_user.h file
	fclose(f);

	//Return 0, meaning all went smoothly
	return 0;	
}

/* EOF: rtdb_user_creator.c */
