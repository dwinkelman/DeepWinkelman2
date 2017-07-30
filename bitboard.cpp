/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 16 April 2017
*
* Implementation for the bitboard and associated types
*/

#include "bitboard.h"

#include <random>

Hash_t BitboardData::zobrist_keys[13][64];
bool BitboardData::is_zobrist_inited = false;
void BitboardData::init_zobrist() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<Hash_t> dis(0, 0xffffffffffffffff);
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 64; j++) {
			zobrist_keys[i][j] = dis(gen);
		}
	}
}

MoveManager Bitboard::move_manager = MoveManager();

Bitboard::Bitboard(const Piece_t squares[64], const Color_t color, const Castling_t castling = 15) {
	// initialize squares
	for (int i = 0; i < 64; i++) {
		this->squares[i] = squares[i];
	}

	depth = 0;

	// initialize bitboards
	for (int i = 0; i < 64; i++) {
		if (squares[i] < BLACK_PAWN && squares[i] != NO_PIECE) {
			history[depth].white |= one << i;
			if (squares[i] == WHITE_PAWN) history[depth].wpawns |= one << i;
		}
		else if (squares[i] != NO_PIECE) {
			history[depth].black |= one << i;
			if (squares[i] == BLACK_PAWN) history[depth].bpawns |= one << i;
		}
		history[depth].pieces[squares[i]] |= one << i;
	}

	// initialize hash and piece score
	for (int i = 0; i < 64; i++) {
		history[depth].hash ^= BitboardData::zobrist_keys[squares[i]][i];
		history[depth].piece_score += sparams.PIECE_VALUES[squares[i]];
	}

	// initialize castling, en passant, color
	history[depth].ep = BitboardMove(NO_MOVE, NO_MOVE);
	history[depth].castling = WHITE_OO | WHITE_OOO | BLACK_OO | BLACK_OOO;
	history[depth].color = color;
}

Bitboard::Bitboard() : Bitboard(DEFAULT_POS, WHITE) {

}



