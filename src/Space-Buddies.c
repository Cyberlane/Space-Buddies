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

long vel = 20000;

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

//TODO: Need logic to set "current tune", toggled by button press
//TODO: Move buffer into EEPROM when data is successful
//TODO: Implement button interrupts to handle button events
//TODO: Implement timer/counter to handle flashing LEDs when state==3
//TODO: Before each send, blink the current tune's LED colour

int main(void)
{
	// Macro untested, left in case macro is wrong!
	//DDRD |= 0b01000000; // Set pin 6 to OUTPUT
	PINMODE_OUTPUT(6);
	
	delay_ms(2000);
	
    while(1)
    {
		switch(state)
		{
			case 1:
				read_data();
				break;
			case 2:
				send_data(mario);
				break;
			case 3:
				play(mario);
				break;
			default:
				check_data();
				break;
		}
    }
}

void check_data()
{
	check_count = 0;
	while(1)
	{
		if (READ_PIN(6) == 1) {
			if (check_count++ == 250) {
				state = 1;
				return;
			}
		} else {
			state = 2;
			return;
		}
	}
}

// Trigger this after receiving over 250 cycles of 38kHz from IR of HIGH
void read_data()
{
	// high pin for 1-100 = 0
	// high pin for 101-200 = 1
	last_set_high = false;
	crc = 0;
	counter = 0;
	while (1) // Should I have a value here to prevent infinite loop bugs?
	{
		if (READ_PIN(6) == 1) {
			if (last_set_high) {
				counter++;
			} else {
				last_set_high = true;
				counter = 1;
			}
		} else {
			if (last_set_high) {
				if (counter >= 125 && counter <= 200) {
					buffer[currentByte] = buffer[currentByte] | (1 << currentBit);
					// we've received a 1, set that on the next bit in the current byte!
				} else if (counter >= 25 && counter <= 100) {
					// we've received a 0, no need to set it as the byte would be empty to begin!
				} else {
					// Bad data!
					memset(buffer, 0, 50);
					currentBit = 0;
					currentByte = 0;
					state = 0;
					return;
				}
				if (currentBit == 7) {
					crc = crc ^ buffer[currentByte];
					if (currentByte == 49) {
						// We're full!
						if (crc != buffer[currentByte]) {
							// CRC failed
							memset(buffer, 0, 50);
							currentBit = 0;
							currentByte = 0;
							state = 0;
						} else {
							buffer[currentByte] = END_MARKER;
							state = 3;
						}
						return;
					}
					currentBit = 0;
					buffer[currentByte++] = 0;
				} else {
					currentBit++;
				}
				last_set_high = false;
				counter = 0;
			} else {
				if (counter >= 1000) {
					// timeout
					if (currentBit != 0 || crc != buffer[currentByte]) {
						// Not a complete byte or CRC failed
						memset(buffer, 0, 50);
						currentBit = 0;
						currentByte = 0;
						state = 0;
					} else {
						buffer[currentByte] = END_MARKER;
						state = 3;
					}
					return;
				}
				counter++;
			}
		}
		delay_us(26);
	}
}

void send_data(uint8_t *pByte)
{
	send_signal(250); // Would I need to send no signal before I begin?
	while(*pByte != END_MARKER)
	{
		currentBit = 0;
		while (currentBit < 8)
		{
			if (READ_BIT(*pByte, currentBit) == 1) {
				send_signal(100);
				} else {
				send_signal(200);
			}
			no_signal(50);
			currentBit++;
		}
		++pByte;
	}
}

void no_signal(int cycles)
{
	delay_us(cycles * 26);
}

void send_signal(int cycles)
{
	while(cycles--)
	{
		//TODO: Extract these into macros
		PORTD |= 0b00100000;
		delay_us(6);
		PORTD &= 0b11011111;
		delay_us(20);
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
	int counter = 1703;
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
		tocar(tone, tvalue);
		++pByte;
	}
}

void tocar(int tom, long tempo_value)
{
	long tempo_gasto = 0;
	while (tempo_gasto < tempo_value && tempo_value < 640000) // enters an infinite loop when tempo_value is a big value
	{
		PORTD |= 0b01000000; // Drive pin 6 to HIGH
		delay_us(tom / 2);
		
		PORTD &= 0b10111111; // Drop pin 6 to LOW
		delay_us(tom / 2);
		
		tempo_gasto += tom;
	}
}