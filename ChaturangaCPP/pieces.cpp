#include "pch.h"
#include "pieces.h"
#include <iostream>

namespace rohit {

	const std::string player_t::displayString[]{
		"First",
		"Second",
		"Unknown",
		"None",
		"Invalid"
	};

	const int player_t::signArray[] = { -1, 1, 0, 0, 0 };


	const char mohra_t::CHAR[] = { 'R', 'S', 'C', 'G', 'A', 'P', '\0' };
	const std::string mohra_t::displayString[]{
		"Raja",
		"Senapati",
		"Ratha",
		"Gaja",
		"Ashva",
		"Padati",
		"Empty",
		"Invalid"
	};

	const bitmohra_t bitmohra_t::Empty(0);
	const bitmohra_t bitmohra_t::Raja(1);
	const bitmohra_t bitmohra_t::Senapati(2);
	const bitmohra_t bitmohra_t::Ratha(4);
	const bitmohra_t bitmohra_t::Gaja(8);
	const bitmohra_t bitmohra_t::Ashva(16);
	const bitmohra_t bitmohra_t::Padati(32);
	const bitmohra_t bitmohra_t::End(64);


	std::ostream & operator<<(std::ostream & os, const player_t player)
	{
		if (player > player_t::unknown) os << "Invalid";
		else os << player_t::displayString[player];
		return os;
	}

	const std::string piece_t::displayString[]{
	"firstRaja",
	"secondRaja",
	"firstSenapati",
	"secondSenapati",
	"firstRatha",
	"secondRatha",
	"firstGaja",
	"secondGaja",
	"firstAshva",
	"secondAshva",
	"firstPadati",
	"secondPadati",
	"Empty",
	"Unknown"
	};

	std::ostream& operator<<(std::ostream& os, const piece_t piece) {
		char mohraChar = piece.getMohra().getChar();

		if (mohraChar == '\0') {
			os.setstate(std::ios_base::failbit);
		}
		else {
			char base = piece.getPlayer() == player_t::first ? mohraChar : mohraChar + -'A' + 'a';
			os << base;
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

	const mohra_t Pieces::initialColMohra[] = //Used only for padati promotion
	{ mohra_t::Ratha, mohra_t::Ashva, mohra_t::Gaja, mohra_t::Senapati, mohra_t::Senapati, mohra_t::Gaja, mohra_t::Ashva, mohra_t::Ratha };

	const col_t Pieces::initialCol[mohra_t::max][2] = {
		{ 4_col, col_t::col_unknown },				// Raja
		{ 3_col, 4_col },							// Senapati, marking both the center
		{ 0_col, 7_col },							// Ratha
		{ 2_col, 5_col },							// Gaja
		{ 1_col, 6_col },							// Ashva
		{ col_t::col_unknown, col_t::col_unknown },	// Padati
	};

	const int Pieces::MaxPossibleMohra[piece_t::max] = { 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 8, 8 };

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
				const Position pos(row, col);
				const Pieces::_Entry *pieceEntry = pieces[pos];
				if (pieceEntry == nullptr) {
					os << " .";
				}
				else {
					if (pieceEntry->val.position != pos) os << '?';
					else os << ' ';
					os << static_cast<piece_t>(pieceEntry->val);
				}
			}
			os << std::endl;
		}
		return os;
	}
}

