#define MAX_BARS 50

#define tone_C 1911
#define tone_C1 1804
#define tone_D 1703
#define tone_Eb 1607
#define tone_E 1517
#define tone_F 1432
#define tone_F1 1352
#define tone_G 1276
#define tone_Ab 1204
#define tone_A 1136
#define tone_Bb 1073
#define tone_B 1012
#define tone_c 955
#define tone_c1 902
#define tone_d 851
#define tone_eb 803
#define tone_e 758
#define tone_f 716
#define tone_f1 676
#define tone_g 638
#define tone_ab 602
#define tone_a 568
#define tone_bb 536
#define tone_b 506
#define tone_p -1  //pause
#define tone_null 0

typedef enum
{
	A_MINOR,
	E_MINOR,
	B_MINOR,
	F_SHARP,
	C_SHARP,
	G_SHARP,
	E_FLAT,
	B_FLAT,
	F_MINOR,
	C_MINOR,
	G_MINOR,
	D_MINOR
} scale_t;

typedef struct _note_
{
	int tone;
	int duration;
} note;

typedef struct _tune_
{
	note notes[MAX_BARS * 24];
	int curr_duration;
} tune;

int count_types[3] = { 4, 3, 2 };

// mario
int mario_m[] = { tone_e, tone_e, tone_e, tone_c, tone_e, tone_g, tone_G, tone_c, tone_G, tone_E, tone_A, tone_B, tone_Bb, tone_A, tone_G, tone_e, tone_g, tone_a, tone_f, tone_g, tone_e, tone_c, tone_d, tone_B, tone_c };
int mario_r[] = { 6, 12, 12, 6, 12, 24, 24, 18, 18, 18, 12, 12, 6, 12, 8, 8, 8, 12, 6, 12, 12, 6, 6, 6, 12 };

tune mario = {
  {
    { tone_e, 6 },  { tone_e, 12 },  { tone_e, 12 }, { tone_c, 6 },  { tone_e, 12 },  { tone_g, 24 },
    { tone_G, 24 }, { tone_c, 18 },  { tone_G, 18 }, { tone_E, 18 }, { tone_A, 12 },  { tone_B, 12 },
    { tone_Bb, 6 }, { tone_A, 12 },  { tone_G, 8 },  { tone_e, 8 },  { tone_g, 8 },   { tone_a, 12 },
    { tone_f, 6 },  { tone_g, 12 },  { tone_e, 12 }, { tone_c, 6 },  { tone_d, 6 },   { tone_B, 6 },
    { tone_c, 12 }
  },
  288
};

int scales[12][18] = {
	{ tone_A, tone_a, tone_B, tone_b, tone_C, tone_c, tone_D, tone_d, tone_E, tone_e, tone_F, tone_f, tone_E, tone_e, tone_F, tone_f, tone_G, tone_g }, // a minor
	{ tone_E, tone_e, tone_F1, tone_f1, tone_G, tone_g, tone_A, tone_a, tone_B, tone_b, tone_C, tone_c, tone_D, tone_d }, // e minor
	{ tone_B, tone_b, tone_C1, tone_c1, tone_D, tone_d, tone_E, tone_e, tone_F1, tone_f1, tone_G, tone_g, tone_A, tone_a }, // b minor
	{ tone_F1, tone_f1, tone_Ab, tone_ab, tone_A, tone_a, tone_B, tone_b, tone_C1, tone_c1, tone_D, tone_d, tone_E, tone_e }, // f sharp
	{ tone_C1, tone_c1, tone_Eb, tone_eb, tone_E, tone_e, tone_F1, tone_f1, tone_A, tone_a, tone_B, tone_b }, // c sharp
	{ tone_Eb, tone_eb, tone_Bb, tone_bb, tone_B, tone_b, tone_C1, tone_c1, tone_Eb, tone_eb, tone_E, tone_e, tone_F1, tone_f1 }, // g sharp
	{ tone_Eb, tone_eb, tone_F, tone_f, tone_F1, tone_f1, tone_Ab, tone_ab, tone_Bb, tone_bb, tone_C, tone_c, tone_F, tone_f }, // e flat
	{ tone_Bb, tone_bb, tone_C, tone_c, tone_C1, tone_c1, tone_Eb, tone_eb, tone_F, tone_f, tone_F1, tone_f1, tone_Ab, tone_ab }, // b flat
	{ tone_F, tone_f, tone_G, tone_g, tone_Ab, tone_ab, tone_Bb, tone_bb, tone_C, tone_c, tone_C1, tone_c1, tone_Eb, tone_eb }, // f minor
	{ tone_C, tone_c, tone_D, tone_d, tone_Eb, tone_eb, tone_F, tone_f, tone_G, tone_g, tone_Ab, tone_ab, tone_Bb, tone_bb }, // c minor
	{ tone_G, tone_g, tone_A, tone_a, tone_Bb, tone_bb, tone_C, tone_c, tone_D, tone_d, tone_Eb, tone_eb, tone_F, tone_f }, // g minor
	{ tone_D, tone_d, tone_E, tone_e, tone_F, tone_f, tone_G, tone_g, tone_A, tone_a, tone_Bb, tone_bb, tone_C, tone_c } // d minor
};

