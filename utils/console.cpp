#include "console.hpp"
#include "assets.hpp"
#include "colors.hpp"
#include "board.hpp"

namespace console {
    void hideCursor() { std::cout << "\x1b[?25l" << std::flush; }
    void showCursor() { std::cout << "\x1b[?25h" << std::flush; }
    void saveCursor() { std::cout << "\x1b[s"    << std::flush; }
    void restCursor() { std::cout << "\x1b[u"    << std::flush; }
}

void clearScreen()
{
    std::cout <<  "\033[H\033[2J";
}

size_t utf8Width(const std::string &str)
{
    mbstate_t state{};
    const char *ptr = str.c_str();
    size_t len = str.size();
    size_t width = 0;

    while (len > 0)
    {
        if (len >= 2 && ptr[0] == '\033' && ptr[1] == '[')
        {
            const char *end = ptr + len;
            const char *m_pos = ptr;
            while (m_pos < end && *m_pos != 'm')
                ++m_pos;

            if (m_pos < end)
            {
                size_t skip = m_pos - ptr + 1;
                ptr += skip;
                len -= skip;
                continue;
            }
            break;
        }

        wchar_t wc;
        size_t bytes = mbrtowc(&wc, ptr, len, &state);
        if (bytes == (size_t)-1 || bytes == (size_t)-2)
            break;
        if (bytes == 0)
            break;

        ptr += bytes;
        len -= bytes;

        int w = wcwidth(wc);
        if (w > 0)
            width += w;
    }
    return width;
}

size_t padd(const std::string &str)
{
    struct winsize wd{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wd);
    return ((wd.ws_col - utf8Width(str)) / 2);
}

void print(const std::vector<std::string> &lines, const char &clr, bool nline)
{
    for (size_t i = 0; i < lines.size(); i++)
    {
        size_t center_padding = padd(lines[i]);
        std::string output = std::string(center_padding, ' ');

        switch (clr)
        {
            case 'p': output += color::purple(lines[i]); break;
            case 'o': output += color::orange(lines[i]); break;
            case 'y': output += color::yellow(lines[i]); break;
            case 'g': output += color::green(lines[i]);  break;
            case 'b': output += color::blue(lines[i]);   break;
            case 'r': output += color::red(lines[i]);    break;
            default : output += lines[i];                break;
        }
        
        std::cout << output;

        if (nline || i < lines.size() - 1)
            std::cout << '\n';
    }
}
