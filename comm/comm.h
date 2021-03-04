/*
 * comm.h
 *
 *  Created on: Apr 26, 2016
 *      Author: ricardodias
 */

#ifndef COMM_COMM_H_
#define COMM_COMM_H_

#include "../rtdb/rtdbdefs.h"
#define BUFFER_SIZE 10000

#define TTUP_US_DEFAULT 20E3 // 10Hz -> 100E3, 20Hz -> 50E3
#define COMM_DELAY_MS 2
#define COMM_DELAY_US COMM_DELAY_MS*1E3
#define MIN_UPDATE_DELAY_US 1E3

// #define DEBUG
// #define FILEDEBUG
#define UNSYNC

#define PERRNO(txt) \
	printf("ERROR: (%s / %s): " txt ": %s\n", __FILE__, __FUNCTION__, strerror(errno))

#define PERR(txt, par...) \
	printf("ERROR: (%s / %s): " txt "\n", __FILE__, __FUNCTION__, ## par)

#ifdef DEBUG
#define PDEBUG(txt, par...) \
	printf("DEBUG: (%s / %s): " txt "\n", __FILE__, __FUNCTION__, ## par)
#else
#define PDEBUG(txt, par...)
#endif

#ifdef FILEDEBUG
	FILE *filedebug;
#define FDEBUG(txt, par...) \
  fprintf(filedebug, txt , ## par)
#else
#define FDEBUG(file, txt, par...)
#endif


#define NOT_RUNNING		0
#define RUNNING			1
#define INSERT			2
#define REMOVE			3
#define MAX_REMOVE_TICKS		10

#define NO	0
#define YES	1


struct _record
{
	int id;			  // id
	int size;		  // data size
	int life;		  // life time
	void* pData;	// pointer to data
};

struct _frameHeader
{
	unsigned char number;			    // agent number
	unsigned int counter;			    // frame counter
	char stateTable[MAX_AGENTS];	// table with my vision of each agent state
	int recordsSize;
};

struct _agent
{
	char state;							          // current state
	char dynamicID;					          // position in frame
	char received;						        // received from agent in the last Ttup?
	struct timeval receiveTimeStamp;	// last receive time stamp
	int delta;							          // delta
	unsigned int lastFrameCounter;		// frame number
	char stateTable[MAX_AGENTS];		  // vision of agents state
  int removeCounter;                // counter to move agent to not_running state
};

typedef struct optionStruct_tag
{
	bool 	nosend;
	char* 	iface;
	char* 	network_name;
	bool 	compressData;
        bool    packet_record;
} optionStruct_t;




#include <iostream>
#include <sys/time.h>

#define MAX_TIMERS 10

class Timer3
{
    public:
        Timer3();
        void calcRate();
        void startTimer(int);
        double endTimer(int);
        double averageTime(int);
        virtual ~Timer3();
    protected:
        int loops;
        struct timeval start, end;
        struct timeval t1[MAX_TIMERS];
        struct timeval t2[MAX_TIMERS];
        double time_avg[MAX_TIMERS];
        int    time_cnt[MAX_TIMERS];
    private:
};


#endif /* COMM_COMM_H_ */
