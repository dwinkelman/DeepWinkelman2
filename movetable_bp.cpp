/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 25 April 2017
*
* Implementation for black pawn move table
*/

#include <bitset>
#include <string>

#include "debug.h"
#include "movetable.h"
#include "util.h"

BPMoveTable::BPMoveTable() {
	generate_bitmasks();
	generate_moves();
	_tests();
}

Bitmask_t BPMoveTable::combo_to_mask(const Coord_t coord, const Combo_t combo) const {
	return (((Bitmask_t)(combo & 7) << (coord - 9))
		| ((Bitmask_t)(combo & 8) << (coord - 19)))
		& masks[coord];
}

Combo_t BPMoveTable::mask_to_combo(const Coord_t coord, const Bitmask_t mask) const {
	static Bitmask_t adjusted;
	adjusted = mask & masks[coord];
	return ((adjusted >> (coord - 9)) & 7) | ((adjusted >> (coord - 19)) & 8);
}

void BPMoveTable::generate_bitmasks() {
	for (int i = 0; i < 8; i++) {
		friendly_masks[i] = enemy_masks[i] = 0;
	}
	// hard code a2 since negative shift does not work
	friendly_masks[8] = one;
	enemy_masks[8] = 2;
	for (int i = 9; i < 48; i++) {
		friendly_masks[i] = one << (i - 8);
		enemy_masks[i] = ((Bitmask_t)5 << (i - 9)) & ((Bitmask_t)0xff << ((i - 8) / 8 * 8));
	}
	for (int i = 48; i < 56; i++) {
		friendly_masks[i] = (Bitmask_t)0x101 << (i - 16);
		enemy_masks[i] = ((Bitmask_t)5 << (i - 9)) & 0xff0000000000;
	}
	for (int i = 56; i < 64; i++) {
		friendly_masks[i] = enemy_masks[i] = 0;
	}
	for (int i = 0; i < 64; i++) {
		masks[i] = friendly_masks[i] | enemy_masks[i];
	}
}

void BPMoveTable::generate_moves() {
	for (int i = 48; i < 56; i++) {
		for (int combo = 0; combo < 16; combo++) {
			rank_2_moves[i - 48][combo] = MoveList(combo_to_mask(i, combo));
		}
	}
	for (int i = 8; i < 48; i++) {
		for (int combo = 0; combo < 8; combo++) {
			reg_moves[i - 8][combo] = MoveList(combo_to_mask(i, combo));
		}
	}
}

MoveList & BPMoveTable::get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy) {
	if (48 <= coord && coord < 56) {
		Combo_t combo = mask_to_combo(coord,
			(friendly_masks[coord] & ~friendly) | (enemy_masks[coord] & enemy));
		return rank_2_moves[coord - 48][mask_to_combo(coord,
			(friendly_masks[coord] & ~friendly) | (enemy_masks[coord] & enemy))];

	}
	else if (8 <= coord && coord < 48) {
		return reg_moves[coord - 8][
			mask_to_combo(coord,
				(friendly_masks[coord] & ~(friendly | enemy)) | (enemy_masks[coord] & enemy))];
	}
	else {
		return NULL_MOVELIST;
	}
}

// Get the number of pieces attacked by pawns
unsigned int BPMoveTable::pieces_attacked(const Bitmask_t pawns, const Bitmask_t pieces) const {
	Bitmask_t pairs1 = (pawns >> 9) & pieces & 0x7f7f7f7f7f7f7f7f;
	Bitmask_t pairs2 = (pawns >> 7) & pieces & 0xfefefefefefefefe;
	return popcount(pairs1 | pairs2);
}
// Get the number of pawns that cannot move forward
unsigned int BPMoveTable::blocked_pawns(const Bitmask_t pawns, const Bitmask_t pieces) const {
	return popcount(pawns & (pieces << 8));
}
// Get the number of squares that pawns control in a region
unsigned int BPMoveTable::square_control(const Bitmask_t pawns, const Bitmask_t region) const {
	Bitmask_t attacks_left = (pawns >> 9) & 0x7f7f7f7f7f7f7f7f;
	Bitmask_t attacks_right = (pawns >> 7) & 0xfefefefefefefefe;
	return popcount(region & (attacks_left | attacks_right));
}
// Get the number of pawns that are doubled
unsigned int BPMoveTable::doubled_pawns(const Bitmask_t pawns) const {
	// rotate pawns from cols into rows
	Bitmask_t x = diag_flip(pawns);

	// popcount of each row
	x = half_popcount(x);

	// remove all rows with only one pawn
	x = x & 0x0e0e0e0e0e0e0e0e | ((x >> 1) & x) | ((x >> 2) & x);

	// finish popcount
	x += x >> 8;
	x += x >> 16;
	x += x >> 32;
	return x & 0x7f;
}
// Get the number of pawns in a rank
unsigned int BPMoveTable::pawns_in_rank(const Bitmask_t pawns, const int rank) const {
	return popcount_max15(pawns & 0xff << (rank << 3));
}
// Get the number of pawns in a file
unsigned int BPMoveTable::pawns_in_file(const Bitmask_t pawns, const int file) const {
	return popcount_max15(pawns & (0x0101010101010101 << file));
}

void BPMoveTable::_tests() {
	// Run tests
	if (MASTER_DEBUG) {
		if (BP_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MASKS) {
			std::cout << "TEST: BPMoveTable Masks\n";
			for (int i = 0; i < 64; i++) {
				std::cout << i << '\n' << print_mask(masks[i]) << '\n';
			}
		}
		if (BP_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_CT) {
			std::cout << "TEST: BPMoveTable Collision Tables\n";
			for (int i = 0; i < 64; i++) {
				std::cout << "Enemy " << i << '\n' << print_mask(enemy_masks[i]) << '\n';
				std::cout << "Friendly " << i << '\n' << print_mask(friendly_masks[i]) << '\n';
			}
		}
		if (BP_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_COMBO2MASK) {
			Coord_t square = 34;
			std::cout << "TEST: BPMoveTable Combo to Mask and Mask to Combo\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 0; i < 8; i++) {
				std::cout << "Combo " << i << '\n' << print_mask(combo_to_mask(square, i));
				if (mask_to_combo(square, combo_to_mask(square, i)) == i) {
					std::cout << "Reverse test PASSED\n\n";
				}
				else {
					std::cout << "Reverse test FAILED\n\n";
				}
			}
			square = 52;
			std::cout << "TEST: BPMoveTable Combo to Mask and Mask to Combo\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 0; i < 16; i++) {
				std::cout << "Combo " << i << '\n' << print_mask(combo_to_mask(square, i));
				if (mask_to_combo(square, combo_to_mask(square, i)) == i) {
					std::cout << "Reverse test PASSED\n\n";
				}
				else {
					std::cout << "Reverse test FAILED\n\n";
				}
			}
		}
		if (BP_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MOVES) {
			Coord_t square = 52;
			std::cout << "TEST: BPMoveTable Moves\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 0; i < 16; i++) {
				std::cout << "Combo " << i << " Mask " << combo_to_mask(square, i)
					<< " " << rank_2_moves[square - 48][i] << '\n';
			}
		}
	}
}