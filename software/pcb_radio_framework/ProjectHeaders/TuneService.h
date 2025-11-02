
#ifndef TuneService_H
#define TuneService

#include "ES_Types.h"

// Public Function Prototypes

bool InitTuneService(uint8_t Priority);
bool PostTuneService(ES_Event_t ThisEvent);
ES_Event_t RunTuneService(ES_Event_t ThisEvent);

#endif /* ServTemplate_H */

