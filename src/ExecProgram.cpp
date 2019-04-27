#include "pch.h"
#include "ExecProgram.hpp"
#include "Error.hpp"

#include <iostream>


// --------------------------------------- STUFF ---------------------------------------------

void Program::set_identifier_value(Operand* oper)
{
	auto ident = dynamic_cast<Identifier*>(oper);

	if (ident->get_variable_type() == var_type::STRING)
		ident->set_string_value(stringStorage[nameToIndex[ident->get_name()]]);
	else
		ident->set_integral_value(integerStorage[nameToIndex[ident->get_name()]]);
}

void* Program::get_variable_address(std::string const& varName)
{
	auto pos = nameToIndex[varName];

	if (nameToType[varName] == var_type::STRING)
		return std::addressof(stringStorage[pos]);
	else
		return std::addressof(integerStorage[pos]);
}

void Program::executeOutput()
{
	while (!printQueue.empty())
	{
		auto ptr = printQueue.front();
		if (ptr->variableType == var_type::STRING)
			std::cout << std::get<std::string>(ptr->data);
		else if (ptr->variableType == var_type::BOOLEAN)
		{
			auto val = std::get<int>(ptr->data);
			if (val)
				std::cout << "true";
			else
				std::cout << "false";
		}
		else
			std::cout << std::get<int>(ptr->data);

		printQueue.pop();
	}
}

// ----------------------------------------- FUNC FOR PROGRAM EXECUTING -------------------------------------------

void Program::run()
{
	
	while(pIndex < poliz.size())
	{
		if (poliz[pIndex]->executable())
			poliz[pIndex]->execute(this);
		else
			operandsStack.push(poliz[pIndex]);

		++pIndex;
	}
}


void Program::execute(EndOfTheProgram const&)
{
	executeOutput();
}

void Program::execute(AssignmentOperation const&) 
{

	auto value = dynamic_cast<Operand*>(operandsStack.top().get());
	if (value->is_identifier())
		set_identifier_value(value);

	operandsStack.pop();

	auto variable = dynamic_cast<Identifier*>(operandsStack.top().get());
	operandsStack.pop();


	if (variable == nullptr || value == nullptr)
		throw Error("Semantic error : invalid arguments for assignment", step_type::SEM_ANALYSIS);

	if (variable->get_variable_type() == var_type::STRING)
	{
		if(value->get_variable_type() != var_type::STRING)
			throw Error("Semantic error : invalid operand for string assignment", step_type::SEM_ANALYSIS);

		auto varAddr = static_cast<std::string*>(this->get_variable_address(variable->get_name()));

		(*varAddr) = value->get_string_value();
	}
	else if (variable->get_variable_type() == var_type::INT)
	{
		if(value->get_variable_type() == var_type::STRING || value->get_variable_type() == var_type::BOOLEAN)
			throw Error("Semantic error : invalid operand for integer assignment", step_type::SEM_ANALYSIS);

		auto varAddr = static_cast<int*>(this->get_variable_address(variable->get_name()));

		(*varAddr) = value->get_integral_value();
	}
	else if (variable->get_variable_type() == var_type::BOOLEAN)
	{
		if(value->get_variable_type() == var_type::STRING)
			throw Error("Semantic error : invalid operand for boolean assignment", step_type::SEM_ANALYSIS);

		auto varAddr = static_cast<int*>(this->get_variable_address(variable->get_name()));

		(*varAddr) = value->get_integral_value() == 0 ? 0 : 1;
	}

}

