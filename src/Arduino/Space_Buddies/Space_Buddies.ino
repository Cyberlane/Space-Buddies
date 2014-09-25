//#define COMMON_ANODE
#define NOTE(tone, dur) ((((((uint8_t)(dur)) & 0x07)<<5) | (((uint8_t)(tone)) & 0x1F)))
#define GET_TONE(note) ((note) & 0x1F)
#define GET_DURATION(note) (((note)>>5) & 0x07)
#define END_MARKER 0xFF

enum {
	T_REST,
	T_C,
	T_CS,
	T_D,
	T_EB,
	T_E,
	T_F,
	T_FS,
	T_G,
	T_AB,
	T_A,
	T_BB,
	T_B,
	T_CX,
	T_CSX,
	T_DX,
	T_EBX,
	T_EX,
	T_FX,
	T_FSX,
	T_GX,
	T_ABX,
	T_AX,
	T_BBX,
	T_BX
};

uint8_t mario[] = {
	NOTE(T_EX, 2),  NOTE(T_EX, 4),  NOTE(T_EX, 4),  NOTE(T_CX, 2),  NOTE(T_EX, 4),  NOTE(T_GX, 7),
	NOTE(T_G, 7),   NOTE(T_CX, 6),  NOTE(T_G, 6),   NOTE(T_E, 6),   NOTE(T_A, 4),   NOTE(T_B, 4),
	NOTE(T_BB, 2),  NOTE(T_A, 4),   NOTE(T_G, 3),   NOTE(T_EX, 3),  NOTE(T_GX, 3),  NOTE(T_AX, 4),
	NOTE(T_FX, 2),  NOTE(T_GX, 4),  NOTE(T_EX, 4),  NOTE(T_CX, 2),  NOTE(T_DX, 2),  NOTE(T_B, 2),
	NOTE(T_CX, 4),  END_MARKER
};

uint8_t felix[] = {
	NOTE(T_EBX, 3), NOTE(T_REST, 2), NOTE(T_AB, 3), NOTE(T_FX, 2), NOTE(T_EBX, 3),
	NOTE(T_REST, 7), NOTE(T_EBX, 2), NOTE(T_CSX, 2), NOTE(T_EBX, 2), NOTE(T_CSX, 2),
	NOTE(T_BB, 2), NOTE(T_G, 2), NOTE(T_EB, 2), NOTE(T_CX, 3), NOTE(T_REST, 7),
	NOTE(T_CX, 2), NOTE(T_BB, 3), NOTE(T_EB, 8), NOTE(T_EB, 8), NOTE(T_EB, 8),
	NOTE(T_CX, 3), NOTE(T_AB, 2), NOTE(T_AB, 2), NOTE(T_AB, 2), NOTE(T_AB, 2),
	NOTE(T_BB, 3), NOTE(T_BB, 2), NOTE(T_BB, 2), NOTE(T_CX, 2), NOTE(T_DX, 2),
	NOTE(T_FX, 3), NOTE(T_EBX, 2), NOTE(T_EBX, 3), END_MARKER
};

uint8_t heman[] = {
	NOTE(T_AB, 3), NOTE(T_AB, 3), NOTE(T_AB, 3), NOTE(T_AB, 5), NOTE(T_F, 5), 
	NOTE(T_AB, 3), NOTE(T_BB, 3), NOTE(T_AB, 3), NOTE(T_FS, 3), NOTE(T_AB, 5), 
	NOTE(T_CS, 5), NOTE(T_AB, 3), NOTE(T_BB, 3), NOTE(T_AB, 3), NOTE(T_FS, 3), 
	NOTE(T_AB, 5), NOTE(T_F, 5), NOTE(T_CS, 3), NOTE(T_EB, 7), END_MARKER
};

