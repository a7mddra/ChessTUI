#pragma once

#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <sstream>
#include <functional>

namespace console
{
    void hideCursor();
    void showCursor();
    void saveCursor();
    void restCursor();
}

class Board;

void clearScreen();
size_t padd(const std::string &str);
void print(const std::vector<std::string> &lines,
           const char &clr, bool nline);
