#pragma once
#include "Ashtapada.h"
#include <list>
#include <unordered_map>

namespace rohit {
	class Evaluator
	{
	public:
		static constexpr board_score_type mohraScores[] { 0, 0, -20, 20, -80, 80, -30, 30, -40, 40, -11, 11, 0, 0 };
		static constexpr board_score_type checkScore[] { 10, -10, 0 };
		static constexpr board_score_type checkmateScore[] { 12345, -12345, 0 };
		static constexpr board_score_type lowest_score[] { 12345, -12345 };

	private:
		const Ashtapada &ashtapada;
		board_score_type score;
		board_hash_type hash;

		board_hash_type zobristTable[row_max][col_max][piece_t::max];

	public:
		Evaluator(const Ashtapada &ashtapada);

		inline board_score_type getScore() { return score + checkScore[ashtapada.isChecked()]; }
		inline board_hash_type getHash() { return hash; }

		void refresh();

		inline void addPiece(const piece_t piece, const Position pos) {
			score += mohraScores[piece];
			hash ^= zobristTable[pos.getRowIndex()][pos.getColIndex()][piece];
		}
		inline void removePiece(const piece_t piece, const Position pos) {
			score -= mohraScores[piece];
			hash ^= zobristTable[pos.getRowIndex()][pos.getColIndex()][piece];
		}
		inline void movePiece(const piece_t piece, const Position from, const Position to) {
			hash ^= zobristTable[from.getRowIndex()][from.getColIndex()][piece];
			hash ^= zobristTable[to.getRowIndex()][to.getColIndex()][piece];
		}

		inline void apply(const Chal &chal) {
			if (chal.capturedList != piece_t::Empty) {
				removePiece(chal.capturedList, chal.to);
			}
			movePiece(chal.atFrom, chal.from, chal.to);
			apply(chal.selfPromotion);
			apply(chal.opponentPromotion);
		}

		inline void apply(const PadatiPromotion promotion) {
			if (promotion != PadatiPromotion::None) {
				const Position pos = promotion.getPosition();
				removePiece(promotion.getRemovedPadati(), pos);
				addPiece(promotion.getPromotedPiece(), pos);
			}
		}

		inline void undo(const Chal &chal) {
			movePiece(chal.atFrom, chal.to, chal.from);
			if (chal.capturedList != piece_t::Empty) {
				addPiece(chal.capturedList, chal.to);
			}
			undo(chal.selfPromotion);
			undo(chal.opponentPromotion);
		}

		inline void undo(const PadatiPromotion promotion) {
			if (promotion != PadatiPromotion::None) {
				const Position pos = promotion.getPosition();
				removePiece(promotion.getPromotedPiece(), pos);
				addPiece(promotion.getRemovedPadati(), pos);
			}
		}
	};
}
