#include "Chal.h"

namespace rohit {

	const PadatiPromotion PadatiPromotion::None = { player_t::unknown, col_t::col_unknown };

	// Add padati is being promoted
	const PadatiPromotion PadatiPromotion::findSelfPadatiPromotion(const Position to, const piece_t atFrom, const Pieces & pieces)
	{
		row_t row_last = getLastRow(atFrom.getPlayer());
		if (atFrom.getMohra() != mohra_t::Padati || to.getRow() != row_last) return PadatiPromotion::None;
		mohra_t promotedMohra = getPromotedMohra(to.getCol());
		piece_t promotedPiece(promotedMohra, atFrom.getPlayer());
		if (pieces.hasCaptured(promotedPiece)) {
			return { atFrom.getPlayer(), to.getCol() };
		}

		// Piece at position is not captured will have to wait
		return PadatiPromotion::None;
	}

	// If padati of oppenent is being promoted
	// If 2 padati are waiting for same Mohra one on left will get it. We are just avoiding alternate option for time being.
	const PadatiPromotion PadatiPromotion::findOpponentPadatiPromotion(const piece_t capturedList, const Pieces & pieces) {
		if (capturedList == piece_t::Empty || capturedList.getMohra() == mohra_t::Padati) return PadatiPromotion::None;
		const piece_t padati(mohra_t::Padati, capturedList.getPlayer());
		row_t row_last = getLastRow(capturedList.getPlayer());
		const Position positions[2] = {
			{row_last, Pieces::getInitialCol(capturedList.getMohra(), 0)},
			{row_last, Pieces::getInitialCol(capturedList.getMohra(), 1)},
		};

		for (auto pos : positions) {
			if (pos.getCol() == col_t::col_unknown) break;
			const auto pieceEntry = pieces[pos];
			if (pieceEntry != nullptr && pieceEntry->val == padati) {
				return { capturedList.getPlayer(), pos.getCol() };
			}
		}

		return PadatiPromotion::None;
	}

	bool MohraChal::addValidEmptyCapture(const Position oldPos, const DiffPosition diffPos, StoreType & moves, const player_t player) const
	{
		const Position newPos = oldPos + diffPos;
		if (!newPos.verify()) return false;
		if (pieces.isEmpty(newPos)) {
			moves.push_back_uncheck(Chal(oldPos, newPos, pieces));
			return true;
		}
		else {
			if (pieces.isPlayerAt(newPos, player.opponent())) {
				moves.push_back_uncheck(Chal(oldPos, newPos, pieces));
			}
			return false;
		}
	}

	bool MohraChal::addValidEmpty(const Position oldPos, const DiffPosition diffPos, StoreType & moves) const
	{
		const Position newPos = oldPos + diffPos;
		if (!newPos.verify()) return false;
		if (pieces.isEmpty(newPos)) {
			moves.push_back_uncheck(Chal(oldPos, newPos, pieces));
			return true;
		}
		else {
			return false;
		}
	}

	bool MohraChal::addValidCapture(const Position oldPos, const DiffPosition diffPos, StoreType & moves, const player_t player) const
	{
		const Position newPos = oldPos + diffPos;
		if (!newPos.verify()) return false;
		if (pieces.isEmpty(newPos))  return true;

		if (pieces.isPlayerAt(newPos, player.opponent())) {
			moves.push_back_uncheck(Chal(oldPos, newPos, pieces));
		}
		return false;
	}

	bool MohraChal::isAttackedHelper(const mohra_t by, const Position to) const
	{
		if (pieces.isEmpty(to)) return 0;
		auto pieceEntry = pieces[to];
		const piece_t piece(by, pieceEntry->val.getPlayer().opponent());
		auto &pieceList = pieces[piece];
		
		auto &activeList = pieceList.getActive();
		for (const auto &entry : activeList) {
			const DiffPosition diffPos = to - entry.position;
			const auto row = unsafeCenter.getRow() + diffPos.getRow();
			const auto col = unsafeCenter.getCol() + diffPos.getCol();
			const bitmohra_t bitmohra = getUnsafe(row, col);

			if (bitmohra.contains(by)) return true;

		}

		return false;
	}

	void RajaChal::addChal(const PieceEntry &entry, StoreType &moves) const
	{
		assert(entry.getMohra() == mohra_t::Raja);
		for (auto diffPos : posList) {
			addValidEmptyCapture(entry.position, diffPos, moves, entry.getPlayer());
		}
	}

