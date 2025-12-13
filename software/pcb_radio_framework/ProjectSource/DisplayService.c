/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "DisplayService.h"
#include "dbprintf.h"
#include "font8x8_basic.h"
#include "RadioService.h"
#include <string.h>
#include <stdio.h>

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;
uint8_t *buffer;
char line1[16];
char line2[16];
char line3[16];
volatile uint8_t n = 16;
volatile uint8_t m = 16;
volatile uint8_t o = 16;
bool updating = false;

/*------------------------------ Module Code ------------------------------*/
bool InitDisplayService(uint8_t Priority) {
    ES_Event_t ThisEvent;
    MyPriority = Priority;
    DB_printf("Init display service.\n");
    
    if (InitOLED()) {
        DB_printf("Display initialized correctly.\n");
    } else {
        DB_printf("Unable to allocate enough space for display buffer.\n");
    }
    
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == true) {
        return true;
    } else {
        return false;
    }
}

bool PostDisplayService(ES_Event_t ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

ES_Event_t RunDisplayService(ES_Event_t ThisEvent) {
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT;
    switch (ThisEvent.EventType) {
    case ES_INIT:
        {
            DB_printf("display on\n");
            strcpy(line1, "      PIG");
            snprintf(line2, sizeof(line2), "Vol:  %d oinks", 20);
            snprintf(line3, sizeof(line3), "Freq: %d.%d", 9010 / 100, (9010 / 10) % 10);
            UpdateBuffer(line1, line2, line3, 16, 16, 16);
            break;
        }

    case ES_UPDATE_VOL:
        {
            updating = true;
            uint8_t vol = (uint8_t)ThisEvent.EventParam;
            snprintf(line2, sizeof(line2), "Vol:  %d oinks", vol);
            char vol_str[8];
            snprintf(vol_str, sizeof(vol_str), "%d", vol);
            UpdateVolume(vol_str);
            updating = false;
            break;
        }

    case ES_UPDATE_FREQ:
        {
            updating = true;
            uint16_t freq = ThisEvent.EventParam;
            snprintf(line3, sizeof(line3), "Freq: %d.%d", freq / 100, (freq / 10) % 10);
            char freq_str[20];
            snprintf(freq_str, sizeof(freq_str), "%d.%d", freq / 100, (freq / 10) % 10);
            UpdateFrequency(freq_str);
            updating = false;
            break;
        }
    }
    return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
bool InitOLED(void) {
    if (!buffer) {
        buffer = (uint8_t *)malloc(WIDTH * ((HEIGHT + 7) / 8));
        if (!buffer) {
            return false;
        }
    }

    uint8_t bytes[8];
    bytes[0] = OLED_ADDRESS;
    bytes[1] = OLED_CONTROL;

    bytes[2] = OLED_DISPLAYOFF;
    bytes[3] = OLED_SETDISPLAYCLOCKDIV;
    bytes[4] = 0x80;
    bytes[5] = OLED_SETMULTIPLEX;
    WriteRegister(bytes, 6);

    bytes[2] = OLED_HEIGHT;
    WriteRegister(bytes, 3);

    bytes[2] = OLED_SETDISPLAYOFFSET;
    bytes[3] = 0x00;
    bytes[4] = OLED_SETSTARTLINE;
    bytes[5] = OLED_CHARGEPUMP;
    WriteRegister(bytes, 6);

    bytes[2] = 0x14;
    WriteRegister(bytes, 3);

    bytes[2] = OLED_MEMORYMODE;
    bytes[3] = 0x00;
    bytes[4] = OLED_SEGREMAP;
    bytes[5] = OLED_COMSCANDEC;
    WriteRegister(bytes, 6);

    bytes[2] = OLED_SETCOMPINS;
    WriteRegister(bytes, 3);

    bytes[2] = 0x12;
    WriteRegister(bytes, 3);

    bytes[2] = OLED_SETCONTRAST;
    WriteRegister(bytes, 3);

    bytes[2] = 0xCF;
    WriteRegister(bytes, 3);

    bytes[2] = OLED_SETPRECHARGE;
    WriteRegister(bytes, 3);

    bytes[2] = 0xF1;
    WriteRegister(bytes, 3);

    bytes[2] = OLED_SETVCOMDETECT;
    bytes[3] = 0x40;
    bytes[4] = OLED_DISPLAYALLON_RESUME;
    bytes[5] = OLED_NORMALDISPLAY;
    bytes[6] = OLED_DEACTIVATESCROLL;
    bytes[7] = OLED_DISPLAYON;
    WriteRegister(bytes, 8);

    return true;
}

void UpdateOLED(void) {
    uint8_t bytes[MAX_BYTES];
    bytes[0] = OLED_ADDRESS;
    bytes[1] = OLED_CONTROL;

    bytes[2] = OLED_PAGEADDR;
    bytes[3] = 0x00;
    bytes[4] = 0xFF;
    bytes[5] = OLED_COLUMNADDR;
    WriteRegister(bytes, 6);

    bytes[2] = 0x00;
    WriteRegister(bytes, 3);

    bytes[2] = 0x7F;
    WriteRegister(bytes, 3);

    uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
    uint8_t *ptr = buffer;
    bytes[1] = 0x40;
    uint16_t bytesOut = 2;
    while (count--) {
        if (bytesOut >= MAX_BYTES) {
            WriteRegister(bytes, MAX_BYTES);
            bytesOut = 2;
        }
        bytes[bytesOut] = *ptr++;
        bytesOut++;
    }
    WriteRegister(bytes, bytesOut);
}

void ClearOLED(void) {
    memset(buffer, 0, WIDTH * ((HEIGHT + 7) / 8));
}

void UpdateBuffer(char *line1, char *line2, char *line3, uint8_t n, uint8_t m, uint8_t o) {
    ClearOLED();
    for (uint8_t i = 0; i < n; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            buffer[j + 8 * i + 128 * 0] = font8x8_basic[line1[i]][j];
        }
    }
    
    for (uint8_t i = 0; i < m; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            buffer[j + 8 * i + 128 * 2] = font8x8_basic[line2[i]][j];
        }
    }

    for (uint8_t i = 0; i < o; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            buffer[j + 8 * i + 128 * 4] = font8x8_basic[line3[i]][j];
        }
    }
    UpdateOLED();
    DB_printf("bruh\n");
}

