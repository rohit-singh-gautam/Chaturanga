#include "Evaluator.h"
#include <random>

namespace rohit {
	const board_score_type Evaluator::mohraScores[] = { 0, 0, -20, 20, -80, 80, -30, 30, -40, 40, -11, 11, 0, 0 };
	const board_score_type Evaluator::checkScore[] = { 10, -10, 0 };
	const board_score_type Evaluator::checkmateScore[] = { 12345, -12345, 0 };
	const board_score_type Evaluator::lowest_score[] = { 12345, -12345 };

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

	Evaluator::~Evaluator()
	{
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