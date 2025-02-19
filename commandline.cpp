/////////////////////////////////////////////////////////////////////////////////////////////
// Author: Rohit Jairaj Singh (rohit@singh.org.in)                                         //
// This program is free software: you can redistribute it and/or modify it under the terms //
// of the GNU General Public License as published by the Free Software Foundation, either  //
// version 3 of the License, or (at your option) any later version.                        //
//                                                                                         //
// This program is distributed in the hope that it will be useful, but WITHOUT ANY         //
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A         //
// PARTICULAR PURPOSE. See the GNU General Public License for more details.                //
//                                                                                         //
// You should have received a copy of the GNU General Public License along with this       //
// program. If not, see <https://www.gnu.org/licenses/>.                                   //
/////////////////////////////////////////////////////////////////////////////////////////////

#include <commandline.h>
#include <typeinfo>
#include <charconv>
#include <filesystem>

namespace rohit {

constexpr bool to_bool(const char *value) {
    return *value == 'T' || *value == 't';
}

constexpr inline const char *string_end(const char *value) {
    while(*value) ++value;
    return value;
}

bool to_type(const std::type_info &id, void *store, const char *value) {
    auto valueend {string_end(value)};
    if (id == typeid(bool)) {
        *(bool *)store = to_bool(value);
    } else if (id == typeid(uint8_t)) {
        uint8_t result { 0 };
        std::from_chars(value, valueend, result);
        *(uint8_t *)store = result;
    } else if (id == typeid(uint16_t)) {
        uint16_t result { 0 };
        std::from_chars(value, valueend, result);
        *(uint16_t *)store = result;
    } else if (id == typeid(uint32_t)) {
        uint32_t result { 0 };
        std::from_chars(value, valueend, result);
        *(uint32_t *)store = result;
    } else if (id == typeid(uint64_t)) {
        uint64_t result { 0 };
        std::from_chars(value, valueend, result);
        *(uint64_t *)store = result;
    } else if (id == typeid(int8_t)) {
        int8_t result { 0 };
        std::from_chars(value, valueend, result);
        *(int8_t *)store = result;
    } else if (id == typeid(int16_t)) {
        int16_t result { 0 };
        std::from_chars(value, valueend, result);
        *(int16_t *)store = result;
    } else if (id == typeid(int32_t)) {
        int32_t result { 0 };
        std::from_chars(value, valueend, result);
        *(int32_t *)store = result;
    } else if (id == typeid(int64_t)) {
        int64_t result { 0 };
        std::from_chars(value, valueend, result);
        *(int64_t *)store = result;
    } else if (id == typeid(const char *)) {
        *(const char **)store = value;
    } else if (id == typeid(std::filesystem::path)) {
        *(std::filesystem::path *)store = std::filesystem::path(value);
    } else if (id == typeid(std::string)) {
        *(std::string *)store = std::string(value);
    } else {
        return false;
    }

    return true;
}

enum class parser_state {
    OPTION,
    VALUE
};

bool commandline::parser(const int argc, char const * const * const argv) {
    name = std::filesystem::path { argv[0] };
    parser_state state = parser_state::OPTION;
    commandline_option *optionentry;
    for(int arg_index = 1; arg_index < argc; ++arg_index) {
        const char *arg_curr = argv[arg_index];
        switch (state) {
        case parser_state::OPTION:
            if (arg_curr[0] != '-') return false;
            if (arg_curr[1] == '-') {
                // Multicharacter options
                auto optionentry_itr = options_name.find(arg_curr + 2);
                if (optionentry_itr == options_name.end()) return false;
                optionentry = optionentry_itr->second;
            } else {
                // Single character option
                auto optionentry_itr = options_ch.find(arg_curr[1]);
                if (optionentry_itr == options_ch.end()) return false;
                optionentry = optionentry_itr->second;
            }

            if (optionentry->type_id == typeid(bool)) {
                bool *store_bl = (bool *)optionentry->variable;
                *store_bl = true;
            } else state = parser_state::VALUE;

            break;

        case parser_state::VALUE:
            if (! to_type(optionentry->type_id, optionentry->variable, arg_curr)) return false;

            state = parser_state::OPTION;
            
            break;
        }
    }

    if (help) {
        return false;
    }

    return state == parser_state::OPTION;
}

// Create help in form
// Usage: <appname> <usage>
//      Description ...
//
// Summary:
//      Summary ...
//
// Parameters:
//      <Parameters>
//
const std::string commandline::usage() const {
    std::string usage_line { std::string{"Usage: "} + std::string { name.filename().string() } + " "};
    for (auto option: options) {
        usage_line += option.get_short_description();
    }
    usage_line += "\n\t" + short_description + "\n\nSummary:\n\t" + summary + "\n\nParameters:\n";
    for (auto option: options) {
        usage_line += option.get_parameter_description();
    }
    return usage_line;
}

const std::string commandline_option::get_short_description() const {
    std::string ret;
    if (!required) ret += "[";
    if (ch != '\0') {
        ret += "-";
        ret.push_back(ch);
        if (!name.empty()) ret += "/";
    }
    if (!name.empty()) ret += "--" + name;
    if (!value_help.empty() && type_id != typeid(bool)) {
        ret += " <";
        ret += value_help;
        ret += ">";
    }
    if (!required) ret += "]";
    ret += " ";
    return ret;
}

const std::string commandline_option::get_parameter_description() const {
    std::string ret { "\t" + get_short_description() };
    ret += "\n\t\t" + help + "\n";
    return ret;
}

}