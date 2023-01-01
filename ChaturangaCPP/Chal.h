#pragma once
#include "pieces.h"
#include "Store.h"
#include "ReleaseCheck.h"

namespace rohit {

	class PadatiPromotion {
		union {
			struct {
				promotion_compressed player : 4;
				promotion_compressed col : 4;

			};
			promotion_compressed value;
		};

	public:
		static const PadatiPromotion findSelfPadatiPromotion(const Position to, const piece_t atFrom, const Pieces & pieces);
		static const PadatiPromotion findOpponentPadatiPromotion(const piece_t capturedList, const Pieces & pieces);
		inline static const row_t getLastRow(const player_t player) { return player == player_t::first ? 7_row : 0_row; }
		inline static const mohra_t getPromotedMohra(const col_t col) { return Pieces::getInitialColMohra(col); }

		static const PadatiPromotion None;

		inline PadatiPromotion(const player_t player, const col_t col) : player(player), col(static_cast<promotion_compressed>(col)) { }

		inline const col_t getCol() const { return static_cast<col_t>(col); }
		inline const player_t getPlayer() const { return player;  }
		inline const row_t getLastRow() const { return getLastRow(getPlayer()); }
		inline const Position getPosition() const { return { getLastRow(),  getCol() }; }
		inline const mohra_t getPromotedMohra() const { return Pieces::getInitialColMohra(getCol()); }
		inline const piece_t getPromotedPiece() const { return { getPromotedMohra(), getPlayer() }; }
		inline const piece_t getRemovedPadati() const { return { mohra_t::Padati, getPlayer() }; }

		inline bool operator==(const PadatiPromotion &rhs) const { return value == rhs.value; }
		inline bool operator!=(const PadatiPromotion &rhs) const { return value != rhs.value; }

	};

	//struct ChalCompressed;

	struct Chal {
		Position from;
		Position to;
		piece_t atFrom;
		piece_t capturedList;
		PadatiPromotion selfPromotion;
		PadatiPromotion opponentPromotion;
		player_t previousCheck;

		inline Chal() : from(Position::Captured), to(Position::Captured), atFrom(piece_t::Unknown), capturedList(piece_t::Unknown),
			selfPromotion(PadatiPromotion::None), opponentPromotion(PadatiPromotion::None), previousCheck(player_t::none) {}

		/*Chal(const Chal &chal) : from(chal.from), to(chal.to), atFrom(chal.atFrom), captured(chal.captured),
			selfPromotion(chal.selfPromotion), opponentPromotion(chal.opponentPromotion), previousCheck(chal.previousCheck) {} */

		inline Chal(const Position &from, const Position &to, const Pieces &pieces) : from(from), to(to),
			atFrom(pieces[from] != nullptr ? pieces[from]->val : piece_t::Empty), capturedList(pieces[to] != nullptr ? pieces[to]->val : piece_t::Empty),
			selfPromotion(PadatiPromotion::findSelfPadatiPromotion(to, atFrom, pieces)),
			opponentPromotion(PadatiPromotion::findOpponentPadatiPromotion(capturedList, pieces)), previousCheck(pieces.isChecked()) {
#if DEBUG_RAJA_CAPTURE == 1
			if (capturedList.getMohra() == mohra_t::Raja) {
				throw std::runtime_error("Raja cannot be captured");
			}
#endif
		}
	};

	std::ostream& operator<<(std::ostream& os, const Chal &pos);

	class MohraChal {
	private:
		static const bitmohra_t unsafe[15][15];

	public:
		static constexpr const int max_possible_chal =
			8 /*Raja*/ + 4 /*Senapati*/ + 14*2 /*Ratha*/ + 4*2 /*Gaja*/ + 8*2 /*Ashva*/ + 3*8 /*Padati*/ + 8*2 /*Extra buffer*/;
		using StoreType = SimpleStore<Chal, max_possible_chal>;

	protected:
		const Pieces &pieces;
		bool addValidEmptyCapture(const Position oldPos, const DiffPosition diffPos, StoreType &moves, const player_t player) const;
		bool addValidEmpty(const Position oldPos, const DiffPosition diffPos, StoreType &moves, const player_t player) const;
		bool addValidCapture(const Position oldPos, const DiffPosition diffPos, StoreType &moves, const player_t player) const;

		static const Position unsafeCenter;
		inline bitmohra_t getUnsafe(row_t row, col_t col) const {
			return unsafe[static_cast<size_t>(row)][static_cast<size_t>(col)];
		}
		bool isAttackedHelper(const mohra_t by, const Position to) const;

	public:
		MohraChal(const Pieces &pieces);
		virtual void addChal(const PieceEntry &entry, StoreType &moves) const = 0;
		inline void addChal(const PieceList &positionList, StoreType &moves) const {
			const PieceList::list &activeList = positionList.getActive();
			for (const PieceEntry &entry: activeList) addChal(entry, moves);
		}
		virtual bool validateChal(const Chal &chal) const = 0;
		virtual bool isAttacked(const Position to) const = 0;
	};

