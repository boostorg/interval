/* Boost interval/detail/cuda_rounding_control.hpp file
 *
 * Copyright 2024 Lorenz Gillner
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or
 * copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_DETAIL_CUDA_ROUNDING_CONTROL_HPP
#define BOOST_NUMERIC_INTERVAL_DETAIL_CUDA_ROUNDING_CONTROL_HPP

#if !defined(__CUDACC__) && !defined(__NVCC__)
#error Boost.Numeric.Interval: This header is intended for CUDA GPUs only.
#endif

#include <cuda.h>
#include <math_constants.h>

#include <cuda/std/cassert>
#include <cuda/std/cmath>
#include <cuda/std/climits>
#include <cuda/std/utility>

#include <boost/numeric/interval/checking.hpp>
#include <boost/numeric/interval/rounding.hpp>

namespace boost {
namespace numeric {
namespace gpu_spec {

  /** templates */
  template <class T> __device__ T min(const T &x, const T &y);
  template <class T> __device__ T max(const T &x, const T &y);

  /** float specialization */
  template <> inline __device__ float min(const float &x, const float &y)
  { return fminf(x, y); }
  template <> inline __device__ float max(const float &x, const float &y)
  { return fmaxf(x, y); }

  /** double specialization */
  template <> inline __device__ double min(const double &x, const double &y)
  { return fmin(x, y); }
  template <> inline __device__ double max(const double &x, const double &y)
  { return fmax(x, y); }
  
} // namespace gpu_spec

namespace interval_lib {

namespace detail {

  /** directed operation templates */
  template <class T> __device__ T add_rd(const T &x, const T &y);
  template <class T> __device__ T add_ru(const T &x, const T &y);
  template <class T> __device__ T sub_rd(const T &x, const T &y);
  template <class T> __device__ T sub_ru(const T &x, const T &y);
  template <class T> __device__ T mul_rd(const T &x, const T &y);
  template <class T> __device__ T mul_ru(const T &x, const T &y);
  template <class T> __device__ T div_rd(const T &x, const T &y);
  template <class T> __device__ T div_ru(const T &x, const T &y);
  template <class T> __device__ T sqrt_rd(const T &x);
  template <class T> __device__ T sqrt_ru(const T &x);
  template <class T> __device__ T pred(const T &x);
  template <class T> __device__ T succ(const T &x);
  template <class T> __device__ T mid(const T &x, const T &y);

  template <class T, class U> __device__ T conv_rd(U const &v);
  template <class T, class U> __device__ T conv_ru(U const &v);

