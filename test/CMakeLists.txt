# Copyright 2018, 2019 Peter Dimov
# Copyright 2025 James E. King III
# Copyright 2026 Sayan
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

include(BoostTest OPTIONAL RESULT_VARIABLE HAVE_BOOST_TEST)

if(NOT HAVE_BOOST_TEST)
  return()
endif()

# The library relies on changing the FPU rounding mode at run time, which the
# compiler is only required to honour under strict FP semantics.  Jamfile.v2 sets
# these per toolset; mirror them here.  MSVC is tested first so that clang-cl
# gets /fp:strict rather than the GCC-style flag.
if(MSVC)
  set(BOOST_TEST_COMPILE_OPTIONS /fp:strict)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
  set(BOOST_TEST_COMPILE_OPTIONS -fp-model=strict)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(BOOST_TEST_COMPILE_OPTIONS -frounding-math)
endif()

set(BOOST_TEST_LINK_LIBRARIES Boost::numeric_interval Boost::core)

boost_test(TYPE compile SOURCES integer.cpp)

foreach(t add det fmod mul overflow pi pow cmp cmp_exp cmp_lex cmp_set test_float)
  boost_test(TYPE run SOURCES ${t}.cpp)
endforeach()

boost_test(TYPE run SOURCES cmp_exn.cpp LINK_LIBRARIES Boost::logic)

# MSVC-only regression test (b2: <build>no <toolset>msvc:<build>yes).
if(MSVC)
  boost_test(TYPE run SOURCES msvc_x64_flags.cpp)
endif()

# https://github.com/boostorg/interval/issues/15
# boost_test(TYPE run SOURCES cmp_tribool.cpp)
