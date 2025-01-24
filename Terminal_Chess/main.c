#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "lib/colors.h"

#define clearScreen() printf("\033[H\033[2J")

void copyBoard(const char *src[8][8], const char *dest[8][8])
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

bool cmp(const char *__s1, const char *__s2)
{
    return strcmp(__s1, __s2) == 0;
}

int type(const char *board[8][8], int x, int y) {
    const char *blackPieces[] = {"♜", "♞", "♝", "♛", "♚", "♟"};
    const char *whitePieces[] = {"♖", "♘", "♗", "♕", "♔", "♙"};
    const char *emptySpace = "·";

    for (int i = 0; i < 6; i++) {
        if (cmp(board[x][y], blackPieces[i])) return 0;
        if (cmp(board[x][y], whitePieces[i])) return 1;
    }

    return cmp(board[x][y], emptySpace) ? 3 : 2;
}

void attack(const char *board[8][8], int x, int y)
{
    if (type(board, x, y) == 0)
        board[x][y] = red(0, board[x][y]);
}

int getPos(char *move)
{
    int row = 8 - move[1] + '0';
    int col = 0 + move[0] - 'a';
    return row * 8 + col;
}

bool checkBounds(int nx, int ny)
{
    return nx >= 0 && nx < 8 && ny >= 0 && ny < 8;
}

void markMove(const char *board[8][8], int nx, int ny) {
    if (checkBounds(nx, ny)) {
        if (cmp(board[nx][ny], "·")) {
            board[nx][ny] = blue(0, "•");
        } else {
            attack(board, nx, ny);
        }
    }
}

void markPieceMoves(const char *board[8][8], int x, int y, int moves[][2], int numMoves, bool isSliding) {
    for (int i = 0; i < numMoves; i++) {
        int nx = x + moves[i][0];
        int ny = y + moves[i][1];
        
        if (isSliding) {
            while (checkBounds(nx, ny)) {
                if (cmp(board[nx][ny], "·"))
                    markMove(board, nx, ny);
                else {
                    markMove(board, nx, ny);
                    break;
                }
                nx += moves[i][0];
                ny += moves[i][1];
            }
        } else {
            markMove(board, nx, ny);
        }
    }
}

void mark(const char *board[8][8], int x, int y) {

    if (cmp(board[x][y], "♙")) {
        if (cmp(board[x - 1][y], "·") && x - 1 >= 0) {
            board[x - 1][y] = blue(0, "•");
            if (x == 6 && cmp(board[4][y], "·"))
            board[4][y] = blue(0, "•");
        }

        if (x - 1 >= 0 && y - 1 >= 0 && !cmp(board[x-1][y-1], "·"))
            markMove(board, x - 1, y - 1);
        if (x - 1 >= 0 && y + 1 <= 7 && !cmp(board[x-1][y+1], "·"))
            markMove(board, x - 1, y + 1);
    }

    if (cmp(board[x][y], "♘")) {
        int knightMoves[8][2] = {
            { 2,  1}, { 2, -1}, { 1,  2}, { 1, -2},
            {-1,  2}, {-1, -2}, {-2,  1}, {-2, -1},
        };
        markPieceMoves(board, x, y, knightMoves, 8, false);
    }

    if (cmp(board[x][y], "♖")) {
        int rookMoves[4][2] = {
            { 0,  1}, { 0, -1}, { 1,  0}, {-1,  0},
        };
        markPieceMoves(board, x, y, rookMoves, 4, true);
    }

    if (cmp(board[x][y], "♗")) {
        int bishopMoves[4][2] = {
            { 1,  1}, { 1, -1}, {-1,  1}, {-1, -1},
        };
        markPieceMoves(board, x, y, bishopMoves, 4, true);
    }


    if (cmp(board[x][y], "♕")) {
        int queenMoves[8][2] = {
            { 0,  1}, { 0, -1}, { 1,  0}, {-1,  0},
            { 1,  1}, { 1, -1}, {-1,  1}, {-1, -1}
        };
        markPieceMoves(board, x, y, queenMoves, 8, true);
    }


    if (cmp(board[x][y], "♔")) {
        int kingMoves[8][2] = {
            { 0,  1}, { 0, -1}, { 1,  0}, {-1 , 0},
            { 1,  1}, { 1, -1}, {-1 , 1}, {-1 ,-1}
        };
        markPieceMoves(board, x, y, kingMoves, 8, false);
    }
}

int enPassant(const char *board[8][8], int x, int y, int ai_row_from, int ai_row_to, int ai_col_from, int ai_col_to)
{
    int row_to = 3;
    if (y - 1 == ai_col_to && x == ai_row_to
        && cmp(board[x][y - 1], "♟")
        && abs(ai_row_from - ai_row_to) == 2)
    {
        board[x - 1][y - 1] = blue(0, "•");
        row_to = 0;
    }

    if (y + 1 == ai_col_to && x == ai_row_to
        && cmp(board[x][y + 1], "♟")
        && abs(ai_row_from - ai_row_to) == 2)
    {
        board[x - 1][y + 1] = blue(0, "•");
        row_to = 1;
    }

    return row_to;
}

