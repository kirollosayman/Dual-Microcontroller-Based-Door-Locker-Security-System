#include "pir.h"
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Initializes the pir sensor pin direction.
*/
void PIRSensor_init(void)
{
	//Initializes the pir sensor pin direction
	GPIO_setupPinDirection(PIR_DATA_PORT_ID,PIR_DATA_PIN_ID,PIN_INPUT);
}

/*
 * Description :
 * Reads the value from the pir sensor and returns it.
*/
uint8 PIR_getState(void)
{
	return GPIO_readPin(PIR_DATA_PORT_ID,PIR_DATA_PIN_ID);
}
