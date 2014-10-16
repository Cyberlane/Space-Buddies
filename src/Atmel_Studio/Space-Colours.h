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

typedef struct {
	colors_codes_t color1;
	colors_codes_t color2;
	colors_codes_t color3;
	colors_codes_t color4;
	colors_codes_t color5;
	colors_codes_t color6;
	colors_codes_t color7;
	colors_codes_t color8;
	colors_codes_t color9;
	colors_codes_t color10;
} color_values_t;

colors_codes_t colours[10] = {
	{ // white
		.red = 0,
		.blue = 0,
		green = 0
	},
	{ // violet
		.red = 0,
		.blue = 100,
		.green = 200
	},
	{ // aqua
		.red = 200,
		.blue = 100,
		.green = 0
	},
	{ // baby blue
		.red = 200,
		.blue = 0,
		.green = 100
	},
	{ // green/blue
		.red = 50,
		.blue = 200,
		.green = 50
	},
	{ // pink
		.red = 0,
		.blue = 240,
		.green = 240
	},
	{ // red
		.red = 0,
		.blue = 255,
		.green = 255
	},
	{ // kryptonite green
		.red = 240,
		.blue = 240,
		.green = 0
	},
	{ // blue
		.red = 255,
		.blue = 0,
		.green = 255
	},
	{ // green
		.red = 255,
		.blue = 255,
		.green = 0
	}
};