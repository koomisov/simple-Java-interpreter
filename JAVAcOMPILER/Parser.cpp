#include "pch.h"
#include <unordered_set>
#include "Parser.hpp"

void Parser::start()
{
	next_token();
	_compilation_unit();
}

bool Parser::_primary()
{
	if (current_token.get_type() == TOKEN_NUM)
	{
		//
		next_token();

		return true;
	}
	else if (current_token.get_type() == TOKEN_ID)
	{
		//
		next_token();
	
		return true;
	}
	else if (current_token.get_type() != TOKEN_LPAREN)
		throw Error("Syntax error: an expression in parentheses, number or identifier expected", SYNT_ANALYSIS, scanner.get_line());

	next_token();
	bool tmp = _expression();

	if (current_token.get_type() != TOKEN_RPAREN)
			throw Error("Syntax error: delim <)> expected", SYNT_ANALYSIS, scanner.get_line());
		next_token();
	
	
	return tmp;
}

bool Parser::_operation_POST(bool)
{
	if (current_token.get_type() == TOKEN_INC)
	{
		next_token();

		return true;
	}
	else if (current_token.get_type() == TOKEN_DEC)
	{
		next_token();
		
		return true;
	}

	return false;
}

bool Parser::_operation_PREF()
{
	if (current_token.get_type() == TOKEN_PLUS)
		return true;
	if (current_token.get_type() == TOKEN_MINUS)
		return true;
	if (current_token.get_type() == TOKEN_INC)
		return  true;
	if (current_token.get_type() == TOKEN_DEC)
		return  true;

	return false;
}

bool Parser::_operation_MD(bool first_arg)
{
	if (!first_arg)
		throw Error("Syntax error: empty argument (operation_MD)", SYNT_ANALYSIS, scanner.get_line());

	if (current_token.get_type() == TOKEN_MUL)
	{
		next_token();
		
		bool second_arg = _expression_MD();

		bool ptr = _operation_MD(second_arg);		
		if (ptr)
			return ptr;

	}
	else if (current_token.get_type() == TOKEN_DIV)
	{
		next_token();

		bool second_arg = _expression_MD();

		bool ptr = _operation_MD(second_arg);
		if (ptr)
			return ptr;

	}
	else if (current_token.get_type() == TOKEN_MOD)
	{
		next_token();

		bool second_arg = _expression_MD();

		bool ptr = _operation_MD(second_arg);
		if (ptr)
			return ptr;
	}
	return false;
}

bool Parser::_operation_CMP(bool first_arg)
{
	if (!first_arg)
		throw Error("Syntax error: empty argument (operation_CMP)", SYNT_ANALYSIS, scanner.get_line());

	if (current_token.get_type() == TOKEN_LOEQ)
	{
		next_token();
		
		bool second_arg = _expression_CMP();

		bool ptr = _operation_CMP(second_arg);

		if (ptr)
			return ptr;

	}
	else if (current_token.get_type() == TOKEN_GOEQ)
	{
		next_token();

		bool second_arg = _expression_CMP();

		bool ptr = _operation_CMP(second_arg);

		if (ptr)
			return ptr;

	}
	else if (current_token.get_type() == TOKEN_LEQ)
	{
		next_token();

		bool second_arg = _expression_CMP();

		bool ptr = _operation_CMP(second_arg);
		if (ptr)
			return ptr;
	}
	else if (current_token.get_type() == TOKEN_GEQ)
	{
		next_token();
		bool second_arg = _expression_CMP();

		bool ptr = _operation_CMP(second_arg);

		if (ptr)
			return ptr;
	}

	return false;
}

bool Parser::_operation_AS(bool first_arg)
{
	if (!first_arg)
		throw Error("Syntax error: empty argument (operation_AS)", SYNT_ANALYSIS, scanner.get_line());

	if (current_token.get_type() == TOKEN_PLUS)
	{
		next_token();
		
		bool second_arg = _expression_AS();
		bool ptr = _operation_AS(second_arg);

		if (ptr)
			return ptr;

	}
	else if (current_token.get_type() == TOKEN_MINUS)
	{
		next_token();

		bool second_arg = _expression_AS();
		bool ptr = _operation_AS(second_arg);

		if (ptr)
			return ptr;
	}

	return false;
}

