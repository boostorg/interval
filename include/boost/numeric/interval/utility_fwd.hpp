/* Boost interval/utility.hpp template interface file
 *
 * Copyright 2013 Cromwell D. Enage
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_NUMERIC_INTERVAL_UTILITY_FWD_HPP
#define BOOST_NUMERIC_INTERVAL_UTILITY_FWD_HPP

#include <boost/config.hpp>
#include <boost/numeric/interval/detail/interval_prototype.hpp>
#include <boost/numeric/interval/detail/bugs.hpp>
#include <utility>

/*
 * Interface of simple functions
 */

namespace boost { namespace numeric {

    /*
     * Utility Functions
     */

    template<class T, class Policies>
    BOOST_GPU_ENABLED const T& lower(const interval<T,Policies>& x);

    template<class T, class Policies>
    BOOST_GPU_ENABLED const T& upper(const interval<T,Policies>& x);

    template<class T, class Policies>
    BOOST_GPU_ENABLED T checked_lower(const interval<T,Policies>& x);

    template<class T, class Policies>
    BOOST_GPU_ENABLED T width(const interval<T,Policies>& x);

    template<class T, class Policies>
    BOOST_GPU_ENABLED T median(const interval<T,Policies>& x);

    template<class T, class Policies>
    BOOST_GPU_ENABLED interval<T,Policies> widen(const interval<T,Policies>& x, const T& v);

    /*
     * Set-like operations
     */

    template <class T, class Policies>
    BOOST_GPU_ENABLED bool empty(const interval<T,Policies>& x);

    template <class T, class Policies>
    BOOST_GPU_ENABLED bool zero_in(const interval<T,Policies>& x);

    template <class T, class Policies>
    BOOST_GPU_ENABLED bool in_zero(const interval<T,Policies>& x);  // DEPRECATED

    template <class T, class Policies>
    BOOST_GPU_ENABLED bool in(const T& x, const interval<T,Policies>& y);

    template <class T, class Policies>
    BOOST_GPU_ENABLED bool
        subset(
            const interval<T,Policies>& x
          , const interval<T,Policies>& y
        );

    template <class T, class Policies1, class Policies2>
    BOOST_GPU_ENABLED bool
        proper_subset(
            const interval<T,Policies1>& x
          , const interval<T,Policies2>& y
        );

    template <class T, class Policies1, class Policies2>
    BOOST_GPU_ENABLED bool
        overlap(
            const interval<T,Policies1>& x
          , const interval<T,Policies2>& y
        );

    template <class T, class Policies>
    BOOST_GPU_ENABLED bool singleton(const interval<T, Policies>& x);

    template <class T, class Policies1, class Policies2>
    BOOST_GPU_ENABLED bool
        equal(
            const interval<T,Policies1>& x
          , const interval<T,Policies2>& y
        );

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T, Policies>
        intersect(
            const interval<T,Policies>& x
          , const interval<T,Policies>& y
        );

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T, Policies>
        hull(const interval<T,Policies>& x, const interval<T,Policies>& y);

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T, Policies>
        hull(const interval<T,Policies>& x, const T& y);

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T, Policies>
        hull(const T& x, const interval<T,Policies>& y);

    template <class T>
    BOOST_GPU_ENABLED interval<T> hull(const T& x, const T& y);
    
    template <class T, class Policies>
    BOOST_GPU_ENABLED BOOST_NUMERIC_INTERVAL_std(pair)<interval<T,Policies>,interval<T,Policies> >
        bisect(const interval<T,Policies>& x);

    /*
     * Elementary functions
     */

    template <class T, class Policies>
    BOOST_GPU_ENABLED T norm(const interval<T,Policies>& x);

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T,Policies> abs(const interval<T,Policies>& x);

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T,Policies>
        max BOOST_PREVENT_MACRO_SUBSTITUTION (
            const interval<T,Policies>& x
          , const interval<T,Policies>& y
        );

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T,Policies>
        max BOOST_PREVENT_MACRO_SUBSTITUTION (
            const interval<T,Policies>& x
          , const T& y
        );

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T,Policies>
        max BOOST_PREVENT_MACRO_SUBSTITUTION (
            const T& x
          , const interval<T,Policies>& y
        );

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T,Policies>
        min BOOST_PREVENT_MACRO_SUBSTITUTION (
            const interval<T,Policies>& x
          , const interval<T,Policies>& y
        );

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T,Policies>
        min BOOST_PREVENT_MACRO_SUBSTITUTION (
            const interval<T,Policies>& x
          , const T& y
        );

    template <class T, class Policies>
    BOOST_GPU_ENABLED interval<T,Policies>
        min BOOST_PREVENT_MACRO_SUBSTITUTION (
            const T& x
          , const interval<T,Policies>& y
        );
}} // namespace boost::numeric

#endif  // include guard

