cmake_minimum_required(VERSION 2.8.12)
project(relevanced)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLibs")

string(ASCII 27 ESC)
macro(WARNING_LOG MESSAGE)
  message("-- ${ESC}[31m${MESSAGE}${ESC}[m")
endmacro(WARNING_LOG)

macro(LOG MESSAGE)
  message("-- ${MESSAGE}")
endmacro(LOG)

# If no explicit compiler override and clang++ exists, prefer clang.
# find_file(CLANGCXX_3_5_EXISTS, "clang++-3.5")
# find_file(CLANGXX_EXISTS "clang++")
# if(DEFINED ENV{CC})
#   set(CMAKE_C_COMPILER "$ENV{CC}")
#   LOG("Overriding C compiler from clang to $ENV{CC}")
# elseif(CLANGCXX_3_5_EXISTS)
#   LOG(${CMAKE_CXX_COMPILER})
#   set(CMAKE_C_COMPILER "clang-3.5")
# elseif(CLANGXX_EXISTS)
#   set(CMAKE_C_COMPILER "clang")
# endif()
# if(DEFINED ENV{CXX})
#   set(CMAKE_CXX_COMPILER "$ENV{CXX}")
#   LOG("Overriding CXX compiler from clang++ to $ENV{CXX}")
# elseif(CLANGCXX_3_5_EXISTS)
#   set(USING_CLANG true)
#   set(CMAKE_CXX_COMPILER "clang++-3.5")
# elseif(CLANGXX_EXISTS)
#   set(USING_CLANG true)
#   set(CMAKE_CXX_COMPILER "clang++")
# endif()

set(CMAKE_CXX_COMPILER "clang++-3.5")
set(CMAKE_C_COMPILER "clang")


LOG(${CMAKE_CXX_COMPILER})

include("${CMAKE_SOURCE_DIR}/CMakeLibs")
include(GetGitVersion)
include(CheckCXXCompilerFlag)
include(CXXFeatureCheck)
include(AddCXXCompilerFlag)
include_directories("${CMAKE_SOURCE_DIR}/src")

add_cxx_compiler_flag(--std=c++14)
if(DEFINED USING_CLANG)
  add_cxx_compiler_flag(-stdlib=libstdc++)
endif()

# make debug (environment variable from Makefile)
if(DEFINED ENV{DEBUG})
  set(DEBUG TRUE)
  set(CMAKE_BUILD_TYPE "Debug")
  add_compile_options(-g -O0)
  add_definitions(-DDEBUG)
  WARNING_LOG("Setting DEBUG build")
else()
  set(DEBUG FALSE)
  add_compile_options(-Os)
  add_definitions(-DNDEBUG)
endif()

if(NOT DEFINED ENV{OPTIMIZED})
  add_compile_options(-march=x86-64 -mno-avx)
endif()

# make analyze (environment variable from Makefile)
if(DEFINED ENV{ANALYZE})
  set(CMAKE_CXX_COMPILER "${CMAKE_SOURCE_DIR}/tools/analysis/clang-analyze.sh")
endif()

include(CMakeLibs)

# Discover build version from an environment variable or from the git checkout.
if(DEFINED ENV{RELEVANCED_BUILD_VERSION})
  set(RELEVANCED_BUILD_VERSION "$ENV{RELEVANCED_BUILD_VERSION}")
else()
  # Generate version from git
  execute_process(
    COMMAND git describe --tags HEAD --always
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE RELEVANCED_BUILD_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()

# We need to link some packages as dynamic/dependent.
set(CMAKE_FIND_LIBRARY_SUFFIXES .dylib .so .a)

find_library(wangle REQUIRED)
find_library(folly REQUIRED)
find_library(mitie REQUIRED)
find_library(thrift REQUIRED)
find_library(thriftcpp2 REQUIRED)
find_library(thriftz REQUIRED)
find_library(rocksdb REQUIRED)
find_library(glog REQUIRED)
find_library(gflags REQUIRED)
find_library(bz2 REQUIRED)
find_library(lz REQUIRED)
find_library(lz4 REQUIRED)
find_library(boost_thread REQUIRED)
find_library(boost_system REQUIRED)
find_library(jemalloc REQUIRED)
find_library(atomic REQUIRED)
find_library(pthread REQUIRED)
find_library(double-conversion REQUIRED)
find_library(snappy REQUIRED)


# enable_testing()

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

add_subdirectory(src)
add_subdirectory(src/test)


# make format-all
add_custom_target(
  format-all
  find src  \( -name "*.h" -o -name "*.cpp" -o -name "*.mm" \)
    -exec clang-format -i {} +
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Format all code with clang-format" VERBATIM
)

