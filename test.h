/******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 22 April 2017
*
* General-purpose tests
*/

#ifndef DEEP_WINKELMAN_TEST
#define DEEP_WINKELMAN_TEST

#include <iostream>
#include <chrono>
#include <ctime>

#include "search.h"

double time_test(void(*function)()) {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	function();
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> dur = end - start;
	std::cout << "Test elapsed in " << dur.count() << " seconds\n";
	return dur.count();
}

void test_basic_make_unmake() {
	Bitboard bitboard;
	std::cout << bitboard;
	bitboard.make(Move(1, 18));
	std::cout << bitboard;
	bitboard.unmake();
	std::cout << bitboard;
}

void _test_tree_gen_benchmark_function() {
	Bitboard board;
	GameTree tree(board);
	tree.uniform_tree(5);
	std::cout << tree.counter << " parent nodes generated ("
		<< tree.root->counter << " nodes total)\n";
}
void test_tree_gen_benchmark() {
	time_test(_test_tree_gen_benchmark_function);
}



#endif