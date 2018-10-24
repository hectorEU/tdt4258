#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "efm32gg.h"

/*
 * TODO calculate the appropriate sample period for the sound wave(s) you
 * want to generate. The core clock (which the timer clock is derived
 * from) runs at 14 MHz by default. Also remember that the timer counter
 * registers are 16 bits.
 */
/*
 * The period between sound samples, in clock cycles
 */
#define   SAMPLE_PERIOD   54688

/*
 * Declaration of peripheral setup functions
 */
void setupTimer(uint32_t period);
void setupDAC();
void setupNVIC();
void setupGPIO();


uint16_t flag = 0;
uint16_t LED_blink_counter = 0;
uint16_t LED_blink_counter2 = 0;
uint16_t LED_blink_loop = 8;

/*
 * Your code will start executing here
 */
int main(void)
{
	/*
	 * Call the peripheral setup functions
	 */
	setupGPIO();
	setupDAC();
	setupTimer(SAMPLE_PERIOD);
	setupNVIC();
	uint8_t c = 200;

	while(1)
	{

		*DAC0_CH0DATA = 0xFFF;
		*DAC0_CH1DATA = 0xFFF;
		
		if(*TIMER1_CNT == SAMPLE_PERIOD-20)
		{
			
			c++;
				if(flag == 1)
				{
					// turn on LEDs
					*GPIO_PA_DOUT = 0x0000;
					flag = 0;
				}
					// turn off LEDs
				else
				{
					*GPIO_PA_DOUT = 0xff00;
					flag = 1;
				}
		}

		//read_buttons();

	}

	return 0;
}





void setupNVIC()
{
	/*
	 * TODO use the NVIC ISERx registers to enable handling of
	 * interrupt(s) remember two things are necessary for interrupt
	 * handling: - the peripheral must generate an interrupt signal - the
	 * NVIC must be configured to make the CPU handle the signal You will
	 * need TIMER1, GPIO odd and GPIO even interrupt handling for this
	 * assignment.
	 */

	 *ISER0 |= 1 << 1;  // Enable interrupt request in NVIC for interrupt line 1 GPIO_EVEN
	 *ISER0 |= 1 << 11; // Enable interrupt request in NVIC for interrupt line 1 GPIO_ODD
	 *ISER0 |= 1 << 12; // Enable interrupt request in NVIC for interrupt line 12 TIMER1
}


/*
 * if other interrupt handlers are needed, use the following names:
 * NMI_Handler HardFault_Handler MemManage_Handler BusFault_Handler
 * UsageFault_Handler Reserved7_Handler Reserved8_Handler
 * Reserved9_Handler Reserved10_Handler SVC_Handler DebugMon_Handler
 * Reserved13_Handler PendSV_Handler SysTick_Handler DMA_IRQHandler
 * GPIO_EVEN_IRQHandler TIMER0_IRQHandler USART0_RX_IRQHandler
 * USART0_TX_IRQHandler USB_IRQHandler ACMP0_IRQHandler ADC0_IRQHandler
 * DAC0_IRQHandler I2C0_IRQHandler I2C1_IRQHandler GPIO_ODD_IRQHandler
 * TIMER1_IRQHandler TIMER2_IRQHandler TIMER3_IRQHandler
 * USART1_RX_IRQHandler USART1_TX_IRQHandler LESENSE_IRQHandler
 * USART2_RX_IRQHandler USART2_TX_IRQHandler UART0_RX_IRQHandler
 * UART0_TX_IRQHandler UART1_RX_IRQHandler UART1_TX_IRQHandler
 * LEUART0_IRQHandler LEUART1_IRQHandler LETIMER0_IRQHandler
 * PCNT0_IRQHandler PCNT1_IRQHandler PCNT2_IRQHandler RTC_IRQHandler
 * BURTC_IRQHandler CMU_IRQHandler VCMP_IRQHandler LCD_IRQHandler
 * MSC_IRQHandler AES_IRQHandler EBI_IRQHandler EMU_IRQHandler
 */
