/* Boost interval/detail/msvc_rounding_control.hpp file
 *
 * Copyright 2000 Maarten Keijzer
 * Copyright 2002 Hervé Brönnimann, Guillaume Melquiond, Sylvain Pion
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_DETAIL_MSVC_ROUNDING_CONTROL_HPP
#define BOOST_NUMERIC_INTERVAL_DETAIL_MSVC_ROUNDING_CONTROL_HPP

#ifndef _MSC_VER
#  error This header is only intended for MSVC, but might work for Borland as well
#endif

#include <float.h>      // MSVC rounding control
#include <boost/numeric/interval/detail/x86_common.hpp>

// Although the function is called _control87, it seems to work for
// other FPUs too, so it does not have to be changed to _controlfp.

namespace boost {
namespace numeric {
namespace interval_lib {
namespace detail {

#if BOOST_MSVC < 1400 || defined(_WIN64)
extern "C" { double rint(double); }
#else
inline double rint(double x)
{
_asm FLD [x] ;
_asm FRNDINT ;
//_asm RET ;
}
#endif

struct x86_rounding
{
  static unsigned int hard2msvc(unsigned short m) {
    unsigned int n = 0;
    if (m & x87_em_invalid)    n |= _EM_INVALID;
    if (m & x87_em_denormal)   n |= _EM_DENORMAL;
    if (m & x87_em_zerodivide) n |= _EM_ZERODIVIDE;
    if (m & x87_em_overflow)   n |= _EM_OVERFLOW;
    if (m & x87_em_underflow)  n |= _EM_UNDERFLOW;
    if (m & x87_em_inexact)    n |= _EM_INEXACT;
    switch (m & x87_pc_mask) {
    case x87_pc_24: n |= _PC_24; break;
    case x87_pc_53: n |= _PC_53; break;
    case x87_pc_64: n |= _PC_64; break;
    }
    switch (m & x87_rc_mask) {
    case x87_rc_nearest: n |= _RC_NEAR; break;
    case x87_rc_down:    n |= _RC_DOWN; break;
    case x87_rc_up:      n |= _RC_UP;   break;
    case x87_rc_zero:    n |= _RC_CHOP; break;
    }
    if (m & x87_ic_affine) n |= _IC_AFFINE; // only useful on 287
    return n;
  }

  static unsigned short msvc2hard(unsigned int n) {
    unsigned short m = 0;
    if (n & _EM_INVALID)    m |= x87_em_invalid;
    if (n & _EM_DENORMAL)   m |= x87_em_denormal;
    if (n & _EM_ZERODIVIDE) m |= x87_em_zerodivide;
    if (n & _EM_OVERFLOW)   m |= x87_em_overflow;
    if (n & _EM_UNDERFLOW)  m |= x87_em_underflow;
    if (n & _EM_INEXACT)    m |= x87_em_inexact;
    switch (n & _MCW_RC) {
    case _RC_NEAR: m |= x87_rc_nearest; break;
    case _RC_DOWN: m |= x87_rc_down;    break;
    case _RC_UP:   m |= x87_rc_up;      break;
    case _RC_CHOP: m |= x87_rc_zero;    break;
    }
    switch (n & _MCW_PC) {
    case _PC_24: m |= x87_pc_24; break;
    case _PC_53: m |= x87_pc_53; break;
    case _PC_64: m |= x87_pc_64; break;
    }
    if ((n & _MCW_IC) == _IC_AFFINE) m |= x87_ic_affine;
    return m;
  }

  typedef unsigned short rounding_mode;
  static void get_rounding_mode(rounding_mode& mode)
  { mode = msvc2hard(_control87(0, 0)); }
  static void set_rounding_mode(const rounding_mode mode)
  {
    _control87(hard2msvc(mode),
      _MCW_EM | _MCW_RC
#if !defined(_M_AMD64) && !defined(_M_ARM) && !defined(_M_ARM64)
      // x64 ignores _MCW_PC and _MCW_IC, and the Debug CRT library actually
      // asserts when these are passed to _control87.
      // MSDN says on '_control87' that changing precision (_MCW_PC) or
      // infinity (_MCW_IC) handling is not supported on the ARM and x64
      // architectures and that _control87 raises an assertion
      // and the invalid parameter handler is invoked.
      | _MCW_PC | _MCW_IC
#endif
    );
  }
  static double to_int(const double& x) { return rint(x); }
};

} // namespace detail
} // namespace interval_lib
} // namespace numeric
} // namespace boost

#endif /* BOOST_NUMERIC_INTERVAL_DETAIL_MSVC_ROUNDING_CONTROL_HPP */
