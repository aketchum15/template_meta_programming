## Template meta programming.

Just for fun.

### src
#### tree.cxx

basic metatype defining a tree

#### hof.cxx

Implementations of some standard higher order functions over a list

#### bf.cxx

Compile time brain fuck interpreter (minus input token  ","). Implemented purely in types and macros, 0 `constexpr`.
`PROG()` macro to build a program from a string, recursive interpreter, and String type with static member to convert
bf program output to a static string.

### brainfuck/
directory for example brainfuck programs.

### build.sh
build script for `brainfuck/HelloWorld.cxx` example. adds some flags to strip down binary size and increase template recursion depth limit.
builds `out/hello_world`. The output binary does not contain the original brainfuck program, running `strings` on it reveals
that the bf program is truly evaluated at compile time and its result is hardcoded into the binary.
