#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

// SYNTH PARAMETERS:
// these files are found in the testfile.h (included in the makefile)
extern unsigned short startSound[33280];
extern unsigned short homer[6328];
extern unsigned short tada[13479];
extern unsigned short sweep[4239];

bool play = true; //indicates if a soundclip is currently beeing played. turns false when soundclip is finnished. also usefull for debouncing buttons.
uint8_t sound2Play = 0; // number indicating which soundclip are played.
uint16_t samplePointer = 0; // used for keeping track of sample.
uint16_t samplePointerScaler = 0; //used by speed factor for the playback of samples at lower speed.

// LED:
uint16_t t, x = 0; // counters for LED debugging.

//BUTTONS
extern bool isButtonPushed(int btn); // found in gpio.c



// SYNTH FUNCTIONS. (SHOULD PROBABLY BE MOVED TO A SEPERATE FILE TOGETHER WITH THE GLOBAL SYNTH PARAMETERS)
void initSound(uint8_t sound)
{

	if (!play) {
		samplePointer = 0;
		sound2Play = sound;
		play = true;
	}

}

int playSample(uint8_t sound, int sampleNumber) // play sample returns the index of the next sample.
{

	if (sound == 0 && play) {
		if (sampleNumber >= sizeof(startSound) / sizeof(startSound[0])) {
			play = false;
			return 0;
		} else {
			*DAC0_CH0DATA = startSound[sampleNumber] * 1;
			*DAC0_CH1DATA = startSound[sampleNumber] * 1;
			samplePointerScaler++;
			if (samplePointerScaler == 10) {
				sampleNumber++;
				samplePointerScaler = 0;
			}
			return sampleNumber;
		}

	}

	if (sound == 1 && play) {
		if (sampleNumber >= sizeof(tada) / sizeof(tada[0])) {
			play = false;
			return 0;
		} else {
			*DAC0_CH0DATA = tada[sampleNumber] * 1;
			*DAC0_CH1DATA = tada[sampleNumber] * 1;
			samplePointerScaler++;
			if (samplePointerScaler == 5) {
				sampleNumber++;
				samplePointerScaler = 0;
			}
			return sampleNumber;
		}

	}

	if (sound == 2 && play) {
		if (sampleNumber >= sizeof(homer) / sizeof(homer[0])) {
			play = false;
			return 0;
		} else {
			*DAC0_CH0DATA = homer[sampleNumber] * 1;
			*DAC0_CH1DATA = homer[sampleNumber] * 1;
			samplePointerScaler++;
			if (samplePointerScaler == 3) {
				sampleNumber++;
				samplePointerScaler = 0;
			}
			return sampleNumber;
		}

	}

	if (sound == 3 && play) {
		if (sampleNumber >= sizeof(sweep) / sizeof(sweep[0])) {
			play = false;
			return 0;
		} else {
			*DAC0_CH0DATA = sweep[sampleNumber] * 1;
			*DAC0_CH1DATA = sweep[sampleNumber] * 1;
			samplePointerScaler++;
			if (samplePointerScaler == 3) {
				sampleNumber++;
				samplePointerScaler = 0;
			}
			return sampleNumber;
		}

	}

	return 0;
}





/*
 * TIMER1 interrupt handler 
 */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	samplePointer = playSample(sound2Play, samplePointer); // feed samples to DAC. 

	*TIMER1_IFC = 1;

	
	
	/// LED ACTION FOR FUN AND DEBUGGING ////
	x++;

	if (x == 1500) {
		*GPIO_PA_DOUT = *GPIO_PA_DOUT << 1;
		x = 0;
		t++;
	}
	if (t == 10) {
		t = 0;
		*GPIO_PA_DOUT = 0x0100;
	}
	//// END LED ACTION ///
	
	
}

/*
 * GPIO even pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	*GPIO_IFC = *GPIO_IF; //clear interrupt

	if (isButtonPushed(0))
		initSound(0);
	if (isButtonPushed(2))
		initSound(2);
}

/*
 * GPIO odd pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{

	*GPIO_IFC = *GPIO_IF; //clear interrupt

	if (isButtonPushed(1))
		initSound(1);
	if (isButtonPushed(3))
		initSound(3);
}
