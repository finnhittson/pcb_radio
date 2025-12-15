/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "RadioService.h"
#include "dbprintf.h"
#include <string.h>
#include "DisplayService.h"
#include "TuneService.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;
bool poweredUp = false;

/*------------------------------ Module Code ------------------------------*/
bool InitRadioService(uint8_t Priority) {
    ES_Event_t ThisEvent;
    MyPriority = Priority;
    DB_printf("Init Radio Service\n");

    // ~{RST} - pin 25 - RB14 - output
    TRISBbits.TRISB14 = 0;
    // set ~{RST} pin high
    LATBbits.LATB14 = 1;

    // ~{SEN} - pin 24 - RB13 - output
    TRISBbits.TRISB13 = 0;
    // set ~{SEN} LOW to set 0x11 address
    LATBbits.LATB13 = 0;

    InitI2C();
    DB_printf("I2C initialized.\n");

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
            // set ~{RST} line low
            LATBbits.LATB14 = 0;
            // start timer for radio reset
            ES_Timer_InitTimer(RADIO_TIMER, 100);
            break;
        }

    case ES_TIMEOUT:
        {
            if (ThisEvent.EventParam == RADIO_TIMER && !poweredUp) {
                // set ~{RST} line high
                LATBbits.LATB14 = 1;
                PowerUp();
                poweredUp = true;
                // ES_Timer_InitTimer(RADIO_TIMER, 2000);
            } else if (ThisEvent.EventParam == RADIO_TIMER) {
                uint8_t bytes[3];
                bytes[0] = WRITE | RADIO_ADDRESS;
                bytes[1] = FM_TUNE_STATUS;
                bytes[2] = 0x00;
                uint8_t result[8];
                WriteAndReadRegister(bytes, 3, result, 8);
            }
            break;
        }

    case ES_UPDATE_VOL:
        {
            DB_printf("sending vol to radio\n");
            SetVolume(ThisEvent.EventParam);            
            break;
        }

    case ES_UPDATE_FREQ:
        {
            DB_printf("sending freq to radio\n");
            SetRadioFrequency(ThisEvent.EventParam);
            break;
        }

    case ES_FREQ_BTN:
        {
            // starts FM seek command searching for valid station
            uint8_t bytes[3];
            bytes[0] = WRITE | RADIO_ADDRESS;
            bytes[1] = FM_SEEK_START;
            bytes[2] = ((uint8_t)(ThisEvent.EventParam) << 3) | 0x04;
            WriteRegister(bytes, 3);
            delay(1000);

            // begins polling status until station frequency is reported in STATUS register
            bool run = true;
            uint8_t result[8];
            uint16_t freq = 0x0000;
            bytes[1] = FM_TUNE_STATUS;
            bytes[2] = 0x01;
            WriteAndReadRegister(bytes, 3, result, 8);
            freq = (result[2] << 8) | result[3];
            while (!freq) {
                WriteAndReadRegister(bytes, 3, result, 8);
                freq = (result[2] << 8) | result[3];
                delay(1000);
            }
            ThisEvent.EventType = ES_UPDATE_FREQ;
            ThisEvent.EventParam = freq;
            SetTuneFrequency(freq);
            PostDisplayService(ThisEvent);
            // DB_printf("Seeked frequency: %d.%d MHz\n", freq / 100, (freq / 10) % 10);

            // clear tune set interrupt
            bytes[1] = FM_TUNE_STATUS;
            bytes[2] = 0x01;
            WriteRegister(bytes, 3);

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

void WriteAndReadRegister(uint8_t *bytes, uint8_t n, uint8_t *result, uint8_t m) {
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
    
    // assert repeated start condition
    I2C1CONbits.RSEN = 1;
    // wait for condition to be set
    while (I2C1CONbits.RSEN);

    // read slave address
    I2C1TRN = READ | bytes[0];
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

void WriteRegister(uint8_t *bytes, uint8_t n) {
    if (bytes[0] == OLED_ADDRESS) {
        I2C1BRG = 0x020;
    } else if (bytes[0] == RADIO_ADDRESS) {
        I2C1BRG = 0x0C6;
    }
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
    uint8_t bytes[4];
    bytes[0] = WRITE | RADIO_ADDRESS;
    bytes[1] = POWER_UP;
    bytes[2] = 0x10;
    bytes[3] = 0x05;
    WriteRegister(bytes, 4);
}

void SetRadioFrequency(uint16_t freq) {
    // send frequency to radio
    uint8_t bytes[6];
    bytes[0] = WRITE | RADIO_ADDRESS;
    bytes[1] = FM_TUNE_FREQ;
    bytes[2] = 0x00;
    bytes[3] = (freq >> 8) & 0xFF;
    bytes[4] = freq & 0xFF;
    bytes[5] = 0x00;
    WriteRegister(bytes, 6);

    // begin polling until frequency is reported in STATUS register
    delay(1000);
    bytes[1] = FM_TUNE_STATUS;
    // clear interrupt flags
    bytes[2] = 0x01;
    uint8_t result[8];
    WriteAndReadRegister(bytes, 3, result, 8);
    freq = (result[2] << 8) | result[3];
    while (!freq) {
        WriteAndReadRegister(bytes, 3, result, 8);
        freq = (result[2] << 8) | result[3];
        delay(1000);
    }
}

void SetVolume(uint8_t vol) {
    uint8_t bytes[7];
    bytes[0] = WRITE | RADIO_ADDRESS;
    bytes[1] = SET_PROPERTY;
    bytes[2] = 0x00;
    bytes[3] = RX_VOLUME_H;
    bytes[4] = RX_VOLUME_L;
    bytes[5] = 0x00;
    bytes[6] = vol;
    WriteRegister(bytes, 7);
}

void delay(int n) {
    while (n > 0) {
        n--;
    }
}