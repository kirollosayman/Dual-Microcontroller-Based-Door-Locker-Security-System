#ifndef PIR_H_
#define PIR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PIR_DATA_PORT_ID                 PORTC_ID
#define PIR_DATA_PIN_ID                  PIN2_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Initializes the pir sensor pin direction.
*/
void PIRSensor_init(void);

/*
 * Description :
 * Reads the value from the pir sensor and returns it.
*/
uint8 PIR_getState(void);

#endif /* PIR_H_ */
