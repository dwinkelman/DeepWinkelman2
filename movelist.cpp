/******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 12 April 2017
*
* Implementation of move lists and other move table types.
*/

#include "movetable.h"

#include <iostream>

std::string print_mask(const Bitmask_t mask) {
	std::string output = "+--------+\n";
	output.reserve(128);
	for (int rank = 7; rank >= 0; rank--) {
		output += '|';
		for (int file = 0; file < 8; file++) {
			if ((mask >> (rank * 8 + file)) & 1) output += '1';
			else output += '0';
		}
		output += '|';
		output += '\n';
	}
	output += "+--------+\n";
	return output;
}

std::ostream & operator <<(std::ostream & os, const MoveList & movelist) {
	os << "Movelist [" << (int)movelist.n_coords << " moves]";
	for (int i = 0; i < movelist.n_coords; i++) {
		os << " " << (char)('a' + movelist.coords[i] % 8) << (char)('1' + movelist.coords[i] / 8);
	}
	return os;
}

MoveList::MoveList() {
	n_coords = 0;
}

MoveList::MoveList(const Bitmask_t mask) {
	n_coords = 0;
	for (int i = 0; i < 64 && n_coords < 8; i++) {
		if ((mask >> i) & 1) {
			coords[n_coords++] = i;
		}
	}
}

void MoveList::add(const Coord_t coord) {
	if (n_coords < 8) {
		coords[n_coords++] = coord;
	}
}

Bitmask_t MoveList::to_bitmask() const {
	Bitmask_t output = 0;
	for (int i = 0; i < n_coords; i++) {
		output |= one << coords[i];
	}
	return output;
}