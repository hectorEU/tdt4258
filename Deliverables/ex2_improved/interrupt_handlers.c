#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"


#include "testfile.h"

uint16_t t,x = 0;
/*
 * TIMER1 interrupt handler 
 */

bool play = true;
uint8_t sound2Play=0;

uint16_t samplePointer = 0; 
uint16_t samplePointerScaler = 0;

void initSound(uint8_t sound){
	
	if(!play){
		samplePointer = 0;
		sound2Play=sound;
		play=true;
	}
	
}

int playSample(uint8_t sound, int sampleNumber){

	if (sound == 0 && play){
		if (sampleNumber>=sizeof(startSound)/sizeof(startSound[0])){
			play=false;
			return 0;
		}else{
			*DAC0_CH0DATA = startSound[sampleNumber]*1;
			*DAC0_CH1DATA = startSound[sampleNumber]*1;
			samplePointerScaler++;
			if (samplePointerScaler == 10) {
				sampleNumber++;
				samplePointerScaler=0;
			}
			return sampleNumber;
		}
		
	}

	if (sound == 1 && play){
		if (sampleNumber>=sizeof(tada)/sizeof(tada[0])){
			play=false;
			return 0;
		}else{
			*DAC0_CH0DATA = tada[sampleNumber]*1;
			*DAC0_CH1DATA = tada[sampleNumber]*1;
			samplePointerScaler++;
			if (samplePointerScaler == 5) {
				sampleNumber++;
				samplePointerScaler=0;
			}
			return sampleNumber;
		}
		
	}
	
	if (sound == 2 && play){
		if (sampleNumber>=sizeof(homer)/sizeof(homer[0])){
			play=false;
			return 0;
		}else{
			*DAC0_CH0DATA = homer[sampleNumber]*1;
			*DAC0_CH1DATA = homer[sampleNumber]*1;
			samplePointerScaler++;
			if (samplePointerScaler == 3) {
				sampleNumber++;
				samplePointerScaler=0;
			}
			return sampleNumber;
		}
		
	}
	
	if (sound == 3 && play){
		if (sampleNumber>=sizeof(sweep)/sizeof(sweep[0])){
			play=false;
			return 0;
		}else{
			*DAC0_CH0DATA = sweep[sampleNumber]*1;
			*DAC0_CH1DATA = sweep[sampleNumber]*1;
			samplePointerScaler++;
			if (samplePointerScaler == 3) {
				sampleNumber++;
				samplePointerScaler=0;
			}
			return sampleNumber;
		}
		
	}


	return 0;			
}

void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	samplePointer = playSample(sound2Play, samplePointer);
	/*
	 * TODO feed new samples to the DAC remember to clear the pending
	 * interrupt by writing 1 to TIMER1_IFC 
	 */
	 
	 	*TIMER1_IFC = 1;
	
	
	
	
	/// LED ACTION FOR FUN ////
			x++;
			
			if (x == 1500){
				*GPIO_PA_DOUT = *GPIO_PA_DOUT  << 1;
				x=0;
				t++;
			} 
			if (t == 10){
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
	/*
	 * TODO handle button pressed event, remember to clear pending
	 * interrupt 
	 */
	 *GPIO_IFC = *GPIO_IF;
	 
	if (isButtonPushed(0)) initSound(0);
	if (isButtonPushed(2)) initSound(2);
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

	 if (isButtonPushed(1)) initSound(1);
	 if (isButtonPushed(3)) initSound(3);
}