bool checkAttack(const char *board[8][8], int nx, int ny, const char *piece) {
    return checkBounds(nx, ny) && cmp(board[nx][ny], piece);
}

bool underAttack(const char *board[8][8], int x, int y) {
    
    int pawn[2][2] = {{-1,  1}, {-1, -1}};

    int knight[8][2] = {{ 1,  2}, { 2,  1}, {-2, -1}, {-1, -2},
                        {-1,  2}, { 1, -2}, {-2,  1}, { 2, -1}};

    int dir[8][2] = {{ 1,  0}, {-1,  0}, { 0,  1}, { 0, -1},
                     { 1,  1}, {-1, -1}, { 1, -1}, {-1,  1}};

    for (int i = 0; i < 2; i++) {
        int nx = x + pawn[i][0];
        int ny = y + pawn[i][1];
        if (checkAttack(board, nx, ny, "♟"))
            return true;
    }

    for (int i = 0; i < 8; i++) {
        int nx = x + knight[i][0];
        int ny = y + knight[i][1];
        if (checkAttack(board, nx, ny, "♞"))
            return true;
    }

    for (int i = 0; i < 8; i++) {
        int dx = dir[i][0];
        int dy = dir[i][1];
        int nx = x + dx, ny = y + dy;

        while (checkBounds(nx, ny)) {
            if (cmp(board[nx][ny], "·") || cmp(board[nx][ny], blue(0, "•"))) {
                nx += dx;
                ny += dy;
                continue;
            }

            if (i < 4 && (cmp(board[nx][ny], "♜") || cmp(board[nx][ny], "♛")))
                return true;

            if (i >= 4 && (cmp(board[nx][ny], "♝") || cmp(board[nx][ny], "♛")))
                return true;

            break;
        }
    }

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0)
                continue;
            int nx = x + dx, ny = y + dy;
            if (checkAttack(board, nx, ny, "♚"))
                return true;
        }
    }

    return false;
}

/*bool inCheck(const char *board[8][8], int x, int y)
{

}*/

void cout(const char *board[8][8], char str[256])
{
    clearScreen();
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
    printf("%s\n", red(0, str));
    char *user = getenv("USER");
    char ch[256];
    snprintf(ch, sizeof(ch), "%s@chess", user);
    printf("%s:%s$ ", green(0, ch), blue(0, "~"));
}

