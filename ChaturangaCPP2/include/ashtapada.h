#pragma once

#include <core.h>

namespace rohit {


struct [[gnu::packed]] ashtapada_entry {
    player_type player : 1;
    mohra_type  mohra : 7;

    constexpr ashtapada_entry() : player { 0 }, mohra { mohra_type::Empty } { }
    constexpr ashtapada_entry(uint8_t player, mohra_type mohra) : player { player }, mohra { mohra } { }
};

class ashtapada {
public:
    static constexpr row_type max_row { 8 };
    static constexpr column_type max_col { 8 };
    static constexpr size_t boardsize { max_row * 8 + max_col };

private:
    ashtapada_entry board[boardsize] { };

    constexpr size_t get_index(const row_type row, const column_type column) { return row*8 + column; }

public:
    constexpr ashtapada() { }

    constexpr ashtapada_entry &operator[] (const row_type row, const column_type column) { return board[get_index(row, column)]; }

}; // class board

    
} // namespace rohit