	class RajaChal : public MohraChal {
		const static DiffPosition posList[];

	public:
		RajaChal(const Pieces &pieces);

		void addChal(const PieceEntry &entry, StoreType &moves) const override;
		bool validateChal(const Chal &chal) const override;
		bool isAttacked(const Position to) const override;
	};

	class SenapatiChal : public MohraChal {
		const static DiffPosition posList[];

	public:
		SenapatiChal(const Pieces &pieces);

		void addChal(const PieceEntry &entry, StoreType &moves) const override;
		bool validateChal(const Chal &chal) const override;
		bool isAttacked(const Position to) const override;
	};

	class RathaChal : public MohraChal {
	public:
		RathaChal(const Pieces &pieces);

		void addChal(const PieceEntry &entry, StoreType &moves) const override;
		bool validateChal(const Chal &chal) const override;
		bool isAttacked(const Position to) const override;
	};

	class GajaChal : public MohraChal {
		const static DiffPosition posList[];

	public:
		GajaChal(const Pieces &pieces);

		void addChal(const PieceEntry &entry, StoreType &moves) const override;
		bool validateChal(const Chal &chal) const override;
		bool isAttacked(const Position to) const override;
	};

	class AshvaChal : public MohraChal {
		const static DiffPosition posList[];

	public:
		AshvaChal(const Pieces &pieces);

		void addChal(const PieceEntry &entry, StoreType &moves) const override;
		bool validateChal(const Chal &chal) const override;
		bool isAttacked(const Position to) const override;
	};

	class PadatiChal : public MohraChal {
		const static DiffPosition capturePosLists[2][2];
		const static DiffPosition posList[];

	public:
		PadatiChal(const Pieces &pieces);

		void addChal(const PieceEntry &entry, StoreType &moves) const override;
		bool validateChal(const Chal &chal) const override;
		bool isAttacked(const Position to) const override;
	};

	class ChalFactory {
		static const int MaxPossibleMoves = 500;
		const Pieces &pieces;

		RajaChal rajaChal;
		SenapatiChal senapatiChal;
		RathaChal rathaChal;
		GajaChal gajaChal;
		AshvaChal ashvaChal;
		PadatiChal padatiChal;
		MohraChal *allChal[mohra_t::max];

	public:
		ChalFactory(const Pieces &pieces) : pieces(pieces),
			rajaChal(pieces), senapatiChal(pieces), rathaChal(pieces), gajaChal(pieces), ashvaChal(pieces), padatiChal(pieces),
			allChal{ &rajaChal, &senapatiChal, &rathaChal, &gajaChal, &ashvaChal, &padatiChal } {

		}

		inline const MohraChal & GetMohraChal(const mohra_t mohra) const {
			return *allChal[mohra];
		}

		inline void FillAllChal(MohraChal::StoreType &moves, const piece_t &piece) {
			const MohraChal &mohraChal = GetMohraChal(piece.getMohra());
			const PieceList &pieceList = pieces[piece];
			mohraChal.addChal(pieceList, moves);
		}

		inline void FillAllChal(MohraChal::StoreType &moves, player_t player) {
			FillAllChal(moves, piece_t(mohra_t::Raja, player));
			FillAllChal(moves, piece_t(mohra_t::Senapati, player));
			FillAllChal(moves, piece_t(mohra_t::Ratha, player));
			FillAllChal(moves, piece_t(mohra_t::Gaja, player));
			FillAllChal(moves, piece_t(mohra_t::Ashva, player));
			FillAllChal(moves, piece_t(mohra_t::Padati, player));
		}

		inline bool isAttacked(const Position to) const {
			assert(to.verify());
			return isAttacked(to, mohra_t::Raja) ||
				isAttacked(to, mohra_t::Senapati) ||
				isAttacked(to, mohra_t::Ratha) ||
				isAttacked(to, mohra_t::Gaja) ||
				isAttacked(to, mohra_t::Ashva) ||
				isAttacked(to, mohra_t::Padati);
		}

		inline std::string stringWhoAttacked(const Position to) const {
			std::string str = "Attacked by: ";

			if (isAttacked(to, mohra_t::Raja)) {
				str += "Raja,";
			}
			if (isAttacked(to, mohra_t::Senapati)) {
				str += "Senapati,";
			}
			if (isAttacked(to, mohra_t::Ratha)) {
				str += "Ratha,";
			}
			if (isAttacked(to, mohra_t::Gaja)) {
				str += "Gaja,";
			}
			if (isAttacked(to, mohra_t::Ashva)) {
				str += "Ashva,";
			}
			if (isAttacked(to, mohra_t::Padati)) {
				str += "Padati,";
			}

			return str;
		}

		inline bool isAttacked(const Position to, const mohra_t mohra) const {
			assert(to.verify());
			assert(mohra.verify());
			const MohraChal &mohraChal = GetMohraChal(mohra);
			return mohraChal.isAttacked(to);
		}
		inline bool isAttacked(const Position from, const Position to) const {
			return isAttacked(to, pieces[from]->val.getMohra());
		}
	};
}

