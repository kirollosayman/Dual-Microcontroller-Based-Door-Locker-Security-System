/*Include drivers*/
#include "keypad.h"
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <util/delay.h>

//Create flags by using bitfield
struct my_bitfield
{
	uint8 lcd :1; //used to print the step massage one time instead of clear and print
	uint8 check :1; //used to check if password is correct
	uint8 last_chance :1; //used to avoid writing password again after last chance
	uint8 wait :1; //used to wait until time ends
	uint8 :5;
}flags;

//Needed variable
uint8 step=1;
uint8 prev_step;
uint8 key;
uint8 password[10]; //10 for 10 digits

//Function deceleration
void Enter_password(uint8 Shift,uint8 store);
void Send_password(uint8 No_of_digits);
uint8 Check_password(uint8 Shift,uint8 store);
void Waiting (void);

//main Function
int main (void)
{
	/* Enable Global Interrupt I-Bit */
	SREG |= (1<<7);

	/* intial values */
	flags.lcd=1;
	flags.last_chance=0;
	/*call init functions*/
	LCD_init();
	/* Create configuration structure for ICU driver */
	UART_ConfigType UART_Configurations = {EGIGHT,EVEN,ONE,38400};
	UART_init(&UART_Configurations);

	/*Get the step from EEPROM from control ECU
	 * if not = 1 this means that the password is saved from last time
	 * this can't be done on proteus*/


	while(1)
	{
		switch (step)
		{
			// Create a system password
			case 1:
				//Enter password for first time
				//LCD clear screen & write the massage
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Plz Enter pass:");
				Enter_password(0,0);
				//Re-enter the password
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Plz Re-Enter the");
				LCD_displayStringRowColumn(1,0,"same pass:");
				Enter_password(10,5);

				//Send the array to the ECU to check
				Send_password(10);
				//Check from control ECU
				if(UART_recieveByte())
				{
					step=2;
				}
				break;

			// Main option
			case 2:
				if(flags.lcd)
				{
					//LCD clear screen & write the massage
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"+ : Open Door");
					LCD_displayStringRowColumn(1,0,"- : Change pass");
					flags.lcd=0;
				}
				key = KEYPAD_getPressedKey();
				_delay_ms(500);
				switch(key)
				{
					case '+':
						UART_sendByte(0x00);
						step=3;
						flags.lcd=1;
						break;
					case '-':
						UART_sendByte(0x01);
						step=4;
						flags.lcd=1;
						break;
				}
				break;

			// open the door
			case 3:
				//LCD clear screen & write the massage
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Plz Enter pass:");
				//password correct
				if(flags.last_chance || Check_password(0,0))
				{
					//clear the flag if used
					flags.last_chance=0;
					//print door unlocking massage
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"Door Unlocking");
					LCD_displayStringRowColumn(1,0,"Please wait");
					//initialize timer to wait 15 sec
					flags.wait=1;
					//Initialize timer to wait 15 sec and set the callback function
					{
						Timer_ConfigType TIMER_Configurations = {0,58593,TIMER1,F_CPU_1024,CTC};
						Timer_init(&TIMER_Configurations);
						Timer_setCallBack(Waiting,TIMER1);
					}
					while(flags.wait);
					//print wait for people until receive from control ECU to close
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"Wait for people");
					LCD_displayStringRowColumn(1,0,"To Enter");
					UART_recieveByte();
					//print door locking massage
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"Door Locking");
					LCD_displayStringRowColumn(1,0,"Please wait");
					//initialize timer to wait 15 sec
					flags.wait=1;
					//Initialize timer to wait 15 sec and set the callback function
					{
						Timer_ConfigType TIMER_Configurations = {0,58593,TIMER1,F_CPU_1024,CTC};
						Timer_init(&TIMER_Configurations);
						Timer_setCallBack(Waiting,TIMER1);
					}
					while(flags.wait);
					//Go to step 2
					step=2;
				}
				//password is not correct
				else
				{
					prev_step=step;
					step=5;
				}
				break;
			// change password
			case 4:
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Plz Enter old");
				LCD_displayStringRowColumn(1,0,"pass: ");
				//password correct
				if(flags.last_chance || Check_password(6,0))
				{
					//clear the flag if used
					flags.last_chance=0;
					step=1;
				}
				//password is not correct
				else
				{
					prev_step=step;
					step=5;
				}
				break;
			// if password entered wrong
			case 5:
				//Ask for password last time
				flags.last_chance=0;
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Plz Enter pass");
				LCD_displayStringRowColumn(1,0,"last Try: ");
				if(Check_password(10,0))
				{
					step=prev_step;
					flags.last_chance=1;
				}
				//password is not correct
				else
				{
					//play warning massage for 1 min
					//LCD clear screen & write the massage & open warning flag
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"System LOCKED!");
					LCD_displayStringRowColumn(1,0,"Wait 1 min");
					for(uint8 count=0;count<60;count++)
					{
						_delay_ms(1000);
					}
					step=2;
				}
				break;
		}
	}
}

//This function take shift to shift * mark and store to store the password
void Enter_password(uint8 Shift,uint8 store)
{
	LCD_moveCursor(1,Shift);
	for(sint8 input_count=0;input_count<6;input_count++)
	{
		//Get pressed key
		key = KEYPAD_getPressedKey();
		_delay_ms(500); /* Press time */
		//if back space
		if(key == 'B')
		{
			if(input_count>0)
			{
				//remove the * mark and return to write again
				LCD_moveCursor(1,Shift+input_count-1);
				LCD_displayCharacter(' ');
				LCD_moveCursor(1,Shift+input_count-1);
				input_count-=2;
			}
			else
			{
				input_count--;
			}
		}
		else
		{
			if(input_count<5)
			{
				if(key <= 9)
				{
					password[store+input_count] = key;
					LCD_displayCharacter('*');
				}
				else
				{
					input_count--;
				}
			}
			else
			{
				if(key == 'E')
				{
					break;
				}
				else
				{
					input_count--;
				}
			}
		}
	}
}

void Send_password(uint8 No_of_digits)
{
	for(uint8 send_count=0;send_count<No_of_digits;send_count++)
	{
		UART_sendByte(password[send_count]);
	}
}

uint8 Check_password(uint8 Shift,uint8 store)
{
	uint8 val;
	Enter_password(Shift,store);
	//Send the array to the ECU to check
	Send_password(5);
	val=UART_recieveByte();
	return val;
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
