#include "utils.h"
#include "terrain.h"

#if defined(_WIN32)
    #include <windows.h>
#endif
#include <iostream>

int main() {
#if defined(_WIN32)
    // Enable ANSI escape sequences in Windows 10+ console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    SetConsoleOutputCP(CP_UTF8);
#endif

    Terrain terrain;
    Unit* touch = nullptr;
    bool is_living = true;
    while (is_living) {
        std::cout << ANSI_CLEAR_AND_HOME;
        std::cout << "SPACE: interact   WASD: move   Q: quit\n";

        terrain.draw(terrain.player_x, terrain.player_y, 8);
        std::cout << "\n\n";
        std::cout << "  ME ";
        terrain.cell(terrain.player_x, terrain.player_y).unit().info();
        if(touch && touch->symbol!=unit::GRASS.symbol) {
            std::cout << "  VS ";
            touch->info();
        }
        else 
            std::cout << "\n";

        char ch = getch_nowait();   // <— no need to hit Enter
        if(ch == ' ' && touch && touch->symbol!=unit::GRASS.symbol) {
            terrain.cell(terrain.player_x, terrain.player_y).unit().fight(touch);
        }
        if(ch == 'q' || ch == 'Q')
            break;
        if(ch == 's' || ch == 'S') {
            if(terrain.move(terrain.player_x, terrain.player_y, terrain.player_x, terrain.player_y + 1))
                terrain.player_y++;
            touch = &terrain.cell(terrain.player_x, terrain.player_y + 1).unit();
        }
        if(ch == 'w' || ch == 'W') {
            if(terrain.move(terrain.player_x, terrain.player_y, terrain.player_x, terrain.player_y - 1))
                terrain.player_y--;
            touch = &terrain.cell(terrain.player_x, terrain.player_y - 1).unit();
        }
        if(ch == 'd' || ch == 'D') {
            if(terrain.move(terrain.player_x, terrain.player_y, terrain.player_x + 1, terrain.player_y))
                terrain.player_x++;
            touch = &terrain.cell(terrain.player_x + 1, terrain.player_y).unit();
        }
        if(ch == 'a' || ch == 'A') {
            if(terrain.move(terrain.player_x, terrain.player_y, terrain.player_x - 1, terrain.player_y))
                terrain.player_x--;
            touch = &terrain.cell(terrain.player_x - 1, terrain.player_y).unit();
        }
        terrain.step();
        if(!terrain.cell(terrain.player_x, terrain.player_y).unit().symbol[0])
            is_living = false;
    }
    if(!is_living) {
        std::cout << ANSI_CLEAR_AND_HOME;
        std::cout << "GAME OVER\n";
        terrain.draw(terrain.player_x, terrain.player_y, 8);
    }
}
