#ifndef CAMBADA_RTDB2STORAGE_H
#define CAMBADA_RTDB2STORAGE_H

#include <string>
#include <ostream>
#include <fstream>
#include <vector>

class RtDB2Storage {
public:
    virtual ~RtDB2Storage() {}

    virtual int insert(std::string key, std::string binary_value) = 0;
    virtual int insert_batch(const std::vector<std::pair<std::string, std::string> >& values) = 0;
    virtual int fetch(std::string key, std::string& value) = 0;
    virtual int fetch_all_data(std::vector<std::pair<std::string, std::string> >& values) = 0;

    friend std::ostream& operator<<(std::ostream& os, RtDB2Storage& obj);
private:
    virtual std::ostream& dump(std::ostream&) = 0;
};

inline std::ostream& operator<<(std::ostream& os, RtDB2Storage& obj) {
    os << (obj.dump(os)).rdbuf();
    return os;
}

#endif //CAMBADA_RTDB2STORAGE_H
