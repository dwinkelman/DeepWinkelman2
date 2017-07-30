/******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 12 April 2017
*
* Implementation for collision table.
*/

#include "movetable.h"

CollisionTable::CollisionTable() {
	generate();
}

void CollisionTable::generate() {
	Combo_t f_mask, e_mask;
	int i;
	for (int square = 0; square < 8; square++) {
		for (int combo = 0; combo < 256; combo++) {
			f_mask = e_mask = 0;
			for (i = square + 1; i < 8; i++) {
				e_mask |= 1 << i;
				if ((combo >> i) & 1) break;
				else f_mask |= 1 << i;
			}
			for (i = square - 1; i >= 0; i--) {
				e_mask |= 1 << i;
				if ((combo >> i) & 1) break;
				else f_mask |= 1 << i;
			}
			e[square][combo] = e_mask;
			f[square][combo] = f_mask;
		}
	}
}