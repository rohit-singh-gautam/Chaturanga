#include "Evaluator.h"
#include <random>

namespace rohit {

	Evaluator::Evaluator(const Ashtapada &ashtapada) : ashtapada(ashtapada), score(0), hash(0), zobristTable() {
		std::default_random_engine generator;
		std::uniform_int_distribution<board_hash_type> distribution(0, ULLONG_MAX);
		for (row_t row = row_t::row_start; row <= row_t::row_last; row++) {
			for (col_t col = col_t::col_start; col <= col_t::col_last; col++) {
				for (piece_t piece = piece_t::begin; piece != player_t::end; piece++) {
					zobristTable[static_cast<size_t>(row)][static_cast<size_t>(col)][piece] = distribution(generator);
				}
			}
		}
	}

	void Evaluator::refresh()
	{
		score = 0;
		hash = 0;
		for (row_t row = row_t::row_start; row <= row_t::row_last; row++) {
			for (col_t col = col_t::col_start; col <= col_t::col_last; col++) {
				const Position pos(row, col);
				const auto pieceEntry = ashtapada[pos];
				if (pieceEntry != nullptr) addPiece(pieceEntry->val, pos);
			}
		}
	}
}