/*
 * Frederico Miguel Santos - frederico.miguel.santos@gmail.com
 * CAMBADA robotic soccer team - www.ieeta.pt/atri/cambada
 * University of Aveiro
 * Copyright (C) 2009
 *
 * This file is part of RTDB middleware.
 *
 * RTDB middleware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RTDB middleware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RTDB middleware.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <fstream>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>
#include <boost/filesystem/operations.hpp>
#include <sstream>


#define PACKET_WRITE_FILE
#define NUM_PACKETS_WRITE   150000
#include "../rtdb2/RtDB2.h"
#include "../rtdb/rtdb2_adapter.h"

#include "multicast.h"
#include "NetworkConfig.h"
#include "comm.h"

int TTUP_US = TTUP_US_DEFAULT;

int end;
int timer;
int MAX_DELTA;

struct timeval lastSendTimeStamp;
int delay;
int nosend;

int lostPackets[MAX_AGENTS];
int myNumber;
struct _agent agent[MAX_AGENTS];
int RUNNING_AGENTS;

typedef struct threadParameters
{
    optionStruct_t* options;
    multiSocket_t* socket;
} threadParams_t;

//  Signal catch
static void signal_catch(int sig) {
    if (sig == SIGINT) {
        end++;
        if (end == 3)
            abort();
    } else if (sig == SIGALRM)
        timer++;
}

// RA-TDMA
int sync_ratdma(int agentNumber) {
    int realDiff, expectedDiff;
    struct itimerval it;

    agent[agentNumber].received = YES;

    if ((agent[agentNumber].state == NOT_RUNNING) || (agent[agentNumber].state == INSERT)) {
        PDEBUG("*****  agent %d - NOT_RUNNING or INSERT  *****", agentNumber);
        return (1);
    }

    // real difference with average medium comm delay
    realDiff = (int) ((agent[agentNumber].receiveTimeStamp.tv_sec - lastSendTimeStamp.tv_sec) * 1E6 +
                      agent[agentNumber].receiveTimeStamp.tv_usec - lastSendTimeStamp.tv_usec);
    realDiff -= (int) COMM_DELAY_US; // travel time
    if (realDiff < 0) {
        PDEBUG("*****  realDiff to agent %d = %d  *****", agentNumber, realDiff);
        return (2);
    }

    // expected difference
    expectedDiff = (int) ((agent[agentNumber].dynamicID - agent[myNumber].dynamicID) * TTUP_US / RUNNING_AGENTS);
    if (expectedDiff < 0)
        expectedDiff += (int) TTUP_US;

    agent[agentNumber].delta = realDiff - expectedDiff;

    // only dynamic agent 0 make adjustments
    if (agent[myNumber].dynamicID == 0) {
        if ((agent[agentNumber].delta > delay) && (agent[agentNumber].delta < MAX_DELTA)) {
            // avoid small corrections
            if (agent[agentNumber].delta > (int) MIN_UPDATE_DELAY_US) {
                delay = agent[agentNumber].delta;
                PDEBUG("delay between %d(%d) and %d(%d) -> %d", myNumber, agent[myNumber].dynamicID, agentNumber,
                       agent[agentNumber].dynamicID, delay);
            }
        }
    } else {
        // only sync from dynamic agent 0
        if (agent[agentNumber].dynamicID == 0) {
            expectedDiff = (int) (TTUP_US - expectedDiff);
            expectedDiff -= (int) COMM_DELAY_US; // travel time
            it.it_value.tv_usec = (long int) (expectedDiff % (int) 1E6);
            it.it_value.tv_sec = (long int) (expectedDiff / (int) 1E6);
            it.it_interval.tv_usec = (__suseconds_t) (TTUP_US);
            it.it_interval.tv_sec = 0;
            setitimer(ITIMER_REAL, &it, NULL);
        }
    }

    // PDEBUG("Delta [%d] = %d\trealDiff = %d\texpectedDiff = %d\t delay = %d", agentNumber, agent[agentNumber].delta, realDiff, expectedDiff, delay);
    // for (i=0; i < agentNumber; i++)
    //     FDEBUG(filedebug, "\t");
    // FDEBUG(filedebug, "%d\n", (int)(realDiff - COMM_DELAY));
    // FDEBUG(filedebug, "Received from %1d->%1d(%4u)-->delay=%7d realDiff=%7d expectedDiff=%7d\n", agent[agentNumber].inFramePos, agentNumber, frameHeader.counter, agent[agentNumber].delta, realDiff, expectedDiff);

    return (0);
}


void update_stateTable(void) {
    int i, j;

    for (i = 0; i < MAX_AGENTS; i++) {
        if (i != myNumber) {
            switch (agent[i].state) {
                case RUNNING:
                    if (agent[i].received == NO)
                        agent[i].state = REMOVE;
                    break;
                case NOT_RUNNING:
                    if (agent[i].received == YES)
                        agent[i].state = INSERT;
                    break;
                case INSERT:
                    if (agent[i].received == NO)
                        agent[i].state = NOT_RUNNING;
                    else {
                        for (j = 0; j < MAX_AGENTS; j++)
                            if ((agent[j].state == RUNNING) &&
                                ((agent[j].stateTable[i] == NOT_RUNNING) || (agent[j].stateTable[i] == REMOVE)))
                                break;
                        agent[i].state = RUNNING;
                    }
                    break;
                case REMOVE:
                    if (agent[i].received == YES) {
                        agent[i].removeCounter = 0;
                        agent[i].state = RUNNING;
                    } else {
                        for (j = 0; j < MAX_AGENTS; j++)
                            if ((agent[j].state == RUNNING) &&
                                ((agent[j].stateTable[i] == RUNNING) || (agent[j].stateTable[i] == INSERT)))
                                break;
                        agent[i].removeCounter++;
                        if (agent[i].removeCounter >= MAX_REMOVE_TICKS) {
                            agent[i].state = NOT_RUNNING;
                            agent[i].removeCounter = 0;
                        }
                    }
                    break;
            }
        }
    }

    // my state
    agent[myNumber].state = RUNNING;
}

Timer3::Timer3() {
    loops = 0;
    gettimeofday(&start, NULL);
    for (int i = 0; i < MAX_TIMERS; ++i) {
        gettimeofday(t1 + i, NULL);
        time_cnt[i] = 0;
    }
}

void Timer3::calcRate() {
    gettimeofday(&end, NULL);

    long seconds, useconds;
    double time;

    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    time = (double) seconds + (double) useconds / 1000000.0;
    if (++loops % 50 == 0) {
        std::cout << "freq: " << ((double) 50) / time << " Hz" << std::endl;

        gettimeofday(&start, NULL);
        loops = 0;
    }
}

void Timer3::startTimer(int t) {
    gettimeofday(t1 + t, NULL);
}

double Timer3::endTimer(int t) {
    gettimeofday(t2 + t, NULL);
    long seconds, useconds;
    double time;

    seconds = t2[t].tv_sec - t1[t].tv_sec;
    useconds = t2[t].tv_usec - t1[t].tv_usec;

    time = (double) seconds + (double) useconds / 1000000.0;
    return time;
}

double Timer3::averageTime(int t) {
    double time = endTimer(t);
    if (time_cnt[t] == 0) {
        time_avg[t] = time;
        time_cnt[t]++;
    } else {
        time_avg[t] = (time_avg[t] * time_cnt[t] + time) / (time_cnt[t] + 1);
        time_cnt[t]++;
    }
    return time_avg[t];
}

Timer3::~Timer3() {

}

//  Receive Thread
//
//  Input:
//    int *sckt = pointer of socket descriptor
//
void *receiveDataThread(void *arg) {
    int recv_id = 0;
    int recvLen;
    char recvBuffer[BUFFER_SIZE];
    int indexBuffer;
    int agentNumber;
    int i;
    struct _frameHeader frameHeader;
    
    threadParams_t* params = (threadParams_t *) arg;

    while (!end) {
        bzero(recvBuffer, BUFFER_SIZE);
        indexBuffer = 0;

        std::string src_ip;
        if ((recvLen = receiveData(params->socket, recvBuffer, BUFFER_SIZE, &src_ip)) > 0) {
            if (recvLen < (int) sizeof(frameHeader)) {
                PERR("[COMM] IP: %s, Message: data is too small to be valid, received %d bytes", src_ip.c_str(), recvLen);
                continue;
            }
            
            if (params->options->packet_record) {
                PDEBUG("[COMM] Writing received packet %d to file", recv_id);
                if (!boost::filesystem::exists("packets/recv/"))
                    boost::filesystem::create_directories("packets/recv/");
                std::stringstream filename;
                filename << "packets/recv/" << recv_id << ".raw";
                FILE* file = fopen(filename.str().c_str(), "wb");
                fwrite(recvBuffer, sizeof(char), recvLen, file);
                fclose(file);

                recv_id++;
                if (recv_id >= NUM_PACKETS_WRITE) recv_id = 0;
            }

            PDEBUG("[COMM] Received a frame with %d bytes from %s", recvLen, src_ip.c_str());
            memcpy(&frameHeader, recvBuffer + indexBuffer, sizeof(frameHeader));
            indexBuffer += sizeof(frameHeader);

            agentNumber = frameHeader.number;

            PDEBUG("[COMM] Received from agent %d", agentNumber);
            if (agentNumber < MAX_AGENTS && agentNumber >= 0) {
                gettimeofday(&(agent[agentNumber].receiveTimeStamp), NULL);
            }

            // Receive from ourself. Not supposed to occur. just to prevent!
            PDEBUG("[COMM] Validating agent number %d", agentNumber);
            if (agentNumber == myNumber)// && (nosend == 0))
                continue;

            // TODO: correction when frameCounter overflows
            PDEBUG("[COMM] Updating state table");
            if (agentNumber < MAX_AGENTS && agentNumber >= 0) {
                if ((agent[agentNumber].lastFrameCounter + 1) != frameHeader.counter)
                    lostPackets[agentNumber] = frameHeader.counter - (agent[agentNumber].lastFrameCounter + 1);
                agent[agentNumber].lastFrameCounter = frameHeader.counter;
                // state team view from received agent
                for (i = 0; i < MAX_AGENTS; i++)
                    agent[agentNumber].stateTable[i] = frameHeader.stateTable[i];
            }
            PDEBUG("[COMM] Allocating memory for the received message");
            unsigned int data_size = frameHeader.recordsSize * sizeof(char);
            if ((long int) (data_size + sizeof(frameHeader)) != recvLen) {
                PERR("[COMM] IP: %s, Message: data size unexpected! Unknown packet was received.", src_ip.c_str());
                continue;
            }
            char * data_arr = (char*) malloc(data_size);
            if (data_arr == NULL) {
                PDEBUG("[COMM] Failed to allocate memory for the message");
                continue;
            }
            memmove(data_arr, recvBuffer + indexBuffer, data_size);
            PDEBUG("[RtDB2] DB_put_batch");
            try {
                DB_put_batch(agentNumber, std::string(data_arr, data_size), COMM_DELAY_MS);
            } catch(std::bad_cast& e) {
                PERR("[COMM] IP %s, Message: Unknown frame was received from agent %d! Caught a bad_cast exception "
                             "while parsing data!", src_ip.c_str(), agentNumber);
                continue;
            } catch(...) {
                PERR("[COMM] IP %s: Message: Something went wrong while inserting data to the RtDB (from agent %d). "
                             "Caught an exception!", src_ip.c_str(), agentNumber);
                continue;
            }
            free(data_arr);
            PDEBUG("[RtDB2] finished DB_put_batch");
            PDEBUG("[COMM] Finish to process frame from agent %d", agentNumber);

#ifndef UNSYNC
            sync_ratdma(agentNumber);
#endif

        }
    }

    return NULL;
}

void printUsage(void) {
    printf("Usage: comm <interface_name> [OPTIONS]\n\n");
    printf("<interface_name> - eth0, wlan0, other\n");
    printf("\nOptional args:\n");
    printf("    -network <network-name>\n");
    printf("        Name of the network, used to find correct network\n");
    printf("        settings as specified in comm_network.conf.\n");
    printf("		If no network is provided, the default will be used.\n");
    printf(" 		If the network is defined in rtdb.conf as well,\n");
    printf("        then only the specified shared records are sent.\n");
    printf("    -nosend\n");
    printf("        Only receives data\n");
    printf("    -record\n");
    printf("        Records packets to the folder packets (sent and received)");
}

int parseArguments(int argc, char **argv, optionStruct_t *options) {
    int i;
    /* reset the options struct */
    memset(options, 0, sizeof(optionStruct_t));

    if (argc < 2) {
        printUsage();
        return -1;
    }

    /* set network interface */
    options->iface = argv[1];

    /* loop over other arguments and parse options. Loop start at two two skip first two arguments
     * (process name and network interface). */
    for (i = 2; i < argc; i++) {
        if (strncmp("-network", argv[i], sizeof("-network")) == 0) {
            if (i + 1 < argc) {
                options->network_name = argv[i + 1];
            }
        } else if (strncmp("-nosend", argv[i], sizeof("-nosend")) == 0) {
            options->nosend = true;
        } else if (strncmp("-compress", argv[i], sizeof("-compress")) == 0) {
            options->compressData = true;
        } else if (strncmp("-record", argv[i], sizeof("-record")) == 0) {
            options->packet_record = true;
        }
    }

    return 0;
}

