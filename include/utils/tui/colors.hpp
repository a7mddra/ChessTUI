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

    inline std::string lgreen(const std::string &s)
    {
        return "\033[38;2;124;156;92m" + s + "\033[0m";
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

    inline std::string gray1(const std::string &s)
    {
        return "\033[38;2;196;196;179m" + s + "\033[0m";
    }

    inline std::string gray2(const std::string &s)
    {
        return "\033[38;2;88;112;64m" + s + "\033[0m";
    }

    inline std::string sq1(const std::string &s)
    {
        return "\033[38;2;0;0;0;48;2;236;236;204m" + s + "\033[0m";
    }

    inline std::string sq2(const std::string &s)
    {
        return "\033[38;2;0;0;0;48;2;124;156;92m" + s + "\033[0m";
    }

    inline std::string hsq1(const std::string &s)
    {
        return "\033[38;2;0;0;0;48;2;245;246;130m" + s + "\033[0m";
    }

    inline std::string hsq2(const std::string &s)
    {
        return "\033[38;2;0;0;0;48;2;185;202;67m" + s + "\033[0m";
    }

    inline std::string rsq1(const std::string &s)
    {
        return "\033[38;2;0;0;0;48;2;175;43;45m" + s + "\033[0m";
    }

    inline std::string rsq2(const std::string &s)
    {
        return "\033[38;2;0;0;0;48;2;176;44;44m" + s + "\033[0m";
    }
}
