/*
 * Space_Buddies
 *
 *  Created: 06/06/2014 22:56:55
 *  Author: Justin Nel
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>
#include "Space-Buddies.h"
#include "Space-Tunes.h"


#define MAXPULSE 65000

long vel = 20000;
uint16_t pulses[400];
uint8_t currentPulse = 0;
uint16_t highpulse = 0, lowpulse = 0;

/*
	State Summary:
	0: check for data
	1: read data
	2: send data
	3: play tune
*/
uint8_t state = 0;
uint8_t check_count = 0;
uint8_t counter = 0;
uint8_t buffer[50];
uint8_t currentBit = 0;
uint8_t currentByte = 0;
uint8_t crc = 0;
bool last_set_high = false;
volatile uint8_t overflow_counter = 0;

//TODO: Need logic to set "current tune", toggled by button press
//TODO: Move buffer into EEPROM when data is successful
//TODO: Implement button interrupts to handle button events
//TODO: Implement timer/counter to handle flashing LEDs when state==3
//TODO: Before each send, blink the current tune's LED colour

int main(void)
{
	// Set RGB LEDs as output
	DDRD |= (1 << PD0);
	DDRD |= (1 << PD1);
	DDRD |= (1 << PD2);
	DDRB |= (1 << PB0);
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB2);
	// Set buttons as input
	DDRD &= ~(1 << PD3);
	DDRD &= ~(1 << PD7);
	// Set speaker as output
	DDRD |= (1 << PD5);
	// Set IR Transmitter as output
	DDRD |= (1 << PD6);
	// Set IR Receiver as input
	DDRC &= ~(1 << PC5);
	// Set internal pull-up for IR Receiver
	PORTC |= (1 << PC5);
	
	_delay_ms(100);
	
    while(1)
    {
		switch(state)
		{
			case 3:
			play(buffer);
			break;
			case 2:
			send_data(mario);
			break;
			default:
			case 1:
			read_ir_data();
			break;
		}
    }
}

void read_ir_data()
{
	while(1)
	{
		// Start out with no pulse length
		highpulse = lowpulse = 0;
		
		// While pin is high
		while(DDRC & (1 << PC6))
		{
			highpulse++;
			_delay_us(26);
			
			/*
			If the pulse is too long, we have timed out
			Either nothing was received or the code is finished
			Process what we've grabbed so far and then reset
			*/
			if ((highpulse >= MAXPULSE) && (currentPulse != 0))
			{
				processData();
				currentPulse = 0;
				return;
			}
		}
		
		// While pin is low
		while(!(DDRC & _BV(PC6)))
		{
			lowpulse++;
			_delay_us(26);
			if ((lowpulse >= MAXPULSE) && (currentPulse != 0))
			{
				processData();
				currentPulse = 0;
				return;
			}
		}
		
		pulses[currentPulse] = lowpulse;
		
		// We've read one high, one low, now let's do another
		currentPulse++;
	}
}

void processData()
{
	// we only want full bytes, which requires 8 bits
	if ((currentPulse+1)%8 != 0)
	{
		// this is bad data, escape!
		state = 2;
		return;
	}
	
	currentBit = 0;
	currentByte = 0;
	
	for (uint8_t i = 0; i < currentPulse; i++)
	{
		if (pulses[i] >= 900 || pulses[i] <= 1500)
		{
			// this is a 1
			buffer[currentByte] = buffer[currentByte] | (1 << currentBit);
		} else if (pulses[i] >= 400 || pulses[i] <= 750)
		{
			// this is a 0
		} else {
			// bad data, escape!
			memset(buffer, 0, 50);
			state = 2;
			return;
		}
		if (++currentBit == 8)
		{
			currentBit = 0;
			currentByte++;
		}
	}
	buffer[currentByte] = END_MARKER;
	state = 3;
}

void send_data(uint8_t *pByte)
{
	while(*pByte != END_MARKER)
	{
		currentBit = 0;
		while (currentBit < 8)
		{
			if (READ_BIT(*pByte, currentBit) == 1) {
				sendIR(1000);
			} else {
				sendIR(500);
			}
			_delay_us(500);
			currentBit++;
		}
		++pByte;
	}
}

void sendIR(int cycles)
{
	while(cycles--)
	{
		//TODO: Extract these into macros
		PORTD |= (1 << PD6);
		_delay_us(10);
		PORTD &= ~(1 << PD6);
		_delay_us(10);
	}
}

void delay_ms(uint16_t count)
{
	while(count--)
	{
		_delay_ms(1);
	}
}

void delay_us(uint16_t count)
{
	while(count--)
	{
		_delay_us(1);
	}
}

void play(uint8_t *pByte)
{
	while(*pByte != END_MARKER)
	{
		int tone = 0;
		int duration = 0;
		switch (GET_TONE(*pByte)) {
			case T_C:
			tone = 1911;
			break;
			case T_CS:
			tone = 1804;
			break;
			case T_D:
			tone = 1703;
			break;
			case T_EB:
			tone = 1607;
			break;
			case T_E:
			tone = 1517;
			break;
			case T_F:
			tone = 1432;
			break;
			case T_FS:
			tone = 1352;
			break;
			case T_G:
			tone = 1276;
			break;
			case T_AB:
			tone = 1204;
			break;
			case T_A:
			tone = 1136;
			break;
			case T_BB:
			tone = 1073;
			break;
			case T_B:
			tone = 1012;
			break;
			case T_CX:
			tone = 955;
			break;
			case T_CSX:
			tone = 902;
			break;
			case T_DX:
			tone = 851;
			break;
			case T_EBX:
			tone = 803;
			break;
			case T_EX:
			tone = 758;
			break;
			case T_FX:
			tone = 716;
			break;
			case T_FSX:
			tone = 676;
			break;
			case T_GX:
			tone = 638;
			break;
			case T_ABX:
			tone = 602;
			break;
			case T_AX:
			tone = 568;
			break;
			case T_BBX:
			tone = 536;
			break;
			case T_BX:
			tone = 506;
			break;
		}
		switch (GET_DURATION(*pByte)) {
			case 0:
			duration = 2;
			break;
			case 1:
			duration = 4;
			break;
			case 2:
			duration = 6;
			break;
			case 3:
			duration = 8;
			break;
			case 4:
			duration = 12;
			break;
			case 5:
			duration = 16;
			break;
			case 6:
			duration = 18;
			break;
			case 7:
			duration = 24;
			break;
		}
		long tvalue = duration * vel;
		play_tone(tone, tvalue);
		++pByte;
	}
}

void play_tone(int tone, long tempo_value)
{
	long tempo_gasto = 0;
	while (tempo_gasto < tempo_value && tempo_value < 640000) // enters an infinite loop when tempo_value is a big value
	{
		PORTD |= (1 << PD5);
		delay_us(tone / 2);
		
		PORTD &= ~(1 << PD5);
		delay_us(tone / 2);
		
		tempo_gasto += tone;
	}
}