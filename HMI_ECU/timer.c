#include "timer.h"
#include "avr/io.h" /* To use the timer Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/interrupt.h> /* For ICU ISR */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*Timer0_callBackPtr)(void) = NULL_PTR;
static volatile void (*Timer1_callBackPtr)(void) = NULL_PTR;
static volatile void (*Timer2_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect)
{
	if(Timer0_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*Timer0_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER0_COMP_vect)
{
	if(Timer0_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*Timer0_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

ISR(TIMER2_OVF_vect)
{
	if(Timer2_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*Timer2_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER2_COMP_vect)
{
	if(Timer2_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*Timer2_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

ISR(TIMER1_OVF_vect)
{
	if(Timer1_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*Timer1_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER1_COMPA_vect)
{
	if(Timer1_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*Timer1_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 *  Function to initialize the Timer driver
 */
void Timer_init(const Timer_ConfigType * Config_Ptr)
{
	switch(Config_Ptr->timer_ID)
	{
		case 0:
			//clear all the register to be ready for configuration
			//Set pin FOC0 as we will not use pwm mode
			TCCR0 = (1<<FOC0);
			//Choose the mode
			switch(Config_Ptr->timer_mode)
			{
				case 0:
					break;
				case 1:
					TCCR0 |= (1<<WGM01);
					break;
			}
			//Set the clock
			TCCR0 = (TCCR0 & 0xF8) | Config_Ptr->timer_clock;
			//Set initial counter
			TCNT0 = (Config_Ptr->timer_InitialValue & 0x00FF);
			//Set Compare register
			OCR0 = (Config_Ptr->timer_compare_MatchValue & 0x00FF);
			//Enable interrupt
			TIMSK &= ~(1<< TOIE0) & ~(1<< OCIE0);
			switch(Config_Ptr->timer_mode)
			{
				case 0:
					TIMSK |= (1<< TOIE0);
					break;
				case 1:
					TIMSK |= (1<< OCIE0);
					break;
			}
			break;

		case 1:
			//clear all the register to be ready for configuration
			//Set pin FOC1A & FOC1B as we will not use pwm mode
			TCCR1A = (1<<FOC1A) | (1<<FOC1B);
			//Choose the mode
			switch(Config_Ptr->timer_mode)
			{
				case 0:
					TCCR1B &= ~(1<<WGM12);
					break;
				case 1:
					TCCR1B |= (1<<WGM12);
					break;
			}
			//Set the clock
			TCCR1B = (TCCR1B & 0xF8) | Config_Ptr->timer_clock;
			//Set initial counter
			TCNT1 = Config_Ptr->timer_InitialValue;
			//Set Compare register
			OCR1A = Config_Ptr->timer_compare_MatchValue;
			//Enable interrupt
			TIMSK &= ~(1<< TOIE1) & ~(1<< OCIE1A);
			switch(Config_Ptr->timer_mode)
			{
				case 0:
					TIMSK |= (1<< TOIE1);
					break;
				case 1:
					TIMSK |= (1<< OCIE1A);
					break;
			}
			break;

		case 2:
			//clear all the register to be ready for configuration
			//Set pin FOC2 as we will not use pwm mode
			TCCR2 = (1<<FOC2);
			//Choose the mode
			switch(Config_Ptr->timer_mode)
			{
				case 0:
					break;
				case 1:
					TCCR2 |= (1<<WGM21);
					break;
			}
			//Set the clock
			TCCR2 = (TCCR2 & 0xF8) | Config_Ptr->timer_clock;
			//Set initial counter
			TCNT2 = (Config_Ptr->timer_InitialValue & 0x00FF);
			//Set Compare register
			OCR2 = (Config_Ptr->timer_compare_MatchValue & 0x00FF);
			//Enable interrupt
			TIMSK &= ~(1<< TOIE2) & ~(1<< OCIE2);
			switch(Config_Ptr->timer_mode)
			{
				case 0:
					TIMSK |= (1<< TOIE2);
					break;
				case 1:
					TIMSK |= (1<< OCIE2);
					break;
			}
			break;
	}
}

/*
 * Description :
 *  Function to disable the Timer via Timer_ID.
 */
void Timer_deInit(Timer_ID_Type timer_type)
{
	switch(timer_type)
	{
		case 0:
			TCCR0 = (TCCR2 & 0xF8) | NO_CLOCK;
			break;
		case 1:
			TCCR1B = (TCCR1B & 0xF8) | NO_CLOCK;
			break;
		case 2:
			TCCR2 = (TCCR2 & 0xF8) | NO_CLOCK;
			break;
	}
}

/*
 * Description :
 *  Function to set the Call Back function address to the required Timer.
 */
void Timer_setCallBack(void(*a_ptr)(void), Timer_ID_Type a_timer_ID )
{
	switch(a_timer_ID)
	{
		case 0:
			Timer0_callBackPtr = a_ptr;
			break;
		case 1:
			Timer1_callBackPtr = a_ptr;
			break;
		case 2:
			Timer2_callBackPtr = a_ptr;
			break;
	}
}
