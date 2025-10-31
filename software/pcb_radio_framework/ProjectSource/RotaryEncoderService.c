/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "RotaryEncoderService.h"
#include <sys/attribs.h>
#include "dbprintf.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
bool InitRotaryEncoderService(uint8_t Priority) {
    clrScrn();
    DB_printf("Init Rotary Encoder Service\n");
    ES_Event_t ThisEvent;
    MyPriority = Priority;
    
    // VOLA - pin 12 - RA4 - input
    TRISAbits.TRISA4 = 1;
    // VOL_BTN - pin 14 - RB5 - input
    TRISBbits.TRISB5 = 1;
    
    // disable interrupt
    IEC0bits.INT2IE = 0;
    // map pin 12 RA4 to INT2 interrupt
    INT2R = 0b0010;
    // set INT2 to interrupt on falling edge
    INTCONbits.INT2EP = 0;
    // set INT2 interrupt priority
    IPC2bits.INT2IP = 7;
    // set INT2 interrupt sub-priority
//    IPC2bits.INT2IS = 3;
    // clear INT2 interrupt flag
    IFS0bits.INT2IF = 0;
    // enable INT2 interrupt
    IEC0bits.INT2IE = 1;
    // enable multi vector interrupt mode
    INTCONbits.MVEC = 1;
    
    __builtin_enable_interrupts();
    
    // post the initial transition event
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == true) {
        return true;
    } else {
        return false;
    }
}

bool PostRotaryEncoderService(ES_Event_t ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

ES_Event_t RunRotaryEncoderService(ES_Event_t ThisEvent) {
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT;
    switch (ThisEvent.EventType) {
    case ES_VOL_BTN:
        {
            
            break;
        }
    }
    return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void __ISR(_EXTERNAL_2_VECTOR, IPL7SOFT) RotaryEncResp(void) {
    DB_printf("VOLA triggered\n");
}