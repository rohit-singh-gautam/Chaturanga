#include "pieces.h"
#include <iostream>

namespace rohit {

	std::ostream & operator<<(std::ostream & os, const player_t player)
	{
		if (player > player_t::unknown) os << "Invalid";
		else os << player_t::displayString[player];
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const piece_t piece) {
		char mohraChar = piece.getMohra().getChar();

		if (mohraChar == '\0') {
			os.setstate(std::ios_base::failbit);
		}
		else {
			os << piece.getShortString();
		}
		return os;
	}

	std::ostream & operator<<(std::ostream & os, const PieceEntry &pieceEntry)
	{
		os << static_cast<piece_t>(pieceEntry) << pieceEntry.position;
		return os;
	}



	std::ostream & operator<<(std::ostream & os, const PieceList & pieceList)
	{
		os << "Active: " << pieceList.activeList << "; Captured: " << pieceList.capturedList;
		return os;
	}

	void Pieces::reset()
	{
		for (row_t row = 0_row; row <= 7_row; row++) {
			for (col_t col = 0_col; col <= 7_col; col++) {
				blocks[Position::getIndex(row, col)] = nullptr;
			}
		}

		checked = player_t::none;
		int pieceCount = 0;
		activeCount[0] = 0;
		activeCount[1] = 0;
		for (piece_t piece = piece_t::begin; piece != piece_t::end; piece++) {
			PieceList &list = pieceMap[piece];
			list.freeAll(); // Removes all the mohra from list
			int maxMohra = MaxPossibleMohra[piece]; // Max possible mohra
			for (int count = 0; count < maxMohra; count++) {
				_Entry *pieceEntry = &pieceEntryAllocation[pieceCount++];
				pieceEntry->val = piece;
				list.insertInCaptured(pieceEntry);
			}
		}
	}

	bool Pieces::addPiece(const piece_t piece, const Position position)
	{
		auto capturedList = getCaptured(piece);
		if (capturedList.isEmpty()  || !position.verify()) return false;

		auto it = capturedList.begin();
		assert(*it == piece);
		it->position = position;
		setEntry(position, it.getPtr());
		getMap(piece).moveToActive(it);
		++activeCount[piece.getPlayer()];

		return true;
	}

	std::ostream & operator<<(std::ostream & os, const Pieces & pieces)
	{
		for (row_t row = row_t::row_rstart; row != row_t::row_rend; row--) {
			for (col_t col = 0_col; col <= col_t::col_last; col++) {
				const Position pos { row, col };
				const Pieces::_Entry *pieceEntry = pieces[pos];
				if (pieceEntry == nullptr) {
					os << piece_t::getNullShortString();
				}
				else {
					os << static_cast<piece_t>(pieceEntry->val);
				}
			}
			os << std::endl;
		}
		return os;
	}
}

