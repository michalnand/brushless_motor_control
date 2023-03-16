#include "motor.h"

#include <gonio.h>
#include <device.h>




//helping stuff
#define SQRT3       ((int32_t)1773)      // sqrt(3)     = 1773/1024
#define SQRT3INV    ((int32_t)591)       // 1/sqrt(3)   = 591/1024

#define max2(x,y) (((x) >= (y)) ? (x) : (y))
#define min2(x,y) (((x) <= (y)) ? (x) : (y))

#define max3(x, y, z) (max2(max2(x, y), z))
#define min3(x, y, z) (min2(min2(x, y), z))


Motor::Motor()
{
    this->init();
}

Motor::~Motor()
{

}

void Motor::init()
{
    this->hw_init();
    this->set_torque(0, 0, 0);
}

void Motor::set_torque(int32_t torque, uint32_t phase, uint32_t rotor_angle)
{
    int32_t q = (torque*cos_tab(phase))/SINE_TABLE_MAX;
    int32_t d = (torque*sin_tab(phase))/SINE_TABLE_MAX;

    this->set_park(d, q, rotor_angle*MOTOR_POLES);
}

 
void Motor::set_park(int32_t d, int32_t q, uint32_t theta)
{
    //inverse Park transform
    int32_t alpha = (d*cos_tab(theta) - q*sin_tab(theta))/SINE_TABLE_MAX;
    int32_t beta  = (d*sin_tab(theta) + q*cos_tab(theta))/SINE_TABLE_MAX;

    this->set_clarke(alpha, beta);
}

void Motor::set_clarke(int32_t alpha, int32_t beta)
{
    //inverse Clarke transform
    int32_t a = alpha;
    int32_t b = -(alpha/2) + (SQRT3*beta)/(2*1024);
    int32_t c = -(alpha/2) - (SQRT3*beta)/(2*1024);


    this->set_phases(a, b, c); 
}

void Motor::set_phases(int32_t a, int32_t b, int32_t c)
{
    //transform into space-vector modulation, to achieve full voltage range
    int32_t min_val = min3(a, b, c);
    int32_t max_val = max3(a, b, c); 

    int32_t com_val = (min_val + max_val)/2;  

    //normalise into 0..MOTOR_CONTROL_MAX
    int32_t a_pwm = ((a - com_val)*SQRT3INV)/1024 + MOTOR_CONTROL_MAX/2;
    int32_t b_pwm = ((b - com_val)*SQRT3INV)/1024 + MOTOR_CONTROL_MAX/2;
    int32_t c_pwm = ((c - com_val)*SQRT3INV)/1024 + MOTOR_CONTROL_MAX/2;
    
     
    a_pwm = clamp((a_pwm*PWM_PERIOD)/MOTOR_CONTROL_MAX, 0, PWM_PERIOD);
    b_pwm = clamp((b_pwm*PWM_PERIOD)/MOTOR_CONTROL_MAX, 0, PWM_PERIOD);
    c_pwm = clamp((c_pwm*PWM_PERIOD)/MOTOR_CONTROL_MAX, 0, PWM_PERIOD);

    set_pwm(a_pwm, b_pwm, c_pwm);
}



void Motor::hw_init()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    //PWM controll pins 
    //PWMA : PA3, TIM2_CH4
    //PWMB : PA2, TIM2_CH3
    //PWMC : PA1, TIM2_CH2

    //RCC_APB1PeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    RCC->AHBENR|= RCC_AHBENR_GPIOAEN;
 
    
    //init pins
    GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    //alternating functions for pins
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2);
    


    //enable controll pins
    //ENA : PB3, TIM2_CH2
    //ENB : PB4, TIM3_CH1
    //ENC : PB5, TIM3_CH2

    RCC->AHBENR|= RCC_AHBENR_GPIOBEN;
    
    //init pins
    GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_Level_1;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    


    //init timer 2

    TIM_TimeBaseInitTypeDef TIM_BaseStruct;

    //enable clock for timer 2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    //RCC->AHBENR|= RCC_APB1ENR_TIM2EN;

    
    // PWM_frequency = timer_clk / (TIM_Period + 1)
    // TIM_Period = timer_clk / PWM_frequency - 1
    TIM_BaseStruct.TIM_Prescaler    = 0;
    TIM_BaseStruct.TIM_CounterMode  = TIM_CounterMode_Up;

    TIM_BaseStruct.TIM_Period               = PWM_PERIOD;
    TIM_BaseStruct.TIM_ClockDivision        = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter    = 0;
	 
    // Initialize TIM2
    TIM_TimeBaseInit(TIM2, &TIM_BaseStruct);
    TIM_Cmd(TIM2, ENABLE);


    //stop motor    
    this->set_pwm(0, 0, 0);

    //set enable to high, for all phases
    GPIOB->ODR |= (1<<3)|(1<<4)|(1<<5); 
}


void Motor::set_pwm(uint32_t a_pwm, uint32_t b_pwm, uint32_t c_pwm)
{
    //PWM output settings
    TIM_OCInitTypeDef TIM_OCStruct;
    
    // PWM mode 2 : clear on compare match
    // PWM mode 1 : set on compare match
    TIM_OCStruct.TIM_OCMode         = TIM_OCMode_PWM2;
    TIM_OCStruct.TIM_OutputState    = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_OCPolarity     = TIM_OCPolarity_Low;

    //HSA pwm, PA3, TIM2_CH4
    TIM_OCStruct.TIM_Pulse = a_pwm;
    TIM_OC4Init(TIM2, &TIM_OCStruct);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

    //HSB pwm, PA2, TIM2_CH3
    TIM_OCStruct.TIM_Pulse = b_pwm;
    TIM_OC3Init(TIM2, &TIM_OCStruct);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

    //HSC pwm, PA1, TIM2_CH2
    TIM_OCStruct.TIM_Pulse = c_pwm;
    TIM_OC2Init(TIM2, &TIM_OCStruct);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
}



int32_t Motor::clamp(int32_t value, int32_t min, int32_t max)
{
    if (value > max)
    {
        value = max;
    }

    if (value < min)
    {
        value = min;
    }

    return value;
}