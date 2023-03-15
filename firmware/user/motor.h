#ifndef _MOTOR_H_
#define _MOTOR_H_

/*
    LSA : PB3, TIM2_CH2
    LSB : PB4, TIM3_CH1
    LSC : PB5, TIM3_CH2

    HSA : PA3, TIM2_CH4
    HSB : PA2, TIM2_CH3
    HSC : PA1, TIM2_CH2
*/


#include <stdint.h>

class Motor
{
    public:
        Motor();

        virtual ~Motor();

        void init();

        /*
            d, q  : torque size and phase
            theta : rotor angle from 0..1023, (SINE_TABLE_SIZE - 1, gonio.h)
        */
        void set_park(int32_t d, int32_t q, uint32_t theta);

        void set_clarke(int32_t alpha, int32_t beta);

        void set_phases(int32_t a, int32_t b, int32_t c);


    public:
        void main();

    private:
        int32_t clamp(int32_t value, int32_t min, int32_t max);

    public:
        int32_t d, q;

};

#endif