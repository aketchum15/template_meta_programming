#include <type_traits>
struct Leaf {};

template<int, typename L, typename R>
struct Node {};


template <typename T>
struct sum {};

//     1
//     /\
//    /  \
//   2    3
//  /\    /\
// L  L  L  L
using tree = Node<1, Node<2, Leaf, Leaf>, Node<3, Leaf, Leaf>>;

// sums all values in the tree
template<int N, typename L, typename R>
struct sum<Node<N, L, R>> {
    static constexpr int count = N + sum<L>::count + sum<R>::count;
};

template<>
struct sum<Leaf> {
    static constexpr int count = 0;
};

static_assert(sum<tree>::count == 6);

// compiler (and lsp) error :(
static_assert(sum<tree>::count == 3);



//Generic transform function applys the function provided to each value in the tree
// ((int -> int), Tree) -> Tree
template<typename T, auto F>
struct transform{};

template <auto F, int N, typename L, typename R>
struct transform<Node<N, L, R>, F> {
    using type = Node<F(N), typename transform<L, F>::type, typename transform<R, F>::type>;
};

template<auto F>
struct transform<Leaf, F> {
    using type = Leaf;
};

// define any constexpr function from int to int 
constexpr int square(int x) { return x * x;};

using tree_prime = transform<tree, square>;

// deep compare the types 
static_assert(std::is_same_v<tree_prime::type,Node<1, Node<4, Leaf, Leaf>, Node<9, Leaf, Leaf>>>);


//even better we can use lambdas
using tree_lambda = transform<tree, [](int i ) {return i*i;}>;





