/* Boost interval/rounded_arith.hpp template implementation file
 *
 * Copyright 2002-2003 Hervé Brönnimann, Guillaume Melquiond, Sylvain Pion
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_ROUNDED_ARITH_HPP
#define BOOST_NUMERIC_INTERVAL_ROUNDED_ARITH_HPP

#include <boost/numeric/interval/rounding.hpp>
#include <boost/numeric/interval/detail/bugs.hpp>
#include <boost/config/no_tr1/cmath.hpp>
#if defined(_MSC_VER) && defined(_M_IX86) && _M_IX86_FP >= 1
#  include <emmintrin.h>
#endif

namespace boost {
namespace numeric {
namespace interval_lib {

namespace detail {

// Read a floating operand back through a volatile object. The compiler does
// not see a rounding-mode change as a dependency, so it may compute the same
// expression once for both ends of an interval (GCC bug 34678); reading one
// operand afresh after the mode change makes each evaluation depend on a
// value the compiler cannot reuse. Used by the rounded_arith_std primitives
// and the sqrt bodies below.
template<class T> inline T reload_operand(const T& x) { return x; }
inline float reload_operand(const float& x)
{ volatile float v = x; return v; }
inline double reload_operand(const double& x)
{ volatile double v = x; return v; }
inline long double reload_operand(const long double& x)
{ volatile long double v = x; return v; }

// Square root in the current rounding mode.  The sqrt of the 32-bit
// Microsoft runtime rounds to nearest whatever the mode, so use the square
// root instruction directly there.
template<class T> inline T sqrt_rounded(const T& x)
{ BOOST_NUMERIC_INTERVAL_using_math(sqrt); return sqrt(x); }

#if defined(_MSC_VER) && defined(_M_IX86)
// Use the unit the compiler uses for the rest of the arithmetic on the type,
// which /arch selects and _M_IX86_FP reports:
//   /arch:IA32  (0): float and double on x87
//   /arch:SSE   (1): float on SSE, double on x87
//   /arch:SSE2  (2): float and double on SSE2 (the default)
// This avoids moving the value between units, and relies only on the
// rounding mode of the unit the type already uses.
# if _M_IX86_FP >= 1
inline float sqrt_rounded(const float& x)
{ return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x))); }
# else
inline float sqrt_rounded(const float& x)
{
  float v = x;
  __asm
  {
    fld v
    fsqrt
    fstp v
  }
  return v;
}
# endif
# if _M_IX86_FP >= 2
inline double sqrt_rounded(const double& x)
{ __m128d mm_x = _mm_set_sd(x); return _mm_cvtsd_f64(_mm_sqrt_sd(mm_x, mm_x)); }
# else
inline double sqrt_rounded(const double& x)
{
  double v = x;
  __asm
  {
    fld v
    fsqrt
    fstp v
  }
  return v;
}
# endif
inline long double sqrt_rounded(const long double& x)
{ return sqrt_rounded(static_cast<double>(x)); }  // same format on MSVC
#endif

} // namespace detail

/*
 * Three classes of rounding: exact, std, opp
 * See documentation for details.
 */

template<class T, class Rounding>
struct rounded_arith_exact: Rounding {
  void init() { }
  template<class U> T conv_down(U const &v) { return v; }
  template<class U> T conv_up  (U const &v) { return v; }
  T add_down (const T& x, const T& y) { return x + y; }
  T add_up   (const T& x, const T& y) { return x + y; }
  T sub_down (const T& x, const T& y) { return x - y; }
  T sub_up   (const T& x, const T& y) { return x - y; }
  T mul_down (const T& x, const T& y) { return x * y; }
  T mul_up   (const T& x, const T& y) { return x * y; }
  T div_down (const T& x, const T& y) { return x / y; }
  T div_up   (const T& x, const T& y) { return x / y; }
  T median   (const T& x, const T& y) { return (x + y) / 2; }
  T sqrt_down(const T& x)
  { BOOST_NUMERIC_INTERVAL_using_math(sqrt); return sqrt(x); }
  T sqrt_up  (const T& x)
  { BOOST_NUMERIC_INTERVAL_using_math(sqrt); return sqrt(x); }
  T int_down (const T& x)
  { BOOST_NUMERIC_INTERVAL_using_math(floor); return floor(x); }
  T int_up   (const T& x)
  { BOOST_NUMERIC_INTERVAL_using_math(ceil); return ceil(x); }
};

