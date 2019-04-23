// JAVAcOMPILER.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include "Scanner.hpp"
#include "Parser.hpp"
#include "ExecProgram.hpp"

using namespace std;



int main()
{
	std::string filename = "PROGRAM1.txt";


	try
	{
		Parser parser(filename);
		parser.start();

		Program prog(parser.get_poliz(), parser.get_var_table());
		prog.run();
	}
	catch (const Error& err)
	{
		std::cout << "<" << err.step_name() << ">  ";
		std::cout << "[line : " << err.what_line() << " ] : " << err.reason() << std::endl;
	}

	std::cout << std::endl;
	system("pause");
	return 0;
}

