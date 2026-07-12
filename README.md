### Template meta programming.

Just for fun.

#### tree.cxx

basic metatype defining a tree

#### hof.cxx

Implementations of some standard higher order functions over a list

#### bf.cxx

Compile time brain fuck interpreter (minus taking input). Implemented purely in types and macros, 0 constexpr.
PROG() macro to build a program from a string, recursive interpreter, and String type with static value to convert
bf program result to static string. Evaluates a hello world example brainfuck program, and prints the result.

#### build.sh
build script for bf.cxx. adds some flags to strip down binary size and increase template recursion depth limit.
builds `out/hello_world`. The output binary does not contain the original brainfuck program, running `strings` on it reveals
that the bf program is truly evaluated at compile time, and its result is hardcoded into the binary.
