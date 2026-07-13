#pragma once

#include <string>
#include <string_view>

namespace core {
struct RGBA {
    double r = 0, g = 0, b = 0, a = 1;

    RGBA() = default;

    RGBA(std::string_view hex, double in_a = 1.0)
    {
        auto s = hex.substr(1);

        auto ch = [&s](int pos) {
            return std::stoi(std::string(s.substr(pos, 2)), nullptr, 16) /
                   255.0;
        };

        r = ch(0);
        g = ch(2);
        b = ch(4);
        a = s.size() >= 8 ? ch(6) : in_a;
    }

    RGBA(double r, double g, double b, double a) : r(r), g(g), b(b), a(a)
    {
    }
};

};  // namespace core
