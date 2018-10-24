#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/*
 * function to setup the timer
 */
void setupTimer(uint16_t period)
{
  /*
	 * TODO enable and set up the timer
	 *
	 * 1. Enable clock to timer by setting bit 6 in CMU_HFPERCLKEN0 */

   *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1;
   *TIMER1_CTRL = 0x7 << 24; // prescale timer clock -> new Frequency = 109375 Hz

	 /* 2. Write the period to register TIMER1_TOP */

   *TIMER1_TOP = period;

   /*3. Enable timer interrupt
	 * generation by writing 1 to TIMER1_IEN */

    //*TIMER1_IEN |= 0x1;

   /*4. Start the timer by writing
	 * 1 to TIMER1_CMD
   * This will cause a timer interrupt to be generated every (period)
	 * cycles. Remember to configure the NVIC as well, otherwise the
	 * interrupt handler will not be invoked. */
   *TIMER1_CMD |= 0x1;

}
