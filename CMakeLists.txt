cmake_minimum_required(VERSION 3.10)

# set the project name
project(mcquote C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include_directories(include)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_library(mcquote src/main.c)

target_link_libraries(mcquote m ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
target_include_directories(mcquote PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})