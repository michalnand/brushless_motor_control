#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <as5600.h>
#include <motor.h>
#include <lqr.h>

class MotorControl
{
    public:
        MotorControl(AS5600 *encoder, Motor *motor);
        virtual ~MotorControl();

        /*
            most inner control loop, call it as fast as possible (1kHz..2kHz)
            handles phases comutation
        */
        void callback_torque();

        /*
            state space controller loop, call it in aprox. 200Hz
        */
        void callback(int32_t dt_us);

    private:
        AS5600  *encoder;
        Motor   *motor;
        LQR     lqr_velocity;
        LQR     lqr_position;

        int32_t torque;
};

#endif