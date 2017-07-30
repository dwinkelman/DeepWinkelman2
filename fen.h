/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 18 May 2017
*
* Parsing of Forsyth-Edwards Notation (FEN) strings into bitboards
*/

#ifndef DEEP_WINKELMAN_FEN
#define DEEP_WINKELMAN_FEN

#include "bitboard.h"

Bitboard parse_fen(std::string fen) {
	Piece_t board[64];

	std::string::iterator it, end;
	it = fen.begin(), end = fen.end();

	// create piece array
	int rank = 7, file = 0;
	while (*it != ' ') {
		if ('1' <= *it && *it <= '8') {
			for (int i = 0; i < *it - '0'; i++) {
				board[rank * 8 + file] = NO_PIECE;
				file++;
			}
		}
		else if (*it == '/') {
			rank--;
			file = 0;
		}
		else {
			switch (*it) {
			case 'P': board[rank * 8 + file] = WHITE_PAWN;		file++; break;
			case 'N': board[rank * 8 + file] = WHITE_KNIGHT;	file++; break;
			case 'B': board[rank * 8 + file] = WHITE_BISHOP;	file++; break;
			case 'R': board[rank * 8 + file] = WHITE_ROOK;		file++; break;
			case 'Q': board[rank * 8 + file] = WHITE_QUEEN;		file++; break;
			case 'K': board[rank * 8 + file] = WHITE_KING;		file++; break;
			case 'p': board[rank * 8 + file] = BLACK_PAWN;		file++; break;
			case 'n': board[rank * 8 + file] = BLACK_KNIGHT;	file++; break;
			case 'b': board[rank * 8 + file] = BLACK_BISHOP;	file++; break;
			case 'r': board[rank * 8 + file] = BLACK_ROOK;		file++; break;
			case 'q': board[rank * 8 + file] = BLACK_QUEEN;		file++; break;
			case 'k': board[rank * 8 + file] = BLACK_KING;		file++; break;
			default:											file++; break;
			}
		}
		++it;
	}
	++it;

	// get color to move
	Color_t color_to_move = *it == 'w' ? WHITE : BLACK;
	++it;

	// get castling
	Castling_t castling =
		(*(it++) == 'K') ? WHITE_OO : 0 |
		(*(it++) == 'Q') ? WHITE_OOO : 0 |
		(*(it++) == 'k') ? BLACK_OO : 0 |
		(*(it++) == 'q') ? BLACK_OOO : 0;

	Bitboard output(board, color_to_move, castling);
	return output;
}

#endif