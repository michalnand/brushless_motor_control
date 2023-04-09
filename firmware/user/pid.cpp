#include <pid.h>

PID::PID()
{

}

void PID::init(int32_t kp, int32_t ki, int32_t kd, int32_t antiwindup_min, int32_t antiwindup_max, int32_t dt)
{
    this->k0 = kp + ((ki*dt)/1000) + ((kd*1000)/dt);
    this->k1 = -kp - 2*((kd*1000)/dt);
    this->k2 = ((kd*1000)/dt); 

    this->antiwindup_min = antiwindup_min;
    this->antiwindup_max = antiwindup_max;

    this->e0    = 0;
    this->e1    = 0;
    this->e2    = 0;

    this->u     = 0;
}

/*
    error = desired_value - measured_value

    returns controller output u
*/
int32_t PID::step(int32_t error)
{
    this->e2 = this->e1;
    this->e1 = this->e0;
    this->e0 = error;

    int32_t du = (k0*e0 + k1*e1 + k2*e2)/16384;
    u+= du;

    //anti windup
    if (u > antiwindup_max)
    {
        u = antiwindup_max;
    }
    
    if (u < antiwindup_min)
    {
        u = antiwindup_min;
    }

    return u;
}


   