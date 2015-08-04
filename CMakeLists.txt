cmake_minimum_required(VERSION 3.3.0)
project(Squirrel)
add_library(Squirrel SHARED Squirrel.cpp SquirrelFactory.cpp Dll.cpp Dll.def Dll.rc util.cpp)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
