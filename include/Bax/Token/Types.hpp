/*
** Bax, 2022
** Benoit Lormeau <blormeau@outlook.com>
** Token / Types.hpp
*/

#pragma once

// -----------------------------------------------------------------------------

#define __ENUMERATE_TOKEN_TYPES           \
	__ENUMERATE(Ampersand)                \
	__ENUMERATE(AmpersandAmpersand)       \
	__ENUMERATE(AmpersandAmpersandEquals) \
	__ENUMERATE(AmpersandEquals)          \
	__ENUMERATE(Asterisk)                 \
	__ENUMERATE(AsteriskAsterisk)         \
	__ENUMERATE(AsteriskAsteriskEquals)   \
	__ENUMERATE(AsteriskEquals)           \
	__ENUMERATE(Backslash)                \
	__ENUMERATE(Caret)                    \
	__ENUMERATE(CaretEquals)              \
	__ENUMERATE(Colon)                    \
	__ENUMERATE(ColonColon)               \
	__ENUMERATE(Comma)                    \
	__ENUMERATE(Default)                  \
	__ENUMERATE(Dot)                      \
	__ENUMERATE(DotDot)                   \
	__ENUMERATE(DotDotDot)                \
	__ENUMERATE(Else)                     \
	__ENUMERATE(Eof)                      \
	__ENUMERATE(Equals)                   \
	__ENUMERATE(EqualsEquals)             \
	__ENUMERATE(EqualsGreater)            \
	__ENUMERATE(Exclamation)              \
	__ENUMERATE(ExclamationEquals)        \
	__ENUMERATE(False)                    \
	__ENUMERATE(For)                      \
	__ENUMERATE(Function)                 \
	__ENUMERATE(Glyph)                    \
	__ENUMERATE(Greater)                  \
	__ENUMERATE(GreaterEquals)            \
	__ENUMERATE(GreaterGreater)           \
	__ENUMERATE(GreaterGreaterEquals)     \
	__ENUMERATE(Identifier)               \
	__ENUMERATE(If)                       \
	__ENUMERATE(LeftBrace)                \
	__ENUMERATE(LeftBracket)              \
	__ENUMERATE(LeftParenthesis)          \
	__ENUMERATE(Less)                     \
	__ENUMERATE(LessEquals)               \
	__ENUMERATE(LessLess)                 \
	__ENUMERATE(LessLessEquals)           \
	__ENUMERATE(Match)                    \
	__ENUMERATE(Minus)                    \
	__ENUMERATE(MinusEquals)              \
	__ENUMERATE(MinusMinus)               \
	__ENUMERATE(Null)                     \
	__ENUMERATE(Number)                   \
	__ENUMERATE(Percent)                  \
	__ENUMERATE(PercentEquals)            \
	__ENUMERATE(Pipe)                     \
	__ENUMERATE(PipeEquals)               \
	__ENUMERATE(PipePipe)                 \
	__ENUMERATE(PipePipeEquals)           \
	__ENUMERATE(Plus)                     \
	__ENUMERATE(PlusEquals)               \
	__ENUMERATE(PlusPlus)                 \
	__ENUMERATE(Question)                 \
	__ENUMERATE(QuestionColon)            \
	__ENUMERATE(QuestionDot)              \
	__ENUMERATE(QuestionQuestion)         \
	__ENUMERATE(QuestionQuestionEquals)   \
	__ENUMERATE(Return)                   \
	__ENUMERATE(RightBrace)               \
	__ENUMERATE(RightBracket)             \
	__ENUMERATE(RightParenthesis)         \
	__ENUMERATE(Semicolon)                \
	__ENUMERATE(Slash)                    \
	__ENUMERATE(SlashEquals)              \
	__ENUMERATE(String)                   \
	__ENUMERATE(Tilde)                    \
	__ENUMERATE(True)                     \
	__ENUMERATE(Unknown)                  \
	__ENUMERATE(UnterminatedGlyph)        \
	__ENUMERATE(UnterminatedString)       \
	__ENUMERATE(Var)                      \
	__ENUMERATE(While)
