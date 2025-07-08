#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <mkl.h>
#include <type_traits>

namespace ublas = boost::numeric::ublas;

// Helper: unwrap transpose expression
template <typename T>
const T& get_base(const T& m) { return m; }

template <typename M>
const M& get_base(const ublas::matrix_transpose<M>& m) { return m.expression(); }

// Helper: check if a type is transposed
template <typename T>
constexpr bool is_transposed_v = std::is_same_v<std::decay_t<T>, ublas::matrix_transpose<const ublas::matrix<double>>>;

template <typename A, typename B>
auto my_product(const A& a_expr, const B& b_expr) {
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

    if constexpr (!A_is_vec && !B_is_vec) {
        // matrix × matrix → matrix
        CBLAS_TRANSPOSE transA = A_is_trans ? CblasTrans : CblasNoTrans;
        CBLAS_TRANSPOSE transB = B_is_trans ? CblasTrans : CblasNoTrans;

        MKL_INT M = (transA == CblasNoTrans) ? A.size1() : A.size2();
        MKL_INT K = (transA == CblasNoTrans) ? A.size2() : A.size1();
        MKL_INT N = (transB == CblasNoTrans) ? B.size2() : B.size1();

        matrix<double> C(M, N);
        cblas_dgemm(CblasRowMajor, transA, transB, M, N, K,
                    1.0, A_data, A.size2(), B_data, B.size2(), 0.0, &C.data()[0], N);
        return C;

    } else if constexpr (A_is_vec && !B_is_vec) {
        // vector × matrix → vector
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

    } else if constexpr (!A_is_vec && B_is_vec) {
        // matrix × vector → vector
        CBLAS_TRANSPOSE transA = A_is_trans ? CblasTrans : CblasNoTrans;
        MKL_INT M = (transA == CblasNoTrans) ? A.size1() : A.size2();
        MKL_INT K = (transA == CblasNoTrans) ? A.size2() : A.size1();

        vector<double> result(M);
        cblas_dgemv(CblasRowMajor, transA,
                    M, K, 1.0,
                    A_data, A.size2(),
                    &b_expr[0], 1, 0.0, &result[0], 1);
        return result;

    } else {
        static_assert(!A_is_vec || !B_is_vec, "vector × vector is not supported");
    }
}
