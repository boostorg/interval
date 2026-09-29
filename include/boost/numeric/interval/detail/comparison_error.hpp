/* Boost interval/detail/comparison_error.hpp file
 *
 * Copyright 2002-2003 Hervé Brönnimann, Guillaume Melquiond, Sylvain Pion
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_DETAIL_COMPARISON_ERROR_HPP
#define BOOST_NUMERIC_INTERVAL_DETAIL_COMPARISON_ERROR_HPP

#include <boost/config.hpp>
#include <stdexcept>

namespace boost {
namespace numeric {
namespace interval_lib {

class BOOST_SYMBOL_VISIBLE comparison_error
  : public std::runtime_error
{
public:
  comparison_error()
    : std::runtime_error("boost::interval: uncertain comparison")
  { }
};

} // namespace interval_lib
} // namespace numeric
} // namespace boost

#endif // BOOST_NUMERIC_INTERVAL_DETAIL_COMPARISON_ERROR_HPP
