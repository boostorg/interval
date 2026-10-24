# Copyright 2018, 2019 Peter Dimov
# Copyright 2025 James E. King III
# Copyright 2026 Sayan Samanta
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

# Each public header must compile on its own.  These compile-only tests are
# slow and add little per configuration, so they are off by default and CI
# turns them on in one job.
option(BOOST_NUMERIC_INTERVAL_TEST_SELF_CONTAINED_HEADERS "Test that each public header compiles on its own" OFF)

if(BOOST_NUMERIC_INTERVAL_TEST_SELF_CONTAINED_HEADERS)
  set(headers_path ${CMAKE_CURRENT_SOURCE_DIR}/../include/boost/numeric)
  file(GLOB_RECURSE headers RELATIVE ${headers_path} CONFIGURE_DEPENDS ${headers_path}/*.hpp)
  list(FILTER headers EXCLUDE REGEX "/detail/")
  # x86 only by design.
  if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64|x86|i[3-6]86)$")
    list(REMOVE_ITEM headers interval/ext/x86_fast_rounding_control.hpp)
  endif()

  foreach(h IN LISTS headers)
    string(MAKE_C_IDENTIFIER "${h}" name)
    boost_test(TYPE compile NAME self_contained-${name} SOURCES self_contained_header.cpp
      COMPILE_DEFINITIONS BOOST_NUMERIC_INTERVAL_TEST_HEADER=${h})
  endforeach()
endif()

foreach(t add det fmod mul nth_root overflow pi pow sqrt_rounding sqrt_point cmp cmp_exp cmp_lex cmp_set test_float)
  boost_test(TYPE run SOURCES ${t}.cpp)
endforeach()

boost_test(TYPE run SOURCES cmp_exn.cpp LINK_LIBRARIES Boost::logic)
boost_test(TYPE run SOURCES cmp_tribool.cpp LINK_LIBRARIES Boost::logic)
boost_test(TYPE run SOURCES converting_ctor.cpp)

# One statement that must not compile per value of the macro; without the
# macro the file must compile.
boost_test(TYPE compile SOURCES converting_ctor_fail.cpp)
foreach(n 1 2 3 4 5 6 7)
  boost_test(TYPE compile-fail NAME converting_ctor_fail_${n} SOURCES converting_ctor_fail.cpp
    COMPILE_DEFINITIONS BOOST_NUMERIC_INTERVAL_TEST_FAIL=${n})
endforeach()

# PowerPC-only regression test.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")
  boost_test(TYPE run SOURCES rounding_mode.cpp)
endif()

# x86 regression tests; rounding_mode_sse runs the arithmetic in the SSE unit.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64|x86|i[3-6]86)$")
  boost_test(TYPE run SOURCES x86_rounding.cpp)
  boost_test(TYPE run NAME rounding_mode_x86 SOURCES rounding_mode.cpp)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT MSVC)
    boost_test(TYPE run NAME rounding_mode_sse SOURCES rounding_mode.cpp
      COMPILE_OPTIONS -msse2 -mfpmath=sse)
  endif()
endif()

# MSVC-only regression test (b2: <build>no <toolset>msvc:<build>yes).
if(MSVC)
  boost_test(TYPE run SOURCES msvc_x64_flags.cpp)
endif()
