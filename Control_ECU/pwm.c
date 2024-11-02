#include "avr/io.h" /* To use the Timer 0 Registers */
#include "pwm.h"
#include "common_macros.h" /* To use the macros like SET_BIT */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void PWM_Timer0_Start(uint8 duty_cycle)
{
	// initialize fast pwm mode
	SET_BIT(TCCR0, WGM00);
	SET_BIT(TCCR0, WGM01);
	//Set prescaler for 64
	SET_BIT(TCCR0, CS00);
	SET_BIT(TCCR0, CS01);
	//Non-inverting mode
	SET_BIT(TCCR0, COM01);
	//Set the duty cycle
	OCR0= duty_cycle;
}