template<class T, class Rounding>
struct rounded_arith_std: Rounding {
# define BOOST_DN(EXPR) this->downward();   return this->force_rounding(EXPR)
# define BOOST_NR(EXPR) this->to_nearest(); return this->force_rounding(EXPR)
# define BOOST_UP(EXPR) this->upward();     return this->force_rounding(EXPR)
  void init() { }
  template<class U> T conv_down(U const &v) { BOOST_DN(detail::reload_operand(v)); }
  template<class U> T conv_up  (U const &v) { BOOST_UP(detail::reload_operand(v)); }
  T add_down(const T& x, const T& y) { BOOST_DN(detail::reload_operand(x) + y); }
  T sub_down(const T& x, const T& y) { BOOST_DN(detail::reload_operand(x) - y); }
  T mul_down(const T& x, const T& y) { BOOST_DN(detail::reload_operand(x) * y); }
  T div_down(const T& x, const T& y) { BOOST_DN(detail::reload_operand(x) / y); }
  T add_up  (const T& x, const T& y) { BOOST_UP(detail::reload_operand(x) + y); }
  T sub_up  (const T& x, const T& y) { BOOST_UP(detail::reload_operand(x) - y); }
  T mul_up  (const T& x, const T& y) { BOOST_UP(detail::reload_operand(x) * y); }
  T div_up  (const T& x, const T& y) { BOOST_UP(detail::reload_operand(x) / y); }
  T median(const T& x, const T& y) { BOOST_NR((x + y) / 2); }
  T sqrt_down(const T& x)
  { BOOST_DN(detail::sqrt_rounded(detail::reload_operand(x))); }
  T sqrt_up  (const T& x)
  { BOOST_UP(detail::sqrt_rounded(detail::reload_operand(x))); }
  T int_down(const T& x) { this->downward(); return this->to_int(x); }
  T int_up  (const T& x) { this->upward();   return this->to_int(x); }
# undef BOOST_DN
# undef BOOST_NR
# undef BOOST_UP
};
  
template<class T, class Rounding>
struct rounded_arith_opp: Rounding {
  void init() { this->upward(); }
# define BOOST_DN(EXPR) \
    this->downward(); \
    T r = this->force_rounding(EXPR); \
    this->upward(); \
    return r
# define BOOST_NR(EXPR) \
    this->to_nearest(); \
    T r = this->force_rounding(EXPR); \
    this->upward(); \
    return r
# define BOOST_UP(EXPR) return this->force_rounding(EXPR)
# define BOOST_UP_NEG(EXPR) return -this->force_rounding(EXPR)
  template<class U> T conv_down(U const &v) { BOOST_UP_NEG(-v); }
  template<class U> T conv_up  (U const &v) { BOOST_UP(v); }
  T add_down(const T& x, const T& y) { BOOST_UP_NEG((-x) - y); }
  T sub_down(const T& x, const T& y) { BOOST_UP_NEG(y - x); }
  T mul_down(const T& x, const T& y) { BOOST_UP_NEG(x * (-y)); }
  T div_down(const T& x, const T& y) { BOOST_UP_NEG(x / (-y)); }
  T add_up  (const T& x, const T& y) { BOOST_UP(x + y); }
  T sub_up  (const T& x, const T& y) { BOOST_UP(x - y); }
  T mul_up  (const T& x, const T& y) { BOOST_UP(x * y); }
  T div_up  (const T& x, const T& y) { BOOST_UP(x / y); }
  T median  (const T& x, const T& y) { BOOST_NR((x + y) / 2); }
  T sqrt_down(const T& x)
  { BOOST_DN(detail::sqrt_rounded(detail::reload_operand(x))); }
  T sqrt_up  (const T& x)
  { BOOST_UP(detail::sqrt_rounded(detail::reload_operand(x))); }
  T int_down(const T& x) { return -this->to_int(-x); }
  T int_up  (const T& x) { return  this->to_int(x); }
# undef BOOST_DN
# undef BOOST_NR
# undef BOOST_UP
# undef BOOST_UP_NEG
};

} // namespace interval_lib
} // namespace numeric
} // namespace boost

#endif // BOOST_NUMERIC_INTERVAL_ROUNDED_ARITH_HPP
