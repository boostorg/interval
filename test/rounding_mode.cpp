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

int main() {
#if !(defined(BOOST_NO_FENV_H) && defined(__GNUC__) && defined(__SSE_MATH__))
  // Without <fenv.h>, GCC and clang on x86 set the x87 control word only,
  // which SSE arithmetic ignores (issue #23).
  test<float>();
  test<double>();
#endif
#if !(defined(__clang__) && (defined(__i386__) || defined(__x86_64__))) \
  && !defined(__i386__) && !defined(_M_IX86) && !defined(_M_X64)
  // clang on x86, and GCC on 32-bit x86, move the long double division
  // across the change of rounding mode (issue #4).
  test<long double>();
#endif
  return boost::report_errors();
}
