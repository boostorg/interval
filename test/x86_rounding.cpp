/* Boost test/x86_rounding.cpp
 * x86 rounding control sets everything in the unit that computes T: the
 * rounding mode, all exceptions masked, FTZ/DAZ off and, on x87, extended
 * precision (#23, #35).  The caller's state, flags included, comes back
 * exactly on exit: flags raised inside are dropped.
 *
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/numeric/interval.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/limits.hpp>

#if !(defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64))

int main() { return 0; } // x86 only

#else

#include <csignal>
#include <cstdio>
#include <cstdlib>
#if !defined(__GNUC__) && !defined(__clang__)
#  include <xmmintrin.h>
#endif

#if defined(__SSE_MATH__) || defined(_M_X64) || defined(_M_IX86_FP) && _M_IX86_FP >= 1
#  define BOOST_INTERVAL_TEST_FLOAT_SSE true
#else
#  define BOOST_INTERVAL_TEST_FLOAT_SSE false
#endif
#if defined(__SSE2_MATH__) || defined(_M_X64) || defined(_M_IX86_FP) && _M_IX86_FP >= 2
#  define BOOST_INTERVAL_TEST_DOUBLE_SSE true
#else
#  define BOOST_INTERVAL_TEST_DOUBLE_SSE false
#endif

namespace {

// MXCSR: flags 0-5, DAZ 6, masks 7-12, rounding 13-14, FTZ 15.
// x87: control word masks 0-5, precision 8-9, rounding 10-11; flags in the
// status word, bits 0-5.  Both units: 0x01 invalid, 0x20 inexact.

#if defined(__GNUC__) || defined(__clang__)
unsigned csr() { unsigned r; __asm__ __volatile__ ("stmxcsr %0" : "=m"(r)); return r; }
void set_csr(unsigned v) { __asm__ __volatile__ ("ldmxcsr %0" : : "m"(v) : "memory"); }
unsigned x87cw() { unsigned short r; __asm__ __volatile__ ("fnstcw %0" : "=m"(r)); return r; }
unsigned x87sw() { unsigned short r; __asm__ __volatile__ ("fnstsw %0" : "=m"(r)); return r; }
void set_x87cw(unsigned v) {
  unsigned short r = static_cast<unsigned short>(v);
  __asm__ __volatile__ ("fldcw %0" : : "m"(r) : "memory");
}
void clear_x87() { __asm__ __volatile__ ("fnclex" : : : "memory"); }
#  define BOOST_INTERVAL_TEST_X87
#else
#  if defined(_M_IX86)
// Inline asm: clang-cl rejects the intrinsics under /arch:IA32.
unsigned csr() { unsigned r; __asm { stmxcsr r } return r; }
void set_csr(unsigned v) { __asm { ldmxcsr v } }
#  else
unsigned csr() { return _mm_getcsr(); }
void set_csr(unsigned v) { _mm_setcsr(v); }
#  endif
#  if defined(_M_IX86)
unsigned x87cw() { unsigned short r; __asm { fnstcw r } return r; }
unsigned x87sw() { unsigned short r; __asm { fnstsw r } return r; }
void set_x87cw(unsigned v) {
  unsigned short r = static_cast<unsigned short>(v);
  __asm { fldcw r }
}
void clear_x87() { __asm { fnclex } }
#    define BOOST_INTERVAL_TEST_X87
#  endif
#endif

#if !defined(BOOST_INTERVAL_TEST_X87)
unsigned x87cw() { return 0; }
unsigned x87sw() { return 0; }
void set_x87cw(unsigned) {}
void clear_x87() {}
#endif

// Which unit computes T.
template<class T> struct on_sse;
template<> struct on_sse<float> { static const bool value = BOOST_INTERVAL_TEST_FLOAT_SSE; };
template<> struct on_sse<double> { static const bool value = BOOST_INTERVAL_TEST_DOUBLE_SSE; };
template<> struct on_sse<long double>
{ static const bool value = sizeof(long double) == sizeof(double) && BOOST_INTERVAL_TEST_DOUBLE_SSE; };

unsigned const csr_default = 0x1f80, cw_default = 0x037f;
// The caller's state: divide-by-zero unmasked, FTZ/DAZ on, 53-bit x87.
unsigned const csr_user = 0x9dc0, cw_user = 0x027b;

template<class T> unsigned control() { return on_sse<T>::value ? csr() & ~0x3fu : x87cw() & 0xf3f; }
template<class T> unsigned flags() { return (on_sse<T>::value ? csr() : x87sw()) & 0x3f; }
template<class T> unsigned user_control(unsigned unmask_pe)
{ return on_sse<T>::value ? csr_user & ~(unmask_pe << 7) : cw_user & ~unmask_pe & 0xf3f; }
// Inside: mode m, everything masked, FTZ/DAZ off, x87 extended precision.
template<class T> unsigned inside_control(unsigned m)
{ return on_sse<T>::value ? 0x1f80 | m << 13 : 0x33f | m << 10; }

void set_default() { set_csr(csr_default); clear_x87(); set_x87cw(cw_default); }

// Hides x from the optimizer.
template<class T>
T runtime(T x) {
#if defined(__GNUC__) || defined(__clang__)
  __asm__ __volatile__ ("" : "+m"(x));
#endif
  return x;
}

// Installs the caller's state, with the invalid flag raised by T's unit
// beforehand.  unmask_pe = 0x20 also unmasks inexact.
template<class T>
void enter_user(unsigned unmask_pe) {
  set_default();
  volatile T z = runtime(T(0));
  volatile T nan = z / z;
  (void)nan;
  set_csr((csr() & 0x3f) | (csr_user & ~(unmask_pe << 7)));
  set_x87cw(cw_user & ~unmask_pe);
}

template<class T>
void test_state() {
  typedef boost::numeric::interval_lib::rounding_control<T> rc;
  void (*const set_mode[4])() =
    { &rc::to_nearest, &rc::downward, &rc::upward, &rc::toward_zero };

  for (unsigned m = 0; m < 4; ++m) {
    enter_user<T>(0);
    typename rc::rounding_mode saved;
    rc::get_rounding_mode(saved);
    set_mode[m]();
    unsigned const in_ctl = control<T>(), in_flags = flags<T>();
    rc::set_rounding_mode(saved);
    unsigned const out_ctl = control<T>(), out_flags = flags<T>();
    set_default();
    BOOST_TEST_EQ(in_ctl, inside_control<T>(m));
    // Switching writes MXCSR whole, flags cleared; the x87 flags stay.
    BOOST_TEST_EQ(in_flags, on_sse<T>::value ? 0u : 0x01u);
    BOOST_TEST_EQ(out_ctl, user_control<T>(0));
    BOOST_TEST_EQ(out_flags, 0x01u);
  }

  // Interval arithmetic: inexact raised inside is dropped, the caller's
  // invalid flag stays.
  typedef boost::numeric::interval<T> I;
  enter_user<T>(0);
  I q = I(runtime(T(1))) / I(runtime(T(3)));
  unsigned const ctl = control<T>(), fl = flags<T>();
  set_default();
  BOOST_TEST(q.lower() < q.upper());
  BOOST_TEST_EQ(ctl, user_control<T>(0));
  BOOST_TEST_EQ(fl, 0x01u);

  // The same from a caller with no flags raised.
  set_default();
  I r = I(runtime(T(1))) / I(runtime(T(3)));
  unsigned const fl0 = flags<T>();
  set_default();
  BOOST_TEST(r.lower() < r.upper());
  BOOST_TEST_EQ(fl0, 0u);
}

// FTZ/DAZ in the caller's state must not reach interval arithmetic.
template<class T>
void test_subnormal() {
  typedef boost::numeric::interval<T> I;
  set_default();
  T const min = runtime((std::numeric_limits<T>::min)());
  T const sub = runtime(min / 4);  // exact, a subnormal
  enter_user<T>(0);
  I a = I(min) / I(runtime(T(4)));
  I b = I(sub) * I(runtime(T(4)));
  I c = I(runtime(T(1))) + I(sub);
  set_default();
  BOOST_TEST(a.lower() <= sub && sub <= a.upper());
  BOOST_TEST(b.lower() <= min && min <= b.upper());
  BOOST_TEST(c.upper() > 1);
}

// to_int rounds in the mode just set, whichever unit it runs on.
template<class T>
void test_to_int() {
  typedef boost::numeric::interval_lib::rounding_control<T> rc;
  void (*const set_mode[4])() =
    { &rc::to_nearest, &rc::downward, &rc::upward, &rc::toward_zero };
  T const pos[4] = { 2, 2, 3, 2 }, neg[4] = { -2, -3, -2, -2 };
  for (unsigned m = 0; m < 4; ++m) {
    set_default();
    set_mode[m]();
    T const p = rc::to_int(runtime(T(2.5))), n = rc::to_int(runtime(T(-2.5)));
    set_default();
    BOOST_TEST_EQ(p, pos[m]);
    BOOST_TEST_EQ(n, neg[m]);
  }
}

char const* current = "";
extern "C" void on_sigfpe(int) {
  std::fprintf(stderr, "SIGFPE in test_unmasked<%s>: inexact trapped\n", current);
  std::abort();
}

// Inexact unmasked by the caller: no trap inside, none afterwards in the
// caller's code, and the inexact flag is not handed back.  Runs last, since
// a failure aborts.
template<class T>
void test_unmasked(char const* name) {
  typedef boost::numeric::interval<T> I;
  current = name;
  enter_user<T>(0x20);
  I q = I(runtime(T(1))) / I(runtime(T(3)));
  volatile T two = runtime(T(2));
  volatile T four = two + two;  // exact; traps on x87 if inexact is pending
  (void)four;
  unsigned const ctl = control<T>(), fl = flags<T>();
  set_default();
  BOOST_TEST(q.lower() < q.upper());
  BOOST_TEST_EQ(ctl, user_control<T>(0x20));
  BOOST_TEST_EQ(fl, 0x01u);
}

} // namespace

int main() {
  test_state<float>();
  test_state<double>();
  test_state<long double>();
  test_subnormal<float>();
  test_subnormal<double>();
  test_subnormal<long double>();
  test_to_int<float>();
  test_to_int<double>();
  test_to_int<long double>();
  std::signal(SIGFPE, on_sigfpe);
  test_unmasked<float>("float");
  test_unmasked<double>("double");
  test_unmasked<long double>("long double");
  return boost::report_errors();
}

#endif
