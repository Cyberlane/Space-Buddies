/*
 * SoftPWM.h
 *
 * Created: 07/10/2014 22:17:20
 *  Author: Justin
 */ 

#define RED_L PD2
#define RED_L_PORT PORTD
#define RED_L_DDR DDRD
#define RED_L_ON() (RED_L_PORT |= (1 << RED_L))
#define RED_L_OFF() (RED_L_PORT &= ~(1 << RED_L))

#define GREEN_L PD1
#define GREEN_L_PORT PORTD
#define GREEN_L_DDR DDRD
#define GREEN_L_ON() (GREEN_L_PORT |= (1 << GREEN_L))
#define GREEN_L_OFF() (GREEN_L_PORT &= ~(1 << GREEN_L))

#define BLUE_L PD0
#define BLUE_L_PORT PORTD
#define BLUE_L_DDR DDRD
#define BLUE_L_ON() (BLUE_L_PORT |= (1 << BLUE_L))
#define BLUE_L_OFF() (BLUE_L_PORT &= ~(1 << BLUE_L))

#define RED_R PB0
#define RED_R_PORT PORTB
#define RED_R_DDR DDRB
#define RED_R_ON() (RED_R_PORT |= (1 << RED_R))
#define RED_R_OFF() (RED_R_PORT &= ~(1 << RED_R))

#define GREEN_R PB1
#define GREEN_R_PORT PORTB
#define GREEN_R_DDR DDRB
#define GREEN_R_ON() (GREEN_R_PORT |= (1 << GREEN_R))
#define GREEN_R_OFF() (GREEN_R_PORT &= ~(1 << GREEN_R))

#define BLUE_R PB2
#define BLUE_R_PORT PORTB
#define BLUE_R_DDR DDRB
#define BLUE_R_ON() (BLUE_R_PORT |= (1 << BLUE_R))
#define BLUE_R_OFF() (BLUE_R_PORT &= ~(1 << BLUE_R))

#define BUTTON_LEFT PD3
#define BUTTON_LEFT_PIN PIND
#define BUTTON_LEFT_PORT PORTD
#define BUTTON_LEFT_DDR DDRD

#define BUTTON_RIGHT PD7
#define BUTTON_RIGHT_PIN PIND
#define BUTTON_RIGHT_PORT PORTD
#define BUTTON_RIGHT_DDR DDRD

#define SPEAKER PD5
#define SPEAKER_PORT PORTD
#define SPEAKER_DDR DDRD
#define SPEAKER_ON() (SPEAKER_PORT |= (1 << SPEAKER))
#define SPEAKER_OFF() (SPEAKER_PORT &= ~(1 << SPEAKER))

#define IR_TX PD6
#define IR_TX_PORT PORTD
#define IR_TX_DDR DDRD
#define IR_TX_ON() (IR_TX_PORT |= (1 << IR_TX))
#define IR_TX_OFF() (IR_TX_PORT &= ~(1 << IR_TX))

#define IR_RX PC5
#define IR_RX_PIN PINC
#define IR_RX_PORT PORTC
#define IR_RX_DDR DDRC
#define IR_RX_READ() (IR_RX_PIN & (1 << IR_RX))
#define IR_RX_ON() (IR_RX_PORT |= (1 << IR_RX))

void softpwm_init(void);