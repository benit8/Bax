/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Interpreter.cpp
*/

#include "Bax/Lexer.hpp"
#include "Bax/Parser.hpp"
#include "Bax/Runtime/Interpreter.hpp"
#include "Common/Assertions.hpp"
#include "Common/Log.hpp"
#include <string>

// -----------------------------------------------------------------------------

namespace Bax
{

Interpreter::Interpreter(const std::vector<std::string>& args, const std::unordered_map<std::string, std::string>& env)
: Runner(args, env)
{
}

Value Interpreter::interpret(const std::string& source)
{
	auto parser = Parser(Lexer(source));
	auto ast = parser.parse();
	if (!ast) {
		Log::error("Parsing failed");
		return Value(false);
	}

	ast->dump();

	return run(ast);
}

Value Interpreter::run(AST::Node* node)
{
	// It would be so much fucking easier to implement this as a virtual method
	// in the AST nodes, however I don't see why a Node class should depend on
	// the Interpreter class; so that will do for now.
	if (auto n = dynamic_cast<AST::BlockStatement*>(node))
		return run_block_statement(n);
	else if (auto n = dynamic_cast<AST::VariableDeclaration*>(node))
		return run_variable_declaration(n);

	Log::alert("Must implement Interpreter::run(AST::{}*)", node->class_name());
	ASSERT_NOT_REACHED();
}

Value Interpreter::run_block_statement(AST::BlockStatement* node)
{
	for (auto stmt : node->statements) {
		run(stmt);
	}

	return Value();
}

Value Interpreter::run_variable_declaration(AST::VariableDeclaration* node)
{
	auto value = run(node->value);
	// current_scope().set_symbol(node->name->name, value);
	return value;
}

}
