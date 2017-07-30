/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 1 May 2017
*
* General purpose functions.
*/

#ifndef DEEP_WINKELMAN_UTIL
#define DEEP_WINKELMAN_BITBOARD

#include <stdint.h>
#include <vector>

inline unsigned int popcount(uint64_t w) {
	w -= (w >> 1) & 0x5555555555555555ULL;
	w = (w & 0x3333333333333333ULL) + ((w >> 2) & 0x3333333333333333ULL);
	w = (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
	return unsigned int((w * 0x0101010101010101ULL) >> 56);
}

inline unsigned int popcount_max15(uint64_t w) {
	w -= (w >> 1) & 0x5555555555555555ULL;
	w = (w & 0x3333333333333333ULL) + ((w >> 2) & 0x3333333333333333ULL);
	return (unsigned int)((w * 0x1111111111111111ULL) >> 60);
}

inline uint64_t half_popcount(uint64_t x) {
	x -= ((x >> 1) & 0x5555555555555555);
	x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
	return x;
}

inline uint64_t diag_flip(uint64_t x){
	uint64_t t;
	const uint64_t k1 = 0x5500550055005500;
	const uint64_t k2 = 0x3333000033330000;
	const uint64_t k4 = 0x0f0f0f0f00000000;
	t	= k4 & (x ^ (x << 28));
	x	^=		t ^ (t >> 28);
	t	= k2 & (x ^ (x << 14));
	x	^=		t ^ (t >> 14);
	t	= k1 & (x ^ (x << 7));
	x	^=		t ^ (t >> 7);
	return x;
}

inline unsigned int maxbit(uint32_t n) {
	n |= (n >> 1);
	n |= (n >> 2);
	n |= (n >> 4);
	n |= (n >> 8);
	n |= (n >> 16);
	return n - (n >> 1);
}

/**
* Find the index of a piece of data in a sorted list.
* If the data is not present, -1 is returned.
*
* Requires that the type have == and < overloaded or defined.
* Requires that data be pre-sorted.
*
* Please see https://repl.it/IJHp/1 for testing/functionality
*/
template <typename T>
unsigned int bin_search(std::vector<T> & data, const T key) {
	// Determine the size of the data
	unsigned int size = data.size();

	// Get the middle of the data (a power of 2 minus 1 to ensure full coverage)
	unsigned int index = maxbit(size) - 1;

	// Step interval is half of the mid-point
	unsigned int step_interval = (index + 1) >> 1;

	// Do an initial check if the middle is the desired point
	if (key == data[index]) return index;

	// Wait until all search options have been exhausted
	while (step_interval > 0) {
		// See if the needed value is on the left or right of the current value
		// Adjust usng the step interval as necessary
		if (key < data[index])
			index -= step_interval;
		else
			index = std::min(index + step_interval, size - 1);

		// Check if the new data is a match
		if (key == data[index]) return index;

		// Reduce the interval by half
		step_interval >>= 1;
	}

	// If nothing already found, the item is not in the data
	return -1;
}

#endif