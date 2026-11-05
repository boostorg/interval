/* Boost test/conv_integer.cpp
 * an interval built from an integer encloses it
 *
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// conv_down must not negate an integer (unsigned wraps, the minimum
// overflows) and must not share one conversion with conv_up.

#include <boost/numeric/interval.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/config.hpp>
#include <climits>
#include <cstdio>

using namespace boost::numeric;
using namespace boost::numeric::interval_lib;

template<class T> struct std_policy {
  typedef interval<T, policies<save_state<rounded_arith_std<T> >,
                               checking_base<T> > > type;
};

template<class I, class U>
void check(const char* name, U v, double lo, double hi)
{
  volatile U w = v;  // not folded at build time
  I a(w), b(w, w), c;
  c = w;
  if (!(a.lower() == lo && a.upper() == hi))
    std::printf("%s: [%.17g, %.17g], expected [%.17g, %.17g]\n",
                name, (double)a.lower(), (double)a.upper(), lo, hi);
  BOOST_TEST(a.lower() == lo && a.upper() == hi);
  BOOST_TEST(b.lower() == lo && b.upper() == hi);
  BOOST_TEST(c.lower() == lo && c.upper() == hi);
}

template<class I>
void test_double()
{
  const double two53 = 9007199254740992.0;
  check<I>("5u", 5u, 5.0, 5.0);
  check<I>("UINT_MAX", UINT_MAX, 4294967295.0, 4294967295.0);
  check<I>("7ul", 7ul, 7.0, 7.0);
  check<I>("INT_MIN", INT_MIN, -2147483648.0, -2147483648.0);
  check<I>("INT_MAX", INT_MAX, 2147483647.0, 2147483647.0);
#if defined(BOOST_HAS_LONG_LONG)
  typedef boost::long_long_type ll;
  typedef boost::ulong_long_type ull;
  const ll p53 = static_cast<ll>(1) << 53;
  check<I>("LLONG_MIN", -(p53 * 512) * 2, -two53 * 1024, -two53 * 1024);
  check<I>("2^53+1", p53 + 1, two53, two53 + 2);
  check<I>("-2^53-1", -p53 - 1, -two53 - 2, -two53);
  check<I>("ULLONG_MAX", ~static_cast<ull>(0),
           18446744073709549568.0, 18446744073709551616.0);
#endif
}

template<class I>
void test_float()
{
  check<I>("2^24+1", 16777217, 16777216.0, 16777218.0);
  check<I>("INT_MIN (float)", INT_MIN, -2147483648.0, -2147483648.0);
  check<I>("INT_MAX (float)", INT_MAX, 2147483520.0, 2147483648.0);
  check<I>("UINT_MAX (float)", UINT_MAX, 4294967040.0, 4294967296.0);
}

int main()
{
  test_double<interval<double> >();
  test_double<std_policy<double>::type>();
  test_float<interval<float> >();
  test_float<std_policy<float>::type>();
  return boost::report_errors();
}
