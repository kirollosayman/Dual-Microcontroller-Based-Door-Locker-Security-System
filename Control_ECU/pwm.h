#ifndef MCAL_PWM_H_
#define MCAL_PWM_H_

#include "std_types.h"

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialize timer 0 in pwm mode and take duty cycle.
 */
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* MCAL_PWM_H_ */
