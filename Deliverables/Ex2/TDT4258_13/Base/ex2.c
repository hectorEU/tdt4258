#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "efm32gg.h"

// these files are found in the testfile.h (included in the makefile)
extern unsigned short startSound[33280];
extern unsigned short homer[6328];
extern unsigned short tada[13479];
extern unsigned short sweep[4239];

/*
 * TODO calculate the appropriate sample period for the sound wave(s) you 
 * want to generate. The core clock (which the timer clock is derived
 * from) runs at 14 MHz by default. Also remember that the timer counter
 * registers are 16 bits. 
 */
/*
 * The period between sound samples, in clock cycles 
 */
#define TIMER_CLK_FREQUENCY 14000000	//19375// timer-cycles per sec. 14000000//
#define DAC_FREQUENCY 		437500
#define SAMPLE_FREQUENCY 	44100	// samples per sec.
#define SAMPLE_PERIOD		TIMER_CLK_FREQUENCY/SAMPLE_FREQUENCY	// timer-cycles per sample.


/*
 * Declaration of peripheral setup functions 
 */
extern void setupTimer(uint32_t period);
extern void setupDAC();
extern void setupGPIO();
extern bool read_button(int btn);

int main(void)
{
	// SETUP FUNCTIONS:
	setupGPIO();
	setupDAC();
	setupTimer(SAMPLE_PERIOD);

	// DEBUG VARIABLES:
	uint16_t t, x = 0;	// used for LED debugging.

	// SYNTH VARIABLES.
	uint16_t z, y = 0;	// used for keeping track of sound samples to feed the DAC.
	bool startup = true;	// will turn false once startupsong has finnished.
	bool buissy1, buissy2, buissy3, buissy4 = false;	//used to avoid the noise generated when trying to play two sound effects at the same time.

	uint16_t squareWave[10] = { 500, 500, 500, 500, 500, 0, 0, 0, 0, 0 };
	uint16_t sawToothWave[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	uint16_t triangleWave[10] = { 0, 1, 2, 3, 4, 5, 4, 3, 2, 1 };

	while (1) {

		//      while(*TIMER1_CNT != 0);// delay for 1 SAMPLE_PERIOD (because TIMER_CNT1 will overflow at 1 sample period.)

		// different waves.
		if (!read_button(6)) {
			y++;
			if (y == 20) {
				z++;
				y = 0;
			}	// slows down how fast the samples are played.
			if (z == 9) {
				z = 0;
			}
			*DAC0_CH0DATA = squareWave[z];
			*DAC0_CH1DATA = squareWave[z];

		}
		if (!read_button(5)) {
			y++;
			if (y == 20) {
				z++;
				y = 0;
			}	// slows down how fast the samples are played.;
			if (z == 9) {
				z = 0;
			}
			*DAC0_CH0DATA = sawToothWave[z] * 50;
			*DAC0_CH1DATA = sawToothWave[z] * 50;

		}
		if (!read_button(4)) {
			y++;
			if (y == 20) {
				z++;
				y = 0;
			}	// slows down how fast the samples are played.
			if (z == 9) {
				z = 0;
			}
			*DAC0_CH0DATA = triangleWave[z] * 50;
			*DAC0_CH1DATA = triangleWave[z] * 50;

		}

		// startup sound effect: windows song.
		if (!read_button(3))
			startup = true;
		if ((startup || buissy1) && !buissy2 && !buissy3 && !buissy4) {
			buissy1 = true;
			y++;
			if (y == 10) {
				z++;
				y = 0;
			}	// slows down how fast the samples are played. (had to be speed up to save ram)
			if (z == sizeof(startSound) / sizeof(startSound[0])) {
				z = 0;
				startup = false;
				buissy1 = false;
			}
			*DAC0_CH0DATA = startSound[z] * 2;
			*DAC0_CH1DATA = startSound[z] * 2;

		}
		// sound effect 2: homer sound
		if ((!read_button(2) || buissy2) && !buissy1 && !buissy3
		    && !buissy4 && !startup) {
			y++;
			buissy2 = true;
			if (y == 3) {
				z++;
				y = 0;
			}	// slows down how fast the samples are played. (had to be speed up to save ram)
			if (z == sizeof(homer) / sizeof(homer[0])) {
				z = 0;
				buissy2 = false;
			}
			*DAC0_CH0DATA = homer[z] * 2;
			*DAC0_CH1DATA = homer[z] * 2;

		}

		// sound effect 3: tada sound
		if ((!read_button(1) || buissy3) && !buissy1 && !buissy2
		    && !buissy4 && !startup) {
			buissy3 = true;
			y++;
			if (y == 5) {
				z++;
				y = 0;
			}	// slows down how fast the samples are played. (had to be speed up to save ram)
			if (z == sizeof(tada) / sizeof(tada[0])) {
				z = 0;
				buissy3 = false;
			}
			*DAC0_CH0DATA = tada[z] * 2;
			*DAC0_CH1DATA = tada[z] * 2;

		}
		// sound effect 4: sweep
		if ((!read_button(0) || buissy4) && !buissy1 && !buissy2
		    && !buissy3 && !startup) {
			buissy4 = true;
			y++;
			if (y == 20) {
				z++;
				y = 0;
			}	// slows down how fast the samples are played. (had to be speed up to save ram)
			if (z == sizeof(sweep) / sizeof(sweep[0])) {
				z = 0;
				buissy4 = false;
			}
			*DAC0_CH0DATA = sweep[z] * 2;
			*DAC0_CH1DATA = sweep[z] * 2;

		}

		// FOR DEBUGGING:
		t++;
		x++;
		if (x == 1000)
			*GPIO_PA_DOUT = *GPIO_PA_DOUT << 1;
		if (t == 20000) {
			*GPIO_PA_DOUT = 1;
			t = 0;
		}
		if (x == 1000)
			x = 0;
		///////////////////

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
