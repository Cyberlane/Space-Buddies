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
volatile uint16_t currentPulse = 0;
volatile uint16_t highpulse = 0;
volatile uint16_t lowpulse = 0;
volatile uint8_t buffer[50];
volatile uint8_t currentBit = 0;
volatile uint8_t currentByte = 0;
uint8_t crc = 0;

// Audio
long vel = 10000;//1.25;

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

void set_next_tune(void)
{
	PORTD |= (1 << PD0);
	uint8_t idx = 0;
	while(1)
	{
		idx = (++currentTune) % 10;
		if(availableTunes[idx])
		{
			currentTune = idx;
			return;
		}
	}
	PORTB |= (1 << PB0);
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
	DDRD |= (1 << PD0); // blue
	DDRD |= (1 << PD1); // green
	DDRD |= (1 << PD2); // red
	DDRB |= (1 << PB0); // red
	DDRB |= (1 << PB1); // green
	DDRB |= (1 << PB2); // blue
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
	clear_buffer();
	timer_init();
	intialise_game();
	
    while(1)
    {
		switch(state)
		{
			case 0:
			{
				checkButtons();
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
				PORTB |= (1 << PB1);
				send_data(currentTune, buffer);
				PORTB &= ~(1 << PB1);
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
	set_next_tune();
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
	PORTD &= ~(1 << RED_L);
	PORTB &= ~(1 << RED_R);
	PORTD &= ~(1 << GREEN_L);
	PORTB &= ~(1 << GREEN_R);
	PORTD &= ~(1 << BLUE_L);
	PORTB &= ~(1 << BLUE_R);
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
		PORTD |= (1 << RED_L);
		PORTB |= (1 << RED_R);
	} else {
		PORTD &= ~(1 << RED_L);
		PORTB &= ~(1 << RED_R);
	}
	
	if (cnt > green)
	{
		PORTD |= (1 << GREEN_L);
		PORTB |= (1 << GREEN_R);
	} else {
		PORTD &= ~(1 << GREEN_L);
		PORTB &= ~(1 << GREEN_L);
	}
	
	if (cnt > blue)
	{
		PORTD |= (1 << BLUE_L);
		PORTB |= (1 << BLUE_R);
	} else {
		PORTD &= ~(1 << BLUE_L);
		PORTB &= ~(1 << BLUE_R);
	}
	
	if (cnt == 0 && checkForButtonPress == 1 && anyButtonPressed == 0)
	{
		if (readCapacitivePin(&DDRD, &PORTD, &PIND, PD3) >= 2)
		{
			anyButtonPressed = 1;
		}
	}
}

void move_selected_to_buffer(void)
{
	const uint8_t *ptr;
	switch (currentTune)
	{
		case 0:
		ptr = stored_tunes.tune1;
		break;
		case 1:
		ptr = stored_tunes.tune2;
		break;
		case 2:
		ptr = stored_tunes.tune3;
		break;
		case 3:
		ptr = stored_tunes.tune4;
		break;
		case 4:
		ptr = stored_tunes.tune5;
		break;
		case 5:
		ptr = stored_tunes.tune6;
		break;
		case 6:
		ptr = stored_tunes.tune7;
		break;
		case 7:
		ptr = stored_tunes.tune8;
		break;
		case 8:
		ptr = stored_tunes.tune9;
		break;
		case 9:
		ptr = stored_tunes.tune10;
		break;
	}
	uint8_t i = 0;
	for(uint8_t data = eeprom_read_byte(ptr++); data != END_MARKER; data = eeprom_read_byte(ptr++))
	{
		buffer[i++] = data;
	}
	buffer[i] = END_MARKER;
}

void save_buffer(void)
{
	const uint8_t *ptr;
	switch (buffer[0])
	{
		case 0:
		ptr = stored_tunes.tune1;
		break;
		case 1:
		ptr = stored_tunes.tune2;
		break;
		case 2:
		ptr = stored_tunes.tune3;
		break;
		case 3:
		ptr = stored_tunes.tune4;
		break;
		case 4:
		ptr = stored_tunes.tune5;
		break;
		case 5:
		ptr = stored_tunes.tune6;
		break;
		case 6:
		ptr = stored_tunes.tune7;
		break;
		case 7:
		ptr = stored_tunes.tune8;
		break;
		case 8:
		ptr = stored_tunes.tune9;
		break;
		case 9:
		ptr = stored_tunes.tune10;
		break;
	}
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

void checkButtons(void)
{
	if (!(PINC & _BV(PC5)))
	{
		state = 1;
		return;
	}
	uint8_t leftButton = readCapacitivePin(&DDRD, &PORTD, &PIND, PD3);
	if (leftButton >= 2){
		//TODO: Add some type of debounce
		leftButtonPressed();
		_delay_ms(200);
	}
	if (!(PINC & _BV(PC5)))
	{
		state = 1;
		return;
	}
	uint8_t rightButton = readCapacitivePin(&DDRD, &PORTD, &PIND, PD7);
	if (rightButton >= 2){
		//TODO: Add some type of debounce
		rightButtonPressed();
		_delay_ms(200);
	}
}

void rightButtonPressed(void)
{
	// Select next tune and flash it's colours
	set_next_tune();
	set_colour(&colours[currentTune]);
	start_timer2();
	_delay_ms(500);
	stop_timer2();
	_delay_ms(100);
	clear_leds();
}

void leftButtonPressed(void)
{
	// Send current tune
	state = 2;
}

uint8_t readCapacitivePin(volatile uint8_t* ddr, volatile uint8_t* port, volatile uint8_t* pin, uint8_t pinNumber)
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
		PORTD |= (1 << PD2);
		_delay_ms(250);
		PORTD &= ~(1 << PD2);
		_delay_ms(250);
	}
	_delay_ms(500);
	uint8_t bigger = subCode / 10;
	uint8_t lesser = subCode % 10;
	while(bigger--)
	{
		PORTB |= (1 << PB0);
		_delay_ms(250);
		PORTB &= ~(1 << PB0);
		_delay_ms(250);
	}
	while (lesser--)
	{
		PORTB |= (1 << PB1);
		_delay_ms(250);
		PORTB &= ~(1 << PB1);
		_delay_ms(250);
	}
	_delay_ms(1000);
}

void read_ir_data(void)
{
	currentBit = currentByte = 0;
	
	while(1)
	{
		// Start out with no pulse length
		highpulse = lowpulse = 0;
		
		// While pin is high
		while(PINC & (1 << PC5))
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
		while(!(PINC & _BV(PC5)))
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
	currentBit = 0;
	while (currentBit < 8)
	{
		if (READ_BIT(index, currentBit) == 1)
		{
			sendIR(220);
		}
		else
		{
			sendIR(100);
		}
		_delay_us(250);
		currentBit++;
	}
	while(*pByte != END_MARKER)
	{
		currentBit = 0;
		while (currentBit < 8)
		{
			if (READ_BIT(*pByte, currentBit) == 1)
			{
				sendIR(220);
			}
			else
			{
				sendIR(100);
			}
			_delay_us(250);
			currentBit++;
		}
		++pByte;
	}
}

void sendIR(int ir_cycles)
{
	// Turn off background interrupts
	cli();
	while(ir_cycles--)
	{
		//TODO: Extract these into macros
		PORTD |= (1 << PD6);
		_delay_us(10);
		PORTD &= ~(1 << PD6);
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
		PORTD |= (1 << PD5);
		delay_us(tone / 2);
		
		PORTD &= ~(1 << PD5);
		delay_us(tone / 2);
		
		tempo_position += tone;
	}
}