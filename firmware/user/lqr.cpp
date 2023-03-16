#include <lqr.h>



   
LQR::LQR()
{

}

void LQR::init(float g, float k0, float k1)
{
    this->g     = g;
    this->k0    = k0;
    this->k1    = k1;
    this->e_sum = 0;
}

float LQR::step(float x_req, float x, float dx)
{
    e_sum+= x_req - x;
    float u     = e_sum*g - k0*x - k1*dx;

    return u;
}


   