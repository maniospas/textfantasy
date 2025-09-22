#ifndef UNIT_H
#define UNIT_H

#include <iostream>

class Terrain; // forward declaration for the process pointer
static const char* NO_SYMBOL = "  ";
using UnitProcess = void(*)(struct Unit* self, Terrain* terrain, long x, long y);

struct Unit {
    const char* name;    // new: human-readable name
    const char* symbol;        // icon/emoji
    const char* effect;
    long  melee;         // melee damage
    long  health;        // current health
    long  max_health;    // maximum health
    long  speed;         // movement speed
    long  size;          // relative size for riding rules
    UnitProcess process; // behaviour callback – may be nullptr

    Unit()
        : name("Unknown"), symbol("  "),
          melee(0), health(5), max_health(5),
          speed(5), size(1), process(nullptr), effect(nullptr) {}

    Unit(const char* nm, const char* sym,
         long melee_, long size_,
         long health_, long max_health_,
         long speed_,
         UnitProcess proc = nullptr)
        : name(nm), symbol(sym),
          melee(melee_), health(health_), max_health(max_health_),
          speed(speed_), size(size_), process(proc), effect(nullptr) {}

    long threat() const { return health * melee; }

    // Print only non-zero stats
    void info() const {
        // 🗡 sword  ❤️ heart  🏃 runner  📏 ruler
        std::cout << symbol << " " << name << "       ";
        if (health) std::cout << "❤️" << health << "/" << max_health << " ";
        if (melee)  std::cout << "🗡 " << melee << " ";
        // if (speed)  std::cout << "🏃" << speed << " ";
        // if (size)   std::cout << "📏" << size << " ";
        std::cout << "\n";
    }
    void fight(Unit* other) {
        if(health<=other->melee)
            health = 0;
        else
            health -= other->melee;
        if(!health)
            symbol = NO_SYMBOL;
        if(other->melee)
            effect = "💥";
        if(other->health<=melee)
            other->health = 0;
        else
            other->health -= melee;
        if(!other->health)
            other->symbol = NO_SYMBOL;
        if(melee)
            other->effect = "💥";
    }
};

namespace unit {

    // ── Terrain / environment
    static const Unit GRASS    ("Grass   ", NO_SYMBOL, 0, 0, 0,0,0);
    static const Unit TREE2    ("Tree    ", "🌳", 0, 0,10,10,0);
    static const Unit TREE     ("Tree    ", "🌲", 0, 0,10,10,0);
    static const Unit ROCK     ("Rock    ", "🪨", 0, 0,10,10,0);
    static const Unit WATER    ("Water   ", "💧", 0, 0, 0,0,0);
    static const Unit MOUNTAIN ("Mountain", "⛰️", 0, 0,0,0,0);

    // ── Civilized folk
    static const Unit HUMAN    ("Human   ", "🙂", 1, 5, 5,5,5);
    static const Unit ELF      ("Elf     ", "🧝", 1, 5, 5,5,5);
    static const Unit DWARF    ("Dwarf   ", "🧔", 1, 5, 5,5,5);
    static const Unit HALFLING ("Halfling", "🧑‍🦱",1,5, 5,5,5);

    // ── Mounts & animals
    static const Unit HORSE    ("Horse   ", "🐎", 0,10, 5,5,10);
    static const Unit WOLF     ("Wolf    ", "🐺", 1, 6, 5,5,10);
    static const Unit BEAR     ("Bear    ", "🐻", 2, 8, 5,5,5);
    static const Unit EAGLE    ("Eagle   ", "🦅", 1, 4, 5,5,5);
    static const Unit UNICORN  ("Unicorn ", "🦄", 2,10, 5,5,10);
    static const Unit LION     ("Lion    ", "🦁", 3, 6, 5,5,5);
    static const Unit DOVE     ("Dove    ", "🕊️",0, 2, 5,5,5);
    static const Unit SERPENT  ("Serpent ", "🐍", 1, 3, 3,3,5);
    static const Unit RABBIT   ("Rabbit  ", "🐇", 0, 1, 2,2,10);
    static const Unit SLIME    ("Slime   ", "🫧", 0, 1, 2,2,5);

    // ── Monsters 
    static const Unit ORC      ("Orc     ", "👹", 1, 5, 5,5,5);
    static const Unit GOBLIN   ("Goblin  ", "👺", 1, 2, 5,5,5);
    static const Unit TROLL    ("Troll   ", "🧌", 1, 7, 5,5,2);
    static const Unit DRAGON   ("Dragon  ", "🐉",10,15, 5,5,5);
    static const Unit GIANT    ("Giant   ", "🗿", 1,12, 5,5,2);
    static const Unit VAMPIRE  ("Vampire ", "🧛", 1, 5, 5,5,5);
    static const Unit KRAKEN   ("Kraken  ", "🐙",10,12, 5,5,2);
    static const Unit MINOTAUR ("Minotaur", "🐂", 1, 7, 5,5,5);
    static const Unit BANSHEE  ("Banshee ", "👻", 1, 5, 5,5,5);

    // ── Magic & undead
    static const Unit WIZARD   ("Wizard  ", "🧙", 1, 5, 5,5,5);
    static const Unit WITCH    ("Witch   ", "🧙‍♀️",1,5, 5,5,5);
    static const Unit SKELETON ("Skeleton", "💀", 1, 5, 5,5,5);
    static const Unit ZOMBIE   ("Zombie  ", "🧟", 1, 5, 5,5,5);
    static const Unit DEMON    ("Demon   ", "😈",10, 8, 5,5,5);
    static const Unit ARCHMAGE ("Archmage", "🔮", 1, 5, 5,5,5);
    static const Unit LICH     ("Lich    ", "☠️", 1, 5, 5,5,5);
    static const Unit GOLEM    ("Golem   ", "🗿", 1,10, 5,5,2);
    static const Unit SHADE    ("Shade   ", "🌑", 1, 5, 5,5,5);

    // ── Spells & magical effects (health = 1, melee = 2, speed = 5)
    static const Unit FIREBALL      ("Fireball",      "🔥", 2, 1, 1,1,5);
    static const Unit ICESPIKE      ("Ice Spike",     "❄️", 2, 1, 1,1,5);
    static const Unit LIGHTNINGBOLT ("Lightning Bolt","⚡", 2, 1, 1,1,5);
    static const Unit HEALINGLIGHT  ("Healing Light", "✨", 2, 1, 1,1,5);
    static const Unit MAGICSHIELD   ("Magic Shield",  "🛡️",2, 1, 1,1,5);
    static const Unit CURSE         ("Curse",         "☠️", 2, 1, 1,1,5);
    static const Unit TELEPORT      ("Teleport",      "🌀", 2, 1, 1,1,5);
    static const Unit TIMESTOP      ("Time Stop",     "⏳", 2, 1, 1,1,5);
    static const Unit METEOR        ("Meteor",        "☄️", 2, 1, 1,1,5);
    static const Unit CHARM         ("Charm",         "💖", 2, 1, 1,1,5);
    static const Unit TORNADO       ("Tornado","🌪️", 1, 6, 5,5,5);
}

#endif // UNIT_H
