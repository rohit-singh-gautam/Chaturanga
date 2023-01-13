#pragma once
#include "Position.h"
#include "LinkedList.h"
#include "ReleaseCheck.h"
#include <cassert>
#include <string>
#include <string_view>

namespace rohit {

	class player_t {
	public:
		enum player_internal_t : player_storate_type {
			begin = 0,
			first = begin,
			second,
			max,
			end = max,
			unknown = max,
			none
		};

	private:
		player_storate_type value;
		static constexpr std::string_view displayString[] {
			"First",
			"Second",
			"Unknown",
			"None",
			"Invalid"
		};
		static constexpr int signArray[] { -1, 1, 0, 0, 0 };

		constexpr size_t getIndex() const { return static_cast<size_t>(value); }

	public:
		constexpr player_t(unsigned char value) : value(static_cast<player_internal_t>(value)) {}
		constexpr player_t(const player_t &player) : value(player.value) {}

		constexpr player_t &operator++() { ++value; return *this; }
		constexpr player_t operator++(int) { player_t temp = *this; ++*this; return temp; }
		constexpr player_t &operator--() { --value; return *this; }
		constexpr player_t operator--(int) { player_t temp = *this; --*this; return temp; }

		constexpr bool operator==(const player_t &rhs) const { return value == rhs.value; }
		constexpr bool operator!=(const player_t &rhs) const { return value != rhs.value; }
		constexpr bool operator==(const player_internal_t rhs) const { return value == rhs; }
		constexpr bool operator!=(const player_internal_t rhs) const { return value != rhs; }

		constexpr player_t& operator=(const player_t& rhs) { value = rhs.value; return *this; }

		constexpr operator player_internal_t() const { return static_cast<player_internal_t>(value); }
		//inline operator unsigned char() const { return static_cast<unsigned char>(value & 0x01); }


		constexpr player_t opponent() const {
			switch (value) {
			case player_internal_t::first:
				return player_internal_t::second;
			case player_internal_t::second:
				return player_internal_t::first;
			default:
				return player_internal_t::unknown;
			}
		}
		constexpr bool verify() const { return value == player_internal_t::first || value == player_internal_t::second; }

		constexpr int getSign() const { return signArray[getIndex()]; }
		constexpr std::string_view to_string() const { return displayString[getIndex()]; }

		friend class piece_t;
		friend std::ostream& operator<<(std::ostream& os, const player_t player);
	};

	class mohra_t {
	public:
		enum mohra_internal_t : mohra_storate_type {
			begin = 0,
			Raja = begin,
			Senapati,
			Ratha,
			Gaja,
			Ashva,
			Padati,
			Empty,
			end = Empty,
			max = end
		};

	private:
		mohra_storate_type value;
		constexpr static char CHAR[] { 'R', 'S', 'C', 'G', 'A', 'P', '\0' };
		static constexpr std::string_view displayString[] {
			"Raja",
			"Senapati",
			"Ratha",
			"Gaja",
			"Ashva",
			"Padati",
			"Empty",
			"Invalid"
		};

		constexpr mohra_t(unsigned char value) : value(static_cast<mohra_storate_type>(value)) {}

	public:
		constexpr mohra_t(const mohra_internal_t value) : value(value) {}
		constexpr mohra_t(const mohra_t &mohra) : value(mohra.value) {}

		constexpr mohra_t &operator++() { ++value; return *this; }
		constexpr mohra_t operator++(int) { mohra_t temp = *this; ++*this; return temp; }
		constexpr mohra_t &operator--() { --value; return *this; }
		constexpr mohra_t operator--(int) { mohra_t temp = *this; --*this; return temp; }

		constexpr bool operator==(const mohra_t rhs) const { return value == rhs.value; }
		constexpr bool operator!=(const mohra_t rhs) const { return value != rhs.value; }
		constexpr bool operator==(const mohra_internal_t rhs) const { return value == rhs; }
		constexpr bool operator!=(const mohra_internal_t rhs) const { return value != rhs; }

		constexpr operator mohra_internal_t() const { return static_cast<mohra_internal_t>(value); }

