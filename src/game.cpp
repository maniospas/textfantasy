#include "utils.h"
#include "terrain.h"
#include "agent.h"
#include "dsl.h"
#include <thread>
#include <chrono>
#include <iostream>

#if defined(_WIN32)
    #include <windows.h>
#endif


void redraw(const Terrain& terrain, Unit* touch, const Unit& player) {
    std::cout << ANSI_CLEAR_AND_HOME;
    terrain.draw(terrain.player_x, terrain.player_y, 8);
    //std::cout << "\n\n";

    if (touch && touch->symbol != unit::GRASS.symbol) {
        if(touch->ai_target!=&player) {
            std::cout << "\033[1;34m";   // bright red for enemy
            std::cout << "  ┌─TARGET───────────────────────────────────────────────────────\n";
            std::cout << "  │ \033[0m  ";
            touch->base_info();
            //std::cout << "\033[1;34m  ├─INFO─────────────────────────────────────────────────────────\n";
            //std::cout << "  \033[1;34m│ \033[0mQ question \033[1;34m│ \033[0m\n";
            //std::cout << "  \033[1;34m│ \033[0mI info     \033[1;34m│ \033[0m\n";  
            //std::cout << "  \033[1;34m│ \033[0m  ";
            // for (auto& item : touch->items) {
            //     std::cout << item.symbol << " " << item.name << " ";
            // }  
            std::cout << "  \033[1;34m│ \033[0m  \n";
            std::cout << "  \033[1;34m│ \033[0m ";
            //std::cout << request_engine(*touch);
            print_status(*touch);
            std::cout << "\n\033[1;34m  └──────────────────────────────────────────────────────────────";
            std::cout << "\033[0m\n";
        }
        else {
            std::cout << "\033[1;31m";   // bright red for enemy
            std::cout << "  ┌─FIGHTING─────────────────────────────────────────────────────\n";
            std::cout << "  │ \033[0m  ";
            touch->base_info();      
            std::cout << "  \033[1;31m│ \033[0m  \n";
            std::cout << "  \033[1;31m│ \033[0m "; 
            //std::cout << request_engine(*touch);    
            print_status(*touch);  
            std::cout << "\n\033[1;31m  └──────────────────────────────────────────────────────────────";
            std::cout << "\033[0m\n";
        }
    } else std::cout << "\n\n\n\n\n";
    std::cout << "\n";
    
    std::cout << "\033[1;32m";   // bright red for enemy
    std::cout << "  ┌─PLAYER───────────────────────────────────────────────────────\n";
    std::cout << "  │ \033[0m  ";
    player.info();     
    std::cout << "  │ \033[0m  ";          
    player.info_secondary();
    std::cout << "\033[1;32m  ├─INVENTORY────────────────────────────────────────────────────";
    std::cout << "\033[0m\n";

    long pos = 1;
    if(player.mood>0) 
        std::cout << "  \033[1;32m│ \033[0m 💖  charmed \033[1;32m│\033[0m Cannot act for a bit\n";
    if(player.frozen) 
        std::cout << "  \033[1;32m│ \033[0m ⏳  immobile \033[1;32m│\033[0m Cannot act or move for a bit\n";
    if(player.mood<=0 && !player.frozen)
        for (auto& item : player.items) {
            if (pos >= 10) break;
            std::cout << "  \033[1;32m│ \033[0m" << pos << " "
                    << item.symbol << " " << item.name
                    << " \033[1;32m│\033[0m " << item.description << "\n";
            pos++;
        }
    std::cout << "\033[1;32m  └──────────────────────────────────────────────────────────────";

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
    auto& player = terrain.cell(terrain.player_x, terrain.player_y).unit();
    player.max_health = 10;
    player.health = 10;

    while (is_living) {
        player = terrain.cell(terrain.player_x, terrain.player_y).unit();
        redraw(terrain, touch, player);

        char ch = getch_nowait();
        while (!(ch = getch_nowait())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            redraw(terrain, touch, player);
        }

        for (size_t i = 0; i < 9; ++i) 
            if (ch == (char)('1' + i) && touch && touch->symbol != unit::GRASS.symbol && player.items.size() > i)
                player.fight(touch, &player.items[i]);

        // if (ch == 'q' || ch == 'Q')
        //     break;

        if (ch == 's' || ch == 'S') {
            if (terrain.move(terrain.player_x, terrain.player_y, terrain.player_x, terrain.player_y + 1))
                terrain.player_y++;
            if(terrain.in_bounds(terrain.player_x, terrain.player_y + 1))
                touch = &terrain.cell(terrain.player_x, terrain.player_y + 1).unit();
        }
        if (ch == 'w' || ch == 'W') {
            if (terrain.move(terrain.player_x, terrain.player_y, terrain.player_x, terrain.player_y - 1))
                terrain.player_y--;
            if(terrain.in_bounds(terrain.player_x, terrain.player_y-1))
                touch = &terrain.cell(terrain.player_x, terrain.player_y - 1).unit();
        }
        if (ch == 'd' || ch == 'D') {
            if (terrain.move(terrain.player_x, terrain.player_y, terrain.player_x + 1, terrain.player_y))
                terrain.player_x++;
            if(terrain.in_bounds(terrain.player_x + 1, terrain.player_y))
                touch = &terrain.cell(terrain.player_x + 1, terrain.player_y).unit();
        }
        if (ch == 'a' || ch == 'A') {
            if (terrain.move(terrain.player_x, terrain.player_y, terrain.player_x - 1, terrain.player_y))
                terrain.player_x--;
            if(terrain.in_bounds(terrain.player_x - 1, terrain.player_y))
                touch = &terrain.cell(terrain.player_x - 1, terrain.player_y).unit();
        }
        //if (!touch || touch->symbol == NO_SYMBOL) 
        //    touch = player.ai_target;
        if (!touch) {
            static const int dx[4] = {1, -1, 0, 0};
            static const int dy[4] = {0, 0, 1, -1};
            for (int i = 0; i < 4; ++i) {
                int nx = terrain.player_x + dx[i];
                int ny = terrain.player_y + dy[i];
                // Make sure the coordinates are inside the map
                if (terrain.in_bounds(nx, ny)) {
                    auto &unit = terrain.cell(nx, ny).unit();
                    if (unit.symbol != NO_SYMBOL) {
                        touch = &unit;
                        break;
                    }
                }
            }
        }

        // draw again
        redraw(terrain, touch, player);
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        terrain.step(0);

        // draw again
        redraw(terrain, touch, player);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        terrain.step(1);

        if (terrain.cell(terrain.player_x, terrain.player_y).unit().symbol == NO_SYMBOL)
            is_living = false;
    }

    if (!is_living) {
        std::cout << ANSI_CLEAR_AND_HOME;
        //terrain.draw(terrain.player_x, terrain.player_y, 8);
        std::cout << "\033[H\n";
        std::cout << "\033[1;31m"; 
        std::cout << "  ┌───────────┐\n";
        std::cout << "  │ GAME OVER │\n";
        std::cout << "  └───────────┘\n";
        std::cout << "\033[0m";
    }
    else {
        std::cout << ANSI_CLEAR_AND_HOME;
        //terrain.draw(terrain.player_x, terrain.player_y, 8);
        std::cout << "\n\n";
        std::cout << "\033[1;33m";
        std::cout << "  ┌─────────────┐\n";
        std::cout << "  │  QUIT GAME  │\n";
        std::cout << "  └─────────────┘\n";
        std::cout << "\033[0m";
    }

}
