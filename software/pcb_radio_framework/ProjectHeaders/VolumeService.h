
#ifndef VolumeService_H
#define VolumeService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitVolumeService(uint8_t Priority);
bool PostVolumeService(ES_Event_t ThisEvent);
ES_Event_t RunVolumeService(ES_Event_t ThisEvent);

#endif /* ServTemplate_H */

