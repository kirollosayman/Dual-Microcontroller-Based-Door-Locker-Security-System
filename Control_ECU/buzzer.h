#ifndef ECAL_BUZZER_H_
#define ECAL_BUZZER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define BUZZER_PORT_ID                 PORTC_ID
#define BUZZER_PIN_ID                  PIN7_ID


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * nitializes the buzzer pin direction and turn off the buzzer.
*/
void Buzzer_init(void);

/*
 * Description :
 Activates the buzzer.
*/
void Buzzer_on(void);

/*
 * Description :
 Deactivates the buzzer.
*/
void Buzzer_off(void);

#endif /* ECAL_BUZZER_H_ */
