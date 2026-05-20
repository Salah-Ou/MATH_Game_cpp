#ifndef UTILS_H
#define UTILS_H
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
    #include <sys/select.h>
    #include <termios.h>
#endif

#define RED   "\033[1;31m"
#define GREEN "\033[1;32m"
#define ORANGE "\033[1;33m"
#define CYAN  "\033[1;36m"
#define WHITE "\033[1;37m"
#define DEFAULT "\033[0m"
#define GRAY "\033[1;30m"

void clearScreen();
void Pause();
void mySleep(int milliseconds);
void gotoxy(int x, int y);
bool getInputWithTimeout(int& value, int seconds, const std::string& levelText) ;

// Starts a background thread that keeps a live HH MM SS clock
// in the top-right corner of the terminal. Safe to call once.
void startClock();
void stopClock();

#endif
