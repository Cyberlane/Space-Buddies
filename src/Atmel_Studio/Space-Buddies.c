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
//uint16_t pulses[400];

/*
	State Summary:
	0: check for data
	1: read data
	2: send data
	3: play tune
*/
uint8_t state = 0;
volatile uint8_t currentPulse = 0;
volatile uint16_t highpulse = 0, lowpulse = 0;
volatile uint8_t buffer[50];
volatile uint8_t currentBit = 0;
volatile uint8_t currentByte = 0;
uint8_t crc = 0;

uint8_t cycles;

//Untested: Migrate capactive touch logic from Arduino to AVR C
//TODO: Need logic to set "current tune", toggled by button press
//TODO: Move buffer into EEPROM when data is successful
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
	//DDRD |= (1 << PD5);
	// Set IR Transmitter as output
	DDRD |= (1 << PD6);
	// Set IR Receiver as input
	DDRC &= ~(1 << PC5);
	// Set internal pull-up for IR Receiver
	PORTC |= (1 << PC5);
	
	_delay_ms(100);
	
	TCCR1A = 0x00;
	TCCR1B |= (1 << WGM12)|(1 << CS11);
	TCNT1 = 0;
	OCR1A = 26;
	TIMSK |= (1 << OCIE1A);
	sei();
	
	state = 1;
	
	for (int i = 0; i < 50; i++) {
		buffer[i] = 0;
	}
	
    while(1)
    {
		//checkleftButton();
		//checkRightButton();
		//switch(state)
		//{
			//case 3:
			//play(buffer);
			//break;
			//case 2:
			//send_data(mario);
			//break;
			//default:
			//case 1:
			//read_ir_data();
			//break;
		//}
    }
}

ISR(TIMER1_COMPA_vect) {
	if (state == 1) {
		if (PINC & (1 << 5)) {
			//high
			if (lowpulse >= 200 && lowpulse <= 300)
			{
				//this is a 1
				buffer[currentByte] |= (1 << currentBit);
			}
			else if (lowpulse >= 50 && lowpulse <= 150)
			{
				//this is a 0
				buffer[currentByte] &= ~(1 << currentBit);
			}
			lowpulse = 0;
			highpulse++;
		} else {
			//low
			lowpulse++;
			highpulse = 0;
		}
		
		if (highpulse >= MAXPULSE || lowpulse >= MAXPULSE) {
			if (currentPulse != 0) {
				if (currentBit != 0) {
					// error, clear everything
				} else {
					buffer[currentByte] = END_MARKER;
					PORTD |= (1 << PD2);
					state = 3;
				}
				currentBit = currentByte = currentPulse = 0;
			}
			PORTD |= (1 << PD0);
			return;
		}
		
		if (++currentBit == 8)
		{
			currentBit = 0;
			currentByte++;
		}
		currentPulse++;
	}
}


void rightButtonPressed()
{
	PORTD ^= (1 << PD0);
}

void leftButtonPressed()
{
	PORTB ^= (1 << PB0);
}

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

void checkleftButton()
{
	//if (millis() - leftButtonTime > leftButtonDebounce)
	//{
		//getleftButtonReading();
		//if (leftButtonAvgTouchVal - leftButtonAvgTouchValOld > 1.0)
		//{
			//leftButtonPinTouched = true;
		//} else {
			//leftButtonPinTouched = false;
		//}
		//if (leftButtonPinTouched == true && leftButtonPinTouchedOld == false)
		//{
			//leftButtonTime = millis();
			//leftButtonPressed();
		//} else if (leftButtonPinTouched == false && leftButtonPinTouchedOld == true)
		//{
			//leftButtonTime = millis();
		//}
		//leftButtonPinTouchedOld = leftButtonPinTouched;
	//}
}

