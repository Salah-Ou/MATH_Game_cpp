#include "utils.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

using namespace std;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        cout << "\033[2J\033[H" << flush;
    #endif
}

void Pause() {
#ifdef _WIN32
    system("pause");
#else
    cin.get();
#endif
}


void mySleep(int milliseconds) {
    #ifdef _WIN32
        Sleep(milliseconds);
    #else
        usleep(milliseconds * 1000);
    #endif
}

void gotoxy(int x, int y) {
#ifdef _WIN32
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
#else
    cout << "\033[" << y + 1 << ";" << x + 1 << "H";
#endif
}

// ── Background clock ─────────────────────────────────────────────────────────
// Runs on its own thread so it never blocks the game.
// Uses ANSI save/restore cursor (\033[s ... \033[u) so writing the clock
// at row 1 doesn't disturb whatever the game is printing.

static atomic<bool> g_running{false};
static thread       g_clockThread;
static mutex        g_ioMutex;

static void drawClock() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm tm = *localtime(&t);

    lock_guard<mutex> lk(g_ioMutex);
    cout << "\0337"            // save cursor
            << "\033[3;80H"       // go to row 1, col 65
            << "\033[2K"         // clear that line from cursor
            << setfill('0')
            << setw(2) << "Current time : "
            << setw(2) << tm.tm_hour << ":"
            << setw(2) << tm.tm_min  << ":"
            << setw(2) << tm.tm_sec
            << "\0338"            // restore cursor
            << flush;
}

void startClock() {
    if (g_running.exchange(true)) return;

    // Reserve row 1 for the clock; game text scrolls in rows 2..bottom.
    cout << "\033[2J"        // clear screen
            << "\033[2;r"       // set scroll region: row 2 to bottom
            << "\033[2;1H"      // move cursor into the scroll region
            << flush;

    g_clockThread = thread([] {
        while (g_running.load()) {
            drawClock();
            this_thread::sleep_for(chrono::seconds(1));
        }
    });
}

void stopClock() {
    if (!g_running.exchange(false)) return;
    if (g_clockThread.joinable()) g_clockThread.join();

    // Release the scroll region so the terminal behaves normally again.
    cout << "\033[r" << flush;
}

// ─────────────────────────────────────────────────────────────────────────────
// getInputWithTimeout
// Reads an integer from the user within a time limit.
// Shows a live countdown in the header while the user types.
// Returns true if the user entered a valid number, false if time ran out.
// ─────────────────────────────────────────────────────────────────────────────
bool getInputWithTimeout(int& value, int seconds, const string& levelText) {

#ifdef _WIN32
    // ── Enable ANSI escape codes on Windows 10+ terminal ─────────────────
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    atomic<bool> done(false); // shared flag to stop the countdown thread
    string input;             // stores what the user is typing
    time_t start = time(NULL);// record start time for countdown

    // ── Countdown thread : updates the header every second ───────────────
    thread timerThread([&]() {
        int last = -1; // track last displayed second to avoid redrawing
        while (!done) {
            int rem = seconds - (int)(time(NULL) - start);
            if (rem < 0) rem = 0;

            if (rem != last) { // only redraw when second changes
                last = rem;
                cout << "\033[s"   // save current cursor position
                    << "\033[4;1H"// jump to header row 4
                    << "\033[2K"  // clear entire header line
                    << GREEN << "\t\t\t\t " << levelText
                    << " ( YOU HAVE ONLY " << RED << rem
                    << GREEN << " SECONDS PER QUESTION )"
                    << "\033[u"   // restore cursor back to input position
                    << flush;
            }
            Sleep(100); // check every 100ms
        }
    });

    // ── Input loop : reads keypresses one by one ──────────────────────────
    while (true) {
        // check if time has run out
        int remaining = seconds - (int)(time(NULL) - start);
        if (remaining <= 0) {
            done = true;
            timerThread.join(); // wait for countdown thread to finish
            return false;       // signal: time's up
        }

        if (_kbhit()) {  // check if a key was pressed
            char ch = _getch();

            if (ch == '\r') {               // Enter key pressed
                if (!input.empty()) {       // only accept if something was typed
                    done = true;
                    timerThread.join();
                    cout << endl;
                    try { value = stoi(input); return true; } // valid number
                    catch (...) { return false; }             // not a number
                }
                // empty Enter → ignore, keep waiting

            } else if (ch == '\b' && !input.empty()) { // Backspace
                cout << "\b \b" << flush; // erase last character on screen
                input.pop_back();         // remove last character from string

            } else if (isdigit(ch) || (ch == '-' && input.empty())) { // digit or minus sign
                cout << ch << flush; // show character on screen
                input += ch;         // add to input string
            }
        }
        Sleep(50); // small delay to avoid busy-waiting
    }

#else
    // ── Switch terminal to raw mode (Mac/Linux) ───────────────────────────
    // Raw mode disables line buffering and auto-echo so we can handle
    // each keypress manually (just like _kbhit/_getch on Windows)
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt); // save original terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // disable line buffer & auto-echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // apply new settings

    atomic<bool> done(false); // shared flag to stop the countdown thread
    string input;             // stores what the user is typing
    time_t start = time(NULL);// record start time for countdown

    // ── Countdown thread : updates the header every second ───────────────
    thread timerThread([&]() {
        int last = -1; // track last displayed second to avoid redrawing
        while (!done) {
            int rem = seconds - (int)(time(NULL) - start);
            if (rem < 0) rem = 0;

            if (rem != last) { // only redraw when second changes
                last = rem;
                cout << "\033[s"   // save current cursor position
                    << "\033[4;1H"// jump to header row 4
                    << "\033[2K"  // clear entire header line
                    << GREEN << "\t\t\t\t " << levelText
                    << " ( YOU HAVE ONLY " << RED << rem
                    << GREEN << " SECONDS PER QUESTION )"
                    << "\033[u"   // restore cursor back to input position
                    << flush;
            }
            usleep(100000); // check every 100ms (microseconds)
        }
    });

    // ── Input loop : reads keypresses one by one ──────────────────────────
    while (true) {
        // check if time has run out
        int remaining = seconds - (int)(time(NULL) - start);
        if (remaining <= 0) {
            done = true;
            timerThread.join();              // wait for countdown thread to finish
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore terminal settings
            return false;                    // signal: time's up
        }

        // use select() to wait for input with a 100ms timeout
        fd_set fds;
        struct timeval tv = {0, 100000};
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        if (select(1, &fds, NULL, NULL, &tv) > 0) { // input is available
            char ch = getchar();

            if (ch == '\n') {               // Enter key pressed
                if (!input.empty()) {       // only accept if something was typed
                    done = true;
                    timerThread.join();
                    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore terminal
                    cout << endl;
                    try { value = stoi(input); return true; } // valid number
                    catch (...) { return false; }             // not a number
                }
                // empty Enter → ignore, keep waiting

            } else if (ch == 127 && !input.empty()) { // Backspace (127 on Mac)
                cout << "\b \b" << flush; // erase last character on screen
                input.pop_back();         // remove last character from string

            } else if (isdigit(ch) || (ch == '-' && input.empty())) { // digit or minus sign
                cout << ch << flush; // show character on screen
                input += ch;         // add to input string
            }
        }
    }
#endif
}