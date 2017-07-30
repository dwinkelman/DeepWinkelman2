/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 23 April 2017
*
* Implementation for white pawn move table
*/

#include <bitset>
#include <string>

#include "debug.h"
#include "movetable.h"
#include "util.h"

WPMoveTable::WPMoveTable() {
	generate_bitmasks();
	generate_moves();
	_tests();
}

Bitmask_t WPMoveTable::combo_to_mask(const Coord_t coord, const Combo_t combo) const {
	return (((Bitmask_t)(combo & 7) << (coord + 7))
		| ((Bitmask_t)(combo & 8) << (coord + 13)))
		& masks[coord];
}

Combo_t WPMoveTable::mask_to_combo(const Coord_t coord, const Bitmask_t mask) const {
	static Bitmask_t adjusted;
	adjusted = mask & masks[coord];
	return ((adjusted >> (coord + 7)) & 7) | ((adjusted >> (coord + 13)) & 8);
}

void WPMoveTable::generate_bitmasks() {
	for (int i = 0; i < 8; i++) {
		friendly_masks[i] = enemy_masks[i] = protection_masks[i] = 0;
	}
	for (int i = 8; i < 16; i++) {
		friendly_masks[i] = (Bitmask_t)0x10100 << i;
		enemy_masks[i] = ((Bitmask_t)5 << (i + 7)) & 0xff0000;
		protection_masks[i] = enemy_masks[i] | (one << i);
	}
	for (int i = 16; i < 56; i++) {
		friendly_masks[i] = one << (i + 8);
		enemy_masks[i] = ((Bitmask_t)5 << (i + 7)) & ((Bitmask_t)0xff00 << ((i / 8) * 8));
		protection_masks[i] = enemy_masks[i] | (one << i);
	}
	for (int i = 56; i < 64; i++) {
		friendly_masks[i] = enemy_masks[i] = protection_masks[i] = 0;
	}
	for (int i = 0; i < 64; i++) {
		masks[i] = friendly_masks[i] | enemy_masks[i];
	}
}

void WPMoveTable::generate_moves() {
	for (int i = 8; i < 16; i++) {
		for (int combo = 0; combo < 16; combo++) {
			rank_2_moves[i - 8][combo] = MoveList(combo_to_mask(i, combo));
		}
	}
	for (int i = 16; i < 56; i++) {
		for (int combo = 0; combo < 8; combo++) {
			reg_moves[i - 16][combo] = MoveList(combo_to_mask(i, combo));
		}
	}
}

MoveList & WPMoveTable::get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy) {
	if (8 <= coord && coord < 16) {
		Combo_t combo = mask_to_combo(coord,
			(friendly_masks[coord] & ~friendly) | (enemy_masks[coord] & enemy));
		return rank_2_moves[coord - 8][mask_to_combo(coord,
			(friendly_masks[coord] & ~friendly) | (enemy_masks[coord] & enemy))];
			
	}
	else if (16 <= coord && coord < 56) {
		return reg_moves[coord - 16][
			mask_to_combo(coord,
				(friendly_masks[coord] & ~(friendly | enemy)) | (enemy_masks[coord] & enemy))];
	}
	else {
		return NULL_MOVELIST;
	}
}

// Get the number of pieces attacked by pawns
unsigned int WPMoveTable::pieces_attacked(const Bitmask_t pawns, const Bitmask_t pieces) const {
	Bitmask_t pairs1 = (pawns << 7) & pieces & 0x7f7f7f7f7f7f7f7f;
	Bitmask_t pairs2 = (pawns << 9) & pieces & 0xfefefefefefefefe;
	return popcount(pairs1 | pairs2);
}
// Get the number of pawns that cannot move forward
unsigned int WPMoveTable::blocked_pawns(const Bitmask_t pawns, const Bitmask_t pieces) const {
	return popcount(pawns & (pieces >> 8));
}
// Get the number of squares that pawns control in a region
unsigned int WPMoveTable::square_control(const Bitmask_t pawns, const Bitmask_t region) const {
	Bitmask_t attacks_left =	(pawns << 7) & 0x7f7f7f7f7f7f7f7f;
	Bitmask_t attacks_right =	(pawns << 9) & 0xfefefefefefefefe;
	return popcount(region & (attacks_left | attacks_right));
}
// Get the number of pawns that are doubled
unsigned int WPMoveTable::doubled_pawns(const Bitmask_t pawns) const {
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
unsigned int WPMoveTable::pawns_in_rank(const Bitmask_t pawns, const int rank) const {
	return popcount_max15(pawns & 0xff << (rank << 3));
}
// Get the number of pawns in a file
unsigned int WPMoveTable::pawns_in_file(const Bitmask_t pawns, const int file) const {
	return popcount_max15(pawns & (0x0101010101010101 << file));
}

void WPMoveTable::_tests() {
	// Run tests
	if (MASTER_DEBUG) {
		if (WP_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MASKS) {
			std::cout << "TEST: WPMoveTable Masks\n";
			for (int i = 0; i < 64; i++) {
				std::cout << i << '\n' << print_mask(masks[i]) << '\n';
			}
		}
		if (WP_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_CT) {
			int file = 3;
			std::cout << "TEST: WPMoveTable Collision Tables\n";
			std::cout << "Using file " << file << '\n';
			for (int i = 0; i < 64; i++) {
				std::cout << "Enemy " << i << '\n' << print_mask(enemy_masks[i]) << '\n';
				std::cout << "Friendly " << i << '\n' << print_mask(friendly_masks[i]) << '\n';
			}
		}
		if (WP_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_COMBO2MASK) {
			Coord_t square = 14;
			std::cout << "TEST: WPMoveTable Combo to Mask and Mask to Combo\n";
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
			square = 34;
			std::cout << "TEST: WPMoveTable Combo to Mask and Mask to Combo\n";
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
		if (WP_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MOVES) {
			Coord_t square = 14;
			std::cout << "TEST: WPMoveTable Moves\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 0; i < 16; i++) {
				std::cout << "Combo " << i << " Mask " << combo_to_mask(square, i)
					<< " " << rank_2_moves[square - 8][i] << '\n';
			}
		}
	}
}