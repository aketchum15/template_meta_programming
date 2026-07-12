//brain fuck interpreter

#include "hof.cxx"

using ex = List<'.', List<'>', List<'<', Unit>>>;

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

#define PROG(s) Program<STR1024(s, 0)>::type

template<char... Prog>
struct Program{};

template <char C, char... Rest>
struct Program<C, Rest...> {
    using type = List<C, typename Program<Rest...>::type>;
};

template<char... Rest>
struct Program<'\0', Rest... > {
    using type = Unit;
};

template<>
struct Program<'\0'> {
    using type = Unit;
};


template<typename L, char Instr, typename R>
struct InstrPtr{};

template <typename L = Unit, char Cell = 0, typename R = Unit>
struct Tape {};

// Helper function for moving forward/backward through the program
// Handles end of list

template <typename InstrPtr>
struct StepPtrRight {};

template <typename IPrev, char Instr, char NextInstr, typename INext>
struct StepPtrRight<InstrPtr<IPrev, Instr, List<NextInstr, INext>>> {
    using type = InstrPtr<List<Instr, IPrev>, NextInstr, INext>;
};

template<typename IPrev, char Instr> 
struct StepPtrRight<InstrPtr<IPrev, Instr, Unit>> {
    using type = Unit;
};

template <typename InstrPtr>
struct StepPtrLeft {};

template <typename IPrev, char PrevInstr, char Instr, typename INext>
struct StepPtrLeft<InstrPtr<List<PrevInstr, IPrev>, Instr, INext>> {
    using type = InstrPtr<IPrev, PrevInstr, List<Instr, INext>>;
};

template <char Instr, typename INext>
struct StepPtrLeft<InstrPtr<Unit, Instr, INext>> {
    using type = Unit;
};

// Helper functions for moving forward/backward through the tape
// Creates a new cell if I reached the end (Unit)

template<typename Tape>
struct StepTapeRight {};

// Normal case, just move to the right 
template <typename TPrev, char Cell, char NextCell, typename TNext>
struct StepTapeRight<Tape<TPrev, Cell, List<NextCell, TNext>>> {
    using type = Tape<List<Cell, TPrev>, NextCell, TNext>;
};

// End of tape, generate new cell
template <typename TPrev, char Cell>
struct StepTapeRight<Tape<TPrev, Cell, Unit>> {
    using type = Tape<List<Cell, TPrev>, char{0}, Unit>;
};

template<typename Tape>
struct StepTapeLeft{};

// Normal case, just move to the right 
template <typename TPrev, char PrevCell, char Cell, typename TNext>
struct StepTapeLeft<Tape<List<PrevCell, TPrev>, Cell, TNext>> {
    using type = Tape<TPrev, PrevCell, List<Cell, TNext>>;
};

// End of tape, generate new cell
template <char Cell, typename TNext>
struct StepTapeLeft<Tape<Unit, Cell, TNext>> {
    using type = Tape<Unit, char{0}, List<Cell, TNext>>;
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
    using type = List<
        Cell,
        typename Step<
            typename StepPtrRight<InstrPtr<IPrev, '.', INext>>::type,
            Tape<TPrev, Cell, TNext>
        >::type 
    >;
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
struct Step<Unit, Tape> {
    using type = Unit;
};

template<typename Program>
struct Eval {};

template<char Instr, typename Rest>
struct Eval<List<Instr, Rest>> {
    using type = typename Step<InstrPtr<Unit, Instr, Rest>, Tape<>>::type;
};

// 33 is the first readable ascii char "!"
using Exclamation = typename PROG("+++++++++++++++++++++++++++++++++.");
using res = typename Eval<Exclamation>::type;


// Hello World ! 
// https://en.wikipedia.org/wiki/Brainfuck#Hello_World!
using HelloWorld = typename PROG("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.");

using HelloRes = typename Eval<HelloWorld>::type;

int main(void) {
    HelloWorld r;
}