std::vector<Move> Bitboard::get_moves() const {
	// capturing moves from MoveMananger
	static MoveList * sets_list[256];
	static MoveList * promotion_sets_list[8];
	static Coord_t square_list[256], promotion_square_list[8];
	static int n_sets, n_moves, n_promotion_sets, n_promotion_moves;
	n_sets = n_moves = n_promotion_sets = n_promotion_moves = 0;

	// gather sets of possible moves
	static Bitmask_t friendly, enemy;
	// white to move
	if (history[depth].color == WHITE) {
		friendly = history[depth].white;
		enemy = history[depth].black;

		static int i;
		for (int i = 0; i < 64; i++) {
			switch (squares[i]) {
			case WHITE_PAWN:
				// check for promotion
				if (i < 48) {
					square_list[n_sets] = i;
					sets_list[n_sets++] = &(move_manager.wp_moves.get_movelist(i, friendly, enemy));
				}
				else {
					promotion_square_list[n_promotion_sets] = i;
					promotion_sets_list[n_promotion_sets++] =
						&(move_manager.wp_moves.get_movelist(i, friendly, enemy));
				}
				break;
			case WHITE_KNIGHT:
				square_list[n_sets] = i;
				sets_list[n_sets++] = &(move_manager.n_moves.get_movelist(i, friendly, enemy));
				break;
			case WHITE_BISHOP:
				square_list[n_sets] = square_list[n_sets + 1] = i;
				sets_list[n_sets++] = &(move_manager.d1_moves.get_movelist(i, friendly, enemy));
				sets_list[n_sets++] = &(move_manager.d2_moves.get_movelist(i, friendly, enemy));
				break;
			case WHITE_ROOK:
				square_list[n_sets] = square_list[n_sets + 1] = i;
				sets_list[n_sets++] = &(move_manager.h_moves.get_movelist(i, friendly, enemy));
				sets_list[n_sets++] = &(move_manager.v_moves.get_movelist(i, friendly, enemy));
				break;
			case WHITE_QUEEN:
				square_list[n_sets] = square_list[n_sets + 1] =
					square_list[n_sets + 2] = square_list[n_sets + 3] = i;
				sets_list[n_sets++] = &(move_manager.h_moves.get_movelist(i, friendly, enemy));
				sets_list[n_sets++] = &(move_manager.v_moves.get_movelist(i, friendly, enemy));
				sets_list[n_sets++] = &(move_manager.d1_moves.get_movelist(i, friendly, enemy));
				sets_list[n_sets++] = &(move_manager.d2_moves.get_movelist(i, friendly, enemy));
				break;
			case WHITE_KING:
				square_list[n_sets] = i;
				sets_list[n_sets++] = &(move_manager.k_moves.get_movelist(i, friendly, enemy));
				break;
			}
		}
	}
	else {
		friendly = history[depth].black;
		enemy = history[depth].white;

		static int i;
		for (int i = 0; i < 64; i++) {
			switch (squares[i]) {
			case BLACK_PAWN:
				// check for promotion
				if (i >= 16) {
					square_list[n_sets] = i;
					sets_list[n_sets++] = &(move_manager.bp_moves.get_movelist(i, friendly, enemy));
				}
				else {
					promotion_square_list[n_promotion_sets] = i;
					promotion_sets_list[n_promotion_sets++] =
						&(move_manager.bp_moves.get_movelist(i, friendly, enemy));
				}
				break;
			case BLACK_KNIGHT:
				square_list[n_sets] = i;
				sets_list[n_sets++] = &(move_manager.n_moves.get_movelist(i, friendly, enemy));
				break;
			case BLACK_BISHOP:
				square_list[n_sets] = square_list[n_sets + 1] = i;
				sets_list[n_sets++] = &(move_manager.d1_moves.get_movelist(i, friendly, enemy));
				sets_list[n_sets++] = &(move_manager.d2_moves.get_movelist(i, friendly, enemy));
				break;
			case BLACK_ROOK:
				square_list[n_sets] = square_list[n_sets + 1] = i;
				sets_list[n_sets++] = &(move_manager.h_moves.get_movelist(i, friendly, enemy));
				sets_list[n_sets++] = &(move_manager.v_moves.get_movelist(i, friendly, enemy));
				break;
			case BLACK_QUEEN:
				square_list[n_sets] = square_list[n_sets + 1] =
					square_list[n_sets + 2] = square_list[n_sets + 3] = i;
				sets_list[n_sets++] = &(move_manager.h_moves.get_movelist(i, friendly, enemy));
				sets_list[n_sets++] = &(move_manager.v_moves.get_movelist(i, friendly, enemy));
				sets_list[n_sets++] = &(move_manager.d1_moves.get_movelist(i, friendly, enemy));
				sets_list[n_sets++] = &(move_manager.d2_moves.get_movelist(i, friendly, enemy));
				break;
			case BLACK_KING:
				square_list[n_sets] = i;
				sets_list[n_sets++] = &(move_manager.k_moves.get_movelist(i, friendly, enemy));
				break;
			}
		}
	}

	// count up number of moves and make an output vector
	static int i;
	for (i = 0; i < n_sets; i++) {
		n_moves += sets_list[i]->n_coords;
	}
	for (i = 0; i < n_promotion_sets; i++) {
		n_promotion_moves += promotion_sets_list[i]->n_coords;
	}
	std::vector<Move> output(n_moves + n_promotion_moves * 4);

	// write moves to output vector
	static int move_it, row, col;
	for (move_it = 0, row = 0; row < n_sets; row++) {
		for (col = 0; col < sets_list[row]->n_coords; col++) {
			output.at(move_it++) = Move(square_list[row], sets_list[row]->coords[col]);
		}
	}
	for (move_it = 0, row = 0; row < n_promotion_sets; row++) {
		for (col = 0; col < promotion_sets_list[row]->n_coords; col++) {
			output.at(move_it++) = Move(
				promotion_square_list[row],
				promotion_sets_list[row]->coords[col],
				WHITE_KNIGHT);
		}
	}

	// handle castling and en_passant moves
	static Castling_t castling;
	static Bitmask_t all_pieces;
	all_pieces = history[depth].white | history[depth].black;
	castling = history[depth].castling;
	if (history[depth].color == WHITE) {
		if (castling & WHITE_OO && !(all_pieces & WHITE_OO_MASK))
			output.push_back(move_white_OO);
		if (castling & WHITE_OOO && !(all_pieces & WHITE_OOO_MASK))
			output.push_back(move_white_OOO);
	}
	else {
		if (castling & BLACK_OO && !(all_pieces & BLACK_OO_MASK))
			output.push_back(move_black_OO);
		if (castling & BLACK_OOO && !(all_pieces & BLACK_OOO_MASK))
			output.push_back(move_black_OOO);
	}
	if (!history[depth].ep.is_null())
		output.push_back(Move(
			history[depth].ep.start, history[depth].ep.end, MOVE_EN_PASSANT
		));

	return output;
}