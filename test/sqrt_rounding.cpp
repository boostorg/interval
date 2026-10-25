/* Boost test/sqrt_rounding.cpp
 * the rounded square roots of the rounding policies follow the rounding mode
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// The sqrt of the 32-bit Microsoft runtime rounds to nearest whatever the
// rounding mode, so sqrt_down and sqrt_up returned the same value there.
//
// sqrt is correctly rounded, so for a non-square x the two ends must differ,
// and for an exact square both must be the exact root.  Both calls take the
// same plain local operand, so the test also fails if the compiler merges
// the two roots across the mode change (GCC bug 34678).  Only the input
// arrays are volatile, so that nothing is folded at build time.

#include <boost/numeric/interval.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/limits.hpp>

template<class Rounding, class T>
void test_policy()
{
  static volatile T non_squares[] = { 2, 3, 5, 7, 10, 1.5f, 0.75f };
  static volatile T squares[] = { 1, 4, 9, 2.25f, 0.25f };
  const int n_non = sizeof(non_squares) / sizeof(non_squares[0]);
  const int n_sq = sizeof(squares) / sizeof(squares[0]);
  for (int i = 0; i < n_non; ++i) {
    Rounding rnd;
    T a = non_squares[i];
    T dn = rnd.sqrt_down(a);
    T up = rnd.sqrt_up(a);
    BOOST_TEST(dn < up);
  }
  for (int i = 0; i < n_sq; ++i) {
    Rounding rnd;
    T a = squares[i];
    T dn = rnd.sqrt_down(a);
    T up = rnd.sqrt_up(a);
    T x = squares[i];
    BOOST_TEST(dn == up);
    BOOST_TEST(dn * dn == x);
  }
}

template<class T>
void test()
{
  using namespace boost::numeric::interval_lib;
  test_policy<save_state<rounded_arith_opp<T> >, T>();
  test_policy<save_state<rounded_arith_std<T> >, T>();
}

int main()
{
  test<float>();
  test<double>();
  // IBM double-double long double (106 bits) does not follow the rounding
  // mode.
  if (std::numeric_limits<long double>::digits != 106)
    test<long double>();
  return boost::report_errors();
}
