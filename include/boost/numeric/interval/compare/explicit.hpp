/* Boost interval/compare/explicit.hpp template implementation file
 *
 * Copyright 2000 Jens Maurer
 * Copyright 2002 Hervé Brönnimann, Guillaume Melquiond, Sylvain Pion
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_COMPARE_EXPLICIT_HPP
#define BOOST_NUMERIC_INTERVAL_COMPARE_EXPLICIT_HPP

#include <boost/numeric/interval/detail/interval_prototype.hpp>

namespace boost {
namespace numeric {
namespace interval_lib {

/*
 * Certainly... operations
 */

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool cerlt(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.upper() < y.lower();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cerlt(const interval<T, Policies>& x, const T& y)
{
  return x.upper() < y;
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cerlt(const T& x, const interval<T, Policies>& y)
{
  return x < y.lower();
}

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool cerle(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.upper() <= y.lower();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cerle(const interval<T, Policies>& x, const T& y)
{
  return x.upper() <= y;
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cerle(const T& x, const interval<T, Policies>& y)
{
  return x <= y.lower();
}

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool cergt(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.lower() > y.upper();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cergt(const interval<T, Policies>& x, const T& y)
{
  return x.lower() > y;
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cergt(const T& x, const interval<T, Policies>& y)
{
  return x > y.upper();
}

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool cerge(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.lower() >= y.upper();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cerge(const interval<T, Policies>& x, const T& y)
{
  return x.lower() >= y;
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cerge(const T& x, const interval<T, Policies>& y)
{
  return x >= y.upper();
}

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool cereq(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.lower() == y.upper() && y.lower() == x.upper();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cereq(const interval<T, Policies>& x, const T& y)
{
  return x.lower() == y && x.upper() == y;
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cereq(const T& x, const interval<T, Policies>& y)
{
  return x == y.lower() && x == y.upper();
}

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool cerne(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.upper() < y.lower() || y.upper() < x.lower();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cerne(const interval<T, Policies>& x, const T& y)
{
  return x.upper() < y || y < x.lower();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool cerne(const T& x, const interval<T, Policies>& y)
{
  return x < y.lower() || y.upper() < x;
}

/*
 * Possibly... comparisons
 */

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool poslt(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.lower() < y.upper();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool poslt(const interval<T, Policies>& x, const T& y)
{
  return x.lower() < y;
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool poslt(const T& x, const interval<T, Policies>& y)
{
  return x < y.upper();
}

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool posle(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.lower() <= y.upper();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool posle(const interval<T, Policies>& x, const T& y)
{
  return x.lower() <= y;
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool posle(const T& x, const interval<T, Policies>& y)
{
  return x <= y.upper();
}

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool posgt(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.upper() > y.lower();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool posgt(const interval<T, Policies>& x, const T& y)
{
  return x.upper() > y;
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool posgt(const T& x, const interval<T, Policies> & y)
{
  return x > y.lower();
}

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool posge(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.upper() >= y.lower();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool posge(const interval<T, Policies>& x, const T& y)
{
  return x.upper() >= y;
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool posge(const T& x, const interval<T, Policies>& y)
{
  return x >= y.lower();
}

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool poseq(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.upper() >= y.lower() && y.upper() >= x.lower();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool poseq(const interval<T, Policies>& x, const T& y)
{
  return x.upper() >= y && y >= x.lower();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool poseq(const T& x, const interval<T, Policies>& y)
{
  return x >= y.lower() && y.upper() >= x;
}

template<class T, class Policies1, class Policies2> inline
BOOST_GPU_ENABLED bool posne(const interval<T, Policies1>& x, const interval<T, Policies2>& y)
{
  return x.upper() != y.lower() || y.upper() != x.lower();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool posne(const interval<T, Policies>& x, const T& y)
{
  return x.upper() != y || y != x.lower();
}

template<class T, class Policies> inline
BOOST_GPU_ENABLED bool posne(const T& x, const interval<T, Policies>& y)
{
  return x != y.lower() || y.upper() != x;
}

} // namespace interval_lib
} // namespace numeric
} //namespace boost

#endif // BOOST_NUMERIC_INTERVAL_COMPARE_EXPLICIT_HPP
