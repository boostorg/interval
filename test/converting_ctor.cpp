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
typedef boost::numeric::interval<unrelated> U;

// same base type as I, different policies
typedef boost::numeric::interval<double,
  boost::numeric::interval_lib::policies<
    boost::numeric::interval_lib::save_state<
      boost::numeric::interval_lib::rounded_transc_std<double> >,
    boost::numeric::interval_lib::checking_no_nan<double> > > N;

// Hides x from the optimizer: GCC up to 11 converts a double constant to float
// at compile time in round-to-nearest, even with -frounding-math, so a literal
// would not test the rounding policy.
double runtime(double x) {
#if defined(__GNUC__)
  __asm__ __volatile__ ("" : "+m"(x));
#endif
  return x;
}

// a number type that is not the base type but converts to it
struct num {
  double v;
  explicit num(double x): v(x) {}
  operator double() const { return v; }
};

#if !defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
// a number type that converts to the base type only explicitly
struct explicit_num {
  double v;
  explicit explicit_num(double x): v(x) {}
  explicit operator double() const { return v; }
};
#endif

// An interval must not claim to be convertible from a type that cannot be
// converted to its base type.
void test_traits() {
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<std::string, I>));
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<unrelated, I>));
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<void*, I>));
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<const char*, I>));
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<U, I>));
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<boost::numeric::interval<std::string>, I>));

  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<double, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<int, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<float, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<long double, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<num, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<double, F>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<F, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<I, F>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<N, I>));
  BOOST_TEST_TRAIT_TRUE((boost::is_convertible<I, N>));

#if !defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
  // an explicit conversion to the base type must not become an implicit
  // conversion to the interval
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<explicit_num, I>));
  BOOST_TEST_TRAIT_FALSE((boost::is_convertible<explicit_num, N>));
#endif
}

int f(I const &) { return 1; }
int f(std::string const &) { return 2; }

int g(I const &) { return 1; }
int g(U const &) { return 2; }

// A string literal converts to std::string only, so the call is not
// ambiguous.
void test_overload() {
  BOOST_TEST_EQ(f("abc"), 2);
  BOOST_TEST_EQ(f(1), 1);
  BOOST_TEST_EQ(f(1.5f), 1);
  // float converts to double only, so only one of the intervals can be built
  BOOST_TEST_EQ(g(F(1.5f)), 1);
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
  F x(runtime(0.1));
  BOOST_TEST(x.lower() < x.upper());
  BOOST_TEST(static_cast<double>(x.lower()) <= 0.1);
  BOOST_TEST(0.1 <= static_cast<double>(x.upper()));

  F y(runtime(0.1), runtime(0.3));
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
  x = runtime(0.1);
  BOOST_TEST(x.lower() < x.upper());
  BOOST_TEST(static_cast<double>(x.lower()) <= 0.1);
  BOOST_TEST(0.1 <= static_cast<double>(x.upper()));

  a = x;
  BOOST_TEST_EQ(a.lower(), static_cast<double>(x.lower()));
  BOOST_TEST_EQ(a.upper(), static_cast<double>(x.upper()));
}

void test_interval_conversion() {
  // same base type, different policies: the bounds are copied
  N n(1., 2.);
  I a(n);
  BOOST_TEST_EQ(a.lower(), 1.);
  BOOST_TEST_EQ(a.upper(), 2.);

  a = N(3., 4.);
  BOOST_TEST_EQ(a.lower(), 3.);
  BOOST_TEST_EQ(a.upper(), 4.);

  // different base type: the bounds go through the rounding policy
  I d(runtime(0.1), runtime(0.3));
  F x(d);
  BOOST_TEST(static_cast<double>(x.lower()) <= 0.1);
  BOOST_TEST(0.3 <= static_cast<double>(x.upper()));

  x = I(runtime(0.1));
  BOOST_TEST(x.lower() < x.upper());
  BOOST_TEST(static_cast<double>(x.lower()) <= 0.1);
  BOOST_TEST(0.1 <= static_cast<double>(x.upper()));
}

// A type that converts to the base type only explicitly is converted by the
// user first; the result then goes through the usual constructors.
void test_explicit_conversion() {
#if !defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
  explicit_num n(runtime(0.1)), m(runtime(0.3));

  I a(static_cast<double>(n));
  BOOST_TEST_EQ(a.lower(), 0.1);
  BOOST_TEST_EQ(a.upper(), 0.1);

  N b(static_cast<double>(n), static_cast<double>(m));
  BOOST_TEST_EQ(b.lower(), 0.1);
  BOOST_TEST_EQ(b.upper(), 0.3);

  b = static_cast<double>(m);
  BOOST_TEST_EQ(b.lower(), 0.3);
  BOOST_TEST_EQ(b.upper(), 0.3);

  // the conversion to float still goes through the rounding policy and
  // rounds outward
  F x(static_cast<double>(n));
  BOOST_TEST(x.lower() < x.upper());
  BOOST_TEST(static_cast<double>(x.lower()) <= 0.1);
  BOOST_TEST(0.1 <= static_cast<double>(x.upper()));
#endif
}

int main() {
  test_traits();
  test_overload();
  test_constructors();
  test_assignment();
  test_interval_conversion();
  test_explicit_conversion();
  return boost::report_errors();
}
