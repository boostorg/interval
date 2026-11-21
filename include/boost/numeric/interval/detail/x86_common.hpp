/* Boost interval/detail/x86_common.hpp file
 *
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_DETAIL_X86_COMMON_HPP
#define BOOST_NUMERIC_INTERVAL_DETAIL_X86_COMMON_HPP

namespace boost {
namespace numeric {
namespace interval_lib {
namespace detail {

// x87 control word fields (Intel SDM vol. 1, 8.1.5).  Each field has its own
// bits, so a control word is the | of one value per field: for example
// x87_pc_64 | x87_rc_up selects extended precision and rounding upward.

// Exception masks, bits 0-5: a set bit means the exception does not trap
// and gives the default result (NaN, infinity, the rounded value) instead.
static const unsigned short x87_em_invalid    = 0x0001; // e.g. 0/0, sqrt(-1)
static const unsigned short x87_em_denormal   = 0x0002; // a subnormal operand
static const unsigned short x87_em_zerodivide = 0x0004; // x/0 with x finite, nonzero
static const unsigned short x87_em_overflow   = 0x0008; // result too large
static const unsigned short x87_em_underflow  = 0x0010; // result too small to be normal
static const unsigned short x87_em_inexact    = 0x0020; // result had to be rounded
static const unsigned short x87_em_all        = 0x003f; // all six: nothing traps
static const unsigned short x87_reserved      = 0x0040; // bit 6, reads as 1

// Precision control, bits 8-9: the significand width results are rounded to.
static const unsigned short x87_pc_mask = 0x0300;
static const unsigned short x87_pc_24   = 0x0000; // float
static const unsigned short x87_pc_53   = 0x0200; // double
static const unsigned short x87_pc_64   = 0x0300; // long double (extended)

// Rounding control, bits 10-11: the direction results are rounded in.
static const unsigned short x87_rc_mask    = 0x0c00;
static const unsigned short x87_rc_nearest = 0x0000; // to nearest, ties to even
static const unsigned short x87_rc_down    = 0x0400; // toward -infinity
static const unsigned short x87_rc_up      = 0x0800; // toward +infinity
static const unsigned short x87_rc_zero    = 0x0c00; // toward zero (truncate)

// Infinity control, bit 12: affine (+inf and -inf distinct).  Only the 287
// uses it; later x87 units ignore it.
static const unsigned short x87_ic_affine  = 0x1000;
// Every bit of the control word, as a _control87 mask: write all of them.
static const unsigned short x87_cw_all     = 0xffff;

} // namespace detail
} // namespace interval_lib
} // namespace numeric
} // namespace boost

#endif /* BOOST_NUMERIC_INTERVAL_DETAIL_X86_COMMON_HPP */
