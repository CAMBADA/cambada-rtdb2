#include "RtDB2.h"
#include <iostream>
#include <utility> // make_pair
#include <fcntl.h> // O_CREAT
#include <random>
#include <boost/filesystem.hpp>

#include "compressor/RtDB2CompressorZstd.h"
#include "compressor/RtDB2CompressorLZ4.h"
#include "tprintf.hpp"

#include "RtDB2SyncPoint.h"

void RtDB2::construct()
{
    // compressor
    const CompressorSettings compressor_settings = _configuration.get_compressor_settings();
    if (compressor_settings.name == "lz4")
    {
        _compressor = boost::make_shared<RtDB2CompressorLZ4>();
    }
    else if (compressor_settings.name == "zstd")
    {
        if (compressor_settings.use_dictionary)
        {
            _compressor = boost::make_shared<RtDB2CompressorZstd>(ZSTD2_DICTIONARY_FILE);
        }
        else
        {
            _compressor = boost::make_shared<RtDB2CompressorZstd>();
        }
    }
    else
    {
        // TODO: better to throw exception?
        // TODO: allow no compressor
        std::cout << "Invalid compressor name (" << compressor_settings.name
                    << ") was received into the RtDB" << std::endl
                    << "Considering default values (zstd with dictionary)" << std::endl;
        _compressor = boost::make_shared<RtDB2CompressorZstd>(ZSTD2_DICTIONARY_FILE);
    }
}

RtDB2::RtDB2(int agentId, std::string const &path)
:
    _agentId(agentId),
    _path(path),
    _configuration(),
    _compressor(NULL)
{
    construct();
}

boost::shared_ptr<RtDB2Storage> RtDB2::getStorage(int agentId, bool isSync)
{
    // select main storage
    auto s = &_storage;
    if (isSync)
    {
        s = &sync_;
    }
    // initialize if needed
    if (!s->count(agentId))
    {
        s->insert(std::pair<int, boost::shared_ptr<RtDB2Storage> >(
                agentId, boost::make_shared<RtDB2LMDB>(_path, createAgentName(agentId, isSync))));
    }
    return s->at(agentId);
}

std::string RtDB2::createAgentName(int agentId, bool isSync)
{
    std::stringstream stream;
    stream << DB_PREPEND_NAME;
    if (isSync)
    {
        stream << "_sync";
    }
    stream << agentId;
    return stream.str();
}

void RtDB2::generateIncompatibilityWarning(std::string const &key)
{
    int agentId = -1;
    // warn only once
    static std::set<std::string> warnedAlready;
    if (warnedAlready.count(key))
    {
        return;
    }
    warnedAlready.insert(key);
    // generate warning
    tprintf("WARNING: bad data found at agent=%d key=%s", agentId, key.c_str());
}

RtDB2Item RtDB2::makeItem(std::string const &key, int agentId, std::string const &serialized_data)
{
    RtDB2Item item;
    agentId = agentId + 0; // dummy, to prevent compiler warning 'unused variable':
    // it was considered to also make key and agentId part of item, but this was dropped after discussion with Ricardo due to data being redundant
    item.data = serialized_data;
    item.timestamp = rtime::now();
    item.shared = _configuration.get_key_details(key).shared;
    return item;
}

int RtDB2::insertItem(std::string const &key, int agentId, RtDB2Item const &item)
{
    // serialize item
    std::string serialized_item;
    int err = RtDB2Serializer::serialize(item, serialized_item);
    if (err == RTDB2_SUCCESS)
    {
        // select storage, create if not existing
        auto storage = getStorage(agentId, false);
        storage->insert(key, serialized_item);
    }
    return err;
}

int RtDB2::getItem(std::string const &key, RtDB2Item &item, int agentId)
{
    return getCore(key, item, agentId, false);
}

int RtDB2::getItem(std::string const &key, RtDB2Item &item)
{
    return getItem(key, item, _agentId);
}

int RtDB2::getCore(std::string const &key, RtDB2Item &item, int agentId, bool consume)
{
    rdebug("getCore start agent=%d consume=%d key=%s", agentId, consume, key.c_str());
    int err = 0;

    // acquire storage, initialize if needed
    auto storage = getStorage(agentId, false);

    // get raw data
    std::string raw_data;
    if (consume)
    {
        err = storage->fetch_and_clear(key, raw_data);
    }
    else
    {
        err = storage->fetch(key, raw_data);
    }
    rdebug("getCore err %d", err);
    if (err != 0)
    {
        return err;
    }

    // decode into item
    err = RtDB2Serializer::deserialize(raw_data, item);

    rdebug("getCore end key=%s err=%d", key.c_str(), err);
    return err;
}

