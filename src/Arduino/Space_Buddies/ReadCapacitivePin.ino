#include "pins_arduino.h"
uint8_t readCapacitivePin(int pinNumber) {
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  volatile uint8_t* pin;
  
  byte bitmask;
  port = portOutputRegister(digitalPinToPort(pinNumber));
  ddr = portModeRegister(digitalPinToPort(pinNumber));
  pin = portInputRegister(digitalPinToPort(pinNumber));
  bitmask = digitalPinToBitMask(pinNumber);
  
  *port &= ~(bitmask);
  *ddr |= bitmask;
  delay(1);
  
  noInterrupts();
  
  *ddr &= ~(bitmask);
  *port |= bitmask;
  
  uint8_t cycles = 17;
       if (*pin & bitmask) { cycles = 0; }
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
  
  interrupts();
  
  *port &= ~(bitmask);
  *ddr |= bitmask;
  
  return cycles;
}

uint8_t readCapacitivePin2(volatile uint8_t* ddr, volatile uint8_t* port, volatile uint8_t* pin, uint8_t pinNumber)
{
	uint8_t bitmask = (1 << pinNumber);

	// set to low
	*port &= ~(bitmask);
	// set to output
	*ddr |= (bitmask);
	delay(1);
	// No background interrupts
	cli();
	// Make pin input with pull-up on
	*ddr &= ~(bitmask);
	*port |= (bitmask);
	
	// Check to see how long the pin stays high
	// Manual if statements instead of a loop to decrease hardware cycles between each read of the pin and increase sensitivity as a result
	uint8_t cycles = 17;
		 if (*pin & bitmask) { cycles =  0; }
	else if (*pin & bitmask) { cycles =  1; }
	else if (*pin & bitmask) { cycles =  2; }
	else if (*pin & bitmask) { cycles =  3; }
	else if (*pin & bitmask) { cycles =  4; }
	else if (*pin & bitmask) { cycles =  5; }
	else if (*pin & bitmask) { cycles =  6; }
	else if (*pin & bitmask) { cycles =  7; }
	else if (*pin & bitmask) { cycles =  8; }
	else if (*pin & bitmask) { cycles =  9; }
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
	*ddr |= (bitmask);
	
	return cycles;
}
