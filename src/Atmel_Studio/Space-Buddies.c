/*
 * Space_Buddies
 *
 *  Created: 06/06/2014 22:56:55
 *  Author: Justin Nel
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>
#include "Space-Buddies.h"
#include "Space-Tunes.h"
#include "SoftPWM.h"

// Generic
#define SET(x) |= (1 << x)
#define CLR(x) &= ~(1 << x)
#define FLIP(x) ^= (1 << x)
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)
volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

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
long vel = 10000/1.25;

// Button Config only
#define BUTTON_NUM_READINGS 5
#define BUTTON_NUM_AVGS (BUTTON_NUM_READINGS + 1)
unsigned long rightButtonDebounce = 0;
unsigned long leftButtonDebounce = 0;
// Right button - variables
unsigned long rightButtonTime = 0;
float rightButtonAvgTouchVal = 0;
float rightButtonAvgTouchValOld = 0;
bool rightButtonPinTouched = false;
bool rightButtonPinTouchedOld = false;
uint8_t rightButtonReadingIndex = 0;
uint8_t rightButtonReadings[BUTTON_NUM_READINGS];
uint8_t rightButtonReadingAvgIndex = 0;
float rightButtonReadingAvg[BUTTON_NUM_AVGS];
uint8_t rightButtonTotal = 0;
// Left button - variables
unsigned long leftButtonTime = 0;
float leftButtonAvgTouchVal = 0;
float leftButtonAvgTouchValOld = 0;
bool leftButtonPinTouched = false;
bool leftButtonPinTouchedOld = false;
uint8_t leftButtonReadingIndex = 0;
uint8_t leftButtonReadings[BUTTON_NUM_READINGS];
uint8_t leftButtonReadingAvgIndex = 0;
float leftButtonReadingAvg[BUTTON_NUM_AVGS];
uint8_t leftButtonTotal = 0;

// Software PWM
volatile uint8_t red = 180;
volatile uint8_t green = 255;
volatile uint8_t blue = 255;
volatile uint8_t cnt = 0;

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

//Untested: Migrate capactive touch logic from Arduino to AVR C
//TODO: Need logic to set "current tune", toggled by button press
//TODO: Move buffer into EEPROM when data is successful
//TODO: Before each send, blink the current tune's LED colour

int main(void)
{
	// Set RGB LEDs as output
	DDRD SET(PD0); // blue
	DDRD SET(PD1); // green
	DDRD SET(PD2); // red
	DDRB SET(PB0); // red
	DDRB SET(PB1); // green
	DDRB SET(PB2); // blue
	// Set buttons as input
	DDRD CLR(PD3);
	DDRD CLR(PD7);
	// Set speaker as output
	DDRD SET(PD5);
	// Set IR Transmitter as output
	DDRD SET(PD6);
	// Set IR Receiver as input
	DDRC CLR(PC5);
	// Set internal pull-up for IR Receiver
	PORTC SET(PC5);
	
	_delay_ms(100);
	
	timer_init();
	
	for (int i = 0; i < BUTTON_NUM_READINGS; i++) {
		leftButtonReadings[i] = 0;
		rightButtonReadings[i] = 0;
	}
	for (int i = 0; i < BUTTON_NUM_AVGS; i++) {
		leftButtonReadingAvg[i] = 0;
		rightButtonReadingAvg[i] = 0;
	}
	for (int i = 0; i < 50; i++) {
		buffer[i] = 0;
	}
	
	//play(felix);
	
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
				send_data(mario);
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
	// Timer 0
	TCCR0 |= (1 << CS01)|(1 << CS00);
	TIMSK |= (1 << TOIE0);
	
	// Timer 1
	//TCCR1A = 0x00;
	//TCCR1B |= (1 << WGM12)|(1 << CS11);
	//TCNT1 = 0;
	//OCR1A = 26;
	//TIMSK |= (1 << OCIE1A);
	
	// Timer 2
	TCCR2 = (1 << WGM21)|(1 << CS20);
	TIMSK |= (1 << OCIE2);
	//OCR2 = 65;
	
	// Turn on interrupts
	sei();
}

void start_timer2(void)
{
	OCR2 = 65;
}

void stop_timer2(void)
{
	OCR2 = 0;
}

ISR(TIMER0_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;
	
	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX)
	{
		f -= FRACT_MAX;
		m += 1;
	}
	
	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;
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

unsigned long millis(void)
{
	unsigned long m;
	uint8_t oldSREG = SREG;
	
	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	m = timer0_millis;
	SREG = oldSREG;
	
	return m;
}

void move_selected_to_buffer(void)
{
	//TODO: Copy selected tune from EEPROM into buffer
}

void save_buffer(void)
{
    //TODO: Save buffer to eeprom
    /*
        First byte is index position (1-10)
        Last byte is crc
        Everything else is the tune!
    */
    
    //TODO: Set current Tune to newly saved index
    state = 5;
}

void checkButtons(void)
{
	checkLeftButton();
	checkRightButton();
	//if (!(PINC & _BV(PC5)))
	//{
		//// If we see any incoming IR, escape button check mode
		////   Perhaps impl a counter to prevent stray data, however if adding that, auto-increase that number to first pulse count to prevent bad data escape!
		//state = 1;
	//}
}

void rightButtonPressed(void)
{
	PORTD FLIP(PD0);
}

void leftButtonPressed(void)
{
	//PORTB FLIP(PB0);
}

