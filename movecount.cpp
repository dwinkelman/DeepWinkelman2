/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 22 May 2017
*
* Counting the moves a piece can make in a position.
*/

#include "movetable.h"
#include "util.h"

unsigned int MoveManager::wp_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return popcount_max15(
		(wp_moves.friendly_masks[coord] & white) |
		(wp_moves.enemy_masks[coord] & black)
	);
}

unsigned int MoveManager::bp_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return popcount_max15(
		(bp_moves.friendly_masks[coord] & black) |
		(bp_moves.enemy_masks[coord] & white)
	);
}

unsigned int MoveManager::wn_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return popcount_max15(
		n_moves.masks[coord] & ~white
	);
}

unsigned int MoveManager::bn_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return popcount_max15(
		n_moves.masks[coord] & ~black
	);
}

unsigned int MoveManager::wb_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return
		d1_moves.get_movelist(coord, white, black).n_coords +
		d2_moves.get_movelist(coord, white, black).n_coords;
}

unsigned int MoveManager::bb_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return
		d1_moves.get_movelist(coord, black, white).n_coords +
		d2_moves.get_movelist(coord, black, white).n_coords;
}

unsigned int MoveManager::wr_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return
		h_moves.get_movelist(coord, white, black).n_coords +
		v_moves.get_movelist(coord, white, black).n_coords;
}

unsigned int MoveManager::br_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return
		h_moves.get_movelist(coord, black, white).n_coords +
		v_moves.get_movelist(coord, black, white).n_coords;
}

unsigned int MoveManager::wq_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return wr_move_count(coord, white, black) + wb_move_count(coord, white, black);
}

unsigned int MoveManager::bq_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return br_move_count(coord, black, white) + bb_move_count(coord, black, white);
}

unsigned int MoveManager::wk_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return popcount_max15(
		k_moves.masks[coord] & ~white
	);
}

unsigned int MoveManager::bk_move_count(
	const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
	return popcount_max15(
		k_moves.masks[coord] & ~black
	);
}