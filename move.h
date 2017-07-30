/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 21 April 2017
*
* Structures for storing types of moves
*/

#ifndef DEEP_WINKELMAN_MOVE
#define DEEP_WINKELMAN_MOVE

#include <string>

#include "movetable.h"

#define NO_MOVE 0xff

typedef uint64_t Hash_t;

// Piece codes for start and end piece
typedef unsigned char Piece_t;
#define NO_PIECE 0x00
#define WHITE_PAWN 0x01
#define WHITE_KNIGHT 0x02
#define WHITE_BISHOP 0x03
#define WHITE_ROOK 0x04
#define WHITE_QUEEN 0x05
#define WHITE_KING 0x06
#define BLACK_PAWN 0x07
#define BLACK_KNIGHT 0x08
#define BLACK_BISHOP 0x09
#define BLACK_ROOK 0x0a
#define BLACK_QUEEN 0x0b
#define BLACK_KING 0x0c

// Masks for castling options
typedef unsigned char Castling_t;
#define WHITE_OO 0x01
#define WHITE_OOO 0x02
#define BLACK_OO 0x04
#define BLACK_OOO 0x08

// Color for the move
typedef signed char Color_t;
#define WHITE 1
#define BLACK -1

// Constants for special moves
#define MOVE_CASTLING 13
#define MOVE_EN_PASSANT 14
#define MOVE_NULL 15

// Masks for determining castling
#define WHITE_OO_MASK 0x60
#define WHITE_OOO_MASK 0xe
#define BLACK_OO_MASK 0x6000000000000000
#define BLACK_OOO_MASK 0xe00000000000000

// Move that has only information essential for making.
// Can be a stand-alone move for 
struct Move {
protected:
	// Start is most significant; promotion is least significant
	uint16_t data;

public:
	Move(const Coord_t start, const Coord_t end,
		const Piece_t promotion_piece = 0) {
		set_start(start);
		set_end(end);
		set_promotion(promotion_piece);
	}
	Move(const std::string str) {
		set_start(		str.at(0) - 'a' + (str.at(1) - '1') * 8);
		set_end(		str.at(3) - 'a' + (str.at(4) - '1') * 8);
		set_promotion(	0);
	}
	Move() : Move(0, 0, MOVE_NULL) {}
	inline void set_start(Coord_t start) {
		data = (data & 0x03ff) | ((start & 0x3f) << 10);
	}
	inline Coord_t start() const {
		return (data >> 10) & 0x003f;
	}
	inline void set_end(Coord_t end) {
		data = (data & 0xfc0f) | ((end & 0x3f) << 4);
	}
	inline Coord_t end() const {
		return (data >> 4) & 0x003f;
	}
	inline void set_promotion(Coord_t promotion) {
		data = (data & 0xfff0) | (promotion & 0xf);
	}
	inline Piece_t promotion_piece() const {
		return data & 0x000f;
	}
	inline Castling_t castling_type() const {
		return start() & 15;
	}
	inline bool is_null() const {
		return promotion_piece() == MOVE_NULL;
	}
	inline bool is_normal() const {
		return promotion_piece() == 0;
	}
	inline bool is_castling() const {
		return promotion_piece() == MOVE_CASTLING;
	}
	inline bool is_en_passant() const {
		return promotion_piece() == MOVE_EN_PASSANT;
	}
	inline bool is_promotion() const {
		return !(is_castling() || is_en_passant() || promotion_piece() == NO_PIECE);
	}
	inline bool operator ==(const Move move) const {
		return this->data == move.data;
	}
	inline bool operator !=(const Move move) const {
		return this->data != move.data;
	}
	inline bool operator <(const Move move) const {
		return this->data < move.data;
	}

	friend std::ostream & operator <<(std::ostream & os, const Move & pair);
};

const static Move move_white_OO(WHITE_OO, 0, MOVE_CASTLING);
const static Move move_white_OOO(WHITE_OOO, 0, MOVE_CASTLING);
const static Move move_black_OO(BLACK_OO, 0, MOVE_CASTLING);
const static Move move_black_OOO(BLACK_OOO, 0, MOVE_CASTLING);

// Move that has information essential for both making and unmaking.
// Can be joined together to make compound moves like castling or en passant.
class BitboardMove {
public:
	Piece_t end_piece, promotion_piece, start_piece;
	Coord_t start, end;
	BitboardMove() {
		this->start = NO_MOVE;
		this->end = NO_MOVE;
		this->end_piece = NO_PIECE;
		this->start_piece = NO_PIECE;
		this->promotion_piece = NO_PIECE;
	}
	// Use start to enter special move code, use end to add arguments
	BitboardMove(const Coord_t start, const Coord_t end, const Piece_t start_piece = NO_PIECE,
		const Piece_t end_piece = NO_PIECE, const Piece_t promotion_piece = NO_PIECE) {
		this->start = start;
		this->end = end;
		this->start_piece = start_piece;
		this->promotion_piece = promotion_piece;
		this->end_piece = end_piece;
	}
	inline bool is_null() const {
		return start == NO_MOVE || end == NO_MOVE;
	}
	friend std::ostream & operator <<(std::ostream & os, const BitboardMove & move);
};

class MoveOptionSet {
	std::vector<Move> regular_moves;
	Move promotion_moves[3], castling_moves[2], en_passant_move;
};

#endif