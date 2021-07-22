##
## Bax Tests, 2021
## Benoît Lormeau <blormeau@outlook.com>
## CMakeLists.txt
##

cmake_minimum_required(VERSION 3.15)

project(
	${CMAKE_PROJECT_NAME}Tests
	LANGUAGES CXX
)

add_subdirectory(googletest)

add_executable(${PROJECT_NAME})
add_test(
	NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)

target_sources(${PROJECT_NAME}
PUBLIC
	sources/Unit.cpp
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
	Bax
	gtest
	gtest_main
)
