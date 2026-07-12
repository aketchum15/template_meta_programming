//brain fuck interpreter
#include <unistd.h>

// evil macro to turn a string literal into a list of chars
// maxes out at 1024
#define SAFE_CHAR(s,i) (i < sizeof(s)-1 ? s[i] : '\0')
#define STR4(s, n)    SAFE_CHAR(s,0+n),SAFE_CHAR(s,1+n),SAFE_CHAR(s,2+n),SAFE_CHAR(s,3+n)
#define STR8(s, n)    STR4(s,n),STR4(s,n+4)
#define STR16(s, n)   STR8(s,n),STR8(s,n+8)
#define STR32(s, n)   STR16(s,n),STR16(s,n+16)
#define STR64(s, n)   STR32(s,n),STR32(s,n+32)
#define STR128(s, n)  STR64(s,n),STR64(s,n+64)
#define STR256(s, n)  STR128(s,n),STR128(s,n+128)
#define STR512(s, n)  STR256(s,n),STR256(s,n+256)
#define STR1024(s, n) STR512(s,n),STR512(s,n+512)

#define PROG(s) Strip<STR1024(s, 0)>::type

struct Unit {};

template <char... Cs>
struct String {
    static const char value[sizeof...(Cs) + 1];
};

template<char... Cs>
const char String<Cs...>::value[sizeof...(Cs) + 1] = {Cs..., '\0'};

template <typename S1, typename S2>
struct Append {};

template <char... Cs, char... Ds>
struct Append<String<Cs...>, String<Ds...>> {
    using type = String<Cs..., Ds...>;
};

template<char... Prog>
struct Strip{};

template <char C, char... Rest>
struct Strip<C, Rest...> {
    using type = typename Append<String<C>, typename Strip<Rest...>::type>::type;
};

template<char... Rest>
struct Strip<'\0', Rest... > {
    using type = String<>;
};

template<>
struct Strip<'\0'> {
    using type = String<>;
};

template<typename L, char Instr, typename R>
struct InstrPtr{};

template <typename L = String<>, char Cell = 0, typename R = String<>>
struct Tape {};

// Helper function for moving forward/backward through the program
// Handles end of list

template <typename InstrPtr>
struct StepPtrRight {};

template <char... IPrev, char Instr, char NextInstr, char ...INext>
struct StepPtrRight<InstrPtr<String<IPrev...>, Instr, String<NextInstr, INext...>>> {
    using type = InstrPtr<String<Instr, IPrev...>, NextInstr, String<INext...>>;
};

template<typename IPrev, char Instr> 
struct StepPtrRight<InstrPtr<IPrev, Instr, String<>>> {
    using type = String<>;
};

template <typename InstrPtr>
struct StepPtrLeft {};

template <char... IPrev, char PrevInstr, char Instr, char... INext>
struct StepPtrLeft<InstrPtr<String<PrevInstr, IPrev...>, Instr, String<INext...>>> {
    using type = InstrPtr<String<IPrev...>, PrevInstr, String<Instr, INext...>>;
};

template <char Instr, typename INext>
struct StepPtrLeft<InstrPtr<String<>, Instr, INext>> {
    using type = Unit;
};

// Helper functions for moving forward/backward through the tape
// Creates a new cell if reached the end

template<typename Tape>
struct StepTapeRight {};

// Normal case, just move to the right 
template <char... TPrev, char Cell, char NextCell, char... TNext>
struct StepTapeRight<Tape<String<TPrev...>, Cell, String<NextCell, TNext...>>> {
    using type = Tape<String<Cell, TPrev...>, NextCell, String<TNext...>>;
};

// End of tape, generate new cell
template <char... TPrev, char Cell>
struct StepTapeRight<Tape<String<TPrev...>, Cell, String<>>> {
    using type = Tape<String<Cell, TPrev...>, char{0}, String<>>;
};

template<typename Tape>
struct StepTapeLeft{};

// Normal case, just move to the right 
template <char... TPrev, char PrevCell, char Cell, char... TNext>
struct StepTapeLeft<Tape<String<PrevCell, TPrev...>, Cell, String<TNext...>>> {
    using type = Tape<String<TPrev...>, PrevCell, String<Cell, TNext...>>;
};

// End of tape, generate new cell
template <char Cell, char... TNext>
struct StepTapeLeft<Tape<String<>, Cell, String<TNext...>>> {
    using type = Tape<String<>, char{0}, String<Cell, TNext...>>;
};


// Function to find and jump to the closing ] 
template<typename InstrPtr, int Depth = 0>
struct ScanRight{};

template <typename IPrev, typename INext>
struct ScanRight<InstrPtr<IPrev, ']',  INext>, 0> {
    using type = typename StepPtrRight<InstrPtr<IPrev, ']', INext>>::type;
};

template <typename IPrev, typename INext, int Depth>
struct ScanRight<InstrPtr<IPrev, ']',  INext>, Depth> {
    using type = typename ScanRight<typename StepPtrRight<InstrPtr<IPrev, ']', INext>>::type, Depth-1>::type;
};

template <typename IPrev, typename INext, int Depth>
struct ScanRight<InstrPtr<IPrev, '[', INext>, Depth> {
    using type = typename ScanRight<typename StepPtrRight<InstrPtr<IPrev, '[', INext>>::type, Depth+1>::type;
};

template <typename IPrev, char Instr, typename INext, int Depth>
struct ScanRight<InstrPtr<IPrev, Instr, INext>, Depth> {
    using type = typename ScanRight<typename StepPtrRight<InstrPtr<IPrev, Instr, INext>>::type, Depth>::type;
};

template<int Depth>
struct ScanRight<Unit, Depth> {
    using type = Unit;
};

