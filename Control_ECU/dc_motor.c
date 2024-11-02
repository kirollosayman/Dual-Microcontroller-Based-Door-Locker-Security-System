#include "dc_motor.h"
#include "gpio.h"
#include "pwm.h"

//Variables
uint8 speed=0;
DcMotor_State state;
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Initializes all DC motor pins direction.
   Turn off the motor at start
*/
void DcMotor_Init(void)
{
	//Initializes all motor pins direction
	GPIO_setupPinDirection(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_EN_PORT_ID,DC_MOTOR_EN_PIN_ID,PIN_OUTPUT);
	// Stop motor at the beginning
	GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
}

/*
 * Description :
 * Controls the motor's state (Clockwise/Anti-Clockwise/Stop) and adjusts the speed based
on the input duty cycle
*/
void DcMotor_Rotate(DcMotor_State state, uint8 speed)
{
	uint8 duty_cycle=0;
	duty_cycle=(uint8)((uint32)speed*255/100);
	PWM_Timer0_Start(duty_cycle);
	switch (state)
	{
		case stop:
			GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
			GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
			break;
		case CW:
			GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_LOW);
			GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_HIGH);
			break;
		case A_CW:
			GPIO_writePin(DC_MOTOR_IN1_PORT_ID,DC_MOTOR_IN1_PIN_ID,LOGIC_HIGH);
			GPIO_writePin(DC_MOTOR_IN2_PORT_ID,DC_MOTOR_IN2_PIN_ID,LOGIC_LOW);
	}
}
