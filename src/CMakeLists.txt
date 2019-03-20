cmake_minimum_required(VERSION 3.5)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

file(GLOB SOURCES "*.cpp")
add_library(fitd-main STATIC
CDefinitions.cpp  CGame.cpp        CGameGraphics.cpp  CTower.cpp
CEnemy.cpp        CGameEntity.cpp  CGfx.cpp)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} fitd-main)
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})