/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 9 May 2017
*
* Methods for probing the game tree
*/

#ifndef DEEP_WINKELMAN_SEARCH
#define DEEP_WINKELMAN_SEARCH

#include <algorithm>

#include "node.h"

class SearchQueue {
protected:
	friend class BST<Score_t, Node *>;

	BST<Score_t, Node *> queue;
	int node_limit, max_nodes;
	int n_nodes;
	Score_t current_min_score;

public:
	SearchQueue() {
		node_limit = 1024, max_nodes = 2048;
		n_nodes = 0;
	}

	SearchQueue(const int max_nodes) {
		this->node_limit = max_nodes;
		n_nodes = 0;
	}

	// insert the node if it is better than the min or
	// the node cap has not been met
	// returns whether the node was added to the queue
	bool enqueue(Node * node, Score_t score) {
		if (n_nodes < node_limit) {
			queue.insert(score, node);
			n_nodes++;
			if (score < current_min_score) current_min_score = score;
			return true;
		}
		else if (n_nodes < max_nodes) {
			if (score > current_min_score) {
				queue.insert(score, node);
				n_nodes++;
				return true;
			}
			return false;
		}
		else {
			return false;
		}
	}

	Node * next() {
		Node * best = queue.remove_max();
		n_nodes--;
		return best;
	}
};

class GameTree {
public:
	Node * root;
	Bitboard board;
	int counter = 0;
	
public:
	// Create a game tree from a bitboard
	GameTree(Bitboard board) {
		this->board = board;
		root = new Node();
		root->color = board.current_data().color;
		Node::ttable.insert(board.current_data().hash, root);
	}

	void uniform_tree(const int depth) {
		root->create_tree(board, depth,
			Node::TreeOptions::NO_TREE_OPTIONS,
			&Bitboard::move_rank,
			0, 0);
	}

	void uniform_tree_expanded_captures(const int depth) {
		root->create_tree(board, depth,
			Node::TreeOptions::FOLLOW_CAPTURES,
			&Bitboard::move_rank,
			0, 0);
	}

	void alpha_beta_tree(const int depth) {
		root->create_tree(board, depth,
			Node::TreeOptions::PRESORT_MOVES,
			&Bitboard::move_rank,
			SCORE_BLACK_WIN, 6000);
	}

	void queue_deeping(const int nodes) {
		Node::searched_nodes = 0;

		// create a uniform tree as a starting point
		uniform_tree(3);

		// create a queue for storing the best lines
		// the BST is used to create easy sorted insertion
		BST<Score_t, Node *> queue;
		
		while (Node::searched_nodes < nodes) {

		}
	}

	void print_tree(const unsigned int max_depth = 128, std::vector<std::string> line = {}) {
		std::vector<MoveNodePair *> best_line = root->best_line();
		std::cout << "Game Tree [Score " << root->score() << " " << best_line << "]\n";
		Node & current = *root;
		/*for (std::string s : line) {
			current = current.find_move(Move(s)).node.get_node();
		}*/
		current.print_tree(max_depth + 1, 1);
	}

	// PARAMETERS:
	// 1. tree_size: how deep to generate each sub-tree
	// 2. n_layers: how many layers to repeat the algorithm for
	// 3. branch: how many best options from each tree to expand upon
	void basic_probe(){

	}
};

#endif