/*
 * Space_Buddies
 *
 *  Created: 06/06/2014 22:56:55
 *  Author: Justin Nel
 */ 

#define F_CPU 8000000UL

#include "avr/eeprom.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>
#include "Space-Buddies.h"
#include "Space-Tunes.h"
#include "Space-Colours.h"
#include "Hardware.h"

// Infrared
#define MAXPULSE 65000
#define IR_RESOLUTION 22
#define IR_HALFLIFE 11
#define IR_ZERO 15
#define IR_ONE 25
#define IR_ZERO_LOWER 12
#define IR_ZERO_UPPER 20
#define IR_ONE_LOWER 22
#define IR_ONE_UPPER 30
#define IR_DELAY 250
// Buttons
#define BUTTON_HOLD 15
// Audio
#define vel 10000l;//1.25;

typedef enum {
	STATE_INITIALISE,
	STATE_MAIN,
	STATE_RECEIVE,
	STATE_SEND,
	STATE_PLAY,
	STATE_RESET,
	STATE_SKIP
} state_t;

state_t state = STATE_INITIALISE;
uint8_t currentTune = 0; // 0-9

int main(void)
{
	// Set RGB LEDs as output
	BLUE_L_DDR |= (1 << BLUE_L);
	GREEN_L_DDR |= (1 << GREEN_L);
	RED_L_DDR |= (1 << GREEN_L);
	RED_R_DDR |= (1 << RED_R);
	GREEN_R_DDR |= (1 << GREEN_R);
	BLUE_R_DDR |= (1 << BLUE_R);
	// Set buttons as input
	BUTTON_LEFT_DDR &= ~(1 << BUTTON_LEFT);
	BUTTON_RIGHT_DDR &= ~(1 << BUTTON_RIGHT);
	// Set speaker as output
	SPEAKER_DDR |= (1 << SPEAKER);
	// Set IR Transmitter as output
	IR_TX_DDR |= (1 << IR_TX);
	// Set IR Receiver as input
	IR_RX_DDR &= ~(1 << IR_RX);
	// Set internal pull-up for IR Receiver
	IR_RX_ON();
	
	_delay_ms(100);
	timer_init();
	
    while(1)
    {
		switch(state)
		{
			case STATE_INITIALISE:
			{
				intialise_game();
				state = STATE_MAIN;
				break;
			}
			case STATE_MAIN:
			{
				state = check_buttons();
				break;
			}
			case STATE_RECEIVE:
			{
				RED_L_ON();
				GREEN_R_ON();
				state = read_ir_data();
				RED_L_OFF();
				GREEN_R_OFF();
				break;
			}
			case STATE_SEND:
			{
				GREEN_R_ON();
				send_data(currentTune);
				_delay_ms(1000);
				GREEN_R_OFF();
				state = STATE_MAIN;
				break;
			}
			case STATE_PLAY:
			{
				play_tune(currentTune);
				state = STATE_MAIN;
				break;
			}
			case STATE_SKIP:
			{
				currentTune = find_next_tune(currentTune);
				set_colour(&colours[currentTune]);
				TIMER2_START();
				_delay_ms(1000);
				TIMER2_STOP();
				_delay_ms(100);
				clear_leds();
				state = STATE_MAIN;
				break;
			}
			case STATE_RESET:
			{
				state = reset_game();
				break;
			}
			default:
			{
				state = STATE_MAIN;
				break;
			}
		}
    }
}

void timer_init(void)
{
	// Timer 2
	TCCR2 = (1 << WGM21)|(1 << CS21);
	OCR2 = 65;
	// Turn on interrupts
	sei();
}

void intialise_game(void)
{
	uint8_t selected = 99;
	uint8_t i = 0;
	
	while(i < 10)
	{
		if (get_tune_state(i))
		{
			selected = i;
			break;
		}
		i++;
	}
	if (selected == 99)
	{
		i = 0;
		TIMER2_START();
		while(1)
		{
			i = i % 10;
			set_colour(&colours[i]);
			_delay_ms(500);
			if (is_left_button_pressed() || is_right_button_pressed())
			{
				break;
			}
			i++;
		}
		TIMER2_STOP();
		make_tune_available(i);
	}
	_delay_ms(500);
	currentTune = find_next_tune(currentTune);
	//play_tune(currentTune);
	clear_leds();
}

