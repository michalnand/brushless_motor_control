#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <gpio.h>
#include <adc.h>
#include <i2c.h>
#include <as5600.h>
#include <motor.h>
#include <pid.h>


class MotorControl
{
    public: 
        MotorControl();
        virtual ~MotorControl();

        void init();

        /*
            most inner control loop, call it as fast as possible (1kHz..2kHz)
            handles phases comutation
        */
        void callback_torque();

        /*
            state space controller loop, call it in aprox. 200Hz
        */
        void callback();

    private:
        void _timer_init();


    private:
        //current sense, PA4, adc_ch 4
        Gpio<TGPIOA, 4, GPIO_Mode_AN>  adc_sense;

        TI2C<TGPIOB, 7, 6>  i2c;
        AS5600              encoder;
        Motor               motor;
    
    private:
        PID                 torque_pid;
    
    public:
        float k0, k1, ki;
        float error_sum; 
        
    public:
        int32_t angle, angle_position, angular_velocity;

        int32_t motor_current;
        int32_t required_current;

        int32_t required_position;
};

#endif