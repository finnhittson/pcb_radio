/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "NeoPixelService.h"
#include <sys/attribs.h>

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;
uint16_t Period = 25;

/*------------------------------ Module Code ------------------------------*/
bool InitNeoPixelService(uint8_t Priority) {
    ES_Event_t ThisEvent;

    MyPriority = Priority;
    
    // disable timer2
    T2CONbits.ON = 0;
    // set timer2 source as internal peripherial clock
    T2CONbits.TCS = 0;
    // diable TGATE
    T2CONbits.TGATE = 0;
    // set timer2 pre-scaler
    T2CONbits.TCKPS = 0;
    // set timer2 period
    PR2 = Period - 1;
    // set timer2 to start at 0
    TMR2 = 0;
    // clear timer2 interrupt flags
    IFS0bits.T2IF = 0;
    // set local mask for timer3 interrupt
    IEC0bits.T2IE = 1;
    // set timer3 interrupt priority
    IPC3bits.T3IP = 3;
    // enable timer 2
    T2CONbits.ON = 1;

    // diable output compare module 1
    OC1CONbits.ON = 0;
    // set to continue in idle mode
    OC1CONbits.SIDL = 0;
    // map RB3 to output compare modele 1
    RPB3R = 0b0101;
    // use 16 bit timer source
    OC1CONbits.OC32 = 0;
    // use timer2
    OC1CONbits.OCTSEL = 0;
    // set duty cycle
    OC1R = 0;
    // set duty cycle
    OC1RS = 0;
    // set to PWM mode on with fault pin disabled
    OC1CONbits.OCM = 0b110;
    // enable output compare module 1
    OC1CONbits.ON = 1;

    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == true) {
        return true;
    } else {
        return false;
    }
}

bool PostNeoPixelService(ES_Event_t ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

ES_Event_t RunNeoPixelService(ES_Event_t ThisEvent) {
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT;
    
    return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void __ISR(_TIMER_2_VECTOR, IPL3SOFT) Timer2Resp(void) {
    IFS0bits.T2IF = 0;
    // set next duty cycle
}

void SendColor(uint8_t r, uint8_t g, uint8_t b) {

}