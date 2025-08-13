cmake_minimum_required(VERSION 3.15)

project(Protocol VERSION 0.1 LANGUAGES C)

add_executable(main protocol.c callback.c common.c main.c)