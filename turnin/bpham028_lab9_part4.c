/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/U5E6PhsGEyM
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
enum SpeakerStates {Start, Init, On} SpeakerState;
enum FreqStates {FStart, FInit, FUp, FDown, FRelease} FreqState;

unsigned char threeLEDs = 0x00;
unsigned char blinkingLED = 0x00;
unsigned char speaker = 0x00;
unsigned char cnt = 0x00;
unsigned char freq = 0x00;

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
			PORTB = (blinkingLED | threeLEDs | speaker);
			break;
		default:
			break;
	}
}

void tick_speaker() {
        switch (SpeakerState) {
                case Start:     
		       SpeakerState = Init; 
		       break;
                case Init:        
			if ((~PINA & 0x07) == 0x04) {
				SpeakerState = On;
				cnt = 0;
			} else {
				SpeakerState = Init;
			} break;
		case On:
			if ((~PINA & 0x07) == 0x04) {
				SpeakerState = On;
			}
			else {
				SpeakerState = Init;
			} break;
                default:   
	   		SpeakerState = Start; 
			break;
        }
        switch (SpeakerState) {
                case Start:      
			break;
                case Init:        
			speaker = 0x00; 
			break;
		case On:		
			if (cnt < freq) {
				speaker = 0x10;
			} else if (cnt < (freq * 2)){
				speaker = 0x00;
			} else if (cnt >= (freq * 2)) {
				cnt = 0;
			} ++cnt;
			break;
                default:
			break;
        }
}

void tick_speakerchange() {
	switch(FreqState) {
		case(FStart):
			FreqState = FInit;
			break;
		case(FInit):
			if ((~PINA & 0x07) == 0x01) {
				FreqState = FUp;
			} else if ((~PINA & 0x07) == 0x02) {
				FreqState = FDown;
			} else {
				FreqState = FInit;
			} break;
		case(FUp):
			FreqState = FRelease;
			break;
		case(FDown):
			FreqState = FRelease;
			break;
		case(FRelease):
			if ((~PINA & 0x07) == 0x00) {
				FreqState = FInit;
			} else {
				FreqState = FRelease;
			} break;
		default:
			FreqState = FStart;
			break;
	}
	switch(FreqState) {
		case(FStart):
			freq = 0x00;
			break;
		case(FInit):
			break;
		case(FUp):
			--freq;
			break;
		case(FDown):
			++freq;
			break;
		case(FRelease):
			break;
		default:
			break;
	}
}

int main(void) {
	unsigned long Three_elapsedTime = 0; //blinks every 300 ms
	unsigned long Blink_elapsedTime = 0; //blinks every 1000 ms
	const unsigned long timerPeriod = 1;
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1);
	TimerOn();
	//ThreeLEDState = ThreeLEDStart;
	//BState = BlinkStart;
	//CombineLEDState = CombineLEDStart;
	while(1) {
		if (Three_elapsedTime >= 300) {
			tick_three();
			Three_elapsedTime = 0;
		}
		if (Blink_elapsedTime >= 1000) {
			tick_blink();
			Blink_elapsedTime = 0;
		}
		tick_speakerchange();
		tick_speaker();
		tick_combine();
		while(!TimerFlag){}
		TimerFlag = 0;
		Three_elapsedTime += timerPeriod;
		Blink_elapsedTime += timerPeriod;
	}
	return 0;
}
