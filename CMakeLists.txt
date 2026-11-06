cmake_minimum_required(VERSION 3.30)
project(fast_weather_lookup)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(fast_weather_lookup main.cpp Trie.cpp)

add_executable(core_tests tests/core_tests.cpp CityIndex.h
               HashMapIndex.h TrieIndex.h DataLoader.h
               DataLoader.cpp Benchmark.h Trie.cpp)

add_executable(weather_cli weather_cli.cpp DataLoader.cpp
               Trie.cpp CityIndex.h HashMapIndex.h
               TrieIndex.h Dataloader.h Benchmark.h)