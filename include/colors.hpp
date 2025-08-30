#pragma once

#include <string>

namespace color
{
    inline std::string red(const std::string &s)
    {
        return "\033[1;31m" + s + "\033[0m";
    }

    inline std::string green(const std::string &s)
    {
        return "\033[1;32m" + s + "\033[0m";
    }

    inline std::string blue(const std::string &s)
    {
        return "\033[38;2;13;146;244m" + s + "\033[0m";
    }

    inline std::string orange(const std::string &s)
    {
        return "\033[38;2;255;102;0m" + s + "\033[0m";
    }

    inline std::string yellow(const std::string &s)
    {
        return "\033[38;2;255;244;85m" + s + "\033[0m";
    }

    inline std::string purple(const std::string &s)
    {
        return "\033[38;2;128;0;128m" + s + "\033[0m";
    }
}
