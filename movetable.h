/******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 12 April 2017
*
* Type definitions for move making tables.
*
* A general strategy is to allocate as much move option storage as possible on
* the stack and use static variables in frequently-accessed functions to
* reduce allocation expenses and increase chances of caching.
*/

#ifndef DEEP_WINKELMAN_MOVETABLE
#define DEEP_WINKELMAN_MOVETABLE

#include <iostream>

// 64-bit integer representing binary states on board squares
typedef uint64_t Bitmask_t;
std::string print_mask(const Bitmask_t mask);
// 1 represented as a bitmask to avoid casting problems
const static Bitmask_t one = 1;

// 8-bit integer representing the index of a set of coordinates in an array
typedef uint8_t Combo_t;

// 8-bit integer representing a square on the board
typedef uint8_t Coord_t;

// Structure for storing a list of possible moves as coordinates.
// Limited to 8 coordinates.
// The number of coordinates in the list is maintained.
// Coordinates cannot be removed.
class MoveList {
public:
	// Number of coordinates counted.
	uint8_t n_coords;
	// 8-array of coordinates. Use n_coords to determine search limits.
	Coord_t coords[8];

	MoveList();
	MoveList(const Bitmask_t mask);

	void add(const Coord_t coord);
	Bitmask_t to_bitmask() const;

	friend std::ostream & operator <<(std::ostream & os, const MoveList & movelist);
};

// Masks for how friendly and enemy pieces affect move options for linear pieces.
class CollisionTable {
public:
	// Friendly piece collisions
	Combo_t f[8][256];

	// Enemy piece collisions
	Combo_t e[8][256];

	CollisionTable();

	void generate();
};

// Structure for accessing lists of moves pieces can make in a position.
// These are differentiated between piece types using subclasses.
// These are initialized objects.
class MoveTable {
protected:
	// Must include some sort of data storage for move lists.
	// Does not have to be public.

public:
	// Must include a function for generating bitmasks for each square.
	virtual void generate_bitmasks() = 0;

	// Must include a function for generating necessary move lists.
	virtual void generate_moves() = 0;

	// Must include a function for translating a move bitmask and a coordinate
	// into a combo-style index.
	virtual Combo_t mask_to_combo(const Coord_t coord, const Bitmask_t mask) const = 0;

	// Must include a function for translating a combo-style index and a
	// coordinate into a move bitmask.
	virtual Bitmask_t combo_to_mask(const Coord_t coord, const Combo_t combo) const = 0;

	// Must include a function for accessing the available moves from a
	// coordinate and a bitmask of friendly and enemy pieces on the board.
	virtual MoveList & get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy) = 0;
};

// Structure for accessing horizontal move options.
// See MoveTable.
class HMoveTable : public MoveTable {
protected:
	friend class MoveManager;

	Bitmask_t masks[64];
	CollisionTable ct;
	MoveList moves[8][256];

public:
	HMoveTable();

	void generate_bitmasks();
	void generate_moves();
	Combo_t mask_to_combo(const Coord_t coord, const Bitmask_t mask) const;
	Bitmask_t combo_to_mask(const Coord_t coord, const Combo_t combo) const;
	MoveList & get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy);

protected:
	void _tests();
};

// Structure for accessing vertical move options.
// See MoveTable.
class VMoveTable : public MoveTable {
protected:
	friend class MoveManager;

	Bitmask_t masks[64];
	CollisionTable ct;
	MoveList moves[8][256];

public:
	VMoveTable();

	void generate_bitmasks();
	void generate_moves();
	Combo_t mask_to_combo(const Coord_t coord, const Bitmask_t mask) const;
	Bitmask_t combo_to_mask(const Coord_t coord, const Combo_t combo) const;
	MoveList & get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy);

protected:
	void _tests();
};

// Structure for accessing a1-h8 diagonal move options.
// See MoveTable.
class D1MoveTable : public MoveTable {
protected:
	friend class MoveManager;

