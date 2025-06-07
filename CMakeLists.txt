cmake_minimum_required(VERSION 3.18.2)
project(alg)


set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-std=c++20)
add_definitions(-g)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_C_FLAGS   "-g -O0")
    set(CMAKE_CXX_FLAGS "-g -O0")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_C_FLAGS   "-g -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS "-g -O3 -DNDEBUG")
endif()


###################
### (1) include ###
###################
include_directories(
    alg/01_algorithm/include 
    alg/02_dynprog_vec/include 
    alg/03_dynprog/include 
    alg/04_fundalmental/include 
    alg/05_template/include 
    alg/06_threading/include0 
    alg/06_threading/include1 
    alg/07_cpp20/include 
    alg/08_problems/include 
    alg/all_tests/include 
    /mnt/d/dev/cpp_dependency/boost_1_84_0 
)

##################
### (2) source ###
##################
file(GLOB SOURCES
     "alg/01_algorithm/src/*.cpp" 
     "alg/01_algorithm/test/*.cpp" 
     "alg/02_dynprog_vec/src/*.cpp" 
     "alg/02_dynprog_vec/test/*.cpp" 
     "alg/03_dynprog/src/*.cpp" 
     "alg/03_dynprog/test/*.cpp" 
     "alg/04_fundalmental/src/*.cpp"
     "alg/04_fundalmental/test/*.cpp"
     "alg/05_template/src/*.cpp"
     "alg/05_template/test/*.cpp"
     "alg/06_threading/src/*.cpp"
     "alg/06_threading/test/*.cpp"
     "alg/07_cpp20/src/*.cpp"
     "alg/07_cpp20/test/*.cpp"
     "alg/08_problems/src/*.cpp"
     "alg/08_problems/test/*.cpp"
     "alg/all_tests/src/*.cpp"
)
add_executable(Test ${SOURCES})       # for building executable
# add_library(Test STATIC ${SOURCES}) # for building static lib .a
# add_library(Test SHARED ${SOURCES}) # for building shared lib .so

##################################
### (3) link path and link lib ###
##################################
target_link_libraries(Test -L/lib/x86_64-linux-gnu)
target_link_libraries(Test -lpthread)





