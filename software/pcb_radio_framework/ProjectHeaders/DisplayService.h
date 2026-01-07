/****************************************************************************

  Header file for template service
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef DisplayService_H
#define DisplayService_H

#include "ES_Types.h"

#define MAX_BYTES					32
#define HEIGHT						64
#define WIDTH						128

#define OLED_ADDRESS				      0x3D << 1
#define OLED_CONTROL				      0x00
#define OLED_DISPLAYOFF       		0xAE
#define OLED_SETDISPLAYCLOCKDIV   0xD5
#define OLED_SETMULTIPLEX     		0xA8
#define OLED_HEIGHT         		  0x3F
#define OLED_SETDISPLAYOFFSET   	0xD3
#define OLED_SETSTARTLINE     		0x40
#define OLED_CHARGEPUMP       		0x8D
#define OLED_MEMORYMODE       		0x20
#define OLED_SEGREMAP       		  0xA1
#define OLED_COMSCANDEC       		0xC8
#define OLED_SETCOMPINS       		0xDA
#define OLED_SETCONTRAST      		0x81
#define OLED_SETPRECHARGE     		0xD9
#define OLED_SETVCOMDETECT      	0xDB
#define OLED_DISPLAYALLON_RESUME  0xA4
#define OLED_NORMALDISPLAY      	0xA6
#define OLED_DEACTIVATESCROLL   	0x2E
#define OLED_DISPLAYON        		0xAF
#define OLED_PAGEADDR       		  0x22
#define OLED_COLUMNADDR       		0x21

// Public Function Prototypes

bool InitDisplayService(uint8_t Priority);
bool PostDisplayService(ES_Event_t ThisEvent);
ES_Event_t RunDisplayService(ES_Event_t ThisEvent);

bool InitOLED(void);
void UpdateOLED(void);
void ClearOLED(void);
void UpdateBuffer(char *line1, char *line2, char *line3, uint8_t n, uint8_t m, uint8_t o);
void UpdateVolume(char *vol);
void UpdateFrequency(char *freq);

bool getUpdateStatus(void);

#endif /* ServTemplate_H */

