// ******************************************************************************************* //
// *** Objective : replace matrix product in UBLAS with MKL, keeping boost UBLAS interface *** //
// ******************************************************************************************* //
using vector = boost::numeric::ublas::vector<double>;
using matrix = boost::numeric::ublas::matrix<double>;

struct matrix_trans
{
    const matrix& mat;
};

inline matrix_trans trans(const matrix& mat)
{
    return matrix_trans{mat};
}




// ************************************************************************** //
// boost::numeric::ublas has a lot of lazy operations, like :
//
//    auto ans = element_product(vec0,vec1) 
//
// which is an unknown type (may even changed across different boost version).
// Therefore we cannot define is_vector<T> traits with std::is_same, 
// instead   we can    define is_vector<T> traits with std::is_convertible, 
// even if the intermediate type may change, it must be able to materialize 
// as boost::numeric::ublas::vector.
// ************************************************************************** //
template<typename T> struct is_vector               : public std::false_type{};
template<>           struct is_vector<vector>       : public std:: true_type{};

template<typename T> struct is_matrix               : public std::false_type{};
template<>           struct is_matrix<matrix>       : public std:: true_type{};
template<>           struct is_matrix<matrix_trans> : public std:: true_type{};

template<typename T> struct is_transpose               : public std::false_type{};
template<>           struct is_transpose<matrix_trans> : public std:: true_type{};

template<typename T> constexpr bool is_vector_v    = is_vector<std::decay_t<T>>::value;
template<typename T> constexpr bool is_matrix_v    = is_matrix<std::decay_t<T>>::value;
template<typename T> constexpr bool is_transpose_v = is_transpose<std::decay_t<T>>::value;
template<typename T> constexpr bool is_dgemm_arg_v = std::is_convertible_v<T,vector> || is_vector_v<T> || is_matrix_v<T>;




// ************************************************************************** //
// 1. "if constexpt" allows to return different types in different branches
// 2.  if some branches return  lvalue or xvalue <--- pass by reference
//        some branches return prvalue           <--- pass by value
//     then declare return type as "decltype(auto)" 
// 3. caller should declare "auto&&", like following :
//
//    auto&& x = fct_that_returns_reference_or_value();
//
// 4. cannot use "const auto&&" 
// -  const auot&& is not universal reference
// -  const auto&& is rvalue reference
// ************************************************************************** //
template<typename T> 
requires is_dgemm_arg_v<T> 
decltype(auto) get_data(T&& x) // where x can be vector, matrix or matrix transpose
{
    if constexpr(is_transpose_v<T>)
    {
        return std::forward<T>(x).mat; // lvalue reference of matrix
    }
    else if constexpr(is_matrix_v<T> || is_vector_v<T>)
    {
        return std::forward<T>(x); // lvalue reference of matrix
    }
    else 
    {
        return vector{std::forward<T>(x)}; // prvalue vector (force it to materalize all lazy vector calculation by conversion to vector)
    }
}




// *************************************************** //
// *** Double general matrix matrix multiplication *** //
// *************************************************** //
template<typename TA, typename TB>
requires is_dgemm_arg_v<TA> &&
         is_dgemm_arg_v<TB>
auto dgemm(TA&& expression_A, TB&& expression_B)
{
    constexpr bool A_is_mat = is_matrix_v<TA>;
    constexpr bool B_is_mat = is_matrix_v<TB>;
    auto&& A = get_data(std::forward<T>(expression_A));
    auto&& B = get_data(std::forward<T>(expression_B));


    if constexpr (A_is_mat && B_is_mat)
    {
        CBLAS_TRANSPOSE transA = is_transpose_v<TA> ? CblasTrans : CblasNoTrans;
        CBLAS_TRANSPOSE transB = is_transpose_v<TB> ? CblasTrans : CblasNoTrans;
        MKL_INT M = (transA == CblasNoTrans) ? A.size1() : A.size2();
        MKL_INT N = (transB == CblasNoTrans) ? B.size2() : B.size1();
        MKL_INT K = (transA == CblasNoTrans) ? A.size2() : A.size1();

        matrix C(M, N);
        cblas_dgemm
        (
            CblasRowMajor, transA, transB, M, N, K, 1.0,
            A.data().begin(), A.size2(),
            B.data().begin(), B.size2(), 0.0,
            C.data().begin(), N
        );
        return C;
    }
    else if constexpr (A_is_mat && !B_is_mat)
    {
        CBLAS_TRANSPOSE transA = is_transpose_v<TA> ? CblasTrans : CblasNoTrans;
        MKL_INT M = (transA == CblasNoTrans) ? A.size1() : A.size2();
        MKL_INT N = 1;
        MKL_INT K = (transA == CblasNoTrans) ? A.size2() : A.size1();

        vector C(M); // M*1 matrix
        cblas_dgemm
        (
            CblasRowMajor, transA, CblasNoTrans, M, N, K, 1.0,
            A.data().begin(), A.size2(),
            B.data().begin(), 1, 0.0,
            C.data().begin(), 1
        );
        return C;
    }
    else if constexpr (!A_is_mat && B_is_mat)
    {
        CBLAS_TRANSPOSE transB = is_transpose_v<TB> ? CblasTrans : CblasNoTrans;
        MKL_INT M = 1;
        MKL_INT N = (transB == CblasNoTrans) ? B.size2() : B.size1();
        MKL_INT K = (transB == CblasNoTrans) ? B.size1() : B.size2();

        vector C(N); // 1*N matrix
        cblas_dgemm
        (
            CblasRowMajor, CblasNoTrans, transB, M, N, K, 1.0,
            A.data().begin(), A.size(),
            B.data().begin(), B.size2(), 0.0,
            C.data().begin(), C.size()
        );
        return C;
    }
    else
    {
        // Todo : dot product of vectors
        return double{0.0}
    }
}




// **************************************** //
// User can call in the following ways, 
// return type is governed by input arg : 
// **************************************** //
void test()
{
    matrix m0;
    matrix m1;
    vector v0;

    auto m2 = dgemm(      m0,        m1);
    auto m3 = dgemm(      m0,  trans(m1));
    auto m4 = dgemm(trans(m0),       m1);
    auto m5 = dgemm(trans(m0), trans(m1));
    auto v1 = dgemm(      m0,        v0);        // m0  * v0  where v0 is col matrix
    auto v2 = dgemm(trans(m0),       v0);        // m0' * v0  where v0 is col matrix
    auto v3 = dgemm(      v0,        m0);        // v0  * m0  where v0 is row matrix
    auto v4 = dgemm(      v0,  trans(m0));       // v0  * m0' where v0 is row matrix
}
