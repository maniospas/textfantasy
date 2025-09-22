#include "utils.h"

std::mt19937 rng{std::random_device{}()};
std::uniform_real_distribution<> prob(0.0, 1.0);
std::uniform_real_distribution<> dist(0.0, 1.0);
std::uniform_int_distribution<int> dirDist(0, 3);