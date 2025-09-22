#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <random>

inline constexpr const char* ANSI_CLEAR_AND_HOME = "\033[3J\033[2J\033[H";
extern std::mt19937 rng;
extern std::uniform_real_distribution<> prob;
extern std::uniform_real_distribution<> dist;
extern std::uniform_int_distribution<int> dirDist;

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/select.h>
#endif

inline char getch_nowait()
{
    char c = '\0';

#ifdef _WIN32
    // Windows: drain all available characters
    while (_kbhit()) {
        c = _getch();
    }
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    fd_set set;
    struct timeval tv {0, 0}; // no wait
    while (true) {
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        if (select(STDIN_FILENO + 1, &set, nullptr, nullptr, &tv) <= 0)
            break;
        char tmp;
        if (read(STDIN_FILENO, &tmp, 1) <= 0)
            break;
        c = tmp;   // keep overwriting to keep only the newest char
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); 
#endif
    return c;
}

inline void flush_input()
{
#ifdef _WIN32
    while (_kbhit()) {
        _getch();
    }
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    fd_set set;
    struct timeval tv {0, 0};
    while (true) {
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        if (select(STDIN_FILENO + 1, &set, nullptr, nullptr, &tv) <= 0)
            break;
        char tmp;
        if (read(STDIN_FILENO, &tmp, 1) <= 0)
            break;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}


#endif // UTILS_H
