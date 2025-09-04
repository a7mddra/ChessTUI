#pragma once

#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

#include "colors.hpp"

namespace console
{
    void hideCursor();
    void showCursor();
    void saveCursor();
    void restCursor();
    void clearScreen();
}

class Board;
size_t padd(const std::string &str);
void print(const std::vector<std::string> &lines,
           const char &clr, bool nline);