	bool RajaChal::validateChal(const Chal & chal) const
	{
		//Not validating from as we are here because of from
		// chal.atFrom == MohraType::Raja
		if (chal.from == chal.to) return false;

		bool valid = false;
		for (auto diffPos : posList) {
			if (chal.from + diffPos == chal.to) {
				valid = true;
				break;
			}
		}

		return valid && (chal.capturedList == piece_t::Empty || chal.atFrom.getPlayer() != chal.capturedList.getPlayer());
	}

	bool RajaChal::isAttacked(const Position to) const
	{
		return isAttackedHelper(mohra_t::Raja, to);
	}

	void SenapatiChal::addChal(const PieceEntry &entry, StoreType &moves) const
	{
		assert(entry.getMohra() == mohra_t::Senapati);
		for (auto diffPos : posList) {
			addValidEmptyCapture(entry.position, diffPos, moves, entry.getPlayer());
		}
	}

	bool SenapatiChal::validateChal(const Chal & chal) const
	{
		// Not validating from as we are here because of from
		// chal.atFrom == MohraType::Senapati
		if (chal.from == chal.to) return false;

		bool valid = false;
		for (auto diffPos : posList) {
			if (chal.from + diffPos == chal.to) {
				valid = true;
				break;
			}
		}

		return valid && (chal.capturedList == piece_t::Empty || chal.atFrom.getPlayer() != chal.capturedList.getPlayer());
	}

	bool SenapatiChal::isAttacked(const Position to) const
	{
		return isAttackedHelper(mohra_t::Senapati, to);
	}

	void RathaChal::addChal(const PieceEntry &entry, StoreType &moves) const
	{
		assert(entry.getMohra() == mohra_t::Ratha);
		for (auto row = 1_row; row <= 7_row && addValidEmptyCapture(entry.position, DiffPosition(row, 0_col), moves, entry.getPlayer()); row++);
		for (auto row = -1_row; row >= -7_row && addValidEmptyCapture(entry.position, DiffPosition(row, 0_col), moves, entry.getPlayer()); row--);
		for (auto col = 1_col; col <= 7_col && addValidEmptyCapture(entry.position, DiffPosition(0_row, col), moves, entry.getPlayer()); col++);
		for (auto col = -1_col; col >= -7_col && addValidEmptyCapture(entry.position, DiffPosition(0_row, col), moves, entry.getPlayer()); col--);
	}

	bool RathaChal::validateChal(const Chal & chal) const
	{
		// Not validating from as we are here because of from
		// chal.atFrom == MohraType::Gaja
		if (chal.from == chal.to) return false;
		if (chal.capturedList != piece_t::Empty && chal.atFrom.getPlayer() == chal.capturedList.getPlayer()) return false;

		if (chal.from.getRow() == chal.to.getRow()) {
			if (chal.from.getCol() < chal.to.getCol()) {
				for (auto col = chal.from.getCol() + 1_col; col < chal.to.getCol(); col++) {
					if (pieces[Position(chal.from.getRow(), col)] != nullptr) return false;
				}
			}
			else {
				for (auto col = chal.from.getCol() - 1_col; col > chal.to.getCol(); col--) {
					if (pieces[Position(chal.from.getRow(), col)] != nullptr) return false;
				}
			}
			return true;
		}
		else if (chal.from.getCol() == chal.to.getCol()) {
			if (chal.from.getRow() < chal.to.getRow()) {
				for (auto row = chal.from.getRow() + 1_row; row < chal.to.getRow(); row++) {
					if (pieces[Position(row, chal.from.getCol())] != nullptr) return false;
				}
			}
			else {
				for (auto row = chal.from.getRow() - 1_row; row > chal.to.getRow(); row--) {
					if (pieces[Position(row, chal.from.getCol())] != nullptr) return false;
				}
			}
			return true;
		}
		else {
			return false;
		}
	}

	bool RathaChal::isAttacked(const Position to) const
	{
		if (pieces.isEmpty(to)) return 0;
		auto pieceEntry = pieces[to];
		const piece_t piece(mohra_t::Ratha, pieceEntry->val.getPlayer().opponent());
		auto &pieceList = pieces[piece];

		auto &activeList = pieceList.getActive();
		for (const auto &entry : activeList) {
			const auto diffPos = entry.position - to;
			const row_t row = unsafeCenter.getRow() + diffPos.getRow();
			const col_t col = unsafeCenter.getCol() + diffPos.getCol();
			const bitmohra_t bitmohra = getUnsafe(row, col);

			if (bitmohra.contains(mohra_t::Ratha)) {
				const auto diffDir = diffPos.getDirectionDiff();
				bool bValid = true;
				for (Position iPos = to + diffDir; iPos != entry.position; iPos = iPos + diffDir) {
					if (pieces[iPos] != nullptr) {
						bValid = false;
						break;
					}
				}
				if (bValid) return true;
			}
		}

		return false;
	}

