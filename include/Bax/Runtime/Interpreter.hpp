/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Interpreter.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include <istream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Bax/AST.hpp"
#include "Bax/Runtime/Runner.hpp"
#include "Bax/Runtime/Value.hpp"

// -----------------------------------------------------------------------------

namespace Bax
{

class Interpreter final : public Runner
{
public:
	Interpreter(
		const std::vector<std::string>& args = {},
		const std::unordered_map<std::string, std::string>& environment = {}
	);

	Value interpret(const std::string& source) override;

private:
	Value run(AST::Node*);
	Value run_block_statement(AST::BlockStatement*);
	Value run_variable_declaration(AST::VariableDeclaration*);
};

}
