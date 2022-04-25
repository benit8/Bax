/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Runtime / Function.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#include "Bax/Runtime/UpValue.hpp"
#include "Bax/Token.hpp"
#include <optional>
#include <tuple>
#include <vector>

// -----------------------------------------------------------------------------

namespace Bax
{

class Scope
{
private:
	Scope* m_parent { nullptr };
	std::vector<Token> m_locals { };
	std::vector<Token> m_up_values { };

public:
	Scope(Scope* parent = nullptr)
	: m_parent(parent)
	{}

	size_t add_local(const Token& token)
	{
		auto res = find_local(token);
		if (res) {
			return std::get<0>(*res);
		}

		m_locals.push_back(token);
		return m_locals.size() - 1;
	}

	std::optional<std::tuple<size_t, bool>> find_local(const Token& token)
	{
		for (size_t i = 0; i < m_locals.size(); ++i) {
			if (m_locals[i] == token)
				return std::make_tuple(i, false);
		}

		for (size_t i = 0; i < m_locals.size(); ++i) {
			if (m_locals[i] == token)
				return std::make_tuple(i, true);
		}

		for (auto p = m_parent; p != nullptr; p = p->parent()) {
			auto res = p->find_local(token);
			if (!res)
				return {};
			auto [ index, is_up_value ] = *res;
			m_up_values.push_back((is_up_value ? p->up_values() : p->locals())[index]);
			return std::make_tuple(m_up_values.size() - 1, true);
		}

		return {};
	}

	Scope* parent() const { return m_parent; }
	const std::vector<Token>& locals() const { return m_locals; }
	const std::vector<Token>& up_values() const { return m_up_values; }
};

}
