cmake_minimum_required(VERSION 3.9)
project(MyPthread C)

set(CMAKE_C_STANDARD 99)

add_executable(MyPthread my_pthread.c my_pthread_t.h)