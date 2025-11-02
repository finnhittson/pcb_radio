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
    // SCL - pin 17 - RB8 - output
    TRISBbits.TRISB8 = 0;
    // SDA - pin 18 - RB9 - output
    TRISBbits.TRISB9 = 0;

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
    // enable acknowledgments
    I2C1CONbits.ACKDT = 0;
    // turn on I2C1 module
    I2C1CONbits.ON = 1;

    I2C1ADD = ADDRESS;
    I2C1TRN = POWER_UP;
    while (1) {
    	if (I2C1STATbits.TRSTAT == 0) {
    		break;
    	}
    }
    DB_printf("Done transmitting\n");
    uint8_t data = I2C1RCV;
    DB_printf("Recieved: %d\n", data);

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
