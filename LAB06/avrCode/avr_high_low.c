#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define F_CPU 1000000UL // AVR clock speed 1MHz
#include <util/delay.h>

int serial_send_char(char, FILE *);
int serial_receive_char(FILE *);
static FILE serial_stream = FDEV_SETUP_STREAM (serial_receive_char, serial_send_char, _FDEV_SETUP_RW);

void init_serial(void);
void do_high_low(void);

// this is the high_low server running on serial on an avr
// it sets up the serial communication for 1200 baud 
// note that the random number generator will always generate the same sequence
int main(void)
{ 
  init_serial();
  _delay_ms(2000);
  while(1) 
  { 
     do_high_low();
  }   
  return 0;
}

// Initializes AVR USART for 1200 baud (assuming 1MHz clock)
// 1MHz/(16*(51+1)) = 1202 about 0.2% error
void init_serial(void)
{
   UBRR0H=0; // UBRR0_HIGH regitster used to set buad rate
   UBRR0L=12; // UBRR0_LOW registe used to set baud rate: 1200 BAUD FOR 1MHZ SYSTEM CLOCK
   UCSR0A= 1<<U2X0; // CSR = control status register A: setting U2X0 doubles the transmission speed 
   UCSR0C= (1<<USBS0)|(3<<UCSZ00) ;  // 8 BIT NO PARITY 2 STOP
   UCSR0B=(1<<RXEN0)|(1<<TXEN0)  ; // ENABLE TX AND RX ALSO 8 BIT
   stdin=&serial_stream; // what is serial_stream and how does it correlate to UDR0 and the serial_send_char()
   stdout=&serial_stream;

}   

// This code is essentially identical to the high_low_server code that
// is one of the examples on the 477 page.  Essentially file I/O.
// stdin/stdout, and serial I/O are all incredibly similar and almost 
// interchangable
void do_high_low(void)
{
   FILE *fp, *fpr; // file pointers for serial data
   int answer, guess;	// answer stores the randomly generated answer
  			// guess stores the user input for the guessing game
   answer=rand()%10000; // generate random number between 0-1000
   fp=stdout; // fp points to standard out
   fpr=stdin; // fpr points to standard in
 
  fprintf(fp,"Welcome to the high-low guessing game \r\n");
  fprintf(fp,"please enter integer guesses between 0 and 10000\r\n");
 
  guess=answer+1;
  while(guess!=answer)
  {   	while(fscanf(fpr,"%d",&guess)!=1) fscanf(fpr,"%*s");
 	if(guess<answer) fprintf(fp,"Too low, try again \r\n");
    	else if (guess>answer) fprintf(fp,"Too High, try again \r\n");
	else fprintf(fp,"Congratualtions you got it right\r\n");
 
  }
   
}

// Send a char: wait for UDRE0 to be set
// UDRE0 is set when the UDR0 data register is empty
// set the UDRn data register with data
// No parity bits and no error checking
int serial_send_char(char val, FILE * fp)
{
  while((UCSR0A&(1<<UDRE0)) == 0); //wait until empty 
   UDR0 = val;
   return 0;
}

// Get a char: wait for RXC0 bit to be set
// RXC0 is set when a char has been received
// Then return the char that has been stored in UDR0
// No parity bits and no error checking
int serial_receive_char(FILE * fp)
{
   while((UCSR0A&(1<<RXC0)) == 0);  //WAIT FOR CHAR
   return UDR0;
}     


