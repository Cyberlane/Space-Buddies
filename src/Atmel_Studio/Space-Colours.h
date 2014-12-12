/*
 * Space_Colours.h
 *
 * Created: 16/10/2014 21:02:41
 *  Author: Justin
 */ 

// Software PWM
volatile uint8_t red = 180;
volatile uint8_t green = 255;
volatile uint8_t blue = 255;
volatile uint8_t cnt = 0;

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} colors_codes_t;

colors_codes_t colours[10] = {
	{ 0, 0, 0 }, // white
	{ 0, 100, 200 }, // violet
	{ 200, 100, 0 }, // aqua
	{ 200, 0, 100 }, // baby blue
	{ 50, 200, 50 }, // green/blue
	{ 0, 240, 240 }, // pink
	{ 0, 255, 255 }, // red
	{ 240, 240, 0 }, // kryptonite green
	{ 255, 0, 255 }, // blue
	{ 255, 255, 0 } // green
};

void set_colour(volatile colors_codes_t *codes);