#include <motor_control.h>
#include <gonio.h>


//void TIM1_BRK_UP_TRG_COM_IRQHandler()   __attribute__ ((weak, alias("Default_Handler")));
//void TIM1_CC_IRQHandler()               __attribute__ ((weak, alias("Default_Handler")));

template<class DType>
DType _abs(DType v)
{
    if (v < 0)
    {
        v = -v;
    }

    return v;
}
 
template<class DType>
DType _sgn(DType v)
{
    if (v > 0)
    {
        return 1;
    }
    if (v < 0)
    {
        return - 1;
    }
    
    return 0;
}

template<class DType>
DType _clip(DType v, DType min_v, DType max_v)
{
    if (v < min_v)
    {
        v = min_v;
    }
    else if (v > max_v)
    {
        v = max_v;
    }

    return v;
}



MotorControl::MotorControl()
{

}

MotorControl::~MotorControl()
{

}


void MotorControl::init()
{
    this->angle             = 0;
    this->angular_velocity  = 0;
    this->motor_current     = 0;
    this->required_current  = 0;
    this->required_position = 0;

    adc_init();


    motor.init();
    motor.hold();

    i2c.init();
    encoder.init(&i2c);

    //torque_pid.init((float)0.3*16384, (float)100.0*16384, (float)0.0*16384, 0, MOTOR_CONTROL_MAX, 1);

    torque_pid.init((float)0.3*16384, (float)50.0*16384, (float)0.0*16384, 0, MOTOR_CONTROL_MAX, 1);
    
    k0 = (float)0.00627266;
    k1 = (float)0.84664735;
    ki = (float)31.6227766; 

    error_sum = 0.0;  
}

     
void MotorControl::callback_torque()
{    
    encoder.update(1);  
    //rotor angle (0..4096)
    this->angle             = encoder.angle;
 
    this->angle_position    = -encoder.position;
    this->angular_velocity  = -encoder.angular_velocity;
    
    //i     = u/r = (adc*3.3/4096)/0.33
    //uref  = 3.3V, R = 0.33ohm, result in mA
    int32_t adc         = adc_read(ADC_Channel_4);
    this->motor_current = (adc*10000)/4096; 

    int32_t u = _abs(torque_pid.step(_abs(this->required_current) - this->motor_current));

    int32_t phase = SINE_TABLE_SIZE/4;
 
    if (this->required_current < 0) 
    { 
        phase = -phase; 
    }

    motor.set_torque(u, phase, this->angle); 
}

void MotorControl::callback()
{ 
    //integral action 
    float error = this->required_position - this->angle_position;
          error = error*(float)(2.0*3.141592654/4096.0)*(float)(1.0/500.0);

    error_sum   = error_sum + error;  
  

    //read motor state : angular velocity, and rotor angle
    //convert to rad/s and rad
    float x0 = this->angular_velocity*(float)(2.0*3.141592654/4096.0);
    float x1 = this->angle_position*(float)(2.0*3.141592654/4096.0);
  
    //LQR controller with integral action
    float u     =  -x0*k0 - x1*k1 + ki*error_sum;
    float u_sat = _clip(u, (float)-1.0, (float)1.0);

    //antiwindup
    float aw = u - u_sat;
    error_sum-= aw*(float)(1.0/500.0);

    //u is in amps, convert to mA
    this->required_current = 1000*u_sat;
}   
