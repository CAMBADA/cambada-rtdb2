#ifndef CAMBADA_RTBD2_ADAPTER_H
#define CAMBADA_RTBD2_ADAPTER_H

#include <boost/smart_ptr/scoped_ptr.hpp>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>

#include "rtdbdefs.h"
#include "../rtdb2/RtDB2.h"

#include <limits>

// External variables that come from the implementation and
// required in the template functions
extern std::map<int, boost::shared_ptr<RtDB2> > rtdb_map;
extern int __agent;

// Functions declarations
void DB_set_config_file(const char* cf);
int DB_init(void);
int DB_init_all (int _second_rtdb);
int DB_comm_ini(RTDBconf_var *rec);
int Whoami(void);
void DB_free(void);
void DB_free_all(int _second_rtdb);
std::string DB_get_batch();
std::string DB_get_batch(int from_agent);
void DB_put_batch(int from_agent, const std::string& data, int life);
void DB_put_batch(int from_agent, int to_agent, const std::string& data, int life);
template<typename T>
int DB_get_from(int _agent, int _from_agent, int _id, T *_value);
template<typename T>
int DB_put_in(int _agent, int _to_agent, int _id, T* _value, int life);
template<typename T>
int DB_comm_put(int _to_agent, int _id, int _size, T *_value, int _life);
template<typename T>
int DB_put(int _id, T *_value);
template<typename T>
int DB_get(int _from_agent, int _id, T *_value);

// Implementation of the template functions
template<typename T>
int DB_get_from(int _agent, int _from_agent, int _id, T *_value) {
    int life = 0;
    std::map<int, boost::shared_ptr<RtDB2> >::iterator it = rtdb_map.find(_agent);
    if (it == rtdb_map.end()) {
        life = -1;
    } else {
        int err = it->second->get(_id, _value, life, _from_agent);
        if (err != RTDB2_SUCCESS) {
            std::stringstream ss;
            ss << "Failed to get value (id = " << _id << ", agent = " << _agent 
                    << ", from = " << _from_agent << ")";
            RTDB_PRINT_ERROR_CODE(err, ss.str());
            if (_value != NULL) {
                memset(_value, 0, sizeof(T));
            }
            life = std::numeric_limits<int>::min();
        }
    }
    return life;
}

template<typename T>
int DB_put_in(int _agent, int _to_agent, int _id, T* _value, int life) {
    std::map<int, boost::shared_ptr<RtDB2> >::iterator it = rtdb_map.find(_agent);
    if (it == rtdb_map.end())
        return 0;
    int err = it->second->put_root(_id, _value, life, _to_agent);
    if (err != RTDB2_SUCCESS) {
        std::stringstream ss;
        ss << "Failed to put value (id = " << _id << ", agent = " << _agent << ", to = " << _to_agent << ")";
        RTDB_PRINT_ERROR_CODE(err, ss.str());
    }
    return 0;
}

template<typename T>
int DB_comm_put(int _to_agent, int _id, int _size, T *_value, int _life) {
    if (_to_agent == SELF || _to_agent == __agent) {
        RTDB_ERROR("Impossible to write in the running agent!");
        return -1;
    }

    return DB_put_in(__agent, _to_agent, _id, _value, _life);
}

template<typename T>
int DB_put(int _id, T *_value) {
    if (__agent == -1)
        return -1;
    return DB_put_in(__agent, __agent, _id, _value, 0);
}

template<typename T>
int DB_get(int _from_agent, int _id, T *_value) {
    if (__agent == -1)
        return -1;
    return DB_get_from(__agent, _from_agent, _id, _value);
}

#endif //CAMBADA_RTBD2_ADAPTER_H
