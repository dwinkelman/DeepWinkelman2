/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 27 April 2017
*
* Implementation for parent nodes
*/

#include <algorithm>

#include "node.h"
#include "bitboard.h"
#include "util.h"
#include "errors.h"

unsigned int Node::counter = 0;

TranspositionTable Node::ttable = TranspositionTable();

int Node::searched_nodes = 0;

Node::Node() {
	color = WHITE;
	allocated = NODE_NOT_ALLOCATED;
	_score = 0;
	// alpha = SCORE_BLACK_WIN;
	// beta = SCORE_WHITE_WIN;
}

Node::Node(const Color_t color, const Score_t score) {
	this->color = color;
	// this->alpha = alpha;
	// this->beta = beta;
	this->_score = score;
	this->n_parents = 0;
}

void Node::populate(Bitboard & bitboard, Bitboard::ScoreFunction score_function){
	std::vector<Move> moves = bitboard.get_moves();
	this->children.reserve(moves.size());
	counter += moves.size();
	int color_multiplier = (color == WHITE) ? -1 : 1;

	for (Move move : moves) {
		bool capture = bitboard.make(move);
		// https://repl.it/IG5L/0 - See on Pointer-to-Member Functions

		children.push_back(MoveNodePair(
			NodePointer((bitboard.*score_function)() * color_multiplier, capture), move
		));

		bitboard.unmake();
	}

	searched_nodes += children.size();
}

MoveNodePair & Node::find_move(const Move move) {
	// Use binary search to locate moves since they are in order
	// Determine the size of the data
	int size = children.size();
	std::vector<MoveNodePair>::iterator last = children.end() - 1;

	// Get the middle of the data (a power of 2 minus 1 to ensure full coverage)
	std::vector<MoveNodePair>::iterator it = children.begin() + (maxbit(size) - 1);

	// Step interval is half of the mid-point
	int step_interval = (it - children.end() + 1) >> 1;

	// Do an initial check if the middle is the desired point
	if (it->move == move) return *it;

	// Wait until all search options have been exhausted
	while (step_interval > 0) {
		// See if the needed value is on the left or right of the current value
		// Adjust usng the step interval as necessary
		if (move < it->move)
			it -= step_interval;
		else
			it = std::min(it + step_interval, last);

		// Check if the new data is a match
		if (move == it->move) return *it;

		// Reduce the interval by half
		step_interval >>= 1;
	}

	// If nothing already found, the item is not in the data
	throw new DeepWinkelmanException("Move not found.");
}

Score_t Node::create_tree(Bitboard & board, int remaining,
	TreeOptions options, Bitboard::MoveRankFunction move_rank_function,
	Score_t alpha, Score_t beta) {
	// generate the list of available moves along with node pointers
	// (this includes preliminary scores)
	populate(board, &Bitboard::score_level_1);

	// alpha-beta pruning
	// since moves are sorted in order of goodness, bad moves at the end can be pruned
	// alpha and beta values are passed through arguments
	// they are not kept as records
	Score_t last_node_score = 0;

	// sort moves by ranking of potential before doing alpha-beta pruning
	if (options & PRESORT_MOVES && remaining > 2) {
		
		// load moves and ranks into a vector to be sorted
		std::vector<MoveRank> ranked(children.size());
		std::vector<MoveRank>::iterator rank_it = ranked.begin();
		for (MoveNodePair & pair : children) {
			*rank_it = MoveRank(&pair, (board.*move_rank_function)(pair.move));
			++rank_it;
		}
		std::sort(ranked.begin(), ranked.end(), &MoveRank::first_greater_than_second);

		// go through each move and do same as below
		
		for(MoveRank & rank : ranked){

			// perform this function on children
			last_node_score = -recurse_create_tree(rank.pair->move, rank.pair->node,
				board, remaining, options, move_rank_function, -alpha, -beta);

			// using fail hard negamax
			// https://chessprogramming.wikispaces.com/Alpha-Beta
			if (last_node_score >= beta) {
				_score = last_node_score;
				return last_node_score;
			}
			if (last_node_score > alpha) {
				alpha = last_node_score;
			}

			/*
			// set alpha/beta score of this node
			// alpha beta only applies when nodes are presorted

			_score = std::max(last_node_score, _score);
			if (color == WHITE) {
				alpha = std::max(-last_node_score, alpha);
				//_score = std::max(last_node_score, _score);
			}
			else {
				beta = std::min(last_node_score, beta);
				//_score = std::min(last_node_score, _score);
			}

			// check if alpha beta has diverged
			// if so, break from the loop
			if (beta < alpha)
				break;
			*/
		}
		_score = alpha;
	}
	else {
		// perform operations on each child node
		for (MoveNodePair & node : children){
			if (remaining > 1) {
				// perform this function on children
				last_node_score = -recurse_create_tree(node.move, node.node,
					board, remaining, options, move_rank_function, -alpha, -beta);
			}

			// set score of this node
			if (last_node_score > _score) _score = last_node_score;
			/*if (color == WHITE) {
				// alpha = std::max(node_it->get_score(), alpha);
				_score = std::max(-node_it->get_score(), _score);
			}
			else {
				// beta = std::min(node_it->get_score(), beta);
				_score = std::min(-node_it->get_score(), _score);
			}*/
		}
	}

	return score();
}

Score_t Node::recurse_create_tree(Move move, NodePointer & nptr,
	Bitboard & board, int remaining,
	TreeOptions options, Bitboard::MoveRankFunction move_rank_function,
	Score_t alpha, Score_t beta) {
	// make the move to the bitboard
	bool capture = board.make(move);

	// check prior existance in transposition table
	Node * child = ttable.get(board.current_data().hash);
	if (child) {
		child->add_parent(&nptr);
		nptr.convert(child->color == WHITE ? BLACK : WHITE);
	}
	else {
		// convert node pointer from score to node mode
		nptr.convert(color == WHITE ? BLACK : WHITE);
		// add node to the transposition table
		ttable.insert(board.current_data().hash, &nptr.get_node());
		// execute this function on the child
		nptr.get_node().create_tree(board, remaining - 1, options, move_rank_function, alpha, beta);
	}
	// step back the bitboard
	board.unmake();

	return nptr.get_score();
}

MoveNodePair * Node::best_node() {
	return &(*std::max_element(children.begin(), children.end(),
		&MoveNodePair::first_greater_than_second));
}

std::vector<MoveNodePair *> Node::best_line() {
	std::vector<MoveNodePair *> output;
	MoveNodePair * current = best_node();
	while (current->node.get_node().children.size() > 0) {
		output.push_back(current);
		current = current->node.get_node().best_node();
	}
	return output;
}

//std::vector<MoveNodePair *> Node::best_nodes(const int n) {
//	// make a copy to do an in-place sort
//	std::vector<MoveNodePair> copy(children);
//	
//	std::partial_sort(copy.begin(), copy.begin() + n, copy.end(),
//		&MoveNodePair::first_greater_than_second);
//
//	// return the slice needed
//	return std::vector<MoveNodePair *>(copy.begin(), copy.begin() + n);
//}