/* Boost test/rounding_merge.cpp
 * the rounded primitives of the rounding policies follow the rounding mode
 *
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// The interval functions call the _down and the _up primitive of a policy on
// the same operands, one after the other, with a rounding-mode change in
// between.  GCC does not see the mode change as a dependency, so it may
// evaluate x + y once and use the result for both ends (GCC bug 34678).  The
// interval is then too narrow and may miss the true value.
//
// Each check below calls both primitives of the same policy object on the
// same plain local operands, so that the compiler is free to merge them.
// Only the input arrays are volatile, so that nothing is folded at build
// time.  For an inexact operation the two results must differ; for an exact
// one both must be the exact value.  The square root is checked in
// sqrt_rounding.cpp and sqrt_point.cpp.
//
// Built by the test suite with -frounding-math (see Jamfile.v2 /
// CMakeLists.txt).  Control: add -DCONTROL_EXACT_POLICY to run the same
// checks on the exact (non-rounding) policy.  The control must FAIL; if it
// passes, the test cannot see a merge.

#include <boost/numeric/interval.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/limits.hpp>
#include <cmath>

using namespace boost::numeric;
using namespace boost::numeric::interval_lib;

#define BINARY_TEST(OP, EXACT)                                         \
template<class Rounding, class T>                                      \
void test_##OP()                                                       \
{                                                                      \
  /* x = a / b and y = c / d, divided at run time: g++ 12.3 with      \
     -frounding-math miscompiles a static initializer that has to     \
     round, such as 0.1L for a float.  The quotients are stored to    \
     volatile T, so that x87 excess precision is dropped. */          \
  static volatile T inexact[][4] =                                     \
    { { 3, 1, 1, 10 }, { 1, 10, 3, 1 }, { 7, 1, 7, 10 }, { 7, 10, 7, 1 } }; \
  const int n = sizeof(inexact) / sizeof(inexact[0]);                  \
  for (int i = 0; i < n; ++i) {                                        \
    volatile T xv = T(inexact[i][0]) / T(inexact[i][1]);               \
    volatile T yv = T(inexact[i][2]) / T(inexact[i][3]);               \
    T x = xv, y = yv;                                                  \
    T dn, up;                                                          \
    {                                                                  \
      Rounding rnd;                                                    \
      dn = rnd.OP##_down(x, y);                                        \
      up = rnd.OP##_up(x, y);                                          \
    }                                                                  \
    BOOST_TEST(dn < up);                                               \
  }                                                                    \
  static volatile T exact[2] = { 1, 2 };                               \
  T x = exact[0], y = exact[1];                                        \
  T dn, up;                                                            \
  {                                                                    \
    Rounding rnd;                                                      \
    dn = rnd.OP##_down(x, y);                                          \
    up = rnd.OP##_up(x, y);                                            \
  }                                                                    \
  BOOST_TEST(dn == up);                                                \
  BOOST_TEST(dn == T(EXACT));                                          \
}

BINARY_TEST(add, 3)
BINARY_TEST(sub, -1)
BINARY_TEST(mul, 2)
BINARY_TEST(div, 0.5)

#undef BINARY_TEST

template<class Rounding, class T>
void test_int()
{
  using std::floor;
  using std::ceil;
  static volatile T values[] = { 2.5f, -2.5f };
  const int n = sizeof(values) / sizeof(values[0]);
  for (int i = 0; i < n; ++i) {
    T x = values[i];
    T dn, up;
    {
      Rounding rnd;
      dn = rnd.int_down(x);
      up = rnd.int_up(x);
    }
    BOOST_TEST(dn == floor(x));
    BOOST_TEST(up == ceil(x));
  }
}

// Only a conversion to float from double is inexact here.
template<class Rounding>
void test_conv()
{
  static volatile double values[] = { 0.1, -0.1 };
  const int n = sizeof(values) / sizeof(values[0]);
  for (int i = 0; i < n; ++i) {
    double v = values[i];
    float dn, up;
    {
      Rounding rnd;
      dn = rnd.conv_down(v);
      up = rnd.conv_up(v);
    }
    BOOST_TEST(dn < up);
  }
}

template<class Rounding, class T>
void test_policy()
{
  test_add<Rounding, T>();
  test_sub<Rounding, T>();
  test_mul<Rounding, T>();
  test_div<Rounding, T>();
  test_int<Rounding, T>();
}

#if defined(CONTROL_EXACT_POLICY)
template<class T> struct std_policy
{ typedef save_state<rounded_arith_exact<T> > type; };
template<class T> struct opp_policy
{ typedef save_state<rounded_arith_exact<T> > type; };
#else
template<class T> struct std_policy
{ typedef save_state<rounded_arith_std<T> > type; };
template<class T> struct opp_policy
{ typedef save_state<rounded_arith_opp<T> > type; };
#endif

template<class T>
void test()
{
  test_policy<typename std_policy<T>::type, T>();
  test_policy<typename opp_policy<T>::type, T>();
}

int main()
{
  test<float>();
  test<double>();
  // IBM double-double long double (106 bits) does not follow the rounding
  // mode: the libgcc division gives the same result in every mode.
  if (std::numeric_limits<long double>::digits != 106)
    test<long double>();
  test_conv<std_policy<float>::type>();
  test_conv<opp_policy<float>::type>();
  return boost::report_errors();
}
