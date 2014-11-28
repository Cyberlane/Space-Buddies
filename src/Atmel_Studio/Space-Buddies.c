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
#include "SoftPWM.h"

// Infrared
#define MAXPULSE 65000
#define IR_RESOLUTION 16

volatile uint8_t buffer[50];
uint8_t crc = 0;

// Audio
#define vel 10000l;//1.25;

volatile uint8_t anyButtonPressed = 0;
volatile uint8_t checkForButtonPress = 0;

/*
	State Summary:
	0: check buttons
	1: read IR data
	2: send IR data
	3: save buffer to EEPROM
	4: play buffer
*/
uint8_t state = 0;
uint8_t currentTune = 0; // 0-9
uint8_t availableTunes[] = {0,0,0,0,0,0,0,0,0,0};

//TODO: Before each send, blink the current tune's LED colour

uint8_t find_next_tune(uint8_t currentTune)
{
	BLUE_L_ON();
	uint8_t idx = 0;
	while(1)
	{
		idx = (++currentTune) % 10;
		if(availableTunes[idx])
		{
			return idx;
		}
	}
	BLUE_L_ON();
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
	clear_buffer();
	timer_init();
	intialise_game();
	
    while(1)
    {
		switch(state)
		{
			case 0:
			{
				check_buttons();
				break;
			}
			case 1:
			{
				read_ir_data();
				break;
			}
			case 2:
			{
				//TODO: Turn on some LEDs to signify data being sent (no PWM)
				clear_buffer();
				move_selected_to_buffer();
				send_data(currentTune, buffer);
				state = 0;
				_delay_ms(1000);
				break;
			}
			case 3:
			{
				save_buffer();
				break;
			}
			case 4:
			{
				play(buffer);
				state = 0;
				break;
			}
			default:
			{
				state = 0;
				break;
			}
		}
    }
}

void load_available_tunes(void)
{
	const uint8_t *ptr = stored_tunes.availableTunes;
	for (uint8_t i = 0; i < 10; i++)
	{
		availableTunes[i] = eeprom_read_byte(ptr++);
	}
}

void intialise_game(void)
{
	load_available_tunes();
	uint8_t selected = 99;
	uint8_t i = 0;
	
	while(i < 10)
	{
		if (availableTunes[i])
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
		availableTunes[i] = 1;
		save_available_tunes();
	}
	_delay_ms(500);
	currentTune = find_next_tune(currentTune);
	move_selected_to_buffer();
	//play(buffer);
	clear_leds();
	clear_buffer();
}

void save_available_tunes(void)
{
	for (uint8_t i = 0; i < 10; i++)
	{
		eeprom_update_byte((uint8_t*)&stored_tunes.availableTunes[i], availableTunes[i]);
	}
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

void move_selected_to_buffer(void)
{
	const uint8_t *ptr = stored_tunes.tunes[currentTune];
	uint8_t i = 0;
	for(uint8_t data = eeprom_read_byte(ptr++); data != END_MARKER; data = eeprom_read_byte(ptr++))
	{
		buffer[i++] = data;
	}
	buffer[i] = END_MARKER;
}

void save_buffer(void)
{
	const uint8_t *ptr = stored_tunes.tunes[buffer[0]];
	for (uint8_t i = 1; i < 50; i++)
	{
		eeprom_update_byte(ptr++, buffer[i]);
	}
	availableTunes[buffer[0]] = 1;
	save_available_tunes();
	for (uint8_t i = 0; i < 49; i++)
	{
		buffer[i] = buffer[i+1];
	}
	buffer[49] = 0;
    /*
		TODO: Last byte is crc
    */
    
    //TODO: Set current Tune to newly saved index
    state = 4;
}

void clear_buffer(void)
{
	for (uint8_t i = 0; i < 50; i++)
	{
		buffer[i] = 0;
	}
}

uint8_t pre_infrared_counter = 0;

void check_buttons(void)
{
	if (!IR_RX_READ())
	{
		state = 1;
		return;
	}
	uint8_t leftButton = read_capacitive_pin(&BUTTON_LEFT_DDR, &BUTTON_LEFT_PORT, &BUTTON_LEFT_PIN, BUTTON_LEFT);
	if (leftButton >= 2){
		//TODO: Add some type of debounce
		left_button_pressed();
		_delay_ms(200);
	}
	if (!IR_RX_READ())
	{
		state = 1;
		return;
	}
	uint8_t rightButton = read_capacitive_pin(&BUTTON_RIGHT_DDR, &BUTTON_RIGHT_PORT, &BUTTON_RIGHT_PIN, BUTTON_RIGHT);
	if (rightButton >= 2){
		//TODO: Add some type of debounce
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
	state = 2;
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
		RED_L_ON();
		_delay_ms(250);
		RED_L_OFF();
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

void read_ir_data(void)
{
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
				if (currentPulse != 0)
				{
					if (currentBit != 0)
					{
						// bad data, escape!
						show_error(1, currentByte);
						state = 0; // check for buttons
					}
					else
					{
						buffer[currentByte] = END_MARKER;
						state = 3; // save buffer to eeprom
					}
					currentPulse = 0;
				}
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
				if (currentPulse != 0)
				{
					if (currentBit != 0)
					{
						// bad data, escape!
						show_error(2, currentBit);
						state = 0; // check for buttons
					}
					else
					{
						buffer[currentByte] = END_MARKER;
						state = 3; // save buffer to eeprom
					}
					currentPulse = 0;
				}
				return;
			}
		}
		
		if (lowpulse >= 200 && lowpulse <= 300)
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
			// bad data, escape!
			if (currentBit == 0)
			{
				// TODO: Add a CRC validator here
			}
			else
			{
				state = 0;
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

void send_data(volatile uint8_t index, volatile uint8_t *pByte)
{
	GREEN_R_ON();
	uint8_t currentBit = 0;
	while (currentBit < 8)
	{
		send_bit(READ_BIT(index, currentBit));
		currentBit++;
	}
	while(*pByte != END_MARKER)
	{
		currentBit = 0;
		while (currentBit < 8)
		{
			send_bit(READ_BIT(*pByte, currentBit));
			currentBit++;
		}
		++pByte;
	}
	GREEN_R_OFF();
}

void send_bit(uint8_t bit)
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

void play(volatile uint8_t *pByte)
{
	start_timer2();
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
			case T_REST:
			tone = 0;
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
		if (tone == 0)
		{
			delay_us(tvalue);
		}
		else
		{
			play_tone(tone, tvalue);	
		}
		++pByte;
	}
	stop_timer2();
	_delay_ms(10);
	clear_leds();
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