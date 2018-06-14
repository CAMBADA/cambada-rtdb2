/****************************************************************
*@Program: xrtdb -> Cambada soccer robots configuration files	*
*		automatic generator				*
*@Authors: Tiago Nunes Mec 37161				*
*	   Milton Gregório Mec 36275				*
*	   David Ferreira Mec 36129				*
*@Year: 2007							*
*@Univ: Universidade de Aveiro					*
*@File: rtdb_ini_creator.c 					*
*@Description: rtdb.ini DB initialization file generation	*
*		module						*
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <libgen.h>

#include "rtdb_ini_creator.h"
#include "rtdb_structs.h"
#include "rtdb_configuration.h"

/* 
Function to write, compile and execute a rtdb_sizeof_tmp.c file
   that has included the headerfile where the datatype is defined
   and write a rtdb_size.tmp with the size of the datatype.
After that, the rtdb_size.tmp file is opened, the size of the
   datatype is read and returned 
*/
int getSizeof(char* headerfl, char* datatype)
{
	int sizeofdata;
	char line[50], *command;
	FILE *f, *ftmp /* , *hfl */;

	//If the headerfile to be included doesn't exist, abort
#if 0
	if ((hfl= fopen(headerfl, "r")) == NULL)
        {
                printf("\nO ficheiro de cabeçalho \e[33m%s\e[0m onde o tipo \e[33m%s\e[0m está definido não existe! A abortar!\n",
                        headerfl, datatype);
		exit(1);
        }
	fclose(hfl);
#endif
	
	//Delete the rtdb_sizeof_tmp.c and the rtdb_size.tmp file just in case they exist
	command= malloc((1+strlen(RM_COMMAND)+strlen(" ./rtdb_sizeof_tmp.cpp ./rtdb_size.tmp"))*sizeof(char));
	sprintf(command, "%s ./rtdb_sizeof_tmp.cpp ./rtdb_size.tmp", RM_COMMAND);
	assert(system(command) != -1);
	free(command);
	
	//Create the rtdb_sizeof_tmp.cpp file for writing
	f=fopen("rtdb_sizeof_tmp.cpp", "w");
	
	//Write the contents of the file
	fprintf(f, "/* AUTOGEN File: rtdb_sizeof_tmp.cpp */\n\n");
	fprintf(f, "#include <stdio.h>\n");
	//Include the necessary headerfile in rtdb_sizeof_tmp.cpp
	fprintf(f, "#include \"%s\"\n\n", headerfl);
	fprintf(f, "#include \"common.h\"\n");
	fprintf(f, "using namespace cambada;\n");
	fprintf(f, "int main(void)\n{\n");
	fprintf(f, "\tFILE* f;\n");
	//Create the rtdb_size.tmp for writing
	fprintf(f, "\tf= fopen(\"rtdb_size.tmp\", \"w\");\n");
	//Write the size of the datatype in the rtdb_size.tmp file
	fprintf(f, "\tfprintf(f, \"%%lu\\n\", sizeof(%s%s));\n", STRUCTPREFIX, datatype);
	fprintf(f, "\tfclose(f);\n");
	fprintf(f, "\n\treturn 0;\n}\n");
	fprintf(f, "\n/* EOF: rtdb_sizeof_tmp.cpp */\n");

	//Close the rtdb_sizeof_tmp.cpp file
	fclose(f);

	//If in Debug Sizeof mode, pause and ask the user what to do
	if (DEBUGSIZEOF)
	{
		char op = '\0';
		while(op != 'c')
		{
			printf("\n\e[33mFicheiro\e[0m \e[32mrtdb_sizeof_tmp.c\e[0m \e[33mescrito.\e[0m\n"
                    "Insira\n"
                    "\"\e[32mv\e[0m\" para ver o ficheiro gerado,\n"
                    "\"\e[32mc\e[0m\" para continuar e compilar ou\n"
                    "\"\e[32ma\e[0m\" para abortar: ");
			assert(scanf("%c", &op) == 1);
			purge(); //Clean stdin

			switch (op)
			{
				//If the user wants to see the rtdb_sizeof_tmp.c file, show it
				case 'v':	{
						printf("\n\e[32mA visualizar o ficheiro \e[33mrtdb_sizeof_tmp.c\e[0m\n");
						command= malloc((1+strlen(TXTVIEWER)+strlen(" rtdb_sizeof_tmp.c")) * sizeof(char));
						sprintf(command, "%s rtdb_sizeof_tmp.c", TXTVIEWER);
						assert(system(command) != -1);
						free(command);
						printf("\e[32mFicheiro \e[33mrtdb_sizeof_tmp.c \e[32mvisto!\e[0m\n");
						break;
						}
				//If the user wants to continue, break
				case 'c':	break;
				//If the user wants to abort, exit the program
				case 'a':	{
						command= malloc((1+strlen(RM_COMMAND)+strlen(" ./rtdb_sizeof_tmp.c")) * sizeof(char));
						sprintf(command, "%s ./rtdb_sizeof_tmp.c", RM_COMMAND);
						assert(system(command) != -1);
						free(command);
						printf("\n\e[33mAbortado pelo utilizador!\e[0m\n");
						exit(1);
						}
				//If not a correct option, notify the user
				default:	printf("\n\e[33mOpção inválida!\e[0m\n");
			}
		}
	}

	//Compile the generated rtdb_sizeof_tmp.c file
	command= malloc((strlen(CC)+1+strlen(CFLAGS)+strlen(" -o rtdb_sizeof_tmp rtdb_sizeof_tmp.cpp")+1)*sizeof(char));
	sprintf(command, "%s %s -o rtdb_sizeof_tmp rtdb_sizeof_tmp.cpp", CC, CFLAGS);
	//printf("CMD: %s\n", command); fflush(stdout);
	assert(system(command) != -1);
	free(command);

	//Execute the rtdb_sizeof_tmp file just compiled
	assert(system("./rtdb_sizeof_tmp") != -1);

	//Remove the rtdb_sizeof_tmp executable
	command= malloc((1+strlen(RM_COMMAND)+strlen(" ./rtdb_sizeof_tmp"))*sizeof(char));
	sprintf(command, "%s ./rtdb_sizeof_tmp", RM_COMMAND);
	assert(system(command) != -1);
	free(command);

	//If in Debug Sizeof mode, pause and ask the user what to do
	if (DEBUGSIZEOF)
	{
		char op = '\0';
		while(op != 'c')
		{
			printf("\n\e[33mFicheiro\e[0m \e[32mrtdb_size.tmp\e[0m \e[33mescrito.\e[0m\n"
                    "Insira\n"
                    "\"\e[32mv\e[0m\" para ver o ficheiro gerado,\n"
                    "\"\e[32mc\e[0m\" para continuar e compilar ou\n"
                    "\"\e[32ma\e[0m\" para abortar: ");
			assert(scanf("%c", &op) == 1);
			purge(); //Clean stdin

			switch (op)
			{
				//If the user wants to see the rtdb_size.tmp file, show it
				case 'v':	{
						printf("\n\e[32mA visualizar o ficheiro \e[33mrtdb_size.tmp\e[0m\n");
						command= malloc((1+strlen(TXTVIEWER)+strlen(" rtdb_size.tmp")) * sizeof(char));
						sprintf(command, "%s rtdb_size.tmp", TXTVIEWER);
						assert(system(command) != -1);
						free(command);	
						printf("\e[32mFicheiro \e[33mrtdb_size.tmp \e[32mvisto!\e[0m\n");		
						break;
						}
				//If the user wants to continue, break
				case 'c':	break;
				//If the user wants to abort, exit the program
				case 'a':	{
						command= malloc((1+strlen(RM_COMMAND)+strlen(" ./rtdb_size.tmp")) * sizeof(char));
						sprintf(command, "%s ./rtdb_size.tmp", RM_COMMAND);
						assert(system(command) != -1);
						free(command);
						printf("\n\e[33mAbortado pelo utilizador!\e[0m\n");
						exit(1);
						}
				//If not a correct option, notify the user
				default:	printf("\n\e[33mOpção inválida!\e[0m\n");
			}
		}
	}
	
	//Open the rtdb_size.tmp file for reading
	ftmp= fopen("rtdb_size.tmp", "r");

	//Read the only line that should be in the rtdb_size.tmp file to the var line
	assert(fgets(line, 50, ftmp) != NULL);

	//Define sizeofdata with the integer just read to line
	sscanf(line, "%d", &sizeofdata);

	//Close the rtdb_size.tmp file
	fclose(ftmp);

	//Delete the rtdb_sizeof_tmp.c temporary file
	command= malloc((1+strlen(RM_COMMAND)+strlen(" ./rtdb_sizeof_tmp.cpp"))*sizeof(char));
	sprintf(command, "%s ./rtdb_sizeof_tmp.cpp", RM_COMMAND);
	assert(system(command) != -1);
	free(command);
	
	//Delete the rtdb_size.tmp temporary file
	command= malloc((1+strlen(RM_COMMAND)+strlen(" ./rtdb_size.tmp"))*sizeof(char));
	sprintf(command, "%s ./rtdb_size.tmp", RM_COMMAND);
	assert(system(command) != -1);
	free(command);

	//If in Debug Sizeof mode ask the user to continue or abort the program
	if (DEBUGSIZEOF)
	{
		char op = '\0';
		printf("\n\n\e[33mTodos os ficheiros temporários lidos e apagados.\n"
                "Prima \"\e[32ma\e[33m\" para abortar ou\n"
                "qualquer outra tecla para sair da função e prosseguir com o programa:\e[0m ");
		assert(scanf("%c", &op) == 1);
		purge(); //Clean stdin

		if (op == 'a')
		{
			printf("\n\e[33mAbortado pelo utilizador!\e[0m\n");
			exit(1);
		}
	}
	
	//Return the size of the datatype received
	return sizeofdata;
}

