#include "pch.h"
#include "Ashtapada.h"
#include <cstddef>

namespace rohit {

	void Ashtapada::promotePadati(const PadatiPromotion &padatiPromotion, const player_t player)
	{
		if (padatiPromotion != PadatiPromotion::None) {
			row_t row_last = player == player_t::first ? 7_row : 0_row;
			const Position posPadati(row_last, padatiPromotion.getCol());
			const piece_t piece(getInitialColMohra(padatiPromotion.getCol()), player);
			list &list = getCaptured(piece);
			assert(!list.isEmpty());
			moveCaptureTo(list.begin(), posPadati);
		}
	}

	void Ashtapada::undoPromotePadati(const PadatiPromotion &padatiPromotion, const player_t player)
	{
		if (padatiPromotion != PadatiPromotion::None) {
			row_t row_last = player == player_t::first ? 7_row : 0_row;
			const Position posPromoted(row_last, padatiPromotion.getCol());
			assert((*this)[posPromoted] != nullptr);
			assert((*this)[posPromoted]->val == piece_t(getInitialColMohra(padatiPromotion.getCol()), player));
			const piece_t piece(mohra_t::Padati, player);
			list &list = getCaptured(piece);
			assert(!list.isEmpty());
			moveCaptureTo(list.begin(), posPromoted);
		}
	}

	void Ashtapada::applyChalNoCheck(const Chal & chal)
	{
		assert(chal.to.verify());
		assert(chal.from.verify());
		assert(chal.capturedList.getMohra() != mohra_t::Raja);
		assert((*this)[chal.from] != nullptr);
		assert(chal.atFrom.verify());
		assert((*this)[chal.from]->val.position == chal.from);
		assert((*this)[chal.from]->val == chal.atFrom);

		markCaptured(chal.to);
		_Entry *pieceEntry = getEntry(chal.from);
		setEntry(chal.from, nullptr);
		pieceEntry->val.position = chal.to;
		setEntry(chal.to, pieceEntry);

		const player_t selfPlayer = chal.atFrom.getPlayer();
		const player_t opponentPlayer = selfPlayer.opponent();

		promotePadati(chal.selfPromotion, selfPlayer);
		promotePadati(chal.opponentPromotion, opponentPlayer);
	}

	void Ashtapada::undoChalNoCheck(const Chal & chal)
	{
		assert(chal.to.verify());
		assert(chal.from.verify());
		assert((*this)[chal.from] == nullptr);
		assert((*this)[chal.to] != nullptr);
		assert(chal.atFrom != piece_t::Empty);
		const player_t selfPlayer = chal.atFrom.getPlayer();
		const player_t opponentPlayer = selfPlayer.opponent();

		undoPromotePadati(chal.selfPromotion, selfPlayer);
		undoPromotePadati(chal.opponentPromotion, opponentPlayer);

		_Entry *pieceEntry = getEntry(chal.to);
		setEntry(chal.to, nullptr);
		pieceEntry->val.position = chal.from;
		setEntry(chal.from, pieceEntry);

		if (chal.capturedList != piece_t::Empty) {
			list &list = getCaptured(chal.capturedList);
			moveCaptureTo(list.begin(), chal.to);
		}
		assert((*this)[chal.from]->val.position == chal.from);
		assert((*this)[chal.from]->val == chal.atFrom);
	}

	bool Ashtapada::applyChalNoValidation(const Chal & chal)
	{
		const player_t selfPlayer = chal.atFrom.getPlayer();
		const player_t opponentPlayer = selfPlayer.opponent();

		assert(selfPlayer == currentPlayer);
		assert(checked != opponentPlayer);
		// This is internal function hence no generic validation for move
		// it must be done at entry point.
		applyChalNoCheck(chal);

		if (checked == selfPlayer) {
			// First remove check
			const PieceEntry &pieceEntry = getRaja(selfPlayer);
			if (chalFactory.isAttacked(pieceEntry.position)) {
				undoChalNoCheck(chal);
				return false;
			}
			checked = player_t::none;
		}
		else {
			//Self check, this is illegal
			if (isRajaAttackedInLastMove(selfPlayer, chal.to)) {
				undoChalNoCheck(chal);
				return false;
			}
		}

		if (isRajaAttackedInLastMove(opponentPlayer, chal.to)) {
			checked = opponentPlayer;
		}

		// We are avoiding check mate check for optimization.
		// If checkmate there will not be more move left.
		// Hence it will be marked automatically at fill moves.
		/*if (checked == opponentPlayer) {
			const PieceEntry *pieceEntry = getRaja(opponentPlayer);
			tempMoves.clear();
			chalFactory.GetMohraChal(mohra_t::Raja).addChal(pieceEntry, tempMoves);
			bool checkmate = true;
			for (Chal &chal1 : tempMoves) {
				applyChalNoCheck(chal1);
				bool isCheck = chalFactory.isAttacked(chal1.to);
				undoChalNoCheck(chal1);
				if (!isCheck) {
					checkmate = false;
					break;
				}
			}
			if (checkmate) {
				this->checkmate = opponentPlayer;
			}
		}*/

		currentPlayer = opponentPlayer;
		history.push_back_uncheck(chal);
		return true;
	}