state_t check_buttons(void)
{
	/*
		hold and release a single button for under 2 seconds = single press
		hold and release a single button for over 2 seconds = long press
		hold both buttons for over 10 seconds = reset


		left press = send
		left hold = receive
		right press = skip/next
		right hold = play current
		both hold = reset
		
		anything else is just invalid and upon release of both buttons does nothing other than reset the counter
	*/
	
	uint8_t leftCounter = 0;
	uint8_t rightCounter = 0;
	
	while (1) {
		uint8_t left = is_left_button_pressed();
		uint8_t right = is_right_button_pressed();
		if (!left && !right) {
			break;
		}
		leftCounter += left ? 1 : 0;
		rightCounter += right ? 1 : 0;
		
		if (leftCounter == 0) {
			RED_L_OFF();
			GREEN_L_OFF();
		} else if (leftCounter <= BUTTON_HOLD) {
			RED_L_ON();
		} else {
			RED_L_OFF();
			GREEN_L_ON();
		}
		if (rightCounter == 0) {
			RED_R_OFF();
			GREEN_R_OFF();
		} else if (rightCounter <= BUTTON_HOLD) {
			RED_R_ON();
		} else {
			RED_R_OFF();
			GREEN_R_ON();
		}
		_delay_ms(200);
	}
	
	RED_L_OFF();
	GREEN_L_OFF();
	RED_R_OFF();
	GREEN_R_OFF();
	
	if (leftCounter > 0 && leftCounter <= BUTTON_HOLD && rightCounter == 0) {
		return STATE_SEND;
	} else if (rightCounter > 0 && rightCounter <= BUTTON_HOLD && leftCounter == 0) {
		return STATE_SKIP;
	} else if (rightCounter > BUTTON_HOLD && leftCounter == 0) {
		return STATE_PLAY;
	} else if (leftCounter > BUTTON_HOLD && rightCounter == 0) {
		return STATE_RECEIVE;
	} else if (leftCounter > BUTTON_HOLD && rightCounter > BUTTON_HOLD) {
		return STATE_RESET;
	} else {
		return STATE_MAIN;
	}
}

uint8_t read_ir_data(void)
{
	uint8_t buffer[50];
	uint8_t currentBit = 0;
	uint8_t currentByte = 0;
	uint16_t currentPulse = 0;
	uint16_t highpulse = 0;
	uint16_t lowpulse = 0;
	
	while(1)
	{
		// Start out with no pulse length
		highpulse = lowpulse = 0;
		
		/* wait for a falling edge */
		while ((PINC & (1 << PC5)) && highpulse < MAXPULSE) {
			highpulse++;
			_delay_us(IR_RESOLUTION);
		};
		
		if (highpulse >= MAXPULSE)
		{
			return validate_buffer(currentPulse, currentBit, currentByte, buffer, 1);
		}
		
		/* pin isn't high anymore. See how long it's low */
		
		cli();
		while (((PINC & (1 << PC5)) == 0) && lowpulse < MAXPULSE) {
			lowpulse++;
			_delay_us(IR_RESOLUTION);
		};
		sei();
		
		// While pin is low
		if (lowpulse >= MAXPULSE)
		{
			return validate_buffer(currentPulse, currentBit, currentByte, buffer, 2);
		}
		
		if (lowpulse >= IR_ONE_LOWER && lowpulse <= IR_ONE_UPPER)
		{
			// this is a 1
			buffer[currentByte] |= (1 << currentBit);
		}
		else if (lowpulse >= IR_ZERO_LOWER && lowpulse <= IR_ZERO_UPPER)
		{
			// this is a 0
			buffer[currentByte] &= ~(1 << currentBit);
		}
		else
		{
			show_signal(lowpulse);
			return STATE_MAIN;
		}
		
		if (++currentBit == 8)
		{
			currentBit = 0;
			currentByte++;
		}
		
		currentPulse++;
	}
}

