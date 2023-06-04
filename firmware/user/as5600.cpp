#include "as5600.h"
#include <device.h>

#define I2C_ADDRESS ((unsigned char)0x36<<1)

#define ZMCO_ADR    ((unsigned char)0x00)
#define ZPOS_H_ADR  ((unsigned char)0x01)
#define ZPOS_L_ADR  ((unsigned char)0x02)
#define MPOS_H_ADR  ((unsigned char)0x03)
#define MPOS_L_ADR  ((unsigned char)0x04)
#define MANG_H_ADR  ((unsigned char)0x05)
#define MANG_L_ADR  ((unsigned char)0x06)
#define CONF_H_ADR  ((unsigned char)0x07)
#define CONF_L_ADR  ((unsigned char)0x08)

#define RAW_ANGLE_H_ADR  ((unsigned char)0x0C)
#define RAW_ANGLE_L_ADR  ((unsigned char)0x0D)
#define ANGLE_H_ADR      ((unsigned char)0x0E)
#define ANGLE_L_ADR      ((unsigned char)0x0F)

#define STATUS_ADR  ((unsigned char)0x0B)
#define AGC_ADR     ((unsigned char)0x1A)
#define MAGNITUDE_H_ADR     ((unsigned char)0x1B)
#define MAGNITUDE_L_ADR     ((unsigned char)0x1C)


AS5600::AS5600()
{

}

AS5600::~AS5600()
{

}

int AS5600::init(I2C_Interface *i2c_)
{
    this->i2c           = i2c_;
    this->angle         = 0; 

    //check if device responds using ACK, since sensor doesn't have WHO_AM_I ID
    unsigned char ack = i2c->check(I2C_ADDRESS);
    if (ack == 0)
    {
        return -1;
    }

      
    //power on
    //hysteresis 1 LSB
    //slow filter only
    i2c->write_reg(I2C_ADDRESS, CONF_L_ADR, (1<<2));
    i2c->write_reg(I2C_ADDRESS, CONF_H_ADR, (1<<0)|(1<<1));
    

    this->position          = 0;
    this->position_prev     = 0;
    this->angular_velocity  = 0;
    this->prev_value        = 0;

    //set zero angle
    set_zero();

    this->read_angle();
    this->update();


    this->position          = this->angle;
    this->position_prev     = this->angle;
    this->angular_velocity  = 0;
    this->prev_value        = 0;
    
    return 0;
}

int32_t AS5600::read_angle()
{
    this->angle = i2c->read_reg_16bit(I2C_ADDRESS, ANGLE_H_ADR)&0x0fff;
    return this->angle;
}


void AS5600::update(int32_t dt_ms)
{
    //int16_t value = i2c->read_reg_16bit(I2C_ADDRESS, RAW_ANGLE_H_ADR)&0x0fff;
    int16_t value = i2c->read_reg_16bit(I2C_ADDRESS, ANGLE_H_ADR)&0x0fff;

    this->angle = value; 
 
    this->position_prev = this->position;
 
    //  whole rotation CW?
    //  less than half a circle
    if ((this->prev_value > 2048) && ( value < (this->prev_value - 2048)))
    {
        position = this->position + 4096 - this->prev_value + value;
    } 
  
    //  whole rotation CCW?
    //  less than half a circle
    else if ((value > 2048) && ( this->prev_value < (value - 2048)))
    {
        position = this->position - 4096 - this->prev_value + value;
    }
    else 
    {
        position = this->position - this->prev_value + value;
    }

    this->position =  position; 
 
     
    this->prev_value = value;

    this->angular_velocity = ((this->position - this->position_prev)*1000)/dt_ms; 
}

/*
void AS5600::update(int32_t dt_us)
{
    //0 ... 4095
    int16_t angle   = i2c->read_reg_16bit(I2C_ADDRESS, RAW_ANGLE_H_ADR)&0x0fff;
    int16_t dangle  = angle - this->prev_value;

    //  assumption is that there is no more than 180° rotation
    //  between two consecutive measurements.
    //  => at least two measurements per rotation (preferred 4).
    if (deltaA >  2048) deltaA -= 4096;
    if (deltaA < -2048) deltaA += 4096;
    float    speed   = (deltaA * 1e6) / deltaT;

    //  remember last time & angle
    _lastMeasurement = now;
    _lastAngle       = angle;

   
}
*/


void AS5600::set_zero()
{
    //set zero angle
    uint8_t raw_h = i2c->read_reg(I2C_ADDRESS, RAW_ANGLE_H_ADR);
    uint8_t raw_l = i2c->read_reg(I2C_ADDRESS, RAW_ANGLE_L_ADR);

    i2c->write_reg(I2C_ADDRESS, ZPOS_H_ADR, raw_h); 
    i2c->write_reg(I2C_ADDRESS, ZPOS_L_ADR, raw_l); 
}