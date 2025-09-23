#ifndef DSL_H
#define DSL_H

#include "unit.h"
#include <iostream>
#include <array>
#include <random>

#define DESC(txt) {auto tmp=txt;if(tmp[0])std::cout << " " << tmp;}
#define RULE(ptr,BODY) if(u.name==(ptr).name){ BODY; std::cout<<"."; return; }
#define OPT(...) DESC(opt(rnd, __VA_ARGS__));

template <typename T,size_t N>
inline const T& pick(const std::array<T,N>& arr, std::mt19937& r){
    static_assert(N>0,"pick() requires non-empty array");
    std::uniform_int_distribution<size_t> d(0,N-1);
    auto val = d(r);
    for (std::size_t i = 0; i < val; ++i) r();
    return arr[val];
}

template<typename... Args>
const char* opt(std::mt19937& r, Args... args) {
    static_assert(sizeof...(args) > 0, "opt() requires at least one argument");
    const char* arr[] = { args... };
    std::uniform_int_distribution<size_t> d(0, sizeof...(args)-1);
    auto val = d(r);
    for (std::size_t i = 0; i < val; ++i) r();
    return arr[val];
}

inline void MOOD_VERB(const Unit& u, std::mt19937& r){
    static const auto frozen = std::to_array<const char*>({"is encased in ice","is stopped in time","is immobile","remains unmoving","has turned into stalactite","lays behind the ice","is frozen","does not move","is berefit of will"});
    static const auto hostile= std::to_array<const char*>({"glares","snarls","bares its teeth","hisses","scowls","growls low","sneers","oozes malice","is ready to engage"});
    static const auto wary   = std::to_array<const char*>({"eyes you warily","watches closely","circles with caution","keeps a distance","studies you","glances nervously","tenses","hesitates"});
    static const auto calm   = std::to_array<const char*>({"beholds you","measures you","watches quietly","stands at ease","observes unconcerned","remains still","looks on","remains calm"});
    static const auto friendl= std::to_array<const char*>({"admires you","greets you","approaches friendily","radiates goodwill","is cheerfull","glances wormly","is cheerful"});
    if(u.frozen) std::cout<<" "<<pick(frozen,r);
    else if(u.mood<=-5) std::cout<<" "<<pick(hostile,r);
    else if(u.mood<0) std::cout<<" "<<pick(wary,r);
    else if(u.mood<5) std::cout<<" "<<pick(calm,r);
    else std::cout<<" "<<pick(friendl,r);
}

