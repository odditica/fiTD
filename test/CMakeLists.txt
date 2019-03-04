project(TEST)

add_test(
  NAME ${PROJECT_NAME}
  COMMAND test
)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
find_package(Catch2 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})
target_link_libraries(${PROJECT_NAME} dependencies)
target_link_libraries(${PROJECT_NAME} Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})