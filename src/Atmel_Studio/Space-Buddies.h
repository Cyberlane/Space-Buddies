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
void send_IR_byte(uint8_t val);

uint8_t validate_buffer(uint8_t currentPulse, uint8_t currentBit, uint8_t currentByte, uint8_t *buffer, uint8_t errorCode);
uint8_t read_ir_data(void);

uint8_t reset_game(void);

uint8_t check_buttons(void);
uint8_t read_capacitive_pin(volatile uint8_t* ddr, volatile uint8_t* port, volatile uint8_t* pin, uint8_t pinNumber);
uint8_t is_left_button_pressed(void);
uint8_t is_right_button_pressed(void);
void clear_leds(void);

void show_error(uint8_t code, uint8_t subCode);
void show_signal(uint16_t signal);

uint8_t find_next_tune(uint8_t currentTune);
uint8_t get_tune_state(uint8_t tuneNumber);
void make_tune_available(uint8_t index);
uint8_t count_available_tunes();

void send_data(volatile uint8_t index);
uint8_t save_buffer(volatile uint8_t *pByte);

void play_tune(uint8_t currentTune);
void play_byte(uint8_t pByte);
void play_success();
void play_tone(int tone, long tempo_value);

void softpwm_init(void);
void delay_us(uint16_t count);