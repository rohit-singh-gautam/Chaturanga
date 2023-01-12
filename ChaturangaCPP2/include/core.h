#pragma once
#include <array>
#include <string_view>

namespace rohit {

typedef uint8_t row_type;
typedef uint8_t column_type;

enum player_type : uint8_t {
    First,
    Second
};

enum mohra_type : uint8_t {
    Empty,
    Raja,
    Senapati,
    Ratha,
    Gaja,
    Ashva,
    Padati,
};

constexpr std::array<std::string_view, 6> mohra_name {
    "Raja",
    "Senapati",
    "Ratha",
    "Gaja",
    "Ashva",
    "Padati"
};

} // namespace rohit

