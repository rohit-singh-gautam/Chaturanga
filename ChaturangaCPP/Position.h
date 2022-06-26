#pragma once
#include "storage.h"
#include <iostream>

namespace rohit {

	enum class row_t : row_storage_type {
		row_unknown = -8,
		row_rend = -1,
		row_start = 0,
		row_rstart = 7, row_last = 7, row_max = 8
	};

	constexpr size_t row_max = 8;

	enum class col_t : col_storage_type {
		col_unknown = -8,
		col_start = 0,
		col_last = 7, col_max = 8
	};

	constexpr size_t col_max = 8;

	constexpr row_t operator "" _row(unsigned long long val) {
		return static_cast<row_t>(val);
	}

	constexpr col_t operator "" _col(unsigned long long val) {
		return static_cast<col_t>(val);
	}

	constexpr row_t operator+(row_t lhs, row_t rhs) { return static_cast<row_t>(static_cast<int>(lhs) + static_cast<int>(rhs)); }
	constexpr col_t operator+(col_t lhs, col_t rhs) { return static_cast<col_t>(static_cast<int>(lhs) + static_cast<int>(rhs)); }
	constexpr row_t operator-(row_t lhs, row_t rhs) { return static_cast<row_t>(static_cast<int>(lhs) - static_cast<int>(rhs)); }
	constexpr col_t operator-(col_t lhs, col_t rhs) { return static_cast<col_t>(static_cast<int>(lhs) - static_cast<int>(rhs)); }
	constexpr row_t operator-(row_t val) { return static_cast<row_t>(-static_cast<int>(val)); }
	constexpr col_t operator-(col_t val) { return static_cast<col_t>(-static_cast<int>(val)); }
	inline row_t& operator++(row_t& self) { return (self = self + 1_row); }
	inline col_t& operator++(col_t& self) { return (self = self + 1_col); }
	inline row_t& operator--(row_t& self) { return (self = self - 1_row); }
	inline col_t& operator--(col_t& self) { return (self = self - 1_col); }
	inline row_t operator++(row_t& self, int) { auto ret = self; self = self + 1_row; return ret; }
	inline col_t operator++(col_t& self, int) { auto ret = self; self = self + 1_col; return ret; }
	inline row_t operator--(row_t& self, int) { auto ret = self; self = self - 1_row; return ret; }
	inline col_t operator--(col_t& self, int) { auto ret = self; self = self - 1_col; return ret; }

	static const row_t row_direction[] = {
		-1_row, -1_row, -1_row, -1_row, -1_row, -1_row, -1_row, 0_row, 1_row, 1_row, 1_row, 1_row, 1_row, 1_row, 1_row
	};

	static const col_t col_direction[] = {
		-1_col, -1_col, -1_col, -1_col, -1_col, -1_col, -1_col, 0_col, 1_col, 1_col, 1_col, 1_col, 1_col, 1_col, 1_col
	};


	class Position {
	public:
		static const Position Captured;

	private:
		union {
			struct {
				row_t row;
				col_t col;
			};
			position_storage_type value;
		};

	public:
		// row_t row; // bottom 0, top 7
		// col_t col; // left 0, right 7

		inline Position() : value(0) {}
		inline Position(const row_t row, const col_t col) : row(row), col(col) {}

		// inline Position(const Position &pos) : row(pos.row), col(pos.col) {} // This is not required.

		inline bool verify() const { return (value & ~0x77) == 0; }

		inline bool operator==(const Position& pos) const { return value == pos.value; }
		inline bool operator!=(const Position& pos) const { return value != pos.value; }

		inline const row_t getRow() const { return static_cast<row_t>(row); }
		inline const col_t getCol() const { return static_cast<col_t>(col); }

		inline const size_t getRowIndex() const { return static_cast<size_t>(row); }
		inline const size_t getColIndex() const { return static_cast<size_t>(col); }

		inline size_t getIndex() const { return ((size_t)col << 3) + (size_t)row; }
		inline static size_t getIndex(row_t row, col_t col) { return ((size_t)col << 3) + (size_t)row; }

	};

	class DiffPosition {
		row_t row;
		col_t col;

	public:

		inline DiffPosition(const row_t row, const col_t col) : row(row), col(col) {}

		inline const row_t getRow() const { return row; }
		inline const col_t getCol() const { return col; }

		inline const DiffPosition getDirectionDiff() const { return { row_direction[static_cast<size_t>(getRow() + 7_row)], col_direction[static_cast<size_t>(getCol() + 7_col)] }; }
	};

	inline const Position operator+(const Position& lhs, const DiffPosition& rhs) { return { lhs.getRow() + rhs.getRow(), lhs.getCol() + rhs.getCol() }; }
	inline const DiffPosition operator-(const Position& lhs, const Position& rhs) { return { lhs.getRow() - rhs.getRow(), lhs.getCol() - rhs.getCol() }; }

	inline std::ostream& operator<<(std::ostream& os, const row_t& row) { return os << (int)row; }
	inline std::ostream& operator<<(std::ostream& os, const col_t& col) { return os << (int)col; }
	inline std::ostream& operator<<(std::ostream& os, const Position& pos) { return os << '(' << pos.getRow() << ',' << pos.getCol() << ')'; }
	inline std::ostream& operator<<(std::ostream& os, const DiffPosition& pos) { return os << '(' << pos.getRow() << ',' << pos.getCol() << ')'; }
}