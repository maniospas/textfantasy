#ifndef TERRAIN_H
#define TERRAIN_H

#include "utils.h"
#include "unit.h"
#include "tile.h"
#include <vector>
#include <list>
#include <random>
#include <iostream>
#include <algorithm>

class Cell {
public:
    Cell() : tile_(nullptr), unit_(nullptr) {}
    void set(Tile* t, Unit* u) { tile_ = t; unit_ = u; }
    Tile& tile()  { return *tile_; }
    Unit& unit() { return *unit_; }
    const Tile& tile() const { return *tile_; }
    const Unit& unit() const { return *unit_; }
    void draw() const {
        if(unit_->effect) {
            std::cout << tile_->bgColor << unit_->effect << bg::reset;
            unit_->effect = nullptr;
        }
        else
            std::cout << tile_->bgColor << unit_->symbol << bg::reset;
    }
private:
    Tile* tile_;
    Unit* unit_;
    friend class Terrain;
};

class Terrain {
public:
    static constexpr long WIDTH  = 128;
    static constexpr long HEIGHT = 128;

    Terrain() : cells(WIDTH * HEIGHT), player_x(32), player_y(32) { randomize(); }

    void randomize() {
        tiles.clear();
        units.clear();

        // shared default grass tile & unit
        tiles.emplace_back(bg::green);
        units.emplace_back(unit::GRASS);
        defaultTile = &tiles.back();
        defaultUnit = &units.back();

        std::mt19937 rng{std::random_device{}()};
        std::uniform_real_distribution<> dist(0.0, 1.0);

        for (long y = 0; y < HEIGHT; ++y) {
            for (long x = 0; x < WIDTH; ++x) {
                auto r = dist(rng);
                Tile* t = defaultTile;
                Unit* u = defaultUnit;
                if(x==player_x && y==player_y) {
                    units.emplace_back(unit::HUMAN);
                    u = &units.back(); 
                }
                else if (r < 0.001) { units.emplace_back(unit::HUMAN); u = &units.back(); }
                else if (r < 0.002) { units.emplace_back(unit::HORSE); u = &units.back(); }
                else if (r < 0.003) { units.emplace_back(unit::GOBLIN); u = &units.back(); }
                else if (r < 0.004)  { units.emplace_back(unit::WOLF); u = &units.back(); }
                else if (r < 0.005) { units.emplace_back(unit::RABBIT); u = &units.back(); }
                else if (r < 0.006)  { units.emplace_back(unit::SLIME); u = &units.back(); }
                else if (r < 0.007)  { units.emplace_back(unit::BANSHEE); u = &units.back(); }
                else if (r < 0.008) { units.emplace_back(unit::DRAGON); u = &units.back(); }
                else if (r < 0.009) { units.emplace_back(unit::ELF); u = &units.back(); }
                else if (r < 0.010) { units.emplace_back(unit::ORC); u = &units.back(); }
                //else if (r < 0.15) { units.emplace_back(unit::TREE2);  u = &units.back(); }
                else if (r < 0.4) { units.emplace_back(unit::TREE); u = &units.back(); }

                cell(x, y).set(t, u);
            }
        }
    }

    bool blocksSight(const Unit& u) const {
        return (u.symbol == unit::TREE.symbol) ||
                (u.symbol == unit::ROCK.symbol) ||
                (u.symbol == unit::MOUNTAIN.symbol);
    }

    bool hasLineOfSight(long x0,long y0,long x1,long y1) const {
        long dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        long dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        long err = dx + dy;
        long cx = x0, cy = y0;

        while (true) {
            if (cx == x1 && cy == y1) return true;
            if (!(cx == x0 && cy == y0)) { // skip the starting square
                if (blocksSight(cell(cx, cy).unit()))
                    return false; // something blocks the ray
            }
            long e2 = 2 * err;
            if (e2 >= dy) { err += dy; cx += sx; }
            if (e2 <= dx) { err += dx; cy += sy; }
        }
    }


