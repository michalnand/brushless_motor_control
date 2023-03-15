#include "motor.h"

#include <gonio.h>

#define PWM_MAX     ((int32_t)255)

#define SQRT3       ((int32_t)322)      // sqrt(3)      = 443/256
#define SQRT3INV    ((int32_t)147)      // 1/sqrt(3)    = 148/256

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
    //TODO PWM and GPIO init
    //max PWM freq. is arround 150kHz !!!, O-driver uses 25kHz

    this->d = 0;
    this->q = 0;
    this->set_park(0, 0, 0);
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
    int32_t b = -(alpha/2) + (SQRT3*beta)/(2*256);
    int32_t c = -(alpha/2) - (SQRT3*beta)/(2*256);

    this->set_phases(a, b, c); 
}


void Motor::set_phases(int32_t a, int32_t b, int32_t c)
{
    //transform into space-vector modulation, to achieve full voltage range
    int32_t min_val = min3(a, b, c);
    int32_t max_val = max3(a, b, c); 

    int32_t com_val = (min_val + max_val)/2; 

    //normalise into 0..pwm_max
    int32_t a_pwm = ((a - com_val)*SQRT3INV)/256 + PWM_MAX/2;
    int32_t b_pwm = ((b - com_val)*SQRT3INV)/256 + PWM_MAX/2;
    int32_t c_pwm = ((c - com_val)*SQRT3INV)/256 + PWM_MAX/2;

    a_pwm = clamp(a_pwm, 0, PWM_MAX);
    b_pwm = clamp(b_pwm, 0, PWM_MAX);
    c_pwm = clamp(c_pwm, 0, PWM_MAX);

    //TODO : set PWMs
}

//callback called in high speed timer
void Motor::main()
{
    int32_t angle = 0; //read_angle();

    this->set_park(this->d, this->q, angle);
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