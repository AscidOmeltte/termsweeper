#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using clock_type = chrono::steady_clock;

/* ===================== UTIL ===================== */

void clearScreen() {
    system("clear");
}

void printLogo() {
    cout <<
    "████████╗███████╗██████╗ ███╗   ███╗\n"
    "╚══██╔══╝██╔════╝██╔══██╗████╗ ████║\n"
    "   ██║   █████╗  ██████╔╝██╔████╔██║\n"
    "   ██║   ██╔══╝  ██╔══██╗██║╚██╔╝██║\n"
    "   ██║   ███████╗██║  ██║██║ ╚═╝ ██║\n"
    "   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝\n"
    "        T E R M S W E E P E R\n\n";
}

long elapsedSeconds(clock_type::time_point start) {
    return chrono::duration_cast<chrono::seconds>(
        clock_type::now() - start
    ).count();
}

/* ===================== BOARD ===================== */

int countMines(const vector<vector<bool>>& mines, int r, int c, int size) {
    int count = 0;
    for (int dr = -1; dr <= 1; dr++)
        for (int dc = -1; dc <= 1; dc++) {
            int nr = r + dr;
            int nc = c + dc;
            if (nr >= 0 && nr < size && nc >= 0 && nc < size && mines[nr][nc])
                count++;
        }
    return count;
}

void floodFill(vector<vector<char>>& board, const vector<vector<bool>>& mines, int r, int c, int size) {
    if (r < 0 || r >= size || c < 0 || c >= size) return;
    if (board[r][c] != 'H') return;

    int nearby = countMines(mines, r, c, size);
    if (nearby > 0) {
        board[r][c] = nearby + '0';
        return;
    }

    board[r][c] = '~';

    for (int dr = -1; dr <= 1; dr++)
        for (int dc = -1; dc <= 1; dc++)
            if (dr != 0 || dc != 0)
                floodFill(board, mines, r + dr, c + dc, size);
}

/* ===================== DISPLAY ===================== */

void printBoard(const vector<vector<char>>& board, int size, long seconds) {
    cout << "Time: " << seconds << " seconds\n\n";

    cout << "   ";
    for (int i = 0; i < size; i++)
        cout << (i % 10) << "  ";
    cout << "\n";

    for (int r = 0; r < size; r++) {
        cout << (r % 10) << "  ";
        for (int c = 0; c < size; c++) {
            if (board[r][c] == 'H') cout << "[ ]";
            else if (board[r][c] == '~') cout << "[~]";
            else if (board[r][c] == '*') cout << "[*]";
            else cout << "[" << board[r][c] << "]";
        }
        cout << "\n";
    }
}

/* ===================== WIN SCREEN ===================== */

void winScreen(long seconds) {
    clearScreen();
    cout <<
    "░░░░░░░░░░░███████░░░░░░░░░░░\n"
    "░░░░░░░████░░░░░░░████░░░░░░░\n"
    "░░░░░██░░░░░░░░░░░░░░░██░░░░░\n"
    "░░░██░░░░░░░░░░░░░░░░░░░██░░░\n"
    "░░█░░░░░░░░░░░░░░░░░░░░░░░█░░\n"
    "░█░░████░░░░░░░░██████░░░░░█░\n"
    "█░░█░░░██░░░░░░█░░░░███░░░░░█\n"
    "█░█░░░░░░█░░░░░█░░░░░░░█░░░░█\n"
    "█░█████████░░░░█████████░░░░█\n"
    "█░░░░░░░░░░░░░░░░░░░░░░░░░░░█\n"
    "█░░░░░░░░░░░░░░░░░░░░░░░░░░░█\n"
    "█░░░████████████████████░░░░█\n"
    "░█░░░█▓▓▓▓▓▓▓▓█████▓▓▓█░░░░█░\n"
    "░█░░░░█▓▓▓▓▓██░░░░██▓██░░░░█░\n"
    "░░█░░░░██▓▓█░░░░░░░▒██░░░░█░░\n"
    "░░░██░░░░██░░░░░░▒██░░░░██░░░\n"
    "░░░░░██░░░░███████░░░░██░░░░░\n"
    "░░░░░░░███░░░░░░░░░███░░░░░░░\n"
    "░░░░░░░░░░█████████░░░░░░░░░░\n\n";

    cout << "🎉 YOU WIN! 🎉\n";
    cout << "Time: " << seconds << " seconds\n\n";
    cout << "Press ENTER to return to menu...";
    cin.ignore();
    cin.get();
}

/* ===================== GAME ===================== */

void startGame(int size, int mineCount) {
    vector<vector<bool>> mines(size, vector<bool>(size, false));
    vector<vector<char>> board(size, vector<char>(size, 'H'));

    srand(time(nullptr));

    int placed = 0;
    while (placed < mineCount) {
        int r = rand() % size;
        int c = rand() % size;
        if (!mines[r][c]) {
            mines[r][c] = true;
            placed++;
        }
    }

    int safeTiles = size * size - mineCount;
    auto startTime = clock_type::now();

    while (true) {
        clearScreen();
        printLogo();
        printBoard(board, size, elapsedSeconds(startTime));

        // Check win
        int revealed = 0;
        for (int r = 0; r < size; r++)
            for (int c = 0; c < size; c++)
                if (board[r][c] != 'H')
                    revealed++;
        if (revealed == safeTiles) {
            winScreen(elapsedSeconds(startTime));
            return;
        }

        cout << "\nEnter row and column: ";

        int r, c;
        if (!(cin >> r >> c)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (r < 0 || r >= size || c < 0 || c >= size)
            continue;

        if (mines[r][c]) {
            // Reveal all mines
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                    if (mines[i][j])
                        board[i][j] = '*';
            clearScreen();
            printLogo();
            printBoard(board, size, elapsedSeconds(startTime));
            cout << "\n💥 Game Over!\n";
            cout << "Press ENTER to return to menu...";
            cin.ignore();
            cin.get();
            return;
        }

        int nearby = countMines(mines, r, c, size);
        if (nearby == 0)
            floodFill(board, mines, r, c, size);
        else
            board[r][c] = nearby + '0';
    }
}

/* ===================== MENUS ===================== */

void optionsMenu() {
    while (true) {
        clearScreen();
        printLogo();

        cout << "1) Easy   (8x8, 10 mines)\n";
        cout << "2) Medium (16x16, 40 mines)\n";
        cout << "3) Hard   (32x32, 160 mines)\n";
        cout << "4) Back\n\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        if (choice == 1) startGame(8, 10);
        else if (choice == 2) startGame(16, 40);
        else if (choice == 3) startGame(32, 160);
        else if (choice == 4) break;
    }
}

void mainMenu() {
    while (true) {
        clearScreen();
        printLogo();

        cout << "1) Start Game\n";
        cout << "2) Quit\n\n";
        cout << "Select: ";

        int choice;
        cin >> choice;

        if (choice == 1) optionsMenu();
        else if (choice == 2) break;
    }
}

/* ===================== MAIN ===================== */

int main() {
    mainMenu();
    return 0;
}