inline void HEALTH_NOTE(const Unit& u, std::mt19937& r){
    static const auto critical= std::to_array<const char*>({", bleeding and weak",", barely standing",", gravely wounded",", battered and fatigued",", panting",", ready to collapse",", covered in wounds",", grievously hurt"});
    static const auto hurt    = std::to_array<const char*>({", worn but steady",", scratched and bruised",", marked by combat",", signs of battle",", lightly wounded",", body torn",", limping",", scarred"});
    if(u.health<u.max_health/3) std::cout<<pick(critical,r);
    else if(u.health<u.max_health) std::cout<<pick(hurt,r);
}
inline void ITEM_NOTE(const Unit& u, std::mt19937& r){
    if(u.items.empty()) return;
    const char* nm = u.items.front().name;

    static const auto weapon_phr  = std::to_array<const char*>({"Shows a fine ","Carries a balanced ","Bears a fine ","Keeps a trusted ","Holds a master ","Displays a worn ","Grips a firm ","Wears a venerable "});
    static const auto claw_phr    = std::to_array<const char*>({"Claws gleam","Claws poised","Claws show quiet power","Claws flex with ease"});
    static const auto bite_phr    = std::to_array<const char*>({"Bite shows quiet menace","Fangs gleam","Teeth hint at power","Jaws hold calm strength"});
    static const auto pound_phr   = std::to_array<const char*>({"Limbs coil with force","Arms carry silent might","Blows land with sure weight","Strength gathers calmly"});
    static const auto fire_hint   = std::to_array<const char*>({"Whispers of a sun not yet born","Sings a hymn to fire","Emits a red hush","Wreathed in curling smoke","The air tastes like ash"});
    static const auto charm_hint  = std::to_array<const char*>({"Sways your heart","Reveals a welcoming smile","Its words drip like honey","Waits in comforting quietness"});
    static const auto freez_hint  = std::to_array<const char*>({"A chill spreads","A palce visage","The air turns cold","Is clad in cold", "Whispers of winters past", "Becones the deep chill"});
    static const auto zap_hint    = std::to_array<const char*>({"Glances at the sky","All hair are standing on edge","Like a hush before the storm","A sizzling sound screeps"});

    std::cout<<". ";
    const char* weapon_nm = nullptr;
    if      (nm==item::sword.name)  weapon_nm = "blade";
    else if (nm==item::axe.name)    weapon_nm = "axe";
    else if (nm==item::hammer.name) weapon_nm = "hammer";
    else if (nm==item::shield.name) weapon_nm = "shield";

    if      (weapon_nm)             std::cout<<pick(weapon_phr,r)<<weapon_nm;
    else if (nm==item::claw.name)   std::cout<<pick(claw_phr,r);
    else if (nm==item::bite.name)   std::cout<<pick(bite_phr,r);
    else if (nm==item::pound.name)  std::cout<<pick(pound_phr,r);
    else if (nm==item::fire.name)   std::cout<<pick(fire_hint,r);
    else if (nm==item::charm.name)  std::cout<<pick(charm_hint,r);
    else if (nm==item::freeze.name) std::cout<<pick(freez_hint,r);
    else if (nm==item::zap.name)    std::cout<<pick(zap_hint,r);
    else                            std::cout<<pick(weapon_phr,r)<<(weapon_nm?weapon_nm:nm);
}




// ── Tree note: conditioned on health ratio ─────────────
inline void TREE_NOTE(const Unit& u, std::mt19937& r){
    static const auto healthy = std::to_array<const char*>({
        "its bark rough to the touch","heavy with dark green needles","home to chittering squirrels",
        "casting a deep cool shade","roots twisting like old ropes","dotted with mossy patches",
        "crowned with vibrant cones","alive with birdsong","sap glistening in the sun",
        "branches stretching to the sky","reaching proudly up","leaves whispering in the breeze","resin scenting the air"
    });
    static const auto worn = std::to_array<const char*>({
        "bark scarred","branches sagging","pocked with holes","streaked with lichen","leaning slightly",
        "bearing splintered knots","roots cracked by shifting soil","foliage thinned",
        "rough and timeworn","limbs bowed but enduring","showing signs of hardship"
    });
    static const auto dying = std::to_array<const char*>({
        "limbs bare and brittle","sap running sluggish and dark","bark peeling in great sheets",
        "leaves yellowed and sparse","roots exposed and dry","branches creak with weakness",
        "trunk hollowed","dying","crown thin and ghostly","brittle twigs litter the ground","the scent of rot clings to it"
    });
    double ratio = u.max_health? double(u.health)/u.max_health : 1.0;
    std::cout<<", ";
    if(ratio>2.0/3.0) std::cout<<pick(healthy,r);
    else if(ratio>1.0/3.0) std::cout<<pick(worn,r);
    else std::cout<<pick(dying,r);
}

inline void ROCK_NOTE(std::mt19937& r){
    static const auto desc = std::to_array<const char*>({
        "pitted with ancient scars","warm from the midday sun","streaked with lichen",
        "smooth from countless rains","that is cold and unyielding","flecked with tiny crystals",
        "veined with glimmering quartz","dotted with pockets of moss",
        "bearing the faint trace of fossil shells","split by a jagged crack",
        "glistening with morning dew","worn to a gentle curve by ages",
        "echoing a distant chill", "marked by time’s patient hand"
    });
    std::cout<<" "<<pick(desc,r);
}