  /** float specialization */
  #define BOOST_NUMERIC_INTERVAL_gpu_spec(a) \
    template <> inline __device__ float a##_rd(const float &x, const float &y) \
    { return __f##a##_rd(x, y); } \
    template <> inline __device__ float a##_ru(const float &x, const float &y) \
    { return __f##a##_ru(x, y); }
  BOOST_NUMERIC_INTERVAL_gpu_spec(add);
  BOOST_NUMERIC_INTERVAL_gpu_spec(sub);
  BOOST_NUMERIC_INTERVAL_gpu_spec(mul);
  BOOST_NUMERIC_INTERVAL_gpu_spec(div);
  #undef BOOST_NUMERIC_INTERVAL_gpu_spec
  template <> inline __device__ float sqrt_rd(const float &x)
  { return __fsqrt_rd(x); }
  template <> inline __device__ float sqrt_ru(const float &x)
  { return __fsqrt_ru(x); }
  template <> inline __device__ float pred(const float &x)
  { return nextafterf(x, -CUDART_INF_F); }
  template <> inline __device__ float succ(const float &x)
  { return nextafterf(x, CUDART_INF_F); }
  template <> inline __device__ float mid(const float &x, const float &y)
  { return __fdiv_rn(__fadd_rn(x, y), 2); }
  template <> inline __device__ float conv_rd(const int &v)
  { return __int2float_rd(v); }
  template <> inline __device__ float conv_ru(const int &v)
  { return __int2float_ru(v); }
  template <> inline __device__ float conv_rd(const long long int &v)
  { return __ll2float_rd(v); }
  template <> inline __device__ float conv_ru(const long long int &v)
  { return __ll2float_ru(v); }
  template <> inline __device__ float conv_rd(const double &v)
  { return __double2float_rd(v); }
  template <> inline __device__ float conv_ru(const double &v)
  { return __double2float_ru(v); }

  /** double specialization */
  #define BOOST_NUMERIC_INTERVAL_gpu_spec(a) \
    template <> inline __device__ double a##_rd(const double &x, const double &y) \
    { return __d##a##_rd(x, y); } \
    template <> inline __device__ double a##_ru(const double &x, const double &y) \
    { return __d##a##_ru(x, y); }
  BOOST_NUMERIC_INTERVAL_gpu_spec(add);
  BOOST_NUMERIC_INTERVAL_gpu_spec(sub);
  BOOST_NUMERIC_INTERVAL_gpu_spec(mul);
  BOOST_NUMERIC_INTERVAL_gpu_spec(div);
  #undef BOOST_NUMERIC_INTERVAL_gpu_spec
  template <> inline __device__ double sqrt_rd(const double &x)
  { return __dsqrt_rd(x); }
  template <> inline __device__ double sqrt_ru(const double &x)
  { return __dsqrt_ru(x); }
  template <> inline __device__ double pred(const double &x)
  { return nextafter(x, -CUDART_INF); }
  template <> inline __device__ double succ(const double &x)
  { return nextafter(x, CUDART_INF); }
  template <> inline __device__ double mid(const double &x, const double &y)
  { return __ddiv_rn(__dadd_rn(x, y), 2); }
  template <> inline __device__ double conv_rd(const long long int &v)
  { return __ll2double_rd(v); }
  template <> inline __device__ double conv_ru(const long long int &v)
  { return __ll2double_ru(v); }

} // namespace detail

/**
 * Checking should be compatible with the GPU "as is", so no further spezializations are needed
 */

template<class T>
struct checking_base_gpu: checking_base<T>
{};

/**
 * GPU rounding control
 */

template <class T>
struct rounding_control_gpu: rounding_control<T>
{};

template <>
struct rounding_control_gpu<float>
{
  __device__ float to_int(const float& x) { return nearbyintf(x); }
};

template <>
struct rounding_control_gpu<double>
{
  __device__ double to_int(const double& x) { return nearbyint(x); }
};

/**
 * Directly rounded arithmetic
 */

template<class T, class Rounding = rounding_control<T> >
struct rounded_arith_direct;

template <class T, class Rounding>
struct rounded_arith_direct : Rounding
{
  __device__ void init() {}
  template <class U> __device__ T conv_down(U const &v) { return detail::conv_rd(v); }
  template <class U> __device__ T conv_up(U const &v) { return detail::conv_ru(v); }
  #define BOOST_NUMERIC_INTERVAL_new_func(a) \
    __device__ T a##_down(const T &x, const T &y) \
    { return detail::a##_rd(x, y); } \
    __device__ T a##_up(const T &x, const T &y) \
    { return detail::a##_ru(x, y); }
  BOOST_NUMERIC_INTERVAL_new_func(add);
  BOOST_NUMERIC_INTERVAL_new_func(sub);
  BOOST_NUMERIC_INTERVAL_new_func(mul);
  BOOST_NUMERIC_INTERVAL_new_func(div);
  #undef BOOST_NUMERIC_INTERVAL_new_func
  __device__ T sqrt_down(const T &x) { return detail::sqrt_rd(x); }
  __device__ T sqrt_up(const T &x)   { return detail::sqrt_ru(x); }
  __device__ T median(const T &x, const T &y) { return detail::mid(x, y); }
  __device__ T int_down(const T &x) { BOOST_NUMERIC_INTERVAL_using_math(floor); return floor(x); }
  __device__ T int_up(const T &x)   { BOOST_NUMERIC_INTERVAL_using_math(ceil); return ceil(x); }
  __device__ T next_down(const T &x) { return detail::pred(x); }
  __device__ T next_up(const T &x)   { return detail::succ(x); }
};

/**
 * Rounded transcendental functions
 */

template<class T, class Rounding = rounded_arith_direct<T> > 
struct rounded_transc_direct;

template <class T, class Rounding>
struct rounded_transc_direct: Rounding
{
  #define BOOST_NUMERIC_INTERVAL_new_func(a) \
    __device__ T a##_down(const T& x) \
    { BOOST_NUMERIC_INTERVAL_using_math(a); T ax = a(x); return (ax > T(-1) && ax != T(0)) ? next_down(ax) : ax; } \
    __device__ T a##_up  (const T& x) \
    { BOOST_NUMERIC_INTERVAL_using_math(a); T ax = a(x); return (ax < T(1) && ax != T(0)) ? next_up(ax) : ax; }
  BOOST_NUMERIC_INTERVAL_new_func(exp)   // 1 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(log)   // 1 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(sin)   // 2 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(cos)   // 2 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(tan)   // 2 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(asin)  // 2 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(acos)  // 2 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(atan)  // 2 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(sinh)  // 2 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(cosh)  // 1 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(tanh)  // 1 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(asinh) // 3 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(acosh) // 3 ulp (double)
  BOOST_NUMERIC_INTERVAL_new_func(atanh) // 2 ulp (double)
  #undef BOOST_NUMERIC_INTERVAL_new_func
};

/** 
 * Convenience wrappers for GPU use
 */

template <class T>
struct rounded_arith_gpu: rounded_arith_direct<T>
{};

template <class T>
struct rounded_transc_gpu: rounded_transc_direct<T>
{};

/* fall back to exact mode if no specialization is available */
template <class T>
struct rounded_math_gpu: save_state_nothing<rounded_arith_exact<T> >
{};

template <>
struct rounded_math_gpu<float>: save_state_nothing<rounded_arith_gpu<float> >
{};

template <>
struct rounded_math_gpu<double>: save_state_nothing<rounded_arith_gpu<double> >
{};

template<class T>
struct default_policies_gpu
{
  typedef policies<rounded_math_gpu<T>, checking_base_gpu<T> > type;
};

} // namespace interval_lib
} // namespace numeric
} // namespace boost

#endif /* BOOST_NUMERIC_INTERVAL_DETAIL_CUDA_ROUNDING_CONTROL_HPP */