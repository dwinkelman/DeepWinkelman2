/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 21 April 2017
*
* Printing functions
*/

#include <string>

#include "bitboard.h"
#include "move.h"
#include "node.h"

// Print a bitboard storage move.
std::ostream & operator <<(std::ostream & os, const BitboardMove & move) {
	os << (char)('a' + move.start % 8) << (char)('1' + move.start / 8)
		<< '-' << (char)('a' + move.end % 8) << (char)('1' + move.end / 8);
	return os;
}

std::string print_move(const Move & move) {
	std::string output;
	const static char piece_chars[14] = ".PNBRQKpnbrqk";
	if (move.is_castling())
		switch (move.castling_type()) {
		case WHITE_OO:
			return "O-O";
		case WHITE_OOO:
			return "O-O-O";
		case BLACK_OO:
			return "O-O";
		case BLACK_OOO:
			return "O-O-O";
		}
	else {
		output += (char)('a' + move.start() % 8);
		output += (char)('1' + move.start() / 8);
		output += '-';
		output += (char)('a' + move.end() % 8);
		output += (char)('1' + move.end() / 8);
	}
	if (move.is_en_passant()) output += "e.p.";
	else if (move.is_promotion()) {
		output += "=";
		output += piece_chars[move.promotion_piece()];
	}
	return output;
}

// Print a move suitable for making.
std::ostream & operator <<(std::ostream & os, const Move & move) {
	os << print_move(move);
	return os;
}

// Print a list of makable moves.
std::ostream & operator <<(std::ostream & os, std::vector<Move> & move_set) {
	os << "Move Set [" << move_set.size() << " moves] [ ";
	std::vector<Move>::iterator it, end;
	for (it = move_set.begin(), end = move_set.end(); it != end; ++it) {
		os << print_move(*it) << ' ';
	}
	os << ']';
	return os;
}

// Print a bitboard.
std::ostream & operator <<(std::ostream & os, const Bitboard & bitboard) {
	// display the board and all important information
	const static char piece_chars[14] = ".PNBRQKpnbrqk";
	os << "+---+-----------------+\n";
	for (int rank = 7; rank >= 0; rank--) {
		os << "| " << (char)('1' + rank) << " |";
		for (int file = 0; file < 8; file++) {
			os << " " << piece_chars[bitboard[rank * 8 + file]];
		}
		os << " |\n";
	}
	os << "+---+-----------------+\n";
	os << "|   | a b c d e f g h |\n";
	os << "+---+-----------------+\n";
	/*os << "White Pieces:\n" << print_mask(bitboard.current_data().white);
	os << "Black Pieces:\n" << print_mask(bitboard.current_data().black);
	os << "White Pawns:\n" << print_mask(bitboard.current_data().wpawns);
	os << "Black Pawns:\n" << print_mask(bitboard.current_data().bpawns);*/
	os << ((bitboard.current_data().color == WHITE) ? "White" : "Black") << " to move\n";
	os << "Hash: " << std::hex << bitboard.current_data().hash << std::dec << '\n';
	os << "Score: " << bitboard.score_level_1() << '\n';
	os << "White Castling: ";
	if (bitboard.current_data().castling & WHITE_OO) os << "O-O ";
	else os << "--- ";
	if (bitboard.current_data().castling & WHITE_OOO) os << "O-O-O ";
	else os << "--- ";
	os << "Black Castling: ";
	if (bitboard.current_data().castling&BLACK_OO) os << "O-O ";
	else os << "--- ";
	if (bitboard.current_data().castling&BLACK_OOO) os << "O-O-O ";
	else os << "--- ";
	os << '\n';
	os << "En Passant: ";
	if (bitboard.current_data().ep.is_null()) os << "---";
	else os << bitboard.current_data().ep;
	os << '\n';

	return os;
}

// Print a node.
std::ostream & operator <<(std::ostream & os, const Node & node) {
	os << "Node [";
	os << node.n_parents << " parents ";
	os << (node.color == WHITE ? "White" : "Black") << " to move ";
	os << "Score: " << node.score();
	// os << " Alpha: " << ((node.alpha < -100000) ? -100000 : node.alpha);
	// os << " Beta: " << ((node.beta > 100000) ? 100000 : node.beta);
	os << "]";
	return os;
}

// Print a node pointer.
std::ostream & operator <<(std::ostream & os, const NodePointer & np) {
	os << "Node Pointer [";
	if (np.is_pointer()) {
		os << "Pointer to " << np.get_node() << "]";
	}
	else {
		os << "Score Eval " << np.get_score() << "]";
	}
	return os;
}

// Print a series of moves.
std::ostream & operator <<(std::ostream & os, std::vector<MoveNodePair *> & moves) {
	std::vector<MoveNodePair *>::iterator it, end;
	it = moves.begin(), end = moves.end();
	for (; it != end; ++it) {
		os << (*it)->move << " ";
	}
	return os;
}

// Print a tree of nodes recursively.
void Node::print_tree(unsigned int max_depth = 0xffffffff, unsigned int depth = 0) {
	for(MoveNodePair & pair : children){
		for (unsigned int i = 0; i < depth; i++) std::cout << '\t';
		std::cout << pair.move << " ";
		if (pair.node.is_pointer()) {
			std::cout << pair.node.get_node() << '\n';
			if(depth < max_depth - 1)
				pair.node.get_node().print_tree(depth + 1, max_depth);
		}
		else {
			std::cout << pair.node << '\n';
		}
	}
}