


#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/operation.hpp> // for trans()
#include <type_traits>

namespace ublas = boost::numeric::ublas;

// Trait to detect if T is a ublas::matrix
template<typename T>
struct is_ublas_matrix : std::false_type {};

template<typename T, typename Alloc>
struct is_ublas_matrix<ublas::matrix<T, Alloc>> : std::true_type {};

template<typename T>
constexpr bool is_ublas_matrix_v = is_ublas_matrix<T>::value;

// Trait to detect if T is a ublas::vector
template<typename T>
struct is_ublas_vector : std::false_type {};

template<typename T, typename Alloc>
struct is_ublas_vector<ublas::vector<T, Alloc>> : std::true_type {};

template<typename T>
constexpr bool is_ublas_vector_v = is_ublas_vector<T>::value;

// Trait to detect if T is a transpose expression
template<typename U>
constexpr bool is_transpose_v = std::is_same<
    std::decay_t<decltype(ublas::trans(std::declval<U>()))>,
    std::decay_t<U>
>::value;

// Combined traits for convenience:
template<typename T>
constexpr bool is_matrix_or_transpose_v = is_ublas_matrix_v<T> || is_transpose_v<T>;

template<typename T>
constexpr bool is_matrix_or_vector_or_transpose_v =
    is_ublas_matrix_v<T> || is_ublas_vector_v<T> || is_transpose_v<T>;

// Example usage (optional, for testing):
#ifdef EXAMPLE_USAGE
#include <iostream>
int main() {
    ublas::matrix<double> m(3,3);
    auto t = ublas::trans(m);
    ublas::vector<double> v(3);

    std::cout << std::boolalpha;
    std::cout << "is_ublas_matrix_v<m> = " << is_ublas_matrix_v<decltype(m)> << "\n";
    std::cout << "is_transpose_v<t> = " << is_transpose_v<decltype(t)> << "\n";
    std::cout << "is_ublas_vector_v<v> = " << is_ublas_vector_v<decltype(v)> << "\n";

    std::cout << "is_matrix_or_vector_or_transpose_v<m> = "
              << is_matrix_or_vector_or_transpose_v<decltype(m)> << "\n";
    std::cout << "is_matrix_or_vector_or_transpose_v<t> = "
              << is_matrix_or_vector_or_transpose_v<decltype(t)> << "\n";
    std::cout << "is_matrix_or_vector_or_transpose_v<v> = "
              << is_matrix_or_vector_or_transpose_v<decltype(v)> << "\n";

    return 0;
}
#endif






template <typename A, typename B>
auto my_product(const A& a_expr, const B& b_expr) 
{
    using ublas::matrix;
    using ublas::vector;

    constexpr bool A_is_vec = std::is_same_v<std::decay_t<A>, vector<double>>;
    constexpr bool B_is_vec = std::is_same_v<std::decay_t<B>, vector<double>>;
    constexpr bool A_is_trans = is_transposed_v<A>;
    constexpr bool B_is_trans = is_transposed_v<B>;

    const auto& A = get_base(a_expr);
    const auto& B = get_base(b_expr);

    const double* A_data = &A.data()[0];
    const double* B_data = &B.data()[0];

    if constexpr (!A_is_vec && !B_is_vec) 
    {
        CBLAS_TRANSPOSE transA = A_is_trans ? CblasTrans : CblasNoTrans;
        CBLAS_TRANSPOSE transB = B_is_trans ? CblasTrans : CblasNoTrans;

        MKL_INT M = (transA == CblasNoTrans) ? A.size1() : A.size2();
        MKL_INT K = (transA == CblasNoTrans) ? A.size2() : A.size1();
        MKL_INT N = (transB == CblasNoTrans) ? B.size2() : B.size1();

        matrix<double> C(M, N);
        cblas_dgemm(CblasRowMajor, transA, transB, M, N, K,
                    1.0, A_data, A.size2(), B_data, B.size2(), 0.0, &C.data()[0], N);
        return C;

    } 
    else if constexpr (A_is_vec && !B_is_vec) 
    {
        CBLAS_TRANSPOSE transB = B_is_trans ? CblasTrans : CblasNoTrans;
        MKL_INT K = A.size();
        MKL_INT N = (transB == CblasNoTrans) ? B.size2() : B.size1();

        vector<double> result(N);
        cblas_dgemv(CblasRowMajor, transB, 
                    (transB == CblasNoTrans) ? B.size1() : B.size2(),
                    (transB == CblasNoTrans) ? B.size2() : B.size1(),
                    1.0, B_data, B.size2(), 
                    &a_expr[0], 1, 0.0, &result[0], 1);
        return result;

    } 
    else if constexpr (!A_is_vec && B_is_vec) 
    {
        CBLAS_TRANSPOSE transA = A_is_trans ? CblasTrans : CblasNoTrans;
        MKL_INT M = (transA == CblasNoTrans) ? A.size1() : A.size2();
        MKL_INT K = (transA == CblasNoTrans) ? A.size2() : A.size1();

        vector<double> result(M);
        cblas_dgemv(CblasRowMajor, transA,
                    M, K, 1.0,
                    A_data, A.size2(),
                    &b_expr[0], 1, 0.0, &result[0], 1);
        return result;

    } 
    else 
    {
    }
}
