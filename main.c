#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "lib/colors.h"

#define clear() printf("\033[H\033[2J")

void copy(const char *src[8][8], const char *dest[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            dest[i][j] = src[i][j];
        }
    }
}

bool cmp(const char *__s1, const char *__s2) {
    return strcmp(__s1, __s2) == 0;
}

int type(const char *board[8][8], int x, int y)
{
    if (
        cmp(board[x][y], "♜") || cmp(board[x][y], "♞") ||
        cmp(board[x][y], "♝") || cmp(board[x][y], "♛") ||
        cmp(board[x][y], "♚") || cmp(board[x][y], "♟"))
    {
        return 0;
    }
    else if (
        cmp(board[x][y], "♖") || cmp(board[x][y], "♘") ||
        cmp(board[x][y], "♗") || cmp(board[x][y], "♕") ||
        cmp(board[x][y], "♔") || cmp(board[x][y], "♙"))
    {
        return 1;
    }
    else if (cmp(board[x][y], "·"))
    {
        return 3;
    }
    else
    {
        return 2;
    }
}

void attack(const char *board[8][8], int x, int y)
{
    if (type(board, x, y) == 0)
    {
        board[x][y] = red(0, board[x][y]);
    }
}

int pos(char *move)
{
    int row = 8 - move[1] + '0';
    int col = 0 + move[0] - 'a';
    return row * 8 + col;
}

bool bounds(int nx, int ny)
{
    return nx >= 0 && nx < 8 && ny >= 0 && ny < 8;
}

void mark(const char *board[8][8], int x, int y)
{

    // Pawn
    if (cmp(board[x][y], "♙"))
    {
        int f = 0;
        if (cmp(board[x - 1][y], "·") && x - 1 >= 0)
        {
            board[x - 1][y] = blue(0, "•");
            f = 1;
        }
        if (cmp(board[4][y], "·") && f == 1 && x == 6)
        {
            board[4][y] = blue(0, "•");
        }

        if (x - 1 >= 0 && y - 1 >= 0)
        {
            attack(board, x - 1, y - 1);
        }
        if (x - 1 >= 0 && y + 1 < 8)
        {
            attack(board, x - 1, y + 1);
        }
    }

    // Knight
    if (cmp(board[x][y], "♘"))
    {
        int moves[8][2] = {
            { 2, 1}, { 2, -1}, { 1, 2}, { 1, -2},
            {-1, 2}, {-1, -2}, {-2, 1}, {-2, -1}
        };

        for (int i = 0; i < 8; i++)
        {
            int nx = x + moves[i][0];
            int ny = y + moves[i][1];

            if (bounds(nx, ny))
            {
                if (cmp(board[nx][ny], "·"))
                {
                    board[nx][ny] = blue(0, "•");
                }
                else
                {
                    attack(board, nx, ny);
                }
            }
        }
    }

    // Rook
    if (cmp(board[x][y], "♖"))
    {
        int dir[4][2] = {
            {0, 1}, {0, -1}, {1, 0}, {-1, 0}
        };

        for (int d = 0; d < 4; d++)
        {
            int dx = dir[d][0];
            int dy = dir[d][1];

            int nx = x + dx;
            int ny = y + dy;

            while (bounds(nx, ny))
            {
                if (cmp(board[nx][ny], "·"))
                {
                    board[nx][ny] = blue(0, "•");
                }
                else
                {
                    attack(board, nx, ny);
                    break;
                }
                nx += dx;
                ny += dy;
            }
        }
    }

    // Bishop
    if (cmp(board[x][y], "♗"))
    {
        int dir[4][2] = {
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
        };

        for (int d = 0; d < 4; d++)
        {
            int dx = dir[d][0];
            int dy = dir[d][1];

            int nx = x + dx;
            int ny = y + dy;

            while (bounds(nx, ny))
            {
                if (cmp(board[nx][ny], "·"))
                {
                    board[nx][ny] = blue(0, "•");
                }
                else
                {
                    attack(board, nx, ny);
                    break;
                }
                nx += dx;
                ny += dy;
            }
        }
    }

    // Queen
    if (cmp(board[x][y], "♕"))
    {
        int dir[8][2] = {
            {0, 1}, {0, -1}, { 1, 0}, {-1,  0},
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
        };

        for (int d = 0; d < 8; d++)
        {
            int dx = dir[d][0];
            int dy = dir[d][1];

            int nx = x + dx;
            int ny = y + dy;

            while (bounds(nx, ny))
            {
                if (cmp(board[nx][ny], "·"))
                {
                    board[nx][ny] = blue(0, "•");
                }
                else
                {
                    attack(board, nx, ny);
                    break;
                }
                nx += dx;
                ny += dy;
            }
        }
    }

    // King
    if (cmp(board[x][y], "♔"))
    {

        int dir[8][2] = {
            {0, 1}, {0, -1}, { 1, 0}, {-1,  0},
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
        };

        for (int d = 0; d < 8; d++)
        {
            int dx = dir[d][0];
            int dy = dir[d][1];

            int nx = x + dx;
            int ny = y + dy;

            if (bounds(nx, ny))
            {
                if (cmp(board[nx][ny], "·"))
                {
                    board[nx][ny] = blue(0, "•");
                }
                else
                {
                    attack(board, nx, ny);
                }
            }
        }
    }
}

