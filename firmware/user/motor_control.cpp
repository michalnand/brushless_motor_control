#include <motor_control.h>

MotorControl::MotorControl(AS5600 *encoder, Motor *motor)
{
    this->encoder   = encoder;
    this->motor     = motor;

    this->torque = 0;

    this->lqr_velocity.init(1.0, 1.0, 0.0, 1.0);
    this->lqr_position.init(1.0, 1.0, 1.0, 1.0);
}

MotorControl::~MotorControl()
{

}

/*
    most inner control loop, call it as fast as possible (1kHz..2kHz)
*/
void MotorControl::callback_torque()
{   
    /*
    //measure current
    float current  = adc_read()/r;

    //convert desired torque to current (T = I*Kt = I/Kv) 
    float error = this->torque*Kv - current;
    
    //PI controller
    error_sum   = error_sum + error*dt;

    u = kp*error + ki*error_sum;

    //apply voltage
    this->motor->set_torque(u, 0, this->encoder->read_angle());
    */

    /*
    //measure current
    int32_t current = adc_read()/r;

    //convert desired torque to current (T = I*Kt = I/Kv) 
    int32_t error   = this->torque*Kv - current
    
    //PI controller
    error_sum       = error_sum + error*dt;
    int32_t u       = kp*error + ki*error_sum;

    //apply voltage
    this->motor->set_torque(u, 0, this->encoder->read_angle());
    */
}   

/*
    state space controller loop, call it in aprox. 200Hz
*/
void MotorControl::callback(int32_t dt_us)
{
    this->encoder->update(dt_us);

    int32_t position    = this->encoder->position;
    int32_t velocity    = this->encoder->angular_velocity;

    //int32_t velocity_req = 4096;
    //this->torque = this->lqr_velocity.step(velocity_req, velocity);

    float position_req = 90;

    this->torque = MOTOR_CONTROL_MAX*this->lqr_position.step(position_req*DEG_TO_RAD, position*BID_TO_RAD, velocity*BID_TO_RAD);
}