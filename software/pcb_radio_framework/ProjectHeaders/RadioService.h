
#ifndef RadioService_H
#define RadioService_H

#include "ES_Types.h"

#define READ		0x01
#define WRITE		0x00
#define ADDRESS 	0x63 << 1
#define POWER_UP	0x01

// Public Function Prototypes

bool InitRadioService(uint8_t Priority);
bool PostRadioService(ES_Event_t ThisEvent);
ES_Event_t RunRadioService(ES_Event_t ThisEvent);

#endif /* ServTemplate_H */

