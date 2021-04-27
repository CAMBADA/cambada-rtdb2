#ifndef CAMBADA_RTDB2LMDB_H
#define CAMBADA_RTDB2LMDB_H

#include <lmdb.h>

#include "RtDB2Storage.h"

class RtDB2LMDB : public RtDB2Storage {
public:
    RtDB2LMDB(std::string parent_path, std::string name);
    virtual ~RtDB2LMDB();

    virtual int insert(std::string key, std::string binary_value);
    virtual int insert_batch(const std::vector<std::pair<std::string, std::string> >& values);
    virtual int fetch(std::string key, std::string& value);
    virtual int fetch_all_data(std::vector<std::pair<std::string, std::string> >& values);
    virtual int fetch_and_clear(std::string key, std::string& value);

    virtual int append_to_sync_list(const std::string& key, const RtDB2SyncPoint& syncPoint);
    virtual int get_and_clear_sync_list(const std::string& key, std::vector<RtDB2SyncPoint>& list);

    virtual std::ostream& dump(std::ostream&);
private:
    // This function might be used to force initialise the storage,
    // although it is initialised on the first insert
    int init_storage();
    int init_storage_if_exists();
    int init_operation(bool read_operation);
    std::string storage_path_;
    MDB_env *env_;
    MDB_dbi dbi_;
    bool is_initialized_;
};


#endif //CAMBADA_RTDB2LMDB_H
