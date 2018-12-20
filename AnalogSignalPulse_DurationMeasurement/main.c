/**
* \file main.c
*
* \brief Main source file.
*
(c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use this software and
    any derivatives exclusively with Microchip products. It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
*/
#include <avr/io.h>

/* set DACREF to 0.8 Volts for Vref = 1.5Volts */
#define DACREF_VALUE    (0.8 * 256 / 1.5)

void PORT0_init (void);
void EVENT_SYSTEM_init (void);
void AC0_init (void);
void TIMER0_init (void);


void PORT0_init (void)
{
	/* Positive Input - Disable digital input buffer */
	PORTD.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
	/*Enable output buffer on PA7*/
	PORTA.DIR |= PIN7_bm;
}

void AC0_init (void)
{

    /* Negative input uses internal reference - voltage reference should be enabled */
    VREF.CTRLA = VREF_AC0REFSEL_1V5_gc;    /* Voltage reference at 1.5V */
    VREF.CTRLB = VREF_AC0REFEN_bm;         /* AC0 DACREF reference enable: enabled */

    AC0.DACREF = DACREF_VALUE;             /* Set DAC voltage reference */

    /*Select proper inputs for comparator*/
    AC0.MUXCTRLA = AC_MUXPOS_PIN0_gc       /* Positive Input - Pin 0 */
                | AC_MUXNEG_DACREF_gc;     /* Negative Input - DAC Voltage Reference */
    
    AC0.CTRLA = AC_ENABLE_bm               /* Enable analog comparator */
               | AC_OUTEN_bm;              /* Output Buffer Enable: enabled */
    
    AC0.INTCTRL = 0;                       /* Analog Comparator 0 Interrupt disabled */

}

/*Init TCB in pulse width-frequency measurement mode, input from Analog Comparator through Event System */
void TIMER0_init (void)
{
    
    TCB0.CTRLB = TCB_CNTMODE_FRQPW_gc;     /* Input Capture Frequency and Pulse-Width measurement */
    TCB0.EVCTRL = TCB_CAPTEI_bm;           /* Event Input Enable: enabled */
        
    TCB0.INTCTRL = TCB_CAPT_bm;            /* Capture or Timeout: enabled */

    TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc     /* CLK_PER/2 (From Prescaler) */
                | TCB_ENABLE_bm            /* Enable: enabled */
                | TCB_RUNSTDBY_bm;         /* Run Standby: enabled */

}

/* Enable event generation from Analog comparator to TCB */
void EVENT_SYSTEM_init (void)
{
    EVSYS.CHANNEL0 = EVSYS_GENERATOR_AC0_OUT_gc;    /* Analog Comparator 0 out linked to Event Channel 0 */
    EVSYS.USERTCB0 = EVSYS_CHANNEL_CHANNEL0_gc;      /* TCB uses Event Channel 0 */
}

int main(void)
{
    uint16_t signal_pulse = 0, signal_period = 0;

    PORT0_init();
    AC0_init();
    EVENT_SYSTEM_init();
    TIMER0_init();

    while(1)
    {
        if (TCB0.INTFLAGS)
        {
            /**
            * First read the CNT register
            * The interrupt flag is cleared by writing 1 to it, or when the Capture register
            * is read in Capture mode
            */
            signal_period = TCB0.CNT;
            signal_pulse  = TCB0.CCMP;
        }
     }
}

