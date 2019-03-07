#!/bin/sh
#
# Compile any C file for ATMEGA88 IC:
# ARGUMENTS: [filename.c]
# 	- Create a.out
#	- Create a.hex
#	- Program the atmega IC

#first argument is the file to be compile
echo "Compiling file: $1"

# Compile and creaete the a.out file
avr-gcc -mmcu=atmega88 $1

# Extract data from a.out and create the a.hex file
avr-objcopy -j .text -j .data -O ihex a.out a.hex

# Program the IC
sudo avrdude -C ~/avrdude_gpio.conf -c pi_1 -p atmega88 -U flash:w:a.hex:i

echo "Script complete"
