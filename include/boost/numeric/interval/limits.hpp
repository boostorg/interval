/* Boost interval/limits.hpp template implementation file
 *
 * Copyright 2000 Jens Maurer
 * Copyright 2002-2003 Hervé Brönnimann, Guillaume Melquiond, Sylvain Pion
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_LIMITS_HPP
#define BOOST_NUMERIC_INTERVAL_LIMITS_HPP


#include <boost/config.hpp>
#include <boost/limits.hpp>
#include <boost/numeric/interval/detail/interval_prototype.hpp>

namespace std {

template<class T, class Policies>
class numeric_limits<boost::numeric::interval<T, Policies> >
  : public numeric_limits<T>
{
private:
  typedef boost::numeric::interval<T, Policies> I;
  typedef numeric_limits<T> bl;
public:
  static BOOST_GPU_ENABLED I min BOOST_PREVENT_MACRO_SUBSTITUTION () BOOST_NOEXCEPT_OR_NOTHROW { return I((bl::min)(), (bl::min)()); }
  static BOOST_GPU_ENABLED I max BOOST_PREVENT_MACRO_SUBSTITUTION () BOOST_NOEXCEPT_OR_NOTHROW { return I((bl::max)(), (bl::max)()); }
  static BOOST_GPU_ENABLED I epsilon() BOOST_NOEXCEPT_OR_NOTHROW { return I(bl::epsilon(), bl::epsilon()); }

  BOOST_STATIC_CONSTANT(float_round_style, round_style = round_indeterminate);
  BOOST_STATIC_CONSTANT(bool, is_iec559 = false);

  static BOOST_GPU_ENABLED I infinity () BOOST_NOEXCEPT_OR_NOTHROW { return I::whole(); }
  static BOOST_GPU_ENABLED I quiet_NaN() BOOST_NOEXCEPT_OR_NOTHROW { return I::empty(); }
  static BOOST_GPU_ENABLED I signaling_NaN() BOOST_NOEXCEPT_OR_NOTHROW
  { return I(bl::signaling_NaN(), bl::signaling_Nan()); }
  static BOOST_GPU_ENABLED I denorm_min() BOOST_NOEXCEPT_OR_NOTHROW
  { return I(bl::denorm_min(), bl::denorm_min()); }
private:
  static I round_error();    // hide this on purpose, not yet implemented
};

} // namespace std


#endif // BOOST_NUMERIC_INTERVAL_LIMITS_HPP
