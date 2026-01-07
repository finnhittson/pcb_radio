/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef NeoPixelService_H
#define NeoPixelService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitNeoPixelService(uint8_t Priority);
bool PostNeoPixelService(ES_Event_t ThisEvent);
ES_Event_t RunNeoPixelService(ES_Event_t ThisEvent);

void SendColor(uint8_t r, uint8_t g, uint8_t b);

#endif /* ServTemplate_H */

