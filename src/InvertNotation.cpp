#include "pch.h"
#include "ExecProgram.hpp"

void EndOfTheProgram::execute(Program* obj)
{
	obj->execute(*this);
}

void AssignmentOperation::execute(Program* obj)
{
	obj->execute(*this);
}


void BinaryLogicalOperation::execute(Program* obj)
{
	obj->execute(*this);
}


void UnaryLogicalOperation::execute(Program* obj)
{
	obj->execute(*this);
}


void BinaryArithmeticOperation::execute(Program* obj)
{
	obj->execute(*this);
}


void PrefArithmeticOperation::execute(Program* obj)
{
	obj->execute(*this);
}


void PostArithmeticOperation::execute(Program* obj)
{
	obj->execute(*this);
}


void ComparisonOperation::execute(Program* obj)
{
	obj->execute(*this);
}


void PrintOperation::execute(Program* obj)
{
	obj->execute(*this);
}


void IfBlock::execute(Program* obj)
{
	obj->execute(*this);
}

void WhileBlock::execute(Program* obj)
{
	obj->execute(*this);
}

void Jump::execute(Program* obj)
{
	obj->execute(*this);
}
