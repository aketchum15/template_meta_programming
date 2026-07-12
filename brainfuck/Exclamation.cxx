#include "../src/bf.cxx"

#include <unistd.h>

// 33 is the first readable ascii char "!"
using Exclamation = typename PROG("+++++++++++++++++++++++++++++++++.");
using res = typename Eval<Exclamation>::type;

int main(void) {
    write(1, res::value, sizeof(res::value));
    return 0;

};

