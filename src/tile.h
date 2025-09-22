#ifndef TILE_H
#define TILE_H

namespace bg {
    static const char* green = "\033[48;5;22m";
    static const char* brown = "\033[43m";
    static const char* blue  = "\033[44m";
    static const char* cyan  = "\033[48;2;173;216;230m";
    static const char* reset = "\033[0m";
    static const char* threatening(const char* color){
        if (color == bg::green) return "\033[48;2;128;0;0m";   // dark red
        if (color == bg::brown) return "\033[48;2;139;0;0m";   // brick red
        if (color == bg::blue)  return "\033[48;2;178;34;34m"; // firebrick
        if (color == bg::cyan)  return "\033[48;2;220;20;60m"; // crimson
        if (color == bg::reset) return bg::reset;             // keep reset as-is
        return "\033[48;2;128;0;0m";                          // default: dark red
    }
}

struct Tile {
    const char* bgColor;
    Tile() = default;
    explicit Tile(const char* color) : bgColor(color) {}
};

#endif // TILE_H