/******************************************************************************
* Deep Winkelman
* 
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 12 April 2017
*
* Main entry point for the Deep Winkelman chess engine.
*
* This program can be compiled by Visual Studio or GCC
*
*
* CURRENT BOTTLENECKS:
*		- Bitboard::score_level_1()
*		- Bitboard::get_moves()
*		- std::vector<NodePointer>::push_back() in Node::populate
*/

#include "search.h"
#include "test.h"
#include "fen.h"

const char * kasparov_1 = "1rb2rk1/1pqn1p1p/2pN2p1/p1N2P2/Pn1QP3/1P5P/4B1P1/2R2RK1 w - - 1 27";

int main() {
	/*
	bitboard.make(Move(12, 28));	//e4
	bitboard.make(Move(52, 36));	//e5
	bitboard.make(Move(6, 21));		//Nf3
	bitboard.make(Move(57, 42));	//Nc6
	bitboard.make(Move(5, 33));		//Bb5
	bitboard.make(Move(48, 40));	//a6
	bitboard.make(Move(33, 24));	//Ba4
	bitboard.make(Move(62, 45));	//Nf6
	bitboard.make(move_white_OO);	//O-O
	bitboard.make(Move(51, 43));	//d6
	bitboard.make(Move(10, 18));	//c3
	*/

	/*
	GameTree n = GameTree(bitboard);
	n.root.populate(bitboard);
	bitboard.make(Move("e2-e4"));
	Node & e4_node = n.root.find_move(Move("e2-e4")).convert();
	e4_node.populate(bitboard);

	n.print_tree();
	*/

	Bitboard bitboard = parse_fen(kasparov_1);
	GameTree gt = GameTree(bitboard);
	std::cout << "Starting tree generation\n";

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	// probe the game tree by deepening using a queue
	gt.alpha_beta_tree(4);
	Node::ttable;

	end = std::chrono::system_clock::now();
	std::chrono::duration<double> dur = end - start;
	std::cout << "Test elapsed in " << dur.count() << " seconds\n";
	std::cout << "Searched " << Node::searched_nodes << " nodes\n";

	gt.print_tree(2, { "d6-c8" });

	char buf;
	std::cin >> buf;

	return 0;
}