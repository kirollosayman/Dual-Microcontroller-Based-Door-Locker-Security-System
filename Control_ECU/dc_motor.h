#ifndef ECAL_DC_MOTOR_H_
#define ECAL_DC_MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
//Motor states
typedef enum
{
	stop,CW,A_CW
}DcMotor_State;
// Motor speed
extern uint8 speed;
extern DcMotor_State state;

#define DC_MOTOR_IN1_PORT_ID                 PORTD_ID
#define DC_MOTOR_IN1_PIN_ID                  PIN6_ID

#define DC_MOTOR_IN2_PORT_ID                 PORTD_ID
#define DC_MOTOR_IN2_PIN_ID                  PIN7_ID

#define DC_MOTOR_EN_PORT_ID                  PORTB_ID
#define DC_MOTOR_EN_PIN_ID                   PIN3_ID



/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Initializes all DC motor pins direction.
   Turn off the motor at start
*/
void DcMotor_Init(void);

/*
 * Description :
 * Controls the motor's state (Clockwise/Anti-Clockwise/Stop) and adjusts the speed based
on the input duty cycle
*/
void DcMotor_Rotate(DcMotor_State state, uint8 speed);

#endif /* ECAL_DC_MOTOR_H_ */