struct ItemsMap {
    char * value;
    unsigned id;
    unsigned item_shared;
};

void analyzeAssignmentList(struct ItemsMap* map, unsigned* current_items, rtdb_Item* item, unsigned shared) {
    unsigned item_mismatch = 0;
    unsigned item_found = 0;
    unsigned map_idx;
    for (map_idx = 0; map_idx < *current_items; map_idx++) {
        if (map[map_idx].id == item->num) {
            if (map[map_idx].item_shared == 0)
                item_mismatch = 1;
            item_found = 1;
            break;
        }
    }

    if (!item_found) {
        map[*current_items].value = strdup(item->id);
        map[*current_items].id = item->num;
        map[*current_items].item_shared = shared;
        *current_items = *current_items + 1;
    } else if (item_mismatch) {
        printf("Chave %s está marcada como LOCAL para alguns agents e SHARED para outros\n",
               item->id);

        char op = '\0';
        while ((op != 'y') && (op != 'n')) {
            printf("Quer marcar a chave como SHARED? (y para shared, n para local): ");
            assert(scanf("%c", &op) == 1);
            purge();
        }

        if (op == 'n') {
            map[map_idx].item_shared = 0;
        } else {
            map[map_idx].item_shared = 1;
        }
    }
}

int compItemMap(const void * a, const void * b) {
    return strcmp(((struct ItemsMap*) a)->value, ((struct ItemsMap*) b)->value);
}

