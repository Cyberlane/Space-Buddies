/*
 * Space_Buddies.c
 *
 * Created: 06/06/2014 22:56:55
 *  Author: Justin Nel
 */ 


#include <avr/io.h>
#include <util/delay.h>

#define NOTE(tone, dur) ((((((uint8_t)(dur)) & 0x07)<<5) | (((uint8_t)(tone)) & 0x1F)))
#define GET_TONE(note) ((note) & 0x1F)
#define GET_DURATION(note) (((note)>>5) & 0x07)
#define END_MARKER 0XFF

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

uint8_t zelda[] = {
	NOTE(T_G, 3), NOTE(T_E, 3), NOTE(T_D, 3), NOTE(T_C, 3), NOTE(T_D, 3),
	NOTE(T_E, 3), NOTE(T_G, 3), NOTE(T_E, 3), NOTE(T_D, 3), NOTE(T_C, 3),
	NOTE(T_D, 1), NOTE(T_E, 1), NOTE(T_D, 1), NOTE(T_E, 1), NOTE(T_G, 3),
	NOTE(T_E, 3), NOTE(T_G, 3), NOTE(T_A, 3), NOTE(T_E, 3), NOTE(T_A, 3),
	NOTE(T_G, 3), NOTE(T_E, 3), NOTE(T_D, 3), NOTE(T_C, 5), END_MARKER
};

uint8_t smoke[] = {
	NOTE(T_E, 4), NOTE(T_G, 4), NOTE(T_A, 6), NOTE(T_E, 4), NOTE(T_G, 4),
	NOTE(T_BB, 2), NOTE(T_A, 6), NOTE(T_E, 4), NOTE(T_G, 4), NOTE(T_A, 6),
	NOTE(T_G, 4), NOTE(T_E, 7), END_MARKER
};

uint8_t natal[] = {
	NOTE(T_G, 4), NOTE(T_A, 1), NOTE(T_G, 3), NOTE(T_E, 5), NOTE(T_G, 4),
	NOTE(T_A, 1), NOTE(T_G, 3), NOTE(T_E, 5), NOTE(T_CX, 4), NOTE(T_CX, 1),
	NOTE(T_A, 5), NOTE(T_B, 4), NOTE(T_B, 1), NOTE(T_G, 5), NOTE(T_A, 4),
	NOTE(T_G, 1), NOTE(T_A, 3), NOTE(T_CX, 3), NOTE(T_B, 3), NOTE(T_A, 3),
	NOTE(T_G, 4), NOTE(T_A, 1), NOTE(T_G, 3), NOTE(T_E, 5), END_MARKER
};

uint8_t LTS[] = {
	NOTE(T_BB, 1), NOTE(T_G, 1), NOTE(T_G, 1), NOTE(T_BB, 1), NOTE(T_G, 1),
	NOTE(T_G, 1), NOTE(T_BB, 1), NOTE(T_G, 1), NOTE(T_G, 1), NOTE(T_BB, 1),
	NOTE(T_G, 1), NOTE(T_G, 1), NOTE(T_BB, 1), NOTE(T_G, 1), NOTE(T_C, 1),
	NOTE(T_G, 1), NOTE(T_BB, 1), NOTE(T_G, 1), NOTE(T_G, 1), NOTE(T_BB, 1),
	NOTE(T_G, 1), NOTE(T_G, 1), NOTE(T_BB, 1), NOTE(T_G, 1), NOTE(T_G, 1),
	NOTE(T_BB, 1), NOTE(T_G, 1), NOTE(T_G, 1), NOTE(T_BB, 1), NOTE(T_G, 1),
	NOTE(T_F, 1), NOTE(T_D, 1), NOTE(T_F, 1), NOTE(T_D, 1), NOTE(T_G, 1),
	NOTE(T_F, 1), NOTE(T_D, 1), NOTE(T_C, 1), NOTE(T_BB, 1), NOTE(T_G, 1),
	NOTE(T_BB, 1), NOTE(T_C, 1), NOTE(T_CS, 1), NOTE(T_C, 1), NOTE(T_BB, 1),
	NOTE(T_F, 1), NOTE(T_D, 1), NOTE(T_BB, 1), NOTE(T_G, 1), NOTE(T_F, 1),
	NOTE(T_D, 1), NOTE(T_C, 1), NOTE(T_BB, 1), NOTE(T_D, 1), NOTE(T_C, 1),
	NOTE(T_BB, 1), NOTE(T_G, 1), END_MARKER
};

//const int PIEZO = 6;
long vel = 20000;

int main(void)
{
	DDRD = DDRD | 0b01000000; // Set pin 6 to OUTPUT
	delay_ms(2000);
	play(mario);
    while(1)
    {
        //TODO:: Please write your application code 
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

void delay_ms(uint16_t count)
{
	count = count * 2;
	while(count--)
	{
		_delay_ms(1);
	}
}

void delay_us(uint16_t count)
{
	count = count * 2;
	while(count--)
	{
		_delay_us(1);
	}
}

void tocar(int tom, long tempo_value)
{
	long tempo_gasto = 0;
	while (tempo_gasto < tempo_value && tempo_value < 640000) // enters an infinite loop when tempo_value is a big value
	{
		PORTD = PORTD | 0b01000000; // Drive pin 6 to HIGH
		delay_us(tom / 2);
		
		PORTD = PORTD & 0b10111111; // Drop pin 6 to LOW
		delay_us(tom / 2);
		
		tempo_gasto += tom;
	}
}