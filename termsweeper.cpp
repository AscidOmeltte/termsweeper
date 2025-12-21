#include <bits/stdc++.h>
#include <iomanip>
#include <chrono>
#include <fstream>
using namespace std;

int boardSize, mines;
vector<vector<char>> board, revealed;
int score;
chrono::time_point<chrono::steady_clock> startTime;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

bool isValid(int r, int c) {
    return r >= 0 && r < boardSize && c >= 0 && c < boardSize;
}

void floodFill(int r, int c) {
    if (!isValid(r, c) || revealed[r][c] != 'H') return;

    int count = 0;
    for (int dr = -1; dr <= 1; dr++)
        for (int dc = -1; dc <= 1; dc++)
            if (isValid(r + dr, c + dc) && board[r + dr][c + dc] == '*')
                count++;

    if (count == 0) {
        revealed[r][c] = '~';
        score++;
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++)
                if (dr != 0 || dc != 0) floodFill(r + dr, c + dc);
    } else {
        revealed[r][c] = '0' + count;
        score++;
    }
}

void placeMines(int firstR, int firstC) {
    int placed = 0;
    srand(time(0));
    while (placed < mines) {
        int r = rand() % boardSize;
        int c = rand() % boardSize;
        if (board[r][c] == '*' || (r == firstR && c == firstC)) continue;
        board[r][c] = '*';
        placed++;
    }
}

bool checkWin() {
    for (int r = 0; r < boardSize; r++)
        for (int c = 0; c < boardSize; c++)
            if (revealed[r][c] == 'H' && board[r][c] != '*')
                return false;
    return true;
}

void printBoard() {
    clearScreen();
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(now - startTime).count();
    cout << "Time: " << elapsed << "s  |  Score: " << score << "\n\n";

    cout << "   ";
    for (int c = 0; c < boardSize; c++) cout << setw(2) << c << " ";
    cout << "\n";

    for (int r = 0; r < boardSize; r++) {
        cout << setw(2) << r << " ";
        for (int c = 0; c < boardSize; c++) {
            if (revealed[r][c] == 'H') cout << "[ ]";
            else if (revealed[r][c] == '~') cout << "[~]";
            else if (revealed[r][c] == '*') cout << "[*]";
            else cout << "[" << revealed[r][c] << "]";
        }
        cout << "\n";
    }
}

void startGame(int s, int m) {
    boardSize = s;
    mines = m;
    board.assign(boardSize, vector<char>(boardSize, '0'));
    revealed.assign(boardSize, vector<char>(boardSize, 'H'));
    score = 0;
    startTime = chrono::steady_clock::now();

    int firstR, firstC;
    printBoard();
    cout << "\nEnter first row and column: ";
    cin >> firstR >> firstC;

    placeMines(firstR, firstC);
    floodFill(firstR, firstC);

    while (true) {
        printBoard();
        int r, c;
        cout << "\nEnter row and column: ";
        cin >> r >> c;

        if (!isValid(r, c)) continue;
        if (board[r][c] == '*') {
            revealed[r][c] = '*';
            printBoard();
            cout << "\nYou hit a mine! Game over.\nPress Enter to return to menu...";
            cin.ignore();
            cin.get();
            return;
        }

        floodFill(r, c);

        if (checkWin()) {
            clearScreen();
            cout << R"(
           ███████
       ████░░░░░░░████
     ██░░░░░░░░░░░░░░░██
   ██░░░░░░░░░░░░░░░░░░░██
  █░░░░░░░░░░░░░░░░░░░░░░░█
 █░░████░░░░░░░░██████░░░░░█
█░░█░░░██░░░░░░█░░░░███░░░░░█
█░█░░░░░░█░░░░░█░░░░░░░█░░░░█
█░█████████░░░░░█████████░░░░█
█░░░░░░░░░░░░░░░░░░░░░░░░░░░█
█░░░░░░░░░░░░░░░░░░░░░░░░░░░█
█░░░████████████████████░░░░█
 █░░░█▓▓▓▓▓▓▓▓█████▓▓▓█░░░░█
 █░░░░█▓▓▓▓▓██░░░░██▓██░░░░█
  █░░░░██▓▓█░░░░░░░▒██░░░░█
   ██░░░░██░░░░░░▒██░░░░██
     ██░░░░███████░░░░██
       ███░░░░░░░░░███
          █████████
)" << "\n";

            int multiplier = 1;
            if (boardSize == 8) multiplier = 1;
            else if (boardSize == 16) multiplier = 2;
            else if (boardSize == 32) multiplier = 3;
            else if (boardSize == 56) multiplier = 5;

            int finalScore = score * multiplier;
            cout << "\nYour Score: " << finalScore << "\n";

            int highScore = 0;
            ifstream fin("highscore.txt");
            if (fin) fin >> highScore;
            fin.close();

            if (finalScore > highScore) {
                ofstream fout("highscore.txt");
                fout << finalScore;
                fout.close();
                cout << "New High Score!\n";
            } else {
                cout << "High Score: " << highScore << "\n";
            }

            cout << "Press Enter to return to menu...";
            cin.ignore();
            cin.get();
            return;
        }
    }
}

