#include <iostream>
#include <algorithm>

#define BOARD_SIZE 8

using namespace std;

enum Status {
    white,
    black,
    space
};

typedef struct Pos {
    int x, y;
} Pos;

Status board[BOARD_SIZE][BOARD_SIZE];
Status player = black;

bool isPieceStatus(Pos pos, Status piece)
{
    if (0 <= pos.x && pos.x < BOARD_SIZE &&
        0 <= pos.y && pos.y < BOARD_SIZE)
        return board[pos.x][pos.y] == piece;

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

bool checkCanPut(Pos pos, Status piece)
{
    // not empty
    if (board[pos.x][pos.y] != space)
        return false;


    Status opponent = piece == white ? black : white;
    bool found = false;
    for (int i = 0; i < 8; i++) {
        // 789
        // 4 6
        // 123
        // must at least one opponent piece
        Pos checkPos = {pos.x + direction[i].x, pos.y + direction[i].y};

        if (!isPieceStatus(checkPos, opponent))
            continue;

        // must at least one self piece at direction
        for (int length = 2; length < BOARD_SIZE; length++) {
            Pos checkPos = {pos.x + length * direction[i].x,
                pos.y + length * direction[i].y};

            if (isPieceStatus(checkPos, space))
                break;

            if (isPieceStatus(checkPos, piece))
                found = true;
        }
    }

    if (found == false)
        return false;

    return true;
}

void put(Pos pos, Status piece)
{
    // reverse
    // must at least one self piece at direction
    for (int i = 0; i < 8; i++) {
        int foundLength = -1;
        for (int length = 1; length < BOARD_SIZE; length++) {
            Pos checkPos = {pos.x + length * direction[i].x,
                pos.y + length * direction[i].y};

            if (isPieceStatus(checkPos, space))
                break;

            if (isPieceStatus(checkPos, piece)) {
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
                board[revPos.x][revPos.y] = piece;
            }
        }
    }

    // set status
    board[pos.x][pos.y] = piece;
}

bool input()
{
    cout << "You are " << (player == black ? "black" : "white") << endl;
    cout << "Please input a position(ex: 0 3):" << endl;
    Pos pos;
    cin >> pos.x >> pos.y;

    if (!cin)
        return false;

    while (!checkCanPut(pos, player)) {
        cout << "can't put!" << endl;
        cin >> pos.x >> pos.y;
    }

    put(pos, player);

    return true;
}

bool isPass(Status piece)
{
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++)
            if (checkCanPut({ x, y }, piece))
                return false;
    }

    return true;
}

bool isGameOver()
{
    return isPass(white) && isPass(black);
}

void display()
{
    cout << " 01234567" << endl;
    for (int y = 0; y < BOARD_SIZE; y++) {
        cout << y;
        for (int x = 0; x < BOARD_SIZE; x++)
            cout << (board[x][y] == space ? " " : board[x][y] == black ? "○" : "●");
//□
        cout << endl;
    }
}

int main()
{
    // initialztion
    fill(&board[0][0], &board[0][0] + BOARD_SIZE * BOARD_SIZE, space);
    board[3][3] = board[4][4] = white;
    board[4][3] = board[3][4] = black;

    // loop
    display();
    while (input()) {

        display();
        if (isGameOver()) {
            cout << "gameover" << endl;
            break;
        }

        player = player == white ? black : white;
        if (isPass(player)) {
            cout << "pass" << endl;
            player = player == white ? black : white;
        }
        // computer()
    }

    // count
    int whiteCount = 0, blackCount = 0;
     for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++)
            if (board[x][y] != space) {
                if (board[x][y] == white)
                    whiteCount++;
                else
                    blackCount++;
            }
    }

    // who win
    if (whiteCount > blackCount)
        cout << "white win" << endl;
    else if (whiteCount < blackCount)
        cout << "black win" << endl;
    else
        cout << "nobody win" << endl;
}
