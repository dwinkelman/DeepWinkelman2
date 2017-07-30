/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 26 April 2017
*
* Implementation for node heap
* 
* Scrapped for now since it is too complicated to implement.
*/

#include "node.h"
#include "errors.h"

/*
NodeHeap::NodeHeap(const unsigned int size) {
	allocate(size);
}

NodeHeap::~NodeHeap() {
	// Delete pointers to arrays
	for (int i = 0; i < clusters.size(); i++) {
		delete clusters.at(i);
	}
}

NodeAddress_t NodeHeap::allocate() {
	for (int i = 0; i < n_nodes; allocation_crawler++, i++) {
		if (this->operator[](allocation_crawler % n_nodes).is_null()) {
			// return the index of the node and mark as allocated
			this->operator[](allocation_crawler % n_nodes).allocated = NODE_IS_ALLOCATED;
			return allocation_crawler % n_nodes;
		}
	}
	throw new DeepWinkelmanException(
		"Out of memory in Node Heap: no nodes available for allocation."
	);
}

void NodeHeap::allocate(const unsigned int size) {
	for (int i = 0; i < size && n_nodes < NODE_HEAP_MAX_CAPACITY; i += NODE_CLUSTER_SIZE) {
		clusters.push_back(new std::array<Node, NODE_CLUSTER_SIZE>);
		n_nodes += NODE_CLUSTER_SIZE;
	}
}
*/