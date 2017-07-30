/******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 13 April 2017
*
* Implementation for a1-h8 diagonal move table.
*/

#include "debug.h"
#include "movetable.h"

#include <bitset>
#include <string>

D1MoveTable::D1MoveTable() {
	generate_bitmasks();
	generate_moves();
	_tests();
}

Bitmask_t D1MoveTable::combo_to_mask(const Coord_t coord, const Combo_t combo) const {
	static Bitmask_t output, shifted_combo;
	output = 0;

	static int rank, file;
	rank = coord / 8, file = coord % 8;

	// reduce combo by offset
	static Combo_t offset;
	offset = combo - move_offsets[(rank > file) ? rank - file : file - rank];

	// shift combo to starting position
	if (rank == file) shifted_combo = offset;
	else if (rank > file) shifted_combo = (Bitmask_t)offset << ((rank - file) * 8);
	else shifted_combo = (Bitmask_t)offset << (file - rank);

	// spread combo along each row
	static int i;
	for (i = 0; i < 64; i += 8) {
		output |= shifted_combo << i;
	}

	// output only the diagonal
	return output & masks[coord];
}

Combo_t D1MoveTable::mask_to_combo(const Coord_t coord, const Bitmask_t mask) const {
	static Combo_t output;
	static Bitmask_t shifted_mask;
	output = 0;

	static int rank, file;
	rank = coord / 8, file = coord % 8;

	// shift mask to starting position and limit to the main coordinate
	if (rank == file) shifted_mask = mask & masks[0];
	else if (rank > file) shifted_mask = (mask & masks[coord]) >> ((rank - file) * 8);
	else shifted_mask = (mask & masks[coord]) >> (file - rank);

	// shift to a combo
	static int i;
	for (i = 0; i < 64; i += 8) {
		output |= shifted_mask >> i;
	}
	
	// adjust for the indexing system
	return output + move_offsets[(rank > file) ? rank - file : file - rank];
}

void D1MoveTable::generate_bitmasks() {
	for (int i = 0; i < 64; i++) {
		Bitmask_t mask = 0;
		int rank = i / 8, file = i % 8;
		for (int x = rank, y = file; 0 <= x && x < 8 && 0 <= y && y < 8; x++, y++) {
			mask |= one << (x * 8 + y);
		}
		for (int x = rank, y = file; 0 <= x && x < 8 && 0 <= y && y < 8; x--, y--) {
			mask |= one << (x * 8 + y);
		}
		masks[i] = mask;
	}
}

void D1MoveTable::generate_moves() {
	// middle diagonal
	for (int combo = 0; combo < 256; combo++) {
		moves_middle[combo] = MoveList(combo_to_mask(0, combo));
	}
	// lower and upper halves
	for (int x = 1; x < 8; x++) {
		for (int combo = 0; combo < (1 << (8 - x)); combo++) {
			moves_low[move_offsets[x] + combo] =
				MoveList(combo_to_mask(x, move_offsets[x] + combo));
			moves_high[move_offsets[x] + combo] =
				MoveList(combo_to_mask(x * 8, move_offsets[x] + combo));
		}
	}
}

MoveList & D1MoveTable::get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy) {
	static Combo_t e_combo, f_combo;
	static Combo_t ef_mask, offset;
	static int rank, file;
	rank = coord / 8, file = coord % 8;
	ef_mask = move_offsets[abs(file - rank) + 1];
	offset = move_offsets[abs(file - rank)];

	if (rank == file) {
		e_combo = ct.e[rank][mask_to_combo(coord, enemy)];
		f_combo = ct.f[rank][mask_to_combo(coord, friendly)];
		return moves_middle[e_combo & f_combo];
	}
	else if (rank > file) {
		// upper half
		e_combo = ct.e[file][mask_to_combo(coord, enemy)] & ~ef_mask;
		f_combo = ct.f[file][mask_to_combo(coord, friendly)] & ~ef_mask;
		return moves_high[offset + (e_combo & f_combo)];
	}
	else {
		// lower half
		e_combo = ct.e[rank][mask_to_combo(coord, enemy)] & ~ef_mask;
		f_combo = ct.f[rank][mask_to_combo(coord, friendly)] & ~ef_mask;
		return moves_low[offset + (e_combo & f_combo)];
	}
}

void D1MoveTable::_tests() {
	// Run tests
	if (MASTER_DEBUG) {
		if (D1_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MASKS) {
			std::cout << "TEST: D1MoveTable Masks\n";
			for (int i = 0; i < 64; i++) {
				std::cout << i << '\n' << print_mask(masks[i]) << '\n';
			}
		}
		if (D1_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_CT) {
			int rank = 3;
			std::cout << "TEST: D1MoveTable Collision Tables\n";
			std::cout << "Using rank " << rank << '\n';
			for (int i = 0; i < 256; i++) {
				std::cout << std::bitset<8>(i) << ":" <<
					" Friendly " << std::bitset<8>(ct.f[rank][i]) <<
					" Enemy " << std::bitset<8>(ct.e[rank][i]) << '\n';
			}
		}
		if (D1_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_COMBO2MASK) {
			Coord_t square = 34;
			std::cout << "TEST: D1MoveTable Combo to Mask and Mask to Combo\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 128; i < 192; i++) {
				std::cout << "Combo " << i << '\n' << print_mask(combo_to_mask(square, i));
				if (mask_to_combo(square, combo_to_mask(square, i)) == i) {
					std::cout << "Reverse test PASSED\n\n";
				}
				else {
					std::cout << "Reverse test FAILED\n\n";
				}
			}
		}
		if (D1_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MOVES) {
			Coord_t square = 34;
			std::cout << "TEST: D1MoveTable Moves\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 128; i < 192; i++) {
				std::cout << "Combo " << i << " Mask " << combo_to_mask(square, i)
					<< " " << moves_high[i] << '\n';
			}
		}
	}
}