int durations[] = { 2, 4, 6, 8, 12, 16, 24 };
 
const int PIEZO = 6;
 
long vel = 20000;


int play_tune[0];
int play_dur[0];

void setup()
{
    pinMode(PIEZO, OUTPUT);
    delay(2000);
    //play_original();  // Somehow this works, but only when play_my_tune() is commented out
    play_my_tune(mario);  // This just does not work...
}

void make_tune(int bars)
{
  // int scale[] = scales[random(0,12)];
  // int maxScale = sizeof(scale);
  // int maxDuration = sizeof(durations);
  // for (int i = 0; i < sizeof(bars); i++) {
  //   int durationCount = 0;
  //   int* bar[1];
  //   while (1 / durationCount != 1) {
  //     int tone = scale[random(0,maxScale-1)];
  //     int duration = scale[random(0,maxDuration-1)];
  //     durationCount += duration;
  //   }
  // }
}
 
void loop()
{
  //play_my_tune(play_dur, play_tune, sizeof(play_tune));
}

void play_original()
{
    int LEN = sizeof(mario_r) / sizeof(int);
    for (int i=0; i < LEN; i++)
    {
        int tom = mario_m[i];
        int tempo = mario_r[i];
        long tvalue = tempo * vel;
        tocar(tom, tvalue);
        delayMicroseconds(1000); //pause between notes
    }
}

void play_my_tune(tune t)
{
    int LEN = sizeof(t) / sizeof(tune);
    for (int i=0; i < LEN; i++)
    {
        int tom = mario.notes[i].tone;
        int tempo = mario.notes[i].duration;
        long tvalue = tempo * vel;
        tocar(tom, tvalue);
        delayMicroseconds(1000); //pause between notes
    }
}
 
void tocar(int tom, long tempo_value)
{
     long tempo_gasto = 0;
     while (tempo_gasto < tempo_value && tempo_value < 640000) // enters an infinite loop when tempo_value is a big value
     {  
        digitalWrite(PIEZO, HIGH);
        delayMicroseconds(tom / 2);
 
        digitalWrite(PIEZO, LOW);
        delayMicroseconds(tom/2);
 
        tempo_gasto += tom;
    }
}



/* 
// Looks like zelda
int zelda_m[] = {tone_G, tone_E, tone_D, tone_C, tone_D, tone_E, tone_G, tone_E, tone_D, tone_C, tone_D, tone_E, tone_D, tone_E,tone_G, tone_E, tone_G, tone_A, tone_E, tone_A, tone_G, tone_E, tone_D, tone_C};
int zelda_r[] = {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 16};
 
// Some electric guitar...
int smoke_m[] = {tone_E, tone_G, tone_A, tone_E, tone_G, tone_Bb, tone_A, tone_E, tone_G, tone_A, tone_G, tone_E};
int smoke_r[] = {12, 12, 18, 12, 12, 6, 18, 12, 12, 18, 12, 24};
 
// Start like something similar to le reine au nez rouge...
int natal_m[] = {tone_G, tone_A, tone_G, tone_E, tone_G, tone_A, tone_G, tone_E, tone_c, tone_c, tone_A, tone_B, tone_B, tone_G, tone_A, tone_G, tone_A, tone_c, tone_B, tone_A, tone_G, tone_A, tone_G, tone_E};
int natal_r[] = {12, 4, 8, 16, 12, 4, 8, 16, 12, 4, 16, 12, 4, 16, 12, 4, 8, 8, 8, 8, 12, 4, 8, 16};
 
// Seems like some electric guitar solo...
int LTS_m[] = {tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_C, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_G, tone_Bb, tone_G, tone_F, tone_D, tone_F, tone_D, tone_G, tone_F, tone_D, tone_C, tone_Bb, tone_G, tone_Bb, tone_C, tone_C1, tone_C, tone_Bb, tone_F, tone_D, tone_Bb, tone_G, tone_F, tone_D, tone_C, tone_Bb, tone_D, tone_C, tone_Bb, tone_G} ;
int LTS_r[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
*/
