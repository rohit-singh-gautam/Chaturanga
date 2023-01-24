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

struct [[gnu::packed]] board_entry {
    bool firstRaja          :1  { false };
    bool firstSenapati      :1  { false };
    bool firstRathaLeft     :1  { false };
    bool firstRathaRight    :1  { false };
    bool firstGajaLeft      :1  { false };
    bool firstGajaRight     :1  { false };
    bool firstAshvaLeft     :1  { false };
    bool firstAshvaRight    :1  { false };
    bool firstPadati0       :1  { false };
    bool firstPadati1       :1  { false };
    bool firstPadati2       :1  { false };
    bool firstPadati3       :1  { false };
    bool firstPadati4       :1  { false };
    bool firstPadati5       :1  { false };
    bool firstPadati6       :1  { false };
    bool firstPadati7       :1  { false };
    bool secondRaja          :1  { false };
    bool secondSenapati      :1  { false };
    bool secondRathaLeft     :1  { false };
    bool secondRathaRight    :1  { false };
    bool secondGajaLeft      :1  { false };
    bool secondGajaRight     :1  { false };
    bool secondAshvaLeft     :1  { false };
    bool secondAshvaRight    :1  { false };
    bool secondPadati0       :1  { false };
    bool secondPadati1       :1  { false };
    bool secondPadati2       :1  { false };
    bool secondPadati3       :1  { false };
    bool secondPadati4       :1  { false };
    bool secondPadati5       :1  { false };
    bool secondPadati6       :1  { false };
    bool secondPadati7       :1  { false };
    bool attackFirstRaja          :1  { false };
    bool attackFirstSenapati      :1  { false };
    bool attackFirstRathaLeft     :1  { false };
    bool attackFirstRathaRight    :1  { false };
    bool attackFirstGajaLeft      :1  { false };
    bool attackFirstGajaRight     :1  { false };
    bool attackFirstAshvaLeft     :1  { false };
    bool attackFirstAshvaRight    :1  { false };
    bool attackFirstPadati0       :1  { false };
    bool attackFirstPadati1       :1  { false };
    bool attackFirstPadati2       :1  { false };
    bool attackFirstPadati3       :1  { false };
    bool attackFirstPadati4       :1  { false };
    bool attackFirstPadati5       :1  { false };
    bool attackFirstPadati6       :1  { false };
    bool attackFirstPadati7       :1  { false };
    bool attackSecondRaja          :1  { false };
    bool attackSecondSenapati      :1  { false };
    bool attackSecondRathaLeft     :1  { false };
    bool attackSecondRathaRight    :1  { false };
    bool attackSecondGajaLeft      :1  { false };
    bool attackSecondGajaRight     :1  { false };
    bool attackSecondAshvaLeft     :1  { false };
    bool attackSecondAshvaRight    :1  { false };
    bool attackSecondPadati0       :1  { false };
    bool attackSecondPadati1       :1  { false };
    bool attackSecondPadati2       :1  { false };
    bool attackSecondPadati3       :1  { false };
    bool attackSecondPadati4       :1  { false };
    bool attackSecondPadati5       :1  { false };
    bool attackSecondPadati6       :1  { false };
    bool attackSecondPadati7       :1  { false };
};

} // namespace rohit

