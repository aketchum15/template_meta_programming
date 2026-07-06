// some basic higher order functions

#include <type_traits>

struct Unit{};

// List will be our basic monad
template<auto T, typename Next>
struct List{};

using L = List<1, List<2, List<3, Unit>>>;


// Map :: ((a -> b), M a) -> M b
template <auto F, typename M>
struct Map {};

template<auto F, auto T, typename Next>
struct Map<F, List<T, Next>> {
    using type = List<F(T), typename Map<F, Next>::type>;
};

//Base Case 
template<auto F>
struct Map<F, Unit> {
    using type = Unit;
};

// Fold ::((a->b), b, M a) -> b
template <auto F, auto Acc, typename M>
struct Fold{};

template<auto F, auto Acc, auto T, typename Next>
struct Fold<F, Acc, List<T, Next>> {
    static constexpr auto val = F(F(T, Acc), Fold<F, Acc, Next>::val);
};

template<auto F, auto Acc>
struct Fold<F, Acc, Unit> {
    static constexpr auto val = Acc;
};

//sums all elements
static_assert(Fold<[](int x, int y){return x + y;}, 0, L>::val == 6);


// Filter :: ((a-> bool), M a) -> M a
template<auto F, typename M>
struct Filter {};

template<auto F, auto T, typename Next>
struct Filter<F, List<T, Next>> {
    using type = std::conditional_t<F(T), List<T, Next>, typename Filter<F, Next>::type>;
};

template<auto F> 
struct Filter<F, Unit> {
    using type = Unit;
};