uint8_t kirby[] = {
	NOTE(T_AB, 2), NOTE(T_BB, 2), NOTE(T_CX, 2), NOTE(T_DX, 2), NOTE(T_CX, 2),
	NOTE(T_DX, 2), NOTE(T_EBX, 4), NOTE(T_BB, 2), NOTE(T_G, 4), NOTE(T_BB, 2),
	NOTE(T_AB, 4), NOTE(T_G, 2), NOTE(T_F, 4), NOTE(T_G, 2), NOTE(T_EB, 4),
	NOTE(T_F, 0), NOTE(T_G, 0), NOTE(T_AB, 0), NOTE(T_BB, 0), NOTE(T_CX, 0),
	NOTE(T_DX, 0), NOTE(T_EBX, 4), END_MARKER
};

uint8_t morning[] = {
	NOTE(T_G, 3), NOTE(T_E, 3), NOTE(T_D, 3), NOTE(T_C, 3), NOTE(T_D, 3),
	NOTE(T_E, 3), NOTE(T_G, 3), NOTE(T_E, 3), NOTE(T_D, 3), NOTE(T_C, 3),
	NOTE(T_D, 1), NOTE(T_E, 1), NOTE(T_D, 1), NOTE(T_E, 1), NOTE(T_G, 3),
	NOTE(T_E, 3), NOTE(T_G, 3), NOTE(T_A, 3), NOTE(T_E, 3), NOTE(T_A, 3),
	NOTE(T_G, 3), NOTE(T_E, 3), NOTE(T_D, 3), NOTE(T_C, 5), END_MARKER
};

uint8_t pony[] = {
	NOTE(T_FSX, 3), NOTE(T_AX, 3), NOTE(T_FSX, 3), NOTE(T_EX, 5), NOTE(T_AX, 5), 
	NOTE(T_DX, 3), NOTE(T_EX, 3), NOTE(T_EX, 3), NOTE(T_CSX, 5), NOTE(T_A, 5), 
	NOTE(T_B, 5), NOTE(T_DX, 3), NOTE(T_DX, 3), NOTE(T_GX, 5), NOTE(T_BX, 5), 
	NOTE(T_AX, 3), NOTE(T_BX, 5), NOTE(T_AX, 7), END_MARKER
};

uint8_t simpsons[] = {
	NOTE(T_CX, 7), NOTE(T_EX, 5), NOTE(T_FSX, 5), NOTE(T_AX, 3), NOTE(T_GX, 7), 
	NOTE(T_EX, 5), NOTE(T_CX, 5), NOTE(T_A, 3), NOTE(T_FS, 3), NOTE(T_FS, 3), 
	NOTE(T_FS, 3), NOTE(T_G, 7), NOTE(T_G, 5), NOTE(T_REST, 3), NOTE(T_FS, 3), 
	NOTE(T_FS, 3), NOTE(T_FS, 3), NOTE(T_G, 3), NOTE(T_BB, 7), NOTE(T_REST, 0), 
	NOTE(T_B, 3), END_MARKER
};

uint8_t smurfs[] = {
	NOTE(T_CX, 7), NOTE(T_FX, 5), NOTE(T_CX, 3), NOTE(T_DX, 5), NOTE(T_BB, 5), 
	NOTE(T_G, 7), NOTE(T_CX, 7), NOTE(T_A, 3), NOTE(T_F, 5), NOTE(T_A, 5), 
	NOTE(T_G, 7), NOTE(T_REST, 5), NOTE(T_G, 1), NOTE(T_A, 1), NOTE(T_BB, 1), 
	NOTE(T_B, 1), NOTE(T_CX, 7), NOTE(T_FX, 5), NOTE(T_CX, 3), NOTE(T_DX, 5), 
	NOTE(T_BB, 5), NOTE(T_G, 7), NOTE(T_C, 7), NOTE(T_A, 3), NOTE(T_BB, 5), 
	NOTE(T_E, 5), NOTE(T_F, 7), END_MARKER
};

uint8_t sonic[] = {
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
};