		constexpr bool verify() const { return value >= begin && value < end; }
		constexpr char getChar() const { if (value < begin || value >= end) return '\0'; else return CHAR[static_cast<int>(value)]; }

		constexpr std::string_view to_string() const { return displayString[static_cast<int>(value)]; }

		friend class piece_t;
	};

	class bitmohra_t {
		bitmohra_internal_t value;

		constexpr static bitmohra_internal_t to_bitmohra_internal(const mohra_t mohra) { return 1 << mohra; }
	public:
		constexpr bitmohra_t(const bitmohra_internal_t value) : value(value) {}
		constexpr bitmohra_t(const mohra_t &mohra) : value(to_bitmohra_internal(mohra)) {}

		constexpr bitmohra_t &operator++() { value <<= 1; return *this; }
		constexpr bitmohra_t operator++(int) { bitmohra_t temp = *this; ++*this; return temp; }
		constexpr bitmohra_t &operator--() { value >>= 1; return *this; }
		constexpr bitmohra_t operator--(int) { bitmohra_t temp = *this; --*this; return temp; }
		constexpr const bitmohra_t operator&(const bitmohra_t rhs) const { return value & rhs.value; }
		constexpr const bitmohra_t operator|(const bitmohra_t rhs) const { return value | rhs.value; }
		constexpr const bitmohra_t operator&(const mohra_t rhs) const { return value & to_bitmohra_internal(rhs); }
		constexpr bool operator==(const mohra_t rhs) const { return value == to_bitmohra_internal(rhs); }
		constexpr bool contains(const mohra_t rhs) const { return (value & to_bitmohra_internal(rhs)) != Empty; }

		static constexpr bitmohra_internal_t Empty { 0 };
		static constexpr bitmohra_internal_t Raja { 1 };
		static constexpr bitmohra_internal_t Senapati {2 };
		static constexpr bitmohra_internal_t Ratha { 4 };
		static constexpr bitmohra_internal_t Gaja { 8 };
		static constexpr bitmohra_internal_t Ashva { 16 };
		static constexpr bitmohra_internal_t Padati { 32 };
		static constexpr bitmohra_internal_t End { 64 };
	};

	struct PieceCompressedMultiple;

	class piece_t {
	public:
		enum piece_internal_t : piece_storate_type {
			begin = 0,
			firstRaja = begin,
			secondRaja,
			firstSenapati,
			secondSenapati,
			firstRatha,
			secondRatha,
			firstGaja,
			secondGaja,
			firstAshva,
			secondAshva,
			firstPadati,
			secondPadati,
			Empty,
			end = Empty,
			max = end,
			Unknown
		};

	protected:
		//mohra_t mohra;
		//player_t player; // first: 0, second: 1
		piece_storate_type value;
		static constexpr piece_storate_type toInternal(const mohra_t mohra, const player_t player) { return player + (mohra << 1);  }
		static constexpr std::string_view displayString[] {
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

	public:
		constexpr piece_t() : value(Empty) {}
		constexpr piece_t(const piece_internal_t value) : value(value) {}
		constexpr piece_t(const mohra_t mohra, player_t player) : value(toInternal(mohra, player)) {}
		constexpr piece_t(const piece_t &piece) : value(piece.value) {}

		constexpr player_t getPlayer() const { return value & 0x01; }
		constexpr mohra_t getMohra() const { return value >> 1; }

		constexpr piece_t &operator++() { ++value; return *this; }
		constexpr piece_t operator++(int) { piece_t temp = *this; ++*this; return temp; }
		constexpr piece_t &operator--() { --value; return *this; }
		constexpr piece_t operator--(int) { piece_t temp = *this; --*this; return temp; }

		constexpr bool operator==(const piece_t rhs) const { return value == rhs.value; }
		constexpr bool operator!=(const piece_t rhs) const { return value != rhs.value; }
		constexpr bool operator==(const piece_internal_t rhs) const { return value == rhs; }
		constexpr bool operator!=(const piece_internal_t rhs) const { return value != rhs; }

		constexpr piece_t& operator=(const piece_t& rhs) { value = rhs.value; return *this; }

		constexpr operator piece_internal_t() const { return static_cast<piece_internal_t>(value); }

		constexpr bool verify() const { return value < 12; }

		constexpr static const piece_t getPiece(const char c) {
			switch (c) {
			case 'R': return firstRaja;
			case 'r': return secondRaja;
			case 'S': return firstSenapati;
			case 's': return secondSenapati;
			case 'C': return firstRatha;
			case 'c': return secondRatha;
			case 'G': return firstGaja;
			case 'g': return secondGaja;
			case 'A': return firstAshva;
			case 'a': return secondAshva;
			case 'P': return firstPadati;
			case 'p': return secondPadati;
			case ' ': case '.': return Empty;
			default: return Unknown;
			}
		}
	};


