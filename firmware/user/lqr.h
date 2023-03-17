#ifndef _LQR_H_
#define _LQR_H_

#include <stdint.h>

class LQR
{
    public:
        LQR();
        void init(float g, float k0, float k1, float antiwindup);
        float step(float xr, float x, float dx);

    private:
        float g, k0, k1;
        float aw;
        float e_sum;
};

#endif