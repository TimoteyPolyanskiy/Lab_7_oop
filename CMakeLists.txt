cmake_minimum_required(VERSION 3.30)
project(Lab_7_oop)

set(CMAKE_CXX_STANDARD 20)

add_executable(Lab_7_oop main.cpp
                ProxyPattern.cpp
                FlyweightPattern.cpp
                ProxyPattern.h
                FlyweightPattern.h)
