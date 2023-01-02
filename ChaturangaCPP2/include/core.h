#pragma once
#include <array>
#include <string_view>

namespace rohit {

enum mohra_type : uint8_t {
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

struct [[gnu::packed]] ashtapada_entry {
    uint8_t player : 1;
    mohra_type   mohra : 7;
    constexpr ashtapada_entry(uint8_t player, mohra_type mohra) : player { player }, mohra { mohra } { }
};

struct ashtapada {

};

} // namespace rohit

