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
    int8_t mood;

    uint8_t health;
    uint8_t max_health;
    uint8_t speed;
    uint8_t size;

    UnitProcess process;
    ItemApply spreads;

    uint8_t to_spread;
    uint8_t provisions;
    uint8_t materials;
    uint8_t stories;

    uint8_t money;
    uint8_t fame;
    uint8_t cycle;
    uint8_t frozen;

    struct Unit* ai_target;
    std::vector<Item> items;

    // Default constructor
    Unit()
        : name("Unknown"), symbol("  "), effect(nullptr),
        mood(0), health(5), max_health(5),
        speed(5), size(1),
        process(nullptr), spreads(), to_spread(0),
        provisions(0), materials(0), stories(0), money(0), fame(0),
        cycle(0), frozen(0),
        ai_target(nullptr)
    {}

    // Basic constructor – starting health = max_health automatically
    Unit(const char* nm, const char* sym,
        long size_, long max_health_,
        long speed_,
        UnitProcess proc = nullptr)
        : name(nm), symbol(sym), effect(nullptr),
        mood(0), health(max_health_), max_health(max_health_),
        speed(speed_), size(size_),
        process(proc), spreads(), to_spread(0),
        provisions(0), materials(0), stories(0), money(0), fame(0),
        cycle(0), frozen(0),
        ai_target(nullptr)
    {}

    // Constructor that accepts one default item
    Unit(const char* nm, const char* sym,
        long size_, long max_health_,
        long speed_,
        const Item& default_item,
        UnitProcess proc = nullptr)
        : name(nm), symbol(sym), effect(nullptr),
        mood(0), health(max_health_), max_health(max_health_),
        speed(speed_), size(size_),
        process(proc), spreads(), to_spread(0),
        provisions(0), materials(0), stories(0), money(0), fame(0),
        cycle(0), frozen(0),
        ai_target(nullptr)
    {
        items.push_back(default_item);
    }

    // Constructor that accepts two default items
    Unit(const char* nm, const char* sym,
        long size_, long max_health_,
        long speed_,
        const Item& default_item1,
        const Item& default_item2,
        UnitProcess proc = nullptr)
        : name(nm), symbol(sym), effect(nullptr),
        mood(0), health(max_health_), max_health(max_health_),
        speed(speed_), size(size_),
        process(proc), spreads(), to_spread(0),
        provisions(0), materials(0), stories(0), money(0), fame(0),
        cycle(0), frozen(0),
        ai_target(nullptr)
    {
        items.push_back(default_item1);
        items.push_back(default_item2);
    }


    // Print basic info and items
    void info() const {
        std::cout << symbol << " " << name << "       ";
        if (health)    std::cout << "❤️ HP "       << (int)health    << "/" << (int)max_health << "   ";
        if (provisions)std::cout << "🍗 Food "     << (int)provisions << "   ";
        if (materials) std::cout << "🪵 Material " << (int)materials;
        //if (size)   std::cout << "📏 size " << (int)size << " ";
        //if (speed)  std::cout << "🐾 speed " << (int)speed << " ";
        //for (auto& item : items) std::cout << item.symbol;
        std::cout << "\n";
    }
    void info_secondary() const {
        std::cout << "                  ";
        if (stories)   std::cout << "📜 Stories "  << (int)stories    << "   ";
        if (money)     std::cout << "🪙 Money "    << (int)money     << "   ";
        if (fame)      std::cout << "💬 Fame "     << (int)fame;
        //if (size)   std::cout << "📏 size " << (int)size << " ";
        //if (speed)  std::cout << "🐾 speed " << (int)speed << " ";
        //for (auto& item : items) std::cout << item.symbol;
        std::cout << "\n";
    }
    void base_info() const {
        std::cout << symbol << " " << name << "       ";
        //if (health) std::cout << "❤️ HP " << (int)health << "/" << (int)max_health << " ";
        //if (size)   std::cout << "📏 size " << (int)size << " ";
        //if (speed)  std::cout << "🐾 speed " << (int)speed << " ";
        //for (auto& item : items) std::cout << item.symbol;
        std::cout << "\n";
    }


    // Fight using a specific item (or bare hands if item==nullptr)
    void fight(Unit* other, Item* item) {
        if(!health && max_health) return;
        if(frozen) {
            effect = "❄️";
            return;
        }
        if(!item || !other || !other->health /*|| other->effect*/ || !item->effect)  return;
        if(mood>0) {
           //mood--;
           effect = "💖";
           return;
        }
        if(other->mood>-10)
            other->mood -= 3;
        item->proficiency++;
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
    static const Item axe    { "Axe   ",  "🪓",  "1 damage +2 vs larger", 0, [](Unit* owner, Unit* other) {
        if(!other) return; 
        uint8_t damage = owner->size < other->size?3:1;
        if(other->health>=damage) other->health -= damage; else other->health = 0;}
    };
    static const Item hammer { "Hammer",  "🔨",  "1 damage +3 vs undamaged +2 vs frozen", 0, [](Unit*, Unit* other) {
        if(!other) return; 
        uint8_t damage = other->health == other->max_health?4:1;
        if(other->frozen) damage += 2;
        other->frozen = 0;
        if(other->health>=damage) other->health -= damage; else other->health = 0;}
    };
    static const Item claw   { "Claw  ",   "🩸",  "1 damage", 0, [](Unit*, Unit* other) {
        if(!other) return; 
        if(other->health>=1) other->health -= 1; else other->health = 0;}
    }; 
    static const Item pound  { "Pound ",  "💥",  "1 damage per 2 HP", 0, [](Unit* owner, Unit* other) {
        if(!other || !owner) return; 
        uint8_t damage = owner->health/2;
        if(other->health>=damage) other->health -= damage; else other->health = 0;}
    };
    static const Item bite   { "Bite  ",   "🦷",  "2 damage, 1 HP regained", 0, [](Unit* owner, Unit* other) {
        if(!other || !owner) return; 
        if(other->health>=2) {
            other->health-=2;
            if(owner->health<owner->max_health)
                owner->health++;
        } 
        else other->health = 0;
    }};
    static void fire_damage(Unit* owner, Unit* other){
        if (!other) return;
        if (other->health >= 1) other->health--;
        else other->health = 0;
        other->effect = "🔥";
        if (other->health && !other->spreads && other->max_health>=9)
            other->spreads = fire_damage;   // self-reference works
    }
    static const Item fire   { "Fire  ",   "🔥",  "1 damage spreads through >8 max HP", 0, fire_damage};
    static const Item zap    { "Zap   ",   "⚡",  "4 damage and 1 to you", 0, [](Unit* owner, Unit* other) {
        if(!other || !owner) return; 
        if(other->health>=4) other->health -= 4; else other->health = 0;
        if(owner->health>=1) owner->health -= 1; else owner->health = 0;
        owner->effect = "⚡";
    }};
    static const Item charm { "Charm ",   "💖",  "reduce hostility for a bit and 1 damage to you", 0, [](Unit* owner, Unit* other) {
        if(!other || !owner) return;
        if(owner->health)
            owner->health--; 
        // deliberately allow underflows/overflows at 128
        if(other->mood<0) 
            other->mood += 10;
        else
            other->mood = 10;
    }};
    static const Item freeze { "Freeze",   "❄️",  "immobilize for a bit and 1 damage to you", 0, [](Unit* owner, Unit* other) {
        if(!other || !owner) return;
        if(owner->health)
            owner->health--; 
        if(other->frozen<32) other->frozen += 10;
    }};
    static const Item shield { "Shield",   "🛡️",  "prevent damage", 0, [](Unit* owner, Unit* other) {
        owner->effect = "🛡️";
        if(other)
            other->effect = nullptr;
    }};
}

