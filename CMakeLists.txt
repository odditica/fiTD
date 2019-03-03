project(fitd)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-Wall -lncurses")

add_custom_target(
	doc ALL
	COMMAND doxygen ./Doxyfile
)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES doc)

find_package(Curses REQUIRED)
include_directories(
	src
	${CURSES_INCLUDE_DIR})

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})