int RtDB2::putFrameString(std::string const &s, bool refreshTime)
{
    int err = 0;
    // optional decompression
    std::string uncompressed = s;
    if (_compressor != NULL)
    {
        err = _compressor->decompress(s, uncompressed);
        if (err != RTDB2_SUCCESS)
        {
            return err;
        }
    }
    // put frame
    RtDB2Frame frame;
    frame.fromString(uncompressed);
    return putFrame(frame, refreshTime);
}

int RtDB2::putFrame(RtDB2Frame const &frame, bool refreshTime)
{
    rdebug("putFrame start");
    int err = 0;
    // since we choose to have one storage per agent, the items must be distributed
    std::vector<RtDB2FrameItem> remainingItems = frame.items;
    while (remainingItems.size())
    {
        int currentAgent = remainingItems[0].agent;
        // get items belonging to this agent
        std::vector<std::pair<std::string, std::string>> data;
        for (auto it = remainingItems.begin(); it != remainingItems.end(); )
        {
            if (it->agent == currentAgent)
            {
                // slice RtDB2Item
                RtDB2Item item = *it;
                if (refreshTime)
                {
                    item.timestamp = rtime::now();
                }
                std::string data_serialized;
                RtDB2Serializer::serialize(item, data_serialized);
                data.push_back(std::make_pair(it->key, data_serialized));
                it = remainingItems.erase(it);
            }
            else
            {
                it++;
            }
        }
        // acquire storage, initialize if needed
        auto storage = getStorage(currentAgent, false);
        // store
        rdebug("putFrame agent=%d size=%d", currentAgent, (int)data.size());
        err = storage->insert_batch(data);
        if (err != RTDB2_SUCCESS)
        {
            break;
        }
    }
    rdebug("putFrame end err=%d", err);
    return err;
}

int RtDB2::getFrameString(std::string &s, RtDB2FrameSelection const &selection, int counter)
{
    rdebug("getFrameString start");
    // get frame
    RtDB2Frame frame;
    int err = getFrame(frame, selection, counter);
    if (err == RTDB2_SUCCESS)
    {
        std::string uncompressed = frame.toString();
        // optional compression
        if (_compressor != NULL)
        {
            err = _compressor->compress(uncompressed, s);
            //tprintf("frame compression ratio: %.2f%% (from %d bytes to %d bytes)", 100.0 * ((float)s.size() / uncompressed.size()), (int)uncompressed.size(), (int)s.size());
        }
        else
        {
            s.assign(uncompressed);
        }
    }
    rdebug("getFrameString end size=%d err=%d", (int)s.size(), err);
    return err;
}

int RtDB2::getFrame(RtDB2Frame &frame, RtDB2FrameSelection const &selection, int counter)
{
    rdebug("getFrame start");
    rtime now = rtime::now();

    // Get data from all storages as serialized RtDB2Item objects
    // Tuple: agent, key, serialized_item
    std::vector<std::tuple<int, std::string, std::string>> data;
    // TODO: how to visit only existing storages? iterating over assumed agents is not a nice solution ...
    for (int agent = 0; agent < 10; ++agent)
    {
        auto storage = getStorage(agent, false);
        std::vector<std::pair<std::string, std::string>> tmpdata;
        storage->fetch_all_data(tmpdata);
        // copy into main list
        for (auto it = tmpdata.begin(); it != tmpdata.end(); ++it)
        {
            data.push_back(std::make_tuple(agent, it->first, it->second));
        }
    }

    // Store in frame while checking selection conditions
    frame.items.clear();
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        bool keep = true;
        int agent = std::get<0>(*it);
        std::string key = std::get<1>(*it);
        // deserialize item
        RtDB2Item item;
        int errItem = 0; // one failing item deserialization should not be fatal
        std::string serialized_item = std::get<2>(*it);
        errItem = RtDB2Serializer::deserialize(serialized_item, item);
        if (errItem != RTDB2_SUCCESS)
        {
            tprintf("deserialize failed: key=%s #before=%d s='%s'", key.c_str(), (int)serialized_item.size(), serialized_item.c_str());
            generateIncompatibilityWarning(key);
            keep = false;
        }
        // construct frame item
        RtDB2FrameItem frameItem(item);
        frameItem.agent = agent;
        frameItem.key = key;
        // check selection criteria
        if (errItem == RTDB2_SUCCESS)
        {
            const KeyDetail& detail = _configuration.get_key_details(frameItem.key);
            // locality attribute check
            if (keep)
            {
                if (!selection.local && (frameItem.shared == false))
                {
                    keep = false;
                }
                if (!selection.shared && (frameItem.shared == true))
                {
                    keep = false;
                }
            }
            // agent id
            if (keep)
            {
                if (selection.agents.size() > 0) // otherwise keep all
                {
                    if (std::find(selection.agents.begin(), selection.agents.end(), frameItem.agent) == selection.agents.end())
                    {
                        keep = false;
                    }
                }
            }
            // sub-sampling: skip if it is not the required period plus its phase
            if (keep)
            {
                if ((counter >= 0) && ((counter - detail.phase_shift) % detail.period != 0))
                {
                    keep = false;
                }
            }
            // age: skip if too old
            if (keep)
            {
                double age = now - frameItem.timestamp;
                double maxAge = selection.maxAge;
                // key might have a timeout attribute
                maxAge = std::min(maxAge, (double)detail.timeout);
                keep = (age <= maxAge);
            }
            // uniqueness: skip if already returned earlier (useful for condensed logging)
            if (keep && selection.unique)
            {
                // key: (agent, key) -> serialized_item
                static std::map<std::tuple<int, std::string>, std::string> seen;
                auto seenkey = std::make_tuple(agent, key);
                keep = seen[seenkey] != serialized_item;
                seen[seenkey] = serialized_item;
            }
        }
        // store in frame
        if (keep)
        {
            frame.items.push_back(frameItem);
        }
    }

    rdebug("getFrame end");
    return 0;
}