int enPassant(const char *board[8][8], int x, int y, int airf, int airt, int aicf, int aict)
{
    int rt = 3;
    if (y - 1 == aict && x == airt
        && cmp(board[x][y - 1], "♟")
        && abs(airf - airt) == 2)
    {
        board[x - 1][y - 1] = blue(0, "•");
        rt = 0;
    }

    if (y + 1 == aict && x == airt
        && cmp(board[x][y + 1], "♟")
        && abs(airf - airt) == 2)
    {
        board[x - 1][y + 1] = blue(0, "•");
        rt = 1;
    }

    return rt;
}

bool uAttack(const char *board[8][8], int x, int y)
{

    int pawn[2][2]   = {{-1, 1}, {-1, -1}                      };

    int knight[8][2] = {{ 1, 2}, { 2,  1}, {-2, -1}, {-1, -2},
                        {-1, 2}, { 1, -2}, {-2,  1}, { 2, -1}  };

    int dir[8][2]    = {{ 1, 0}, {-1,  0}, { 0,  1}, { 0, -1},
                        { 1, 1}, {-1, -1}, { 1, -1}, {-1,  1}  };

    for (int i = 0; i < 2; i++)
    {
        int nx = x + pawn[i][0];
        int ny = y + pawn[i][1];
        if (bounds(nx, ny) && cmp(board[nx][ny], "♟"))
        {
            return true;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        int nx = x + knight[i][0];
        int ny = y + knight[i][1];
        if (bounds(nx, ny) && cmp(board[nx][ny], "♞"))
        {
            return true;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        int dx = dir[i][0];
        int dy = dir[i][1];
        int nx = x + dx, ny = y + dy;

        while (bounds(nx, ny))
        {
            if (cmp(board[nx][ny],         "·" )
            ||  cmp(board[nx][ny], blue(0, "•")))
            {
                nx += dx;
                ny += dy;
                continue;
            }

            if ((i <  4 && (cmp(board[nx][ny], "♜")
            ||              cmp(board[nx][ny], "♛")))
            ||  (i >= 4 && (cmp(board[nx][ny], "♝")
            ||              cmp(board[nx][ny], "♛"))))
            {
                return true;
            }
            break;
        }
    }

    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            if (dx == 0 && dy == 0)
                continue;
            int nx = x + dx, ny = y + dy;
            if (bounds(nx, ny) && cmp(board[nx][ny], "♚"))
            {
                return true;
            }
        }
    }

    return false;
}

/*bool inCheck(const char *board[8][8], int x, int y)
{

}*/

