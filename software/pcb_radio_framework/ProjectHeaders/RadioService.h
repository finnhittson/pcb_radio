
#ifndef RadioService_H
#define RadioService_H

#include "ES_Types.h"

#define READ			0x01
#define WRITE			0x00
#define ADDRESS 		0x63 << 1
#define POWER_UP		0x01
#define FM_TUNE_FREQ	0x20

// Public Function Prototypes

bool InitRadioService(uint8_t Priority);
bool PostRadioService(ES_Event_t ThisEvent);
ES_Event_t RunRadioService(ES_Event_t ThisEvent);

void InitI2C(void);
void WriteRegister(uint8_t *bytes, uint8_t n, uint8_t *result, uint8_t m);

void PowerUp(void);
void SetFrequency(uint16_t freq);	

#endif /* ServTemplate_H */

