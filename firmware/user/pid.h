#ifndef _PID_H_
#define _PID_H_

#include <stdint.h>

class PID
{
    public:
        PID();
        void init(int32_t kp, int32_t ki, int32_t kd, int32_t antiwindup);
        int32_t step(int32_t error);


    private:
        int32_t k0, k1, k2;
        int32_t e0, e1, e2;
        int32_t u;
        int32_t antiwindup;
};

#endif