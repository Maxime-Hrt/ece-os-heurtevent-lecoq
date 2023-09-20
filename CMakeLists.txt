cmake_minimum_required(VERSION 3.26)
project(MultiLab C)

set(CMAKE_C_STANDARD 11)

add_executable(Lab1 Lab1/main.c)

add_executable(Lab2 Lab2/main.c Lab2/receiver.c Lab2/sender.c)