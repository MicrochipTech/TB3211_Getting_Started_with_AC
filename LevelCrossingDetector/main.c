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
#include <avr/interrupt.h>

/* set DACREF to 0.8 Volts for Vref = 1.5Volts */
#define DACREF_VALUE    (0.8 * 256 / 1.5)

void PORT0_init (void);
void AC0_init(void);
            
ISR(AC0_AC_vect)
{
    /* Insert AC interrupt handling code here */

    /* The interrupt flag has to be cleared manually */
    AC0.STATUS = AC_CMP_bm;
}

void PORT0_init (void)
{
	/* Positive Input - Disable digital input buffer */
	PORTD.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
	/*Enable output buffer on PA7*/
	PORTA.DIR |= PIN7_bm;
}

void AC0_init(void)
{
 
    /* Negative input uses internal reference - voltage reference should be enabled */
    VREF.CTRLA = VREF_AC0REFSEL_1V5_gc;    /* Voltage reference at 1.5V */
    VREF.CTRLB = VREF_AC0REFEN_bm;         /* AC0 DACREF reference enable: enabled */

    AC0.DACREF = DACREF_VALUE;             /* Set DAC voltage reference */

    /*Select proper inputs for comparator*/
    AC0.MUXCTRLA = AC_MUXPOS_PIN0_gc       /* Positive Input - Analog Positive Pin 0 */
                | AC_MUXNEG_DACREF_gc;     /* Negative Input - DAC Voltage Reference */
    
    AC0.CTRLA = AC_ENABLE_bm               /* Enable analog comparator */
               | AC_OUTEN_bm;              /* Output Buffer Enable: enabled */
    
    AC0.INTCTRL = AC_CMP_bm;               /* Analog Comparator 0 Interrupt enabled */

}


int main(void)
{
    PORT0_init();
    AC0_init();
    sei();            /* Global interrupts enabled */

    while (1) 
    {
        ;    
    }
}

