
#ifndef RadioService_H
#define RadioService_H

#include "ES_Types.h"
#include "RadioService.h"

#define READ			0x01
#define WRITE			0x00
#define RADIO_ADDRESS 	0x11 << 1
#define POWER_UP		0x01
#define GET_REV			0x10
#define SET_PROPERTY	0x12
#define GET_PROPERTY	0x13
#define GET_INT_STATUS	0x14
#define FM_TUNE_FREQ	0x20
#define FM_SEEK_START	0x21
#define FM_TUNE_STATUS	0x22
#define RX_VOLUME_H		0x40
#define RX_VOLUME_L		0x00

// Public Function Prototypes

bool InitRadioService(uint8_t Priority);
bool PostRadioService(ES_Event_t ThisEvent);
ES_Event_t RunRadioService(ES_Event_t ThisEvent);

void InitI2C(void);
void WriteAndReadRegister(uint8_t *bytes, uint8_t n, uint8_t *result, uint8_t m);
void WriteRegister(uint8_t *bytes, uint8_t n);
void ReadRegister(uint8_t *result, uint8_t n);

void PowerUp(void);
void SetRadioFrequency(uint16_t freq);
void SetVolume(uint8_t vol);
void delay(int n);
bool GetPoweredUp(void);

#endif /* ServTemplate_H */

