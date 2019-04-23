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
		int num = std::stoi(current_token.get_value());
		poliz.add(std::make_shared<Number>(num));
		next_token();

		return true;
	}
	else if (current_token.get_type() == TOKEN_ID)
	{
		if (poliz.is_variable_declared(current_token.get_value()))
		{
			var_type curVarType = poliz.get_var_type(current_token.get_value());
			poliz.add(std::make_shared<Identifier>(curVarType, current_token.get_value()));
		}
		else
			throw Error("Logical error : variable is not declared", step_type::SEM_ANALYSIS, scanner.get_line());

		next_token();
		return true;
	}
	else if (current_token.get_type() != TOKEN_LPAREN)
		throw Error("Syntax error: an expression in parentheses, number or identifier expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	bool tmp = _expression();

	if (current_token.get_type() != TOKEN_RPAREN)
			throw Error("Syntax error: delim <)> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
		next_token();
	
	
	return tmp;
}

item_type Parser::_operation_POST(bool)
{
	if (current_token.get_type() == TOKEN_INC)
	{
		next_token();

		return item_type::POST_INC;
	}
	else if (current_token.get_type() == TOKEN_DEC)
	{
		next_token();
		
		return item_type::POST_DEC;
	}

	return item_type::EMPTY;
}

item_type Parser::_operation_PREF()
{
	token_type t = current_token.get_type();

	if (t == TOKEN_INC)
		return item_type::PREF_INC;
	else if (t == TOKEN_DEC)
		return item_type::PREF_DEC;
	else
		return item_type::EMPTY;
}

bool Parser::_operation_MD(bool first_arg)
{
	if (!first_arg)
		throw Error("Syntax error: empty argument (operation_MD)", step_type::SYNT_ANALYSIS, scanner.get_line());

	if (current_token.get_type() == TOKEN_MUL)
	{
		next_token();
		
		bool second_arg = _expression_MD();

		poliz.add(std::make_shared<BinaryArithmeticOperation>(item_type::MUL));

		bool ptr = _operation_MD(second_arg);		
		if (ptr)
			return ptr;

	}
	else if (current_token.get_type() == TOKEN_DIV)
	{
		next_token();

		bool second_arg = _expression_MD();

		poliz.add(std::make_shared<BinaryArithmeticOperation>(item_type::DIV));

		bool ptr = _operation_MD(second_arg);
		if (ptr)
			return ptr;

	}
	else if (current_token.get_type() == TOKEN_MOD)
	{
		next_token();

		bool second_arg = _expression_MD();

		poliz.add(std::make_shared<BinaryArithmeticOperation>(item_type::MOD));

		bool ptr = _operation_MD(second_arg);
		if (ptr)
			return ptr;
	}
	return false;
}

bool Parser::_operation_CMP(bool first_arg)
{
	if (!first_arg)
		throw Error("Syntax error: empty argument (operation_CMP)", step_type::SYNT_ANALYSIS, scanner.get_line());

	if (current_token.get_type() == TOKEN_LOEQ)
	{
		next_token();
		
		bool second_arg = _expression_CMP();

		poliz.add(std::make_shared<ComparisonOperation>(item_type::LOEQ));

		bool ptr = _operation_CMP(second_arg);

		if (ptr)
			return ptr;

	}
	else if (current_token.get_type() == TOKEN_GOEQ)
	{
		next_token();

		bool second_arg = _expression_CMP();

		poliz.add(std::make_shared<ComparisonOperation>(item_type::GOEQ));

		bool ptr = _operation_CMP(second_arg);

		if (ptr)
			return ptr;

	}
	else if (current_token.get_type() == TOKEN_LEQ)
	{
		next_token();

		bool second_arg = _expression_CMP();

		poliz.add(std::make_shared<ComparisonOperation>(item_type::LEQ));

		bool ptr = _operation_CMP(second_arg);
		if (ptr)
			return ptr;
	}
	else if (current_token.get_type() == TOKEN_GEQ)
	{
		next_token();
		bool second_arg = _expression_CMP();

		poliz.add(std::make_shared<ComparisonOperation>(item_type::GEQ));

		bool ptr = _operation_CMP(second_arg);

		if (ptr)
			return ptr;
	}

	return false;
}

bool Parser::_operation_AS(bool first_arg)
{
	if (!first_arg)
		throw Error("Syntax error: empty argument (operation_AS)", step_type::SYNT_ANALYSIS, scanner.get_line());

	if (current_token.get_type() == TOKEN_PLUS)
	{
		next_token();
		
		bool second_arg = _expression_AS();

		poliz.add(std::make_shared<BinaryArithmeticOperation>(item_type::PLUS));

		bool ptr = _operation_AS(second_arg);

		if (ptr)
			return ptr;

	}
	else if (current_token.get_type() == TOKEN_MINUS)
	{
		next_token();

		bool second_arg = _expression_AS();

		poliz.add(std::make_shared<BinaryArithmeticOperation>(item_type::MINUS));

		bool ptr = _operation_AS(second_arg);

		if (ptr)
			return ptr;
	}

	return false;
}

bool Parser::_operation_EQ(bool first_arg)
{
	if (!first_arg)
		throw Error("Syntax error: empty argument (operation_EQ)", step_type::SYNT_ANALYSIS, scanner.get_line());

	if (current_token.get_type() == TOKEN_EQ) 
	{
		next_token();
		
		bool second_arg = _expression_EQ();
		
		poliz.add(std::make_shared<ComparisonOperation>(item_type::EQ));

		bool ptr = _operation_EQ(second_arg);

		if (ptr)
			return ptr;
	}
	else if (current_token.get_type() == TOKEN_NEQ)
	{
		next_token();

		bool second_arg = _expression_EQ();

		poliz.add(std::make_shared<ComparisonOperation>(item_type::NEQ));

		bool ptr = _operation_EQ(second_arg);

		if (ptr)
			return ptr;
	}

	return false;
}

bool Parser::_operation_AND(bool first_arg)
{
	if (first_arg == false)
		throw Error("Syntax error: empty argument (operation <&&>)", step_type::SYNT_ANALYSIS, scanner.get_line());
	if (current_token.get_type() == TOKEN_AND)
	{
		next_token();

		bool second_arg = _expression_AND();

		poliz.add(std::make_shared<BinaryLogicalOperation>(item_type::AND));

		bool ptr = _operation_AND(second_arg);

		if (ptr)
			return ptr;
	}
	return false;
}

bool Parser::_operation_OR(bool first_arg)
{
	if (!first_arg)
		throw Error("Syntax error: empty argument (operation <||>)", step_type::SYNT_ANALYSIS, scanner.get_line());

	if (current_token.get_type() == TOKEN_OR)
	{
		next_token();

		bool second_arg = _expression_OR();

		poliz.add(std::make_shared<BinaryLogicalOperation>(item_type::OR));

		bool ptr = _operation_OR(second_arg);

		if (ptr)
			return ptr;
	}

	return false;
}

bool Parser::_expression_PREF()
{
	bool arg = _primary();

	item_type post = _operation_POST(arg);

	if (post != item_type::EMPTY)
	{
		poliz.add(std::make_shared<PostArithmeticOperation>(post));
		return true;
	}

	return arg;
}

bool Parser::_expression_NOT()
{
	bool arg = false;
	
	item_type tmp = item_type::EMPTY;

	std::unordered_set<token_type> tokens = { TOKEN_PLUS, TOKEN_MINUS, TOKEN_INC, TOKEN_DEC };

	if (tokens.count(current_token.get_type()) != 0)
	{
		tmp = _operation_PREF();
		next_token();
	}
	arg = _expression_PREF();

	if (tmp != item_type::EMPTY)
	{
		poliz.add(std::make_shared<PrefArithmeticOperation>(tmp));
		return true;
	}

	return arg;
}

bool Parser::_expression_MD()
{
	bool arg = false, tmp = false;

	if (current_token.get_type() == TOKEN_NOT)
	{
		tmp = true;
		next_token();
	}

	arg = _expression_NOT();

	if (tmp)
	{		
		poliz.add(std::make_shared<UnaryLogicalOperation>(item_type::NOT));
		return tmp;
	}

	return arg;
}

bool Parser::_expression_AS()
{
	bool  first_arg = _expression_MD();

	_operation_MD(first_arg);

	return first_arg;
}

bool Parser::_expression_CMP()
{
	bool first_arg = _expression_AS();

	_operation_AS(first_arg);

	return first_arg;
}

bool Parser::_expression_EQ()
{
	bool first_arg = _expression_CMP();

	 _operation_CMP(first_arg);

	return first_arg;
}

bool Parser::_expression_AND()
{
	bool first_arg = _expression_EQ();
	_operation_EQ(first_arg);

	return first_arg;
}

bool Parser::_expression_OR()
{
	bool first_arg = _expression_AND();
	 _operation_AND(first_arg);


	return first_arg;
}

bool Parser::_expression()
{
	bool first_arg = _expression_OR();

	_operation_OR(first_arg);

	return first_arg;
}



// -----------------------------------------------------------------------------------------------------------------



void Parser::_branch()
{
	next_token();
	if (current_token.get_type() != TOKEN_LPAREN)
		throw Error("Syntax error: delim <(> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	_expression();

	auto ifblock = std::make_shared<IfBlock>();
	poliz.add(ifblock);

	if (current_token.get_type() != TOKEN_RPAREN)
		throw Error("Syntax error: delim <)> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	_statement();


	if (current_token.get_type() == TOKEN_ELSE)
	{
		auto label = std::make_shared<Jump>();
		poliz.add(label);

		ifblock->set_jump_index(poliz.size());

		next_token();
		_statement();

		label->set_jump_index(poliz.size());
	}
	else
		ifblock->set_jump_index(poliz.size());
}

void Parser::_while()
{
	next_token();
	if (current_token.get_type() != TOKEN_LPAREN)
		throw Error("Syntax error: delim <(> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	unsigned int savedIndex = poliz.size();
	_expression();
	
	auto whileBlock = std::make_shared<WhileBlock>();
	poliz.add(whileBlock);

	if (current_token.get_type() != TOKEN_RPAREN)
		throw Error("Syntax error: delim <)> expected", step_type::SYNT_ANALYSIS, scanner.get_line());

	next_token();

	_statement();
	
	auto push_ptr = std::make_shared<Jump>();
	push_ptr->set_jump_index(savedIndex);

	poliz.add(push_ptr);

	whileBlock->set_jump_index(poliz.size());

}


void Parser::_assignment()
{
	if (poliz.is_variable_declared(current_token.get_value()) == false)
		throw Error("Logical error: variable is not declared", step_type::SEM_ANALYSIS, scanner.get_line());
	poliz.add(std::make_shared<Identifier>(poliz.get_var_type(current_token.get_value()), current_token.get_value()));

	next_token();

	if (current_token.get_type() != TOKEN_ASSIGN)
		throw Error("Syntax error: identifier expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	if (current_token.get_type() == TOKEN_TRUE || current_token.get_type() == TOKEN_FALSE)
	{
		bool val = current_token.get_type() == TOKEN_TRUE ? true : false;
		poliz.add(std::make_shared<LogicalConstant>(val));
		next_token();
	}
	else if (current_token.get_type() == TOKEN_STR)
	{
		poliz.add(std::make_shared<ConstCharString>(current_token.get_value()));
		next_token();
	}
	else
		_expression();


	if (current_token.get_type() != TOKEN_SEMICOL)
		throw Error("Syntax error: delim <;> Eexpected", step_type::SYNT_ANALYSIS, scanner.get_line());

	poliz.add(std::make_shared<AssignmentOperation>());

	next_token();
}

void Parser::_print_statement()
{
	next_token();

	if (current_token.get_type() != TOKEN_DOT)
		throw Error("Syntax error: delim <.> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	if (current_token.get_type() != TOKEN_OUT)
		throw Error("Syntax error: keyword <out> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	if (current_token.get_type() != TOKEN_DOT)
		throw Error("Syntax error: delim <.> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	if (current_token.get_type() != TOKEN_PRINTL)
		throw Error("Syntax error: keyword <println> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	if (current_token.get_type() != TOKEN_LPAREN)
		throw Error("Syntax error: delim <(> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	if (current_token.get_type() != TOKEN_STR && current_token.get_type() != TOKEN_ID)
		throw Error("Syntax error: string expected", step_type::SYNT_ANALYSIS, scanner.get_line());

	
	if (current_token.get_type() == TOKEN_STR)
		poliz.add(std::make_shared<ConstCharString>(current_token.get_value()));
	else
	{
		if (poliz.is_variable_declared(current_token.get_value()))
		{
			var_type curVarType = poliz.get_var_type(current_token.get_value());
			poliz.add(std::make_shared<Identifier>(curVarType, current_token.get_value()));
		}
		else
			throw Error("Logical error : variable is not declared", step_type::SEM_ANALYSIS, scanner.get_line());

	}

	poliz.add(std::make_shared<PrintOperation>());

	//
	next_token();
	if (current_token.get_type() != TOKEN_RPAREN)
		throw Error("Syntax error: delim <)> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

	if (current_token.get_type() != TOKEN_SEMICOL)
		throw Error("Syntax error: delim <;> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
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

	case TOKEN_INC:
	case TOKEN_DEC:
		_expression();
		if (current_token.get_type() != TOKEN_SEMICOL)
			throw Error("Synt er");
		next_token();
		return true;

	default:
		return false;
	}
}

void Parser::_variable_declaration()
{
	var_type curVarType;
	switch (current_token.get_type())
	{
	case TOKEN_INT:
		curVarType = var_type::INT;
		break;
	case TOKEN_BOOL: 
		curVarType = var_type::BOOLEAN;
		break;
	case TOKEN_STRING: 
		curVarType = var_type::STRING;
		break;
	}

	next_token();
	if (current_token.get_type() != TOKEN_ID)
		throw Error("Syntax error: identifier expected", step_type::SYNT_ANALYSIS, scanner.get_line());

	if (poliz.is_variable_declared(current_token.get_value()) == false)
		poliz.declare_variable(curVarType, current_token.get_value());
	else
		throw Error("Logical error : variable has already been declared", step_type::SEM_ANALYSIS, scanner.get_line());

	next_token();
	while (current_token.get_type() == TOKEN_COMMA)
	{
		next_token();
		if (current_token.get_type() != TOKEN_ID)
			throw Error("Syntax error: identifier expected", step_type::SYNT_ANALYSIS, scanner.get_line());

		if (poliz.is_variable_declared(current_token.get_value()) == false)
			poliz.declare_variable(curVarType, current_token.get_value());
		else
			throw Error("Logical error : variable has already been declared", step_type::SEM_ANALYSIS, scanner.get_line());

		next_token();
	}

	if (current_token.get_type() != TOKEN_SEMICOL)
		throw Error("Syntax error: delim <;> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();

}

bool Parser::_block_statement()
{
	token_type type = current_token.get_type();
	if (type == TOKEN_INT || type == TOKEN_BOOL || type == TOKEN_STRING)
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
		throw Error("Syntax error: delim <{> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();


	bool stat = _block_statement();
	while (stat)
	{
		stat = _block_statement();
	}		

	if (current_token.get_type() != TOKEN_RBLOCK)
		throw Error("Syntax error: missing delim <}> at the end of the block", step_type::SYNT_ANALYSIS, scanner.get_line());

	next_token();

}

void Parser::_main_declaration()
{
	if (current_token.get_type() != TOKEN_PUBL)
		throw Error("Syntax error: keyword <public> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_STATIC)
		throw Error("Syntax error: keyword <static> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_VOID)
		throw Error("Syntax error: keyword <void> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_MAIN)
		throw Error("Syntax error: function name <main> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_LPAREN)
		throw Error("Syntax error: delim <(> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_RPAREN)
		throw Error("Syntax error: delim <)> expected", step_type::SYNT_ANALYSIS, scanner.get_line());

	next_token();


	_block();

}

void Parser::_compilation_unit()
{
	if (current_token.get_type() != TOKEN_PUBL)
		throw Error("Syntax error: keyword <public> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_CLASS)
		throw Error("Syntax error: keyword <class> expected", step_type::SYNT_ANALYSIS, scanner.get_line());
	next_token();
	if (current_token.get_type() != TOKEN_ID)
		throw Error("Syntax error: class identifier expected", step_type::SYNT_ANALYSIS, scanner.get_line());

	next_token();
	if (current_token.get_type() != TOKEN_LBLOCK)
		throw Error("Syntax error: delim <{> expected", step_type::SYNT_ANALYSIS, scanner.get_line());

	next_token();
	_main_declaration();

	if (current_token.get_type() != TOKEN_RBLOCK)
		throw Error("Syntax error: missing delim <}> at the end of the block", step_type::SYNT_ANALYSIS, scanner.get_line());

	poliz.add(std::make_shared<EndOfTheProgram>());

}
