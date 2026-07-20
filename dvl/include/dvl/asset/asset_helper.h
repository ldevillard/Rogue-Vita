#pragma once

#include <cstdint>

namespace dvl
{
    constexpr std::uint32_t MakeMagic(char a, char b, char c, char d)
    {
        return (static_cast<std::uint32_t>(a) << 0)  |
               (static_cast<std::uint32_t>(b) << 8)  |
               (static_cast<std::uint32_t>(c) << 16) |
               (static_cast<std::uint32_t>(d) << 24);
    }
}