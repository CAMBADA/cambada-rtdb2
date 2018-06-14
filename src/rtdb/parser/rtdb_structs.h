/****************************************************************
*@Program: xrtdb -> Cambada soccer robots configuration files	*
*		automatic generator				*
*@Authors: Tiago Nunes Mec 37161				*
*	   Milton Gregório Mec 36275				*
*	   David Ferreira Mec 36129				*
*@Year: 2007							*
*@Univ: Universidade de Aveiro					*
*@File: rtdb_structs.h 						*
*@Description: Data types definition for agents, items, schemas	*
*		and assignments storage				*
*****************************************************************/
#ifndef _RTDB_STRUCTS_H
#define _RTDB_STRUCTS_H

/* Structure to store an agent */
typedef struct
{
	unsigned num;	// Unique agent identification number (0-n)
	char* id;	// Agent name (alfanum)
} rtdb_Agent;

/* Structure to store a list of agents */
typedef struct
{
	rtdb_Agent* agents;	// Dynamic agent list 
	unsigned numAg; 	// Total number of agents in list
} rtdb_AgentList;

/* Structure to store an item */
typedef struct
{
	unsigned num;		// Unique item identification number (0-n)
	char* id;		// Item name (alfanum)
	char* datatype;		// C datatype identifier (alfanum)
	char* headerfile;	// C headerfile name where the datatype is declared (alfanum)
	unsigned period;	// Broadcasting period of DB item (1-4)
} rtdb_Item;

/* Structure to store an items list */
typedef struct
{
	rtdb_Item* items;	// Dynamic item list
	unsigned numIt;		// Total number os items in list
} rtdb_ItemList;

/* Structure to store a schema */
typedef struct
{
	char* id;			// Schema name (alfanum)
	rtdb_ItemList sharedItems;	// Shared items list in schema
	rtdb_ItemList localItems;	// Local items list in schema
} rtdb_Schema;

/* Structure to store a schemas list */
typedef struct
{
	rtdb_Schema* schemas;		// List of Schemas
	unsigned numSc;			// Total number of schemas in list
} rtdb_SchemaList;

/* Structure to store an assignment of items to agents */
typedef struct
{
	rtdb_Schema* schema;		// Schema to be assigned
	rtdb_AgentList agentList;	// Agents to use the schema
} rtdb_Assignment;

/* List of assignments of items to agents */
typedef struct 
{
	rtdb_Assignment* asList;	// List of assignments
	unsigned numAs;			// Total number os assignments in list
} rtdb_AssignmentList;


#endif

/* EOF: rtdb_structs.h */
