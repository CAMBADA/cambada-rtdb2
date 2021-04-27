#ifndef CAMBADA_RTDB2SYNCPOINT_H
#define CAMBADA_RTDB2SYNCPOINT_H

#include <sys/sem.h>
#include "RtDB2Definitions.h"

struct RtDB2SyncPoint {
    //TODO: int process_id; // PID of the "subscriber"
    int sem_ID; // semaphore ID

    SERIALIZE_DATA(sem_ID);
};

#endif //CAMBADA_RTDB2SYNCPOINT_H
