#pragma once
#include "pieces.h"
#include "Chal.h"

namespace rohit {

	class Ashtapada : public Pieces
	{
	public:
		static constexpr std::string_view DEFAULT {
			"cagrsgac"
			"pppppppp"
			"........"
			"........"
			"........"
			"........"
			"PPPPPPPP"
			"CAGSRGAC"
		};
		static constexpr size_t max_chal = 500;

		using HistoryType = SimpleStore<Chal, max_chal>;

	private:
		HistoryType history;
		ChalFactory chalFactory;
		player_t checkmate;
		player_t currentPlayer;

		// Optimizing allocations

		MohraChal::StoreType tempMoves;
		MohraChal::StoreType tempAllMoves;
		MohraChal::StoreType validMoves;

		void promotePadati(const PadatiPromotion &padatiPromotion, const player_t player);
		void undoPromotePadati(const PadatiPromotion &padatiPromotion, const player_t player);

		//Apply and Undo must go in order, unorder operation will make board inconsistent
		void applyChalNoCheck(const Chal &chal);
		void undoChalNoCheck(const Chal &chal);

		bool isRajaAttackedInLastMove(const player_t player, const Position &to) const;
		bool applyChalNoValidation(const Chal &chal);
		bool undoChalNoValidation();

		bool findValidMoves();
		const Chal getChal(const Position &from, const Position &to);
		int validateChalWithIndex(const Position from, const Position to);

		// Internal this one is for optimization
		bool apply(const char *str, const player_t currentPlayer);

	public:
		// Initialization
		inline Ashtapada() : history(), chalFactory(*this), checkmate(player_t::none), currentPlayer(player_t::first) {
			findValidMoves();
		}
		~Ashtapada();

		inline bool apply(const std::string_view &str, const player_t currentPlayer) {
			if (str.size() != Pieces::block_count) return false;
			return apply(str.data(), currentPlayer);
		}

		inline bool apply(const std::string &str, const player_t currentPlayer) {
			if (str.size() != Pieces::block_count) return false;
			return apply(str.c_str(), currentPlayer);
		}

		//Statis
		inline const player_t isCheckmate() const { return checkmate; }
		inline const player_t getCurrentPlayer() const { return currentPlayer; }
		inline size_t getMoveCount() const { return history.size(); }

		bool validateChal(const Position from, const Position to);

		//bool applyChal(const Chal &chal);
		bool applyChal(const Position from, const Position to);
		bool undoChal();

		// Moves can generate checks, hence can be invalid.
		// For optimization reason we do not check this till moves are applied.
		// It will clear moves before filling in.
		void fillAllMoves(MohraChal::StoreType& moves);
		friend std::ostream & operator<<(std::ostream & os, const Ashtapada &ashtapada);
		friend class Niti;
		friend class AshtapadaCompressed;

		typedef int display_flag_type;
		static const int
			display_board = 1,
			display_history = 2,
			display_piecelist = 4,
			display_captured = 8,
			display_summary = 16;

		display_flag_type display_flag = display_board | display_history | display_piecelist | display_captured | display_summary;
	};

	std::ostream & operator<<(std::ostream & os, const Ashtapada &ashtapada);
	std::ostream& operator<<(std::ostream& os, const MohraChal::StoreType &moves);
	std::ostream& operator<<(std::ostream& os, const Ashtapada::HistoryType &moves);

	class Niti {
	protected:
		Ashtapada &ashtapada;

		inline bool applyChalNoValidation(const Chal &chal) { return ashtapada.applyChalNoValidation(chal); }
		inline bool undoChalNoValidation() { return ashtapada.undoChalNoValidation(); }
		inline const MohraChal::StoreType &getValidMoves() const { return ashtapada.validMoves; }

	public:
		inline Niti(Ashtapada &ashtapada) : ashtapada(ashtapada) {}
		virtual const Chal getNextMove(niti_depth_type depth = -1) = 0;
	};
}