void showTutorial() {
    clearScreen();
    cout << "=== TERMSWEEPER TUTORIAL ===\n\n";
    cout << "Welcome to Termsweeper!\n";
    cout << "- The board is made of tiles: [ ] = hidden, [~] = empty, [1-8] = number of adjacent mines.\n";
    cout << "- Enter the row and column numbers to reveal a tile.\n";
    cout << "- Avoid mines: if you reveal a [*], you lose.\n";
    cout << "- Revealing an empty tile will flood-fill adjacent empty tiles automatically.\n";
    cout << "- The score counts how many safe tiles you reveal, with difficulty multipliers.\n";
    cout << "- Win by revealing all safe tiles. Your score is displayed at the top.\n\n";
    cout << "Press Enter to return to the main menu...";
    cin.ignore();
    cin.get();
}

void mainMenu() {
    while (true) {
        clearScreen();

        // ASCII logo
        cout << R"( _______  _______  _______  _______  _______
████████╗███████╗██████╗ ███╗   ███╗███████╗██╗    ██╗███████╗███████╗██████╗ ███████╗██████╗ 
╚══██╔══╝██╔════╝██╔══██╗████╗ ████║██╔════╝██║    ██║██╔════╝██╔════╝██╔══██╗██╔════╝██╔══██╗
   ██╔╝   █████╗  ██████╔╝██╔████╔██║███████╗██║ █╗ ██║█████╗  █████╗  ██████╔╝█████╗  ██████╔╝
   ██║    ██╔══╝  ██╔══██╗██║╚██╔╝██║╚════██║██║███╗██║██╔══╝  ██╔══╝  ██╔══╝  ██╔══╝  ██╔══██╗
   ██║    ███████╗██║  ██║██║ ╚═╝ ██║███████║╚███╔███╔╝███████╗███████╗██║     ███████╗██║  ██║
   ╚═╝    ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝ ╚══╝╚══╝ ╚══════╝╚══════╝╚═╝    ╚══════╝╚═╝  ╚═╝
)" << "\n";

        // Display high score
        int highScore = 0;
        ifstream fin("highscore.txt");
        if (fin) fin >> highScore;
        fin.close();
        cout << "High Score: " << highScore << "\n\n";
        cout << "Select difficulty:\n";
        cout << "1) Easy   (8x8, 10 mines)\n";
        cout << "2) Medium (16x16, 40 mines)\n";
        cout << "3) Hard   (32x32, 160 mines)\n";
        cout << "4) Demon  (56x56, 800 mines)\n";
        cout << "5) Tutorial\n";
        cout << "6) Quit\n";

        int choice;
        cin >> choice;
        if (choice == 1) startGame(8, 10);
        else if (choice == 2) startGame(16, 40);
        else if (choice == 3) startGame(32, 160);
        else if (choice == 4) startGame(56, 800);
        else if (choice == 5) showTutorial();
        else if (choice == 6) break;
    }
}

int main() {
    mainMenu();
    return 0;
}
