#include "unit.h"
#include "terrain.h"

void Unit::ai_step(Terrain& terrain, long my_x, long my_y, long interpolation_step) {
    if (speed < 5 && !interpolation_step && dist(rng) < static_cast<double>(speed) / 5.0)
        return;
    if(speed > 5 && interpolation_step && dist(rng) < static_cast<double>(speed) / 5.0-1.0)
        return;

    if (ai_target && (ai_target->health == 0 || ai_target->symbol == NO_SYMBOL)) 
        ai_target = nullptr;
    if (!ai_target) {
        std::uniform_int_distribution<long> dist(-8,8);
        for (int attempt = 0; attempt < 20 && !ai_target; ++attempt) {
            long nx = my_x + dist(rng);
            long ny = my_y + dist(rng);
            if (nx < 0 || nx >= terrain.WIDTH || ny < 0 || ny >= terrain.HEIGHT)
                continue;
            Unit &cand = terrain.cell(nx, ny).unit();
            if (cand.symbol != NO_SYMBOL && cand.health > 0) 
                ai_target = &cand;
        }
    }
    if (ai_target) {
        long tx = -1, ty = -1;
        for (long y = 0; y < terrain.HEIGHT && ty == -1; ++y)
            for (long x = 0; x < terrain.WIDTH && tx == -1; ++x)
                if (&terrain.cell(x,y).unit() == ai_target) {
                    tx = x; 
                    ty = y;
                }

        if (tx != -1) {
            long dxMove = (tx == my_x ? 0 : (tx > my_x ? 1 : -1));
            long dyMove = (ty == my_y ? 0 : (ty > my_y ? 1 : -1));
            if (std::abs(tx - my_x) <= 1 && std::abs(ty - my_y) <= 1) {
                if (!items.empty()) {
                    if(!interpolation_step)
                        fight(ai_target, &items[0]);
                }
                else
                    terrain.move(my_x, my_y, my_x - dxMove, my_y - dyMove);
            }
            else terrain.move(my_x, my_y, my_x + dxMove, my_y + dyMove);
        }
    }
    else {
        static const int dx[4] = { 0, 0, -1, 1 };
        static const int dy[4] = { -1, 1, 0, 0 };
        auto s = speed;
        if (s < 5) {
            if (prob(rng) < s/5.0) {
                int dir = dirDist(rng);
                terrain.move(my_x, my_y, my_x + dx[dir], my_y + dy[dir]);
            }
        } else {
            int dir = dirDist(rng);
            terrain.move(my_x, my_y, my_x + dx[dir], my_y + dy[dir]);
        }
    }
}