	Bitmask_t masks[64];
	CollisionTable ct;
	// moves_middle is used for squares on the a1-h8 diagonal
	// moves_low is used for squares below, moves_high used for squares above
	// move_offsets is used to calculate the offset within moves_low and moves_high:
	// the index used in move_offsets is the number of diagonals away from the central
	MoveList moves_middle[256], moves_low[256], moves_high[256];
	// Start indexes for low and high moves
	const Combo_t move_offsets[9] = { 0, 0, 128, 192, 224, 240, 248, 252, 254 };

public:
	D1MoveTable();

	void generate_bitmasks();
	void generate_moves();
	Combo_t mask_to_combo(const Coord_t coord, const Bitmask_t mask) const;
	Bitmask_t combo_to_mask(const Coord_t coord, const Combo_t combo) const;
	MoveList & get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy);

protected:
	void _tests();
};

// Structure for accessing h1-a8 diagonal move options.
// See MoveTable.
class D2MoveTable : public MoveTable {
protected:
	friend class MoveManager;

	Bitmask_t masks[64];
	CollisionTable ct;
	// moves_middle is used for squares on the h1-a8 diagonal
	// moves_low is used for squares below, moves_high used for squares above
	// move_offsets is used to calculate the offset within moves_low and moves_high:
	// the index used in move_offsets is the number of diagonals away from the central
	MoveList moves_middle[256], moves_low[256], moves_high[256];
	// Start indexes for low and high moves
	const Combo_t move_offsets[9] = { 0, 0, 128, 192, 224, 240, 248, 252, 254 };

public:
	D2MoveTable();

	void generate_bitmasks();
	void generate_moves();
	Combo_t mask_to_combo(const Coord_t coord, const Bitmask_t mask) const;
	Bitmask_t combo_to_mask(const Coord_t coord, const Combo_t combo) const;
	MoveList & get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy);

protected:
	void _tests();
};

// Structure for accessing knight move options.
// See MoveTable.
class NMoveTable : public MoveTable {
protected:
	friend class MoveManager;

	Bitmask_t masks[64];

	Coord_t move_option_coords[64][8];
	const int n_move_options[64]{
		2,3,4,4,4,4,3,2,
		3,4,6,6,6,6,4,3,
		4,6,8,8,8,8,6,4,
		4,6,8,8,8,8,6,4,
		4,6,8,8,8,8,6,4,
		4,6,8,8,8,8,6,4,
		3,4,6,6,6,6,4,3,
		2,3,4,4,4,4,3,2
	};

	// big block of memory to be allocated for each square
	MoveList data[(4 * 4) + (8 * 8) + (20 * 16) + (16 * 64) + (16 * 256)];
	// offsets in move array for each square
	MoveList * moves[64];

public:
	NMoveTable();

	void generate_bitmasks();
	void generate_moves();
	Combo_t mask_to_combo(const Coord_t coord, const Bitmask_t mask) const;
	Bitmask_t combo_to_mask(const Coord_t coord, const Combo_t combo) const;
	MoveList & get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy);

protected:
	void _tests();
};

// Structure for accessing knight move options.
// See MoveTable.
class KMoveTable : public MoveTable {
protected:
	friend class MoveManager;

	Bitmask_t masks[64];

	Coord_t move_option_coords[64][8];
	const int n_move_options[64]{
		3,5,5,5,5,5,5,3,
		5,8,8,8,8,8,8,5,
		5,8,8,8,8,8,8,5,
		5,8,8,8,8,8,8,5,
		5,8,8,8,8,8,8,5,
		5,8,8,8,8,8,8,5,
		5,8,8,8,8,8,8,5,
		3,5,5,5,5,5,5,3
	};

	// big block of memory to be allocated for each square
	MoveList data[(4 * 8) + (24 * 32) + (36 * 256)];
	// offsets in move array for each square
	MoveList * moves[64];

public:
	KMoveTable();

	void generate_bitmasks();
	void generate_moves();
	Combo_t mask_to_combo(const Coord_t coord, const Bitmask_t mask) const;
	Bitmask_t combo_to_mask(const Coord_t coord, const Combo_t combo) const;
	MoveList & get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy);