//*************************
//  Main
//
int main(int argc, char *argv[]) {
    multiSocket_t sckt;
    pthread_t recvThread;
    char sendBuffer[BUFFER_SIZE];
    int indexBuffer;
    unsigned int frameCounter = 0;
    int i, j;

    bool syncMode = false;

    struct sched_param proc_sched;
    pthread_attr_t thread_attr;
    struct itimerval it;
    struct _frameHeader frameHeader;
    struct timeval tempTimeStamp;

    // Parse args
    optionStruct_t options;
    int argParser = 0;
    if ((argParser = parseArguments(argc, argv, &options)) != 0)
        return argParser;

    sckt.compressedData = options.compressData;

    NetworkConfig networkConfig("../config/comm_network.conf");
    networkConfig.set_network_iface(options.iface);
    networkConfig.get_network_config(&options.network_name);
    networkConfig.print_config();

    if (networkConfig.frequency != 0)
        TTUP_US = 1E6 / ((float) networkConfig.frequency);
    if (networkConfig.frequency < 0)
        syncMode = true;

    /* initializations */
    delay = 0;
    timer = 0;
    end = 0;
    RUNNING_AGENTS = 1;

    /* Assign a real-time priority to process */
    proc_sched.sched_priority = 60;
    if ((sched_setscheduler(getpid(), SCHED_FIFO, &proc_sched)) < 0) {
        PERRNO("setscheduler");
        return -1;
    }

    if (signal(SIGALRM, signal_catch) == SIG_ERR) {
        PERRNO("signal");
        return -1;
    }

    if (signal(SIGINT, signal_catch) == SIG_ERR) {
        PERRNO("signal");
        return -1;
    }

    if (openSocket(&sckt, &networkConfig) == -1) {
        PERR("openMulticastSocket");
        printf("\nUsage: comm <interface_name>\n\n");
        return -1;
    }

    setenv("AGENT", "0", 0); // If AGENT is not set, it is set to 0; DB_init uses this variable
    if (DB_init() == -1) {
        PERR("DB_init");
        closeSocket(&sckt);
        return -1;
    }

#ifdef FILEDEBUG
    if ((filedebug = fopen("log.txt", "w")) == NULL)
    {
        PERRNO("fopen");
        DB_free();
        closeSocket(sckt);
        return -1;
    }
#endif

    /* initializations */
    for (i = 0; i < MAX_AGENTS; i++) {
        lostPackets[i] = 0;
        agent[i].lastFrameCounter = 0;
        agent[i].state = NOT_RUNNING;
        agent[i].removeCounter = 0;
    }
    myNumber = Whoami();
    agent[myNumber].state = RUNNING;

    /* receive thread */
    threadParams_t params;
    params.options = &options;
    params.socket = &sckt;
    pthread_attr_init(&thread_attr);
    pthread_attr_setinheritsched(&thread_attr, PTHREAD_INHERIT_SCHED);
    if ((pthread_create(&recvThread, &thread_attr, receiveDataThread, (void *) &params)) != 0) {
        PERRNO("pthread_create");
        DB_free();
        closeSocket(&sckt);
        return -1;
    }

    /* Set itimer to reactivate the program */
    it.it_value.tv_usec = (__suseconds_t) (TTUP_US);
    it.it_value.tv_sec = 0;
    it.it_interval.tv_usec = (__suseconds_t) (TTUP_US);
    it.it_interval.tv_sec = 0;
    setitimer(ITIMER_REAL, &it, NULL);

    printf("communication: STARTED in ");

    if (syncMode)
        printf("sync mode...\n");
    else
        printf("unsync mode...\n");

    if (options.compressData)
        printf(" [COMPRESS DATA]\n");
    else
        printf(" [RAW DATA]\n");

    Timer3 freqTimer;
    int send_id = 0;
    
    // Main thread that is responsible for SENDING packets starts here
    while (!end) {
        if (options.nosend) {
            printf("No Send\n");
            sleep(1);
            continue;
        }

            // Using rand() instead of mersenee twister
            double waitTime = (2 * (((double) rand()) / ((double) RAND_MAX)) - 1) * TTUP_US * 0.05 + TTUP_US;
            usleep(waitTime);


        // not timer event
        //if (timer == 0)
        //	continue;

#ifndef UNSYNC
        // dynamic agent 0
        if ((delay > (int)MIN_UPDATE_DELAY_US) && (agent[myNumber].dynamicID == 0) && timer == 1)
        {
            it.it_value.tv_usec = (__suseconds_t)(delay - (int)MIN_UPDATE_DELAY_US/2);
            it.it_value.tv_sec = 0;
            setitimer (ITIMER_REAL, &it, NULL);
            delay = 0;
            continue;
        }
#endif

        timer = 0;

        indexBuffer = 0;
        bzero(sendBuffer, BUFFER_SIZE);

        update_stateTable();

        // update dynamicID
        j = 0;
        for (i = 0; i < MAX_AGENTS; i++) {
            if ((agent[i].state == RUNNING) || (agent[i].state == REMOVE)) {
                agent[i].dynamicID = j;
                j++;
            }
            agent[myNumber].stateTable[i] = agent[i].state;
        }
        RUNNING_AGENTS = j;

        MAX_DELTA = (int) (TTUP_US / RUNNING_AGENTS * 2 / 3);

        // frame header
        frameHeader.number = myNumber;
        frameHeader.counter = frameCounter;
        frameCounter++;
        for (i = 0; i < MAX_AGENTS; i++)
            frameHeader.stateTable[i] = agent[myNumber].stateTable[i];

        PDEBUG("[RtDB2] DB_get_batch");
        std::string data;
        try {
            data = DB_get_batch();
        } catch(std::bad_cast& e) {
            PERR("[RtDB2] Could not send frame, failed to unserialize! Caught a bad_cast exception while parsing data!");
            continue;
        } catch(...) {
            PERR("[RtDB2] Something went wrong while getting data from the RtDB to send. Caught an exception!");
            continue;
        }
        PDEBUG("[RtDB2] finished DB_get_batch");

        // 50 Hz -> 5 records each second of the RtDB = 5 * 60 *
        /*if (frameCounter % 10 == 0)  {
            std::ofstream file;
            std::stringstream ss;
            ss << "data" << std::setfill('0') << std::setw(10) << frameCounter;
            file.open(ss.str(), std::ios::binary);
            file.write(data.c_str(), data.length());
            file.close();
        }*/
        int data_size = static_cast<int>(data.size());
        frameHeader.recordsSize = data_size;

        if (data_size > BUFFER_SIZE) {
            PERR("[COMM] Pretended frame (%d bytes) is bigger that the available buffer", data_size);
            continue;
        }

        memcpy(sendBuffer + indexBuffer, &frameHeader, sizeof(frameHeader));
        indexBuffer += sizeof(frameHeader);
        memmove(sendBuffer + indexBuffer, data.c_str(), data.size());
        indexBuffer = indexBuffer + data_size;

	if (nosend == 0) {
            if (sendData(&sckt, sendBuffer, indexBuffer) != indexBuffer)
                PERRNO("[COMM] Error sending data");
        }

        if (options.packet_record) {
            PDEBUG("[COMM] Writing packet %d sent to file", send_id);
            if (!boost::filesystem::exists("packets/send/"))
                boost::filesystem::create_directories("packets/send/");
            std::stringstream filename;
            filename << "packets/send/" << send_id << ".raw";
            FILE* file = fopen(filename.str().c_str(), "wb");
            fwrite(sendBuffer, sizeof(char), indexBuffer, file);
            fclose(file);

            send_id++;
            if (send_id >= NUM_PACKETS_WRITE) send_id = 0;
        }

        gettimeofday(&tempTimeStamp, NULL);
        lastSendTimeStamp.tv_sec = tempTimeStamp.tv_sec;
        lastSendTimeStamp.tv_usec = tempTimeStamp.tv_usec;

        // reset values for next round
        for (i = 0; i < MAX_AGENTS; i++) {
            agent[i].delta = 0;
            agent[i].received = NO;
        }
    }

    FDEBUG (filedebug, "\nLost Packets:\n");
    for (i = 0; i < MAX_AGENTS; i++)
            FDEBUG (filedebug, "%d\t", lostPackets[i]);
    FDEBUG (filedebug, "\n");

    printf("communication: STOPED.\nCleaning process...\n");


#ifdef FILEDEBUG
    fclose (filedebug);
#endif

    closeSocket(&sckt);
    pthread_join(recvThread, NULL);
    DB_free();
    printf("communication: FINISHED.\n");

    return 0;
}
