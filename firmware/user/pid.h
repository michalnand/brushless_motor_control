#ifndef _PID_H_
#define _PID_H_

#include <stdint.h>

class PID
{
    public:
        PID();

        /*
            integer values used, with 14bit precission,
            to convert from float32 multiply by 16384 :

            kp : 16384 x Kp
            ki : 16384 x Ki
            kd : 16384 x Kd

            dt : dt step in miliseconds, 1s = 1000
        */
        void init(int32_t kp, int32_t ki, int32_t kd, int32_t antiwindup_min, int32_t antiwindup_max, int32_t dt);
        int32_t step(int32_t error);


    private:
        int32_t k0, k1, k2;
        int32_t e0, e1, e2;
        int32_t u;
        int32_t antiwindup_min, antiwindup_max;
};

#endif