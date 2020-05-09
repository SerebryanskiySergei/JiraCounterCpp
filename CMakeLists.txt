cmake_minimum_required(VERSION 3.1...3.17)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(ModernCMake
    VERSION 0.1
    DESCRIPTION "An example project with CMake"
    LANGUAGES CXX)

# If we are at the top-level CMake
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Place for CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE
    # use -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)
    # support for folders in IDE's
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    # include CTest
    include(CTest)
    # include doxygen builder
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
	    add_subdirectory(docs)
    else()
	    message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

include(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14
if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(cmake/add_FetchContent_MakeAvailable.cmake)
endif()

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG        v1.6.0
)
FetchContent_MakeAvailable(spdlog)

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