inline const char* health_adj(const Unit& u, std::mt19937& r) {
    static const auto critical = std::to_array<const char*>({"faint","bloodied","dying","broken","wan","feeble","battered","ashen"});
    static const auto hurt = std::to_array<const char*>({"wounded","scarred","bruised","limping","cut","marked","hurt","aching"});
    static const auto healthy = std::to_array<const char*>({"vigorous","robust","hale","strong","spry","fit","hardy","sound"});
    double ratio = u.max_health? double(u.health)/u.max_health : 1.0;
    if(ratio < 1.0/3.0) return pick(critical,r);
    if(ratio < 1.0)     return pick(hurt,r);
    return pick(healthy,r);
}

inline const char* mood_adj(const Unit& u, std::mt19937& r) {
    static const auto frozen = std::to_array<const char*>({"frozen","immobile","ice-encased","petrified","timestopped"});
    static const auto hostile = std::to_array<const char*>({"hostile","snarling","menacing","malicious","feral","spiteful"});
    static const auto wary   = std::to_array<const char*>({"wary","uneasy","cautious","guarded","nervous","hesitant"});
    static const auto friendl= std::to_array<const char*>({"friendly","cheerful","warm","amiable","kindly","genial"});
    if(u.frozen) return pick(frozen,r);
    if(u.mood<=-5) return pick(hostile,r);
    else if(u.mood<0) return pick(wary,r);
    //else if(u.mood<5) return "";//pick(calm,r);
    return pick(friendl,r);
}
inline const char* item_adj(const Unit& u, std::mt19937& r) {
    if (u.items.empty()) {
        static const auto none   = std::to_array<const char*>({"unarmed","bare","plain","simple","innocent","unadorned","playful","timid"});
        return pick(none, r);
    }

    const char* nm = u.items.front().name;

    // adjective banks
    static const auto weapon  = std::to_array<const char*>({"armed","battle-ready","steel-clad","blade-wielding","martial","weaponed"});
    static const auto claw    = std::to_array<const char*>({"clawed","taloned","hooked","razor-tipped","barbed","sharp-clawed"});
    static const auto bite    = std::to_array<const char*>({"fanged","toothed","jagged-mouthed","sharp-jawed","fang-bearing","keen-fanged"});
    static const auto pound   = std::to_array<const char*>({"powerful-limbed","mighty-armed","heavy-striking","forceful","strong-limbed","impactful"});
    static const auto fire    = std::to_array<const char*>({"fiery","embered","smouldering","flame-touched","ash-kissed","charred"});
    static const auto charm   = std::to_array<const char*>({"charming","bewitching","honey-tongued","glamoured","entrancing","spellbound"});
    static const auto freeze  = std::to_array<const char*>({"cold","chilly","winter-touched","pale","snow-clad"});
    static const auto zap     = std::to_array<const char*>({"charged","crackling","sparking","stormy","electrified","static"});

    // choose adjective family based on the item
    if      (nm==item::sword.name || nm==item::axe.name || nm==item::hammer.name || nm==item::shield.name) return pick(weapon, r);
    else if (nm==item::claw.name)   return pick(claw, r);
    else if (nm==item::bite.name)   return pick(bite, r);
    else if (nm==item::pound.name)  return pick(pound, r);
    else if (nm==item::fire.name)   return pick(fire, r);
    else if (nm==item::charm.name)  return pick(charm, r);
    else if (nm==item::freeze.name) return pick(freeze, r);
    else if (nm==item::zap.name)    return pick(zap, r);

    // default: treat unknown as a generic weapon
    return pick(weapon, r);
}



