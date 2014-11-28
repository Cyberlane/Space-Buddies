/*
 * Space_Buddies.h
 *
 * Created: 08/06/2014 00:09:25
 *  Author: Justin
 */ 

#define NOTE(tone, dur) ((((((uint8_t)(dur)) & 0x07)<<5) | (((uint8_t)(tone)) & 0x1F)))
#define GET_TONE(note) ((note) & 0x1F)
#define GET_DURATION(note) (((note)>>5) & 0x07)
#define END_MARKER 0XFF
#define READ_BIT(byte, bit) ((byte & (1 << bit)) >> bit)

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

void intialise_game(void);
void timer_init(void);
void send_IR(int cycles);
void send_IR_bit(uint8_t bit);
void read_ir_data(void);
uint8_t find_next_tune(uint8_t currentTune);
void check_buttons(void);
uint8_t read_capacitive_pin(volatile uint8_t* ddr, volatile uint8_t* port, volatile uint8_t* pin, uint8_t pinNumber);
void left_button_pressed(void);
void right_button_pressed(void);
void start_timer2(void);
void stop_timer2(void);
void save_available_tunes(void);
void clear_leds(void);

void send_data(volatile uint8_t index);
void save_buffer(volatile uint8_t *pByte);

void play_tune(uint8_t currentTune);
void play_byte(uint8_t pByte);
void play_tone(int tone, long tempo_value);