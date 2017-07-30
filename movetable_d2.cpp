/******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 14 April 2017
*
* Implementation for h1-a8 diagonal move table.
*/

#include "debug.h"
#include "movetable.h"

#include <bitset>
#include <string>

D2MoveTable::D2MoveTable() {
	generate_bitmasks();
	generate_moves();
	_tests();
}

Bitmask_t D2MoveTable::combo_to_mask(const Coord_t coord, const Combo_t combo) const {
	static Bitmask_t output, shifted_combo;
	output = 0;

	static int rank, file;
	rank = coord / 8, file = coord % 8;

	// reduce combo by offset
	static Combo_t offset;
	offset = move_offsets[(file + rank < 7) ? 7 - rank - file : file + rank - 7];

	// shift combo to starting position
	if (rank + file <= 7) shifted_combo = combo;
	else shifted_combo = combo << (file + rank - 7);

	// spread combo along each row
	for (int i = 0; i < 64; i += 8) {
		output |= shifted_combo << i;
	}

	// output only the diagonal
	return output & masks[coord];
}

Combo_t D2MoveTable::mask_to_combo(const Coord_t coord, const Bitmask_t mask) const {
	static Combo_t output;
	static Bitmask_t shifted_mask;
	output = 0;

	static int rank, file;
	rank = coord / 8; file = coord % 8;

	// shift combo to starting position
	if (rank + file == 7) shifted_mask = mask;
	else if (rank + file < 7) shifted_mask = mask << ((7 - rank - file) * 8);
	else shifted_mask = mask >> (file + rank - 7);
	shifted_mask &= masks[7];

	// spread combo along each row
	static int i;
	for (i = 0; i < 8; i++) {
		output |= ((shifted_mask >> ((7 - i) * 8 + i)) & 1) << i;
	}

	// adjust for the indexing system
	int index = (file + rank < 7) ? 7 - file - rank : file + rank - 7;
	return output + move_offsets[index];
}

void D2MoveTable::generate_bitmasks() {
	for (int i = 0; i < 64; i++) {
		Bitmask_t mask = 0;
		int rank = i / 8, file = i % 8;
		for (int x = rank, y = file; 0 <= x && x < 8 && 0 <= y && y < 8; x++, y--) {
			mask |= one << (x * 8 + y);
		}
		for (int x = rank, y = file; 0 <= x && x < 8 && 0 <= y && y < 8; x--, y++) {
			mask |= one << (x * 8 + y);
		}
		masks[i] = mask;
	}
}

void D2MoveTable::generate_moves() {
	// middle diagonal
	for (int combo = 0; combo < 256; combo++) {
		moves_middle[combo] = MoveList(combo_to_mask(0, combo));
	}

	// lower and upper halves
	for (int x = 1; x < 8; x++) {
		for (int combo = 0; combo < (1 << (8 - x)); combo++) {
			moves_low[move_offsets[x] + combo] =
				MoveList(combo_to_mask(7 - x, move_offsets[x] + combo));
			moves_high[move_offsets[x] + combo] =
				MoveList(combo_to_mask(56 + x, move_offsets[x] + combo));
		}
	}
}

MoveList & D2MoveTable::get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy) {
	static Combo_t e_combo, f_combo;
	static Combo_t ef_mask, offset;
	static int rank, file, index;
	rank = coord / 8, file = coord % 8;
	index = (file + rank < 7) ? 7 - file - rank : file + rank - 7;
	ef_mask = move_offsets[index + 1];
	offset = move_offsets[index];

	if (rank + file == 7) {
		e_combo = ct.e[rank][mask_to_combo(coord, enemy)];
		f_combo = ct.f[rank][mask_to_combo(coord, friendly)];
		return moves_middle[e_combo & f_combo];
	}
	else if (rank + file > 7) {
		// upper half
		e_combo = ct.e[7 - rank][mask_to_combo(coord, enemy)] & ~ef_mask;
		f_combo = ct.f[7 - rank][mask_to_combo(coord, friendly)] & ~ef_mask;
		return moves_high[offset + (e_combo & f_combo)];
	}
	else {
		// lower half
		e_combo = ct.e[file][mask_to_combo(coord, enemy)] & ~ef_mask;
		f_combo = ct.f[file][mask_to_combo(coord, friendly)] & ~ef_mask;
		return moves_low[offset + (e_combo & f_combo)];
	}
}

void D2MoveTable::_tests() {
	// Run tests
	if (MASTER_DEBUG) {
		if (D2_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MASKS) {
			std::cout << "TEST: D2MoveTable Masks\n";
			for (int i = 0; i < 64; i++) {
				std::cout << i << '\n' << print_mask(masks[i]) << '\n';
			}
		}
		if (D2_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_CT) {
			int rank = 3;
			std::cout << "TEST: D1MoveTable Collision Tables\n";
			std::cout << "Using rank " << rank << '\n';
			for (int i = 0; i < 256; i++) {
				std::cout << std::bitset<8>(i) << ":" <<
					" Friendly " << std::bitset<8>(ct.f[rank][i]) <<
					" Enemy " << std::bitset<8>(ct.e[rank][i]) << '\n';
			}
		}
		if (D2_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_COMBO2MASK) {
			Coord_t square = 12;
			std::cout << "TEST: D2MoveTable Combo to Mask and Mask to Combo\n";
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
		if (D2_MOVE_TABLE_DEBUG & MOVE_TABLE_CHECK_MOVES) {
			Coord_t square = 12;
			std::cout << "TEST: D2MoveTable Moves\n";
			std::cout << "Using square " << (int)square << '\n';
			for (int i = 128; i < 192; i++) {
				std::cout << "Combo " << i << " Mask " << combo_to_mask(square, i)
					<< " " << moves_low[i] << '\n';
			}
		}
	}
}