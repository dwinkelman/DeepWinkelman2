/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 16 April 2017
*
* Bitboard for making and unmaking moves.
*/

#ifndef DEEP_WINKELMAN_BITBOARD
#define DEEP_WINKELMAN_BITBOARD

#include <vector>

#include "move.h"
#include "score.h"
#include "params.h"

class BitboardData {
protected:
	friend class Bitboard;
	static Hash_t zobrist_keys[13][64];
	static void init_zobrist();
	static bool is_zobrist_inited;

public:
	// Current positions of white and black pieces and white and black pawns.
	Bitmask_t white, black, wpawns, bpawns;
	// Current positions of all pieces represented as bitmasks
	Bitmask_t pieces[13];
	// Color to move.
	Color_t color;
	// If an en passant can happen, the start and end coordinates.
	// No en passant is indicated with a null move
	BitboardMove ep;
	// The options for castling kingside/queenside for white and black.
	// No castling privileges is indicated with 0
	Castling_t castling;
	// The move to get to the next dataset
	BitboardMove move1, move2;
	// The hash of the current position.
	Hash_t hash;
	// The sum of the values of all pieces
	Score_t piece_score;
	// The sum of the score of the positioning of the pieces
	Score_t positioning_score;
	// The number of pieces on the board
	unsigned int n_pieces;
	// Position of the kings
	Coord_t white_king, black_king;

	BitboardData() {
		if (!is_zobrist_inited) {
			init_zobrist();
			is_zobrist_inited = true;
		}

		// zero-initialize everything (since stupid VC++ likes 0xcc)
		white = black = wpawns = bpawns = 0;
		color = WHITE;
		ep = BitboardMove(NO_MOVE, NO_MOVE);
		castling = 0;
		move1 = move2 = BitboardMove(NO_MOVE, NO_MOVE);
		hash = 0;
		piece_score = 0;
		n_pieces = 0;
		white_king = black_king = 0;
	}
};

class Bitboard {
protected:
	// Table to store piece positions
	Piece_t squares[64];
	Piece_t DEFAULT_POS[64] = {
		4,2,3,5,6,3,2,4,
		1,1,1,1,1,1,1,1,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		7,7,7,7,7,7,7,7,
		10,8,9,11,12,9,8,10 };
	// Move history storage
	const static unsigned int HISTORY_DEPTH = MAX_SEARCH_DEPTH;
	BitboardData history[HISTORY_DEPTH], temp;
	int depth;

	// Scoring parameters
	ScoreParams sparams;

	// Move finding
	static MoveManager move_manager;

public:
	Bitboard();
	// Create from a 64 byte grid
	Bitboard(const Piece_t squares[64], const Color_t color, const Castling_t castling);

	inline Piece_t operator[](const int index) const {
		return squares[index];
	}

protected:
	// Make a move to the bitboard.
	// Returns whether a capture occurred.
	// Sophisticated version with specification of current/output data.
	// Pass special move codes to start and parameters to end.
	bool make(const Coord_t start, const Coord_t end, const Piece_t promotion_piece,
		const BitboardData & current, BitboardData & next);

	// Undo the effects of an individual move on the bitboard.
	void unmake(const BitboardMove & move);

	bool make_normal(const Coord_t start, const Coord_t end);
	bool make_castling(Castling_t castling);
	bool make_ep(const Coord_t start, const Coord_t end);
	bool make_promotion(const Coord_t start, const Coord_t end, const Piece_t promotion_piece);

	void increment_depth();

public:
	// Make a move to change the board state
	bool make(const Move move);
	// Make a series of moves to the board
	void make(std::vector<Move> & moves);
	// Go back a certain number of moves
	void unmake();

	// Get a list of moves available in the position
	// The moves are guaranteed to be sorted according to start then end
	std::vector<Move> get_moves() const;

	// Access to read-only current board state
	inline const BitboardData & current_data() const {
		return history[depth];
	}

	// Get the score of the material on the board
	Score_t score_material() const;
	// Get the score of the pawn structure
	Score_t score_pawn_structure() const;
	// Get the score of the mobility/position of the pieces
	Score_t score_piece_position() const;
	// Get the king safety score
	Score_t score_king_safety() const;

	typedef Score_t(Bitboard::*ScoreFunction)() const;
	// Softest scoring setting based only on material
	Score_t score_level_0() const;
	// Next hardest scoring setting to determine rough score for end nodes
	Score_t score_level_1() const;

	typedef Move_Rank_t(Bitboard::*MoveRankFunction)(const Move);
	// Get a ranking for likely best move before exploring
	Move_Rank_t move_rank(const Move move);

	friend std::ostream & operator <<(std::ostream & os, const Bitboard & bitboard);
	friend std::ostream & operator <<(std::ostream & os, std::vector<Move> & move_set);
};


#endif