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

LOG(${CMAKE_CXX_COMPILER})

include("${CMAKE_SOURCE_DIR}/CMakeLibs")
include(GetGitVersion)
include(GetGitRevisionSHA)
include(CheckCXXCompilerFlag)
include(CXXFeatureCheck)
include(AddCXXCompilerFlag)
include_directories("${CMAKE_SOURCE_DIR}/src")

add_cxx_compiler_flag(--std=c++11)
if(DEFINED USING_CLANG)
  add_cxx_compiler_flag(-stdlib=libstdc++)
endif()

# make analyze (environment variable from Makefile)
if(DEFINED ENV{ANALYZE})
  set(CMAKE_CXX_COMPILER "${CMAKE_SOURCE_DIR}/scripts/clang-analyze.sh")
endif()

include(CMakeLibs)

if(DEFINED ENV{RELEVANCED_BUILD_GIT_VERSION})
  set(RELEVANCED_BUILD_GIT_VERSION "$ENV{RELEVANCED_BUILD_GIT_VERSION}")
else()
  get_git_version(RELEVANCED_BUILD_GIT_VERSION)
endif()

if(DEFINED ENV{RELEVANCED_BUILD_GIT_REVISION_SHA})
  set(RELEVANCED_BUILD_GIT_REVISION_SHA "$ENV{RELEVANCED_BUILD_GIT_REVISION_SHA}")
else()
  get_git_revision_sha(RELEVANCED_BUILD_GIT_REVISION_SHA)
endif()

string(TIMESTAMP RELEVANCED_BUILD_TIMESTAMP_UTC)

configure_file(
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/src/config.h"
)
include_directories("${PROJECT_BINARY_DIR}/src")

add_subdirectory(src)