void getleftButtonReading()
{
	leftButtonTotal -= leftButtonReadings[leftButtonReadingIndex];
	leftButtonReadings[leftButtonReadingIndex] = readCapacitivePin(PD7);
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

void checkRightButton()
{
	//if (millis() - rightButtonTime > rightButtonDebounce)
	//{
		//getRightButtonReading();
		//if (rightButtonAvgTouchVal - rightButtonAvgTouchValOld > 1.0)
		//{
			//rightButtonPinTouched = true;
			//} else {
			//rightButtonPinTouched = false;
		//}
		//if (rightButtonPinTouched == true && rightButtonPinTouchedOld == false)
		//{
			//rightButtonTime = millis();
			//rightButtonPressed();
		//} else if (rightButtonPinTouched == false && rightButtonPinTouchedOld == true)
		//{
			//rightButtonTime = millis();
		//}
		//rightButtonPinTouchedOld = rightButtonPinTouched;
	//}
}

void getRightButtonReading()
{
	rightButtonTotal -= rightButtonReadings[rightButtonReadingIndex];
	rightButtonReadings[rightButtonReadingIndex] = readCapacitivePin(PD7);
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

int readCapacitivePin(int pinToMeasure)
{
	// set to output
	DDRD |= (1 << pinToMeasure);
	// set to low
	PORTD &= ~(1 << pinToMeasure);
	_delay_ms(1);
	// No background interrupts
	cli();
	// Make pin input with pull-up on
	DDRD &= ~(1 << pinToMeasure);
	PORTD |= (1 << pinToMeasure);
	
	// Check to see how long the pin stays high
	// Manual if statements instead of a loop to decrease hardware cycles between each read of the pin and increase sensitivity as a result
	
	cycles = 17;
	if (PORTD & (1 << pinToMeasure)) { cycles = 0; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 0; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 1; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 2; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 3; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 4; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 5; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 6; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 7; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 8; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 9; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 10; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 11; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 12; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 13; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 14; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 15; }
	else if (PORTD & (1 << pinToMeasure)) { cycles = 16; }
	
	// Turn background interrupts back on	
	sei();
	
	// Discharge the pin by setting it to low and output
	// Extremely important to pull these back down, otherwise you cannot use this on multiple pins
	PORTD &= ~(1 << pinToMeasure);
	DDRD |= (1 << pinToMeasure);
	
	return cycles;
}

void read_ir_data()
{
	//while(1)
	//{
		//// Start out with no pulse length
		//highpulse = lowpulse = 0;
		//
		//// While pin is high
		//while(DDRC & (1 << PC6))
		//{
			//highpulse++;
			//_delay_us(20);
			//
			///*
			//If the pulse is too long, we have timed out
			//Either nothing was received or the code is finished
			//Process what we've grabbed so far and then reset
			//*/
			//if ((highpulse >= MAXPULSE) && (currentPulse != 0))
			//{
				//processData();
				//currentPulse = 0;
				//return;
			//}
		//}
		//
		//// While pin is low
		//while(!(DDRC & (1 << PC6)))
		//{
			//lowpulse++;
			//_delay_us(20);
			//if ((lowpulse >= MAXPULSE) && (currentPulse != 0))
			//{
				//processData();
				//currentPulse = 0;
				//return;
			//}
		//}
		//
		//pulses[currentPulse] = lowpulse;
		//
		//// We've read one high, one low, now let's do another
		//currentPulse++;
	//}
}

void processData()
{
	//// we only want full bytes, which requires 8 bits
	//if ((currentPulse+1)%8 != 0)
	//{
		//// this is bad data, escape!
		//state = 2;
		//return;
	//}
	//
	//currentBit = 0;
	//currentByte = 0;
	//
	//for (uint8_t i = 0; i < currentPulse; i++)
	//{
		//if (pulses[i] >= 900 || pulses[i] <= 1500)
		//{
			//// this is a 1
			//buffer[currentByte] = buffer[currentByte] | (1 << currentBit);
		//} else if (pulses[i] >= 400 || pulses[i] <= 750)
		//{
			//// this is a 0
		//} else {
			//// bad data, escape!
			//memset(buffer, 0, 50);
			//state = 2;
			//return;
		//}
		//if (++currentBit == 8)
		//{
			//currentBit = 0;
			//currentByte++;
		//}
	//}
	//buffer[currentByte] = END_MARKER;
	//state = 3;
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
	// Turn off background interrupts
	cli();
	while(cycles--)
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