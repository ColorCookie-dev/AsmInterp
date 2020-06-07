#include "AsmInterp.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    // std::vector<std::string> program{"mov a, -5", "inc a",     "dec a",
    //"inc a",     "jnz a, -1", "inc a"};
    std::string program =
        "; My first program\n"
        "mov  a, 5\n"
        "inc  a\n"
        "call function\n"
        "msg  '(5+1)/2 = ', a    ; output message\n"
        "end\n"
        "\n"
        "function:\n"
        "	div  a, 2\n"
        "	ret\n";

    std::string program2 =
		"mov a, 2 ; value1\n"
		"mov b, 10 ; value2\n"
		"mov c, a ; temp1\n"
		"mov d, b ; temp2\n"
		"call proc_func\n"
		"call print\n"
		"end\n"
		"\n"
		"proc_func:\n"
		"cmp d, 1\n"
		"je continue\n"
		"mul c, a\n"
		"dec d\n"
		"call proc_func\n"
		"\n"
		"continue:\n"
		"ret\n"
		"\n"
		"print:\n"
		"msg a, '^', b, ' = ', c\n"
		"ret\n";

	std::cout << assembler_interpreter(program) << '\n';
	std::cout << assembler_interpreter(program2) << '\n';
}