inline void print_status(const Unit& u){
    std::seed_seq seed{ static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(&u)) };
    std::mt19937 rnd(seed);
    bool adj_mode = std::uniform_int_distribution<int>(0,2)(rnd);


    // Each uses: "A/An", adjective, noun
    RULE(unit::TREE,    DESC("A"); OPT("towering","tall","ancient","young","gnarled","mighty","slender","twisted"); OPT("fir","oak","pine","yew","maple","willow","ash","elm"); TREE_NOTE(u,rnd));
    RULE(unit::ROCK,    DESC("A"); OPT("solid","squat","jagged","mossy","timeworn","ancient"); OPT("rock","boulder","stone","crag","monolith"); ROCK_NOTE(rnd));
    RULE(unit::WATER,   DESC("A"); OPT("calm","rippling","misty","rushing","tranquil","sparkling"); OPT("pool","stream","cascade","pond","fall","sheet of water"));
    
    if(adj_mode==0){ // ── Item adjective ──
        RULE(unit::HUMAN,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("human","traveller","wanderer","pilgrim","scout"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::ELF,     OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("elf","ranger","sentinel","scout"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::DWARF,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("dwarf","miner","warrior","blacksmith","guard","prospector"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::WOLF,    OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("wolf","prowler","hunter","lone wolf"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::DRAGON,  OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("dragon","wyrm","serpent","leviathan","inferno"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::ORC,     OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("orc","raider","warrior","marauder"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::GOBLIN,  OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("goblin","thief","sneak","trickster"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::TROLL,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("troll","giant","brute"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::VAMPIRE, OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("vampire","fiend","night-stalker","predator"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::ZOMBIE,  OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("zombie","corpse","walker","ghoul","husk"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::DEMON,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("demon","fiend","hellspawn","devil"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::WIZARD,  OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("wizard","spellcaster","mage","sorcerer","conjurer"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::GOLEM,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("golem","sentinel","construct","colossus"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::HALFLING,OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("halfling","rogue","traveller","spryfolk","smallfolk"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::HORSE,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("horse","steed","charger","mount","stallion"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::BEAR,    OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("bear","grizzly"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::EAGLE,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("eagle","raptor","hunter","sentinel"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::UNICORN, OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("unicorn","horned steed","magical steed"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::LION,    OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("lion","king","predator","hunter"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::DOVE,    OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("dove","white bird","messenger"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::SNAKE,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("snake","viper","constrictor","ambusher"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::RABBIT,  OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("rabbit","bunny","hare","cottontail"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::MONKEY,  OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("monkey","primate","trickster","tree-climber"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::SLIME,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("slime","ooze","jelly","glob","mass"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::KRAKEN,  OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("kraken","sea terror","leviathan","monster"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::MINOTAUR,OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("minotaur","brute","warrior","guardian"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::BANSHEE, OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("banshee","spirit","phantom","specter"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::WITCH,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("witch","sorceress","crone","enchantress"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::SKELETON,OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("skeleton","bones","fighter"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::ARCHMAGE,OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("archmage","sorcerer","wizard","mage"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::LICH,    OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("lich","necromancer","overlord","lich king"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
        RULE(unit::SHADE,   OPT("A","Some","The"); DESC(item_adj(u,rnd)); OPT("shade","phantom","spirit","apparition"); MOOD_VERB(u,rnd); HEALTH_NOTE(u,rnd));
    }
    else if(adj_mode==1){ // ── Mood adjectives ──
        RULE(unit::HUMAN,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("human","traveller","wanderer","pilgrim","scout"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::ELF,     OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("elf","ranger","sentinel","scout"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::DWARF,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("dwarf","miner","warrior","blacksmith","guard","prospector"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::WOLF,    OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("wolf","prowler","hunter","lone wolf"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::DRAGON,  OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("dragon","wyrm","serpent","leviathan","inferno"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::ORC,     OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("orc","raider","warrior","marauder"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::GOBLIN,  OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("goblin","thief","sneak","trickster"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::TROLL,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("troll","giant","brute"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::VAMPIRE, OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("vampire","fiend","night-stalker","predator"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::ZOMBIE,  OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("zombie","corpse","walker","ghoul","husk"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::DEMON,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("demon","fiend","hellspawn","devil"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::WIZARD,  OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("wizard","spellcaster","mage","sorcerer","conjurer"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::GOLEM,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("golem","sentinel","construct","colossus"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::HALFLING,OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("halfling","rogue","traveller","spryfolk","smallfolk"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::HORSE,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("horse","steed","charger","mount","stallion"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::BEAR,    OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("bear","grizzly"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::EAGLE,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("eagle","raptor","hunter","sentinel"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::UNICORN, OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("unicorn","horned steed","magical steed"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::LION,    OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("lion","king","predator","hunter"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::DOVE,    OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("dove","white bird","messenger"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::SNAKE,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("snake","viper","constrictor","ambusher"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::RABBIT,  OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("rabbit","bunny","hare","cottontail"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::MONKEY,  OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("monkey","primate","trickster","tree-climber"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::SLIME,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("slime","ooze","jelly","glob","mass"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::KRAKEN,  OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("kraken","sea terror","leviathan","monster"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::MINOTAUR,OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("minotaur","brute","warrior","guardian"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::BANSHEE, OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("banshee","spirit","phantom","specter"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::WITCH,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("witch","sorceress","crone","enchantress"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::SKELETON,OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("skeleton","bones","fighter"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::ARCHMAGE,OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("archmage","sorcerer","wizard","mage"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::LICH,    OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("lich","necromancer","overlord","lich king"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::SHADE,   OPT("A","Some","The"); DESC(mood_adj(u,rnd)); OPT("shade","phantom","spirit","apparition"); HEALTH_NOTE(u,rnd); ITEM_NOTE(u,rnd));
    }
    else { // ── Health adjectives ──
        RULE(unit::HUMAN,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("human","traveller","wanderer","pilgrim","scout"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::ELF,     OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("elf","ranger","sentinel","scout"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::DWARF,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("dwarf","miner","warrior","blacksmith","guard","prospector"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::WOLF,    OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("wolf","prowler","hunter","lone wolf"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::DRAGON,  OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("dragon","wyrm","serpent","leviathan","inferno"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::ORC,     OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("orc","raider","warrior","marauder"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::GOBLIN,  OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("goblin","thief","sneak","trickster"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::TROLL,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("troll","giant","brute"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::VAMPIRE, OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("vampire","fiend","night-stalker","predator"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::ZOMBIE,  OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("zombie","corpse","walker","ghoul","husk"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::DEMON,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("demon","fiend","hellspawn","devil"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::WIZARD,  OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("wizard","spellcaster","mage","sorcerer","conjurer"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::GOLEM,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("golem","sentinel","construct","colossus"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::HALFLING,OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("halfling","rogue","traveller","spryfolk","smallfolk"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::HORSE,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("horse","steed","charger","mount","stallion"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::BEAR,    OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("bear","grizzly"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::EAGLE,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("eagle","raptor","hunter","sentinel"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::UNICORN, OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("unicorn","horned steed","magical steed"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::LION,    OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("lion","king","predator","hunter"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::DOVE,    OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("dove","white bird","messenger"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::SNAKE,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("snake","viper","constrictor","ambusher"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::RABBIT,  OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("rabbit","bunny","hare","cottontail"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::MONKEY,  OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("monkey","primate","trickster","tree-climber"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::SLIME,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("slime","ooze","jelly","glob","mass"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::KRAKEN,  OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("kraken","sea terror","leviathan","monster"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::MINOTAUR,OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("minotaur","brute","warrior","guardian"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::BANSHEE, OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("banshee","spirit","phantom","specter"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::WITCH,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("witch","sorceress","crone","enchantress"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::SKELETON,OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("skeleton","bones","fighter"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::ARCHMAGE,OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("archmage","sorcerer","wizard","mage"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::LICH,    OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("lich","necromancer","overlord","lich king"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
        RULE(unit::SHADE,   OPT("A","Some","The"); DESC(health_adj(u,rnd)); OPT("shade","phantom","spirit","apparition"); MOOD_VERB(u,rnd); ITEM_NOTE(u,rnd));
    }
    
    
    DESC("An unknown presence");
}

#undef RULE
#undef DESC
#endif // DSL_H
