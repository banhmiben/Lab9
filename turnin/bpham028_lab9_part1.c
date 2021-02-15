/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/rCoShBmwaVk
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

enum ThreeLEDStates {ThreeLEDStart, ThreeLEDInit, ThreeLEDZero, ThreeLEDOne, ThreeLEDTwo} ThreeLEDState;
enum BStates {BlinkStart, BlinkInit, BlinkOn, BlinkOff} BState;
enum CombineLEDStates {CombineLEDStart, CombineLEDInit} CombineLEDState;

unsigned char threeLEDs = 0x00;
unsigned char blinkingLED = 0x00;

void TimerSet(unsigned long M) {
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}

void TimerOn() {

	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;

}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {

	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}

}

void tick_three() {
	switch(ThreeLEDState) {
		case(ThreeLEDStart):
			ThreeLEDState = ThreeLEDInit;
			break;
		case(ThreeLEDInit):
			ThreeLEDState = ThreeLEDZero;
			break;
		case(ThreeLEDZero):
			ThreeLEDState = ThreeLEDOne;
			break;
		case(ThreeLEDOne):
			ThreeLEDState = ThreeLEDTwo;
			break;
		case(ThreeLEDTwo):
			ThreeLEDState = ThreeLEDZero;
			break;
		default:
			ThreeLEDState = ThreeLEDStart;
			break;
	}

	switch(ThreeLEDState) {
		case(ThreeLEDStart):
			break;
		case(ThreeLEDInit):
//			threeLEDs = 0x00;
			break;
		case(ThreeLEDZero):
			threeLEDs = 0x01;
			break;
		case(ThreeLEDOne):
			threeLEDs = 0x02;
			break;
		case(ThreeLEDTwo):
			threeLEDs = 0x04;
			break;
		default:
			break;
	}
}

void tick_blink() {
	switch(BState) {
		case(BlinkStart):
			BState = BlinkInit;
			break;
		case(BlinkInit):
			BState = BlinkOn;
			break;
		case(BlinkOn):
			BState = BlinkOff;
			break;
		case(BlinkOff):
			BState = BlinkOn;
			break;
		default:
			BState = BlinkStart;
			break;
	}

	switch(BState) {
		case(BlinkStart):
			break;
		case(BlinkInit):
			blinkingLED = 0x00;
			break;
		case(BlinkOn):
			blinkingLED = 0x08;
			break;
		case(BlinkOff):
			blinkingLED = 0x00;
			break;
		default:
			break;
	}
}

void tick_combine() {
	switch(CombineLEDState) {
		case(CombineLEDStart):
			CombineLEDState = CombineLEDInit;
			break;
		case(CombineLEDInit):
			CombineLEDState = CombineLEDInit;
			break;
		default:
			CombineLEDState = CombineLEDStart;
			break;
	}
	switch(CombineLEDState) {
		case(CombineLEDStart):
			break;
		case(CombineLEDInit):
			PORTB = (blinkingLED | threeLEDs);
			break;
		default:
			break;
	}
}

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
//	ThreeLEDState = ThreeLEDStart;
//	BState = BlinkStart;
//	CombineLEDState = CombineLEDStart;
	while(1) {
		tick_three();
		tick_blink();
		tick_combine();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
	return 0;
}
