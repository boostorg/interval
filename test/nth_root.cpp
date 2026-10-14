/* Boost test/nth_root.cpp
 * test the nth_root function
 *
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/numeric/interval.hpp>
#include <boost/core/lightweight_test.hpp>

// nth_root must leave the rounding mode as it found it. 1/3 is not
// representable in a binary format, so one of 1/3 and -1/3 changes whenever
// the mode is no longer to nearest.
template<class T>
void test_rounding_mode(T l, T u, int k) {
  typedef boost::numeric::interval<T> I;
  boost::numeric::interval_lib::rounding_control<T>::to_nearest();
  // volatile, so that the quotients are computed at run time and rounded to T
  volatile T one = 1, three = 3;
  volatile T pos1 = one / three, neg1 = -one / three;
  I r = boost::numeric::nth_root(I(l, u), k);
  volatile T pos2 = one / three, neg2 = -one / three;
  BOOST_TEST(pos1 == pos2);
  BOOST_TEST(neg1 == neg2);
  (void)r;
}

// The square of a float is exact in double, so the bounds of a square root
// can be checked without any rounding error. Each call starts in rounding
// to nearest, which is the mode of a caller that is not using intervals.
void test_enclosure() {
  typedef boost::numeric::interval<float> I;
  int below = 0, above = 0;
  for (unsigned long i = 1; i <= 100000; ++i) {
    // 24 pseudo-random bits, so that x is in (0,1) and exact
    unsigned long m = ((i * 2654435761UL) & 0xffffffffUL) >> 8;
    if (m == 0) continue;
    float x = static_cast<float>(m) / 16777216.0f;
    boost::numeric::interval_lib::rounding_control<float>::to_nearest();
    I r = boost::numeric::nth_root(I(x), 2);
    double l = r.lower(), u = r.upper();
    if (l * l > x) ++above;
    if (u * u < x) ++below;
  }
  BOOST_TEST_EQ(above, 0);
  BOOST_TEST_EQ(below, 0);
}

template<class T>
void test_values() {
  typedef boost::numeric::interval<T> I;
  using boost::numeric::nth_root;
  // { t^(1/k) : t in x } must be a subset of nth_root(x, k), which may be
  // wider, e.g. [4^(1/2), 9^(1/2)] = [2, 3] for x = [4, 9] and k = 2.
  BOOST_TEST(subset(I(2, 3), nth_root(I(4, 9), 2)));
  BOOST_TEST(subset(I(2, 3), nth_root(I(8, 27), 3)));
  BOOST_TEST(subset(I(-2, 3), nth_root(I(-8, 27), 3)));
  BOOST_TEST(subset(I(-3, -2), nth_root(I(-27, -8), 3)));
  BOOST_TEST(subset(I(0, 3), nth_root(I(-4, 9), 2)));
  BOOST_TEST(nth_root(I(-4, 9), 2).lower() == 0);
}

template<class T>
void test() {
  test_rounding_mode<T>(0.5, 0.5, 2);
  test_rounding_mode<T>(2, 3, 2);
  test_rounding_mode<T>(0.25, 2, 5);
  test_rounding_mode<T>(-27, -8, 3);
  test_rounding_mode<T>(-8, 27, 3);
  test_values<T>();
}

int main() {
  test<float>();
  test<double>();
  test_enclosure();
  return boost::report_errors();
}