void Program::execute(BinaryLogicalOperation const& operation)
{
	auto secondOperand = dynamic_cast<Operand*>(operandsStack.top().get());
	if (secondOperand->is_identifier())
		set_identifier_value(secondOperand);
	operandsStack.pop();

	auto firstOperand = dynamic_cast<Operand*>(operandsStack.top().get());
	if (firstOperand->is_identifier())
		set_identifier_value(firstOperand);
	operandsStack.pop();

	auto ft = firstOperand->get_variable_type(), st = secondOperand->get_variable_type();
	if (!firstOperand || !secondOperand || ft== var_type::STRING || st == var_type::STRING)
		throw Error("Invalid arguments for binary logical operation", step_type::SEM_ANALYSIS);
	

	std::shared_ptr<LogicalConstant> ptr;
	switch (operation.get_type())
	{
	case item_type::AND:
		ptr = std::make_shared<LogicalConstant>(firstOperand->get_integral_value() && secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;

	case item_type::OR:
		ptr = std::make_shared<LogicalConstant>(firstOperand->get_integral_value() || secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;
	}

	temporary.push_back(ptr);
}

void Program::execute(UnaryLogicalOperation const&)
{
	auto operand = dynamic_cast<Operand*>(operandsStack.top().get());
	if (operand->is_identifier())
		set_identifier_value(operand);
	operandsStack.pop();

	if (operand->get_variable_type() == var_type::STRING)
		throw Error("Semantic error : invalid argument for <!> operation", step_type::SEM_ANALYSIS);

	int push_value = operand->get_integral_value() == 0 ? 0 : 1;

	auto ptr = std::make_shared<LogicalConstant>(push_value);
	operandsStack.push(ptr);
	temporary.push_back(ptr);
	
}

void Program::execute(BinaryArithmeticOperation const& operation)
{
	auto secondOperand = dynamic_cast<Operand*>(operandsStack.top().get());
	if (secondOperand->is_identifier())
		set_identifier_value(secondOperand);
	operandsStack.pop();

	auto firstOperand = dynamic_cast<Operand*>(operandsStack.top().get());
	if (firstOperand->is_identifier())
		set_identifier_value(firstOperand);
	operandsStack.pop();

	auto ft = firstOperand->get_variable_type(), st = secondOperand->get_variable_type();
	if (!firstOperand || !secondOperand || ft == var_type::STRING || st == var_type::STRING || ft == var_type::BOOLEAN || st == var_type::BOOLEAN)
		throw Error("Invalid arguments for binary arithmetic operation", step_type::SEM_ANALYSIS);


	std::shared_ptr<Number> ptr;
	switch (operation.get_type())
	{
	case item_type::PLUS:
		ptr = std::make_shared<Number>(firstOperand->get_integral_value() + secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;

	case item_type::MINUS:
		ptr = std::make_shared<Number>(firstOperand->get_integral_value() - secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;

	case item_type::MUL:
		ptr = std::make_shared<Number>(firstOperand->get_integral_value() * secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;

	case item_type::DIV:
		ptr = std::make_shared<Number>(firstOperand->get_integral_value() / secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;

	case item_type::MOD:
		ptr = std::make_shared<Number>(firstOperand->get_integral_value() % secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;
	}

	temporary.push_back(ptr);
}

void Program::execute(PrefArithmeticOperation const& operation)
{
	auto t = operation.get_type();

	auto operand = dynamic_cast<Operand*>(operandsStack.top().get());
	operandsStack.pop();

	if (operand->get_variable_type() != var_type::INT)
		throw Error("Semantic error : invalid operand for pref operation, integer expected", step_type::SEM_ANALYSIS);
	else if (operand->is_identifier())
	{
		auto ident = dynamic_cast<Identifier*>(operand);
		auto identPointer = static_cast<int*>(get_variable_address(ident->get_name()));

		if (t == item_type::PREF_INC)
			*identPointer += 1;
		else if (t == item_type::PREF_DEC)
			*identPointer -= 1;

		auto ptr = std::make_shared<Number>(*identPointer);
		operandsStack.push(ptr);
		temporary.push_back(ptr);
	}
	else
	{
		int push_value = operand->get_integral_value();
		if (t == item_type::MINUS)
			push_value *= -1;

		if(t != item_type::MINUS && t != item_type::PLUS)
			throw Error("Semantic error : invalid operand for pref operation, identifier expected", step_type::SEM_ANALYSIS);

		auto ptr = std::make_shared<Number>(push_value);
		operandsStack.push(ptr);
		temporary.push_back(ptr);
	}


}

void Program::execute(PostArithmeticOperation const& operation)
{
	auto operand = dynamic_cast<Identifier*>(operandsStack.top().get());
	if (operand == nullptr || operand->get_variable_type() != var_type::INT)
		throw Error("Semantic error : invalid operand for post operation", step_type::SEM_ANALYSIS);

	operandsStack.pop();

	auto identPointer = static_cast<int*>(get_variable_address(operand->get_name()));

	int savedValue = *identPointer;

	if (operation.get_type() == item_type::POST_INC)
		*identPointer += 1;
	else
		*identPointer -= 1;


	auto ptr = std::make_shared<Number>(savedValue);
	operandsStack.push(ptr);
	temporary.push_back(ptr);
}

void Program::execute(ComparisonOperation const& operation)
{
	auto secondOperand = dynamic_cast<Operand*>(operandsStack.top().get());
	if (secondOperand->is_identifier())
		set_identifier_value(secondOperand);
	operandsStack.pop();

	auto firstOperand = dynamic_cast<Operand*>(operandsStack.top().get());
	if (firstOperand->is_identifier())
		set_identifier_value(firstOperand);
	operandsStack.pop();

	if (firstOperand->get_variable_type() != secondOperand->get_variable_type() && firstOperand->get_variable_type() != var_type::STRING)
		throw Error("Semantic error : invalid aruments for comparison operation", step_type::SEM_ANALYSIS);

	std::shared_ptr<LogicalConstant> ptr;
	switch (operation.get_type())
	{
	case item_type::EQ:
		ptr = std::make_shared<LogicalConstant>(firstOperand->get_integral_value() == secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;

	case item_type::NEQ:
		ptr = std::make_shared<LogicalConstant>(firstOperand->get_integral_value() != secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;

	case item_type::GOEQ:
		ptr = std::make_shared<LogicalConstant>(firstOperand->get_integral_value() >= secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;

	case item_type::LOEQ:
		ptr = std::make_shared<LogicalConstant>(firstOperand->get_integral_value() <= secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;

	case item_type::LEQ:
		ptr = std::make_shared<LogicalConstant>(firstOperand->get_integral_value() < secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;

	case item_type::GEQ:
		ptr = std::make_shared<LogicalConstant>(firstOperand->get_integral_value() > secondOperand->get_integral_value());
		operandsStack.push(ptr);
		break;
	}

	temporary.push_back(ptr);
}

void Program::execute(PrintOperation const&)
{
	auto operand = dynamic_cast<Operand*>(operandsStack.top().get());
	if (operand == nullptr)
		throw Error("Semantic error : invalid argument for print operation", step_type::SEM_ANALYSIS);

	if (operand->is_identifier())
		set_identifier_value(operand);

	auto push_ptr = std::make_shared<printingItem>(operand->get_variable_type());

	if (operand->get_variable_type() == var_type::STRING)
		push_ptr->set_data(operand->get_string_value());
	else
		push_ptr->set_data(operand->get_integral_value());

	printQueue.push(push_ptr);
}


void Program::execute(IfBlock const& block)
{
	auto condition = dynamic_cast<Operand*>(operandsStack.top().get());
	operandsStack.pop();

	if (condition->get_variable_type() == var_type::STRING)
		throw Error("Semantic error : invalid expression in if-block", step_type::SEM_ANALYSIS);

	if (condition->is_identifier())
		set_identifier_value(condition);

	if (condition->get_integral_value() == 0)
		pIndex = block.get_jump_index() - 1;

}

void Program::execute(WhileBlock const& block)
{
	auto condition = dynamic_cast<Operand*>(operandsStack.top().get());
	operandsStack.pop();

	if (condition->get_variable_type() == var_type::STRING)
		throw Error("Semantic error : invalid expression in while-block", step_type::SEM_ANALYSIS);

	if (condition->is_identifier())
		set_identifier_value(condition);
	
	if (condition->get_integral_value() == 0)
		pIndex = block.get_jump_index() - 1;
}

void Program::execute(Jump const& obj)
{
	pIndex = obj.get_jump_index() - 1;
}

// --------------------------------------------------------------------