int printRtDB2File(rtdb_AgentList agL, rtdb_AssignmentList asL) {
        (void) agL;
	char* command;
	FILE *f;

	/* construct path to RTDB_INI file */
	char rtdbIniPath[256];
	sprintf(rtdbIniPath, "%s", RTDB2_CONF);

	//Open the file rtdb.ini for reading to check if it exists
	f= fopen(rtdbIniPath, "r");
	if(f != NULL)
	{
		//If it exists close it and ask the user permission to overwrite it
		fclose(f);
		char op = '\0';
		while ((op != 'y') && (op != 'n'))
		{
			printf("\nO ficheiro \e[33mrtdb2_configuration.xml\e[0m ja existe!\nDeseja substitui-lo? (y/n): ");
			assert(scanf("%c", &op) == 1);
			//Clean stdin
			purge();
		}

		if (op == 'n')
		{
			return 1;
		}
	}

	//Remove the rtdb.ini file in case it exists, if we have permission from the user
	command= malloc((2+strlen(RM_COMMAND)+strlen(rtdbIniPath)) * sizeof(char));
	sprintf(command, "%s %s", RM_COMMAND, rtdbIniPath);
	assert(system(command) != -1);
	free(command);

	//If for some reason we can't create the file, abort
	if ((f= fopen(rtdbIniPath, "w")) == NULL)
	{
		return 2;
	}

	unsigned i, j;

	fprintf(f, "<!-- This file can be generated using xrtdb in order to \n");
    fprintf(f, "     exist backward compatibility between RtDB and RtDB2 -->\n");
    fprintf(f, "<!-- Every key might have the following parameters:\n");
    fprintf(f, "     * id that stands for a string identifier;\n");
    fprintf(f, "     * shared that indicates if the object is shared or local to\n");
    fprintf(f, "       real-time database;\n");
    fprintf(f, "     * oid (optional) corresponds to an older identifier in the\n");
    fprintf(f, "       RtDB older version (this field should disappear after a \n");
    fprintf(f, "       complete upgrade.\n");
    fprintf(f, "     * period (optional) and phase (optional) in order to schedule\n");
    fprintf(f, "       how an item is shared by the Communication Manager -->\n\n");
    fprintf(f, "<RtDB2Configuration>\n");
    fprintf(f, "  <General>\n");
    fprintf(f, "    <DefaultKeyValue shared=\"true\" period=\"1\" phase=\"0\"/>\n");
	fprintf(f, "    <Compressor name=\"zstd\" dictionary=\"true\"/>\n");
    fprintf(f, "  </General>\n");

    fprintf(f, "  <Keys>\n");

    unsigned map_idx;
    struct ItemsMap map[1000];

    unsigned current_items = 0;
    for (i = 0; i < asL.numAs; i++) {
        for (j = 0; j < asL.asList[i].schema->sharedItems.numIt; j++) {
            analyzeAssignmentList(map, &current_items, &(asL.asList[i].schema->sharedItems.items[j]), 1);
        }

        for (j = 0; j < asL.asList[i].schema->localItems.numIt; j++) {
            analyzeAssignmentList(map, &current_items, &(asL.asList[i].schema->localItems.items[j]), 0);
        }
    }

    qsort(map, current_items, sizeof(struct ItemsMap), compItemMap);
    for (map_idx = 0; map_idx < current_items; map_idx++) {
        fprintf(f, "    <key id=\"%s\" ", map[map_idx].value);
        if (!map[map_idx].item_shared)
            fprintf(f, "shared=\"false\" ");
        fprintf(f, "oid=\"%d\"/>\n", map[map_idx].id);
    }

    fprintf(f, "  </Keys>\n");
    fprintf(f, "</RtDB2Configuration>\n");

	fprintf(stderr,"DONE!\n");

	fclose(f);

	//Return 0, meaning all went smoothly
	return 0;
}

/* EOF: rtdb_ini_creator.c */
