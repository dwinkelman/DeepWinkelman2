/******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 24 May 2017
*
* Transposition table implementation to avoid duplicate trees.
*
* Hybrid of hashmap and binary search tree.
*/

#ifndef DEEP_WINKELMAN_TRANSPOSITION
#define DEEP_WINKELMAN_TRANSPOSITION

#include "bst.h"

// Forward-declare hash and node instead of including
typedef uint64_t Hash_t;
class Node;

class TranspositionTable {
protected:
	const static int n_pools = 1024;
	const static Hash_t pool_mask = 1023;
	BST<Hash_t, Node *> bst[n_pools];

	int n_nodes;

public:
	TranspositionTable();

	void insert(const Hash_t hash, Node * node);
	Node * get(const Hash_t hash) const;
	void set(const Hash_t hash, Node * node);
	void remove(const Hash_t hash);
	bool exists(const Hash_t hash) const;
};

#endif