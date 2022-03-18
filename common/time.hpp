#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

using std::chrono::high_resolution_clock;
using std::chrono::system_clock;

uint64_t Now();
std::string ToString(uint64_t nanoseconds_);

uint64_t ToMs(uint64_t Us);
