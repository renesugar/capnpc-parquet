project(“capnpc-parquet” CXX)
cmake_minimum_required(VERSION 3.1)

if(APPLE)
  set(CMAKE_MACOSX_RPATH 1)
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
endif()

# This ensures that things like gnu++11 get passed correctly
set(CMAKE_CXX_STANDARD 11)

# We require a C++11 compliant compiler
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# where to find cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
set(BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build-support")

# if no build build type is specified, default to debug builds
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

# find rapidjson headers
find_package(RapidJson REQUIRED)
include_directories(${RAPIDJSON_INCLUDE_DIR})
message(STATUS "RapidJson include dir: " ${RAPIDJSON_INCLUDE_DIR})

find_package(Boost REQUIRED COMPONENTS thread regex program_options filesystem system date_time)
include_directories(${Boost_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})
message(STATUS "Boost include dir: " ${Boost_INCLUDE_DIRS})
message(STATUS "Boost libraries: " ${Boost_LIBRARIES})

find_package(Parquet REQUIRED)
include_directories(SYSTEM ${PARQUET_INCLUDE_DIR})
set(LIBS ${LIBS} ${PARQUET_LIBS})
message(STATUS "Parquet include dir: " ${PARQUET_INCLUDE_DIR})
message(STATUS "Parquet libraries: " ${PARQUET_LIBS})

find_package(Arrow REQUIRED)
include_directories(SYSTEM ${ARROW_INCLUDE_DIR})
set(LIBS ${LIBS} ${ARROW_LIBS})
message(STATUS "Arrow include dir: " ${ARROW_INCLUDE_DIR})
message(STATUS "Arrow libraries: " ${ARROW_LIBS})

# find Capn Proto headers
find_package(CapnProto CONFIG REQUIRED)

add_executable(capnpc-parquet capnpparquet.cpp)
target_link_libraries(capnpc-parquet CapnProto::capnp CapnProto::capnpc CapnProto::kj ${Boost_LIBRARIES} ${PARQUET_SHARED_LIB})
target_include_directories(capnpc-parquet PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Boost_INCLUDE_DIRS} ${ARROW_INCLUDE_DIR} ${PARQUET_INCLUDE_DIR})
