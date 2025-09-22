#ifndef TILE_H
#define TILE_H

namespace bg {
    static const char* green = "\033[48;5;22m";
    static const char* brown = "\033[43m";
    static const char* blue  = "\033[44m";
    static const char* cyan  = "\033[48;2;173;216;230m";
    static const char* reset = "\033[0m";
    static const char* midthreat = "\033[43m";
    static const char* nothreat = "\033[42m";
    static const char* highthreat = "\033[44m";
}

struct Tile {
    const char* bgColor;
    Tile() = default;
    explicit Tile(const char* color) : bgColor(color) {}
};

#endif // TILE_H