namespace unit {

    // ── Terrain / environment
    static const Unit GRASS    ("GRASS   ", NO_SYMBOL, 0,0,0);
    static const Unit TREE2    ("TREE    ", "🌳", 8,9,0);
    static const Unit TREE     ("TREE    ", "🌲", 8,9,0);
    static const Unit ROCK     ("ROCK    ", "🪨", 5,5,0);
    static const Unit WATER    ("WATER   ", "💧", 10, 0,0);
    static const Unit MOUNTAIN ("MOUNTAIN", "⛰️", 10, 0,0);

    // ── Civilized folk
    //static const Unit KNIGHT   ("KNIGHT  ", "🤺", 5,9,5, item::sword);
    static const Unit HUMAN    ("HUMAN   ", "🙂", 5,5,5, item::sword);
    static const Unit ELF      ("ELF     ", "🧝", 5,5,7);
    static const Unit DWARF    ("DWARG   ", "🧔", 3,9,3, item::hammer);
    static const Unit HALFLING ("SPRYFOLK", "🧑‍🦱", 3,3,7, item::axe);

    // ── Mounts & animals
    static const Unit HORSE    ("HORSE   ", "🐎",10,5,9);
    static const Unit WOLF     ("WOLF    ", "🐺", 6,5,10, item::bite);
    static const Unit BEAR     ("BEAR    ", "🐻", 8,5,5, item::claw);
    static const Unit EAGLE    ("EAGLE   ", "🦅", 4,5,5, item::claw);
    static const Unit UNICORN  ("UNICORN ", "🦄",10,9,9, item::pound);
    static const Unit LION     ("LION    ", "🦁",10,5,5, item::bite);
    static const Unit DOVE     ("DOVE    ", "🕊️", 2,5,5);
    static const Unit SNAKE    ("SNAKE   ", "🐍", 3,3,5, item::bite);
    static const Unit RABBIT   ("RABBIT  ", "🐇", 1,3,10);
    static const Unit MONKEY   ("MONKEY  ", "🐒", 3,5,9, item::pound);
    static const Unit SLIME    ("SLIME   ", "🫧", 1,9,3);

