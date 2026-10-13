/* Boost test/converting_ctor_fail.cpp
 * conversions to interval that must not compile
 *
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/numeric/interval.hpp>
#include <string>

// BOOST_NUMERIC_INTERVAL_TEST_FAIL selects one statement that must be
// rejected. Without it this file has to compile, which shows that nothing
// else in it is the cause of a failure.

typedef boost::numeric::interval<double> I;

struct unrelated {};

int f(I const &) { return 1; }
int f(std::string const &) { return 2; }

int main() {
  I x(1., 2.);
  unrelated u;
#if !defined(BOOST_NUMERIC_INTERVAL_TEST_FAIL)
  x = 3;
  (void)u;
  return f("abc") == 2 ? 0 : 1;
#elif BOOST_NUMERIC_INTERVAL_TEST_FAIL == 1
  // constructor from a type that does not convert to the base type
  I y(std::string("abc"));
  (void)y;
#elif BOOST_NUMERIC_INTERVAL_TEST_FAIL == 2
  // implicit conversion
  I y = u;
  (void)y;
#elif BOOST_NUMERIC_INTERVAL_TEST_FAIL == 3
  // two-argument constructor with one bound that does not convert
  I y(1., u);
  (void)y;
#elif BOOST_NUMERIC_INTERVAL_TEST_FAIL == 4
  // assignment
  x = std::string("abc");
#elif BOOST_NUMERIC_INTERVAL_TEST_FAIL == 5
  // a stray pointer must not be taken for the parameter that constrains
  // the one-argument constructor
  I y(1., &u);
  (void)y;
#endif
  (void)x;
  (void)u;
  return 0;
}