uint8_t ducktales[] = {
	NOTE(T_FS, 3), NOTE(T_BB, 3), NOTE(T_CSX, 3), NOTE(T_EBX, 3), NOTE(T_EX, 3), 
	NOTE(T_EX, 1), NOTE(T_EBX, 1), NOTE(T_REST, 1), NOTE(T_CSX, 1), NOTE(T_B, 3), 
	NOTE(T_B, 5), NOTE(T_BB, 4), NOTE(T_REST, 5), NOTE(T_B, 5), NOTE(T_BB, 4), 
	NOTE(T_REST, 5), NOTE(T_FS, 3), NOTE(T_BB, 3), NOTE(T_CSX, 3), NOTE(T_EBX, 3), 
	NOTE(T_EX, 3), NOTE(T_EX, 1), NOTE(T_EBX, 1), NOTE(T_REST, 1), NOTE(T_CSX, 1), 
	NOTE(T_B, 3), NOTE(T_EX, 5), NOTE(T_EBX, 4), NOTE(T_REST, 5), NOTE(T_EX, 5), 
	NOTE(T_EBX, 4), NOTE(T_REST, 5), END_MARKER
};

uint8_t addams[] = {
	NOTE(T_E, 3), NOTE(T_A, 5), NOTE(T_CSX, 3), NOTE(T_A, 5), NOTE(T_FS, 3), 
	NOTE(T_D, 5), NOTE(T_B, 7), NOTE(T_G, 3), NOTE(T_AB, 5), NOTE(T_B, 3), 
	NOTE(T_AB, 5), NOTE(T_E, 3), NOTE(T_CS, 5), NOTE(T_A, 7), NOTE(T_E, 3), 
	NOTE(T_A, 5), NOTE(T_CSX, 3), NOTE(T_A, 5), NOTE(T_FS, 3), NOTE(T_D, 5), 
	NOTE(T_B, 5), NOTE(T_REST, 3), NOTE(T_A, 3), NOTE(T_AB, 5), NOTE(T_E, 3), 
	NOTE(T_FS, 5), NOTE(T_AB, 3), NOTE(T_A, 5), END_MARKER
};

/*
	State Summary:
	0: check for data
	1: send data
	2: play tune
*/
uint8_t state = 0;
// how fast to play tunes
long vel = 20000;
// IR receiver variables
#define MAXPULSE 65000
uint16_t pulses[400];
uint8_t currentPulse = 0;
uint16_t highpulse = 0, lowpulse = 0;
uint8_t currentBit = 0;
uint8_t currentByte = 0;
uint8_t buffer[50];
// IR sender
#define READ_BIT(byte, bit) ((byte & (1 << bit)) >> bit)
// Button config
#define BUTTON_NUM_READINGS 5
#define BUTTON_NUM_AVGS (BUTTON_NUM_READINGS + 1)
unsigned long buttonIdleTimer = 0;
// Left button
unsigned long leftButtonDebounce = 0;
unsigned long leftButtonTime = 0;
float leftButtonAvgTouchVal = 0;
float leftButtonAvgTouchValOld = 0;
boolean leftButtonPinTouched = false;
boolean leftButtonPinTouchedOld = false;
uint8_t leftButtonTotal = 0;
uint8_t leftButtonReadings[BUTTON_NUM_READINGS];
uint8_t leftButtonReadingIndex = 0;
float leftButtonReadingAvg[BUTTON_NUM_AVGS];
uint8_t leftButtonReadingAvgIndex = 0;
// Right button
unsigned long rightButtonDebounce = 0;
unsigned long rightButtonTime = 0;
float rightButtonAvgTouchVal = 0;
float rightButtonAvgTouchValOld = 0;
boolean rightButtonPinTouched = false;
boolean rightButtonPinTouchedOld = false;
uint8_t rightButtonTotal = 0;
uint8_t rightButtonReadings[BUTTON_NUM_READINGS];
uint8_t rightButtonReadingIndex = 0;
float rightButtonReadingAvg[BUTTON_NUM_AVGS];
uint8_t rightButtonReadingAvgIndex = 0;


void setup() {
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
	
  delayMicroseconds(100);
  
  for (int i = 0; i < BUTTON_NUM_READINGS; i++) {
    leftButtonReadings[i] = 0;
    rightButtonReadings[i];
  }
  for (int i = 0; i < BUTTON_NUM_AVGS; i++) {
    leftButtonReadingAvg[i] = 0;
    rightButtonReadingAvg[i] = 0;
  }
}

