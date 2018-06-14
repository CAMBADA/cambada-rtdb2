#ifndef __RTDBDEFS_H
#define __RTDBDEFS_H

// #define DEBUG

#define SELF 3333

#define CONFIG_FILE	"../config/rtdb.ini"

#define SHMEM_KEY 0x2000
#define SHMEM_SECOND_TEAM_KEY 0x3000

// definicoes hard-coded
// alterar de acordo com a utilizacao pretendida

#define MAX_AGENTS 8	// numero maximo de agentes
#define MAX_RECS 100	// numero maximo de 'variaveis' (shared + local)

// fim das definicoes hard-coded

typedef struct
{
	int id;				// identificador da 'variavel'
	int size;			// tamanho de dados
	int period;			// periodicidade de refrescamento via wireless
} RTDBconf_var;

#endif
