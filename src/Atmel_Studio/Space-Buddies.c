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
#define IR_RESOLUTION 16
// Audio
#define vel 10000l;//1.25;

volatile uint8_t anyButtonPressed = 0;
volatile uint8_t checkForButtonPress = 0;

typedef enum {
	STATE_MAIN,
	STATE_RECEIVE,
	STATE_SEND,
	STATE_PLAY
} state_t;

state_t state = STATE_MAIN;
uint8_t currentTune = 0; // 0-9

//TODO: Before each send, blink the current tune's LED colour

uint8_t find_next_tune(uint8_t currentTune)
{
	BLUE_L_ON();
	uint8_t idx = 0;
	while(1)
	{
		idx = (++currentTune) % 10;
		if(get_tune_state(idx))
		{
			return idx;
		}
	}
	BLUE_L_OFF();
}

void set_colour(volatile colors_codes_t *codes)
{
	red = codes->red;
	blue = codes->blue;
	green = codes->green;
}

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
	intialise_game();
	
    while(1)
    {
		switch(state)
		{
			case STATE_MAIN:
			{
				check_buttons();
				break;
			}
			case STATE_RECEIVE:
			{
				read_ir_data();
				break;
			}
			case STATE_SEND:
			{
				//TODO: Turn on some LEDs to signify data being sent (no PWM)
				send_data(currentTune);
				_delay_ms(1000);
				state = STATE_MAIN;
				break;
			}
			case STATE_PLAY:
			{
				play_tune(currentTune);
				state = STATE_MAIN;
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

uint8_t get_tune_state(uint8_t tuneNumber)
{
	return eeprom_read_byte(&stored_tunes.availableTunes[tuneNumber]);
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
		anyButtonPressed = 0;
		checkForButtonPress = 1;
		start_timer2();
		while(1)
		{
			i = i % 10;
			set_colour(&colours[i]);
			_delay_ms(200);
			if (anyButtonPressed)
			{
				break;
			}
			i++;
		}
		stop_timer2();
		make_tune_available(i);
	}
	_delay_ms(500);
	currentTune = find_next_tune(currentTune);
	//play_tune(currentTune);
	clear_leds();
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

void timer_init(void)
{
	// Timer 2
	TCCR2 = (1 << WGM21)|(1 << CS21);
	OCR2 = 65;
	// Turn on interrupts
	sei();
}

void start_timer2(void)
{
	TIMSK |= (1 << OCIE2);
}

void stop_timer2(void)
{
	TIMSK &= ~(1 << OCIE2);
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
	
	if (cnt == 0 && checkForButtonPress == 1 && anyButtonPressed == 0)
	{
		if (read_capacitive_pin(&BUTTON_LEFT_DDR, &BUTTON_LEFT_PORT, &BUTTON_LEFT_PIN, BUTTON_LEFT) >= 2)
		{
			anyButtonPressed = 1;
		}
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
    state = STATE_PLAY;
}

void check_buttons(void)
{
	if (!IR_RX_READ())
	{
		state = STATE_RECEIVE;
		return;
	}
	uint8_t leftButton = read_capacitive_pin(&BUTTON_LEFT_DDR, &BUTTON_LEFT_PORT, &BUTTON_LEFT_PIN, BUTTON_LEFT);
	if (leftButton >= 2){
		left_button_pressed();
		_delay_ms(200);
	}
	if (!IR_RX_READ())
	{
		state = STATE_RECEIVE;
		return;
	}
	uint8_t rightButton = read_capacitive_pin(&BUTTON_RIGHT_DDR, &BUTTON_RIGHT_PORT, &BUTTON_RIGHT_PIN, BUTTON_RIGHT);
	if (rightButton >= 2){
		right_button_pressed();
		_delay_ms(200);
	}
}

void right_button_pressed(void)
{
	// Select next tune and flash it's colours
	currentTune = find_next_tune(currentTune);
	set_colour(&colours[currentTune]);
	start_timer2();
	_delay_ms(500);
	stop_timer2();
	_delay_ms(100);
	clear_leds();
}

void left_button_pressed(void)
{
	// Send current tune
	state = STATE_SEND;
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
	for(int i = 0; i < 16; i++)
	{
		if (*pin & bitmask)
		{
			cycles = i;
			break;
		}
	}
	*port &= ~(bitmask);
	*ddr |= bitmask;
	
	return cycles;
}

void show_error(uint8_t code, uint8_t subCode)
{
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

void read_ir_data(void)
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
		
		// While pin is high
		while(IR_RX_READ())
		{
			highpulse++;
			_delay_us(IR_RESOLUTION);
			/*
			If the pulse is too long, we have timed out
			Either nothing was received or the code is finished
			Process what we've grabbed so far and then reset
			*/
			if (highpulse >= MAXPULSE)
			{
				validate_buffer(currentPulse, currentBit, currentByte, buffer, 1);
				return;
			}
		}
		
		// While pin is low
		while(!IR_RX_READ())
		{
			lowpulse++;
			_delay_us(IR_RESOLUTION);
			if (lowpulse >= MAXPULSE)
			{
				validate_buffer(currentPulse, currentBit, currentByte, buffer, 2);
				return;
			}
		}
		
		if (lowpulse >= 200 && lowpulse <= 320)
		{
			// this is a 1
			buffer[currentByte] |= (1 << currentBit);
		}
		else if (lowpulse >= 50 && lowpulse <= 150)
		{
			// this is a 0
			buffer[currentByte] &= ~(1 << currentBit);
		}
		else
		{
			show_signal(lowpulse);
			_delay_ms(2000);
			// bad data, escape!
			if (currentBit == 0)
			{
				show_error(4, 0);
				// TODO: Add a CRC validator here
			}
			else
			{
				state = STATE_MAIN;
				show_error(3, currentBit);
				return;
			}
		}
		
		if (++currentBit == 8)
		{
			currentBit = 0;
			currentByte++;
		}
		
		currentPulse++;
	}
}

void validate_buffer(uint8_t currentPulse, uint8_t currentBit, uint8_t currentByte, uint8_t *buffer, uint8_t errorCode)
{
	if (currentPulse != 0)
	{
		if (currentBit != 0)
		{
			// bad data, escape!
			show_error(errorCode, currentBit);
			state = STATE_MAIN; // check for buttons
		}
		else
		{
			buffer[currentByte] = END_MARKER;
			save_buffer(buffer);
			state = STATE_PLAY; // play current tune
		}
	} else {
		show_error(5, 0);
	}
}

void send_data(volatile uint8_t index)
{
	GREEN_R_ON();
	uint8_t currentBit = 0;
	while (currentBit < 8)
	{
		send_IR_bit(READ_BIT(index, currentBit));
		currentBit++;
	}
	
	const uint8_t *ptr = stored_tunes.tunes[currentTune];
	for(uint8_t data = eeprom_read_byte(ptr++); data != END_MARKER; data = eeprom_read_byte(ptr++))
	{
		currentBit = 0;
		while (currentBit < 8)
		{
			send_IR_bit(READ_BIT(data, currentBit));
			currentBit++;
		}
	}
	GREEN_R_OFF();
}

void send_IR_bit(uint8_t bit)
{
	if (bit == 1)
	{
		send_IR(220);
	}
	else
	{
		send_IR(100);
	}
	_delay_us(250);
}

void send_IR(int ir_cycles)
{
	// Turn off background interrupts
	cli();
	while(ir_cycles--)
	{
		//TODO: Extract these into macros
		IR_TX_ON();
		_delay_us(10);
		IR_TX_OFF();
		_delay_us(10);
	}
	// Turn background interrupts back on
	sei();
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

void play_tune(uint8_t currentTune)
{
	start_timer2();
	const uint8_t *ptr = stored_tunes.tunes[currentTune];
	for(uint8_t data = eeprom_read_byte(ptr++); data != END_MARKER; data = eeprom_read_byte(ptr++))
	{
		play_byte(data);
	}
	stop_timer2();
	_delay_ms(10);
	clear_leds();
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