bool Parser::_operation_EQ(bool first_arg)
{
	if (!first_arg)
		throw Error("Syntax error: empty argument (operation_EQ)", SYNT_ANALYSIS, scanner.get_line());

	if (current_token.get_type() == TOKEN_EQ) 
	{
		next_token();
		
		bool second_arg = _expression_EQ();
		
		bool ptr = _operation_EQ(second_arg);

		if (ptr)
			return ptr;
	}
	else if (current_token.get_type() == TOKEN_NEQ)
	{
		next_token();

		bool second_arg = _expression_EQ();

		bool ptr = _operation_EQ(second_arg);

		if (ptr)
			return ptr;
	}

	return false;
}

bool Parser::_operation_AND(bool first_arg)
{
	if (first_arg == false)
		throw Error("Syntax error: empty argument (operation <&&>)", SYNT_ANALYSIS, scanner.get_line());
	if (current_token.get_type() == TOKEN_AND)
	{
		next_token();

		bool second_arg = _expression_AND();

		bool ptr = _operation_AND(second_arg);

		if (ptr)
			return ptr;
	}
	return false;
}

bool Parser::_operation_OR(bool first_arg)
{
	if (!first_arg)
		throw Error("Syntax error: empty argument (operation <||>)", SYNT_ANALYSIS, scanner.get_line());

	if (current_token.get_type() == TOKEN_OR)
	{
		next_token();

		bool second_arg = _expression_OR();

		bool ptr = _operation_OR(second_arg);

		if (ptr)
			return ptr;
	}

	return false;
}

bool Parser::_expression_PREF()
{
	bool arg = _primary();
	bool post;

	post = _operation_POST(arg);
	if (post)
		return post;


	return arg;
}

bool Parser::_expression_NOT()
{
	bool arg{}, tmp{};
	std::unordered_set<token_type> tokens = { TOKEN_PLUS, TOKEN_MINUS, TOKEN_INC, TOKEN_DEC };

	if (tokens.count(current_token.get_type()) != 0)
	{
		tmp = _operation_PREF();
		next_token();
	}
	arg = _expression_PREF();
	if (tmp)
	{
		
		return tmp;
	}

	return arg;
}

bool Parser::_expression_MD()
{
	bool arg{}, tmp{};

	if (current_token.get_type() == TOKEN_NOT)
	{
		tmp = true;
		next_token();
		while (current_token.get_type() == TOKEN_NOT)
		{
			next_token();
		}
	}
	arg = _expression_NOT();
	if (tmp)
	{		
		return tmp;
	}

	return arg;
}

bool Parser::_expression_AS()
{
	bool  first_arg = _expression_MD();
	bool tmp;

	tmp = _operation_MD(first_arg);
	if (tmp)
	{
		return tmp;
	}

	return first_arg;
}

bool Parser::_expression_CMP()
{
	bool first_arg = _expression_AS();
	bool tmp;

	tmp = _operation_AS(first_arg);
	if (tmp)
		return tmp;

	return first_arg;
}

bool Parser::_expression_EQ()
{
	bool first_arg = _expression_CMP();
	bool tmp;

	tmp = _operation_CMP(first_arg);
	if (tmp)
	{
		return tmp;
	}

	return first_arg;
}

bool Parser::_expression_AND()
{
	bool first_arg = _expression_EQ();
	bool tmp;

	tmp = _operation_EQ(first_arg);
	if (tmp)
	{
		return tmp;
	}

	return first_arg;
}

bool Parser::_expression_OR()
{
	bool first_arg = _expression_AND();
	bool tmp;

	tmp = _operation_AND(first_arg);
	if (tmp)
	{
		return tmp;
	}

	return first_arg;
}

bool Parser::_expression()
{
	bool first_arg = _expression_OR();

	bool tmp = _operation_OR(first_arg);
	if (tmp)
	{
		return tmp;
	}

	return first_arg;
}



// -----------------------------------------------------------------------------------------------------------------



