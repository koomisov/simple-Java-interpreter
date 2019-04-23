#pragma once


#include "InvertNotation.hpp"
#include <stack>
#include <queue>
#include <iostream>
#include <variant>

class Program
{
	using shPtr = std::shared_ptr<TokenItem>;
	std::vector<shPtr> poliz;

	std::unordered_map<std::string, var_type> nameToType;
	std::unordered_map<std::string, unsigned int> nameToIndex;

	std::vector<std::string> stringStorage;
	std::vector<int> integerStorage;
	std::stack<shPtr> operandsStack;
	std::vector<shPtr> temporary;

	struct printingItem {
		printingItem(var_type t) : variableType(t) {}
		var_type variableType;
		std::variant<std::string, int> data;
		template<typename T> void set_data(T const& d) { data = d; }
	};

	std::queue<std::shared_ptr<printingItem>> printQueue;

	unsigned int pIndex = 0;

	void executeOutput();

	void* get_variable_address(std::string const& varName);

public:

	Program(std::vector<shPtr> const& vec, std::unordered_map<std::string, var_type> const &hash)
	{
		poliz = vec;
		nameToType = hash;

		unsigned int stringCount = 0, integerCount = 0;
		for (auto const& Pair : hash)
		{
			if (Pair.second == var_type::STRING)
			{
				nameToIndex[Pair.first] = stringCount;
				++stringCount;
			}
			else
			{
				nameToIndex[Pair.first] = integerCount;
				++integerCount;
			}
		}

		stringStorage.resize(stringCount);
		integerStorage.resize(integerCount);
	}


	void run(); 
	void set_identifier_value(Operand*);

	void execute(EndOfTheProgram const&); 
	void execute(AssignmentOperation const&); 
	void execute(BinaryLogicalOperation const&);
	void execute(UnaryLogicalOperation const&);
	void execute(BinaryArithmeticOperation const&);
	void execute(PrefArithmeticOperation const&);
	void execute(PostArithmeticOperation const&);
	void execute(ComparisonOperation const& );
	void execute(PrintOperation const&);
	void execute(IfBlock const&);
	void execute(WhileBlock const&);
	void execute(Jump const&);

};