int RtDB2::waitForPut(std::string const &key)
{
    return waitForPut(key, _agentId);
}

int RtDB2::waitForPut(std::string const &key, int agentId)
{
    rdebug("waitForPut start agent=%d key=%s", agentId, key.c_str());

    RtDB2SyncPoint syncPoint = RtDB2SyncPoint();
    // TODO: check if registration is needed?
    

    //// Create semaphore
    sem_t* sem;
    char semFile[255];
    
    // Init randomizer
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(1, INT_MAX); // Generate random semaphore key between 1 and INT_MAX


    // Try at most 10 times to find a unique semaphore
    for (int i = 0; i < 10; i++)
    {
        syncPoint.sem_ID = distrib(gen);

        // Put key in named semaphore
        // e.g., "/dev/shm/sem.12345"
        sprintf(semFile, "/dev/shm/sem.%d", syncPoint.sem_ID);

        // If /dev/shm/sem.`sem_ID` does not exist, break
        if ( !boost::filesystem::exists( semFile ) )
        {
            break;
        }

        tprintf("waitForPut semaphore exists '%s'", semFile);
    }


    // Put key in named semaphore
    // e.g., "/12345"
    char semBuf[255];
    sprintf(semBuf, "/%d", syncPoint.sem_ID);

    // Create and initialize semaphore
    sem = sem_open(semBuf, O_CREAT | O_EXCL, 0644, 0); /* Initial value is 0 */
    if (sem == SEM_FAILED)
    {
        tprintf("waitForPut RTDB2_FAILED_SEMAPHORE_CREATION '%s': %s", semBuf, strerror(errno));
        return RTDB2_FAILED_SEMAPHORE_CREATION;
    }



    // Auto-Register if needed
    auto it = sync_.find(agentId);
    if(it == sync_.end())
    {
        it = sync_.insert(std::pair<int, boost::shared_ptr<RtDB2Storage> >(
                agentId, boost::make_shared<RtDB2LMDB>(_path, createAgentName(agentId, true)))).first;
    }
    it->second->append_to_sync_list(key, syncPoint);


    
    // Decrement (lock) the semaphore.
    // If the semaphore's value is zero, the call blocks until it becomes possible to perform the decrement (semaphores can not become negative)
    // The increment (unlock) is done in RtDB.h (putCore) using sem_post()
    int ret = sem_wait(sem);
    if (ret < 0)
    {
        // cleanup semaphore on failure
        sem_close(sem);
        sem_unlink(semBuf);

        tprintf("waitForPut RTDB2_FAILED_SEMAPHORE_WAIT '%s': %s", semBuf, strerror(errno));
        return RTDB2_FAILED_SEMAPHORE_WAIT;
    }

    // Close and cleanup the created semaphore
    sem_close(sem);
    sem_unlink(semBuf);

    rdebug("waitForPut end");
    return RTDB2_SUCCESS;
}

const RtDB2Configuration& RtDB2::getConfiguration() const
{
    return _configuration;
}

void RtDB2::compress(std::string &s)
{
    if (_compressor != NULL)
    {
        std::string result;
        int err = _compressor->compress(s, result);
        if (err == RTDB2_SUCCESS)
        {
            s = result;
        }
    }
}

void RtDB2::decompress(std::string &s)
{
    if (_compressor != NULL)
    {
        std::string result;
        int err = _compressor->decompress(s, result);
        if (err == RTDB2_SUCCESS)
        {
            s = result;
        }
    }
}

