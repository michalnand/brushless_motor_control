#ifndef _AS5600_H_
#define _AS5600_H_

#include <stdint.h>
#include <i2c_interface.h>


#define RPM_TO_RAD      ((float)(3.141592654/30.0))
#define RAD_TO_RPM      ((float)(30.0/3.141592654))

#define DEG_TO_RAD      ((float)(3.141592654/180.0))
#define RAD_TO_DEG      ((float)(180.0/3.141592654))

#define BID_TO_RAD      ((float)((2.0*3.141592654)/4096.0))
#define RAD_TO_BID      ((float)(4096.0/(2.0*3.141592654)))


class AS5600
{
    public:
        AS5600();
        virtual ~AS5600();

        int init(I2C_Interface *i2c_);

        int32_t read_angle();

        /*
            update following values
            position            : 1full rotation    = 4096
            angular_velocity    : 1full rotation/s  = 4096

            1/4096 of circle let be named : bid (binary degree)

            360degrees      = 4096   bid
            100degrees/s    = 1137.7 bid/s
        */
        void    update(int32_t dt_us = 10000);

        void    set_zero();


    public:
        int32_t angle; 
        int32_t position;
        int32_t angular_velocity;

    private:
        I2C_Interface *i2c;

        int32_t position_prev;
        int32_t prev_value;
};

#endif