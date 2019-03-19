project(fitd)

string(TIMESTAMP TODAY "%d/%m/%y")

set (PROJECT_VERSION_MAJOR 1)
set (PROJECT_VERSION_MINOR 0)
set (PROJECT_VERSION_PATCH 1)
set (PROJECT_BUILD_DATE ${TODAY})

configure_file(
	"src/config.h.in"
	"src/config.h"
)

set (CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.5)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES doc)

add_custom_target(
	doc ALL
	COMMAND doxygen ./Doxyfile
)
set_target_properties(doc PROPERTIES EXCLUDE_FROM_ALL TRUE)
file(COPY data DESTINATION build)

enable_testing()

include_directories(src)
add_subdirectory(src build)
add_subdirectory(test)

