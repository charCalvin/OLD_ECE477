#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <math.h>
#include <avr/sleep.h>

void init_pwm(void);
void update_clock_speed(void);

//This code is distributed with no warranty expressed or implied.
//It does not contain any known bugs, but has not been tested.  
//What it is intended to do is use the first two bytes of eeprom
//as an offset and direction for adjusting the internal oscillator
//The first byte is an unsigned byte that is the amount to adjust 
//the OSCCAL register.  The next byte will be 0 or 1 depending on 
//whether the adjustment should be positive (0) or negative (1).
//The value 0xff is intentionally avoided to distinguish unprogrammed
//eeprom locations.

//main first calls update_clock_speed to make the adjustments to
//the oscillator calibration and then calls init_pwm to set up 
//a 100Hz 50% duty cycle square wave on OC1A (pin 15 on the 28 pin 
//DIP package).

#define PC1_val (PINC & 0x02)	// This reads value Port C pin 1
#define PC2_val (PINC & 0x04)	// This reads value Port C pin 2
#define PC3_val (PINC & 0x08)	// This reads value Port C pin 3


int main()
{
	update_clock_speed();  //adjust OSCCAL
	init_pwm();            //set up hardware PWM
	
	// Set DDRC bit 1-3 to zero
	DDRC &= ~0x06;	// 0b00001110
	PORTC &= ~0x06;	// clear bits 1-3 for no pull up

	// Set DDRC pin3 to output = 1
	DDRC |= 0x08;
	PORTC |= 0x08;
		
/*	This while loop reads two pins: PC1, PC2, The raspberry pi 
	sends a signal telling the avr chip to add or subtract from 
	the OSCCAL. PC1 is status pin; when its high iformation is
	being sent on PC2. PC2 denotes sign; whether to add or
	subtract from OSCCAL.
*/
	OSCCAL = 0xFF;
  	while(1){
		PORTC &= ~0x08;
		while (PC1_val == 0);
		if (PC2_val == 1) OSCCAL+=1;	// increase clock speed
		else OSCCAL-=1;		// decrease clock speed
		while (PC1_val == 0);
		while (PC1_val == 1);		
		PORTC |= 0x08;
		//OSCCAL+= -1;
	}             
}

//read the first two bytes of eeprom, if they have been programmed
//use the first byte as an offset for adjusting OSCCAL and the second as
//the direction to adjust 0=increase, 1=decrease.
//Any offset is allowed and users are cautioned that it is possible to
// adjust the oscillator beyond safe operating bounds.
void update_clock_speed(void)
{
  char temp;
  temp=eeprom_read_byte((void *)1); //read oscillator offset sign 
  	                            //0 is positive 1 is  negative
                                    //erased reads as ff (so avoid that)
  if(temp==0||temp==1)      //if sign is invalid, don't change oscillator
  {
      if(temp==0)
	  {
	     temp=eeprom_read_byte((void *)0);
		 if(temp != 0xff) OSCCAL+=temp;
	  }
	  else
	  {
	     temp=eeprom_read_byte((void *)0);
		 if(temp!=0xff) OSCCAL -=temp;
	  }
  }
}


void init_pwm(void)
{
  // **************************************************************
  // *     Timer 1                                                *
  // **************************************************************
  
  DDRB |= (1<<PB1);  //set OC1A as an output
  OCR1A=19999;    //set initial compare at 50%
  ICR1=39999U; // 8 MHz /40000/2 = PWM frequency = 100 Hz
  TCCR1A = (1<<COM1A1); //zeros in COM1B1,COM1B0,WGM11,WGM10  
  //internal clock, no prescaler , PWM mode 8
  TCCR1B = (1<<WGM13) | (1<<CS10);
}


