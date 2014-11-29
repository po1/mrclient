cmake_minimum_required(VERSION 2.8)
project(mrclient)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Ncursesw REQUIRED)

include_directories(SYSTEM ${NCURSESW_INCLUDE_DIRS})

add_executable(mrclient main.c)
target_link_libraries(mrclient ${NCURSESW_LIBRARIES})
