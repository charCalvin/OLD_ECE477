// This file was originally written by Bruce Segee at the University of Maine Orono
// It has been edited by Calvin Skinner for use in ECE 477 Lab 6 

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

void from_to(int ,int);
void setup_stdin(void);
int init(int baud);

int main(int argc, char *argv[]){	// todo command line arguments (DONE)
	// Print message about setting up the UART channel on the pi
	// Where is config.txt
	printf("Remember to add \"enable_uart=1\" in the file config.txt\n");
	printf("and take out \"console=serial0,115200\" in the same file.\n\n");

	// Get baud rate form arguments passed from CMD LINE
	int BAUD;
	char *serial_str;
	if (argc != 3) { // we are looking 3 args: default(0), Buadrate(1), serial port(2) 
		prinf("error: number of arguments is incorrect.\n\n");
		return -1;
	} else {
		BAUD = atoi(argv[1]); // index 1 is the second arg and is the first arg passed by the user
		serial_str = argv[2];
		printf("Using Buadrate: %d\n", BAUD);
		printf("Using serial port: %s\n", *serial_str);
	}

 	int fd1;
 	char buf[1000];
 	setup_stdin();
 	fd1=init();

 	//todo make sure fd1 is ok
 	if(fork()) from_to(fd1,1);
 	else from_to(0,fd1);

	//todo sure would be nice to have an exit condition
	return 1;
 }

void setup_stdin(void){ 
 	struct termios tc;
 	tcgetattr(0, &tc);
 	tc.c_lflag &=~ICANON;
 	tc.c_lflag &=~ECHO;
 	tcsetattr(0, TCSANOW, &tc);
}


void from_to(int f1, int f2){
	char c;
	while(1) if(read(f1,&c,1))write(f2,&c,1); // WTF is this????
}

int  init(int baud){
    int fd1;
    struct termios tc;                // terminal control structure

    //todo serial port should not be hard coded
    fd1 = open("/dev/ttyS0", O_RDWR|O_NOCTTY);  // really ought to check for error
    tcgetattr(fd1, &tc);
    tc.c_iflag = IGNPAR;
    tc.c_oflag = 0;
    tc.c_cflag = CS8 | CREAD | CLOCAL; //8 bit chars enable receiver no modem status lines
    tc.c_lflag =0 ;

    //todo baud rate should not be hard coded (DONE)
    cfsetispeed(&tc, baud);
    cfsetospeed(&tc, baud);

    //todo should have bits per character set
    tcsetattr(fd1, TCSANOW, &tc);

    return fd1;
 }
