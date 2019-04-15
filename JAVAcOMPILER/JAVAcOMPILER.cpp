// JAVAcOMPILER.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include "Scanner.hpp"
#include "Parser.hpp"

using namespace std;



int main()
{
	std::string filename = "PROGRAM.txt";


	try
	{
		Parser parsing(filename);
		parsing.start();
	}
	catch (const Error& err)
	{
		std::cout << "<" << err.step_name() << ">  ";
		std::cout << "[line : " << err.what_line() << " ] : " << err.reason() << std::endl;
	}

	system("pause");
	return 0;
}

