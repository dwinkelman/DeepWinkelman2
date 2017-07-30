/******************************************************************************
* Binary Search Tree
*
* Copyright(c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 24 May 2017
*
* Implementations for tree methods
*/

#include "bst.h"
#include "node.h"

// Initialize a BST node
template<typename K, typename V>
BST<K, V>::Node::Node() {
	this->key = (K)0;
	this->value = (V)0;
	left = right = nullptr;
}

// Initialize a BST node with data
template<typename K, typename V>
BST<K, V>::Node::Node(K key, V value) {
	this->key = key;
	this->value = value;
	left = right = nullptr;
}

// Print nodes recursively to create a tree visualization
template<typename K, typename V>
std::string BST<K, V>::Node::print(int level) {
	std::stringstream ss;
	ss << std::string(level, '\t');
	ss << key << ", " << value << '\n';
	if (left != nullptr) ss << left->print(level + 1);
	if (right != nullptr) ss << right->print(level + 1);
	return ss.str();
}

// Initialize a binary search tree
template<typename K, typename V>
BST<K, V>::BST(K middle_value, V null_value) {
	root = BST<K, V>::Node(middle_value, null_value);
}

// Insert an element into the tree
template<typename K, typename V>
void BST<K, V>::insert(K key, V value) {
	insert(new BST<K, V>::Node(key, value));
}

// Get an element by key from the tree
// Throws an error 1 is the element does not exist
template<typename K, typename V>
V BST<K, V>::get(K key) const {
	BST<K, V>::Node * current = (key > root.key) ? root.right : root.left;
	if (current != nullptr) {
		while (current->key != key) {
			current = (key > current->key) ? current->right : current->left;
			if (current == nullptr) break;
		}
	}
	if (current == nullptr) {
		std::cout << "Error. Key not found.\n";
		throw 1;
	}
	return current->value;
}

// Get an element by key from the tree
// Throws an error 1 is the element does not exist
template<typename K, typename V>
V BST<K, V>::get_if_exists(K key, bool * exists) const {
	BST<K, V>::Node * current = (key > root.key) ? root.right : root.left;
	if (current != nullptr) {
		while (current->key != key) {
			current = (key > current->key) ? current->right : current->left;
			if (current == nullptr) break;
		}
	}
	if (current == nullptr) {
		*exists = false;
		return 0;
	}
	*exists = true;
	return current->value;
}

// Set an element by key from the tree to a new value
// Throws an error 1 is the element does not exist
template<typename K, typename V>
void BST<K, V>::set(K key, V value) {
	BST<K, V>::Node * current = (key > root.key) ? root.right : root.left;
	if (current != nullptr) {
		while (current->key != key) {
			current = (key > current->key) ? current->right : current->left;
			if (current == nullptr) break;
		}
	}
	if (current == nullptr) {
		std::cout << "Error. Key not found.\n";
		throw 1;
	}
	current->value = value;
}

// Check if an element exists in the tree
template<typename K, typename V>
bool BST<K, V>::exists(K key) const {
	BST<K, V>::Node * current = (key > root.key) ? root.right : root.left;
	if (current != nullptr) {
		while (current->key != key) {
			current = (key > current->key) ? current->right : current->left;
			if (current == nullptr) break;
		}
	}
	return current != nullptr;
}

// Remove an element from the tree and return it
// Throws an error 1 is the element does not exist
template<typename K, typename V>
V BST<K, V>::remove(K key) {
	BST<K, V>::Node ** current = (key > root.key) ? &root.right : &root.left;
	if (*current != nullptr) {
		while ((*current)->key != key) {
			current = (key > (*current)->key) ? &(*current)->right : &(*current)->left;
			if (*current == nullptr) break;
		}
	}
	if (*current == nullptr) {
		std::cout << "Error. Key not found.\n";
		throw 1;
	}

	V output = (*current)->value;

	remove(current);

	return output;
}

// Find the node with the minimum key in the tree
template<typename K, typename V>
typename BST<K, V>::Node BST<K, V>::min() const {
	Node * current = root.left;
	while (current->left != nullptr) {
		current = current->left;
	}
	return Node(current->key, current->value);
}

// Find the node with the maximum key in the tree
template<typename K, typename V>
typename BST<K, V>::Node BST<K, V>::max() const {
	Node * current = root.right;
	while (current->right != nullptr) {
		current = current->right;
	}
	return Node(current->key, current->value);
}

// Return and delete the minimum key in the tree
template<typename K, typename V>
V BST<K, V>::remove_max() {
	Node ** current = &root.right;
	while ((*current)->right != nullptr) {
		current = &(*current)->right;
	}
	V output = (*current)->value;
	remove(current);
	return output;
}

// Print the tree node by node with indentation
template<typename K, typename V>
std::string BST<K, V>::print() {
	return root.left->print(0) + root.right->print(0);
}

// Private method for inserting a node pointer into the tree
template<typename K, typename V>
void BST<K, V>::insert(Node * node) {
	Node ** current = (node->key > root.key) ? &root.right : &root.left;
	while (*current != nullptr) {
		current = (node->key > (*current)->key) ? &(*current)->right : &(*current)->left;
	}
	*current = node;
}

// Private method for removing a node from the tree
template<typename K, typename V>
void BST<K, V>::remove(Node ** node) {
	// attach right side directly to parent, insert left
	BST<K, V>::Node * right = (*node)->right;
	BST<K, V>::Node * left = (*node)->left;
	delete *node;
	if (left != nullptr) {
		*node = left;
		if (right != nullptr) insert(right);
	}
	else if (right != nullptr) {
		*node = right;
	}
	else {
		*node = nullptr;
	}
}

/******************************************************************************
Place specializations of BST here
(so they can be explicitly compiled)
***/
template class BST<Hash_t, Node *>;
template class BST<Score_t, Node *>;