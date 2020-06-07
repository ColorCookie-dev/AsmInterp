#include "AsmInterp.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    // std::vector<std::string> program{"mov a, -5", "inc a",     "dec a",
    //"inc a",     "jnz a, -1", "inc a"};
    std::vector<std::string> program = {
        "; My first program",
        "mov  a, 5",
        "inc  a",
        "call function",
        "msg  '(5+1)/2 = ', a    ; output message",
        "end",
        "",
        "function:",
        "	div  a, 2",
        "	ret"
	};

    std::vector<std::string> program2 = {
		"mov a, 2 ; value1",
		"mov b, 10 ; value2",
		"mov c, a ; temp1",
		"mov d, b ; temp2",
		"call proc_func",
		"call print",
		"end",
		"",
		"proc_func:",
		"cmp d, 1",
		"je continue",
		"mul c, a",
		"dec d",
		"call proc_func",
		"",
		"continue:",
		"ret",
		"",
		"print:",
		"msg a, '^', b, ' = ', c",
		"ret"
	};

    //assembler(program);
    assembler(program2);
}
