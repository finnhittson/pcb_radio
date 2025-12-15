/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "VolumeService.h"
#include <sys/attribs.h>
#include "dbprintf.h"
#include "DisplayService.h"
#include "RadioService.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;
bool VOLAFalling = false;
bool VOLARising = false;
bool VOLBFalling = false;
bool VOLBRising = false;
volatile uint8_t vol = 20;

/*------------------------------ Module Code ------------------------------*/
bool InitVolumeService(uint8_t Priority) {
    clrScrn();
    DB_printf("Init Volume Service\n");
    ES_Event_t ThisEvent;
    MyPriority = Priority;
    
    // VOLA - pin 12 - RA4 - input 
    TRISAbits.TRISA4 = 1;
    // VOLB - pin 11 - RB4 - input
    TRISBbits.TRISB4 = 1;
    // VOL_BTN - pin 14 - RB5 - input
    TRISBbits.TRISB5 = 1;
    
    // VOLA interrupt
    // disable interrupt
    IEC0bits.INT2IE = 0;
    // map pin 12 RA4 to INT2 interrupt
    INT2R = 0b0010;
    // set INT2 to interrupt on falling edge
    INTCONbits.INT2EP = 0;
    // set INT2 interrupt priority
    IPC2bits.INT2IP = 7;
    // clear INT2 interrupt flag
    IFS0bits.INT2IF = 0;
    // enable INT2 interrupt
    IEC0bits.INT2IE = 1;

    // VOLB interrupt
    // disable interrupt
    IEC0bits.INT4IE = 0;
    // map pin 11 RB4 to INT4 interrupt
    INT4R = 0b0010;
    // set INT4 to interrupt on falling edge
    INTCONbits.INT4EP = 0;
    // set INT4 interrupt priority
    IPC4bits.INT4IP = 6;
    // clear INT4 interrupt flag
    IFS0bits.INT4IF = 0;
    // enable INT4 interrupt
    IEC0bits.INT4IE = 1;
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

bool PostVolumeService(ES_Event_t ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

ES_Event_t RunVolumeService(ES_Event_t ThisEvent) {
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT;
    switch (ThisEvent.EventType) {
    case ES_INIT:
        {
            DB_printf("volume init\n");
            break;
        }
        
    case ES_VOL_BTN:
        {
         	DB_printf("Volume button pressed\n");
         	// clrScrn();
            break;
        }

    case ES_VOLA:
    	{
            // disable interrupt
            IEC0bits.INT2IE = 0;
            if (INTCONbits.INT2EP == 0) {
                VOLAFalling = true;
                // change INT2 to interrupt on rising edge
                INTCONbits.INT2EP = 1;
            } else if (INTCONbits.INT2EP = 1) {
                VOLARising = true;
                // change INT2 to interrupt on falling edge
                INTCONbits.INT2EP = 0;
            }
            // enable inerrupt
            IEC0bits.INT2IE = 1;

            if (VOLAFalling && VOLBFalling && VOLARising) {
                VOLAFalling = false;
                VOLBFalling = false;
                VOLARising = false;
                if (vol < 63) {
                    vol++;
                }
                INTCONbits.INT2EP = 0;
                INTCONbits.INT4EP = 0;
                DB_printf("Volume: %d\n", vol);
                if (getUpdateStatus()) {
                    // vol--;
                } else {
                    ThisEvent.EventType = ES_UPDATE_VOL;
                    ThisEvent.EventParam = vol;
                    PostRadioService(ThisEvent);
                    PostDisplayService(ThisEvent);
                }
            } else if (VOLAFalling && !VOLBFalling && VOLARising) {
                VOLAFalling = false;
                VOLARising = false;
                INTCONbits.INT2EP = 0;
                INTCONbits.INT4EP = 0;
            }
    		break;
    	}

    case ES_VOLB:
    	{
            // disable interrupt
            IEC0bits.INT4IE = 0;
            if (INTCONbits.INT4EP == 0) {
                VOLBFalling = true;
                // change INT4 to interrupt on rising edge
                INTCONbits.INT4EP = 1;
            } else if (INTCONbits.INT4EP == 1) {
                VOLBRising = true;
                // change INT4 to interrupt on falling edge
                INTCONbits.INT4EP = 0;
            }
            // enable interrupt
            IEC0bits.INT4IE = 1;

            if (VOLBFalling && VOLAFalling && VOLBRising) {
                VOLBFalling = false;
                VOLAFalling = false;
                VOLBRising = false;
                if (vol) {
                    vol--;
                }
                INTCONbits.INT2EP = 0;
                INTCONbits.INT4EP = 0;
                DB_printf("Volume: %d\n", vol);
                if (getUpdateStatus()) {
                    // vol++;
                } else {
                    ThisEvent.EventType = ES_UPDATE_VOL;
                    ThisEvent.EventParam = vol;
                    PostRadioService(ThisEvent);
                    PostDisplayService(ThisEvent);
                }
            } else if (VOLBFalling && !VOLAFalling && VOLBRising) {
                VOLBFalling = false;
                VOLBRising = false;
                INTCONbits.INT2EP = 0;
                INTCONbits.INT4EP = 0;
            }
    		break;
    	}
    }
    return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/
void __ISR(_EXTERNAL_2_VECTOR, IPL7SOFT) VOLAResp(void) {
	IFS0bits.INT2IF = 0;
    ES_Event_t ThisEvent;
    ThisEvent.EventType = ES_VOLA;
    PostVolumeService(ThisEvent);
}

void __ISR(_EXTERNAL_4_VECTOR, IPL6SOFT) VOLBResp(void) {
	IFS0bits.INT4IF = 0;
    ES_Event_t ThisEvent;
    ThisEvent.EventType = ES_VOLB;
    PostVolumeService(ThisEvent);
}