/******************************************************************************
* Binary Search Tree
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 24 May 2017
*
* Simple, light-weight binary search tree.
*
* All operations on the tree are O(log n) complexity.
* Inserting a list of n elements to a tree has complexity of O(n (log n - 1)).
*/

#ifndef BST_BST_H
#define BST_BST_H

#include <iostream>
#include <sstream>
#include <string>

template <typename K, typename V>
class BST {
protected:
	struct Node {
	public:
		K key;
		V value;
		Node * left, * right;
		
		Node();
		Node(K key, V value);
		
		std::string print(int level);
	};
	
	Node root;
	
public:
	BST(K middle_value = 0, V null_value = (V)0);

	void insert(K key, V value);
	V get(K key) const;
	V get_if_exists(K key, bool * exists) const;
	void set(K key, V value);
	bool exists(K key) const;
	V remove(K key);
	
	Node min() const;
	Node max() const;
	V remove_max();
	
	std::string print();
	
protected:
	void insert(Node * node);
	void remove(Node ** node);
};

#endif