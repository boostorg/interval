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
  volatile U wv = v;  // not folded at build time
  U w = wv;           // plain copy: lets g++ merge conversions if it can
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

#if defined(BOOST_HAS_LONG_LONG)
// User-defined integers shaped like Boost.Multiprecision: two words with a
// user copy, so not held in a register; smint is unbounded, so min() ==
// max() == 0; umint has no unary minus.
typedef boost::long_long_type ll;
typedef boost::ulong_long_type ull;
struct smint {
  ll val, unused;
  explicit smint(ll v): val(v), unused(0) {}
  smint(const smint& o): val(o.val), unused(0) {}
  smint operator-() const { return smint(-val); }
  operator double() const { return static_cast<double>(val); }
};
inline bool operator==(const smint& a, const smint& b)
{ return a.val == b.val; }
inline bool operator<(const smint& a, const smint& b)
{ return a.val < b.val; }
struct umint {
  ull mag, unused;
  explicit umint(ull m): mag(m), unused(0) {}
  umint(const umint& o): mag(o.mag), unused(0) {}
  operator double() const { return static_cast<double>(mag); }
};

namespace std {
template<> struct numeric_limits<smint>: numeric_limits<int> {
  BOOST_STATIC_CONSTANT(bool, is_bounded = false);
  BOOST_STATIC_CONSTANT(int, digits = INT_MAX);
  static smint (min)() { return smint(0); }
  static smint (max)() { return smint(0); }
};
template<> struct numeric_limits<umint>: numeric_limits<ull> {
  static umint (min)() { return umint(0); }
  static umint (max)() { return umint(~static_cast<ull>(0)); }
};
}

template<class I, class U>
void check_udt(const char* name, U v, double lo, double hi)
{
  I a(v), b(v, v), c;
  c = v;
  if (!(a.lower() == lo && a.upper() == hi))
    std::printf("%s: [%.17g, %.17g], expected [%.17g, %.17g]\n",
                name, (double)a.lower(), (double)a.upper(), lo, hi);
  BOOST_TEST(a.lower() == lo && a.upper() == hi);
  BOOST_TEST(b.lower() == lo && b.upper() == hi);
  BOOST_TEST(c.lower() == lo && c.upper() == hi);
}

template<class I>
void test_udt()
{
  const double two53 = 9007199254740992.0;
  volatile ll p53 = static_cast<ll>(1) << 53;  // not folded at build time
  check_udt<I>("smint 0", smint(0), 0.0, 0.0);
  check_udt<I>("smint 2^53+1", smint(p53 + 1), two53, two53 + 2);
  check_udt<I>("smint -2^53-1", smint(-p53 - 1), -two53 - 2, -two53);
  check_udt<I>("umint 0", umint(0), 0.0, 0.0);
  check_udt<I>("umint 2^53+1", umint(static_cast<ull>(p53) + 1),
               two53, two53 + 2);
}
#endif

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
#if defined(BOOST_HAS_LONG_LONG)
  test_udt<interval<double> >();
  test_udt<std_policy<double>::type>();
#endif
  return boost::report_errors();
}