    void draw(long pos_x, long pos_y, long sight) const {
        long start_y = std::max(0L, pos_y - sight);
        long end_y   = std::min<long>(HEIGHT - 1, pos_y + sight);
        long start_x = std::max(0L, pos_x - sight);
        long end_x   = std::min<long>(WIDTH  - 1, pos_x + sight);

        std::cout << "\n\n";
        for (long y = start_y; y <= end_y; ++y) {
            std::cout << "  ";
            for (long x = start_x; x <= end_x; ++x) {
                if (hasLineOfSight(pos_x, pos_y, x, y)) {
                    cell(x, y).draw();
                } else {
                    // show unexplored / unseen cells as blank grass or dark tile
                    std::cout << "  ";
                }
            }
            std::cout << '\n';
        }
        std::cout << bg::reset << std::endl;
    }


    Cell& cell(long x, long y) { return cells[y * WIDTH + x]; }
    const Cell& cell(long x, long y) const { return cells[y * WIDTH + x]; }

    void step() {
        static std::mt19937 rng{std::random_device{}()};
        std::uniform_real_distribution<> prob(0.0, 1.0);
        std::uniform_int_distribution<int> dirDist(0, 3);
        const int dx[4] = { 0, 0, -1, 1 };
        const int dy[4] = { -1, 1, 0, 0 };
        for (long y = 0; y < HEIGHT; ++y) {
            for (long x = 0; x < WIDTH; ++x) {
                if (player_x == x && player_y == y) continue;

                Unit &u = cell(x, y).unit();
                if (u.symbol==NO_SYMBOL || !u.speed) continue;

                double s = u.speed;  // assumes Unit has a numeric member "speed"
                if (s < 5.0) {
                    if (prob(rng) < s/5.0) {
                        int dir = dirDist(rng);
                        move(x, y, x + dx[dir], y + dy[dir]); // move immediately
                    }
                } else {
                    // speed ≥ 5 : always at least one move
                    int dir = dirDist(rng);
                    move(x, y, x + dx[dir], y + dy[dir]);
                    // second move will be handled by step_again()
                }
            }
        }
    }
    void step_again() {
        static std::mt19937 rng{std::random_device{}()};
        std::uniform_real_distribution<> prob(0.0, 1.0);
        std::uniform_int_distribution<int> dirDist(0, 3);
        static const int dx[4] = { 0, 0, -1, 1 };
        static const int dy[4] = { -1, 1, 0, 0 };

        for (long y = 0; y < HEIGHT; ++y) {
            for (long x = 0; x < WIDTH; ++x) {
                if (player_x == x && player_y == y) 
                    continue;
                auto &u = cell(x, y).unit();
                if (u.symbol==NO_SYMBOL || u.speed<5) 
                    continue;
                double s = u.speed;
                if (s > 5) {
                    double p = s/5.0 - 1.0;
                    if (prob(rng) < p) {
                        int dir = dirDist(rng);
                        move(x, y, x + dx[dir], y + dy[dir]);
                    }
                }
            }
        }
    }


    bool move(long fromX, long fromY, long toX, long toY) {
        if (fromX < 0 || fromX >= WIDTH || fromY < 0 || fromY >= HEIGHT ||
            toX   < 0 || toX   >= WIDTH || toY   < 0 || toY   >= HEIGHT) {
            return false;
        }
        Cell& src = cell(fromX, fromY);
        Cell& dst = cell(toX, toY);
        if (&dst.unit() != defaultUnit && dst.unit().symbol!=NO_SYMBOL) 
            return false;
        Unit* u = &src.unit();
        if (u == defaultUnit) 
            return false;
        dst.set(dst.tile_ ? &dst.tile() : defaultTile, u);
        src.set(src.tile_ ? &src.tile() : defaultTile, defaultUnit);
        return true;
    }

    long player_x;
    long player_y;
private:
    std::vector<Cell> cells;

    // list guarantees stable addresses for elements
    std::list<Tile> tiles;
    std::list<Unit> units;

    Tile* defaultTile = nullptr;
    Unit* defaultUnit = nullptr;
};

#endif // TERRAIN_H
