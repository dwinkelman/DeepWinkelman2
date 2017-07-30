/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 16 April 2017
*
* Implementation for knight move table.
*/

#include "debug.h"
#include "movetable.h"

#include <bitset>
#include <string>

NMoveTable::NMoveTable() {
	generate_bitmasks();
	generate_moves();
	_tests();
}

Bitmask_t NMoveTable::combo_to_mask(const Coord_t coord, const Combo_t combo) const {
	static Bitmask_t output;
	output = 0;

	static int i;
	for (i = 0; i < n_move_options[coord]; i++) {
		output |= (one & (combo >> i)) << move_option_coords[coord][i];
	}
	return output;
}

Combo_t NMoveTable::mask_to_combo(const Coord_t coord, const Bitmask_t mask) const {
	static Combo_t output;
	output = 0;

	static int i;
	for (i = 0; i < n_move_options[coord]; i++) {
		output |= ((mask >> move_option_coords[coord][i]) & 1) << i;
	}
	return output;
}

void NMoveTable::generate_bitmasks() {
	int coord_changes[8][2] =
	{ {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1} };
	for (int square = 0; square < 64; square++) {
		int rank = square / 8, file = square % 8;
		Bitmask_t mask = 0;
		for (int i = 0, n = 0; i < 8 && n < n_move_options[square]; i++) {
			int x = coord_changes[i][0] + rank, y = coord_changes[i][1] + file;
			if (0 <= x && x < 8 && 0 <= y && y < 8) {
				move_option_coords[square][n++] = x * 8 + y;
				mask |= one << (x * 8 + y);
			}
		}
		masks[square] = mask;
	}
}

void NMoveTable::generate_moves() {
	int offset = 0;
	for (int square = 0; square < 64; square++) {
		// increment offset
		moves[square] = data + offset;
		offset += 1 << n_move_options[square];

		// generate moves from combinations
		for (int combo = 0; combo < (1 << n_move_options[square]); combo++) {
			for (int i = 0; i < n_move_options[square]; i++) {
				if ((combo >> i) & 1)
					moves[square][combo].add(move_option_coords[square][i]);
			}
		}
	}
}

MoveList & NMoveTable::get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy) {
	return moves[coord][mask_to_combo(coord, ~friendly)];
}

void NMoveTable::_tests() {
	// Run tests
	if (MASTER_DEBUG) {
		if (N_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MASKS) {
			std::cout << "TEST: NMoveTable Masks\n";
			for (int i = 0; i < 64; i++) {
				std::cout << i << '\n' << print_mask(masks[i]) << '\n';
			}
		}
		if (N_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_COMBO2MASK) {
			Coord_t square = 27;
			std::cout << "TEST: NMoveTable Combo to Mask and Mask to Combo\n";
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
		if (N_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MOVES) {
			Coord_t square = 27;
			std::cout << "TEST: NMoveTable Moves\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 0; i < 256; i++) {
				std::cout << "Combo " << i << " Mask " << combo_to_mask(square, i)
					<< " " << moves[square][i] << '\n';
			}
		}
	}
}