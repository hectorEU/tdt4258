#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/*
 * TIMER1 interrupt handler
 */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/*
	 * TODO feed new samples to the DAC remember to clear the pending
	 * interrupt by writing 1 to TIMER1_IFC
	 */

   //*TIMER1_IFC = 1;   // clear interrupt flag of Timer1

/*    LED_blink_counter2;
    if(LED_blink_counter2 == 125)
    {
      LED_blink_counter = 1 << LED_blink_loop;

      *GPIO_PA_DOUT ^= LED_blink_counter;

      LED_blink_loop++;

      if(LED_blink_loop == 17)
      {
        LED_blink_loop = 8;
        *GPIO_PA_DOUT = 0xff00;
      }

      LED_blink_counter2 = 0;
    }
*/

}

/*
 * GPIO even pin interrupt handler
 */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	/*
	 * TODO handle button pressed event, remember to clear pending
	 * interrupt
	 */
   *GPIO_IFC = *GPIO_IF;
   read_buttons();
}

/*
 * GPIO odd pin interrupt handler
 */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	/*
	 * TODO handle button pressed event, remember to clear pending
	 * interrupt
	 */
   *GPIO_IFC = *GPIO_IF;
   read_buttons();
}
