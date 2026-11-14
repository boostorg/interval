/* Boost interval/detail/x86gcc_rounding_control.hpp file
 *
 * Copyright 2000 Jens Maurer
 * Copyright 2002 Hervé Brönnimann, Guillaume Melquiond, Sylvain Pion
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_DETAIL_X86GCC_ROUNDING_CONTROL_HPP
#define BOOST_NUMERIC_INTERVAL_DETAIL_X86GCC_ROUNDING_CONTROL_HPP

#if !defined(__GNUC__) && !(defined(__BORLANDC__) && defined(__clang__))
#  error This header only works with GNU CC.
#endif

#if !defined(__i386__) && !defined(__x86_64__)
#  error This header only works on x86 CPUs.
#endif

namespace boost {
namespace numeric {
namespace interval_lib {
namespace detail {

struct x86_rounding
{
  typedef unsigned short rounding_mode;

  // The "memory" clobber keeps GCC from moving x87 arithmetic across the
  // change of rounding mode (issue #41).
  static void set_rounding_mode(const rounding_mode& mode)
  { __asm__ __volatile__ ("fldcw %0" : : "m"(mode) : "memory"); }

  static void get_rounding_mode(rounding_mode& mode)
  { __asm__ __volatile__ ("fnstcw %0" : "=m"(mode)); }

  template<class T>
  static T to_int(T r)
  {
    T r_;
    __asm__ __volatile__ ("frndint" : "=&t"(r_) : "0"(r));
    return r_;
  }
};

struct x87_status_asm
{
  // Status bits 0-7: the flags, stack fault and error summary.
  static unsigned short x87_status_get()
  {
    unsigned short s;
    __asm__ __volatile__ ("fnstsw %0" : "=m"(s) : : "memory");
    return s & 0xff;
  }

  // Loads the control word and status bits 0-7 together, so that no flag
  // raised inside is pending while mode unmasks it (it would trap).
  static void x87_restore(const x86_rounding::rounding_mode& mode, unsigned short s)
  {
    struct { unsigned short w[14]; } env;  // GCC < 4.8 rejects an array "m"
    __asm__ __volatile__ ("fnstenv %0" : "=m"(env));
    env.w[0] = mode;
    env.w[2] = (env.w[2] & ~0x80ff) | s;  // and clear the busy bit
    __asm__ __volatile__ ("fldenv %0" : : "m"(env) : "memory");
  }
};
typedef x87_status_asm x86_x87_status;

struct mxcsr_asm
{
  static unsigned sse_get()
  {
    unsigned r;
    __asm__ __volatile__ ("stmxcsr %0" : "=m"(r));
    return r;
  }

  static void sse_set(unsigned r)
  { __asm__ __volatile__ ("ldmxcsr %0" : : "m"(r) : "memory"); }
};
typedef mxcsr_asm x86_mxcsr;

} // namespace detail
} // namespace interval_lib
} // namespace numeric
} // namespace boost

#endif /* BOOST_NUMERIC_INTERVAL_DETAIL_X86GCC_ROUNDING_CONTROL_HPP */
