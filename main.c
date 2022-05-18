/*
 * Final Project - Motion Based Welcome Program
 * Author : Jace Johnson
 * Created: 5/3/2020 9:31:22 PM
 *	  Rev 1 5/3/2020
 * Description:	ATMEGA2560 takes in information from a PIR motion sensor and
 *				outputs a signal to an LED and to an LCD screen and sends 
 *				information to a computer over SSH. This is to simulate a home
 *				security system, where the PIR sensor and LED represent a 
 *				motion sensing light, the LCD screen represents a home 
 *				security panel interface, and the computer represents an 
 *				alert system for the home owner.
 *
 * Hardware:	ATMega 2560 operating at 16 MHz
 *				Computer connected to ATMega using SSH
 *				LCD screen
 *				10 KOhm potentiometer (POT)
 *				PIR Motion Sensor
 *				Red LED
 *				Breadboard Power Module
 *				jumper wires
 * Configuration shown below
 *
 * ATMega 2560
 *  PORT   pin		   LCD Screen
 * -----------         ----------
 * |		 |	  GND--|K		|
 * |		 |	   5V--|A		|
 * | A7    29|---------|D7		|
 * | A6    28|---------|D6		|
 * | A5    27|---------|D5		|
 * | A4    26|---------|D4		|
 * |      	 |         |		|
 * | C1    52|---------|E		|
 * |		 |	  GND--|RW		|	10KOhm Potentiometer
 * | C0    53|---------|RS		|			POT
 * |      	 |         |		|		-----------
 * |		 |		   |	  V0|-------|W		5V|--5V
 * |	     |    5V---|VDD		|		|	   GND|--GND
 * |	  GND|---GND---|VSS		|		-----------
 * |		 |		   ----------
 * |		 |
 * |		 |		PIR motion Sensor
 * |		 |         ----------
 * |		 |	  GND--|GND		|
 * |		 |	   5V--|Vcc		|
 * | L0	   49|---------|Vout	|
 * |		 |		   ----------
 * |		 |
 * |		 |			   LED
 * |		 |         ----------
 * |		 |	  GND--|Cathode	|
 * | L2	   47|-1 KOhm--|Anode	|
 * -----------         ----------
 */

#define F_CPU 16000000

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "LCD.h"	//custom header files
#include "USART0.h"

void printStatusWindow(int lightOn);
void LCDMessage(int status);

/*
 * Function:  main
 *  Calls functions to initialize USART0 and the LCD. also sets up input and 
 *	output pins for PIR sensor and LED. Also repeatedly calls functions to 
 *	print Messages to LCD screen and computer terminal
 *
 *  returns:  1   program ends unexpectedly
 */
int main(void)
{
	sei();	//enable global interrupts
	//initialize serial port (USART0) and LCD screen
	initUSART0();
	LCD_init();
	
	DDRL &= !(1<<0);	//set pin 0 as input for motion sensor
	DDRL |= (1<<2);		//set pin 2 as output for LED
	
    while(1){
		LCDMessage(PINL & (1<<0));			//print welcome message on LCD
		printStatusWindow(PINL & (1<<0));	//print status to computer screen
	}
	
    return 1;
}

/*
 * Function:  printStatusWindow
 *  Takes input and sends the status of the LED light to the computer's 
 *	terminal.
 *	lightOn	int		flag for status of light
 *
 *  returns:  none
 */
void printStatusWindow(int lightOn){
	if(lightOn){
		fprintf(&USART0_OUT, "Light is on\n");
	}
	else{
		fprintf(&USART0_OUT, "Light is off\n");
	}
	return;
}

/*
 * Function:  LCDMessage
 *  Changes light state and prints message to LCD screen depending on input 
 *	status value.
 *	status	int		flag for status of PIR sensor
 *
 *  returns:  none
 */
void LCDMessage(int status){
	int line = 0;				//first line
	char str[16] = "Welcome";	//first half of welcome message
	
	//check status of sensor
	if(status){				//if sensor reads true
		PORTL |= (1<<2);	//turn on light
		
		outputLine(str, &line, 0);	//print first line to LCD
		
		line = 1;
		strcpy(str, "Homeowner!");	//print second line to LCD
		outputLine(str, &line, 0);
	}
	else{						//if sensor reads false
		PORTL &= !(1<<2);		//turn off light
		
		LCD_clear_line(&line);	//clear LCD
		line = 1;
		LCD_clear_line(&line);
	}
	return;
}