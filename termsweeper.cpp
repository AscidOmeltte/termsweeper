#include <bits/stdc++.h>
#include <iomanip>
#include <chrono>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

int boardSize, mines;
vector<vector<char>> board, revealed;
int score;
int highScore = 0;
const string SCORE_FILE = "highscore.txt";

int cursorR = 0, cursorC = 0;
bool firstMove = true;
string currentDifficulty;
chrono::time_point<chrono::steady_clock> startTime;

/* ---------- Terminal helpers ---------- */

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

/* ---------- High score I/O ---------- */

void loadHighScore() {
    ifstream file(SCORE_FILE);
    if (file.is_open()) {
        file >> highScore;
        file.close();
    } else {
        highScore = 0;
    }
}

void saveHighScore() {
    ofstream file(SCORE_FILE);
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

/* ---------- Input handling ---------- */

enum Key {
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_FLAG,
    KEY_NONE
};

#ifdef _WIN32
Key readKey() {
    int ch = _getch();
    if (ch == 224) {
        ch = _getch();
        if (ch == 72) return KEY_UP;
        if (ch == 80) return KEY_DOWN;
        if (ch == 75) return KEY_LEFT;
        if (ch == 77) return KEY_RIGHT;
    } else if (ch == 13) {
        return KEY_ENTER;
    } else if (ch == 'f' || ch == 'F') {
        return KEY_FLAG;
    }
    return KEY_NONE;
}
#else
Key readKey() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch;
    read(STDIN_FILENO, &ch, 1);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    if (ch == '\033') {
        char seq[2];
        read(STDIN_FILENO, &seq[0], 1);
        read(STDIN_FILENO, &seq[1], 1);
        if (seq[1] == 'A') return KEY_UP;
        if (seq[1] == 'B') return KEY_DOWN;
        if (seq[1] == 'C') return KEY_RIGHT;
        if (seq[1] == 'D') return KEY_LEFT;
    } else if (ch == '\n') {
        return KEY_ENTER;
    } else if (ch == 'f' || ch == 'F') {
        return KEY_FLAG;
    }

    return KEY_NONE;
}
#endif

/* ---------- Game logic ---------- */

int countFlags() {
    int flags = 0;
    for (int r = 0; r < boardSize; r++)
        for (int c = 0; c < boardSize; c++)
            if (revealed[r][c] == 'F')
                flags++;
    return flags;
}

void floodFill(int r, int c) {
    if (!isValid(r, c) || revealed[r][c] != 'H') return;

    int count = 0;
    for (int dr = -1; dr <= 1; dr++)
        for (int dc = -1; dc <= 1; dc++)
            if (isValid(r + dr, c + dc) && board[r + dr][c + dc] == '*')
                count++;

    cout << "\a";

    if (count == 0) {
        revealed[r][c] = '~';
        score++;
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++)
                if (dr || dc) floodFill(r + dr, c + dc);
    } else {
        revealed[r][c] = '0' + count;
        score++;
    }
}

