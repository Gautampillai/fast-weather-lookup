cmake_minimum_required(VERSION 3.30)
project(fast_weather_lookup)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(fast_weather_lookup main.cpp Trie.cpp
        CityIndex.h HashMapIndex.h)