# 🧮 Math Quiz Game

A terminal-based math quiz game written in C++ that challenges players across four progressive difficulty levels — Addition, Subtraction, Multiplication, and Division — each with a live countdown timer per question.

---

## 📋 Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Building & Running](#building--running)
- [Gameplay](#gameplay)
- [Project Structure](#project-structure)
- [Platform Support](#platform-support)

---

## ✨ Features

- Four progressive quiz levels (Addition → Subtraction → Multiplication → Division)
- Per-question countdown timer (15 / 20 / 25 / 30 seconds per level)
- Live clock display in the terminal header (HH:MM:SS)
- Raw-mode keyboard input — no need to press Enter to start
- Color-coded feedback (green = correct, red = wrong/timeout)
- Score and percentage displayed at the end of each level
- Retry option on failure; advance option on perfect score (5/5)
- Cross-platform: Windows and Linux/macOS

---

## 🛠 Requirements

| Tool | Version |
|------|---------|
| C++ compiler | C++14 or later (g++, clang++, MSVC) |
| Standard library | `<thread>`, `<atomic>`, `<chrono>`, `<mutex>` |
| POSIX (Linux/macOS) | `termios.h`, `sys/select.h`, `unistd.h` |
| Windows | `windows.h`, `conio.h` |

No third-party dependencies are required.

---

## 🚀 Building & Running

### Linux / macOS

```bash
g++ -std=c++14 -pthread -o math_game main.cpp utils.cpp
./math_game
```

### Windows (MinGW / MSYS2)

```bash
g++ -std=c++14 -o math_game.exe main.cpp utils.cpp
math_game.exe
```

### Windows (MSVC)

```bash
cl /EHsc /std:c++14 main.cpp utils.cpp /Fe:math_game.exe
math_game.exe
```

> **Note:** The `-pthread` flag is required on Linux/macOS because the background clock and countdown timer use `std::thread`.

---

## 🎮 Gameplay

1. Launch the program and enter your name.
2. A 5-second countdown starts before Level 1 begins.
3. Each level presents 5 random math questions. You must answer before the timer runs out.
4. Score **5/5** to unlock the next level. Otherwise you can retry the same level.
5. Complete all 4 levels to win! 🎉

### Levels & Time Limits

| Level | Operation      | Time per Question |
|:-------:|:----------------:|:-------------------:|
| 1     | Addition       | 15 seconds        |
| 2     | Subtraction    | 20 seconds        |
| 3     | Multiplication | 25 seconds        |
| 4     | Division       | 30 seconds        |

> Division questions are guaranteed to have integer answers (no remainders).

---

## 📁 Project Structure

```
math_game/
├── main.cpp      # Entry point and all four quiz level functions
├── utils.cpp     # Terminal utilities, clock thread, input-with-timeout logic
├── utils.h       # Declarations, platform guards, ANSI color macros
└── README.md     # This file
```

---

## 🖥 Platform Support

| Feature | Windows | Linux | macOS |
|:---------:|:---------:|:-------:|:-------:|
| Color output | ✅ (ANSI via VTP) | ✅ | ✅ |
| Raw keyboard input | ✅ (`_kbhit`/`_getch`) | ✅ (`termios` + `select`) | ✅ (`termios` + `select`) |
| Background clock | ✅ | ✅ | ✅ |
| Screen clear | ✅ (`cls`) | ✅ (ANSI) | ✅ (ANSI) |
| Cursor positioning | ✅ (`SetConsoleCursorPosition`) | ✅ (ANSI `\033[y;xH`) | ✅ (ANSI `\033[y;xH`) |