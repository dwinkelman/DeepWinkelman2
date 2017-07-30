/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 19 April 2017
*
* Values and methods for scoring a position.
*/

#ifndef DEEP_WINKELMAN_SCORE
#define DEEP_WINKELMAN_SCORE

#include <stdint.h>

// Only actually 24 bits long
typedef int32_t Score_t;
typedef int32_t Move_Rank_t;

#define SCORE_WHITE_WIN 0x007fffff
#define SCORE_BLACK_WIN -0x007fffff
#define SCORE_DRAW 0x0
#define SCORE_INVALID 0x00cccccc

// Container for the scoring parameters to be used.
// Scores are in thousands of a pawn.
class ScoreParams {
public:
	// Values for each of the pieces.
	Score_t PIECE_VALUES[13] = {
		0, 1000, 3000, 3200, 5000, 9000, 2000000, -1000, -3000, -3200, -5000, -9000, -2000000
	};

	// Values for each of the moves of each piece
	Score_t PIECE_MOBILITY[13] = {
		0, 100, 120, 130, 140, 140, 50, -100, -120, -130, -140, -140, -50
	};

	// Score for each pawn defended by another pawn
	Score_t PAWN_DEFENDING_PAWN = 121;
	// Score for each piece defended by a pawn
	Score_t PAWN_DEFENDING_PIECE = 85;
	// Score for each pawn blocked by another piece
	Score_t PAWN_BLOCKED = -63;
	// Score for each doubled pawn
	Score_t PAWN_DOUBLED = -200;
	// Score for each attack into the center 16 squares
	Score_t PAWN_CENTER_ATTACK = 52;
	// Score for each pawn on each rank
	Score_t
		PAWN_RANK_2 = 40,
		PAWN_RANK_3 = 78,
		PAWN_RANK_4 = 105,
		PAWN_RANK_5 = 150,
		PAWN_RANK_6 = 200,
		PAWN_RANK_7 = 400;
};

#endif