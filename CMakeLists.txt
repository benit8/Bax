##
## Bax, 2021
## Benoît Lormeau <blormeau@outlook.com>
## Main CMakeLists
##

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13")
	cmake_policy(SET CMP0076 NEW)
endif()

project(
	"Bax"
	VERSION 0.1.0
	LANGUAGES C CXX
)

add_library(${PROJECT_NAME} SHARED)

# External dependencies
add_subdirectory(extern)

# set_target_properties(${PROJECT_NAME}
# PROPERTIES
# 	POSITION_INDEPENDENT_CODE ON
# )

target_compile_features(${PROJECT_NAME}
PUBLIC
	cxx_std_20
)

target_compile_options(${PROJECT_NAME}
PRIVATE
	-Wall -Wextra
)

target_include_directories(${PROJECT_NAME}
PUBLIC
	include
PROTECTED
	extern
	sources
)

target_sources(${PROJECT_NAME}
PUBLIC
	include/Bax/Compiler/AST.hpp
	include/Bax/Compiler/Compiler.hpp
	include/Bax/Compiler/Lexer.hpp
	include/Bax/Compiler/Parser.hpp
	include/Bax/Compiler/Token.hpp
	include/Bax/Compiler/TokenTypes.hpp
	include/Bax/VM/Value.hpp
	include/Bax/VM/VM.hpp
PRIVATE
	sources/Common/Assertions.hpp
	sources/Common/GenericLexer.cpp
	sources/Common/GenericLexer.hpp
	sources/Common/Log.cpp
	sources/Common/Log.hpp
	sources/Common/OptionParser.cpp
	sources/Common/OptionParser.hpp
	sources/Common/TTYEscapeSequences.hpp
	sources/Compiler/AST.cpp
	sources/Compiler/Compiler.cpp
	sources/Compiler/Lexer.cpp
	sources/Compiler/Parser.cpp
	sources/Compiler/Token.cpp
	sources/VM/VM.cpp
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
	fmt::fmt-header-only
	# nlohmann_json::nlohmann_json
)

# CLI program
add_executable(bax sources/main.cpp)
target_link_libraries(bax PUBLIC Bax)

# Tests
enable_testing()
add_subdirectory(tests)
