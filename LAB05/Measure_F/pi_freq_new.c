/* pi_freq.c
 *
 * Calvin Skinner
 * 3/6/2019
 * 
 * 	Measure frequency with a gpio pin. Measure time when the pin is low and then when the pin is high.
 * time_A and time_B correspond to consecutive state changes. These values are then subtracted giving the
 * the period: T
 *
 *	T = abs(time_B - time_A)
 *
 *	1 / T = frequency
 *	  ______        ______        ______
 *	 |      |      |      |      |
 *	_|      |______|      |______|
 *	        ^SC1   ^SC2   ^SC3
 *	SC1(state change one) is where the first time sample is captured
 *	SC3 is where the second time sample is captured
*/

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <math.h>

double measureFreq (unsigned int pin);

int main(int argc, char **argv){
	
	// Variables:
	double		 AVG_freq = 0; // stores average frequency
	unsigned int	 samples = 32; // number of samples to average
	double	freq_samples[samples]; // stores frequency samples

	// initialize wiring pi
	wiringPiSetup();
	
	// MAIN LOOP
	while (1) {
		
		// sum frequencies
		for (int i=0; i<samples; i++) {
			AVG_freq += measureFreq(26);
		}
		
		AVG_freq = AVG_freq / samples;

		printf("Frequency = %f\n\n", AVG_freq);
		AVG_freq = 0; // reset
		delay(1000); // delay in milliseconds
	}

	return 0;
}

//////////////////////////////////////////
//		FUNCTIONS		//
//////////////////////////////////////////

// function: measure frequency on given pin

double measureFreq (unsigned int pin) {

	unsigned char	pin_state = 0; // stores state of pin:  high or low
	unsigned int	   time_A = 0; // stores time event in micro seconds
	unsigned int	   time_B = 0; // stores time event in micro seconds
	unsigned int	   period = 0; // stores calculated period of input signal
	double		frequency = 0; // stores calculated frequency of input signal
	
	// wiringPi setup function will reset the counter for micros()
	wiringPiSetup();

	//Set up gpio pin BCM12-WP26 as input
	pinMode(pin, INPUT);
	
	// SC1
	// Wait for signal to change and capture time
	pin_state = digitalRead(pin); // store current value state of pin
	while (digitalRead(pin) == pin_state); // when pin 26 doesn't equal pin_state (state changed)
	time_A = micros(); // save time of initial state change
	pin_state = digitalRead(pin); // record current state

	// SC2
	// wait for state to change
	while (digitalRead(pin) == pin_state); // when pin 26 doesn't equal pin_state (state changed)
	pin_state = digitalRead(pin); // record current state
	
	// SC3
	// wait for state change
	while (digitalRead(pin) == pin_state); // when pin 26 doesn't equal pin_state (state changed)
	time_B = micros(); // save time of initial state change
	pin_state = digitalRead(pin); // record current state

	// calculate frequency
	period = (time_B - time_A); // store value of period in micro seconds
	frequency = (1.0 / period); // store calculated frequencey, units = 1/us
	frequency *= 1000000.0; // convert 1/us to 1/s or Hz

	return frequency;
}


