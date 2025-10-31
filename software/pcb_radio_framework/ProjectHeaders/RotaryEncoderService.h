
#ifndef RotaryEncoderService_H
#define RotaryEncoderService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitRotaryEncoderService(uint8_t Priority);
bool PostRotaryEncoderService(ES_Event_t ThisEvent);
ES_Event_t RunRotaryEncoderService(ES_Event_t ThisEvent);

#endif /* ServTemplate_H */

