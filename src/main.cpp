// JAVAcOMPILER.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <fstream>
#include "Parser.hpp"
#include "ExecProgram.hpp"



void printErrorInfo(Error const& err)
{
	std::cout << "<" << err.step_name() << ">  ";
	std::cout << "[line : " << err.what_line() << " ] : " << err.reason() << std::endl;
}


void execute(std::string const& filename)
{
	Parser parser(filename);
	parser.start();

	Program prog(parser.get_poliz(), parser.get_var_table());
	prog.run();
}

int main()
{
	std::string filename = "PROGRAM.txt";

	try
	{
		execute(filename);
	}
	catch (const Error& err)
	{
		printErrorInfo(err);
	}


	std::cout << std::endl;
	system("pause");
	return 0;
}

