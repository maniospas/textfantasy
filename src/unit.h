#ifndef UNIT_H
#define UNIT_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <cmath> 
#include "utils.h"

class Terrain; // forward declaration
static const char* NO_SYMBOL = "  ";
using UnitProcess = void(*)(struct Unit* self, Terrain* terrain, long x, long y);
using ItemApply = void(*)(struct Unit* self, struct Unit* other);
using ItemDescripton = void(*)(struct Item* self);

struct Item {
    const char* name;
    const char* symbol;
    const char* description;
    long proficiency;
    ItemApply effect;
    uint8_t armor;
};

struct Unit {
    const char* name;
    const char* symbol;
    const char* effect;
    uint8_t to_spread;
    ItemApply spreads;
    uint8_t health;
    uint8_t max_health;
    uint8_t speed;
    uint8_t size;
    UnitProcess process;
    struct Unit* ai_target;
    std::vector<Item> items;

    // Default constructor
    Unit()
        : name("Unknown"), symbol("  "),
          health(5), max_health(5),
          speed(5), size(1),to_spread(0),
          process(nullptr), effect(nullptr), ai_target(nullptr) {}

    // Basic constructor – starting health = max_health automatically
    Unit(const char* nm, const char* sym,
         long size_, long max_health_,
         long speed_,
         UnitProcess proc = nullptr)
        : name(nm), symbol(sym),
          health(max_health_), max_health(max_health_),
          speed(speed_), size(size_),to_spread(0),
          process(proc), effect(nullptr), ai_target(nullptr) {}

    // Constructor that accepts one default item
    Unit(const char* nm, const char* sym,
         long size_, long max_health_,
         long speed_,
         const Item& default_item,
         UnitProcess proc = nullptr)
        : name(nm), symbol(sym),
          health(max_health_), max_health(max_health_),
          speed(speed_), size(size_),to_spread(0),
          process(proc), effect(nullptr), ai_target(nullptr)
    {
        items.push_back(default_item);
    }

    // Print basic info and items
    void info() const {
        std::cout << symbol << " " << name << "       ";
        if (health) std::cout << "❤️" << (int)health << "/" << (int)max_health << " ";
        if (size)   std::cout << "📏" << (int)size << " ";
        if (speed)  std::cout << "🐾" << (int)speed << " ";
        //for (auto& item : items) std::cout << item.symbol;
        std::cout << "\n";
    }

    // Fight using a specific item (or bare hands if item==nullptr)
    void fight(Unit* other, Item* item) {
        if(!item || !other || !other->health || other->effect || !item->effect) 
            return;
        item->proficiency += 1;
        other->effect = item->symbol;
        item->effect(this, other);
        other->ai_target = this;
    }

    void ai_step(Terrain& terrain, long my_x, long my_y, long interpolation_step);

};

// -----------------------------------------------------------------------------
//   Items
// -----------------------------------------------------------------------------
namespace item {
    static const Item sword  { "Sword ",  "🗡️",  "2 damage", 0, [](Unit*, Unit* other) {
        if(!other) return; 
        if(other->health>=2) other->health -= 2; else other->health = 0;}
    };
    static const Item axe    { "Axe   ",  "🪓",  "1 damage + 2 vs larger", 0, [](Unit* owner, Unit* other) {
        if(!other) return; 
        u_int8_t dmg = owner->size < other->size?3:1;
        if(other->health>=dmg) other->health -= dmg; else other->health = 0;}
    };
    static const Item hammer { "Hammer",  "🔨",  "1 damage + 3 vs undamaged", 0, [](Unit*, Unit* other) {
        if(!other) return; 
        u_int8_t dmg = other->health == other->max_health?4:1;
        if(other->health>=dmg) other->health -= dmg; else other->health = 0;}
    };
    static const Item claw   { "Claw  ",   "🩸",  "1 damage", 0, [](Unit*, Unit* other) {
        if(!other) return; if(other->health>=1) other->health -= 1; else other->health = 0;}
    }; 
    static const Item pound  { "Pound ",  "💥",  "1 damage", 0, [](Unit*, Unit* other) {
        if(!other) return; if(other->health>=1) other->health -= 1; else other->health = 0;}
    };
    static const Item bite   { "Bite  ",   "🦷",  "1 damage", 0, [](Unit*, Unit* other) {
        if(!other) return; if(other->health>=1) other->health -= 1; else other->health = 0;}
    };
    static void fire_damage(Unit* owner, Unit* other){
        if (!other) return;
        if (other->health >= 1) other->health -= 1;
        else other->health = 0;
        other->effect = "🔥";
        if (other->health && !other->spreads && other->max_health>=9)
            other->spreads = fire_damage;   // self-reference works
    }
    static const Item fire   { "Fire  ",   "🔥",  "1 damage, spreads through >9 max health", 0, fire_damage};
    static const Item zap    { "Zap   ",   "⚡",  "4 damage, 1 to yourself", 0, [](Unit* owner, Unit* other) {
        if(!other || !owner) return; 
        if(other->health>=4) other->health -= 4; else other->health = 0;
        if(owner->health>=1) owner->health -= 1; else owner->health = 0;
        owner->effect = "⚡";
    }};
    static const Item shield { "Shield",   "🛡️",  "prevent damage", 0, [](Unit* owner, Unit* other) {
        owner->effect = "🛡️";
        if(other)
            other->effect = nullptr;
    }};
}