	std::ostream& operator<<(std::ostream& os, const piece_t piece);

	class  PieceEntry : public piece_t {
	public:
		Position position; //Non const here

		inline PieceEntry() : position(Position::Captured) {}
		inline void reset() { position = Position::Captured; }

		inline PieceEntry operator=(const piece_t piece) { this->value = piece; return *this; }

		friend std::ostream& operator<<(std::ostream& os, const PieceEntry &pieceEntry);
	};

	class PieceList {
	public:
		typedef struct fixed_list_entry<PieceEntry> _Entry;
		typedef class fixed_list_simple<PieceEntry> list;

	private:
		list activeList;
		list capturedList;

	public:
		PieceList() {}

		inline list &getActive() { return activeList; }
		inline const list &getActive() const { return activeList; }

		inline list &getCaptured() { return capturedList; }
		inline const list &getCaptured() const { return capturedList; }

		inline void moveToActive(list::iterator it)
		{
			assert(it->position.verify());
			assert(it->verify());
			activeList.to_begin(it);
		}
		inline void insertInActive(_Entry *entry) { activeList.push_front(entry); }
		inline void moveToCaptured(list::iterator it) {
			assert(it->position == Position::Captured);
			assert(it->verify());
			capturedList.to_begin(it);
		}
		void insertInCaptured(_Entry *entry) { capturedList.push_front(entry); }

		inline void freeAll() { activeList.clear(); capturedList.clear(); }

		friend std::ostream& operator<<(std::ostream& os, const PieceList &pieceList);
	};

	class Pieces {
	private:
		static constexpr mohra_t initialColMohra[col_max] //Used only for padati promotion
			{ mohra_t::Ratha, mohra_t::Ashva, mohra_t::Gaja, mohra_t::Senapati, mohra_t::Senapati, mohra_t::Gaja, mohra_t::Ashva, mohra_t::Ratha };
		static constexpr col_t initialCol[mohra_t::max][2] {
			{ 4_col, col_t::col_unknown },				// Raja
			{ 3_col, 4_col },							// Senapati, marking both the center
			{ 0_col, 7_col },							// Ratha
			{ 2_col, 5_col },							// Gaja
			{ 1_col, 6_col },							// Ashva
			{ col_t::col_unknown, col_t::col_unknown },	// Padati
		};

	public:
		static constexpr int block_count = 64;
		static constexpr int piece_count = 32;
		static constexpr int row_size = 8;
		static constexpr int col_size = 8;

		inline static const mohra_t getInitialColMohra(col_t col) {
			return initialColMohra[static_cast<size_t>(col)];
		}

		inline static col_t getInitialCol(const mohra_t mohra, int index) {
			return initialCol[mohra][index];
		}

		static constexpr int MaxPossibleMohra[piece_t::max] { 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 8, 8 };

		typedef struct fixed_list_entry<PieceEntry> _Entry;
		typedef class fixed_list_simple<PieceEntry> list;

	protected:
		_Entry *blocks[block_count];
		_Entry pieceEntryAllocation[piece_count];
		PieceList pieceMap[piece_t::max];
		int activeCount[player_t::max];
		player_t checked;

