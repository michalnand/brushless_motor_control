#ifndef _AS5600_H_
#define _AS5600_H_

#include <stdint.h>
#include <i2c_interface.h>

class AS5600
{
    public:
        AS5600();
        virtual ~AS5600();

        int init(I2C_Interface *i2c_);

        void read();
        void set_zero();

    public:
        int32_t angle;

    private:
        int32_t zero_angle;

    private:
        I2C_Interface *i2c;
};

#endif