	void GajaChal::addChal(const PieceEntry &entry, StoreType &moves) const
	{
		assert(entry.getMohra() == mohra_t::Gaja);
		for (auto diffPos : posList) {
			addValidEmptyCapture(entry.position, diffPos, moves, entry.getPlayer());
		}
	}

	bool GajaChal::validateChal(const Chal & chal) const
	{
		// Not validating from as we are here because of from
		// chal.atFrom == MohraType::Gaja
		if (chal.from == chal.to) return false;

		bool valid = false;
		for (auto diffPos : posList) {
			if (chal.from + diffPos == chal.to) {
				valid = true;
				break;
			}
		}

		return valid && (chal.capturedList == piece_t::Empty || chal.atFrom.getPlayer() != chal.capturedList.getPlayer());
	}

	bool GajaChal::isAttacked(const Position to) const
	{
		return isAttackedHelper(mohra_t::Gaja, to);
	}

	void AshvaChal::addChal(const PieceEntry &entry, StoreType &moves) const
	{
		assert(entry.getMohra() == mohra_t::Ashva);
		for (auto diffPos : posList) {
			addValidEmptyCapture(entry.position, diffPos, moves, entry.getPlayer());
		}
	}

	bool AshvaChal::validateChal(const Chal & chal) const
	{
		// Not validating from as we are here because of from
		// chal.atFrom == MohraType::Ashva
		if (chal.from == chal.to) return false;

		bool valid = false;
		for (auto diffPos : posList) {
			if (chal.from + diffPos == chal.to) {
				valid = true;
				break;
			}
		}

		return valid && (chal.capturedList == piece_t::Empty || chal.atFrom.getPlayer() != chal.capturedList.getPlayer());
	}

	bool AshvaChal::isAttacked(const Position to) const
	{
		return isAttackedHelper(mohra_t::Ashva, to);
	}

	void PadatiChal::addChal(const PieceEntry &entry, StoreType &moves) const
	{
		assert(entry.getMohra() == mohra_t::Padati);
		player_t player = entry.getPlayer();
		addValidEmpty(entry.position, posList[player], moves);

		for (const DiffPosition &diffPos : capturePosLists[player]) {
			addValidCapture(entry.position, diffPos, moves, player);
			assert(moves.back_uncheck().capturedList.getMohra() != mohra_t::Raja);
		}

	}

	bool PadatiChal::validateChal(const Chal & chal) const
	{
		if (chal.from == chal.to) return false;
		const player_t player = chal.atFrom.getPlayer();
		if (chal.capturedList == piece_t::Empty && chal.to == chal.from + posList[player]) return true;

		if (chal.capturedList == piece_t::Empty) return false;

		bool valid = false;
		for (auto diffPos : capturePosLists[player]) {
			if (chal.from + diffPos == chal.to) {
				valid = true;
				break;
			}
		}

		return valid == true && chal.atFrom.getPlayer() != chal.capturedList.getPlayer();

	}

	bool PadatiChal::isAttacked(const Position to) const
	{
		if (pieces.isEmpty(to)) return false;
		auto atTo = pieces[to];
		assert(atTo->val.getMohra() == mohra_t::Raja);
		const piece_t padati(mohra_t::Padati, atTo->val.getPlayer().opponent());
		for (auto diffPos : capturePosLists[atTo->val.getPlayer()]) {
			auto newPos = to + diffPos;
			if (!newPos.verify()) continue;
			auto pieceEntry = pieces[newPos];
			if (pieceEntry != nullptr && pieceEntry->val == padati) return true;
		}

		return false;
	}

	std::ostream & operator<<(std::ostream & os, const Chal & chal)
	{
		os << chal.atFrom << '{' << chal.from << "->" << chal.to;
		if (chal.capturedList != piece_t::Empty) os << ",X" << chal.capturedList;
		if (chal.selfPromotion != PadatiPromotion::None) {
			const piece_t oldPiece(mohra_t::Padati, chal.atFrom.getPlayer());
			const piece_t newPiece(Pieces::getInitialColMohra(chal.selfPromotion.getCol()), chal.atFrom.getPlayer());
			os << ',' << oldPiece << chal.selfPromotion.getCol() << "->" << newPiece;
		}
		if (chal.selfPromotion != PadatiPromotion::None) {
			const player_t opponentPlayer = chal.atFrom.getPlayer().opponent();
			const piece_t oldPiece(mohra_t::Padati, opponentPlayer);
			const piece_t newPiece(Pieces::getInitialColMohra(chal.selfPromotion.getCol()), opponentPlayer);
			os << ',' << oldPiece << chal.selfPromotion.getCol() << "->" << newPiece;
		}

		os << '}';

		return os;
	}
}
