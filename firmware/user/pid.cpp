#include <pid.h>

PID::PID()
{

}

void PID::init(int32_t kp, int32_t ki, int32_t kd, int32_t antiwindup)
{
    this->k0 = kp + ki + kd;
    this->k1 = -kp -2*kd;
    this->k2 = kd;

    this->antiwindup = antiwindup;

    this->e0 = 0;
    this->e1 = 0;
    this->e2 = 0;

    this->u  = 0;
}

int32_t PID::step(int32_t error)
{
    e2 = e1;
    e1 = e0;
    e0 = error;

    u = u + k0*e0 + k1*e1 + k2*e2;

    if (u > antiwindup)
    {
        u = antiwindup;
    }

    if (u < -antiwindup)
    {
        u = -antiwindup;
    }

    return u;
}


   