void cin(char *input)
{
    int ln = 0;
    while (1)
    {
        char c = getchar();
        if (c == '\n') {
            input[ln] = '\0';
            break;
        }

        if (ln < sizeof(input) - 1) {
            input[ln++] = c;
            printf("%c", c);
            fflush(stdout);
        }
    }
}

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
    copyBoard(board, tmp);
    char input[256];
    char str[256];
    char currnet_move[3];
    char my_move[6];
    char ai_move[6];
    char memory[1000][6];
    int try = 0, rnd = 0, cnt = 0;
    int nrow, ncol, ep = 3;
    int ai_row_from = 1, ai_col_from = 3,
        ai_row_to   = 3, ai_col_to   = 3;
    bool king_moved       = false;
    bool left_rook_moved  = false;
    bool right_rook_moved = false;
    bool is_promotion     = false;
    bool my_turn = true;

    str[0] = '\0';
    clearScreen();
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 6; j++) {
            memory[i][j] = '\0';
        }
    }
    while (1)
    {
        cout(board, str);
        cin(input);

        if (strlen(input) == 1 && is_promotion)
        {
            if (cmp(input, "r")) {
                tmp[nrow][ncol] = "♖";
                board[nrow][ncol] = "♖";
            } else if (cmp(input, "n")) {
                tmp[nrow][ncol] = "♘";
                board[nrow][ncol] = "♘";
            } else if (cmp(input, "b")) {
                tmp[nrow][ncol] = "♗";
                board[nrow][ncol] = "♗";
            } else if (cmp(input, "q")) {
                tmp[nrow][ncol] = "♕";
                board[nrow][ncol] = "♕";
            } strcpy(str, "");
        }

        else if (strlen(input) == 2 && my_turn)
        {
            char move[3];
            strcpy(move, input);

            int idx = getPos(move);
            int row = idx / 8;
            int col = idx % 8;

            if (checkBounds(row, col)) {

                if (type(board, row, col) == 1) {
                    try++;
                    if (try > 1)
                        copyBoard(tmp, board);
                    mark(board, row, col);
                    strcpy(currnet_move, move);

                    // King Castling
                    if (cmp(board[row][col], "♔") && !king_moved)
                    {
                        // Queen side castling
                        if (cmp(board[7][1], "·")
                        &&  cmp(board[7][2], "·")
                        &&  cmp(board[7][3], blue(0, "•"))
                        &&  !left_rook_moved
                        &&  !underAttack(tmp, 7, 4)
                        &&  !underAttack(tmp, 7, 3)
                        &&  !underAttack(tmp, 7, 2))
                            board[7][2] = blue(0, "•");

                        // King side castling
                        if (cmp(board[7][6] ,"·")
                        &&  cmp(board[7][5], blue(0, "•"))
                        &&  !right_rook_moved
                        &&  !underAttack(tmp, 7, 4)
                        &&  !underAttack(tmp, 7, 5)
                        &&  !underAttack(tmp, 7, 6))
                            board[7][6] = blue(0, "•");
                            
                    }
                    // En Passant
                    if (cmp(board[row][col], "♙") && row == 3)
                        ep = enPassant(board, row, col,
                        ai_row_from, ai_row_to, ai_col_from, ai_col_to);
                }
                else if (type(board, row, col) == 2)
                {
                    my_turn = !my_turn;
                    strcat(my_move, currnet_move);
                    strcat(my_move, move);
                    strcpy(memory[cnt], my_move);
                    cnt++;
                    my_move[0]='\0';

                    char req[600] = "";

                    for (int i = 0; i < cnt; i++) {
                        strcat(req, memory[i]);
                        strcat(req, " ");
                    }
                    idx = getPos(currnet_move);
                    int r = idx / 8;
                    int c = idx % 8;
                    nrow = row;
                    ncol = col;
                    tmp[row][col] = tmp[r][c];
                    tmp[r][c] = "·";

                    rnd++;
                    // Pawn Promotion
                    if (cmp(board[r][c], "♙") && row == 0)
                    {
                        strcpy(str, "Promote as [r / n / b / q]");
                        is_promotion = true;
                    }
                    // king Castling
                    if (cmp(board[r][c], "♔"))
                        king_moved = true;
                    if (cmp(board[r][c], "♖") && c == 0)
                        left_rook_moved = true;
                    if (cmp(board[r][c], "♖") && c == 7)
                        right_rook_moved = true;
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
                        tmp[row + 1][col] = "·";
                    else if (ep == 1 && col == c + 1)
                        tmp[row - 1][col] = "·";

                    cout(tmp, str);

                    char cmd[1000];
                    snprintf(cmd, sizeof(cmd), "python3 ai.py \"%s\"", req);
                    printf("\nThinking...\n");
                    FILE *fp = popen(cmd, "r");
                    if (fp != NULL)
                    {
                        fgets(ai_move, sizeof(ai_move), fp);
                        pclose(fp);
                        strcpy(memory[cnt], ai_move);
                    }   else break;
                    clearScreen();
                }
                if (type(board, row, col) == 2) {
                    try = 0;
                    cnt++;
                }
                if (type(board, row, col) != 1)
                    copyBoard(tmp, board);
            }
        }

        if (!my_turn)
        {
            /*************
                AI MOVE
            **************/
            
            ai_move[strlen(ai_move) - 1] = '\0';

            char from[3];
            strncpy(from, ai_move, 2);
            from[2] = '\0';
            int idx_from = getPos(from);
            int row_from = idx_from / 8;
            int col_from = idx_from % 8;
             ai_row_from = row_from;
             ai_col_from = col_from;

            char to[3];
            strncpy(to, ai_move + strlen(ai_move) - 2, 2);
            to[2] = '\0';
            int idx_to = getPos(to);
            int row_to = idx_to / 8;
            int col_to = idx_to % 8;
             ai_row_to = row_to;
             ai_col_to = col_to;

            char t[3];
            strncpy(t, ai_move + strlen(ai_move) - 4, 1);
            t[1] = '\0';

            // Castling
            bool king_castling = false;
            bool queen_castling = false;

            if (cmp(from, "e8") && cmp(to, "c8") && cmp(tmp[row_from][col_from], "♚")) {
                tmp[0][2] = "♚";
                tmp[0][3] = "♜";
                tmp[0][0] = "·";
                tmp[0][4] = "·";
                queen_castling = true;
            } else
            if (cmp(from, "e8") && cmp(to, "g8") && cmp(tmp[row_from][col_from], "♚")) {
                tmp[0][6] = "♚";
                tmp[0][5] = "♜";
                tmp[0][7] = "·";
                tmp[0][4] = "·";
                king_castling = true;
            }

            // En Passant
            if (
            (col_from-1 >= 0
            && cmp(tmp[row_from][col_from-1], "♙")
            && cmp(tmp[row_from][col_from], "♟")
            && row_to == row_from+1 && col_to == col_from-1
            )
            ||
            (col_from+1 < 8
            && cmp(tmp[row_from][col_from+1], "♙")
            && cmp(tmp[row_from][col_from], "♟")
            && row_to == row_from+1 && col_to == col_from+1
            )
            ) tmp[row_from][col_to] = "·";

            // Promotion
            if (cmp(t, "r"))
                tmp[row_to][col_to] = "♜";
            else if (cmp(t, "n"))
                tmp[row_to][col_to] = "♞";
            else if (cmp(t, "b"))
                tmp[row_to][col_to] = "♝";
            else if (cmp(t, "q"))
                tmp[row_to][col_to] = "♛";
            if (!king_castling && !queen_castling)
            {
                tmp[row_to][col_to] = tmp[row_from][col_from];
                tmp[row_from][col_from] = "·";
            }

            copyBoard(tmp, board);
            my_turn = !my_turn;
        }

        if (cmp(input, "x"))
            return 0;  
    } 
}
