#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>


enum class var_type { INT, BOOLEAN, STRING };
enum class item_type { EMPTY, PLUS, MINUS, AND, OR, NOT, POST_INC, PREF_INC, POST_DEC, PREF_DEC, LOEQ, GOEQ, GEQ, LEQ, EQ, NEQ, MUL, DIV, MOD};

class Program;

class TokenItem
{
public:
	item_type type = item_type::EMPTY;
	virtual bool executable() const = 0;

	virtual void execute(Program*) = 0;

	virtual ~TokenItem() {}
};


class InvertNotation
{
private:

	using shPtr = std::shared_ptr<TokenItem>;
	std::vector<shPtr> record;


	std::unordered_map<std::string, var_type> nameToType;

public:
	
	~InvertNotation() = default;

	void declare_variable(var_type type, std::string const& name)
	{
		nameToType[name] = type;
		nameToType.insert(std::make_pair(name, type));
	}

	var_type get_var_type(std::string const& name)
	{
		return nameToType[name];
	}

	bool is_variable_declared(std::string const& name) const
	{
		return (nameToType.find(name) != std::end(nameToType));
	}

	void add(std::shared_ptr<TokenItem> newItem)
	{
		record.push_back(newItem);
	}

	unsigned int size() const
	{
		return record.size();
	}

	std::vector<shPtr> get_record() const
	{
		return record;
	}

	std::unordered_map<std::string, var_type> get_table() const
	{
		return nameToType;
	}
};




// --------------------------------------------------------------------------


class Operation : public TokenItem
{
protected:
	
public:
	Operation() = default;
	Operation(item_type t) { this->type = t; }

	bool executable() const override {
		return true;
	}

};

class Operand : public TokenItem
{
protected:
	var_type variable_type;
public:
	
	bool executable() const override {
		return false;
	}

	var_type get_variable_type() const {
		return variable_type;
	}

	virtual bool is_identifier() const = 0;

	virtual std::string get_string_value() const = 0;
	virtual int get_integral_value() const = 0;

};

// ------------------------------------OPERANDS----------------------------------

class LogicalConstant : public Operand
{
	bool value;
public: 
	
	LogicalConstant(bool newVal) : value(newVal) { this->variable_type = var_type::BOOLEAN; }

	
	int get_integral_value() const override {
		return value;
	}
	std::string get_string_value() const override {
		return {};
	}

	bool is_identifier() const override {
		return false;
	}

	void execute(Program*) override {}
};

class Number : public Operand
{
	int value;
public:
	Number(int newVal) : value(newVal) { this->variable_type = var_type::INT; }

	int get_integral_value() const override {
		return value;
	}

	std::string get_string_value() const override {
		return {};
	}

	bool is_identifier() const override {
		return false;
	}

	void execute(Program*) override {}
};

class ConstCharString : public Operand
{
	std::string value;
public:
	ConstCharString(std::string str) : value(str) { this->variable_type = var_type::STRING; }

	std::string get_value() const {
		return value;
	}

	std::string get_string_value() const override {
		return value;
	}

	int get_integral_value() const override {
		return {};
	}

	bool is_identifier() const override {
		return false;
	}

	void execute(Program*) override {}
};



class Identifier : public Operand
{
	std::string name;
	std::string str_value;
	int int_value;
	bool bool_value;
public:
	Identifier(var_type t, std::string const& name) : name(name) { this->variable_type = t; }
	
	std::string get_name() const {
		return name;
	}

	void set_string_value(std::string const&  s) {
		str_value = s;
	}

	void set_integral_value(int val) {
		int_value = val;
		bool_value = val;
	}

	std::string get_string_value() const override {
		return str_value;
	}

	int get_integral_value() const override {
		if (variable_type == var_type::INT)
			return int_value;
		else
			return bool_value;
	}

	bool is_identifier() const override {
		return true;
	}

	void execute(Program*) override {}

};


// ---------------------------------------------- OPERATIONS ------------------------------------------

class EndOfTheProgram : public Operation 
{
public:
	void execute(Program*) override;
};


class AssignmentOperation : public Operation  
{
public:
	void execute(Program*) override;
};

class BinaryLogicalOperation : public Operation
{
public:
	BinaryLogicalOperation(item_type t) : Operation(t) {}

	item_type get_type() const {
		return this->type;
	}
	
	void execute(Program*) override;
};

class UnaryLogicalOperation : public Operation
{
public:
	UnaryLogicalOperation(item_type t) : Operation(t) {}

	item_type get_type() const {
		return this->type;
	}
	void execute(Program*) override;
};

class BinaryArithmeticOperation : public Operation
{
public:
	BinaryArithmeticOperation(item_type t) : Operation(t) {}

	item_type get_type() const {
		return this->type;
	}

	void execute(Program*) override;
};

class PrefArithmeticOperation : public Operation
{
public:
	PrefArithmeticOperation(item_type t) : Operation(t) {}

	item_type get_type() const {
		return this->type;
	}

	void execute(Program*) override;
};

class PostArithmeticOperation : public Operation
{
public:
	PostArithmeticOperation(item_type t) : Operation(t) {}

	item_type get_type() const {
		return this->type;
	}
	void execute(Program*) override;
};

class ComparisonOperation : public Operation
{
public:
	ComparisonOperation() = default;
	ComparisonOperation(item_type t) : Operation(t) {}

	item_type get_type() const {
		return this->type;
	}

	void execute(Program*) override;
};

class PrintOperation : public Operation
{
	void execute(Program*) override;
};

class IfBlock : public Operation
{
	unsigned int jump_index;
public:

	void set_jump_index(unsigned int val) {
		jump_index = val;
	}

	unsigned int get_jump_index() const {
		return jump_index;
	}
	
	void execute(Program*) override;
};

class WhileBlock : public Operation
{
	unsigned int jump_index;
public:

	void set_jump_index(unsigned int val) {
		jump_index = val;
	}

	unsigned int get_jump_index() const {
		return jump_index;
	}

	void execute(Program*) override;
};


class Jump : public Operation
{
	unsigned int jump_index;
public:

	void set_jump_index(unsigned int val) {
		jump_index = val;
	}

	unsigned int get_jump_index() const {
		return jump_index;
	}

	void execute(Program*) override;
};
