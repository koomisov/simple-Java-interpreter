#pragma once

#include <vector>
#include <string>

enum token_type
{
	TOKEN_NULL,
	TOKEN_PUBL,
	TOKEN_CLASS,
	TOKEN_STATIC,
	TOKEN_VOID,
	TOKEN_MAIN,
	TOKEN_IF,
	TOKEN_ELSE,
	TOKEN_WHILE,
	TOKEN_SYS,
	TOKEN_OUT,
	TOKEN_PRINTL,
	TOKEN_INT,
	TOKEN_BOOL,
	TOKEN_LBLOCK,
	TOKEN_RBLOCK,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_SEMICOL,
	TOKEN_COMMA,
	TOKEN_DOT,
	TOKEN_INC,
	TOKEN_DEC,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_MOD,
	TOKEN_LOEQ,
	TOKEN_GOEQ,
	TOKEN_LEQ,
	TOKEN_GEQ,
	TOKEN_EQ,
	TOKEN_NEQ,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_NOT,
	TOKEN_ASSIGN,
	TOKEN_NUM,
	TOKEN_ID,
	TOKEN_COMMENT,
	TOKEN_STR,
	TOKEN_FIN
};

class Token
{

private:

	token_type type;
	std::string value;

public:

	using value_type = std::string;

	Token(token_type t = TOKEN_NULL, value_type val = value_type() ) 
		: type(t), value(val) {}

	token_type get_type()  const {
		return type; 
	}

	value_type get_value() const {
		return value; 
	}

};
