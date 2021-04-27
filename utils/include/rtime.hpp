/*
 * rtime.hpp
 *
 * RtDB2 timestamp utilities, conversions, standards.
 *
 *  Created on: Aug 12, 2018
 *      Author: Jan Feitsma
 */

#ifndef RTDB2_RTIME_HPP_
#define RTDB2_RTIME_HPP_

#include <string>
#include <sys/time.h>
#include <chrono>
#include <boost/function.hpp>
#include "RtDB2Definitions.h" // for serialization


using rtdb_timepoint_type = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;


class rtime
{
public:

    rtdb_timepoint_type _timepoint; // using chrono for underlying data type

    rtime(double v = 0);
    ~rtime();

    // get current timestamp
    static rtime now();

    // run a timed loop, calling function on given frequency until it returns false
    static void loop(float frequency, boost::function<bool(void)> callback, std::string const &id="");


    // converters
    rtime &fromTimeval(const struct timeval tv);
    struct timeval toTimeval() const;
    std::string toStrDate() const; // handy for file names, example: 20180812_200159
    std::string toStr() const; // handy for tracing, example: 2018-08-12,20:01:59.442195
    rtime &fromDouble(double d);
    double toDouble() const;
    operator double() const { return toDouble(); }

    // operators (see also more free functions below)
    rtime &operator+=(const double &d);
    rtime &operator-=(const double &d);

    // custom serialization
    template <typename Packer> void msgpack_pack(Packer& pk) const
    {
        struct timeval tv = toTimeval();
        msgpack::type::make_define_array(tv.tv_sec, tv.tv_usec).msgpack_pack(pk);
    }
    void msgpack_unpack(msgpack::object const& o)
    {
        struct timeval tv;
        msgpack::type::make_define_array(tv.tv_sec, tv.tv_usec).msgpack_unpack(o);
        fromTimeval(tv);
    }
    template <typename MSGPACK_OBJECT> void msgpack_object(MSGPACK_OBJECT* o, msgpack::zone& z) const
    {
        struct timeval tv;
        msgpack::type::make_define_array(tv.tv_sec, tv.tv_usec).msgpack_object(o, z);
    }

private:
    operator float() const; // not implemented, to force compile time error, since implicitly throwing away least-significant bits is typically not desired

};


inline rtime operator+(rtime lhs, const double rhs)
{
    lhs += rhs;
    return lhs;
}

inline rtime operator-(rtime lhs, const double rhs)
{
    lhs -= rhs;
    return lhs;
}

inline rtime operator+(rtime lhs, const float rhs)
{
    return lhs + double(rhs);
}

inline rtime operator-(rtime lhs, const float rhs)
{
    return lhs - double(rhs);
}

inline double operator-(rtime lhs, const rtime& rhs)
{
    std::chrono::duration<double> diff = lhs._timepoint - rhs._timepoint;
    return diff.count();
}

inline bool operator==(const rtime& lhs, const rtime& rhs)
{
    return lhs._timepoint == rhs._timepoint;
}

inline bool operator<(const rtime& lhs, const rtime& rhs)
{
    return lhs._timepoint < rhs._timepoint;
}

inline bool operator!=(const rtime& lhs, const rtime& rhs)
{
    return !operator==(lhs, rhs);
}

inline bool operator>(const rtime& lhs, const rtime& rhs)
{
    return operator<(rhs, lhs);
}

inline bool operator<=(const rtime& lhs, const rtime& rhs)
{
    return !operator>(lhs, rhs);
}

inline bool operator>=(const rtime& lhs, const rtime& rhs)
{
    return !operator<(lhs, rhs);
}


#endif

