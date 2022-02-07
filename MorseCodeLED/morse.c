/*
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    Blinky Hello World.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */

#define DOT 250
#define DASH 750
#define SPACE 750
char morseCode[26][5] ={".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..",
"--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};

void dotOrDash(int);
void delay_ms(unsigned short);
void displayMorse(char*, int);


int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    PRINTF("Hello World\n");

    SIM->SCGC5 |= 1<<12;
    PORTD->PCR[5] &= ~(0x700);
    PORTD->PCR[5] |= 0x700 & (1 << 8);
    GPIOD->PDDR |= (1 << 5);

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */

    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        displayMorse("Hello World", strlen("Hello World"));
        __asm volatile ("nop");
    }
    return 0 ;
}

//symbolic constants are defined as DOT and DASH respectively
void dotOrDash(int time){
	GPIOD->PTOR |= (1<<5);
	delay_ms(time);
	GPIOD->PSOR |= (1<<5);
}

void displayMorse(char *message, int length){
	char letter;

	//for loop iterates through each letter in the message
	for(int i = 0; i < length; i++, message++){

		if(*message == ' '){
			delay_ms(1000);
			continue;
		}
		else
			letter = toupper(*message);

		printf("%c ", *message); //current letter
		char *morsePtr = &morseCode[letter - 65]; //65 offset is to shift index down to 0 to avoid wasting memory

		//while loop iterates through morse sequence for each symbol
		while(*morsePtr != '\0'){
			printf("%c ", *morsePtr); //print morse code sequence to console
			if(*morsePtr == '.')
				dotOrDash(DOT);
			else if(*morsePtr == '-')
				dotOrDash(DASH);


			if(*(morsePtr + 1) != '\0')  //if on the last symbol in a sequence, skip the wait
				delay_ms(250);

			morsePtr++;
		}
		delay_ms(750);  //time between letters
		printf("\n");
	}
}

void delay_ms(unsigned short delay_t) {

    SIM->SCGC6 |= (1 << 24); // Clock Enable TPM0
    SIM->SOPT2 |= (0x2 << 24); // Set TPMSRC to OSCERCLK
    TPM0->CONF |= (0x1 << 17); // Stop on Overflow
    TPM0->SC = (0x1 << 7) | (0x07); // Reset Timer Overflow Flag, Set Prescaler 128
    TPM0->MOD = delay_t * 61 + delay_t/2; //

    TPM0->SC |= 0x01 << 3; // Start the clock!

    while(!(TPM0->SC & 0x80)){} // Wait until Overflow Flag
    return;
}