	bool Ashtapada::undoChalNoValidation()
	{
		assert(!history.empty());
		if (history.empty()) return false;
		const Chal chal = history.back_uncheck();
		checkmate = player_t::none; // Always reset checkmate, otherwise next move was not possible.

		undoChalNoCheck(chal);
		checked = chal.previousCheck;

		currentPlayer = currentPlayer.opponent();
		assert(currentPlayer == chal.atFrom.getPlayer());
		history.pop_back_uncheck();
		return true;
	}

	bool Ashtapada::findValidMoves()
	{
		tempAllMoves.clear();
		validMoves.clear();
		if (checkmate != player_t::none) return false;
		fillAllMoves(tempAllMoves);
		for (const Chal &chal : tempAllMoves) {
			if (applyChalNoValidation(chal)) {
				validMoves.push_back_uncheck(chal);
				undoChalNoValidation();
			}
		}
		if (validMoves.empty()) checkmate = getCurrentPlayer();
		return !validMoves.empty();
	}

	bool Ashtapada::isRajaAttackedInLastMove(const player_t playerOfRaja, const Position & movedTo) const
	{
		bool isAttacked;
		const _Entry *pieceEntry = getEntry(movedTo);

		if (pieceEntry->val.getPlayer() == playerOfRaja) {
			// Raja has moved check for attack.
			if (pieceEntry->val.getMohra() == mohra_t::Raja) {
				isAttacked = chalFactory.isAttacked(movedTo); // Find if this move results into an attack
			}
			else {
				// Piece has moved that can result into attacked by Ratha
				const Position pos = getRaja(playerOfRaja).position;
				isAttacked = chalFactory.isAttacked(pos, mohra_t::Ratha);
#if DEBUG_RAJA_CAPTURE_VERIFY == 1
				bool isAttackedVerify = chalFactory.isAttacked(pos);
				if (isAttacked != isAttackedVerify) {
					std::string str = "Bad Raja attack calculation, total attacked: " + chalFactory.stringWhoAttacked(pos);
					throw new std::exception(str.c_str());
				}
#endif
			}
		}
		else {
			const Position pos = getRaja(playerOfRaja).position;
			isAttacked = chalFactory.isAttacked(movedTo, pos) || chalFactory.isAttacked(pos, mohra_t::Ratha);
#if DEBUG_RAJA_CAPTURE_VERIFY == 1
			bool isAttackedVerify = chalFactory.isAttacked(pos);
			if (isAttacked != isAttackedVerify) {
				throw new std::exception("Bad Raja attack calculation 2");
			}
#endif
		}

		return isAttacked;
	}


	Ashtapada::~Ashtapada() {}

	const Chal Ashtapada::getChal(const Position & from, const Position & to)
	{
		return Chal(from, to, *this);
	}

	int Ashtapada::validateChalWithIndex(const Position from, const Position to)
	{
		for (int index = 0; index < validMoves.size(); index++) {
			const Chal &chal = validMoves[index];
			if (chal.from == from && chal.to == to) {
				return index;
			}
		}
		return -1;
	}

	bool Ashtapada::validateChal(const Position from, const Position to)
	{

		return validateChalWithIndex(from, to) != -1;
	}

	bool Ashtapada::applyChal(const Position from, const Position to)
	{
		int index = validateChalWithIndex(from, to);
		if (index == -1) return false;

		const Chal chal = validMoves[index];

		if (!applyChalNoValidation(chal)) return false;

		if (!findValidMoves()) {
			checkmate = getCurrentPlayer();
		}
		return true;
	}

	bool Ashtapada::undoChal()
	{
		// Only reason this can fail is no history present
		if (!undoChalNoValidation()) return false;

		bool validMove = findValidMoves();
		assert(validMove);

		return true;
	}