void loop() {
  switch(state) {
    case 0:
    read_ir_data();
    break;
    case 1:
    send_data(mario);
    break;
    case 2:
    play(buffer);
    break;
    default:
    state = 0;
    break;
  }
  checkLeftButton();
  checkRightButton();
}

void rightButtonPressed()
{
  PORTD ^= (1 << PD0);
}

void leftButtonPressed()
{
  PORTB ^= (1 << PB0);
}

void checkLeftButton()
{
  buttonIdleTimer = millis();
  while(1) {
	if (millis() - leftButtonTime > leftButtonDebounce)
	{
		getleftButtonReading();
		if (leftButtonAvgTouchVal - leftButtonAvgTouchValOld > 1.0)
		{
			leftButtonPinTouched = true;
		} else {
			leftButtonPinTouched = false;
		}
		if (leftButtonPinTouched == true && leftButtonPinTouchedOld == false)
		{
			leftButtonTime = millis();
			leftButtonPressed();
		} else if (leftButtonPinTouched == false && leftButtonPinTouchedOld == true)
		{
			leftButtonTime = millis();
		}
		leftButtonPinTouchedOld = leftButtonPinTouched;
	}
  if ((millis() - buttonIdleTimer > 2000) && (millis() - leftButtonTime > 2000))
  {
    return;
  }
  }
}

void getleftButtonReading()
{
	leftButtonTotal -= leftButtonReadings[leftButtonReadingIndex];
	leftButtonReadings[leftButtonReadingIndex] = readCapacitivePin(7);
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
		leftButtonReadingAvgIndex = 0;
	}
	leftButtonAvgTouchValOld = leftButtonReadingAvg[leftButtonReadingAvgIndex];
}

void checkRightButton()
{
  buttonIdleTimer = millis();
  while(1) {
	if (millis() - rightButtonTime > rightButtonDebounce)
	{
		getRightButtonReading();
		if (rightButtonAvgTouchVal - rightButtonAvgTouchValOld > 1.0)
		{
			rightButtonPinTouched = true;
			} else {
			rightButtonPinTouched = false;
		}
		if (rightButtonPinTouched == true && rightButtonPinTouchedOld == false)
		{
			rightButtonTime = millis();
			rightButtonPressed();
		} else if (rightButtonPinTouched == false && rightButtonPinTouchedOld == true)
		{
			rightButtonTime = millis();
		}
		rightButtonPinTouchedOld = rightButtonPinTouched;
	}
  if ((millis() - buttonIdleTimer > 2000) && (millis() - rightButtonTime > 2000))
  {
    return;
  }
  }
}

void getRightButtonReading()
{
	rightButtonTotal -= rightButtonReadings[rightButtonReadingIndex];
	rightButtonReadings[rightButtonReadingIndex] = readCapacitivePin(3);
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
		rightButtonReadingAvgIndex = 0;
	}
	rightButtonAvgTouchValOld = rightButtonReadingAvg[rightButtonReadingAvgIndex];
}

void sendIR(int cycles)
{
	// Turn off background interrupts
	cli();
	while(cycles--)
	{
		//TODO: Extract these into macros
		PORTD |= (1 << PD6);
		delayMicroseconds(10);
		PORTD &= ~(1 << PD6);
		delayMicroseconds(10);
	}
	// Turn background interrupts back on
	sei();
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
			delayMicroseconds(500);
			currentBit++;
		}
		++pByte;
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
			delayMicroseconds(20);
			
			/*
			If the pulse is too long, we have timed out
			Either nothing was received or the code is finished
			Process what we've grabbed so far and then reset
			*/
			if (highpulse >= MAXPULSE)
			{
                            if (currentPulse != 0) {
				processData();
				currentPulse = 0;
                            }
			    return;
			}
		}
		
		// While pin is low
		while(!(DDRC & (1 << PC6)))
		{
			lowpulse++;
			delayMicroseconds(20);
			if (lowpulse >= MAXPULSE)
			{
                            if (currentPulse != 0) {
				processData();
				currentPulse = 0;
                            }
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
		delayMicroseconds(tone / 2);
		
		PORTD &= ~(1 << PD5);
		delayMicroseconds(tone / 2);
		
		tempo_gasto += tone;
	}
}
