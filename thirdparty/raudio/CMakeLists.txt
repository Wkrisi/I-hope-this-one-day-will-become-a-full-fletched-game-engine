cmake_minimum_required(VERSION 3.1)
project(raudio)

add_library(raudio)
target_sources(raudio PRIVATE src/raudio.c)
target_include_directories(raudio PUBLIC include)
set_target_properties(raudio PROPERTIES C_STANDARD 11 C_EXTENSIONS YES)