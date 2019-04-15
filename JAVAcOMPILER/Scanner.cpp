#include "pch.h"
#include "Scanner.hpp"

Token Scanner::get_token()
{
	state = START;
	do 
	{
		switch (state)
		{
		case START:
			current_token = "";
			current_token += symbol;
			if (isspace(symbol))
			{
				if (symbol == '\n')
					++line;
				current_token = "";
				symbol = file.get();
			}
			else if (isalpha(symbol)) { symbol = file.get(); state = IDENT; }
			else if (isdigit(symbol)) { symbol = file.get(); state = NUMBER; }
			else if (symbol == '-') { symbol = file.get(); state = MINUS; }
			else if (symbol == '+') { symbol = file.get(); state = PLUS; }
			else if (symbol == '&') { symbol = file.get(); state = AND; }
			else if (symbol == '|') { symbol = file.get(); state = OR; }
			else if (symbol == '/') { symbol = file.get(); state = DIV; }
			else if (symbol == '"') { symbol = file.get(); state = STRING; }
			else if ((symbol == '>') || (symbol == '<') || (symbol == '!') || (symbol == '=')) { symbol = file.get(); state = EQU; }
			else { symbol = file.get(); state = DELIM; }
			break;

		case IDENT:
			if (isalpha(symbol) || (isdigit(symbol)) || (symbol == '_'))
			{
				current_token += symbol;
				symbol = file.get();
			}
			else
				return look_words(current_token);
			break;

		case NUMBER:
			if (isdigit(symbol))
			{
				current_token += symbol;
				symbol = file.get();
			}
			else
				return to_number(current_token);
			break;

		case PLUS:
			if (symbol == '+')
			{
				current_token += symbol;
				symbol = file.get();
				return Token(TOKEN_INC, current_token);
			}
			else return
				Token(TOKEN_PLUS, current_token);
			break;

		case MINUS:
			if (symbol == '-')
			{
				current_token += symbol;
				symbol = file.get();
				return Token(TOKEN_DEC, current_token);
			}
			else return
				Token(TOKEN_MINUS, current_token);
			break;

		case EQU:
			if (symbol == '=') {
				current_token += symbol;
				symbol = file.get();
				return look_delimiters(current_token);
			}
			else
				return look_delimiters(current_token);
			break;

		case AND:
			if (symbol == '&')
			{
				current_token += symbol;
				symbol = file.get();
				return Token(TOKEN_AND, current_token);
			}
			else
				throw Error("Error in token: < & > - did you mean < && >?", LEX_ANALYSIS, line);
			break;

		case OR:
			if (symbol == '|')
			{
				current_token += symbol;
				symbol = file.get();
				return Token(TOKEN_OR, current_token);
			}
			else
				throw Error("Error in token: < | > - did you mean < || >?", LEX_ANALYSIS, line);
			break;

		case DIV:
			if (symbol == '/')
			{
				current_token += symbol;
				symbol = file.get();
				state = COMMENT;
			}
			else
				return Token(TOKEN_DIV, current_token);
			break;

		case COMMENT:
			if ((symbol == '\n') || (symbol == EOF))
				return Token(TOKEN_COMMENT, current_token);
			else
			{
				current_token += symbol;
				symbol = file.get();
			}
			break;

		case STRING:
			if (symbol == '"')
			{
				current_token += symbol;
				symbol = file.get();
				return Token(TOKEN_STR, current_token);
			}
			else if (symbol == '\\')
			{
				state = SCREENING;
				symbol = file.get();
			}
			else if ((symbol == '\n') || (symbol == EOF))
				throw Error("Error in token: missing terminating caracter in string", LEX_ANALYSIS, line);
			else
			{
				current_token += symbol;
				symbol = file.get();
			}
			break;

		case SCREENING:
			if (symbol == 'n')
				current_token.push_back('\n');
			else if (symbol == 't')
				current_token += "    ";
			else if (symbol == EOF || symbol == '\n')
				throw Error("Error in token: missing terminating caracter in string", LEX_ANALYSIS, line);
			else 
				current_token += symbol;

			symbol = file.get();
			state = STRING;
			break;

		case DELIM:
			return look_delimiters(current_token);
			break;
		}

	} while (symbol != EOF);

	return Token(TOKEN_FIN);
}

Token Scanner::look_delimiters(std::string token)
{
	if (table_delimiters.count(token))
		return Token(table_delimiters[token], token);
	else
		throw Error("Error in token: invalid combination or characters", LEX_ANALYSIS, line);
}

Token Scanner::look_words(std::string token)
{
	if (table_words.count(token))
		return Token(table_words[token], token);
	else
		return Token(TOKEN_ID, token);
}

Token Scanner::to_number(std::string token)
{
	try
	{
		unsigned long number = std::stoul(token);
		if (number > max_num)
			throw Error("Error in token: number out of range", LEX_ANALYSIS, line);
		return Token(TOKEN_NUM, std::to_string(number));
	}
	catch (std::invalid_argument) { throw Error("Error in token: invalid number", LEX_ANALYSIS, line); }
	catch (std::out_of_range) { throw Error("Error in token: number out range", LEX_ANALYSIS, line); }
}

void Scanner::init_tables()
{
	init_pair_type keywords
	{
		{"public", TOKEN_PUBL},
		{"class", TOKEN_CLASS},
		{"static", TOKEN_STATIC},
		{"void", TOKEN_VOID},
		{"main", TOKEN_MAIN},
		{"if", TOKEN_IF},
		{"else", TOKEN_ELSE},
		{"while", TOKEN_WHILE},
		{"System", TOKEN_SYS},
		{"out", TOKEN_OUT},
		{"println", TOKEN_PRINTL},
		{"int", TOKEN_INT},
		{"boolean", TOKEN_BOOL}
	};


	init_pair_type delimiters
	{
		{"{", TOKEN_LBLOCK},
		{"}", TOKEN_RBLOCK},
		{"(", TOKEN_LPAREN},
		{")", TOKEN_RPAREN},
		{";", TOKEN_SEMICOL},
		{",", TOKEN_COMMA},
		{".", TOKEN_DOT},
		{"++", TOKEN_INC},
		{"--", TOKEN_DEC},
		{"+", TOKEN_PLUS},
		{"-", TOKEN_MINUS},
		{"!", TOKEN_NOT},
		{"*", TOKEN_MUL},
		{"/", TOKEN_DIV},
		{"%", TOKEN_MOD},
		{"<=", TOKEN_LOEQ},
		{">=", TOKEN_GOEQ},
		{"<", TOKEN_LEQ},
		{">", TOKEN_GEQ},
		{"==", TOKEN_EQ},
		{"!=", TOKEN_NEQ},
		{"&&", TOKEN_AND},
		{"||", TOKEN_OR},
		{"=", TOKEN_ASSIGN}
	};

	table_words.insert(std::make_move_iterator(keywords.begin()), std::make_move_iterator(keywords.end()));
	table_delimiters.insert(std::make_move_iterator(delimiters.begin()), std::make_move_iterator(delimiters.end()));
}