void send_data(volatile uint8_t index)
{
	send_IR_byte(index);
	const uint8_t *ptr = stored_tunes.tunes[index];
	for(uint8_t data = eeprom_read_byte(ptr++); data != END_MARKER; data = eeprom_read_byte(ptr++))
	{
		send_IR_byte(data);
	}
}

void play_tune(uint8_t currentTune)
{
	TIMER2_START();
	const uint8_t *ptr = stored_tunes.tunes[currentTune];
	for(uint8_t data = eeprom_read_byte(ptr++); data != END_MARKER; data = eeprom_read_byte(ptr++))
	{
		play_byte(data);
	}
	TIMER2_STOP();
	_delay_ms(10);
	clear_leds();
}

void play_success()
{
	const uint8_t *ptr = ffx;
	while (*ptr != END_MARKER)
	{
		play_byte(*ptr++);
	}
}

uint8_t find_next_tune(uint8_t currentTune)
{
	uint8_t idx = 0;
	while(1)
	{
		idx = (++currentTune) % 10;
		if(get_tune_state(idx))
		{
			return idx;
		}
	}
}

uint8_t reset_game(void)
{
	for (uint8_t i = 0; i < 10; i++)
	{
		eeprom_update_byte(&stored_tunes.availableTunes[i], 0);
	}
	return STATE_INITIALISE;
}

uint8_t get_tune_state(uint8_t tuneNumber)
{
	return eeprom_read_byte(&stored_tunes.availableTunes[tuneNumber]);
}

void set_colour(volatile colors_codes_t *codes)
{
	red = codes->red;
	blue = codes->blue;
	green = codes->green;
}

uint8_t is_left_button_pressed(void)
{
	uint8_t leftButton = read_capacitive_pin(&BUTTON_LEFT_DDR, &BUTTON_LEFT_PORT, &BUTTON_LEFT_PIN, BUTTON_LEFT);
	return (leftButton >= 2) ? 1 : 0;
}

uint8_t is_right_button_pressed(void)
{
	uint8_t rightButton = read_capacitive_pin(&BUTTON_RIGHT_DDR, &BUTTON_RIGHT_PORT, &BUTTON_RIGHT_PIN, BUTTON_RIGHT);
	return (rightButton >= 2) ? 1 : 0;
}

void make_tune_available(uint8_t index)
{
	eeprom_update_byte((uint8_t*)&stored_tunes.availableTunes[index], 1);
}

void clear_leds(void)
{
	RED_L_OFF();
	RED_R_OFF();
	GREEN_L_OFF();
	GREEN_R_OFF();
	BLUE_L_OFF();
	BLUE_R_OFF();
}

uint8_t read_capacitive_pin(volatile uint8_t* ddr, volatile uint8_t* port, volatile uint8_t* pin, uint8_t pinNumber)
{
	uint8_t bitmask = 1 << pinNumber;
	*port &= ~(bitmask);
	*ddr |= bitmask;
	_delay_ms(1);
	*ddr &= ~(bitmask);
	*port |= bitmask;
	int cycles = 17;
	if (*pin & bitmask) { cycles =  0;}
	else if (*pin & bitmask) { cycles =  1;}
	else if (*pin & bitmask) { cycles =  2;}
	else if (*pin & bitmask) { cycles =  3;}
	else if (*pin & bitmask) { cycles =  4;}
	else if (*pin & bitmask) { cycles =  5;}
	else if (*pin & bitmask) { cycles =  6;}
	else if (*pin & bitmask) { cycles =  7;}
	else if (*pin & bitmask) { cycles =  8;}
	else if (*pin & bitmask) { cycles =  9;}
	else if (*pin & bitmask) { cycles = 10;}
	else if (*pin & bitmask) { cycles = 11;}
	else if (*pin & bitmask) { cycles = 12;}
	else if (*pin & bitmask) { cycles = 13;}
	else if (*pin & bitmask) { cycles = 14;}
	else if (*pin & bitmask) { cycles = 15;}
	else if (*pin & bitmask) { cycles = 16;}
	*port &= ~(bitmask);
	*ddr |= bitmask;
	
	return cycles;
}

