project(fitd)
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

