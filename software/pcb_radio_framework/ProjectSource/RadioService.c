/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "RadioService.h"
#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;
uint8_t *buffer;

/*------------------------------ Module Code ------------------------------*/
bool InitRadioService(uint8_t Priority) {
    ES_Event_t ThisEvent;
    MyPriority = Priority;
    DB_printf("Init Radio Service\n");

    // ~{RST} - pin 25 - RB14 - output
    TRISBbits.TRISB14 = 0;
    // ~{SEN} - pin 24 - RB13 - output
    TRISBbits.TRISB13 = 0;

    InitI2C();
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

bool PostRadioService(ES_Event_t ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

ES_Event_t RunRadioService(ES_Event_t ThisEvent) {
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT;
    switch (ThisEvent.EventType) {
    case ES_INIT:
        {
            // set reset line low
            LATBbits.LATB14 = 0;
            // start timer for radio reset
            // ES_Timer_InitTimer(RADIO_TIMER, 100);
            break;
        }

    case ES_TIMEOUT:
        {
            if (ThisEvent.EventParam == RADIO_TIMER) {
                // set reset line high
                LATBbits.LATB14 = 1;

                // set ~{SEN} low to enable Si4735
                LATBbits.LATB13 = 1;

                PowerUp();
                // SetFrequency(9010);
                uint8_t bytes[1] = {0x10};
                uint8_t result[2];
                WriteRegister(bytes, 1, result, 2);
            }
            break;
        }
    }
    return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void InitI2C(void) {
    // SCL - pin 17 - RB8 - input
    TRISBbits.TRISB8 = 1;
    // SDA - pin 18 - RB9 - input
    TRISBbits.TRISB9 = 1;

    // set baud rate
    I2C1BRG = 0x0C6;

    // turn off I2C1 module
    I2C1CONbits.ON = 0;
    // continue I2C1 module operation in idle mode
    I2C1CONbits.SIDL = 0;
    // disable I2C reserved address rule since we only have one device
    I2C1CONbits.STRICT = 0;
    // select 7 bit address
    I2C1CONbits.A10M = 0;
    // enable slew rate control
    I2C1CONbits.DISSLW = 0;
    // disable SMBus input voltage thresholds
    I2C1CONbits.SMEN = 0;
    // turn on I2C1 module
    I2C1CONbits.ON = 1;
}

void WriteRegister(uint8_t *bytes, uint8_t n, uint8_t *result, uint8_t m) {
    // assert start condition 
    I2C1CONbits.SEN = 1;
    // wait for condition to be set
    while (I2C1CONbits.SEN);

    // write slave address
    I2C1TRN = WRITE | RADIO_ADDRESS;
    // wait for transmission to finish
    while (I2C1STATbits.TRSTAT);
    // check for acknowledgement from slave
    if (I2C1STATbits.ACKSTAT) {
        // handle NACK
    }

    for (uint8_t i = 0; i < n; i++) {
        // write data
        I2C1TRN = bytes[i];
        // wait for acknowledgement from slave
        while (I2C1STATbits.TRSTAT);
    }
    
    // assert repeated start condition
    I2C1CONbits.RSEN = 1;
    // wait for condition to be set
    while (I2C1CONbits.RSEN);

    // write slave address
    I2C1TRN = READ | RADIO_ADDRESS;
    // wait for acknowledgement from slave
    while (I2C1STATbits.TRSTAT);

    for (uint8_t j = 0; j < m; j++) {
        // enable master receive serial data
        I2C1CONbits.RCEN = 1;
        // wait for data
        while (!I2C1STATbits.RBF);
        // read data
        result[j] = I2C1RCV;
        if (j == m - 1) {
            // send NACK in acknowledgement sequence
            I2C1CONbits.ACKDT = 1;
        } else {
            // send ACK in acknowledgement sequence
            I2C1CONbits.ACKDT = 0;
        }
        // send acknowledgement sequence
        I2C1CONbits.ACKEN = 1;
        // wait for condition to be set
        while (I2C1CONbits.ACKEN);
    }

    // send stop bit
    I2C1CONbits.PEN = 1;
    // wait for condition to be set
    while (I2C1CONbits.PEN);
}

void WriteCommand(uint8_t *bytes, uint8_t n) {
    // assert start condition 
    I2C1CONbits.SEN = 1;
    // wait for condition to be set
    while (I2C1CONbits.SEN);

    for (uint8_t i = 0; i < n; i++) {
        // write data
        I2C1TRN = bytes[i];
        // wait for acknowledgement from slave
        while (I2C1STATbits.TRSTAT);
    }
    
    // send stop bit
    I2C1CONbits.PEN = 1;
    // wait for condition to be set
    while (I2C1CONbits.PEN);
}

void PowerUp(void) {
    uint8_t bytes[3];
    uint8_t result[1];
    bytes[0] = POWER_UP;
    bytes[1] = 0x10;
    bytes[2] = 0x05;
    WriteRegister(bytes, 3, result, 1);
}

void SetFrequency(uint16_t freq) {
    uint8_t bytes[5];
    uint8_t result[1];
    bytes[0] = FM_TUNE_FREQ;
    bytes[1] = 0x00;
    bytes[2] = (freq >> 8) & 0xFF;
    bytes[3] = freq & 0xFF;
    bytes[4] = 0x00;
    WriteRegister(bytes, 5, result, 1);
}

bool InitOLED(void) {
	if ((!buffer) && !(buffer = (uint8_t *)malloc(WIDTH * ((HEIGHT + 7) / 8)))) {
    	return false;
	}

	uint8_t bytes[8];
	bytes[0] = OLED_ADDRESS;
    bytes[1] = OLED_CONTROL;

    bytes[2] = OLED_DISPLAYOFF;
    bytes[3] = OLED_SETDISPLAYCLOCKDIV;
    bytes[4] = 0x80;
    bytes[5] = OLED_SETMULTIPLEX;
    WriteCommand(bytes, 6);

    bytes[2] = OLED_HEIGHT;
    WriteCommand(bytes, 3);

    bytes[2] = OLED_SETDISPLAYOFFSET;
    bytes[3] = 0x00;
    bytes[4] = OLED_SETSTARTLINE;
    bytes[5] = OLED_CHARGEPUMP;
    WriteCommand(bytes, 6);

    bytes[2] = 0x14;
    WriteCommand(bytes, 3);

    bytes[2] = OLED_MEMORYMODE;
    bytes[3] = 0x00;
    bytes[4] = OLED_SEGREMAP;
    bytes[5] = OLED_COMSCANDEC;
    WriteCommand(bytes, 6);

    bytes[2] = OLED_SETCOMPINS;
    WriteCommand(bytes, 3);

    bytes[2] = 0x12;
    WriteCommand(bytes, 3);

    bytes[2] = OLED_SETCONTRAST;
    WriteCommand(bytes, 3);

    bytes[2] = 0xCF;
    WriteCommand(bytes, 3);

    bytes[2] = OLED_SETPRECHARGE;
    WriteCommand(bytes, 3);

    bytes[2] = 0xF1;
    WriteCommand(bytes, 3);

    bytes[2] = OLED_SETVCOMDETECT;
    bytes[3] = 0x40;
    bytes[4] = OLED_DISPLAYALLON_RESUME;
    bytes[5] = OLED_NORMALDISPLAY;
    bytes[6] = OLED_DEACTIVATESCROLL;
    bytes[7] = OLED_DISPLAYON;
    WriteCommand(bytes, 8);

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
    WriteCommand(bytes, 6);

    bytes[2] = 0x00;
    WriteCommand(bytes, 3);

    bytes[2] = 0x7F;
    WriteCommand(bytes, 3);

	uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
	uint8_t *ptr = buffer;
	bytes[1] = 0x40;
	uint16_t bytesOut = 2;
	while (count--) {
		if (bytesOut >= MAX_BYTES) {
			WriteCommand(bytes, MAX_BYTES);
			bytesOut = 2;
		}
		bytes[bytesOut] = *ptr++;
		bytesOut++;
	}
	WriteCommand(bytes, bytesOut);
}