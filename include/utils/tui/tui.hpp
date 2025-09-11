#pragma once

#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <termios.h>

#include "colors.hpp"

namespace console
{
    char getch();
    void hideCursor();
    void showCursor();
    void saveCursor();
    void restCursor();
    void clearScreen();
}

class Board;
size_t padd(const std::string &str);
size_t utf8Width(const std::string &str);
void print(const std::vector<std::string> &lines, const char &clr, bool nline);