void placeMines(int firstR, int firstC) {
    srand(time(0));
    int placed = 0;
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

/* ---------- Rendering ---------- */

void printBoard() {
    clearScreen();
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(now - startTime).count();

    cout << "Difficulty: " << currentDifficulty
         << " | Time: " << elapsed << "s"
         << " | Score: " << score
         << " | Flags: " << countFlags() << "/" << mines
         << "\n\n";

    for (int r = 0; r < boardSize; r++) {
        for (int c = 0; c < boardSize; c++) {
            if (r == cursorR && c == cursorC && revealed[r][c] == 'H')
                cout << "[#]";
            else if (revealed[r][c] == 'H')
                cout << "[ ]";
            else
                cout << "[" << revealed[r][c] << "]";
        }
        cout << "\n";
    }
}

/* ---------- Game loop ---------- */

void startGame(int s, int m) {
    boardSize = s;
    mines = m;
    board.assign(boardSize, vector<char>(boardSize, '0'));
    revealed.assign(boardSize, vector<char>(boardSize, 'H'));
    score = 0;
    cursorR = cursorC = 0;
    firstMove = true;
    startTime = chrono::steady_clock::now();

    while (true) {
        printBoard();
        Key k = readKey();

        if (k == KEY_UP && cursorR > 0) cursorR--;
        else if (k == KEY_DOWN && cursorR < boardSize - 1) cursorR++;
        else if (k == KEY_LEFT && cursorC > 0) cursorC--;
        else if (k == KEY_RIGHT && cursorC < boardSize - 1) cursorC++;

        else if (k == KEY_FLAG) {
            cout << "\a";
            if (revealed[cursorR][cursorC] == 'H')
                revealed[cursorR][cursorC] = 'F';
            else if (revealed[cursorR][cursorC] == 'F')
                revealed[cursorR][cursorC] = 'H';
        }

        else if (k == KEY_ENTER) {
            if (revealed[cursorR][cursorC] == 'F')
                continue;

            if (firstMove) {
                placeMines(cursorR, cursorC);
                firstMove = false;
            }

            if (board[cursorR][cursorC] == '*') {
                revealed[cursorR][cursorC] = '*';
                printBoard();
                cout << "\nYou hit a mine! Game over.\nPress Enter...";
                cin.ignore();
                cin.get();
                return;
            }

            floodFill(cursorR, cursorC);
        }

        if (checkWin()) {
            clearScreen();
            cout << "\aYou win!\nScore: " << score << "\n";

            if (score > highScore) {
                highScore = score;
                saveHighScore();
                cout << "NEW HIGH SCORE!\n";
            }

            cout << "Press Enter...";
            cin.ignore();
            cin.get();
            return;
        }
    }
}

/* ---------- Tutorial ---------- */

void showTutorial() {
    clearScreen();
    cout << "=== TERMSWEEPER TUTORIAL ===\n\n";
    cout << "Arrow Keys  : Move cursor\n";
    cout << "Enter       : Reveal tile\n";
    cout << "F           : Flag / unflag tile\n\n";
    cout << "Numbers show how many mines are near you.\n";
    cout << "Clear all safe tiles to win.\n";
    cout << "Flags are useless unless your dumb btw.\n\n";
    cout << "Press Enter to return to menu...";
    cin.ignore();
    cin.get();
}

/* ---------- Menu ---------- */

void mainMenu() {
    while (true) {
        clearScreen();
        cout << "High Score: " << highScore << "\n\n";

        cout << R"(
\\                  ║                 //                                                          
  \\\          /═════════\          //                                                            
     \\     ///           \\\    ///                                                              
       \\ //    ∙            \\//                                                                 
        //              ∙      \\                                                                 
       /                         \                                                                
       /░ ∙     ∙           ∙    \                                                                
      │░░             ∙           \                                                           
══════│▒░░                        ═════════                                                       
      \▓▒░░     ∙             ∙  (_______)                                                        
       \▓▒▒▒░░           ∙       /   _ _____  ____ ____   ___ _ _ _ _____ _____ ____  _____  ____ 
       \▓▓▓▒▒░░░                 /  | | ___ |/ ___|    \ /___| | | | ___ | ___ |  _ \| ___ |/ ___)
        \\▓▓▓▒▒▒▒░░            //\\ | | ____| |   | | | |___ | | | | ____| ____| |_| | ____| |    
       // \\▓▓▓▓▒▒▒░░░       //    \|_|_____|_|   |_|_|_(___/ \___/|_____|_____|  __/|_____|_|    
    ///     \\\▓▓▓▓▒▒▒░░░░///        \\                                        |_|                
  //           \═════════/             \\                                                         
                    ║                                                                             
)" << "\n";

        cout << "1) Easy\n2) Medium\n3) Hard\n4) Demon\n5) Tutorial\n6) Quit\n";
        int choice;
        cin >> choice;

        if (choice == 1) { currentDifficulty = "Easy"; startGame(8, 10); }
        else if (choice == 2) { currentDifficulty = "Medium"; startGame(16, 40); }
        else if (choice == 3) { currentDifficulty = "Hard"; startGame(32, 160); }
        else if (choice == 4) { currentDifficulty = "Demon"; startGame(56, 800); }
        else if (choice == 5) showTutorial();
        else if (choice == 6) break;
    }
}

/* ---------- Main ---------- */

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    loadHighScore();
    mainMenu();
    return 0;
}
