#ifndef _LQR_H_
#define _LQR_H_

#include <stdint.h>
#include <tmm.h>

template<unsigned int system_order, unsigned int inputs_count>
class LQR
{
    public:
        LQR()
        {
            tmm::fill<system_order*inputs_count, float>(this->k, 0);
            tmm::fill<system_order*inputs_count, float>(this->ki, 0);

            tmm::fill<system_order, float>(e_sum, 0);
            tmm::fill<inputs_count, float>(u, 0);
            tmm::fill<inputs_count, float>(u_sat, 0);
        }

        void init(float *k, float *ki, float antiwindup)
        {
            tmm::copy<system_order*inputs_count, float>(this->k, k);
            tmm::copy<system_order*inputs_count, float>(this->ki, ki);

            tmm::fill<system_order, float>(e_sum, 0);
            tmm::fill<inputs_count, float>(u, 0);
            tmm::fill<inputs_count, float>(u_sat, 0);

            this->antiwindup = antiwindup;
        }

        float* step(float *xr, float *x, float dt)
        {
            //compute error and integral action
            //error = xr - x
            //error_sum_new = error_sum + error*self.dt
            tmm::mac<inputs_count, float>(error_sum, xr, y, dt, -dt);
                
            //LQR controll law
            //u = -self.k@x + self.ki@error_sum_new
            tmm::mm<inputs_count, system_order, 1, float>(u, k, x, nullptr,   -1, 0);
            tmm::mm<inputs_count, system_order, 1, float>(u, ki, x, error_sum, 1, 1);

            //antiwindup
            //error_sum-= (u - u_sat)*dt
            u_sat = tmm::clip<inputs_count, float>(u, -antiwindup, antiwindup);
            tmm::mac<inputs_count, float>(error_sum, u, u_sat, -dt, +dt);

            return u;
        } 

    private:
        float k[system_order*inputs_count];
        float ki[system_order*inputs_count];
        float e_sum[system_order];
        float u[inputs_count];
        float u_sat[inputs_count];

        float antiwindup;    
};

#endif