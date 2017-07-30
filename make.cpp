/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 21 April 2017
*
* Making and unmaking moves on a bitboard
*/

#include "bitboard.h"
#include "errors.h"

// Promotion piece must be the same as end piece if no promotion happens.
bool Bitboard::make(const Coord_t start, const Coord_t end, const Piece_t promotion_piece,
	const BitboardData & current, BitboardData & next) {
	static Piece_t start_piece, end_piece;
	start_piece = squares[start], end_piece = squares[end];

	// adjust board squares
	squares[start] = NO_PIECE;
	squares[end] = promotion_piece;

	// increment piece score
	next.piece_score = current.piece_score - sparams.PIECE_VALUES[end_piece];
	if (end_piece != promotion_piece)
		next.piece_score += sparams.PIECE_VALUES[promotion_piece]
		- sparams.PIECE_VALUES[start_piece];

	// update number of pieces on the board
	if (end_piece == NO_PIECE) next.n_pieces = current.n_pieces;
	else next.n_pieces = current.n_pieces - 1;

	// update the positions of the white and black kings
	if (start_piece == WHITE_KING) next.white_king = end;
	else next.white_king = current.white_king;
	if (start_piece == BLACK_KING) next.black_king = end;
	else next.black_king = current.black_king;

	// increment hash
	// remove start piece * add empty at start * remove end piece * add start piece at end
	next.hash = current.hash
		^ BitboardData::zobrist_keys[start_piece][start]
		^ BitboardData::zobrist_keys[NO_PIECE][start]
		^ BitboardData::zobrist_keys[end_piece][end]
		^ BitboardData::zobrist_keys[start_piece][end];

	// increment bitboards
	if (current.color == WHITE) {
		next.white = (current.white & ~(one << start)) | (one << end);
		next.black = current.black & ~(one << end);
		next.wpawns = next.white & current.wpawns;
		next.bpawns = next.black & current.bpawns;
		// add the pawn to the end square only if it did not promote
		if (start_piece == WHITE_PAWN && start_piece == promotion_piece)
			next.wpawns |= one << end;
	}
	else {
		next.white = current.white & ~(one << end);
		next.black = (current.black & ~(one << start)) | (one << end);
		next.wpawns = next.white & current.wpawns;
		next.bpawns = next.black & current.bpawns;
		// add the pawn to the end square only if it did not promote
		if (start_piece == BLACK_PAWN && start_piece == promotion_piece)
			next.bpawns |= one << end;
	}

	// increment piece bitboards
	

	// update castling
	next.castling = current.castling;
	if (start_piece == WHITE_KING) next.castling &= ~(WHITE_OO | WHITE_OOO);
	else if (start_piece == WHITE_ROOK) {
		if (squares[0] != WHITE_ROOK) next.castling &= ~WHITE_OOO;
		if (squares[7] != WHITE_ROOK) next.castling &= ~WHITE_OO;
	}
	else if (start_piece == BLACK_KING) next.castling &= ~(BLACK_OO | BLACK_OOO);
	else if (start_piece == BLACK_ROOK) {
		if (squares[56] != BLACK_ROOK)next.castling &= ~BLACK_OOO;
		if (squares[63] != BLACK_ROOK)next.castling &= ~BLACK_OO;
	}

	// update en passant
	next.ep = BitboardMove(NO_MOVE, NO_MOVE);
	// Need the pawn to move up two squares, have an enemy pawn on the left or right
	if (start_piece == WHITE_PAWN && end - start == 16) {
		if (start > 24 && start < 32 && squares[end - 1] == BLACK_PAWN) {
			next.ep = BitboardMove(end - 1, end);
		}
		else if (start > 23 && start < 31 && squares[end + 1] == BLACK_PAWN) {
			next.ep = BitboardMove(end + 1, end);
		}
	}
	else if (start_piece == BLACK_PAWN && start - end == 16) {
		if (start > 32 && start < 40 && squares[end - 1] == WHITE_PAWN) {
			next.ep = BitboardMove(end - 1, end);
		}
		else if (start > 31 && start < 39 && squares[end + 1] == WHITE_PAWN) {
			next.ep = BitboardMove(end + 1, end);
		}
	}

	return end_piece != NO_PIECE;
}

