/*
 * test_rtime.cpp
 *
 * Tester for timestamp utilities, conversions, standards.
 *
 */


#include "../rtime.hpp"


int main()
{
    // test now() and string conversion
    rtime t = rtime::now();
    printf("now() as string: %s\n", t.toStr().c_str());
    
    // double conversion
    double d = (double)t;
    printf("now() as double: %.6f\n", d);
    
    // float conversion should generate a compile-time error
    // reason: precision would be lost, which is typically not desired
    // and runtime errors can be too obscure to trigger / pinpoint
    //float f = (float)t;
    
    // operator-
    rtime delta = t - t;
    printf("delta as string: %s\n", delta.toStr().c_str()); // unix EPOCH (1-1-1970), possibly with some hours timezone offset
    printf("delta as double: %.6f\n", double(delta));
    
    // operator+
    rtime tnext = t + 0.033;
    printf("tnext as string: %s\n", tnext.toStr().c_str());
    printf("tnext as double: %.6f\n", double(tnext));
    
    // convertors
    rtime t2;
    t2.fromDouble(d);
    printf("equality after conversion from double: %d\n", (t2 == t));
    
    // delta after subtraction
    printf("dt after operator- as double: %.6f\n", double(tnext - t)); // should show 0.033
    printf("dt after operator- as float: %.6f (requires explicit cast)\n", float(double(tnext - t)));
    
    exit(0);
}


