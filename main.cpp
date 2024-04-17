#include <iostream>
#include <algorithm>

/*
g++ -std=c++11 -o reversi main.cpp
*/

#define BOARD_SIZE 8

using namespace std;

enum Status {
    white,
    black,
    space,
    puttable,
};

typedef struct Pos {
    int x, y;
} Pos;

typedef enum Status BoardArray[BOARD_SIZE][BOARD_SIZE];

bool isPieceStatus(Pos pos, Status piece, BoardArray *board)
{
    if (0 <= pos.x && pos.x < BOARD_SIZE &&
        0 <= pos.y && pos.y < BOARD_SIZE)
        return (*board)[pos.x][pos.y] == piece;

    return false;
}

Pos direction[8] = {
    {1, 1},
    {1, 0},
    {1, -1},
    {0, -1},
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, 1},
};

void count_game(BoardArray *board, int *whiteCount, int *blackCount);

bool checkCanPut(Pos pos, Status piece, BoardArray *board)
{
    // not empty
    if ((*board)[pos.x][pos.y] != space)
        return false;


    Status opponent = piece == white ? black : white;
    bool found = false;
    for (int i = 0; i < 8; i++) {
        // 789
        // 4 6
        // 123
        // must at least one opponent piece
        Pos checkPos = {pos.x + direction[i].x, pos.y + direction[i].y};

        if (!isPieceStatus(checkPos, opponent, board))
            continue;

        // must at least one self piece at direction
        for (int length = 2; length < BOARD_SIZE; length++) {
            Pos checkPos = {pos.x + length * direction[i].x,
                pos.y + length * direction[i].y};

            if (isPieceStatus(checkPos, space, board))
                break;

            if (isPieceStatus(checkPos, piece, board))
                found = true;
        }
    }

    if (found == false)
        return false;

    return true;
}

void put(Pos pos, Status piece, BoardArray *board)
{
    // reverse
    // must at least one self piece at direction
    for (int i = 0; i < 8; i++) {
        int foundLength = -1;
        for (int length = 1; length < BOARD_SIZE; length++) {
            Pos checkPos = {pos.x + length * direction[i].x,
                pos.y + length * direction[i].y};

            if (isPieceStatus(checkPos, space, board))
                break;

            if (isPieceStatus(checkPos, piece, board)) {
                foundLength = length;
                break;
            }
        }

        // if found then reverse!
        if (foundLength != -1) {
            for (int length = 1; length < foundLength; length++) {
                Pos revPos = {pos.x + length * direction[i].x,
                    pos.y + length * direction[i].y};

                // set status
                (*board)[revPos.x][revPos.y] = piece;
            }
        }
    }

    // set status
    (*board)[pos.x][pos.y] = piece;
}

string put_prompt = "Please input a position(ex: 0 3):";
bool input(Status player, BoardArray *board)
{
    cout << "You are " << (player == black ? "black" : "white") << endl;
    cout << put_prompt << endl;
    Pos pos;
    cin >> pos.x >> pos.y;

    if (!cin)
        return false;

    while (!checkCanPut(pos, player, board)) {
        cout << "can't put!" << endl;
        cout << put_prompt << endl;
        cin >> pos.x >> pos.y;
    }

    cout << "put at " << pos.x << " " << pos.y << endl;

    put(pos, player, board);

    return true;
}

bool isPass(Status piece, BoardArray *board)
{
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++)
            if (checkCanPut({ x, y }, piece, board))
                return false;
    }

    return true;
}

bool isGameOver(BoardArray *board)
{   
    bool all_player_pass = isPass(white, board) && isPass(black, board);
    int whiteCount = 0, blackCount = 0;
    count_game(board, &whiteCount, &blackCount);
    bool no_stone = whiteCount == 0 || blackCount == 0;
    return all_player_pass || no_stone;
}

void find_puttable(BoardArray *src, BoardArray *dst, Status player)
{
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++)
            (*dst)[x][y] = checkCanPut({ x, y }, player, src) ? puttable : (*src)[x][y];
    }
}

void display(BoardArray *show_board, Status player)
{
    cout << "= display board =" << endl;

    // find all puttable
    BoardArray display_board;
    find_puttable(show_board, &display_board, player);

    cout << " 01234567" << endl;
    for (int y = 0; y < BOARD_SIZE; y++) {
        cout << y;
        for (int x = 0; x < BOARD_SIZE; x++)
            // NOTE: we assume terminal is black background, so ○ is black
            if (display_board[x][y] == puttable)
                cout << "·";
            else
                cout << ((*show_board)[x][y] == space ? " " : (*show_board)[x][y] == black ? "○" : "●");
        cout << endl;
    }
}

void count_game(BoardArray *board, int *whiteCount, int *blackCount)
{
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if ((*board)[x][y] == space)
                continue;
            
            if ((*board)[x][y] == white)
                *whiteCount += 1;
            else
                *blackCount += 1;
        }       
    }
}

void find_best_puttable(BoardArray *board, Pos *putPos, Status bot_player)
{
    int maxCount = 0;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (!checkCanPut({ x, y }, bot_player, board))
                continue;

            BoardArray tmp_board;
            copy(&(*board)[0][0], &(*board)[0][0] + BOARD_SIZE * BOARD_SIZE, &tmp_board[0][0]);
            put({ x, y }, bot_player, &tmp_board);

            int whiteCount = 0, blackCount = 0;
            count_game(&tmp_board, &whiteCount, &blackCount);

            int count = bot_player == white ? whiteCount : blackCount;
            if (count > maxCount) {
                maxCount = count;
                putPos->x = x;
                putPos->y = y;
            }

            // reset
            copy(&(*board)[0][0], &(*board)[0][0] + BOARD_SIZE * BOARD_SIZE, &tmp_board[0][0]);
        }
    }
}

void computer(BoardArray *board, Status bot_player)
{
    Pos putPos = {0, 0};

    find_best_puttable(board, &putPos, bot_player);

    if (!checkCanPut(putPos, bot_player, board))
        abort();
    
    put(putPos, bot_player, board);
}

int main()
{
    BoardArray board;
    Status current_player = black;
    Status input_player = black;
    
    // initialztion
    fill(&board[0][0], &board[0][0] + BOARD_SIZE * BOARD_SIZE, space);
    board[3][3] = board[4][4] = white;
    board[4][3] = board[3][4] = black;

    // loop
    display(&board, current_player);
    while (!isGameOver(&board)) {
        if (current_player == input_player) {
            while(!input(current_player, &board))
                ;
        } else {
            cout << "computer" << endl;
            computer(&board, current_player);
        }

        current_player = current_player == white ? black : white;
        if (isPass(current_player, &board)) {
            cout << "pass" << endl;
            current_player = current_player == white ? black : white;
        }

        display(&board, current_player);
    }

    cout << "gameover" << endl;

    // count
    int whiteCount = 0, blackCount = 0;
    count_game(&board, &whiteCount, &blackCount);

    // who win
    if (whiteCount > blackCount)
        cout << "white win" << endl;
    else if (whiteCount < blackCount)
        cout << "black win" << endl;
    else
        cout << "nobody win" << endl;
    cout << "white:" << whiteCount << endl;
    cout << "black:" << blackCount << endl;
}
