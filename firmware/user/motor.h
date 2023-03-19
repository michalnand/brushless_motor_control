#ifndef _MOTOR_H_
#define _MOTOR_H_

/*
    PWM controll pins :

    PWMA : PA3, TIM2_CH4
    PWMB : PA2, TIM2_CH3
    PWMC : PA1, TIM2_CH2


    enable controll pins:

    ENA : PB3, TIM2_CH2
    ENB : PB4, TIM3_CH1
    ENC : PB5, TIM3_CH2
*/

 


#define MOTOR_CONTROL_MAX       ((int32_t)1024)
#define MOTOR_POLES             ((int32_t)12)
#define MOTOR_KV                ((int32_t)250)
#define MOTOR_CURRENT_MAX       ((int32_t)5)


 
//20kHz PWM, 48MHz is timer 2 clock source
#define PWM_FREQUENCY           ((uint32_t)20000)
#define PWM_PERIOD              ((uint32_t)48000000/PWM_FREQUENCY - 1)

//maximal motor torque x 4096
#define MOTOR_TORQUE_MAX        ((4096*MOTOR_CURRENT_MAX)/MOTOR_KV)


#include <stdint.h>

class Motor
{
    public:
        Motor();

        virtual ~Motor();

        void init();

        /*
            main motor control

            torque t in range 0..MOTOR_CONTROL_MAX
            phase       : TODO, 0..SINE_TABLE_SIZE
            rotor_angle : measured rotor angle, 0..SINE_TABLE_SIZE
        */
        void set_torque(int32_t torque, uint32_t phase, uint32_t rotor_angle);

        //inverse Park transform
        //can be used for PI torque control
        void set_park(int32_t d, int32_t q, uint32_t theta);


    public:
        //inverse Clarke transform
        void set_clarke(int32_t alpha, int32_t beta);

        //transform into space-vector modulation, to achieve full voltage range
        void set_phases(int32_t a, int32_t b, int32_t c);

    private:
        void    hw_init();
        void    set_pwm(uint32_t a_pwm, uint32_t b_pwm, uint32_t c_pwm);
        int32_t clamp(int32_t value, int32_t min, int32_t max);
};

#endif