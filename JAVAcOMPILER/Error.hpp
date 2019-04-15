#pragma once

#include <string>
#include <unordered_map>
#include <initializer_list>

enum step_type { INIT, LEX_ANALYSIS, SYNT_ANALYSIS, SEM_ANALYSIS };

class Error
{
private:

	std::string err_reason;
	step_type err_step;
	long err_line;

	std::unordered_map<step_type, std::string> name_table;

public:

	Error(std::string reason, step_type step = INIT, long line = 0) : 
		err_reason(reason), err_step(step), err_line(line) 
	{
		init_table();
	}

	void init_table()
	{
		std::initializer_list<std::pair<step_type, std::string>> lst {
			{INIT, "Initial"},
			{LEX_ANALYSIS, "Lexical analysis"},
			{SYNT_ANALYSIS, "Syntax analysis"},
			{SEM_ANALYSIS, "Semantic analysis"}
		};

		name_table.insert(lst.begin(), lst.end());

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

	long what_line() const { 
		return err_line; 
	}

};

