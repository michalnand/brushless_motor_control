# stm32 FOC brushless motor controller

![board](doc/images/spinning.jpg)

![board](doc/images/top_board.jpg)
![board](doc/images/bottom_board.jpg)

# FOC block diagrams

![inner_loop](doc/diagrams/motor_control-inner_loop.png)

![outer_loop](doc/diagrams/motor_control-outer_loop.png)


# transformation tests

basic tranformations tests :
- clarke - parke - pwm scaling
- see [transform_test.py](utils/transform_test.py)


![transformations](utils/transformations.png)




# LQR torque controller

see [lqr_design.ipynb](utils/lqr_design.ipynb)

## closed loop response + disturbance rejection

![closed_loop_response](utils/closed_loop_response.png)


## poles

![poles](utils/poles.png)