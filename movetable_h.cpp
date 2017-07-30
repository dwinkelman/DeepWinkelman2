/******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 12 April 2017
*
* Implementation for horizontal move table.
*/

#include "debug.h"
#include "movetable.h"

#include <bitset>
#include <string>

HMoveTable::HMoveTable() {
	generate_bitmasks();
	generate_moves();
	_tests();
}

Bitmask_t HMoveTable::combo_to_mask(const Coord_t coord, const Combo_t combo) const {
	return (Bitmask_t)combo << ((coord / 8) * 8);
}

Combo_t HMoveTable::mask_to_combo(const Coord_t coord, const Bitmask_t mask) const {
	return (mask >> ((coord / 8) * 8)) & 0xff;
}

void HMoveTable::generate_bitmasks() {
	for (int i = 0; i < 64; i++) {
		masks[i] = (Bitmask_t)0xff << ((i / 8) * 8);
	}
}

void HMoveTable::generate_moves() {
	for (int rank = 0; rank < 8; rank++) {
		for (int combo = 0; combo < 256; combo++) {
			moves[rank][combo] = MoveList(combo_to_mask(rank * 8, combo));
		}
	}
}

MoveList & HMoveTable::get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy) {
	static Combo_t e_combo, f_combo;
	e_combo = ct.e[coord % 8][mask_to_combo(coord, enemy)];
	f_combo = ct.f[coord % 8][mask_to_combo(coord, friendly)];

	return moves[coord / 8][e_combo & f_combo];
}

void HMoveTable::_tests() {
	// Run tests
	if (MASTER_DEBUG) {
		if (H_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MASKS) {
			std::cout << "TEST: HMoveTable Masks\n";
			for (int i = 0; i < 64; i++) {
				std::cout << i << '\n' << print_mask(masks[i]) << '\n';
			}
		}
		if (H_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_CT) {
			int file = 3;
			std::cout << "TEST: HMoveTable Collision Tables\n";
			std::cout << "Using file " << file << '\n';
			for (int i = 0; i < 256; i++) {
				std::cout << std::bitset<8>(i) << ":" <<
					" Friendly " << std::bitset<8>(ct.f[file][i]) <<
					" Enemy " << std::bitset<8>(ct.e[file][i]) << '\n';
			}
		}
		if (H_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_COMBO2MASK) {
			Coord_t square = 14;
			std::cout << "TEST: HMoveTable Combo to Mask and Mask to Combo\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 0; i < 256; i++) {
				std::cout << "Combo " << i << '\n' << print_mask(combo_to_mask(square, i));
				if (mask_to_combo(square, combo_to_mask(square, i)) == i) {
					std::cout << "Reverse test PASSED\n\n";
				}
				else {
					std::cout << "Reverse test FAILED\n\n";
				}
			}
		}
		if (H_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MOVES) {
			Coord_t square = 14;
			std::cout << "TEST: HMoveTable Moves\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 0; i < 256; i++) {
				std::cout << "Combo " << i << " Mask " << combo_to_mask(square, i)
					<< " " << moves[square / 8][i] << '\n';
			}
		}
	}
}