		inline _Entry *getEntry(const Position pos) { 
			auto index = pos.getIndex();
#if DEBUG_PIECES_INDEX == 1
			if (index >= block_count) throw std::runtime_error("index beyond range");
#endif
			return blocks[index];
		}
		inline const _Entry *getEntry(const Position pos) const {
			auto index = pos.getIndex();
#if DEBUG_PIECES_INDEX == 1
			if (index >= block_count) throw std::runtime_error("index beyond range");
#endif
			return blocks[index];
		}
		inline void setEntry(const Position pos, _Entry *entry) {
			auto index = pos.getIndex();
#if DEBUG_PIECES_INDEX == 1
			if (index >= block_count) throw std::runtime_error("index beyond range");
			if (entry != nullptr && (entry < pieceEntryAllocation || entry > pieceEntryAllocation + piece_count - 1)) {
				if (entry < pieceEntryAllocation) {
					std::cout << "Underflow" << std::endl;
					throw std::runtime_error("wrong entry under flow");
				}
				if (entry < pieceEntryAllocation + piece_count) {
					std::cout << "Overflow" << std::endl;
					throw std::runtime_error("wrong entry overflow flow");
				}
				std::cout << "Must not reach here" << std::endl;
				throw std::runtime_error("wrong entry calculation error");
			}
#endif
			blocks[index] = entry;
		}

		inline PieceList &getMap(const piece_t piece) {
#if DEBUG_PIECEMAP_INDEX == 1
			if (piece >= piece_t::end) {
				throw std::runtime_error("Piece range exceeeds limit.");
			}
#endif
			return pieceMap[piece]; 
		}
		inline const PieceList &getMap(const piece_t piece) const {
#if DEBUG_PIECEMAP_INDEX == 1
			if (piece >= piece_t::end) {
				throw std::runtime_error("Piece range exceeeds limit.");
			}
#endif
			return pieceMap[piece];
		}

		inline bool hasCaptured(const piece_t piece) const { return !getCaptured(piece).isEmpty(); }
		inline list &getCaptured(const piece_t piece) { return getMap(piece).getCaptured(); }
		inline const list &getCaptured(const piece_t piece) const { return getMap(piece).getCaptured(); }

		inline bool hasActive(const piece_t piece) const { return getActive(piece).isEmpty(); }
		inline list &getActive(const piece_t piece) { return getMap(piece).getActive(); }
		inline const list &getActive(const piece_t piece) const { return getMap(piece).getActive(); }

		inline const PieceEntry &getRaja(const player_t player) const {
			const list &activeList = getMap(piece_t(mohra_t::Raja, player)).getActive();
			assert(activeList.begin() != activeList.end());
			auto &entry = activeList.front();
			assert(entry.verify());
			assert(entry.getMohra() == mohra_t::Raja);
			return entry;
		}

		void reset(); //Clears entire board
		bool addPiece(const piece_t piece, const Position position);
		inline void markCaptured(list::iterator it) {
			it->position = Position::Captured;
			--activeCount[it->getPlayer()];
			getMap(*it).moveToCaptured(it);
		}

		inline void markCaptured(const Position position) {
			_Entry *originalPiece = getEntry(position);
			if (originalPiece != nullptr) {
				markCaptured(originalPiece);
				setEntry(position, nullptr);
			}
		}

		inline void moveCaptureTo(list::iterator it, const Position position) {
			markCaptured(position);
			it->position = position;
			setEntry(position, it.getPtr());
			++activeCount[it->getPlayer()];
			getMap(*it).moveToActive(it);
		}

	public:
		Pieces() : blocks(), activeCount(), checked(player_t::none) {}
		inline bool isEmpty(const Position pos) const { return getEntry(pos) == nullptr; }
		inline bool isPlayerAt(const Position pos, const player_t player) const { return getEntry(pos)->val.getPlayer() == player; }

		// inline _Entry *operator[](const Position pos) { return getEntry(pos); }
		inline _Entry const * operator[](const Position pos) const { return getEntry(pos); }
		inline PieceList &operator[](const piece_t piece) { return getMap(piece); }
		inline const PieceList &operator[](const piece_t piece) const { return getMap(piece); }
		inline const player_t isChecked() const { return checked; }

		friend class AshtapadaCompressed;
		friend class PadatiPromotion;

		friend std::ostream& operator<<(std::ostream& os, const Pieces &pieces);
	};

	std::ostream& operator<<(std::ostream& os, const Pieces &pieces);
}
