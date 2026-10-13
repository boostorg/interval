/* Boost test/converting_ctor.cpp
 * test the converting constructors and assignment operators
 *
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/numeric/interval.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <string>

typedef boost::numeric::interval<double> I;
typedef boost::numeric::interval<float> F;

struct unrelated {};

// a number type that is not the base type but converts to it
struct num {
  double v;
  explicit num(double x): v(x) {}
  operator double() const { return v; }
};

// An interval must not claim to be convertible from a type that cannot be
// converted to its base type.
void test_traits() {
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<std::string, I>));
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<unrelated, I>));
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<void*, I>));
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<const char*, I>));

  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<double, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<int, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<float, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<long double, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<num, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<double, F>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<F, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<I, F>));
}

int f(I const &) { return 1; }
int f(std::string const &) { return 2; }

// A string literal converts to std::string only, so the call is not
// ambiguous.
void test_overload() {
  BOOST_TEST_EQ(f("abc"), 2);
  BOOST_TEST_EQ(f(1), 1);
  BOOST_TEST_EQ(f(1.5f), 1);
}

void test_constructors() {
  I a = 1;
  BOOST_TEST_EQ(a.lower(), 1.);
  BOOST_TEST_EQ(a.upper(), 1.);

  I b(1, 2);
  BOOST_TEST_EQ(b.lower(), 1.);
  BOOST_TEST_EQ(b.upper(), 2.);

  I c(1, 2.5f);
  BOOST_TEST_EQ(c.lower(), 1.);
  BOOST_TEST_EQ(c.upper(), 2.5);

  // a literal zero as second argument is a bound, not a null pointer
  I d(-1.5, 0);
  BOOST_TEST_EQ(d.lower(), -1.5);
  BOOST_TEST_EQ(d.upper(), 0.);

  I e = num(3.);
  BOOST_TEST_EQ(e.lower(), 3.);
  BOOST_TEST_EQ(e.upper(), 3.);

  // 0.1 is not representable in float, so the conversion has to round
  // outward
  F x(0.1);
  BOOST_TEST(x.lower() < x.upper());
  BOOST_TEST(static_cast<double>(x.lower()) <= 0.1);
  BOOST_TEST(0.1 <= static_cast<double>(x.upper()));

  F y(0.1, 0.3);
  BOOST_TEST(static_cast<double>(y.lower()) <= 0.1);
  BOOST_TEST(0.3 <= static_cast<double>(y.upper()));
}

void test_assignment() {
  I a;
  a = 3;
  BOOST_TEST_EQ(a.lower(), 3.);
  BOOST_TEST_EQ(a.upper(), 3.);

  a = 2.5f;
  BOOST_TEST_EQ(a.lower(), 2.5);
  BOOST_TEST_EQ(a.upper(), 2.5);

  a = num(4.);
  BOOST_TEST_EQ(a.lower(), 4.);
  BOOST_TEST_EQ(a.upper(), 4.);

  F x;
  x = 0.1;
  BOOST_TEST(x.lower() < x.upper());
  BOOST_TEST(static_cast<double>(x.lower()) <= 0.1);
  BOOST_TEST(0.1 <= static_cast<double>(x.upper()));

  a = x;
  BOOST_TEST_EQ(a.lower(), static_cast<double>(x.lower()));
  BOOST_TEST_EQ(a.upper(), static_cast<double>(x.upper()));
}

int main() {
  test_traits();
  test_overload();
  test_constructors();
  test_assignment();
  return boost::report_errors();
}