bool getUpdateStatus(void) {
    return updating;
}

void UpdateVolume(char *vol) {
    uint8_t bytes[MAX_BYTES];
    bytes[0] = OLED_ADDRESS;
    bytes[1] = OLED_CONTROL;

    bytes[2] = OLED_PAGEADDR;
    bytes[3] = 0x02;
    bytes[4] = 0xFF;
    bytes[5] = OLED_COLUMNADDR;
    WriteRegister(bytes, 6);

    bytes[2] = 0x30;
    WriteRegister(bytes, 3);

    bytes[2] = 0x7F;
    WriteRegister(bytes, 3);

    bytes[1] = 0x40;

    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            bytes[2 + 8 * i + j] = font8x8_basic[vol[i]][j];
        }
    }
    WriteRegister(bytes, 2 + 2 * 8);
}

void UpdateFrequency(char *freq) {
    uint8_t bytes[MAX_BYTES];
    bytes[0] = OLED_ADDRESS;
    bytes[1] = OLED_CONTROL;

    bytes[2] = OLED_PAGEADDR;
    bytes[3] = 0x04;
    bytes[4] = 0xFF;
    bytes[5] = OLED_COLUMNADDR;
    WriteRegister(bytes, 6);

    bytes[2] = 0x30;
    WriteRegister(bytes, 3);

    bytes[2] = 0x7F;
    WriteRegister(bytes, 3);

    bytes[1] = 0x40;
    uint8_t bytecount = 2;
    for (uint8_t i = 0; i < 5; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            if (bytecount >= MAX_BYTES) {
                WriteRegister(bytes, MAX_BYTES);
                bytecount = 2;
            }
            bytes[bytecount] = font8x8_basic[freq[i]][j];
            bytecount++;
        }
    }
    WriteRegister(bytes, bytecount);
}