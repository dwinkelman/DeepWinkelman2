/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 26 April 2017
*
* Implementation for node pointers
*/

#include "node.h"
#include "params.h"

// Default constructor
NodePointer::NodePointer() {
	mode = NODE_POINTER_SCORE_MODE;
	data.score = 0;
}

NodePointer::NodePointer(const Score_t score, bool capture = 0) {
	data.score = score;
	this->capture = capture;
	this->mode = NODE_POINTER_SCORE_MODE;
}

NodePointer::NodePointer(Node * node) {
	data.node = node;
	this->mode = NODE_POINTER_NODE_MODE;
}

NodePointer::~NodePointer() {
	// check if a node has been allocated
	if (mode == NODE_POINTER_NODE_MODE) delete data.node;
}

// Turn into a score node
void NodePointer::set_score(const Score_t score) {
	mode = NODE_POINTER_SCORE_MODE;
	data.score = score;
}

// Turn into a reference node
void NodePointer::set_node(Node * node) {
	mode = NODE_POINTER_NODE_MODE;
	data.node = node;
}

Score_t NodePointer::get_score() const {
	if (mode == NODE_POINTER_SCORE_MODE) return data.score;
	else return data.node->score();
}

Node & NodePointer::get_node() const {
	if (mode == NODE_POINTER_NODE_MODE) return *data.node;
	static Node null_node;
	return null_node;
}

// Returns whether the pointer is pointing to another node
bool NodePointer::is_pointer() const {
	return mode == NODE_POINTER_NODE_MODE;
}

// Converts a score node to a pointer
Node & NodePointer::convert(const Color_t color, const Score_t alpha, const Score_t beta) {
	Score_t old_score = data.score;
	data.node = new Node(color, old_score);
	mode = NODE_POINTER_NODE_MODE;
	// add a parent to the node
	data.node->add_parent(this);
	return *data.node;
}

// If the node is in pointer mode, delete the child node and convert to score mode
void NodePointer::delete_node() {
	if (mode == NODE_POINTER_NODE_MODE) {
		Score_t score = data.node->score();
		// Get permission to delete the node
		if(data.node->remove_parent(this)) delete data.node;
		data.score = score;
		mode = NODE_POINTER_SCORE_MODE;
	}
}