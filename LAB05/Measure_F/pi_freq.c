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

int main(int argc, char **argv){
	// Variables:
	unsigned char	pin_state = 0; // stores state of pin:  high or low
	double		 AVG_freq = 0; // stores average frequency
	unsigned int	   time_A = 0; // stores time event in micro seconds
	unsigned int	   time_B = 0; // stores time event in micro seconds
	unsigned int	   period = 0; // stores calculated period of input signal
	unsigned int	 samples = 32; // number of samples to average
	double		frequency = 0; // stores calculated frequency of input signal
	double	freq_samples[samples]; // stores frequency samples

	while (1) {
		// for loop to average the frequency	
		for (int i=0; i<samples; i++) {
			// wiringPi setup function
			wiringPiSetup();

			//Set up gpio pin BCM12-WP26 as input
			pinMode(26, INPUT);
			
			// Wait for signal to change and capture time (SC1)
			pin_state = digitalRead(26); // store current value state of pin
			while (digitalRead(26) == pin_state); // when pin 26 doesn't equal pin_state (state changed)
			time_A = micros(); // save time of initial state change
			pin_state = digitalRead(26); // record current state

			// wait for state to change (SC2)
			while (digitalRead(26) == pin_state); // when pin 26 doesn't equal pin_state (state changed)
			pin_state = digitalRead(26); // record current state
			
			// wait for state change (SC3)
			while (digitalRead(26) == pin_state); // when pin 26 doesn't equal pin_state (state changed)
			time_B = micros(); // save time of initial state change
			pin_state = digitalRead(26); // record current state

			// calculate frequency
			period = (time_B - time_A); // store value of period in micro seconds
			frequency = (1.0 / period); // store calculated frequencey, units = 1/us
			frequency *= 1000000.0; // convert 1/us to 1/s or Hz
			
			// store frequency in freq_samples
			freq_samples[i] = frequency;
		}
		
		// sum frequencies
		for (int i=0; i<samples; i++) {
			AVG_freq += freq_samples[i]; // add up frequency measurements
		}
		
		AVG_freq = AVG_freq / samples;

		printf("Frequency = %f\n\n", AVG_freq);
		AVG_freq = 0; // reset
		delay(1000); // delay three seconds
	}

	return 0;
}
