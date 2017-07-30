/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 18 April 2017
*
* Nodes for the bitboard to store the game tree.
*
* "Nodes" are nodes that contain scoring and color information in addition to
* a list of move options NodePointers.
*
* "NodePointers" are nodes that contain only a score or a reference to another
* Node stored in a global allocation called a NodeHeap.
*
* "NodeHeaps" is a dynamically-allocated array of Nodes that can be accessed
* similarly to a vector. The purpose is to reduce overhead from allocation,
* support transposition tables in the future, contain references to nodes in
* a central memory location to avoid pointer inconsistency, constrain system
* memory allocation.
*
* A sample structure may look like:
*								Node
*								 |
*			NodePointer -- NodePointer -- NodePointer
*			Score: 0.5			 |		  Score: 0.7
*								Node
*								 |
*					NodePointer -- NodePointer
*					Score: 1.0			|
*									   Node
*										:
*										:
*/

#ifndef DEEP_WINKELMAN_NODE
#define DEEP_WINKELMAN_NODE

#include <map>
#include <array>

#include "bitboard.h"
#include "score.h"
#include "transposition.h"

typedef uint32_t NodeAddress_t;
class Node;
class NodeHeap;

// Modes for Node Pointer: toggle between pointing to a node or giving a score
#define NODE_POINTER_SCORE_MODE 0
#define NODE_POINTER_NODE_MODE 1

union NodePointerData_t {
	Node * node;
	Score_t score;
};

// Data structure that can function as both a score and a pointer
struct NodePointer {
protected:
	unsigned int mode : 1;
	unsigned int capture : 1;
	NodePointerData_t data;

public:
	NodePointer();
	NodePointer(const Score_t score, bool capture);
	NodePointer(Node * node);
	~NodePointer();
	void set_score(const Score_t score);
	void set_node(Node * node);
	Score_t get_score() const;
	Node & get_node() const;
	bool is_pointer() const;
	Node & convert(const Color_t color = WHITE,
		const Score_t alpha = SCORE_BLACK_WIN,
		const Score_t beta = SCORE_WHITE_WIN);
	void delete_node();

	friend std::ostream & operator <<(std::ostream & os, const NodePointer & np);
};

#define NODE_IS_ALLOCATED 1
#define NODE_NOT_ALLOCATED 0

struct MoveNodePair {
	Move move;
	NodePointer node;
	MoveNodePair(NodePointer node, Move move) {
		this->node = node;
		this->move = move;
	}
	friend std::ostream & operator <<(std::ostream & os, std::vector<MoveNodePair *> & moves);
	static bool first_greater_than_second(MoveNodePair & first, MoveNodePair & second) {
		return first.node.get_score() > second.node.get_score();
	}
};

struct MoveRank {
public:
	Move_Rank_t rank;
	MoveNodePair * pair;

	MoveRank() {}

	MoveRank(MoveNodePair * pair, Move_Rank_t rank) {
		this->rank = rank;
		this->pair = pair;
	}

	static bool first_greater_than_second(MoveRank & first, MoveRank & second) {
		return first.rank > second.rank;
	}
};

class Node {
protected:
	friend class GameTree;

	Score_t _score;
	// Score_t alpha, beta;
	Color_t color : 2;
	unsigned int allocated : 1;
	int n_parents : 13;
	std::vector<MoveNodePair> children;

public:
	static TranspositionTable ttable;

	static int searched_nodes;

public:
	static unsigned int counter;

	Node();
	Node(const Color_t color, const Score_t score);

	inline Score_t score() const {
		return _score;
	}
	inline bool is_null() const {
		return allocated == NODE_NOT_ALLOCATED;
	}
	inline void add_parent(NodePointer * ptr) {
		n_parents++;
	}
	inline bool remove_parent(NodePointer * ptr) {
		n_parents--;
		return n_parents <= 0;
	}

	// Find the node corresponding to a move
	// Throws an error if the move is not found
	MoveNodePair & find_move(const Move move);

	
	typedef enum TreeOptions {
		NO_TREE_OPTIONS = 0x00,
		// Extend the tree automatically when a capture is made
		FOLLOW_CAPTURES = 0x01,
		// Sort move options before expanding
		PRESORT_MOVES = 0x02
	};

	// Create NodePointers to all possible moves in the position
	// Assuming that the bitboard is already in position for the node
	// Have the option of choosing the method to determine the score of each node
	void populate(Bitboard & bitboard, Bitboard::ScoreFunction score_function);
	
	// Generate a uniform move tree starting from this node of depth
	// The depth includes a layer of NodePointers
	Score_t create_tree(
		Bitboard & board, int remaining,
		TreeOptions options, Bitboard::MoveRankFunction move_rank_function,
		Score_t alpha, Score_t beta);

	// Get the highest-scoring node that is a direct child
	MoveNodePair * best_node();
	// Get the highest-scoring line
	std::vector<MoveNodePair *> best_line();
	// Get the best n moves
	std::vector<MoveNodePair *> best_nodes(const int n);

	friend std::ostream & operator <<(std::ostream & os, const Node & node);

protected:
	Score_t recurse_create_tree(
		Move move, NodePointer & nptr,
		Bitboard & board, int remaining,
		TreeOptions options, Bitboard::MoveRankFunction move_rank_function,
		Score_t alpha, Score_t beta);

public:
	void print_tree(unsigned int max_depth, unsigned int depth);
	
};

#endif