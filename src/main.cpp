#include "AsmInterp.h"
#include <iostream>
#include <string>
#include <vector>

auto main() -> int {
    constexpr const std::string_view program =
R"PROGEND(
; My first program
mov  a, 5
inc  a
call function
msg  '(5+1)/2 = ', a    ; output message
end

function:
	div  a, 2
	ret
)PROGEND";

    constexpr const std::string_view program2 =
R"PROGEND(
mov a, 2 ; value1
mov b, 10 ; value2
mov c, a ; temp1
mov d, b ; temp2
call proc_func
call print
end

proc_func:
cmp d, 1
je continue
mul c, a
dec d
call proc_func

continue:
ret

print:
msg a, '^', b, ' = ', c
ret
)PROGEND";

	std::cout << assembler_interpreter(program) << '\n';
	std::cout << assembler_interpreter(program2) << '\n';
}
