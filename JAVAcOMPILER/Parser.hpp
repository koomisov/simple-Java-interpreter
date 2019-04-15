#pragma once

#include <iostream>
#include "Scanner.hpp"



class Parser
{
public:

	Parser(std::string filename) : 
		scanner(filename) {}

	~Parser() {}

	void start();

private:
	Scanner scanner;
	Token current_token;

	Parser(const Parser &);
	Parser &operator=(const Parser &);

	void next_token()
	{
		current_token = scanner.get_token();
		while (current_token.get_type() == TOKEN_COMMENT)
			current_token = scanner.get_token();
		std::cout << current_token.get_value() << std::endl;
	}

	void _compilation_unit();//
	void  _main_declaration();//
	void _block(); //
	bool _block_statement();//
	void _variable_declaration(); //
	bool _statement();//
	void _branch(); // 
	void _while(); //
	void _assignment(); // 
	void _print_statement(); // 
	bool _expression(); // 
	bool _expression_OR(); // 
	bool _expression_AND(); //
	bool _expression_EQ(); // 
	bool _expression_CMP(); // 
	bool _expression_AS(); // 
	bool _expression_MD(); // 
	bool _expression_NOT(); // 
	bool _expression_PREF(); // 
	bool _operation_OR(bool); // 
	bool _operation_AND(bool);//
	bool _operation_EQ(bool); // 
	bool _operation_CMP(bool); // 
	bool _operation_AS(bool); // 
	bool _operation_MD(bool); // 
	bool _operation_PREF(); // 
	bool _operation_POST(bool); // 
	bool _primary(); // 
};