bool Bitboard::make_normal(const Coord_t start, const Coord_t end) {
	// write to history, then make the move
	history[depth].move1 = BitboardMove(start, end, squares[start], squares[end]);

	// make the move
	static bool capture;
	capture = make(start, end, squares[start], history[depth], history[depth + 1]);

	// increment color
	history[depth + 1].color = (history[depth].color == WHITE) ? BLACK : WHITE;

	// increment depth
	increment_depth();

	return capture;
}

// Does not double check move clearance or check status of each square
bool Bitboard::make_castling(Castling_t castling) {
	// determine coords for making the move
	static Coord_t k_start, k_end, r_start, r_end;
	switch (castling) {
	case WHITE_OO:
		k_start = 4, k_end = 6;
		r_start = 7, r_end = 5;
		break;
	case WHITE_OOO:
		k_start = 4, k_end = 2;
		r_start = 0, r_end = 3;
		break;
	case BLACK_OO:
		k_start = 60, k_end = 62;
		r_start = 63, r_end = 61;
		break;
	case BLACK_OOO:
		k_start = 60, k_end = 58;
		r_start = 56, r_end = 59;
		break;
	}

	// write to history
	history[depth].move1 = BitboardMove(k_start, k_end, squares[k_start], squares[k_end]);
	history[depth].move2 = BitboardMove(r_start, r_end, squares[r_start], squares[r_end]);

	// make the move
	make(k_start, k_end, squares[k_start], history[depth], temp);
	make(r_start, r_end, squares[r_start], temp, history[depth + 1]);

	// increment color
	history[depth + 1].color = (history[depth].color == WHITE) ? BLACK : WHITE;

	// increment depth
	increment_depth();

	return false;
}

bool Bitboard::make_ep(const Coord_t start, const Coord_t end) {
	if (history[depth].color == WHITE) {
		// write to history
		history[depth].move1 = BitboardMove(start, end - 8, squares[start], squares[end - 8]);
		history[depth].move2 = BitboardMove(end - 8, end, squares[end - 8], squares[end]);

		// make the move
		make(start, end - 8, squares[start], history[depth], temp);
		make(end - 8, end, squares[end - 8], temp, history[depth + 1]);
	}
	else {
		// write to history
		history[depth].move1 = BitboardMove(start, end + 8, squares[start], squares[end + 8]);
		history[depth].move2 = BitboardMove(end + 8, end, squares[end + 8], squares[end]);

		// make the move
		make(start, end + 8, squares[start], history[depth], temp);
		make(end + 8, end, squares[end + 8], temp, history[depth + 1]);
	}

	// increment color
	history[depth + 1].color = (history[depth].color == WHITE) ? BLACK : WHITE;

	// increment depth
	increment_depth();

	return true;
}

bool Bitboard::make_promotion(const Coord_t start, const Coord_t end, const Piece_t promotion_piece) {
	// write to history, then make the move
	history[depth].move1 = BitboardMove(start, end, squares[start], squares[end], promotion_piece);

	// make the move
	static bool capture;
	capture = make(start, end, squares[start], history[depth], history[depth + 1]);

	// increment color
	history[depth + 1].color = (history[depth].color == WHITE) ? BLACK : WHITE;

	// increment depth
	increment_depth();

	return capture;
}

bool Bitboard::make(const Move move) {
	static bool capture;
	if (move.is_normal()) {
		capture = make_normal(move.start(), move.end());
	}
	else if (move.is_castling()) {
		capture = make_castling(move.castling_type());
	}
	else if (move.is_en_passant()) {
		capture = make_ep(move.start(), move.end());
	}
	else if (move.is_promotion()) {
		capture = make_promotion(move.start(), move.end(), move.promotion_piece());
	}
	return capture;
}

void Bitboard::make(std::vector<Move>& moves)
{
	std::vector<Move>::iterator it, end;
	for (it = moves.begin(), end = moves.end(); it != end; ++it) {
		make(*it);
	}
}

void Bitboard::unmake(const BitboardMove & move) {
	squares[move.start] = move.start_piece;
	squares[move.end] = move.end_piece;
}

void Bitboard::unmake() {
	// go back in history one level
	depth--;
	if (depth < 0) depth = 0;

	// change the squares
	if (!history[depth].move2.is_null())
		unmake(history[depth].move2);
	unmake(history[depth].move1);
}

void Bitboard::increment_depth() {
	depth++;
	if (depth >= HISTORY_DEPTH)
		throw new DeepWinkelmanException(
			"Bitboard cannot move beyond maximum history depth."
		);
}