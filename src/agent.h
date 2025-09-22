#ifndef AGENT_H
#define AGENT_H

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <regex>
#include <thread>
#include <chrono>

extern std::string requested;
extern std::string cached;
std::string request_engine(Unit& u);

#endif //AGENT_H