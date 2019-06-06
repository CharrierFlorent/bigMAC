# cmake_minimum_required(VERSION <specify CMake version here>)
project(bigMAC C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(bigMAC)

add_executable(bigMAC
        bigMAC/AC8.c
        bigMAC/AC8.h
        bigMAC/BT.c
        bigMAC/BT.h
        bigMAC/FC.c
        bigMAC/FC.h
        bigMAC/generateur.c
        bigMAC/generateur.h
        bigMAC/list.c
        bigMAC/list.h
        bigMAC/main.c
        bigMAC/structure.c
        bigMAC/structure.h
        bigMAC/PC8.c bigMAC/PC8.h)
