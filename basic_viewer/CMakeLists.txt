cmake_minimum_required(VERSION 3.8)
project(basic_viewer)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF()

MESSAGE(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_C_FLAGS_DEBUG "-O2 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -g")

set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

LIST(APPEND CMAKE_C_FLAGS "-Wall -Wextra -Wno-deprecated-declarations")
LIST(APPEND CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-deprecated-declarations")

# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   add_definitions(-DCOMPILEDWITHC0X)
   message(STATUS "Using flag -std=c++0x.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

#find_package(OpenCV 3.1.0 REQUIRED PATHS /home/nvidia/Documents/opencv_build/share/OpenCV NO_DEFAULT_PATH)
find_package(OpenCV 3.1.0 REQUIRED PATHS /home/hopkinsonlab/Documents/Libraries/OpenCV/OpenCV_3_4/install/share/OpenCV NO_DEFAULT_PATH)

add_executable(basic_viewer src/basic_viewer.cpp)
target_link_libraries(basic_viewer 
  ${OpenCV_LIBRARIES} 
)

