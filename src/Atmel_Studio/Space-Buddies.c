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
#include "SoftPWM.h"

// Generic
#define SET(x) |= (1 << x)
#define CLR(x) &= ~(1 << x)
#define FLIP(x) ^= (1 << x)
#define PINMODE_OUTPUT(x) |= (1 << x)
#define PINMODE_INPUT(x) &= ~(1 << x)

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

// Software PWM
volatile uint8_t red = 180;
volatile uint8_t green = 255;
volatile uint8_t blue = 255;
volatile uint8_t cnt = 0;

typedef struct {
	uint8_t tune1[50];
	uint8_t tune2[50];
	uint8_t tune3[50];
	uint8_t tune4[50];
	uint8_t tune5[50];
	uint8_t tune6[50];
	uint8_t tune7[50];
	uint8_t tune8[50];
	uint8_t tune9[50];
	uint8_t tune10[50];
	uint8_t isInitialised;
	uint8_t availableTunes[10];
} tunes_t;

tunes_t stored_tunes EEMEM = {
	.tune1 = { //mario
		NOTE(T_EX, 2),  NOTE(T_EX, 4),  NOTE(T_EX, 4),  NOTE(T_CX, 2),  NOTE(T_EX, 4),  NOTE(T_GX, 7),
		NOTE(T_G, 7),   NOTE(T_CX, 6),  NOTE(T_G, 6),   NOTE(T_E, 6),   NOTE(T_A, 4),   NOTE(T_B, 4),
		NOTE(T_BB, 2),  NOTE(T_A, 4),   NOTE(T_G, 3),   NOTE(T_EX, 3),  NOTE(T_GX, 3),  NOTE(T_AX, 4),
		NOTE(T_FX, 2),  NOTE(T_GX, 4),  NOTE(T_EX, 4),  NOTE(T_CX, 2),  NOTE(T_DX, 2),  NOTE(T_B, 2),
		NOTE(T_CX, 4),  END_MARKER
	},
	.tune2 = { //felix
		NOTE(T_EBX, 3), NOTE(T_REST, 2), NOTE(T_AB, 3), NOTE(T_FX, 2), NOTE(T_EBX, 3),
		NOTE(T_REST, 7), NOTE(T_EBX, 2), NOTE(T_CSX, 2), NOTE(T_EBX, 2), NOTE(T_CSX, 2),
		NOTE(T_BB, 2), NOTE(T_G, 2), NOTE(T_EB, 2), NOTE(T_CX, 3), NOTE(T_REST, 7),
		NOTE(T_CX, 2), NOTE(T_BB, 3), NOTE(T_EB, 8), NOTE(T_EB, 8), NOTE(T_EB, 8),
		NOTE(T_CX, 3), NOTE(T_AB, 2), NOTE(T_AB, 2), NOTE(T_AB, 2), NOTE(T_AB, 2),
		NOTE(T_BB, 3), NOTE(T_BB, 2), NOTE(T_BB, 2), NOTE(T_CX, 2), NOTE(T_DX, 2),
		NOTE(T_FX, 3), NOTE(T_EBX, 2), NOTE(T_EBX, 3), END_MARKER
	},
	.tune3 = { //heman
		NOTE(T_AB, 3), NOTE(T_AB, 3), NOTE(T_AB, 3), NOTE(T_AB, 5), NOTE(T_F, 5),
		NOTE(T_AB, 3), NOTE(T_BB, 3), NOTE(T_AB, 3), NOTE(T_FS, 3), NOTE(T_AB, 5),
		NOTE(T_CS, 5), NOTE(T_AB, 3), NOTE(T_BB, 3), NOTE(T_AB, 3), NOTE(T_FS, 3),
		NOTE(T_AB, 5), NOTE(T_F, 5), NOTE(T_CS, 3), NOTE(T_EB, 7), END_MARKER
	},
	.tune4 = { //kirby
		NOTE(T_AB, 2), NOTE(T_BB, 2), NOTE(T_CX, 2), NOTE(T_DX, 2), NOTE(T_CX, 2),
		NOTE(T_DX, 2), NOTE(T_EBX, 4), NOTE(T_BB, 2), NOTE(T_G, 4), NOTE(T_BB, 2),
		NOTE(T_AB, 4), NOTE(T_G, 2), NOTE(T_F, 4), NOTE(T_G, 2), NOTE(T_EB, 4),
		NOTE(T_F, 0), NOTE(T_G, 0), NOTE(T_AB, 0), NOTE(T_BB, 0), NOTE(T_CX, 0),
		NOTE(T_DX, 0), NOTE(T_EBX, 4), END_MARKER
	},
	.tune5 = { //morning
		NOTE(T_G, 3), NOTE(T_E, 3), NOTE(T_D, 3), NOTE(T_C, 3), NOTE(T_D, 3),
		NOTE(T_E, 3), NOTE(T_G, 3), NOTE(T_E, 3), NOTE(T_D, 3), NOTE(T_C, 3),
		NOTE(T_D, 1), NOTE(T_E, 1), NOTE(T_D, 1), NOTE(T_E, 1), NOTE(T_G, 3),
		NOTE(T_E, 3), NOTE(T_G, 3), NOTE(T_A, 3), NOTE(T_E, 3), NOTE(T_A, 3),
		NOTE(T_G, 3), NOTE(T_E, 3), NOTE(T_D, 3), NOTE(T_C, 5), END_MARKER
	},
	.tune6 = { // pony
		NOTE(T_FSX, 3), NOTE(T_AX, 3), NOTE(T_FSX, 3), NOTE(T_EX, 5), NOTE(T_AX, 5),
		NOTE(T_DX, 3), NOTE(T_EX, 3), NOTE(T_EX, 3), NOTE(T_CSX, 5), NOTE(T_A, 5),
		NOTE(T_B, 5), NOTE(T_DX, 3), NOTE(T_DX, 3), NOTE(T_GX, 5), NOTE(T_BX, 5),
		NOTE(T_AX, 3), NOTE(T_BX, 5), NOTE(T_AX, 7), END_MARKER
	},
	.tune7 = { // simpsons
		NOTE(T_CX, 7), NOTE(T_EX, 5), NOTE(T_FSX, 5), NOTE(T_AX, 3), NOTE(T_GX, 7),
		NOTE(T_EX, 5), NOTE(T_CX, 5), NOTE(T_A, 3), NOTE(T_FS, 3), NOTE(T_FS, 3),
		NOTE(T_FS, 3), NOTE(T_G, 7), NOTE(T_G, 5), NOTE(T_REST, 3), NOTE(T_FS, 3),
		NOTE(T_FS, 3), NOTE(T_FS, 3), NOTE(T_G, 3), NOTE(T_BB, 7), NOTE(T_REST, 0),
		NOTE(T_B, 3), END_MARKER
	},
	.tune8 = { // smurfs
		NOTE(T_CX, 7), NOTE(T_FX, 5), NOTE(T_CX, 3), NOTE(T_DX, 5), NOTE(T_BB, 5),
		NOTE(T_G, 7), NOTE(T_CX, 7), NOTE(T_A, 3), NOTE(T_F, 5), NOTE(T_A, 5),
		NOTE(T_G, 7), NOTE(T_REST, 5), NOTE(T_G, 1), NOTE(T_A, 1), NOTE(T_BB, 1),
		NOTE(T_B, 1), NOTE(T_CX, 7), NOTE(T_FX, 5), NOTE(T_CX, 3), NOTE(T_DX, 5),
		NOTE(T_BB, 5), NOTE(T_G, 7), NOTE(T_C, 7), NOTE(T_A, 3), NOTE(T_BB, 5),
		NOTE(T_E, 5), NOTE(T_F, 7), END_MARKER
	},
	.tune9 = { // sonic
		NOTE(T_CSX, 3), NOTE(T_REST, 4), NOTE(T_CSX, 3), NOTE(T_DX, 4), NOTE(T_B, 6),
		NOTE(T_CSX, 3), NOTE(T_CSX, 3), NOTE(T_CSX, 3), NOTE(T_A, 3), NOTE(T_G, 4),
		NOTE(T_B, 6), NOTE(T_REST, 3), NOTE(T_CSX, 3), NOTE(T_REST, 4), NOTE(T_CSX, 3),
		NOTE(T_DX, 4), NOTE(T_B, 6), NOTE(T_CSX, 3), NOTE(T_CSX, 3), NOTE(T_CSX, 3),
		NOTE(T_A, 3), NOTE(T_G, 4), NOTE(T_B, 5), NOTE(T_REST, 6), NOTE(T_A, 0),
		NOTE(T_B, 0), NOTE(T_CSX, 0), NOTE(T_DX, 0), NOTE(T_EX, 0), NOTE(T_FSX, 0),
		NOTE(T_B, 0), NOTE(T_CSX, 0), NOTE(T_EBX, 0), NOTE(T_EX, 0), NOTE(T_FSX, 0),
		NOTE(T_ABX, 0), NOTE(T_EB, 0), NOTE(T_E, 0), NOTE(T_FS, 0), NOTE(T_AB, 0),
		NOTE(T_A, 0), NOTE(T_B, 0), NOTE(T_F, 0), NOTE(T_FS, 0), NOTE(T_G, 0),
		NOTE(T_BB, 0), NOTE(T_CX, 0), NOTE(T_CSX, 0), END_MARKER
	},
	.tune10 = { // ducktales
		NOTE(T_FS, 3), NOTE(T_BB, 3), NOTE(T_CSX, 3), NOTE(T_EBX, 3), NOTE(T_EX, 3),
		NOTE(T_EX, 1), NOTE(T_EBX, 1), NOTE(T_REST, 1), NOTE(T_CSX, 1), NOTE(T_B, 3),
		NOTE(T_B, 5), NOTE(T_BB, 4), NOTE(T_REST, 5), NOTE(T_B, 5), NOTE(T_BB, 4),
		NOTE(T_REST, 5), NOTE(T_FS, 3), NOTE(T_BB, 3), NOTE(T_CSX, 3), NOTE(T_EBX, 3),
		NOTE(T_EX, 3), NOTE(T_EX, 1), NOTE(T_EBX, 1), NOTE(T_REST, 1), NOTE(T_CSX, 1),
		NOTE(T_B, 3), NOTE(T_EX, 5), NOTE(T_EBX, 4), NOTE(T_REST, 5), NOTE(T_EX, 5),
		NOTE(T_EBX, 4), NOTE(T_REST, 5), END_MARKER
	},
	.isInitialised = 0,
	.availableTunes = {
		0,0,0,0,0,
		0,0,0,0,0
	}
};

