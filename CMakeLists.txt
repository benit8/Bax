##
## Bax, 2022
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
	-Wall -Wextra -g3
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
	include/Bax/Runtime/Fiber.hpp
	include/Bax/Runtime/Function.hpp
	include/Bax/Runtime/Interpreter.hpp
	include/Bax/Runtime/Object.hpp
	include/Bax/Runtime/UpValue.hpp
	include/Bax/Runtime/Value.hpp
	include/Bax/Runtime/VM.hpp
	include/Bax/AST.hpp
	include/Bax/Compiler.hpp
	include/Bax/Lexer.hpp
	include/Bax/Parser.hpp
	include/Bax/Token.hpp
PRIVATE
	include/Bax/Runtime/Runner.hpp
	include/Bax/Token/Types.hpp
	sources/Common/Assertions.hpp
	sources/Common/GenericLexer.cpp
	sources/Common/GenericLexer.hpp
	sources/Common/Log.cpp
	sources/Common/Log.hpp
	sources/Common/OptionParser.cpp
	sources/Common/OptionParser.hpp
	sources/Common/TTYEscapeSequences.hpp
	sources/Parser/GrammarRules.cpp
	sources/Runtime/Interpreter.cpp
	sources/Runtime/VM.cpp
	sources/AST.cpp
	sources/Compiler.cpp
	sources/Lexer.cpp
	sources/Parser.cpp
	sources/Token.cpp
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
