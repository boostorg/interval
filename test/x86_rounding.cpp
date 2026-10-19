/* Boost test/x86_rounding.cpp
 * On x86 the rounding mode lives in two registers: the x87 control word and
 * MXCSR, which governs the SSE unit.  Check that rounding_control sets both
 * (issue #23) and leaves the exception masks and the x87 precision control as
 * it found them (issue #35).
 *
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/numeric/interval.hpp>
#include <boost/core/lightweight_test.hpp>

#if defined(BOOST_NO_FENV_H) && defined(__GNUC__)
// Without <fenv.h>, GCC and clang set the x87 control word only.
#  define BOOST_INTERVAL_TEST_X87_ONLY
#endif

#if !(defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64))

int main() { return 0; } // x86 only

#elif defined(BOOST_INTERVAL_TEST_X87_ONLY) && defined(__SSE_MATH__)

int main() { return 0; } // SSE arithmetic does not follow the x87 mode

#else

#if !defined(__GNUC__)
#  include <float.h>
#  include <xmmintrin.h>
#endif

namespace {

// Rounding control is bits 13-14 of MXCSR and bits 10-11 of the x87 control
// word, with the same codes: 0 nearest, 1 down, 2 up, 3 toward zero.
// MXCSR bit 9 and x87 bit 2 mask the divide-by-zero exception.

#if defined(__GNUC__)
unsigned mxcsr() { unsigned r; __asm__ __volatile__ ("stmxcsr %0" : "=m"(r)); return r; }
void set_mxcsr(unsigned r) { __asm__ __volatile__ ("ldmxcsr %0" : : "m"(r)); }
unsigned x87cw() { unsigned short r; __asm__ __volatile__ ("fnstcw %0" : "=m"(r)); return r; }
void set_x87cw(unsigned r) {
  unsigned short r_ = static_cast<unsigned short>(r);
  __asm__ __volatile__ ("fnclex\n\tfldcw %0" : : "m"(r_));
}
#else
unsigned mxcsr() { return _mm_getcsr(); }
void set_mxcsr(unsigned r) { _mm_setcsr(r); }
#  if defined(_M_IX86)
// The x87 control word in the CRT's encoding, where the rounding codes are
// _RC_NEAR, _RC_DOWN, _RC_UP and _RC_CHOP: 0 to 3 shifted left by 8.
unsigned x87cw() {
  unsigned int x87, sse;
  __control87_2(0, 0, &x87, &sse);
  return x87;
}
#  endif
#endif

// Hides x from the optimizer, so that it cannot reuse a result computed in
// another rounding mode.
template<class T>
T runtime(T x) {
#if defined(__GNUC__)
  __asm__ __volatile__ ("" : "+m"(x));
#endif
  return x;
}

template<class T>
void test() {
  typedef boost::numeric::interval_lib::rounding_control<T> rc;
  void (*const set_mode[4])() =
    { &rc::to_nearest, &rc::downward, &rc::upward, &rc::toward_zero };

  // Unmask divide-by-zero in both units; nothing below divides by zero.
  unsigned const saved_csr = mxcsr();
  set_mxcsr(saved_csr & ~0x23fu);  // also clear the exception flags
  unsigned const csr = mxcsr();
#if defined(__GNUC__)
  unsigned const saved_cw = x87cw();
  set_x87cw(saved_cw & ~0x4u);
  unsigned const cw = x87cw();
#else
  unsigned saved_fp;
  _controlfp_s(&saved_fp, 0, _EM_ZERODIVIDE);
  unsigned const fp = _controlfp(0, 0) & _MCW_EM;
#  if defined(_M_IX86)
  unsigned const cw = x87cw();
#  endif
#endif

  // Each mode reaches both units, and only the rounding bits change.
  for (unsigned m = 0; m < 4; ++m) {
    set_mode[m]();
#if !defined(BOOST_INTERVAL_TEST_X87_ONLY)
    BOOST_TEST_EQ(mxcsr(), (csr & ~0x6000u) | m << 13);
#endif
#if defined(__GNUC__)
    BOOST_TEST_EQ(x87cw(), (cw & ~0xc00u) | m << 10);
#elif defined(_M_IX86)
    BOOST_TEST_EQ(x87cw(), (cw & ~static_cast<unsigned>(_MCW_RC)) | m << 8);
#endif
  }

  // Interval arithmetic switches the mode and restores it.
  rc::to_nearest();
  typedef boost::numeric::interval<T> I;
  I q = I(runtime(T(1))) / I(runtime(T(3)));
  BOOST_TEST(q.lower() < q.upper());
  BOOST_TEST_EQ(mxcsr() & ~0x3fu, csr);
#if defined(__GNUC__)
  BOOST_TEST_EQ(x87cw(), cw);
  set_x87cw(saved_cw);
#else
  BOOST_TEST_EQ(_controlfp(0, 0) & _MCW_EM, fp);
#  if defined(_M_IX86)
  BOOST_TEST_EQ(x87cw(), cw);
#  endif
  _controlfp_s(&saved_fp, _EM_ZERODIVIDE, _EM_ZERODIVIDE);
#endif
  set_mxcsr(saved_csr);

  // to_int follows the mode: 2.5 and -2.5 in each of the four.
  T const pos[4] = { 2, 2, 3, 2 }, neg[4] = { -2, -3, -2, -2 };
  for (unsigned m = 0; m < 4; ++m) {
    set_mode[m]();
    BOOST_TEST_EQ(rc::to_int(runtime(T(2.5))), pos[m]);
    BOOST_TEST_EQ(rc::to_int(runtime(T(-2.5))), neg[m]);
  }
  rc::to_nearest();
}

#if defined(__GNUC__) && defined(__i386__)
// 2^60 + 1.5 is exact in an x87 long double, not in a double.  fmod finds n
// with to_int, which must not go through a double: fmod would return [1.5,1.5]
// and [65.5,65.5], against 0 <= lower < y.  Not checked on x86-64, where
// to_int still goes through rint(double).
void test_fmod_long_double() {
  typedef boost::numeric::interval<long double> I;
  long double const x = 1152921504606846976.0L + 1.5L;
  I z1 = fmod(I(runtime(x)), I(runtime(1.0L)));
  I z3 = fmod(I(runtime(x)), I(runtime(3.0L))); // 2^60 = 1 (mod 3)
  BOOST_TEST(z1.lower() == 0.5L && z1.upper() == 0.5L);
  BOOST_TEST(z3.lower() == 2.5L && z3.upper() == 2.5L);
}
#endif

} // namespace

int main() {
  test<float>();
  test<double>();
  test<long double>();
#if defined(__GNUC__) && defined(__i386__)
  test_fmod_long_double();
#endif
  return boost::report_errors();
}

#endif
