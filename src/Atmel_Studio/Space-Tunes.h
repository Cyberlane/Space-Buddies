/*
 * Space_Tunes.h
 *
 * Created: 16/06/2014 21:45:35
 *  Author: Justin
 */ 

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
