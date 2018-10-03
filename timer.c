#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/*
 * function to enable and set up the timer
 *
 * 1. Enable clock to timer by setting bit 6 in CMU_HFPERCLKEN0 2.
 * Write the period to register TIMER1_TOP 3. Enable timer interrupt
 * generation by writing 1 to TIMER1_IEN 4. Start the timer by writing
 * 1 to TIMER1_CMD
 *
 * This will cause a timer interrupt to be generated every (period)
 * cycles. Remember to configure the NVIC as well, otherwise the
 * interrupt handler will not be invoked.
 */
void setupTimer(uint16_t period)
{
    *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1;	// Enable clock to the timer module
    *TIMER1_TOP = period;                           /* Write the period (number of timer
                                                    ticks between interrupts, tick count will go back
                                                    to zero upon reaching this value) */
    *TIMER1_IEN = 1;                                // Enable timer interrupt generation
    *TIMER1_CMD = 1;                                // Start the timer
}
