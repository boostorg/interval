/* Boost interval/detail/bugs.hpp file
 *
 * Copyright 2000 Jens Maurer
 * Copyright 2002 Hervé Brönnimann, Guillaume Melquiond, Sylvain Pion
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_DETAIL_BUGS
#define BOOST_NUMERIC_INTERVAL_DETAIL_BUGS

#include <boost/config.hpp>

#if defined(__GLIBC__) && (defined(__USE_MISC) || defined(__USE_XOPEN_EXTENDED) || defined(__USE_ISOC99)) && !defined(__ICC)
#  define BOOST_HAS_INV_HYPERBOLIC
#endif

#ifdef BOOST_NO_STDC_NAMESPACE
#  define BOOST_NUMERIC_INTERVAL_using_math(a) using ::a
#  ifdef BOOST_HAS_INV_HYPERBOLIC
#    define BOOST_NUMERIC_INTERVAL_using_ahyp(a) using ::a
#  endif
#else
#  define BOOST_NUMERIC_INTERVAL_using_math(a) using std::a
#  if defined(BOOST_HAS_INV_HYPERBOLIC)
#    if defined(__GLIBCPP__) || defined(__GLIBCXX__)
#    define BOOST_NUMERIC_INTERVAL_using_ahyp(a) using ::a
#    else
#    define BOOST_NUMERIC_INTERVAL_using_ahyp(a) using std::a
#  endif
#  endif
#endif

#if defined(__COMO__) || defined(BOOST_INTEL)
#  define BOOST_NUMERIC_INTERVAL_using_max(a) using std::a
#elif defined(BOOST_NO_STDC_NAMESPACE)
#  define BOOST_NUMERIC_INTERVAL_using_max(a) using ::a
#else
#  define BOOST_NUMERIC_INTERVAL_using_max(a) using std::a
#endif

#ifndef BOOST_NUMERIC_INTERVAL_using_ahyp
#  define BOOST_NUMERIC_INTERVAL_using_ahyp(a)
#endif

#if defined(__CUDACC__)
#  define BOOST_GPU_DISABLED __host__
#else
#  define BOOST_GPU_DISABLED
#endif

#if defined(__CUDA_ARCH__)
#  define BOOST_NUMERIC_INTERVAL_std(a) cuda::std::a
#  undef BOOST_USING_STD_MIN
#  define BOOST_USING_STD_MIN() using boost::numeric::gpu_spec::min
#  undef BOOST_USING_STD_MAX
#  define BOOST_USING_STD_MAX() using boost::numeric::gpu_spec::max
#  define BOOST_NUMERIC_INTERVAL_throw(exception) printf("%s\n", exception); __trap()
#else
#  define BOOST_NUMERIC_INTERVAL_std(a) std::a
#  define BOOST_NUMERIC_INTERVAL_throw(exception) throw std::runtime_error(exception)
#endif

#define BOOST_NUMERIC_INTERVAL_using_std(a) using BOOST_NUMERIC_INTERVAL_std(a)

#endif // BOOST_NUMERIC_INTERVAL_DETAIL_BUGS
