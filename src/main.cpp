#include "AsmInterp.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> program{"mov a -5", "inc a",    "dec a",
                                     "inc a",    "jnz a -1", "inc a"};
    // Complex2 - This test case creates some complications for obvious reasons
    // mov d 100
    // dec d
    // mov b d
    // jnz b -2
    // inc d
    // mov a d
    // jnz 5 10
    // mov c a

    auto regs = assembler(program);
    for (auto &reg : regs) {
        std::cout << reg.first << ": " << reg.second << '\n';
    }
}
