#include <stdio.h>
#include <stdlib.h>

#include "rtdbdefs.h"
#include "rtdb2_adapter.h"

// Agent identifier and pointer to the respective RtDB
std::map<int, boost::shared_ptr<RtDB2> > rtdb_map;
// Agent identifier
int __agent = -1;

void DB_set_config_file(const char *cf) {
    (void) cf;
}

void _DB_free(int _agent) {
    (void) _agent;
}

void DB_free(void) {
    if (__agent != -1)
        _DB_free(__agent);
}


void DB_free_all(int _second_rtdb) {
    int i;
    int offset;
    if (_second_rtdb == 1)
        offset = MAX_AGENTS;
    else
        offset = 0;

    for (i = 0; i < MAX_AGENTS; i++)
        _DB_free(i + offset);
}

int DB_initialization(int _agent, int _second_rtdb) {
    (void) _second_rtdb;

    std::stringstream ss;
    ss << DEFAULT_PATH << "/agent" << _agent;
    rtdb_map.insert(std::pair<int, boost::shared_ptr<RtDB2> >(
            _agent,
            boost::make_shared<RtDB2>(_agent, ss.str()))
    );
    return 0;
}

int DB_init() {
    char *environment;
    int agent;
    int second_rtdb = 0;

    // retrieve agent number
    if ((environment = getenv("AGENT")) == NULL) {
        RTDB_ERROR("getenv");
        return -1;
    }
    agent = atoi(environment);

    __agent = agent;
    RTDB_DEBUG("agent = %d", agent);

    if ((environment = getenv("SECOND_RTDB")) == NULL) {
        second_rtdb = 0;
    } else {
        second_rtdb = atoi(environment);
        agent = MAX_AGENTS + agent;
    }


    return (DB_initialization(agent, second_rtdb));
}

int DB_init_all(int _second_rtdb) {
    (void) _second_rtdb;

    int i;
    int agent;

    for (i = 0; i < MAX_AGENTS; i++) {
        if (_second_rtdb == 0)
            agent = i;
        else
            agent = MAX_AGENTS + i;

        if (DB_initialization(agent, _second_rtdb) == -1)
            return (-1);
    }

    return (0);
}

int Whoami(void) {
    return __agent;
}

int DB_comm_ini(RTDBconf_var *rec) {
    (void) rec;
    
    return 0;
}

std::string DB_get_batch() {
    return DB_get_batch(__agent);
}

std::string DB_get_batch(int from_agent) {
    std::string serialized_data;
    std::map<int, boost::shared_ptr<RtDB2> >::iterator it = rtdb_map.find(from_agent);
    if (it == rtdb_map.end())
        return serialized_data;
    std::string data;
    int err = it->second->get_batch(data);
    if (err) {
        std::stringstream ss;
        ss << "Failed to get batch (from = " << from_agent << ")";
        RTDB_PRINT_ERROR_CODE(err, ss.str());
    }
    return data;
}

void DB_put_batch(int from_agent, const std::string& data, int life) {
    DB_put_batch(from_agent, __agent, data, life);
}

void DB_put_batch(int from_agent, int to_agent, const std::string& data, int life) {
    std::map<int, boost::shared_ptr<RtDB2> >::iterator it = rtdb_map.find(to_agent);
    if (it == rtdb_map.end())
        return;
    int err = it->second->put_batch(from_agent, data, life, true);
    if (err) {
        std::stringstream ss;
        ss << "Failed to put batch (from = " << from_agent << ", to = " << to_agent << ")";
        RTDB_PRINT_ERROR_CODE(err, ss.str());
    }
}
