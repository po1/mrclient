cmake_minimum_required(VERSION 2.8)
project(mrclient)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Ncurses REQUIRED)

include_directories(SYSTEM ${NCURSES_INCLUDE_DIRS})

add_definitions(-D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED)

add_executable(mrclient main.c)
target_link_libraries(mrclient ${NCURSES_LIBRARIES})
