/******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 24 May 2017
*
* Transposition table method implementation.
* Basically wrappers around BST methods.
*/

#include "transposition.h"

TranspositionTable::TranspositionTable() {
	for (int i = 0; i < n_pools; i++) {
		bst[i] = BST<Hash_t, Node *>(0x8000000000000000);
	}
	n_nodes = 0;
}

void TranspositionTable::insert(const Hash_t hash, Node * node) {
	n_nodes++;
	bst[hash & pool_mask].insert(hash, node);
}

Node * TranspositionTable::get(const Hash_t hash) const {
	static bool exists;
	Node * node = bst[hash & pool_mask].get_if_exists(hash, &exists);
	return node;
}

void TranspositionTable::set(const Hash_t hash, Node * node) {
	bst[hash & pool_mask].set(hash, node);
}

void TranspositionTable::remove(const Hash_t hash) {
	n_nodes--;
	bst[hash & pool_mask].remove(hash);
}

bool TranspositionTable::exists(const Hash_t hash) const {
	return bst[hash & pool_mask].exists(hash);
}