void play_byte(uint8_t pByte)
{
	int tone = 0;
	int duration = 0;
	switch (GET_TONE(pByte)) {
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
		case T_REST:
		tone = 0;
		break;
	}
	switch (GET_DURATION(pByte)) {
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
	if (tone == 0)
	{
		delay_us(tvalue);
	}
	else
	{
		play_tone(tone, tvalue);
	}
}

void play_tone(int tone, long tempo_value)
{
	long tempo_position = 0;
	while (tempo_position < tempo_value && tempo_value < 640000) // enters an infinite loop when tempo_value is a big value
	{
		SPEAKER_ON();
		delay_us(tone / 2);
		
		SPEAKER_OFF();
		delay_us(tone / 2);
		
		tempo_position += tone;
	}
}

void save_buffer(volatile uint8_t *pByte)
{
	currentTune = *pByte;
	uint8_t *ptr = stored_tunes.tunes[currentTune];
	uint8_t ctr = 0;
	while (*pByte != END_MARKER)
	{
		eeprom_update_byte(ptr++, *pByte++);
		ctr++;
	}
	eeprom_update_byte(ptr++, END_MARKER);
    /*
		TODO: Last byte is crc
    */
    make_tune_available(currentTune);
}

void show_error(uint8_t code, uint8_t subCode)
{
	clear_leds();
	_delay_ms(500);
	while(code--)
	{
		BLUE_R_ON();
		_delay_ms(250);
		BLUE_R_OFF();
		_delay_ms(250);
	}
	_delay_ms(500);
	uint8_t bigger = subCode / 10;
	uint8_t lesser = subCode % 10;
	while(bigger--)
	{
		RED_R_ON();
		_delay_ms(250);
		RED_R_OFF();
		_delay_ms(250);
	}
	while (lesser--)
	{
		GREEN_R_ON();
		_delay_ms(250);
		GREEN_R_OFF();
		_delay_ms(250);
	}
	_delay_ms(1000);
}

void show_signal(uint16_t signal)
{
	clear_leds();
	_delay_ms(500);
	uint8_t h = signal / 100;
	uint8_t t = (signal % 100) / 10;
	uint8_t s = (signal % 100) % 10;
	while (h--)
	{
		RED_R_ON();
		_delay_ms(250);
		RED_R_OFF();
		_delay_ms(250);
	}
	_delay_ms(1000);
	while (t--)
	{
		GREEN_R_ON();
		_delay_ms(250);
		GREEN_R_OFF();
		_delay_ms(250);
	}
	_delay_ms(1000);
	while (s--)
	{
		BLUE_R_ON();
		_delay_ms(250);
		BLUE_R_OFF();
		_delay_ms(250);
	}
}

uint8_t validate_buffer(uint8_t currentPulse, uint8_t currentBit, uint8_t currentByte, uint8_t *buffer, uint8_t errorCode)
{
	if (currentPulse != 0)
	{
		if (currentBit != 0)
		{
			// bad data, escape!
			show_error(errorCode, currentBit);
		}
		else
		{
			buffer[currentByte] = END_MARKER;
			save_buffer(buffer);
			return STATE_PLAY;
		}
	} else {
		show_error(5, 0);
	}
	return STATE_MAIN;
}

void send_IR_byte(uint8_t val)
{
	uint8_t i, cycles;
	
	cli();
	for (i=0; i<8; i++) {
		cycles = (val & (1 << i)) ? IR_ONE : IR_ZERO;
		
		while (cycles--) {
			IR_TX_ON();
			_delay_us(10);
			IR_TX_OFF();
			_delay_us(10);
		};
		
		/* inter-bit time */
		_delay_us(IR_DELAY);
	}
	sei();
}

void delay_us(uint16_t count)
{
	while(count--)
	{
		_delay_us(1);
	}
}

ISR(TIMER2_COMP_vect)
{
	cnt++;
	
	if (cnt > red)
	{
		RED_L_ON();
		RED_R_ON();
	} else {
		RED_L_OFF();
		RED_R_OFF();
	}
	
	if (cnt > green)
	{
		GREEN_L_ON();
		GREEN_R_ON();
	} else {
		GREEN_L_OFF();
		GREEN_R_OFF();
	}
	
	if (cnt > blue)
	{
		BLUE_L_ON();
		BLUE_R_ON();
	} else {
		BLUE_L_OFF();
		BLUE_R_OFF();
	}
}