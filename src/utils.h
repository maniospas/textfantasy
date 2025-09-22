#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <random>

inline constexpr const char* ANSI_CLEAR_AND_HOME = "\033[2J\033[H";

#ifdef _WIN32
    #include <conio.h>   // _kbhit, _getch
#else
    #include <termios.h>
    #include <unistd.h>
#endif
char getch_nowait() {
#ifdef _WIN32
    // Blocks until a key is pressed, does not echo
    return _getch();
#else
    // --- POSIX version: set terminal to raw mode ---
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char c = 0;
    read(STDIN_FILENO, &c, 1);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore settings
    return c;
#endif
}



#endif // UTILS_H