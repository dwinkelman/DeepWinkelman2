/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 28 April 2017
*
* Implementation of scoring methods
*/

#include "bitboard.h"
#include "util.h"

Score_t Bitboard::score_level_0() const {
	/**
	* Considers only piece score
	*/
	return history[depth].piece_score;// *history[depth].color;
}

Score_t Bitboard::score_level_1() const {
	/**
	* Considers:
	*	- piece score
	*	- advancement of pawns
	*	- connectivity of pawns
	*/
	return (score_material() + score_pawn_structure());// *history[depth].color;
}

Score_t Bitboard::score_material() const {
	return score_level_0();
}

Score_t Bitboard::score_pawn_structure() const {
	/**
	 * Advancement of pawns
	 * Connectivity of pawns
	 * Doubled pawns
	 * Blocked pawns
	 * Central control
	**/

	Score_t output = 0;
	const BitboardData & data = current_data();

	const Bitmask_t center_mask = 0x0000c3c3c3c30000;

	// Advancement
	Bitmask_t w_rows = half_popcount(data.wpawns);
	Bitmask_t b_rows = half_popcount(data.bpawns);
	output +=
		sparams.PAWN_RANK_2 * ((0xff & (w_rows >>  8)) - (0xff & (b_rows >> 48))),
		sparams.PAWN_RANK_3 * ((0xff & (w_rows >> 16)) - (0xff & (b_rows >> 40))),
		sparams.PAWN_RANK_4 * ((0xff & (w_rows >> 24)) - (0xff & (b_rows >> 32))),
		sparams.PAWN_RANK_5 * ((0xff & (w_rows >> 32)) - (0xff & (b_rows >> 24))),
		sparams.PAWN_RANK_6 * ((0xff & (w_rows >> 40)) - (0xff & (b_rows >> 16))),
		sparams.PAWN_RANK_7 * ((0xff & (w_rows >> 48)) - (0xff & (b_rows >> 8)));
	// Connectivity
	unsigned int w_defended_pawns = move_manager.wp_moves.pieces_attacked(data.wpawns, data.wpawns);
	unsigned int b_defended_pawns = move_manager.bp_moves.pieces_attacked(data.bpawns, data.bpawns);
	output += sparams.PAWN_DEFENDING_PAWN *
		(signed)(w_defended_pawns - b_defended_pawns);
	// Doubled Pawns
	unsigned int w_doubled_pawns = move_manager.wp_moves.doubled_pawns(data.wpawns);
	unsigned int b_doubled_pawns = move_manager.bp_moves.doubled_pawns(data.bpawns);
	output += sparams.PAWN_DOUBLED *
		(signed)(w_doubled_pawns - b_doubled_pawns);
	// Blocked Pawns
	unsigned int w_blocked_pawns = move_manager.wp_moves.blocked_pawns(
		data.wpawns, data.white | data.black);
	unsigned int b_blocked_pawns = move_manager.bp_moves.blocked_pawns(
		data.bpawns, data.white | data.black);
	output += sparams.PAWN_BLOCKED * 
		(signed)(w_blocked_pawns - b_blocked_pawns);
	// Central Control
	unsigned int w_center_squares = move_manager.wp_moves.square_control(data.wpawns, center_mask);
	unsigned int b_center_squares = move_manager.bp_moves.square_control(data.bpawns, center_mask);
	output += sparams.PAWN_CENTER_ATTACK *
		(signed)(w_center_squares - b_center_squares);

	return output;
}

Score_t Bitboard::score_piece_position() const {
	/**
	 * Centralization of pieces
	**/

	Score_t output = 0;
	const BitboardData & data = current_data();

	for (int i = 0; i < 64; i++) {
		unsigned int n_moves =
			((move_manager.*(move_manager.move_counters[squares[i]])))
			(i, data.white, data.black);
		sparams.PIECE_MOBILITY[squares[i]] * (signed)n_moves;
			
	}

	return output;
}

Move_Rank_t Bitboard::move_rank(const Move move) {
	make(move);
	Score_t s = score_level_1();
	unmake();
	return s * ((current_data().color == WHITE) ? 1 : -1);
}