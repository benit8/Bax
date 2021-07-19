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

add_executable(${PROJECT_NAME})
add_test(
	NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)

target_compile_features(${PROJECT_NAME}
PUBLIC
	cxx_std_20
)

target_compile_options(${PROJECT_NAME}
PUBLIC
	-g3
)

target_sources(${PROJECT_NAME}
PUBLIC
	sources/Unit.cpp
)

add_subdirectory(googletest)
target_link_libraries(${PROJECT_NAME}
PUBLIC
	gtest
	gtest_main
)