// Function to find and jump to the opening [
template<typename Instrs, int Depth = 0>
struct ScanLeft{};

template <typename IPrev, typename INext>
struct ScanLeft<InstrPtr<IPrev, '[', INext>, 0> {
    using type = typename StepPtrRight<InstrPtr<IPrev, '[', INext>>::type;
};

template <typename IPrev, typename INext, int Depth>
struct ScanLeft<InstrPtr<IPrev, '[', INext>, Depth> {
    using type = typename ScanLeft<typename StepPtrLeft<InstrPtr<IPrev, '[', INext>>::type, Depth-1>::type;
};

template <typename IPrev, typename INext, int Depth>
struct ScanLeft<InstrPtr<IPrev, ']', INext>, Depth> {
    using type = typename ScanLeft<typename StepPtrLeft<InstrPtr<IPrev, ']', INext>>::type, Depth+1>::type;
};

template <typename IPrev, char Instr, typename INext, int Depth>
struct ScanLeft<InstrPtr<IPrev, Instr, INext>, Depth> {
    using type = typename ScanLeft<typename StepPtrLeft<InstrPtr<IPrev, Instr, INext>>::type, Depth>::type;
};

template<int Depth>
struct ScanLeft<Unit, Depth> {
    using type = Unit;
};

// Execute an Instruction
template <typename Instrs, typename Tape>
struct Step{};

// PLUS
template<typename IPrev, typename INext, typename TPrev, char Cell, typename TNext>
struct Step<InstrPtr<IPrev, '+', INext>, Tape<TPrev, Cell, TNext>> {
    using type = typename Step<
        typename StepPtrRight<InstrPtr<IPrev, '+', INext>>::type,
        Tape<TPrev, Cell+1, TNext>
    >::type;
};

// MINUS
template<typename IPrev, typename INext, typename TPrev, char Cell, typename TNext>
struct Step<InstrPtr<IPrev, '-', INext>, Tape<TPrev, Cell, TNext>> {
    using type = typename Step<
        typename StepPtrRight<InstrPtr<IPrev, '-', INext>>::type, 
        Tape<TPrev, Cell-1, TNext>
    >::type;
};

// RIGHT
template<typename IPrev, typename INext, typename Tape>
struct Step<InstrPtr<IPrev, '>', INext>, Tape> {
    using type = typename Step<
        typename StepPtrRight<InstrPtr<IPrev, '>', INext>>::type,
        typename StepTapeRight<Tape>::type
    >::type;
};

// LEFT
template<typename IPrev, typename INext, typename Tape>
struct Step<InstrPtr<IPrev, '<', INext>, Tape> {
    using type = typename Step<
        typename StepPtrRight<InstrPtr<IPrev, '<', INext>>::type,
        typename StepTapeLeft<Tape>::type
    >::type;
};

// DOT
template<typename IPrev, typename INext, typename TPrev, char Cell, typename TNext>
struct Step<InstrPtr<IPrev, '.', INext>, Tape<TPrev, Cell, TNext>> {
    using type = typename Append<
        String<Cell>,
        typename Step<
            typename StepPtrRight<InstrPtr<IPrev, '.', INext>>::type,
            Tape<TPrev, Cell, TNext>
        >::type 
    >::type;
};

// No , for now :(

// OPEN_BRACKET 
template<typename IPrev, typename INext, typename TPrev, typename TNext>
struct Step<InstrPtr<IPrev, '[', INext>, Tape<TPrev, 0, TNext>> {
    using type = typename Step<
                    typename ScanRight<
                        typename StepPtrRight<InstrPtr<IPrev, '[', INext>>::type
                    >::type,
                    Tape<TPrev, 0, TNext>
                >::type;
};

template<typename IPrev, typename INext, typename TPrev, char Cell, typename TNext>
struct Step<InstrPtr<IPrev, '[', INext>, Tape<TPrev, Cell, TNext>> {
    using type = typename Step<
                    typename StepPtrRight<InstrPtr<IPrev, '[', INext>>::type,
                    Tape<TPrev, Cell, TNext>
                >::type;
};


// CLOSE_BRACKET
template<typename IPrev, typename INext, typename TPrev, typename TNext>
struct Step<InstrPtr<IPrev, ']', INext>, Tape<TPrev, 0, TNext>> {
    using type = typename Step<
                    typename StepPtrRight<InstrPtr<IPrev, ']', INext>>::type,
                    Tape<TPrev, 0, TNext>
                >::type;
};

template<typename IPrev, typename INext, typename TPrev, char Cell, typename TNext>
struct Step<InstrPtr<IPrev, ']', INext>, Tape<TPrev, Cell, TNext>> {
    using type = typename Step<
                    typename ScanLeft<
                        typename StepPtrLeft<InstrPtr<IPrev, ']', INext>>::type
                    >::type,
                    Tape<TPrev, Cell, TNext>
                >::type;
};


template<typename Tape>
struct Step<String<>, Tape> {
    using type = String<>;
};

template<typename Program>
struct Eval {};

template<char Instr, char... Rest>
struct Eval<String<Instr, Rest...>> {
    using type = typename Step<InstrPtr<String<>, Instr, String<Rest...>>, Tape<>>::type;
};

// 33 is the first readable ascii char "!"
using Exclamation = typename PROG("+++++++++++++++++++++++++++++++++.");
using res = typename Eval<Exclamation>::type;


// Hello World ! 
// https://en.wikipedia.org/wiki/Brainfuck#Hello_World!
using HelloWorld = typename PROG("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.");

using HelloRes = typename Eval<HelloWorld>::type;

int main(void) {
    write(1, HelloRes::value, sizeof(HelloRes::value));
    return 0;
}
