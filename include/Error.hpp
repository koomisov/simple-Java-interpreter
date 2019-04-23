#pragma once

#include <string>
#include <unordered_map>
#include <initializer_list>


enum class step_type { INIT, LEX_ANALYSIS, SYNT_ANALYSIS, SEM_ANALYSIS };

using line_type = unsigned int;

class Error
{
private:

	std::string err_reason;
	step_type err_step;
	long err_line;

	std::unordered_map<step_type, std::string> name_table;

public:

	Error(std::string reason, step_type step = step_type::INIT, long line = 0) :
		err_reason(reason), err_step(step), err_line(line) 
	{
		init_table();
	}

	void init_table()
	{
		std::initializer_list<std::pair<step_type, std::string>> lst {
			{step_type::INIT, "Initial"},
			{step_type::LEX_ANALYSIS, "Lexical analysis"},
			{step_type::SYNT_ANALYSIS, "Syntax analysis"},
			{step_type::SEM_ANALYSIS, "Semantic analysis"}
		};

		name_table.insert(std::make_move_iterator(lst.begin()), std::make_move_iterator(lst.end()));

	}

	const std::string &reason() const {
		return err_reason; 
	}

	step_type step() const {
		return err_step; 
	}

	std::string step_name() const {
		return name_table.at(err_step);
	}

	line_type what_line() const { 
		return err_line; 
	}

};

