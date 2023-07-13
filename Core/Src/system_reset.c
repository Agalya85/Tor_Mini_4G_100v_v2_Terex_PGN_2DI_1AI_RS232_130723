

#include "system_reset.h"
#include "main.h"


/****************************************************************************
 Function: systemReset
 Purpose: Soft Reset for system

 Input:	None
 Return value: None


 Note(s)(if-any) :
 This will trigger system IDWT.

 Change History:
 Author            	Date                Remarks
 KloudQ Team        23/05/2020			initial definition
******************************************************************************/
void systemReset(void)
{
	while(1)
	{
		HAL_GPIO_TogglePin(GPS_LED_GPIO_Port, GPS_LED_Pin);
		HAL_Delay(50);
	}
}