void checkLeftButton(void)
{
	if (millis() - leftButtonTime > leftButtonDebounce)
	{
		getLeftButtonReading();
		if (leftButtonAvgTouchVal - leftButtonAvgTouchValOld > 1.0)
		{
			leftButtonPinTouched = true;
		}
		else
		{
			leftButtonPinTouched = false;
		}
		if (leftButtonPinTouched == true && leftButtonPinTouchedOld == false)
		{
			leftButtonTime = millis();
			leftButtonPressed();
		}
		else if (leftButtonPinTouched == false && leftButtonPinTouchedOld == true)
		{
			leftButtonTime = millis();
		}
		leftButtonPinTouchedOld = leftButtonPinTouched;
	}
}

void getLeftButtonReading(void)
{
	leftButtonTotal -= leftButtonReadings[leftButtonReadingIndex];
	leftButtonReadings[leftButtonReadingIndex] = readCapacitivePin(&DDRD, &PORTD, &PIND, PD7);
	leftButtonTotal += leftButtonReadings[leftButtonReadingIndex];
	leftButtonReadingIndex++;
	if (leftButtonReadingIndex >= BUTTON_NUM_READINGS)
	{
		leftButtonReadingIndex = 0;
	}
	leftButtonAvgTouchVal = (float)leftButtonTotal/BUTTON_NUM_READINGS;
	leftButtonReadingAvg[leftButtonReadingAvgIndex] = leftButtonAvgTouchVal;
	leftButtonReadingAvgIndex++;
	if (leftButtonReadingAvgIndex >= BUTTON_NUM_AVGS)
	{
		leftButtonReadingAvgIndex++;
	}
	leftButtonAvgTouchValOld = leftButtonReadingAvg[leftButtonReadingAvgIndex];
}

void checkRightButton(void)
{
	if (millis() - rightButtonTime > rightButtonDebounce)
	{
		getRightButtonReading();
		if (rightButtonAvgTouchVal - rightButtonAvgTouchValOld > 1.0)
		{
			rightButtonPinTouched = true;
		}
		else
		{
			rightButtonPinTouched = false;
		}
		if (rightButtonPinTouched == true && rightButtonPinTouchedOld == false)
		{
			rightButtonTime = millis();
			rightButtonPressed();
		}
		else if (rightButtonPinTouched == false && rightButtonPinTouchedOld == true)
		{
			rightButtonTime = millis();
		}
		rightButtonPinTouchedOld = rightButtonPinTouched;
	}
}

void getRightButtonReading(void)
{
	rightButtonTotal -= rightButtonReadings[rightButtonReadingIndex];
	rightButtonReadings[rightButtonReadingIndex] = readCapacitivePin(&DDRD, &PORTD, &PIND, PD3);
	rightButtonTotal += rightButtonReadings[rightButtonReadingIndex];
	rightButtonReadingIndex++;
	if (rightButtonReadingIndex >= BUTTON_NUM_READINGS)
	{
		rightButtonReadingIndex = 0;
	}
	rightButtonAvgTouchVal = (float)rightButtonTotal/BUTTON_NUM_READINGS;
	rightButtonReadingAvg[rightButtonReadingAvgIndex] = rightButtonAvgTouchVal;
	rightButtonReadingAvgIndex++;
	if (rightButtonReadingAvgIndex >= BUTTON_NUM_AVGS)
	{
		rightButtonReadingAvgIndex++;
	}
	rightButtonAvgTouchValOld = rightButtonReadingAvg[rightButtonReadingAvgIndex];
}

int readCapacitivePin(volatile uint8_t* ddr, volatile uint8_t* port, volatile uint8_t* pin, uint8_t pinNumber)
{
	uint8_t bitmask = (1 << pinNumber);

	// set to low
	*port &= ~(bitmask);
	// set to output
	*ddr |= bitmask;
	_delay_ms(1);
	// No background interrupts
	cli();
	// Make pin input with pull-up on
	*ddr &= ~(bitmask);
	*port |= bitmask;
	
	// Check to see how long the pin stays high
	// Manual if statements instead of a loop to decrease hardware cycles between each read of the pin and increase sensitivity as a result
	
	uint8_t cycles = 17;
	if (*pin & bitmask) { cycles = 0; }
	else if (*pin & bitmask) { cycles = 0; }
	else if (*pin & bitmask) { cycles = 1; }
	else if (*pin & bitmask) { cycles = 2; }
	else if (*pin & bitmask) { cycles = 3; }
	else if (*pin & bitmask) { cycles = 4; }
	else if (*pin & bitmask) { cycles = 5; }
	else if (*pin & bitmask) { cycles = 6; }
	else if (*pin & bitmask) { cycles = 7; }
	else if (*pin & bitmask) { cycles = 8; }
	else if (*pin & bitmask) { cycles = 9; }
	else if (*pin & bitmask) { cycles = 10; }
	else if (*pin & bitmask) { cycles = 11; }
	else if (*pin & bitmask) { cycles = 12; }
	else if (*pin & bitmask) { cycles = 13; }
	else if (*pin & bitmask) { cycles = 14; }
	else if (*pin & bitmask) { cycles = 15; }
	else if (*pin & bitmask) { cycles = 16; }
	
	// Turn background interrupts back on	
	sei();
	
	// Discharge the pin by setting it to low and output
	// Extremely important to pull these back down, otherwise you cannot use this on multiple pins
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