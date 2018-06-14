/****************************************************************
*@Program: xrtdb -> Cambada soccer robots configuration files	*
*		automatic generator				*
*@Authors: Tiago Nunes Mec 37161				*
*	   Milton Gregório Mec 36275				*
*	   David Ferreira Mec 36129				*
*@Year: 2007							*
*@Univ: Universidade de Aveiro					*
*@File: rtdb_functions.h					*
*@Description: Module with the necessary functions to create	*
*		and manipulate the datatypes defined in		*
*		rtdb_structs.h					*
*****************************************************************/
#ifndef _RTDB_FUNCTIONS_H_
#define _RTDB_FUNCTIONS_H_

#include "rtdb_structs.h"
#include "rtdb_configuration.h"

// Global Lists to store all read data from the input file
rtdb_AgentList agList;
rtdb_ItemList itList;
rtdb_SchemaList schemaList;
rtdb_AssignmentList assignList;


/**************  Agents manipulation Function  **************/

//Add an agent to the agents global list if it wasn't previouly declared
void agentCreate(char* );

/**************  Items manipulation Functions  **************/

//Add an item to the items global list if it wasn't previouly declared
rtdb_Item* itemCreate(char* );
//Define a datatype in the item specified if it wasn't previouly defined
void itemAddDatatype(rtdb_Item* , char* );
//Define a period in the item specified if it wasn't previouly defined
void itemAddPeriod(rtdb_Item* , char* );
//Define an headerfile in the item specified if it wasn't previouly defined
void itemAddHeaderfile(rtdb_Item* , char* );
//Check if the item is well defined, all fields non specified, with default fields, are defined here
void itemVerify(rtdb_Item* );

/**************  Schemas manipulation Functions  **************/

//Add a schema to the schemas global list (if it was previouly declared, return a pointer to it)
rtdb_Schema* schemaCreate(char* );
//Add an item to the shared items list in the schema specified if it wasn't previouly defined
void schemaAddSharedItem(rtdb_Schema* , char* );
//Add an item to the local items list in the schema specified if it wasn't previouly defined
void schemaAddLocalItem(rtdb_Schema* , char* );
//Check if the schema is well defined (if both items lists aren't empty)
void schemaVerify(rtdb_Schema* );

/**************  Assignments manipulation Functions  **************/

//Add an assignment to the assignments global list
rtdb_Assignment* assignmentCreate(void);
//Define a schema in the assignment specified if it wasn't previouly defined
void assignmentAddSchema(rtdb_Assignment* , char* );
//Add an agent to the items list of the assignment specified if it wasn't previouly defined
void assignmentAddAgent(rtdb_Assignment* , char* );
//Check if the assignment is well defined (if the agents list isn't empty and if the schema is defined)
void assignmentVerify(rtdb_Assignment* );

#endif

/* EOF: rtdb_functions.h */