    // ── Monsters
    static const Unit ORC      ("ORC     ", "👹", 6,6,6, item::sword);
    static const Unit GOBLIN   ("GOBLIN  ", "👺", 2,3,5, item::sword);
    static const Unit TROLL    ("TROLL   ", "🧌", 7,5,2, item::hammer);
    static const Unit DRAGON   ("DRAGON  ", "🐲",15,25,5, item::fire); // 🐲 🐉
    static const Unit VAMPIRE  ("VAMPIRE ", "🧛", 5,5,5, item::bite, item::charm);
    static const Unit KRAKEN   ("KRAKEN  ", "🐙",12,5,2);
    static const Unit MINOTAUR ("MINOTAUR", "🐂", 7,5,5, item::pound);
    static const Unit BANSHEE  ("BANSHEE ", "👻", 5,5,5, item::charm, item::fire);

    // ── Magic & undead
    static const Unit WIZARD   ("WIZARD  ", "🧙", 5,5,4, item::fire);
    static const Unit WITCH    ("WITCH   ", "🧙‍♀️", 5,5,4, item::freeze);
    static const Unit SKELETON ("SKELETON", "💀", 5,3,5, item::sword);
    static const Unit ZOMBIE   ("ZOMBIE  ", "🧟", 5,5,3, item::pound);
    static const Unit DEMON    ("DEMON   ", "😈", 8,5,5, item::fire, item::charm);
    static const Unit ARCHMAGE ("ARCHMAGE", "🔮", 5,5,5, item::fire, item::freeze);
    static const Unit LICH     ("LICH    ", "☠️", 5,5,5, item::fire, item::freeze);
    static const Unit GOLEM    ("GOLEM   ", "🗿",12,9,2, item::pound);
    static const Unit SHADE    ("SHADE   ", "🌑", 5,5,5, item::freeze);

    // ── Sample spell-like entities
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
