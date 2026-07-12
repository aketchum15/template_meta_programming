#include "bf.cxx"

#include <unistd.h>

// Hello World ! 
// https://en.wikipedia.org/wiki/Brainfuck#Hello_World!
using HelloWorld = typename PROG("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.");

using HelloRes = typename Eval<HelloWorld>::type;

int main(void) {
    write(1, HelloRes::value, sizeof(HelloRes::value));
    return 0;
}
