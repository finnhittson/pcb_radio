/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "RadioService.h"
#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
bool InitRadioService(uint8_t Priority) {
    ES_Event_t ThisEvent;
    MyPriority = Priority;
    DB_printf("Init Radio Service\n");

    // do not need to initialize I2C pins
    // SCL - pin 17 - RB8 - input
    TRISBbits.TRISB8 = 1;
    // SDA - pin 18 - RB9 - input
    TRISBbits.TRISB9 = 1;

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
    
    // set I2C baud rate
    I2C1BRG = 0x0C6;

    // assert start condition 
    I2C1CONbits.SEN = 1;
    // wait for condition to be set
    while (I2C1CONbits.SEN);
    // write slave address
    I2C1TRN = WRITE | ADDRESS;
    // wait for acknowledgement from slave
    while (I2C1STATbits.ACKSTAT);
    // write data
    I2C1TRN = POWER_UP;
    // wait for acknowledgement from slave
    while (I2C1STATbits.ACKSTAT);
    // assert repeated start condition
    I2C1CONbits.RSEN = 1;
    // wait for condition to be set
    while (I2C1CONbits.RSEN);
    // write slave address
    I2C1TRN = READ | ADDRESS;
    // wait for acknowledgement from slave
    while (I2C1STATbits.ACKSTAT);
    // enable master receive serial data
    I2C1CONbits.RCEN = 1;
    // wait finite time for receive buffer to fill
    bool received = false;
    for (int i = 0; i < 1000; i++) {
        if (I2C1STATbits.RBF) {
            received = true;
            break;
        }
    }
    if (received) {
        I2C1CONbits.ACKDT = 0;
    } else {
        I2C1CONbits.ACKDT = 1;
    }
    // send acknowledgement sequence
    I2C1CONbits.ACKEN = 1;
    // wait for condition to be set
    while (I2C1CONbits.ACKEN);
    // send stop bit
    I2C1CONbits.PEN = 1;
    // wait for condition to be set
    while (I2C1CONbits.PEN);

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
    
    return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
