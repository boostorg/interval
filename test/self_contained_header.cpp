/* Boost test/self_contained_header.cpp
 * test that a public header compiles when included on its own
 *
 * The header is given relative to boost/numeric by the build system in
 * BOOST_NUMERIC_INTERVAL_TEST_HEADER, e.g. interval/compare/certain.hpp.
 *
 * Copyright 2026 Sayan Samanta
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define BOOST_NUMERIC_INTERVAL_TEST_INCLUDE_HEADER() <boost/numeric/BOOST_NUMERIC_INTERVAL_TEST_HEADER>

#include BOOST_NUMERIC_INTERVAL_TEST_INCLUDE_HEADER()

int main() {
  return 0;
}
