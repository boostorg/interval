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
#include <limits>
#include "bugs.hpp"

// Each function below changes the rounding mode, performs one operation and
// restores the rounding mode. They are not inlined, so that the compiler
// cannot evaluate the operation beforehand but is still free to move it
// across the changes of rounding mode if nothing prevents it.

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

template<class T>
BOOST_NOINLINE T add_down(T a, T b) {
  typedef boost::numeric::interval_lib::rounding_control<T> rc;
  typename rc::rounding_mode mode;
  rc::get_rounding_mode(mode);
  rc::downward();
  T r = rc::force_rounding(a + b);
  rc::set_rounding_mode(mode);
  return r;
}

template<class T>
BOOST_NOINLINE T add_up(T a, T b) {
  typedef boost::numeric::interval_lib::rounding_control<T> rc;
  typename rc::rounding_mode mode;
  rc::get_rounding_mode(mode);
  rc::upward();
  T r = rc::force_rounding(a + b);
  rc::set_rounding_mode(mode);
  return r;
}

template<class T>
void test() {
  volatile T v1 = 1, v3 = 3, vmin = (std::numeric_limits<T>::min)();
  T one = v1, three = v3, tiny = vmin;

  // 1/3 is not representable in a binary format.
  BOOST_TEST(div_down(one, three) < div_up(one, three));

  // 1 + tiny is not representable either, unless the format has a variable
  // precision (double-double).
  if ((one + tiny) - one != tiny) {
    BOOST_TEST(add_down(one, tiny) == one);
    BOOST_TEST(add_up(one, tiny) > one);
    BOOST_TEST(add_down(-one, -tiny) < -one);
    BOOST_TEST(add_up(-one, -tiny) == -one);
  }

  // The same through the interval class.
  typedef boost::numeric::interval<T> I;
  I q = I(one) / I(three);
  BOOST_TEST(q.lower() < q.upper());
}

int main() {
  test<float>();
  test<double>();
  test<long double>();
# ifdef BOOST_BORLANDC
  ::detail::ignore_warnings();
# endif
  return boost::report_errors();
}
