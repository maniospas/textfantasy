#include "utils.h"
#include "terrain.h"
#include <thread>
#include <chrono>

#if defined(_WIN32)
    #include <windows.h>
#endif
#include <iostream>

void redraw(const Terrain& terrain, Unit* touch, const Unit& player) {
    std::cout << ANSI_CLEAR_AND_HOME;
    terrain.draw(terrain.player_x, terrain.player_y, 8);
    std::cout << "\n\n";

    if (touch && touch->symbol != unit::GRASS.symbol) {
        if(touch->ai_target!=&player) {
            std::cout << "\033[1;34m";   // bright red for enemy
            std::cout << "  ┌─TARGET─────────────────────────────────────────\n";
            std::cout << "  │ \033[0m  ";
            touch->info();
            std::cout << "\033[1;34m  ├─INTERACT───────────────────────────────────────\n";
            std::cout << "  \033[1;34m│ \033[0mQ question \033[1;34m│ \033[0m\n";
            std::cout << "  \033[1;34m│ \033[0mI info     \033[1;34m│ \033[0m\n";        
            std::cout << "\033[1;34m  └────────────────────────────────────────────────";
            std::cout << "\033[0m\n";
        }
        else {
            std::cout << "\n\n\n\033[1;31m";   // bright red for enemy
            std::cout << "  ┌─FIGHTING───────────────────────────────────────\n";
            std::cout << "  │ \033[0m  ";
            touch->info();               
            std::cout << "\033[1;31m  └────────────────────────────────────────────────";
            std::cout << "\033[0m\n";
        }
    } else std::cout << "\n\n\n\n\n\n";

    
    std::cout << "\033[1;32m";   // bright red for enemy
    std::cout << "  ┌─PLAYER─────────────────────────────────────────\n";
    std::cout << "  │ \033[0m  ";
    player.info();               
    std::cout << "\033[1;32m  ├─INVENTORY──────────────────────────────────────";
    std::cout << "\033[0m\n";

    long pos = 1;
    for (auto& item : player.items) {
        if (pos >= 10) break;
        std::cout << "  \033[1;32m│ \033[0m" << pos << " "
                  << item.symbol << " " << item.name
                  << " \033[1;32m│\033[0m " << item.description << "\n";
        pos++;
    }
    std::cout << "\033[1;32m  └────────────────────────────────────────────────";

    std::cout << "\033[0m\n    WASD: move   Number: use item   Ctrl+C: quit\n";
}

int main() {
#if defined(_WIN32)
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
        auto& player = terrain.cell(terrain.player_x, terrain.player_y).unit();
        redraw(terrain, touch, player);

        char ch = getch_nowait();
        while (!(ch = getch_nowait())) 
            std::this_thread::sleep_for(std::chrono::milliseconds(15));

        for (int i = 0; i < 9; ++i) 
            if (ch == '1' + i && touch && touch->symbol != unit::GRASS.symbol && player.items.size() > i)
                player.fight(touch, &player.items[i]);

        // if (ch == 'q' || ch == 'Q')
        //     break;

        if (ch == 's' || ch == 'S') {
            if (terrain.move(terrain.player_x, terrain.player_y, terrain.player_x, terrain.player_y + 1))
                terrain.player_y++;
            touch = &terrain.cell(terrain.player_x, terrain.player_y + 1).unit();
        }
        if (ch == 'w' || ch == 'W') {
            if (terrain.move(terrain.player_x, terrain.player_y, terrain.player_x, terrain.player_y - 1))
                terrain.player_y--;
            touch = &terrain.cell(terrain.player_x, terrain.player_y - 1).unit();
        }
        if (ch == 'd' || ch == 'D') {
            if (terrain.move(terrain.player_x, terrain.player_y, terrain.player_x + 1, terrain.player_y))
                terrain.player_x++;
            touch = &terrain.cell(terrain.player_x + 1, terrain.player_y).unit();
        }
        if (ch == 'a' || ch == 'A') {
            if (terrain.move(terrain.player_x, terrain.player_y, terrain.player_x - 1, terrain.player_y))
                terrain.player_x--;
            touch = &terrain.cell(terrain.player_x - 1, terrain.player_y).unit();
        }

        // draw again
        redraw(terrain, touch, player);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        terrain.step(0);

        // draw again
        redraw(terrain, touch, player);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        terrain.step(1);

        if (terrain.cell(terrain.player_x, terrain.player_y).unit().symbol == NO_SYMBOL)
            is_living = false;
    }

    if (!is_living) {
        std::cout << ANSI_CLEAR_AND_HOME;
        terrain.draw(terrain.player_x, terrain.player_y, 8);
        std::cout << "\n\n";
        std::cout << "\033[1;31m"; 
        std::cout << "  ┌───────────┐\n";
        std::cout << "  │ GAME OVER │\n";
        std::cout << "  └───────────┘\n";
        std::cout << "\033[0m";
    }
    else {
        std::cout << ANSI_CLEAR_AND_HOME;
        terrain.draw(terrain.player_x, terrain.player_y, 8);
        std::cout << "\n\n";
        std::cout << "\033[1;33m";
        std::cout << "  ┌─────────────┐\n";
        std::cout << "  │  QUIT GAME  │\n";
        std::cout << "  └─────────────┘\n";
        std::cout << "\033[0m";
    }

}
