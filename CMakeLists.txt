cmake_minimum_required(VERSION 3.26)
project(MultiLab C)  # Choisissez un nom de projet approprié

set(CMAKE_C_STANDARD 11)

# Créez un exécutable à partir de main.c dans Lab1
add_executable(Lab1 Lab1/main.c)

# Créez un exécutable à partir de main.c, process1.c et process2.c dans Lab2
add_executable(Lab2 Lab2/main.c Lab2/process1.c Lab2/process2.c)