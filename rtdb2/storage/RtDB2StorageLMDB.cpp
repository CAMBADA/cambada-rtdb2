#include "RtDB2StorageLMDB.h"

#include <boost/filesystem.hpp>
#include <sstream>
#include "../RtDB2ErrorCode.h"
#include "../RtDB2Definitions.h"

RtDB2LMDB::RtDB2LMDB(std::string parent_path, std::string name) : is_initialized_(false)
{
    std::stringstream file_path;
    file_path << parent_path;
    file_path << "/";
    file_path << name;

    storage_path_ = file_path.str();
}

RtDB2LMDB::~RtDB2LMDB() {
    if (is_initialized_) {
        mdb_dbi_close(env_, dbi_);
        mdb_env_close(env_);
    }
}

int RtDB2LMDB::init_operation(bool read_operation) {
    int err;
    if (!is_initialized_) {
        err = read_operation ? init_storage_if_exists() : init_storage();
        if (err != RTDB2_SUCCESS)
            return err;
    }

    int dead;
    err = mdb_reader_check(env_, &dead);
    if (dead != 0)
        RTDB_ERROR("[LMDB] Cleared %d reader stalls", dead);
    return err;
}

int RtDB2LMDB::init_storage() {
    // Create the folder used to store the databases
    if (!boost::filesystem::exists(storage_path_))
        boost::filesystem::create_directories(storage_path_);

    // Creating environment
    mdb_env_create(&env_);
    // This should be checked, the db may be resized but there must be no active transactions
    mdb_env_set_mapsize(env_, 100 * 1024 * 1024);
    mdb_env_open(env_, storage_path_.c_str(), MDB_WRITEMAP | MDB_NOSYNC | MDB_NOMETASYNC, 0664);

    MDB_txn *txn;
    // Creating the database
    mdb_txn_begin(env_, NULL, 0, &txn);
    mdb_dbi_open(txn, NULL, MDB_CREATE, &dbi_);
    mdb_txn_commit(txn);

    is_initialized_ = true;
    return RTDB2_SUCCESS;
}


int RtDB2LMDB::init_storage_if_exists() {
    if (!boost::filesystem::exists(storage_path_))
        return RTDB2_STORAGE_DOES_NOT_EXISTS;
    return init_storage();
}

int RtDB2LMDB::insert(std::string key, std::string binary_value) {
    int err;
    if ((err = init_operation(false)) != RTDB2_SUCCESS)
        return err;

    MDB_val mdb_key, mdb_value;
    MDB_txn *txn;

    mdb_key.mv_size = key.size();
    mdb_key.mv_data = const_cast<char*>(key.c_str());

    mdb_value.mv_size = binary_value.size();
    mdb_value.mv_data = const_cast<char*>(binary_value.c_str());

    mdb_txn_begin(env_, NULL, 0, &txn);
    mdb_put(txn, dbi_, &mdb_key, &mdb_value, 0);
    mdb_txn_commit(txn);
    return RTDB2_SUCCESS;
}

int RtDB2LMDB::fetch(std::string key, std::string& value) {
    int err;
    if (init_operation(true) != RTDB2_SUCCESS)
        return RTDB2_KEY_NOT_FOUND;

    MDB_txn *txn;
    MDB_val obj_key, obj_data;

    obj_key.mv_size = key.size();
    obj_key.mv_data = const_cast<char*>(key.c_str());

    mdb_txn_begin(env_, NULL, MDB_RDONLY, &txn);
    err = mdb_get(txn, dbi_, &obj_key, &obj_data);
    mdb_txn_abort(txn);

    if (err)
        return RTDB2_KEY_NOT_FOUND;
    value = std::string(static_cast<char*>(obj_data.mv_data), obj_data.mv_size);
    return 0;
}


int RtDB2LMDB::fetch_all_data(std::vector<std::pair<std::string, std::string> >& values) {
    if (init_operation(true) != RTDB2_SUCCESS)
        return RTDB2_KEY_NOT_FOUND;

    MDB_txn *txn;
    MDB_cursor *cursor;
    MDB_val key, data;

    mdb_txn_begin(env_, NULL, MDB_RDONLY, &txn);
    mdb_cursor_open(txn, dbi_, &cursor);
    while (mdb_cursor_get(cursor, &key, &data, MDB_NEXT) == 0) {
        values.push_back(std::pair<std::string, std::string>(
                std::string(static_cast<char*>(key.mv_data), key.mv_size),
                std::string(static_cast<char*>(data.mv_data), data.mv_size))
        );
    }
    mdb_cursor_close(cursor);
    mdb_txn_abort(txn);
    return 0;
}

int RtDB2LMDB::insert_batch(const std::vector<std::pair<std::string, std::string> >& values) {
    int err;
    if ((err = init_operation(false)) != RTDB2_SUCCESS)
        return err;

    MDB_txn *txn;
    MDB_cursor *cursor;
    mdb_txn_begin(env_, NULL, 0, &txn);
    mdb_cursor_open(txn, dbi_, &cursor);
    
    std::vector<std::pair<std::string, std::string> >::const_iterator entry;
    for (entry = values.begin(); entry != values.end(); entry++) {
        MDB_val mdb_key, mdb_value;
        mdb_key.mv_size = entry->first.size();
        mdb_key.mv_data = const_cast<char*>(entry->first.c_str());

        mdb_value.mv_size = entry->second.size();
        mdb_value.mv_data = const_cast<char*>(entry->second.c_str());

        mdb_cursor_put(cursor, &mdb_key, &mdb_value, 0);
    }
    mdb_cursor_close(cursor);
    mdb_txn_commit(txn);
    return 0;
}

std::ostream& RtDB2LMDB::dump(std::ostream& os) {
    if (init_operation(true) != RTDB2_SUCCESS)
        return os;

    MDB_txn *txn;
    MDB_cursor *cursor;
    MDB_val key, data;

    mdb_txn_begin(env_, NULL, MDB_RDONLY, &txn);
    mdb_cursor_open(txn, dbi_, &cursor);
    while (mdb_cursor_get(cursor, &key, &data, MDB_NEXT) == 0) {
        std::string obj_key(static_cast<char*>(key.mv_data), key.mv_size);
        std::string obj_data(static_cast<char*>(data.mv_data), data.mv_size);
        os << "key: " << obj_key << ", data: " << obj_data << std::endl;
    }
    mdb_cursor_close(cursor);
    mdb_txn_abort(txn);
    return os;
}