void Parser::_branch()
{
	next_token();
	if (current_token.get_type() != TOKEN_LPAREN)
		throw Error("Syntax error: delim <(> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();


	_expression();
	if (current_token.get_type() != TOKEN_RPAREN)
		throw Error("Syntax error: delim <)> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();

	_statement();

	if (current_token.get_type() == TOKEN_ELSE)
	{
		next_token();
	}
}

void Parser::_while()
{
	next_token();
	if (current_token.get_type() != TOKEN_LPAREN)
		throw Error("Syntax error: delim <(> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();

	_expression();
	if (current_token.get_type() != TOKEN_RPAREN)
		throw Error("Syntax error: delim <)> expected", SYNT_ANALYSIS, scanner.get_line());

	next_token();

	_statement();

}

void Parser::_assignment()
{
		next_token();
		if (current_token.get_type() != TOKEN_ASSIGN)
			throw Error("Syntax error: identifier expected", SYNT_ANALYSIS, scanner.get_line());
		next_token();
		_expression();
		if (current_token.get_type() != TOKEN_SEMICOL)
			throw Error("Syntax error: delim <;> expected", SYNT_ANALYSIS, scanner.get_line());
		next_token();
}

void Parser::_print_statement()
{
	next_token();
	if (current_token.get_type() != TOKEN_DOT)
		throw Error("Syntax error: delim <.> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_OUT)
		throw Error("Syntax error: keyword <out> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_DOT)
		throw Error("Syntax error: delim <.> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_PRINTL)
		throw Error("Syntax error: keyword <println> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_LPAREN)
		throw Error("Syntax error: delim <(> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_STR)
		throw Error("Syntax error: string expected", SYNT_ANALYSIS, scanner.get_line());


	next_token();
	if (current_token.get_type() != TOKEN_RPAREN)
		throw Error("Syntax error: delim <)> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_SEMICOL)
		throw Error("Syntax error: delim <;> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();

}

bool Parser::_statement()
{
	token_type tmp = current_token.get_type();
	switch (tmp)
	{
	case TOKEN_SEMICOL:
		next_token();
		return true;
	
	case TOKEN_IF:
		_branch();
		return true;

	case TOKEN_WHILE:
		_while();
		return true;

	case TOKEN_ID:
		 _assignment();
		 return true;

	case TOKEN_SYS:
		 _print_statement();
		 return true;

	case TOKEN_LBLOCK:
		 _block();
		return true;

	default:
		return false;
	}
}

void Parser::_variable_declaration()
{
		next_token();
		if (current_token.get_type() != TOKEN_ID)
			throw Error("Syntax error: identifier expected", SYNT_ANALYSIS, scanner.get_line());
		
		next_token();
		while (current_token.get_type() == TOKEN_COMMA)
		{
			next_token();
			if (current_token.get_type() != TOKEN_ID)
				throw Error("Syntax error: identifier expected", SYNT_ANALYSIS, scanner.get_line());

			next_token();
		}
		if (current_token.get_type() != TOKEN_SEMICOL)
			throw Error("Syntax error: delim <;> expected", SYNT_ANALYSIS, scanner.get_line());
		next_token();

}

bool Parser::_block_statement()
{
	if ((current_token.get_type() == TOKEN_INT) || (current_token.get_type() == TOKEN_BOOL))
	{
		_variable_declaration();
		return true;
	}
	else
		return _statement();
}

void Parser::_block()
{
	if (current_token.get_type() != TOKEN_LBLOCK)
		throw Error("Syntax error: delim <{> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();


	bool stat = _block_statement();
	while (stat)
	{
		stat = _block_statement();
	}		

	if (current_token.get_type() != TOKEN_RBLOCK)
		throw Error("Syntax error: missing delim <}> at the end of the block", SYNT_ANALYSIS, scanner.get_line());

	next_token();

}

void Parser::_main_declaration()
{
	if (current_token.get_type() != TOKEN_PUBL)
		throw Error("Syntax error: keyword <public> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_STATIC)
		throw Error("Syntax error: keyword <static> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_VOID)
		throw Error("Syntax error: keyword <void> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_MAIN)
		throw Error("Syntax error: function name <main> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_LPAREN)
		throw Error("Syntax error: delim <(> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_RPAREN)
		throw Error("Syntax error: delim <)> expected", SYNT_ANALYSIS, scanner.get_line());

	next_token();


	_block();

}

void Parser::_compilation_unit()
{
	if (current_token.get_type() != TOKEN_PUBL)
		throw Error("Syntax error: keyword <public> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_CLASS)
		throw Error("Syntax error: keyword <class> expected", SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_ID)
		throw Error("Syntax error: class identifier expected", SYNT_ANALYSIS, scanner.get_line());

	next_token();
	if (current_token.get_type() != TOKEN_LBLOCK)
		throw Error("Syntax error: delim <{> expected", SYNT_ANALYSIS, scanner.get_line());

	next_token();
	_main_declaration();

	if (current_token.get_type() != TOKEN_RBLOCK)
		throw Error("Syntax error: missing delim <}> at the end of the block", SYNT_ANALYSIS, scanner.get_line());

}