protected:
	void _tests();
};

// Structure for accessing white pawn move options.
// See MoveTable.
class WPMoveTable : public MoveTable {
protected:
	friend class MoveManager;

	Bitmask_t friendly_masks[64], enemy_masks[64], masks[64];
	Bitmask_t protection_masks[64];
	MoveList rank_2_moves[8][16], reg_moves[40][8];
	// Return default if no moves are legal (first/eighth ranks)
	MoveList NULL_MOVELIST;

public:
	WPMoveTable();

	void generate_bitmasks();
	void generate_moves();
	Combo_t mask_to_combo(const Coord_t coord, const Bitmask_t mask) const;
	Bitmask_t combo_to_mask(const Coord_t coord, const Combo_t combo) const;
	MoveList & get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy);

	unsigned int pieces_attacked(const Bitmask_t pawns, const Bitmask_t pieces) const;
	unsigned int blocked_pawns(const Bitmask_t pawns, const Bitmask_t pieces) const;
	unsigned int doubled_pawns(const Bitmask_t pawns) const;
	unsigned int square_control(const Bitmask_t pawns, const Bitmask_t region) const;
	unsigned int pawns_in_rank(const Bitmask_t pawns, const int rank) const;
	unsigned int pawns_in_file(const Bitmask_t pawns, const int file) const;

protected:
	void _tests();
};

// Structure for accessing black pawn move options.
// See MoveTable.
class BPMoveTable : public MoveTable {
protected:
	friend class MoveManager;

	Bitmask_t friendly_masks[64], enemy_masks[64], masks[64];
	MoveList rank_2_moves[8][16], reg_moves[40][8];
	// Return default if no moves are legal (first/eighth ranks)
	MoveList NULL_MOVELIST;

public:
	BPMoveTable();

	void generate_bitmasks();
	void generate_moves();
	Combo_t mask_to_combo(const Coord_t coord, const Bitmask_t mask) const;
	Bitmask_t combo_to_mask(const Coord_t coord, const Combo_t combo) const;
	MoveList & get_movelist(const Coord_t coord, const Bitmask_t friendly, const Bitmask_t enemy);

	unsigned int pieces_attacked(const Bitmask_t pawns, const Bitmask_t pieces) const;
	unsigned int blocked_pawns(const Bitmask_t pawns, const Bitmask_t pieces) const;
	unsigned int doubled_pawns(const Bitmask_t pawns) const;
	unsigned int square_control(const Bitmask_t pawns, const Bitmask_t region) const;
	unsigned int pawns_in_rank(const Bitmask_t pawns, const int rank) const;
	unsigned int pawns_in_file(const Bitmask_t pawns, const int file) const;

protected:
	void _tests();
};

class MoveManager {
public:
	HMoveTable h_moves;
	VMoveTable v_moves;
	D1MoveTable d1_moves;
	D2MoveTable d2_moves;
	NMoveTable n_moves;
	KMoveTable k_moves;
	WPMoveTable wp_moves;
	BPMoveTable bp_moves;

	unsigned int no_piece_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black) {
		return 0;
	}
	unsigned int wp_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int bp_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int wn_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int bn_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int wb_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int bb_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int wr_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int br_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int wq_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int bq_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int wk_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);
	unsigned int bk_move_count(
		const Coord_t coord, const Bitmask_t white, const Bitmask_t black);

	unsigned int(MoveManager::*move_counters[13])
		(const Coord_t, const Bitmask_t, const Bitmask_t) = {
		&MoveManager::no_piece_count,
		&MoveManager::wp_move_count,
		&MoveManager::wn_move_count,
		&MoveManager::wb_move_count,
		&MoveManager::wr_move_count,
		&MoveManager::wq_move_count,
		&MoveManager::wk_move_count,
		&MoveManager::bp_move_count,
		&MoveManager::bn_move_count,
		&MoveManager::bb_move_count,
		&MoveManager::br_move_count,
		&MoveManager::bq_move_count,
		&MoveManager::bk_move_count
	};
};

#endif