	void Ashtapada::fillAllMoves(MohraChal::StoreType & moves)
	{
		moves.clear();
		chalFactory.FillAllChal(moves, currentPlayer);
	}

	bool Ashtapada::apply(const char *str, const player_t currentPlayer)
	{
		reset();
		checkmate = player_t::none;

		//String is top down, board is bottom up
		for (row_t row = row_t::row_last; row >= 0_row; --row) {
			for (col_t col = 0_col; col <= col_t::col_last; ++col) {
				const piece_t piece = piece_t::getPiece(*(str++));
				if (piece == piece_t::Unknown) return false;
				if (piece != piece_t::Empty) {
					const Position pos(row, col);
					if (!addPiece(piece, pos)) {
						reset();
						return false;
					}
				}
			}
		}
		this->currentPlayer = currentPlayer;
		findValidMoves();
		return true;
	}

	// Count will be 1 less i.e. 0 = 1 and 7 = 8 and 15 = 16

	const std::string Ashtapada::DEFAULT =
		"cagrsgac"
		"pppppppp"
		"........"
		"........"
		"........"
		"........"
		"PPPPPPPP"
		"CAGSRGAC";

	const size_t Ashtapada::DEFAULTLength = sizeof(Ashtapada::DEFAULT) / sizeof(Ashtapada::DEFAULT[0]);

	std::ostream& operator<<(std::ostream& os, const MohraChal::StoreType &moves) {
		for (size_t i = 0; i < moves.size(); i++) {
			os << i << ":" << moves[i] << "; ";
		}
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const Ashtapada::HistoryType &moves) {
		for (size_t i = 0; i < moves.size(); i++) {
			os << i << ":" << moves[i] << "; ";
		}
		return os;
	}

	std::ostream & operator<<(std::ostream & os, const Ashtapada & ashtapada)
	{
		if (ashtapada.display_flag & ashtapada.display_board) os << static_cast<Pieces>(ashtapada);
		if (ashtapada.display_flag & ashtapada.display_history) os << "History:: " << ashtapada.history << std::endl;
		if (ashtapada.display_flag & ashtapada.display_piecelist) {
			for (player_t player = player_t::begin; player != player_t::end; player++) {
				for (mohra_t mohraIndex = mohra_t::begin; mohraIndex != mohra_t::end; mohraIndex++) {
					const piece_t piece(mohraIndex, player);
					os << piece << "::" << ashtapada[piece] << std::endl;
				}
			}
		}
		if (ashtapada.display_flag & ashtapada.display_captured) {
			os << "Captured: {";
			for (player_t player = player_t::begin; player != player_t::end; player++) {
				for (mohra_t mohraIndex = mohra_t::begin; mohraIndex != mohra_t::end; mohraIndex++) {
					const piece_t piece(mohraIndex, player);
					const Pieces::list &list = ashtapada.getCaptured(piece);
					for (const PieceEntry &entry : list) {
						os << static_cast<piece_t>(entry) << ' ';
					}
				}
			}
			os << '}' << std::endl;
		}
		if (ashtapada.display_flag & ashtapada.display_summary) {
			if (ashtapada.checkmate != player_t::none) os << "CHECKMATE:" << ashtapada.checkmate << "; ";
			else if (ashtapada.checked != player_t::none) os << "CHECK:" << ashtapada.checked << "; ";
			os << "Played:" << ashtapada.currentPlayer;
			if (!ashtapada.history.empty()) os << ",Last:" << ashtapada.history.back_uncheck() << std::endl;
			else os << ",";
			os << "Moves:" << ashtapada.getMoveCount() << "; ";
			for (player_t player = player_t::begin; player != player_t::end; player++) {
				int activeCount = 0;
				int capturedCount = 0;
				for (mohra_t mohraIndex = mohra_t::begin; mohraIndex != mohra_t::end; mohraIndex++) {
					const piece_t piece(mohraIndex, player);
					const Pieces::list &capturedList = ashtapada.getCaptured(piece);
					for (const PieceEntry &entry : capturedList) ++capturedCount;

					const Pieces::list &activeList = ashtapada.getActive(piece);
					for (const PieceEntry &entry : activeList) ++activeCount;
				}
				assert(activeCount == ashtapada.activeCount[player]);
				os << player << "{Active:" << activeCount << ",Captured:" << capturedCount << "} ";
			}
			os << std::endl;
		}

		return os;
	}
}
