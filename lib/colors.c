#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"

const char *color(char ch, const char *st, const char *code)
{
    char *coloredSt = NULL;
    int bits;
    if (st == NULL) {
        bits = 16;
        coloredSt = (char *)malloc(bits * sizeof(char));
        snprintf(coloredSt, bits, "\033[%sm%c\033[0m", code, ch);
    } else {
        bits = 1024;
        coloredSt = (char *)malloc(bits * sizeof(char));
        snprintf(coloredSt, bits, "\033[%sm%s\033[0m", code, st);
    }
    return coloredSt;
}

const char *red(char ch, const char *str)
{
    return color(ch, str, "1;31");
}

const char *green(char ch, const char *str)
{
    return color(ch, str, "1;32");
}

const char *blue(char ch, const char *str)
{
    return color(ch, str, "1;34");
}
