#pragma once

#include <string>

namespace Console {
    inline constexpr const char* RESET  = "\033[0m";
    inline constexpr const char* RED    = "\033[31m";
    inline constexpr const char* GREEN  = "\033[32m";
    inline constexpr const char* YELLOW = "\033[33m";
    inline constexpr const char* BLUE   = "\033[34m";
    inline constexpr const char* MAGENTA= "\033[35m";
    inline constexpr const char* CYAN   = "\033[36m";
    inline constexpr const char* BOLD   = "\033[1m";

    inline std::string color(const char* code, const std::string& s) {
        return std::string(code) + s + RESET;
    }

    inline std::string red(const std::string& s) { return color(RED, s); }
    inline std::string green(const std::string& s) { return color(GREEN, s); }
    inline std::string yellow(const std::string& s) { return color(YELLOW, s); }
    inline std::string blue(const std::string& s) { return color(BLUE, s); }
    inline std::string cyan(const std::string& s) { return color(CYAN, s); }
}
