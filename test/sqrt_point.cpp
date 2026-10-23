// Copyright 2026 Sonny Guardian
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
// Regression test: the interval square root of a point interval must enclose
// the true square root. Both ends are checked exactly, in C++03, without fma:
// for a candidate end r, r*r is split exactly into p + e (Dekker product),
// d = p - x is exact (Sterbenz: p is within a factor 2 of x for every input
// here), and the rounded sum d + e has the sign of the exact value r*r - x.
// These checks run in round-to-nearest, which save_state restores after
// each interval operation.
//
// Built by the test suite with -frounding-math (see Jamfile.v2 / CMakeLists.txt).
// Control: add -DCONTROL_EXACT_POLICY to run the same checks on the exact
// (non-rounding) policy. The control must FAIL; if it passes, the test
// cannot see a narrowed interval.
#include <boost/numeric/interval.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cstdio>

using namespace boost::numeric;
using namespace boost::numeric::interval_lib;

#if defined(CONTROL_EXACT_POLICY)
typedef interval<double, policies<save_state<rounded_arith_exact<double> >,
                                  checking_base<double> > > I;
#elif defined(TEST_STD_POLICY)
typedef interval<double, policies<save_state<rounded_arith_std<double> >,
                                  checking_base<double> > > I;
#else
typedef interval<double> I;  // default policy: save_state<rounded_arith_opp<double> >
#endif

// Inputs are read through volatile storage so nothing is folded at build time.
static volatile double corpus[96];
static int corpus_size = 0;

static void fill_corpus() {
  static const double fixed[] = {3.0, 2.0, 5.0, 7.0, 10.0, 0.3, 1.5,
                                 1e10 + 1.0, 123456.789, 0.7, 4.0, 9.0,
                                 2.25, 1.0};
  const int nfixed = sizeof(fixed) / sizeof(fixed[0]);
  for (int i = 0; i < nfixed; ++i) corpus[corpus_size++] = fixed[i];
  // Deterministic values in [1, 4): 1 + 3 * k / 2^30 with k from a 30-bit
  // linear congruential sequence. Every value is exactly representable.
  unsigned long s = 12345UL;
  while (corpus_size < 96) {
    s = (s * 1103515245UL + 12345UL) & 0x3fffffffUL;
    corpus[corpus_size++] = 1.0 + 3.0 * (static_cast<double>(s) / 1073741824.0);
  }
}

// Veltkamp split of a into hi + lo, each with at most 26 significant bits.
static void split(double a, double& hi, double& lo) {
  volatile double c = 134217729.0 * a;  // 2^27 + 1
  volatile double big = c - a;
  hi = c - big;
  lo = a - hi;
}

// Sign of the exact value r*r - x.
static int sign_sq_minus(double r, double x) {
  volatile double p = r * r;
  double rh, rl;
  split(r, rh, rl);
  // Each partial product is exact; the sum gives the exact error e = r*r - p.
  volatile double e = ((rh * rh - p) + 2.0 * (rh * rl)) + rl * rl;
  volatile double d = p - x;  // exact by Sterbenz for the inputs used here
  volatile double s = d + e;  // a correctly rounded sum keeps the exact sign
  return (s > 0) - (s < 0);
}

int main() {
  fill_corpus();
  int misses = 0;
  for (int i = 0; i < corpus_size; ++i) {
    double x = corpus[i];
    I y = sqrt(I(x));
    double lo = y.lower(), hi = y.upper();
    bool lo_ok = sign_sq_minus(lo, x) <= 0;
    bool hi_ok = sign_sq_minus(hi, x) >= 0;
    if (!lo_ok || !hi_ok) {
      ++misses;
      std::printf("miss input=%.17g lower=%.17g upper=%.17g side=%s\n",
                  x, lo, hi, !hi_ok ? "upper" : "lower");
    }
    BOOST_TEST(lo_ok);
    BOOST_TEST(hi_ok);
  }
  std::printf("inputs=%d misses=%d\n", corpus_size, misses);
  return boost::report_errors();
}
