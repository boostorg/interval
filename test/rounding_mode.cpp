/* Boost test/rounding_mode.cpp
 * test if operations are performed in the rounding mode that was requested
 *
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/numeric/interval.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/config.hpp>

// The functions are not inlined, so that the compiler cannot evaluate the
// division beforehand but is still free to move it across the changes of
// rounding mode if nothing prevents it.

template<class T>
BOOST_NOINLINE T div_down(T a, T b) {
  typedef boost::numeric::interval_lib::rounding_control<T> rc;
  typename rc::rounding_mode mode;
  rc::get_rounding_mode(mode);
  rc::downward();
  T r = rc::force_rounding(a / b);
  rc::set_rounding_mode(mode);
  return r;
}

template<class T>
BOOST_NOINLINE T div_up(T a, T b) {
  typedef boost::numeric::interval_lib::rounding_control<T> rc;
  typename rc::rounding_mode mode;
  rc::get_rounding_mode(mode);
  rc::upward();
  T r = rc::force_rounding(a / b);
  rc::set_rounding_mode(mode);
  return r;
}

// 1/3 is not representable in a binary format.
template<class T>
void test() {
  volatile T one = 1, three = 3;
  BOOST_TEST(div_down<T>(one, three) < div_up<T>(one, three));
}

// Exact constants reach force_rounding once the division is folded; GCC
// used to reject the PowerPC long double asm then.
template<class T>
void test_constant() {
  typedef boost::numeric::interval<T> I;
  I q = I(6) / I(2);
  BOOST_TEST(q.lower() == 3 && q.upper() == 3);
}

int main() {
  test<float>();
  test<double>();
  test<long double>();
  test_constant<float>();
  test_constant<double>();
  test_constant<long double>();
  return boost::report_errors();
}
