
#ifndef RadioService_H
#define RadioService_H

#include "ES_Types.h"

#define READ			0x01
#define WRITE			0x00
#define RADIO_ADDRESS 	0x63 << 1
#define POWER_UP		0x01
#define FM_TUNE_FREQ	0x20

#define MAX_BYTES					32
#define HEIGHT 						64
#define WIDTH						128

#define OLED_ADDRESS				0x3D << 1
#define OLED_CONTROL				0x00
#define OLED_DISPLAYOFF				0xAE
#define OLED_SETDISPLAYCLOCKDIV		0xD5
#define OLED_SETMULTIPLEX			0xA8
#define OLED_HEIGHT					0x3F
#define OLED_SETDISPLAYOFFSET		0xD3
#define OLED_SETSTARTLINE			0x40
#define OLED_CHARGEPUMP				0x8D
#define OLED_MEMORYMODE				0x20
#define OLED_SEGREMAP				0xA1
#define OLED_COMSCANDEC				0xC8
#define OLED_SETCOMPINS				0xDA
#define OLED_SETCONTRAST			0x81
#define OLED_SETPRECHARGE			0xD9
#define OLED_SETVCOMDETECT			0xDB
#define OLED_DISPLAYALLON_RESUME	0xA4
#define OLED_NORMALDISPLAY			0xA6
#define OLED_DEACTIVATESCROLL		0x2E
#define OLED_DISPLAYON 				0xAF
#define OLED_PAGEADDR				0x22
#define OLED_COLUMNADDR				0x21

// Public Function Prototypes

bool InitRadioService(uint8_t Priority);
bool PostRadioService(ES_Event_t ThisEvent);
ES_Event_t RunRadioService(ES_Event_t ThisEvent);

void InitI2C(void);
void WriteRegister(uint8_t *bytes, uint8_t n, uint8_t *result, uint8_t m);

void PowerUp(void);
void SetFrequency(uint16_t freq);

bool InitOLED(void);
void WriteCommand(uint8_t *bytes, uint8_t n);
void UpdateOLED(void);
void ClearOLED(void);

#endif /* ServTemplate_H */

