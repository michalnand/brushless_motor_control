#include <lqr.h>

LQR::LQR()
{

}

void LQR::init(float g, float k0, float k1, float antiwindup)
{
    this->g     = g;
    this->k0    = k0;
    this->k1    = k1;
    this->aw    = antiwindup;
    this->e_sum = 0;
}

float LQR::step(float x_req, float x, float dx)
{
    float e = x_req - x;
    
    //integral action
    e_sum+= e;
    
    //LQR control law
    float u = g*e_sum - k0*x - k1*dx;

    //antiwindup
    //raising e_sum is no more effecting u,
    //since u is out of range 
    if (u > aw)
    {
        u = aw;
        u-= e;
    }
    if (u < -aw)
    {
        u = -aw;
        u-= e;
    }


    return u;
}   