int main()
{

    const char *board[8][8] = {
        {"♜", "♞", "♝", "♛", "♚", "♝", "♞", "♜"},
        {"♟", "♟", "♟", "♟", "♟", "♟", "♟", "♟"},
        {"·", "·", "·", "·", "·", "·", "·", "·"},
        {"·", "·", "·", "·", "·", "·", "·", "·"},
        {"·", "·", "·", "·", "·", "·", "·", "·"},
        {"·", "·", "·", "·", "·", "·", "·", "·"},
        {"♙", "♙", "♙", "♙", "♙", "♙", "♙", "♙"},
        {"♖", "♘", "♗", "♕", "♔", "♗", "♘", "♖"}};

    const char *tmp[8][8];
    copy(board, tmp);
    char input[256];
    char str[256];
    char curr[3];
    char play[6];
    char aiplay[6];
    char memory[1000][6];
    int input_len = 0;
    int try = 0, rnd = 0, cnt = 0;
    int nr, nc, ep = 3;
    int airf = 1, aicf = 3, airt = 3, aict = 3;
    bool kmoved  = false;
    bool lrmoved = false;
    bool rrmoved = false;
    bool isprmt  = false;
    bool myturn  = true;


    str[0] = '\0';
    clear();
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 6; j++) {
            memory[i][j] = '\0';
        }
    }
    while (1)
    {

        clear();
        for (int i = 0; i < 8; i++)
        {
            printf("%d ", 8 - i);
            for (int j = 0; j < 8; j++)
            {
                printf("%s ", board[i][j]);
            }
            printf("\n");
        }
        printf("  a b c d e f g h\n");
        /*for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                printf("%c ", memory[i][j]);
            }
            printf(", ");
        }*/
        printf("%s\n", red(0, str));
        char *user = getenv("USER");
        char ch[256];
        snprintf(ch, sizeof(ch), "%s@chess", user);
        printf("%s:%s$ ", green(0, ch), blue(0, "~"));

        input_len = 0;
        while (1)
        {
            char c = getchar();

            if (c == '\n')
            {
                input[input_len] = '\0';
                break;
            }

            if (input_len < sizeof(input) - 1)
            {
                input[input_len++] = c;
                printf("%c", c);
                fflush(stdout);
            }
        }

        if (strlen(input) == 1 && isprmt)
        {
            if (cmp(input, "r"))
            {
                tmp[nr][nc] = "♖";
                board[nr][nc] = "♖";
            }
            else if (cmp(input, "n"))
            {
                tmp[nr][nc] = "♘";
                board[nr][nc] = "♘";
            }
            else if (cmp(input, "b"))
            {
                tmp[nr][nc] = "♗";
                board[nr][nc] = "♗";
            }
            else if (cmp(input, "q"))
            {
                tmp[nr][nc] = "♕";
                board[nr][nc] = "♕";
            }
            strcpy(str, "");
        }

        else if (strlen(input) == 2 && myturn)
        {
            char move[3];
            strcpy(move, input);

            int idx = pos(move);
            int row = idx / 8;
            int col = idx % 8;

            if (bounds(row, col))
            {

                if (type(board, row, col) == 1)
                {
                    try++;
                    if (try > 1)
                    {
                        copy(tmp, board);
                    }
                    mark(board, row, col);
                    strcpy(curr, move);

                    // King Castling
                    if (cmp(board[row][col], "♔") && !kmoved)
                    {
                        // Queen side castling
                        if (cmp(board[7][1], "·") && cmp(board[7][2], "·") && cmp(board[7][3], blue(0, "•")) && !lrmoved)
                        {
                            if (!uAttack(tmp, 7, 4) && !uAttack(tmp, 7, 3) && !uAttack(tmp, 7, 2))
                            {
                                board[7][2] = blue(0, "•");
                            }
                        }

                        // King side castling
                        if (cmp(board[7][5], blue(0, "•")) && cmp(board[7][6] ,"·") && !rrmoved)
                        {
                            if (!uAttack(tmp, 7, 4) && !uAttack(tmp, 7, 5) && !uAttack(tmp, 7, 6))
                            {
                                board[7][6] = blue(0, "•");
                            }
                        }
                    }
                    // En Passant
                    if (cmp(board[row][col], "♙") && row == 3)
                    {
                        ep = enPassant(board, row, col, airf, airt, aicf, aict);
                    }
                }
                else if (type(board, row, col) == 2)
                {
                    myturn = !myturn;
                    strcat(play, curr);
                    strcat(play, move);
                    strcpy(memory[cnt], play);
                    cnt++;
                    play[0]='\0';

                    char req[600] = "";

                    for (int i = 0; i < cnt; i++)
                    {
                        strcat(req, memory[i]);
                        strcat(req, " ");
                    }
                    idx = pos(curr);
                    int r = idx / 8;
                    int c = idx % 8;
                    nr = row;
                    nc = col;
                    tmp[row][col] = tmp[r][c];
                    tmp[r][c] = "·";

                    rnd++;
                    // Pawn Promotion
                    if (cmp(board[r][c], "♙") && row == 0)
                    {
                        strcpy(str, "Promote as [r / n / b / q]");
                        isprmt = true;
                    }
                    // king Castling
                    if (cmp(board[r][c], "♔"))
                    {
                        kmoved = true;
                    }
                    if (cmp(board[r][c], "♖") && c == 0)
                    {
                        lrmoved = true;
                    }
                    if (cmp(board[r][c], "♖") && c == 7)
                    {
                        rrmoved = true;
                    }
                    // king side
                    if (row == 7 && col == 6 && cmp(board[r][c], "♔"))
                    {
                        tmp[row][7] = "·";
                        tmp[row][5] = "♖";
                    }
                    // queen side
                    if (row == 7 && col == 2 && cmp(board[r][c], "♔"))
                    {
                        tmp[row][0] = "·";
                        tmp[row][3] = "♖";
                    }
                    // En Passant
                    if (ep == 0 && col == c - 1)
                    {
                        tmp[row + 1][col] = "·";
                    }
                    else if (ep == 1 && col == c + 1)
                    {
                        tmp[row - 1][col] = "·";
                    }
                    clear();
                    for (int i = 0; i < 8; i++)
                    {
                        printf("%d ", 8 - i);
                        for (int j = 0; j < 8; j++)
                        {
                            printf("%s ", tmp[i][j]);
                        }
                        printf("\n");
                    }
                    printf("  a b c d e f g h\n");
                    printf("%s\n", red(0, str));
                    printf("%s:%s$ ", green(0, "a7md@chess"), blue(0, "~"));

                    char cmd[1000];
                    snprintf(cmd, sizeof(cmd), "python3 ai.py \"%s\"", req);
                    printf("\nThinking...\n");
                    FILE *fp = popen(cmd, "r");
                    if (fp != NULL)
                    {

                        fgets(aiplay, sizeof(aiplay), fp);
                        pclose(fp);
                        strcpy(memory[cnt], aiplay);
                        
                    }
                    else
                    {
                        strcpy(str, "Error launching AI");
                        break;
                    }
                    clear();
                }
                if (type(board, row, col) == 2)
                {
                    try = 0;cnt++;
                }
                if (type(board, row, col) != 1)
                {
                    copy(tmp, board);
                }
            }
        }

        if (!myturn)
        {
            
            /*-------------
            ****AI MOVE****
            -------------*/
            
            aiplay[strlen(aiplay) - 1] = '\0';

            char from[3];
            strncpy(from, aiplay, 2);
            from[2] = '\0';
            int idxf = pos(from);
            int rf = idxf / 8;
            int cf = idxf % 8;
            airf = rf;
            aicf = cf;

            char to[3];
            strncpy(to, aiplay + strlen(aiplay) - 2, 2);
            to[2] = '\0';
            int idxt = pos(to);
            int rt = idxt / 8;
            int ct = idxt % 8;
            airt = rt;
            aict = ct;

            char t[3];
            strncpy(t, aiplay + strlen(aiplay) - 4, 1);
            t[1] = '\0';

            // Castling
            bool kc = false;
            bool qc = false;

            if (cmp(from, "e8") && cmp(to, "c8") && cmp(tmp[rf][cf], "♚")) {
                tmp[0][2] = "♚";
                tmp[0][3] = "♜";
                tmp[0][0] = "·";
                tmp[0][4] = "·";
                qc = true;
            } else if (cmp(from, "e8") && cmp(to, "g8") && cmp(tmp[rf][cf], "♚")) {
                tmp[0][6] = "♚";
                tmp[0][5] = "♜";
                tmp[0][7] = "·";
                tmp[0][4] = "·";
                kc = true;
            }

            // En Passant
            if (
            (cf-1 >= 0
            && cmp(tmp[rf][cf-1], "♙")
            && cmp(tmp[rf][cf], "♟")
            && rt == rf+1 && ct == cf-1
            )
            ||
            (cf+1 < 8
            && cmp(tmp[rf][cf+1], "♙")
            && cmp(tmp[rf][cf], "♟")
            && rt == rf+1 && ct == cf+1
            )
            ) {
                tmp[rf][ct] = "·";
            }

            // Promotion
            if (cmp(t, "r")) {
                tmp[rt][ct] = "♜";
            }
            else if (cmp(t, "n")) {
                tmp[rt][ct] = "♞";
            }
            else if (cmp(t, "b")) {
                tmp[rt][ct] = "♝";
            }
            else if (cmp(t, "q")) {
                tmp[rt][ct] = "♛";
            }
            if (!kc && !qc)
            {
                tmp[rt][ct] = tmp[rf][cf];
                tmp[rf][cf] = "·";
            }

            copy(tmp, board);
            myturn = !myturn;
        }

        if (cmp(input, "x"))
        {
            break;
        }
    }


    return 0;
}
