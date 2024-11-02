/*Include drivers*/
#include "pir.h"
#include "dc_motor.h"
#include "buzzer.h"
#include "external_eeprom.h"
#include "timer.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <util/delay.h>

//Create flags by using bitfield
struct my_bitfield
{
	uint8 start :1; //used to print the step massage one time instead of clear and print
	uint8 error :1; //used to detect error in password match
	uint8 option :1; //used to detect the selected option if 0:open the door / if 1:change password
	uint8 last_chanse :1; //used to avoid writing password again after last chance
	uint8 wait :1; //used to wait until time ends
	uint8 :3;
}flags;

//Needed variable
uint8 password[10]; //10 for 10 digits

//Function Deceleration
void Check_password (uint8 No_of_digits);
void Waiting (void);
void Door_open(void);

//main Function
int main (void)
{
	/* Enable Global Interrupt I-Bit */
	SREG |= (1<<7);

	/* intial values */
	flags.start=1;

	/*call init functions*/
	Buzzer_init();
	PIRSensor_init();
	DcMotor_Init();

	/* Create configuration structure for ICU driver */
	UART_ConfigType UART_Configurations = {EGIGHT,EVEN,ONE,38400};
	UART_init(&UART_Configurations);

	/*Get the start flag from EEPROM from control ECU
	 * if = 0 this means that the password is saved from last time
	 * & get the stored password instead of read it each time from eeprom
	 * this can't be done on proteus*/


	while(1)
	{
		//Create password
		if(flags.start)
		{
			Check_password (10);
			if(flags.error==0)
			{
				//close the start flag
				flags.start=0;
				//send password to eeprom
				for(uint8 count=0;count<5;count++)
				{
					EEPROM_writeByte((uint16)count, password[count]);
					_delay_ms(10);
				}
			}
		}
		//password is created
		else
		{
			//Wait to get the option
			flags.option = (UART_recieveByte() & 0x01);
			//Check the password
			//insert stored password in last 5 digits in array
			for(uint8 count=0;count<5;count++)
			{
				EEPROM_readByte(count,password+count+5);
			}
			//check if matches
			Check_password (5);
			//password is correct
			if(flags.error==0)
			{
				//change password option
				if(flags.option)
				{
					flags.start=1;
				}
				//open the door option
				else
				{
					Door_open();
				}
			}
			else
			{
				//check password for last time
				Check_password (5);
				//password is wrong
				if(flags.error)
				{
					//Turn on buzzer for 1 min
					Buzzer_on();
					for(uint8 count=0;count<60;count++)
					{
						_delay_ms(1000);
					}
					Buzzer_off();
				}
				else
				{
					//change password option
					if(flags.option)
					{
						flags.start=1;
					}
					//open the door option
					else
					{
						Door_open();
					}
				}
			}
		}
	}
}

void Check_password (uint8 No_of_digits)
{
	//clear error flag if happens
	flags.error=0;
	for(uint8 count=0;count<No_of_digits;count++)
	{
		password[count]=UART_recieveByte();
	}
	//check if first 5 digits = second ones
	for(uint8 count=0;count<5;count++)
	{
		if(password[count] != password[count+5])
		{
			flags.error=1;
			break;
		}
	}
	if(flags.error)
	{
		//send failure to HMI ECU
		UART_sendByte(0x00);
	}
	else
	{
		//send success to HMI ECU
		UART_sendByte(0x01);
	}
}

void Waiting (void)
{
	static uint8 tic =0;
	tic++;
	if(tic==2)
	{
		flags.wait=0;
		tic=0;
		//close the timer
		Timer_deInit(TIMER1);
	}
}

void Door_open(void)
{
	//Turn on dc motor cw for 15 sec
	DcMotor_Rotate(CW,100);
	//initialize timer to wait 15 sec
	flags.wait=1;
	//Initialize timer to wait 15 sec and set the callback function
	{
		Timer_ConfigType TIMER_Configurations = {0,58593,TIMER1,F_CPU_1024,CTC};
		Timer_init(&TIMER_Configurations);
		Timer_setCallBack(Waiting,TIMER1);
	}
	while(flags.wait);
	//Stop the motor
	DcMotor_Rotate(stop,100);
	//wait will pir is reading
	while(PIR_getState());
	//Send anything to HMI ECU to start closing
	UART_sendByte(0x00);
	//closing the door
	//Turn on dc motor A-cw for 15 sec
	DcMotor_Rotate(A_CW,100);
	//initialize timer to wait 15 sec
	flags.wait=1;
	//Initialize timer to wait 15 sec and set the callback function
	{
		Timer_ConfigType TIMER_Configurations = {0,58593,TIMER1,F_CPU_1024,CTC};
		Timer_init(&TIMER_Configurations);
		Timer_setCallBack(Waiting,TIMER1);
	}
	while(flags.wait);
	//Stop the motor
	DcMotor_Rotate(stop,100);
}
