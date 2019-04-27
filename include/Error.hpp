#pragma once

#include <string>


enum class step_type { INIT, LEX_ANALYSIS, SYNT_ANALYSIS, SEM_ANALYSIS };

using line_type = unsigned int;

class Error
{
private:

	std::string err_reason;
	step_type err_step;
	unsigned long err_line;

public:

	Error(std::string reason, step_type step = step_type::INIT, long line = 0) :
		err_reason(reason), err_step(step), err_line(line) {}


	const std::string &reason() const {
		return err_reason; 
	}

	step_type step() const {
		return err_step; 
	}

	line_type what_line() const {
		return err_line;
	}

	std::string step_name() const 
	{
		switch (err_step)
		{
		case step_type::INIT:
				return "Initial";
		case step_type::LEX_ANALYSIS:
			return "Lexical analysis";
		case step_type::SYNT_ANALYSIS:
			return "Syntax analysis";
		case step_type::SEM_ANALYSIS:
			return "Semantic analysis";
		default:
			return {};
		}
	}

};