/*
	State Summary:
	0: check buttons
	1: read IR data
	2: send IR data
	3: save buffer to EEPROM
	4: copy selected tune from EEPROM to buffer
	5: play buffer
*/
uint8_t state = 0;
uint8_t currentTune = 0; // 0-9
uint8_t availableTunes[] = {1,0,0,0,0,0,0,0,0,0};

//TODO: Need logic to set "current tune", toggled by button press
//TODO: Before each send, blink the current tune's LED colour

void set_next_tune(void)
{
	//TODO: Copy from EEPROM to variable
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
}

int main(void)
{
	// Set RGB LEDs as output
	DDRD PINMODE_OUTPUT(PD0); // blue
	DDRD PINMODE_OUTPUT(PD1); // green
	DDRD PINMODE_OUTPUT(PD2); // red
	DDRB PINMODE_OUTPUT(PB0); // red
	DDRB PINMODE_OUTPUT(PB1); // green
	DDRB PINMODE_OUTPUT(PB2); // blue
	// Set buttons as input
	DDRD PINMODE_INPUT(PD3);
	DDRD PINMODE_INPUT(PD7);
	// Set speaker as output
	DDRD PINMODE_OUTPUT(PD5);
	// Set IR Transmitter as output
	DDRD PINMODE_OUTPUT(PD6);
	// Set IR Receiver as input
	DDRC PINMODE_INPUT(PC5);
	// Set internal pull-up for IR Receiver
	PORTC SET(PC5);
	
	_delay_ms(100);
	clear_buffer();
	timer_init();
	
    while(1)
    {
		if (state != 0)
		{
			state = 0;
		}
		
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
				PORTB SET(PB1);
				//send_data(mario);
				PORTB CLR(PB1);
				break;
			}
			case 3:
			{
				save_buffer();
				break;
			}
			case 4:
			{
				move_selected_to_buffer();
				// play(buffer);
				send_data(buffer);
				break;
			}
			case 5:
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

void timer_init(void)
{
	// Timer 2
	TCCR2 = (1 << WGM21)|(1 << CS20);
	OCR2 = 65;
	// Turn on interrupts
	sei();
}

void start_timer2(void)
{
	TIMSK SET(OCIE2);
}

void stop_timer2(void)
{
	TIMSK CLR(OCIE2);
}

ISR(TIMER2_COMP_vect)
{
	cnt++;
	
	if (cnt > red)
	{
		PORTD SET(RED_L);
		PORTB SET(RED_R);
	} else {
		PORTD CLR(RED_L);
		PORTB CLR(RED_R);
	}
	
	if (cnt > green)
	{
		PORTD SET(GREEN_L);
		PORTB SET(GREEN_R);
	} else {
		PORTD CLR(GREEN_L);
		PORTB CLR(GREEN_L);
	}
	
	if (cnt > blue)
	{
		PORTD SET(BLUE_L);
		PORTB SET(BLUE_R);
	} else {
		PORTD CLR(BLUE_L);
		PORTB CLR(BLUE_R);
	}
}

void move_selected_to_buffer(void)
{
	const uint8_t *ptr = stored_tunes.tune5;
	uint8_t i = 0;
	for(uint8_t data = eeprom_read_byte(ptr++); data != END_MARKER; data = eeprom_read_byte(ptr++))
	{
		buffer[i++] = data;
	}
	buffer[i] = END_MARKER;
	//TODO: Copy selected tune from EEPROM into buffer
}

void save_buffer(void)
{
	const uint8_t *ptr = stored_tunes.tune5;
	for (uint8_t i = 0; i < 50; i++)
	{
		eeprom_write_byte(ptr++, buffer[i]);
	}
    /*
		TODO:
        First byte is index position (1-10)
        Last byte is crc
        Everything else is the tune!
    */
    
    //TODO: Set current Tune to newly saved index
    state = 5;
}

void clear_buffer(void)
{
	for (uint8_t i = 0; i < 50; i++)
	{
		buffer[i] = 0;
	}
}

void checkButtons(void)
{
	uint8_t leftButton = readCapacitivePin(&DDRD, &PORTD, &PIND, PD3);
	uint8_t rightButton = readCapacitivePin(&DDRD, &PORTD, &PIND, PD7);
	if (leftButton >= 2){
		//TODO: Add some type of debounce
		leftButtonPressed();
		_delay_ms(200);
	}
	if (rightButton >= 2){
		//TODO: Add some type of debounce
		rightButtonPressed();
		_delay_ms(200);
	}
	//if (!(PINC & _BV(PC5)))
	//{
		//// If we see any incoming IR, escape button check mode
		////   Perhaps impl a counter to prevent stray data, however if adding that, auto-increase that number to first pulse count to prevent bad data escape!
		//state = 1;
	//}
}

void rightButtonPressed(void)
{
	PORTB FLIP(PB0);
}

void leftButtonPressed(void)
{
	PORTD FLIP(PD0);
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
		PORTD SET(PD2);
		_delay_ms(250);
		PORTD CLR(PD2);
		_delay_ms(250);
	}
	_delay_ms(500);
	uint8_t bigger = subCode / 10;
	uint8_t lesser = subCode % 10;
	while(bigger--)
	{
		PORTB SET(PB0);
		_delay_ms(250);
		PORTB CLR(PB0);
		_delay_ms(250);
	}
	while (lesser--)
	{
		PORTB SET(PB1);
		_delay_ms(250);
		PORTB CLR(PB1);
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

void send_data(volatile uint8_t *pByte)
{
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
			_delay_us(500);
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
		PORTD SET(PD6);
		_delay_us(10);
		PORTD CLR(PD6);
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
}

void play_tone(int tone, long tempo_value)
{
	long tempo_position = 0;
	while (tempo_position < tempo_value && tempo_value < 640000) // enters an infinite loop when tempo_value is a big value
	{
		PORTD SET(PD5);
		delay_us(tone / 2);
		
		PORTD CLR(PD5);
		delay_us(tone / 2);
		
		tempo_position += tone;
	}
}