#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "math.h"

#define MAX_AMPLITUDE 0xfff

volatile double n_sample = 0;


/*
 * TIMER1 interrupt handler
 feed new samples to the DAC remember to clear the pending
 interrupt by writing 1 to TIMER1_IFC
 */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
    int current_amplitude = round((MAX_AMPLITUDE*sin(n_sample))) + MAX_AMPLITUDE/2;
    *DAC0_COMBDATA = (current_amplitude << 16) | current_amplitude;
    n_sample++;
    *TIMER1_IFC = 1;	// clear the interrupt
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
}
