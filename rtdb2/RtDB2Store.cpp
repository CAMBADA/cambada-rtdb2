#include "RtDB2Store.h"

RtDB2* RtDB2Store::getRtDB2(int db_identifier)
{
    return getRtDBInstance( std::make_pair(db_identifier, "") );
}
RtDB2* RtDB2Store::getRtDB2(int db_identifier, char team_char)
{
    std::string path = "";
    // TODO: this should be reworked in a more general way, the SIM path switching was basically a Falcons hack
    //if (team_char == 'B')
    //{
    //    path = RTDB2_SIM_TEAM_B_PATH;
    //}
    return getRtDBInstance( std::make_pair(db_identifier, path) );
}
RtDB2* RtDB2Store::getRtDB2(int db_identifier, std::string path)
{
    return getRtDBInstance( std::make_pair(db_identifier, path) );
}
RtDB2* RtDB2Store::getRtDBInstance(const RtDB2InstanceKey& key)
{
    // find key.
    // if not found, create a new rtdb instance and add to the map under the key.
    // if found, return the existing rtdb instance
    auto it = _rtdbInstances.find(key);

    if (it != _rtdbInstances.end())
    {
        return it->second;
    }
    else
    {
        // rtdb instance not found.
        // create instance and add to the map
        RtDB2* newInstance = 0;
        if (key.second == "")
        {
            newInstance = new RtDB2(key.first);
        }
        else
        {
            newInstance = new RtDB2(key.first, key.second);
        }

        _rtdbInstances[key] = newInstance;

        return newInstance;
    }
}
