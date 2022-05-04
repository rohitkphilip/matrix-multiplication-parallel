cmake_minimum_required(VERSION 2.8)

project (matrix-multiply)

# SET(CMAKE_C_COMPILER mpicc) 
# SET(CMAKE_CXX_COMPILER mpicxx) 

# no boost libraries are used, yet
#find_package(Boost 1.72 REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})
#include_directories(${CMAKE_SOURCE_DIR})
#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME ON)
#set(BOOST_ROOT "/usr/local/gcc/boost_1_72_0")

# app targets
add_executable(matrix-multiply src/matrix_multiply.cpp)

target_compile_features(matrix-multiply
   PRIVATE 
      cxx_std_17
   )