namespace unit {

    // ── Terrain / environment
    static const Unit GRASS    ("Grass   ", NO_SYMBOL, 0,0,0);
    static const Unit TREE2    ("Tree    ", "🌳", 8,9,0);
    static const Unit TREE     ("Tree    ", "🌲", 8,9,0);
    static const Unit ROCK     ("Rock    ", "🪨", 5,5,0);
    static const Unit WATER    ("Water   ", "💧", 10, 0,0);
    static const Unit MOUNTAIN ("Mountain", "⛰️", 10, 0,0);

    // ── Civilized folk
    static const Unit KNIGHT   ("Knight  ", "🤺", 5,9,5, item::sword);
    static const Unit HUMAN    ("Human   ", "🙂", 5,5,5, item::sword);
    static const Unit ELF      ("Elf     ", "🧝", 5,5,7);
    static const Unit DWARF    ("Dwarf   ", "🧔", 3,9,3, item::hammer);
    static const Unit HALFLING ("Halfling", "🧑‍🦱", 3,3,7, item::axe);

    // ── Mounts & animals
    static const Unit HORSE    ("Horse   ", "🐎",10,5,9);
    static const Unit WOLF     ("Wolf    ", "🐺", 6,5,10, item::bite);
    static const Unit BEAR     ("Bear    ", "🐻", 8,5,5, item::claw);
    static const Unit EAGLE    ("Eagle   ", "🦅", 4,5,5);
    static const Unit UNICORN  ("Unicorn ", "🦄",10,9,9, item::pound);
    static const Unit LION     ("Lion    ", "🦁",10,5,5, item::bite);
    static const Unit DOVE     ("Dove    ", "🕊️", 2,5,5);
    static const Unit SNAKE    ("Snake   ", "🐍", 3,3,5, item::bite);
    static const Unit RABBIT   ("Rabbit  ", "🐇", 1,3,10);
    static const Unit MONKEY   ("Monkey  ", "🐒", 3,5,9);
    static const Unit SLIME    ("Slime   ", "🫧", 1,9,3);

    // ── Monsters
    static const Unit ORC      ("Orc     ", "👹", 6,6,6, item::sword);
    static const Unit GOBLIN   ("Goblin  ", "👺", 2,3,5, item::sword);
    static const Unit TROLL    ("Troll   ", "🧌", 7,5,2, item::hammer);
    static const Unit DRAGON   ("Dragon  ", "🐉",15,25,5, item::fire);
    static const Unit VAMPIRE  ("Vampire ", "🧛", 5,5,5, item::bite);
    static const Unit KRAKEN   ("Kraken  ", "🐙",12,5,2);
    static const Unit MINOTAUR ("Minotaur", "🐂", 7,5,5, item::pound);
    static const Unit BANSHEE  ("Banshee ", "👻", 5,5,5, item::fire);

    // ── Magic & undead
    static const Unit WIZARD   ("Wizard  ", "🧙", 5,5,4, item::fire);
    static const Unit WITCH    ("Witch   ", "🧙‍♀️",5,5,5);
    static const Unit SKELETON ("Skeleton", "💀", 5,3,5, item::sword);
    static const Unit ZOMBIE   ("Zombie  ", "🧟", 5,5,3, item::pound);
    static const Unit DEMON    ("Demon   ", "😈", 8,5,5);
    static const Unit ARCHMAGE ("Archmage", "🔮", 5,5,5);
    static const Unit LICH     ("Lich    ", "☠️", 5,5,5);
    static const Unit GOLEM    ("Golem   ", "🗿",12,9,2, item::pound);
    static const Unit SHADE    ("Shade   ", "🌑", 5,5,5);

    // ── Sample spell-like entities
    static const Unit ICESPIKE      ("Ice Spike",     "❄️",1,1,5);
    static const Unit LIGHTNINGBOLT ("Lightning Bolt","⚡",1,1,5);
    static const Unit HEALINGLIGHT  ("Healing Light", "✨",1,1,5);
    static const Unit MAGICSHIELD   ("Magic Shield",  "🛡️",1,1,5);
    static const Unit CURSE         ("Curse",         "☠️",1,1,5);
    static const Unit TELEPORT      ("Teleport",      "🌀",1,1,5);
    static const Unit TIMESTOP      ("Time Stop",     "⏳",1,1,5);
    static const Unit METEOR        ("Meteor",        "☄️",1,1,5);
    static const Unit CHARM         ("Charm",         "💖",1,1,5);
    static const Unit TORNADO       ("Tornado",       "🌪️",6,5,5);
}

#endif // UNIT_H
