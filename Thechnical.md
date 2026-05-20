# 🔧 Technical Documentation — Math Quiz Game

---

## Table of Contents

1. [Architecture Overview](#1-architecture-overview)
2. [File Breakdown](#2-file-breakdown)
3. [Threading Model](#3-threading-model)
4. [Terminal I/O System](#4-terminal-io-system)
5. [Input-with-Timeout — Deep Dive](#5-input-with-timeout--deep-dive)
6. [Background Clock — Deep Dive](#6-background-clock--deep-dive)
7. [Game Flow & Control Structure](#7-game-flow--control-structure)
8. [ANSI Escape Code Reference](#8-ansi-escape-code-reference)
9. [Platform Abstraction Layer](#9-platform-abstraction-layer)
10. [Known Limitations & Future Improvements](#10-known-limitations--future-improvements)

---

## 1. Architecture Overview

```
┌──────────────────────────────────────────────────────┐
│                       main.cpp                       │
│                                                      │
│   main()                                             │
│     └─ startClock()          (background thread)     │
│     └─ Adding()                                      │
│           └─ getInputWithTimeout()  (timer thread)   │
│                 └─ Subtracting()                     │
│                       └─ multiply()                  │
│                             └─ division()            │
│     └─ stopClock()                                   │
└──────────────────┬───────────────────────────────────┘
                   │ includes
┌──────────────────▼───────────────────────────────────┐
│                      utils.h / utils.cpp             │
│                                                      │
│  Terminal layer:  clearScreen, gotoxy, Pause,        │
│                   mySleep                            │
│  Clock subsystem: startClock, stopClock, drawClock   │
│  Input subsystem: getInputWithTimeout                │
└──────────────────────────────────────────────────────┘
```

The program runs on **three threads at most simultaneously**:

| Thread | Role |
|:--------:|:------:|
| Main thread | Game logic, rendering questions, reading scores |
| Clock thread | Draws `HH:MM:SS` in the header every second |
| Timer thread | Counts down per-question time in the level header |

---

## 2. File Breakdown

### `utils.h`

- Platform `#include` guards: pulls in `<windows.h>` + `<conio.h>` on Windows; `<termios.h>` + `<sys/select.h>` + `<unistd.h>` on POSIX.
- ANSI color macros (`RED`, `GREEN`, `ORANGE`, `CYAN`, `WHITE`, `DEFAULT`, `GRAY`).
- Public API declarations: `clearScreen`, `Pause`, `mySleep`, `gotoxy`, `getInputWithTimeout`, `startClock`, `stopClock`.

### `utils.cpp`

Implements the full platform abstraction and concurrency subsystems. See sections 3–6 for details.

### `main.cpp`

Contains:
- `flushCin()` — discards the trailing `\n` left by `cin >>` so subsequent `getline`-based input works correctly.
- Four level functions: `Adding`, `Subtracting`, `multiply`, `division`.
- `main()` — seeds RNG, starts the clock, greets the player, fires the countdown, then chains into `Adding()`.

---

## 3. Threading Model

### Clock Thread (long-lived)

```
startClock()  ── creates ──►  g_clockThread
                              loops every 1 s
                              calls drawClock()
stopClock()   ── sets ──►  g_running = false
              ── joins ──►  g_clockThread
```

- Protected by `std::atomic<bool> g_running`.
- I/O protected by `std::mutex g_ioMutex` (guards `cout` inside `drawClock`).

### Timer Thread (per-question, short-lived)

```
getInputWithTimeout()  ── creates ──►  timerThread
                                       loops every 100 ms
                                       redraws countdown header
                       ── sets ──►  done = true  (on answer or timeout)
                       ── joins ──►  timerThread
```

- Protected by `std::atomic<bool> done`.
- Lives only for the duration of one question.

### Potential Race Condition (known)

Both `g_clockThread` and `timerThread` write to `stdout`. They use ANSI save/restore cursor sequences (`\033[s` / `\033[u`) to try to minimize visual tearing, but no single global mutex protects all `cout` calls across both threads. In practice this is visually acceptable for a game, but see section 10 for improvement notes.

---

## 4. Terminal I/O System

### `clearScreen()`
- Windows: calls `system("cls")`.
- POSIX: emits `\033[2J\033[H` (erase display + move cursor to origin).

### `gotoxy(int x, int y)`
- Windows: uses `SetConsoleCursorPosition` with a `COORD` struct.
- POSIX: emits `\033[{y+1};{x+1}H`.  
  Note: ANSI rows/cols are 1-based, hence the `+1` offsets.

### `Pause()`
- Windows: `system("pause")`.
- POSIX: `cin.get()` (waits for a single character).

### `mySleep(int ms)`
- Windows: `Sleep(ms)`.
- POSIX: `usleep(ms * 1000)`.

---

## 5. Input-with-Timeout — Deep Dive

**Signature:**
```cpp
bool getInputWithTimeout(int& value, int seconds, const string& levelText);
```

**Returns:** `true` if the user typed a valid integer and pressed Enter before the deadline; `false` on timeout or parse failure.

### Windows Path

1. Enables ANSI VTP on the console handle so ANSI codes work in Windows Terminal.
2. Spawns `timerThread` — polls `time(NULL)` every 100 ms and updates the header.
3. Main loop polls `_kbhit()` every 50 ms.
   - `\r` (Enter): if `input` is non-empty → parse with `stoi`, return `true`.
   - `\b` (Backspace): erase last character from `input` and from screen.
   - Digit / `-` (only at start): echo to screen and append to `input`.
4. On timeout: set `done = true`, join timer thread, return `false`.

### POSIX Path

1. Saves terminal attributes via `tcgetattr`.
2. Sets raw mode: `ICANON` (line buffering) and `ECHO` disabled via `tcsetattr`.
3. Spawns `timerThread` — same logic as Windows, but uses `usleep(100000)`.
4. Main loop uses `select()` with a 100 ms timeout on `STDIN_FILENO`.
   - `\n` (Enter): same as Windows `\r` path.
   - `127` (DEL/Backspace on macOS/Linux): erase logic.
   - Digit / `-`: echo and append.
5. On timeout or successful read: restores terminal attributes via `tcsetattr(…, &oldt)`.

### Why Raw Mode?

Standard terminal line buffering means the program only receives input after the user presses Enter. By switching to raw mode (`~ICANON`), the program receives each keypress immediately — enabling the live character-by-character display and the ability to interrupt mid-input when time runs out.

---

## 6. Background Clock — Deep Dive

```cpp
static void drawClock() {
    // Gets current system time → formats as HH:MM:SS
    // Locks g_ioMutex, then:
    cout << "\0337"         // ANSI: save cursor position
         << "\033[3;80H"    // Move to row 3, col 80
         << "\033[2K"       // Clear to end of line
         << "Current time : HH:MM:SS"
         << "\0338"         // ANSI: restore cursor position
         << flush;
}
```

### Scroll Region Trick

`startClock()` sets the terminal scroll region to rows 2–bottom via `\033[2;r`. This means normal scrolling output from the game never overwrites row 1/3 where the clock lives. The region is released by `stopClock()` with `\033[r` (reset to full screen).

---

## 7. Game Flow & Control Structure

```
main()
 ├─ srand(time(NULL))         seed RNG
 ├─ startClock()              start clock thread
 ├─ get player name
 ├─ 5-second countdown loop   mySleep(800ms) per tick
 └─ Adding()
       ├─ 5 questions loop
       │     ├─ generate rand numbers
       │     └─ getInputWithTimeout(15s)
       ├─ score == 5  ──yes──► Subtracting()
       └─ score < 5   ──yes──► goto Adding (retry)
             └─ Subtracting()
                   ├─ 5 questions loop (20s each)
                   ├─ score == 5  ──► multiply()
                   └─ score < 5  ──► goto Subtracting
                         └─ multiply()
                               ├─ 5 questions loop (25s each)
                               ├─ score == 5  ──► division()
                               └─ score < 5  ──► goto multiply
                                     └─ division()
                                           ├─ 5 questions loop (30s each)
                                           ├─ score == 5  ──► 🎉 congratulations
                                           └─ score < 5  ──► goto division
 └─ stopClock()
```

### `goto` Usage

Each level function uses a local `goto` label (e.g., `Adding:`) to re-run the level on failure. This avoids re-entering the function recursively and keeps the call stack flat. It is a deliberate stylistic choice in the original code; a loop or recursive call would be equivalent.

### Random Number Generation

```cpp
srand((unsigned int)time(NULL));   // seeded once in main()

// Addition / Subtraction / Division:
int number1 = rand() % 50 + 1;    // [1, 50]
int number2 = rand() % 50 + 1;    // [1, 50]

// Multiplication:
int number1 = rand() % 40 + 1;    // [1, 40]  (smaller to keep products reasonable)
int number2 = rand() % 40 + 1;

// Division (guaranteed integer result):
int number2 = rand() % 15 + 1;          // divisor [1, 15]
int number1 = number2 * (rand() % 15 + 1);  // dividend = divisor × [1,15]
```

---

## 8. ANSI Escape Code Reference

| Code | Meaning | Used in |
|:------:|:---------:|:---------:|
| `\033[2J` | Erase entire screen | `clearScreen`, `startClock` |
| `\033[H` | Move cursor to origin (1,1) | `clearScreen` |
| `\033[y;xH` | Move cursor to row y, col x | `gotoxy` (POSIX) |
| `\033[2K` | Erase entire current line | `drawClock`, `timerThread` |
| `\033[s` / `\033[u` | Save / restore cursor position | `drawClock`, `timerThread` |
| `\0337` / `\0338` | Save / restore cursor (DEC variant) | `drawClock` |
| `\033[2;r` | Set scroll region from row 2 to bottom | `startClock` |
| `\033[r` | Reset scroll region to full screen | `stopClock` |
| `\033[1;3Xm` | Bold + color (31=red, 32=green, etc.) | color macros |
| `\033[0m` | Reset all attributes | `DEFAULT` macro |

---

## 9. Platform Abstraction Layer

Every platform-specific call is isolated behind `#ifdef _WIN32` guards. This table summarises the mapping:

| Functionality | Windows | POSIX (Linux / macOS) |
|:---------------:|:---------:|:----------------------:|
| Screen clear | `system("cls")` | `\033[2J\033[H` |
| Cursor move | `SetConsoleCursorPosition` | `\033[y;xH` |
| Sleep (ms) | `Sleep(ms)` | `usleep(ms*1000)` |
| Pause | `system("pause")` | `cin.get()` |
| Raw keyboard input | `_kbhit()` / `_getch()` | `tcsetattr` + `select()` + `getchar()` |
| ANSI colors | Enabled via `SetConsoleMode` + VTP flag | Native |
| Thread sleep | `Sleep(100)` | `usleep(100000)` |

---

## 10. Known Limitations & Future Improvements

### Limitations

| # | Issue | Detail |
|:---:|:-------:|:--------:|
| 1 | `goto` usage | Local `goto` labels work but reduce readability; a `do { } while(retry)` loop is more idiomatic C++. |
| 2 | Global `user_answer` | Declared as a global `int` in `main.cpp`; passing it as a parameter would be cleaner. |
| 3 | `system("cls")` / `system("pause")` | These spawn a child process — slower and potentially unsafe; prefer ANSI/POSIX alternatives. |
| 4 | Unsynchronised `cout` | `g_clockThread` and `timerThread` both write to `stdout` without a shared lock, which can cause occasional visual glitches. |
| 5 | Score threshold | Progression requires a perfect 5/5. A configurable pass mark (e.g., 4/5) would be more forgiving. |
| 6 | No high-score persistence | Scores are lost when the program exits. |
| 7 | `rand()` quality | `rand()` has poor statistical properties. `std::mt19937` (from `<random>`) is the modern C++ alternative. |

### Suggested Improvements

```cpp
// 1. Replace goto with a loop
void Adding() {
    bool retry = true;
    while (retry) {
        // ... quiz logic ...
        retry = (score < 5 && userWantsRetry());
    }
    if (score == 5) Subtracting();
}

// 2. Better RNG
#include <random>
mt19937 rng(random_device{}());
uniform_int_distribution<int> dist(1, 50);
int number1 = dist(rng);

// 3. Global I/O mutex shared by clock and timer threads
extern mutex g_ioMutex; // in utils.h
lock_guard<mutex> lk(g_ioMutex);
cout << ... << flush;
```