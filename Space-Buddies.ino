#define NOTE(tone, dur) ((((((byte)(dur)) & 0x07)<<5) | (((byte)(tone)) & 0x1F)))
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

// mario
// int mario_m[] = { tone_e, tone_e, tone_e, tone_c, tone_e, tone_g, tone_G, tone_c, tone_G, tone_E, tone_A, tone_B, tone_Bb, tone_A, tone_G, tone_e, tone_g, tone_a, tone_f, tone_g, tone_e, tone_c, tone_d, tone_B, tone_c };
// int mario_r[] = { 6, 12, 12, 6, 12, 24, 24, 18, 18, 18, 12, 12, 6, 12, 8, 8, 8, 12, 6, 12, 12, 6, 6, 6, 12 };

byte mario[] = {
    NOTE(T_EX, 2),  NOTE(T_EX, 4),  NOTE(T_EX, 4),  NOTE(T_CX, 2),  NOTE(T_EX, 4),  NOTE(T_GX, 7),
    NOTE(T_G, 7),   NOTE(T_CX, 6),  NOTE(T_G, 6),   NOTE(T_E, 6),   NOTE(T_A, 4),   NOTE(T_B, 4),
    NOTE(T_BB, 2),  NOTE(T_A, 4),   NOTE(T_G, 3),   NOTE(T_EX, 3),  NOTE(T_GX, 3),  NOTE(T_AX, 4),
    NOTE(T_FX, 2),  NOTE(T_GX, 4),  NOTE(T_EX, 4),  NOTE(T_CX, 2),  NOTE(T_DX, 2),  NOTE(T_B, 2),
    NOTE(T_CX, 4),  END_MARKER
};

int durations[] = { 2, 4, 6, 8, 12, 16, 18, 24 };
 
const int PIEZO = 6;
 
long vel = 20000;

void setup()
{
    pinMode(PIEZO, OUTPUT);
    delay(2000);
    play(mario);
    //play_original();  // Somehow this works, but only when play_my_tune() is commented out
    // play_my_tune(mario);  // This just does not work...
}
 
void loop()
{
  //play_my_tune(play_dur, play_tune, sizeof(play_tune));
}

void play(byte *pByte)
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

// void play_original()
// {
//     int LEN = sizeof(mario_r) / sizeof(int);
//     for (int i=0; i < LEN; i++)
//     {
//         int tom = mario_m[i];
//         int tempo = mario_r[i];
//         long tvalue = tempo * vel;
//         tocar(tom, tvalue);
//         delayMicroseconds(1000); //pause between notes
//     }
// }
 
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
