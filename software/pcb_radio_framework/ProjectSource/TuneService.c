/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "TuneService.h"
#include <sys/attribs.h>
#include "dbprintf.h"
#include "RadioService.h"
#include "DisplayService.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;
bool FREQAFalling = false;
bool FREQARising = false;
bool FREQBFalling = false;
bool FREQBRising = false;
volatile uint16_t freq = 6400;

/*------------------------------ Module Code ------------------------------*/
bool InitTuneService(uint8_t Priority) {
    DB_printf("Init Tune Service!\n");
    ES_Event_t ThisEvent;
    MyPriority = Priority;
    
    // FREQA - pin 22 - RB11 - input
    TRISBbits.TRISB10 = 1;
    // FREQB - pin 21 - RB10 - input
    TRISBbits.TRISB11 = 1;
    // FREQ_BTN - pin 23 - RB12 - input
    TRISBbits.TRISB12 = 1;
    
    // FREQA interrupt
    // disable interrupt
    IEC0bits.INT1IE = 0;
    // map pin 21 RB10 to INT1 interrupt
    INT1R = 0b0011;
    // set INT1 to interrupt on falling edge
    INTCONbits.INT1EP = 0;
    // set INT1 interrupt priority
    IPC1bits.INT1IP = 5;
    // clear INT1 interrupt flag
    IFS0bits.INT1IF = 0;
    // enable INT1 interrupt
    IEC0bits.INT1IE = 1;

    // FREQB interrupt
    // disable interrupt
    IEC0bits.INT3IE = 0;
    // map pin 22 RB11 to INT3 interrupt
    INT3R = 0b0011;
    // set INT3 to interrupt on falling edge
    INTCONbits.INT3EP = 0;
    // set INT3 interrupt priority
    IPC3bits.INT3IP = 4;
    // clear INT3 interrupt flag
    IFS0bits.INT3IF = 0;
    // enable INT3 interrupt
    IEC0bits.INT3IE = 1;

    __builtin_enable_interrupts();

    // post the initial transition event
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == true) {
        return true;
    } else {
        return false;
    }
}

bool PostTuneService(ES_Event_t ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

ES_Event_t RunTuneService(ES_Event_t ThisEvent) {
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT;
    switch (ThisEvent.EventType) {
    case ES_FREQ_BTN:
        {
         	DB_printf("Frequency button pressed\n");
         	// clrScrn();
            break;
        }

    case ES_FREQA:
    	{
            // disable interrupt
            IEC0bits.INT1IE = 0;
            if (INTCONbits.INT1EP == 0) {
                FREQAFalling = true;
                // change INT1 to interrupt on rising edge
                INTCONbits.INT1EP = 1;
            } else if (INTCONbits.INT1EP = 1) {
                FREQARising = true;
                // change INT1 to interrupt on falling edge
                INTCONbits.INT1EP = 0;
            }
            // enable inerrupt
            IEC0bits.INT1IE = 1;

            if (FREQAFalling && FREQBFalling && FREQARising) {
                FREQAFalling = false;
                FREQBFalling = false;
                FREQARising = false;
                if (freq == 6400) {
                    freq = 10800;
                } else {
                    freq = freq - 10;
                }
                INTCONbits.INT1EP = 0;
                INTCONbits.INT3EP = 0;
                // DB_printf("Frequency: %d\n", freq);
                if (getUpdateStatus()) {
                    // freq = freq + 10;
                } else {
                    ThisEvent.EventType = ES_UPDATE_FREQ;
                    ThisEvent.EventParam = freq;
                    PostDisplayService(ThisEvent);
                }
            } else if (FREQAFalling && !FREQBFalling && FREQARising) {
                FREQAFalling = false;
                FREQARising = false;
                INTCONbits.INT1EP = 0;
                INTCONbits.INT3EP = 0;
            }
    		break;
    	}

    case ES_FREQB:
    	{
            // disable interrupt
            IEC0bits.INT3IE = 0;
            if (INTCONbits.INT3EP == 0) {
                FREQBFalling = true;
                // change INT3 to interrupt on rising edge
                INTCONbits.INT3EP = 1;
            } else if (INTCONbits.INT3EP == 1) {
                FREQBRising = true;
                // change INT3 to interrupt on falling edge
                INTCONbits.INT3EP = 0;
            }
            // enable interrupt
            IEC0bits.INT3IE = 1;

            if (FREQBFalling && FREQAFalling && FREQBRising) {
                FREQBFalling = false;
                FREQAFalling = false;
                FREQBRising = false;
                if (freq == 10800) {
                    freq = 6400;
                } else {
                    freq = freq + 10;
                }
                INTCONbits.INT1EP = 0;
                INTCONbits.INT3EP = 0;
                // DB_printf("Frequency: %d\n", freq);
                if (getUpdateStatus()) {
                    // freq = freq - 10;
                } else {
                    ThisEvent.EventType = ES_UPDATE_FREQ;
                    ThisEvent.EventParam = freq;
                    PostDisplayService(ThisEvent);
                }
            } else if (FREQBFalling && !FREQAFalling && FREQBRising) {
                FREQBFalling = false;
                FREQBRising = false;
                INTCONbits.INT1EP = 0;
                INTCONbits.INT3EP = 0;
            }
    		break;
    	}
    }
    return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void __ISR(_EXTERNAL_1_VECTOR, IPL5SOFT) FREQAResp(void) {
	IFS0bits.INT1IF = 0;
    ES_Event_t ThisEvent;
    ThisEvent.EventType = ES_FREQA;
    PostTuneService(ThisEvent);
}

void __ISR(_EXTERNAL_3_VECTOR, IPL4SOFT) FREQBResp(void) {
	IFS0bits.INT3IF = 0;
    ES_Event_t ThisEvent;
    ThisEvent.EventType = ES_FREQB;
    PostTuneService(ThisEvent);
}