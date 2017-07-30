/******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 13 April 2017
*
* Implementation for vertical move table.
*/

#include "debug.h"
#include "movetable.h"

#include <bitset>
#include <string>

VMoveTable::VMoveTable() {
	generate_bitmasks();
	generate_moves();
	_tests();
}

Bitmask_t VMoveTable::combo_to_mask(const Coord_t coord, const Combo_t combo) const {
	static Bitmask_t output, shifted_combo;
	output = 0;
	shifted_combo = (Bitmask_t)combo << (coord % 8);

	static int i;
	for (i = 0; i < 56; i += 7) {
		output |= shifted_combo << i;
	}
	return output & masks[coord];
}

Combo_t VMoveTable::mask_to_combo(const Coord_t coord, const Bitmask_t mask) const {
	static Combo_t output;
	output = 0;
	static Bitmask_t shifted_mask;
	shifted_mask = (mask & masks[coord]) >> (coord % 8);

	static int i;
	for (i = 0; i < 56; i += 7) {
		output |= 0xff & (shifted_mask >> i);
	}
	return output;
}

void VMoveTable::generate_bitmasks() {
	for (int i = 0; i < 64; i++) {
		masks[i] = 0x101010101010101 << (i % 8);
	}
}

void VMoveTable::generate_moves() {
	for (int file = 0; file < 8; file++) {
		for (int combo = 0; combo < 256; combo++) {
			moves[file][combo] = MoveList(combo_to_mask(file, combo));
		}
	}
}

MoveList & VMoveTable::get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy) {
	static Combo_t e_combo, f_combo;
	e_combo = ct.e[coord / 8][mask_to_combo(coord, enemy)];
	f_combo = ct.f[coord / 8][mask_to_combo(coord, friendly)];

	return moves[coord % 8][e_combo & f_combo];
}

void VMoveTable::_tests() {
	// Run tests
	if (MASTER_DEBUG) {
		if (V_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MASKS) {
			std::cout << "TEST: VMoveTable Masks\n";
			for (int i = 0; i < 64; i++) {
				std::cout << i << '\n' << print_mask(masks[i]) << '\n';
			}
		}
		if (V_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_CT) {
			int rank = 3;
			std::cout << "TEST: VMoveTable Collision Tables\n";
			std::cout << "Using rank " << rank << '\n';
			for (int i = 0; i < 256; i++) {
				std::cout << std::bitset<8>(i) << ":" <<
					" Friendly " << std::bitset<8>(ct.f[rank][i]) <<
					" Enemy " << std::bitset<8>(ct.e[rank][i]) << '\n';
			}
		}
		if (V_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_COMBO2MASK) {
			Coord_t square = 14;
			std::cout << "TEST: VMoveTable Combo to Mask and Mask to Combo\n";
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
		if (V_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MOVES) {
			Coord_t square = 14;
			std::cout << "TEST: VMoveTable Moves\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 0; i < 256; i++) {
				std::cout << "Combo " << i << " Mask " << combo_to_mask(square, i)
					<< " " << moves[square % 8][i] << '\n';
			}
		}
	}
}