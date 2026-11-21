/* Boost interval/detail/x86gcc_rounding_control.hpp file
 *
 * This header provides a rounding control policy
 * that avoids flushing results to memory. In
 * order for this optimization to be reliable, it
 * should be used only when no underflow or
 * overflow would happen without it. Indeed, only
 * values in range are correctly rounded.
 *
 * Copyright 2002 Hervé Brönnimann, Guillaume Melquiond, Sylvain Pion
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_EXT_X86_FAST_ROUNDING_CONTROL_HPP
#define BOOST_NUMERIC_INTERVAL_EXT_X86_FAST_ROUNDING_CONTROL_HPP

#include <boost/numeric/interval/rounding.hpp>
#include <boost/numeric/interval/detail/x86_rounding_control.hpp>

namespace boost {
namespace numeric {
namespace interval_lib {

namespace detail {

// All exceptions masked; precision and rounding are added below.
static const unsigned short x87_cw_base = x87_ic_affine | x87_reserved | x87_em_all;
// The base | the precision of each type, so results need no rounding to memory.
static const unsigned short x87_cw_f = x87_cw_base | x87_pc_24;
static const unsigned short x87_cw_d = x87_cw_base | x87_pc_53;
static const unsigned short x87_cw_l = x87_cw_base | x87_pc_64;
// One control word per rounding mode: | the rounding field.
static const fpu_rounding_modes rnd_mode_f = {
  x87_cw_f | x87_rc_nearest, x87_cw_f | x87_rc_down, x87_cw_f | x87_rc_up, x87_cw_f | x87_rc_zero };
static const fpu_rounding_modes rnd_mode_d = {
  x87_cw_d | x87_rc_nearest, x87_cw_d | x87_rc_down, x87_cw_d | x87_rc_up, x87_cw_d | x87_rc_zero };
static const fpu_rounding_modes rnd_mode_l = {
  x87_cw_l | x87_rc_nearest, x87_cw_l | x87_rc_down, x87_cw_l | x87_rc_up, x87_cw_l | x87_rc_zero };

} // namespace detail

template<class T>
struct x86_fast_rounding_control;

template<>
struct x86_fast_rounding_control<float>: detail::x86_rounding
{
  static void to_nearest()  { set_rounding_mode(detail::rnd_mode_f.to_nearest);  }
  static void downward()    { set_rounding_mode(detail::rnd_mode_f.downward);    }
  static void upward()      { set_rounding_mode(detail::rnd_mode_f.upward);      }
  static void toward_zero() { set_rounding_mode(detail::rnd_mode_f.toward_zero); }
  static const float& force_rounding(const float& r) { return r; }
};

template<>
struct x86_fast_rounding_control<double>: detail::x86_rounding
{
  static void to_nearest()  { set_rounding_mode(detail::rnd_mode_d.to_nearest);  }
  static void downward()    { set_rounding_mode(detail::rnd_mode_d.downward);    }
  static void upward()      { set_rounding_mode(detail::rnd_mode_d.upward);      }
  static void toward_zero() { set_rounding_mode(detail::rnd_mode_d.toward_zero); }
  static const double& force_rounding(const double& r) { return r; }
};

template<>
struct x86_fast_rounding_control<long double>: detail::x86_rounding
{
  static void to_nearest()  { set_rounding_mode(detail::rnd_mode_l.to_nearest);  }
  static void downward()    { set_rounding_mode(detail::rnd_mode_l.downward);    }
  static void upward()      { set_rounding_mode(detail::rnd_mode_l.upward);      }
  static void toward_zero() { set_rounding_mode(detail::rnd_mode_l.toward_zero); }
  static const long double& force_rounding(const long double& r) { return r; }
};

} // namespace interval_lib
} // namespace numeric
} // namespace boost

#endif // BOOST_NUMERIC_INTERVAL_EXT_X86_FAST_ROUNDING_CONTROL_HPP
