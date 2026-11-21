/* Boost interval/detail/x86_rounding_control.hpp file
 *
 * Copyright 2000 Jens Maurer
 * Copyright 2002 Hervé Brönnimann, Guillaume Melquiond, Sylvain Pion
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_DETAIL_X86_ROUNDING_CONTROL_HPP
#define BOOST_NUMERIC_INTERVAL_DETAIL_X86_ROUNDING_CONTROL_HPP

// Shared by the platform headers below, each of which names its x87 status
// access x86_x87_status and its MXCSR access x86_mxcsr.
namespace boost {
namespace numeric {
namespace interval_lib {
namespace detail {

// For units without x87 flag handling: restoring sets the control word only.
template<class B> struct x87_status_none
{
  static unsigned short x87_status_get() { return 0; }
  static void x87_restore(const typename B::rounding_mode mode, unsigned short)
  { B::set_rounding_mode(mode); }
};

// For targets without SSE arithmetic.
struct mxcsr_none
{
  static unsigned sse_get() { return 0; }
  static void sse_set(unsigned) {}
};

} // namespace detail
} // namespace interval_lib
} // namespace numeric
} // namespace boost

#if defined(__GNUC__) || defined(__BORLANDC__) && defined(__clang__)
#  include <boost/numeric/interval/detail/x86gcc_rounding_control.hpp>
#elif defined(__BORLANDC__)
#  include <boost/numeric/interval/detail/bcc_rounding_control.hpp>
#elif defined(_MSC_VER)
#  include <boost/numeric/interval/detail/msvc_rounding_control.hpp>
#elif defined(__MWERKS__) || defined(__ICC) || defined (__SUNPRO_CC)
#  define BOOST_NUMERIC_INTERVAL_USE_C99_SUBSYSTEM
#  include <boost/numeric/interval/detail/c99sub_rounding_control.hpp>
#else
#  error Unsupported C++ compiler.
#endif

namespace boost {
namespace numeric {
namespace interval_lib {

namespace detail {

#ifdef BOOST_NUMERIC_INTERVAL_USE_C99_SUBSYSTEM
typedef c99_rounding_control x86_rounding_control;
#undef BOOST_NUMERIC_INTERVAL_USE_C99_SUBSYSTEM
#else
struct fpu_rounding_modes
{
  unsigned short to_nearest;
  unsigned short downward;
  unsigned short upward;
  unsigned short toward_zero;
};

// exceptions masked, extended precision
// hardware default is 0x037f (0x1000 only has a meaning on 287)
static const fpu_rounding_modes rnd_mode = { 0x137f, 0x177f, 0x1b7f, 0x1f7f };

// Sets the x87 unit and MXCSR: every exception masked, FTZ/DAZ off.  Restoring
// puts back the caller's state exactly, flags included: flags raised inside
// are dropped.  MXCSR is written last, since _control87 changes it.
struct x86_rounding_control: x86_rounding, x86_x87_status, x86_mxcsr
{
  struct rounding_mode
  {
    x86_rounding::rounding_mode x87;
    unsigned short x87_status;
    unsigned sse;
  };

  static void get_rounding_mode(rounding_mode& mode)
  {
    x86_rounding::get_rounding_mode(mode.x87);
    mode.x87_status = x87_status_get();
    mode.sse = sse_get();
  }

  static void set_rounding_mode(const rounding_mode& mode)
  {
    x87_restore(mode.x87, mode.x87_status);
    sse_set(mode.sse);
  }

  static void set(x86_rounding::rounding_mode x87, unsigned rc)
  {
    x86_rounding::set_rounding_mode(x87);
    sse_set(0x1f80 | rc << 13);
  }

  static void to_nearest()  { set(rnd_mode.to_nearest, 0);  }
  static void downward()    { set(rnd_mode.downward, 1);    }
  static void upward()      { set(rnd_mode.upward, 2);      }
  static void toward_zero() { set(rnd_mode.toward_zero, 3); }
};
#endif // BOOST_NUMERIC_INTERVAL_USE_C99_SUBSYSTEM

} // namespace detail

template<>
struct rounding_control<float>: detail::x86_rounding_control
{
  static float force_rounding(const float& r) 
  { volatile float r_ = r; return r_; }
};

template<>
struct rounding_control<double>: detail::x86_rounding_control
{
  /*static double force_rounding(double r) 
  { asm volatile ("" : "+m"(r) : ); return r; }*/
  static double force_rounding(const double& r) 
  { volatile double r_ = r; return r_; }
};

namespace detail {

template<bool>
struct x86_rounding_control_long_double;

template<>
struct x86_rounding_control_long_double<false>: x86_rounding_control
{
  static long double force_rounding(long double const &r)
  { volatile long double r_ = r; return r_; }
};

// An x87 long double needs no rounding to memory, but the result must still be
// computed before the next change of rounding mode.  The empty asm takes the
// value as an operand, which makes the computation a dependency of it, and the
// "memory" clobber keeps it before the mode change.
template<>
struct x86_rounding_control_long_double<true>: x86_rounding_control
{
#if defined(__GNUC__)
  static long double force_rounding(long double r)
  { __asm__ __volatile__ ("" : "+t"(r) : : "memory"); return r; }
#else
  static long double const &force_rounding(long double const &r)
  { return r; }
#endif
};

} // namespace detail

template<>
struct rounding_control<long double>:
  detail::x86_rounding_control_long_double< (sizeof(long double) >= 10) >
{};

} // namespace interval_lib
} // namespace numeric
} // namespace boost

#undef BOOST_NUMERIC_INTERVAL_NO_HARDWARE

#endif /* BOOST_NUMERIC_INTERVAL_DETAIL_X86_ROUNDING_CONTROL_HPP */
