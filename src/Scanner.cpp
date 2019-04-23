#include "pch.h"
#include "Scanner.hpp"

Token Scanner::get_token()
{
	state = state::START;
	do 
	{
		switch (state)
		{
		case state::START:
			current_token = "";
			current_token += symbol;
			if (isspace(symbol))
			{
				if (symbol == '\n')
					++line;
				current_token = "";
				symbol = file.get();
			}
			else if (isalpha(symbol)) { symbol = file.get(); state = state::IDENT; }
			else if (isdigit(symbol)) { symbol = file.get(); state = state::NUMBER; }
			else if (symbol == '-') { symbol = file.get(); state = state::MINUS; }
			else if (symbol == '+') { symbol = file.get(); state = state::PLUS; }
			else if (symbol == '&') { symbol = file.get(); state = state::AND; }
			else if (symbol == '|') { symbol = file.get(); state = state::OR; }
			else if (symbol == '/') { symbol = file.get(); state = state::DIV; }
			else if (symbol == '"') { symbol = file.get(); current_token.pop_back(); state = state::STRING; }
			else if ((symbol == '>') || (symbol == '<') || (symbol == '!') || (symbol == '=')) { symbol = file.get(); state = state::EQU; }
			else { symbol = file.get(); state = state::DELIM; }
			break;

		case state::IDENT:
			if (isalpha(symbol) || (isdigit(symbol)) || (symbol == '_'))
			{
				current_token += symbol;
				symbol = file.get();
			}
			else
				return look_words(current_token);
			break;

		case state::NUMBER:
			if (isdigit(symbol))
			{
				current_token += symbol;
				symbol = file.get();
			}
			else
				return to_number(current_token);
			break;

		case state::PLUS:
			if (symbol == '+')
			{
				current_token += symbol;
				symbol = file.get();
				return Token(TOKEN_INC, current_token);
			}
			else return
				Token(TOKEN_PLUS, current_token);
			break;

		case state::MINUS:
			if (symbol == '-')
			{
				current_token += symbol;
				symbol = file.get();
				return Token(TOKEN_DEC, current_token);
			}
			else return
				Token(TOKEN_MINUS, current_token);
			break;

		case state::EQU:
			if (symbol == '=') 
			{
				current_token += symbol;
				symbol = file.get();
				return look_delimiters(current_token);
			}
			else
				return look_delimiters(current_token);
			break;

		case state::AND:
			if (symbol == '&')
			{
				current_token += symbol;
				symbol = file.get();
				return Token(TOKEN_AND, current_token);
			}
			else
				throw Error("Error in token: < & > - did you mean < && >?", step_type::LEX_ANALYSIS, line);
			break;

		case state::OR:
			if (symbol == '|')
			{
				current_token += symbol;
				symbol = file.get();
				return Token(TOKEN_OR, current_token);
			}
			else
				throw Error("Error in token: < | > - did you mean < || >?", step_type::LEX_ANALYSIS, line);
			break;

		case state::DIV:
			if (symbol == '/')
			{
				current_token += symbol;
				symbol = file.get();
				state = state::COMMENT;
			}
			else
				return Token(TOKEN_DIV, current_token);
			break;

		case state::COMMENT:
			if ((symbol == '\n') || (symbol == EOF))
				return Token(TOKEN_COMMENT, current_token);
			else
			{
				current_token += symbol;
				symbol = file.get();
			}
			break;

		case state::STRING:
			if (symbol == '"')
			{
				symbol = file.get();
				return Token(TOKEN_STR, current_token);
			}
			else if (symbol == '\\')
			{
				state = state::SCREENING;
				symbol = file.get();
			}
			else if ((symbol == '\n') || (symbol == EOF))
				throw Error("Error in token: missing terminating caracter in string", step_type::LEX_ANALYSIS, line);
			else
			{
				current_token += symbol;
				symbol = file.get();
			}
			break;

		case state::SCREENING:
			if (symbol == 'n')
				current_token.push_back('\n');
			else if (symbol == 't')
				current_token += "    ";
			else if (symbol == EOF || symbol == '\n')
				throw Error("Error in token: missing terminating caracter in string", step_type::LEX_ANALYSIS, line);
			else 
				current_token += symbol;

			symbol = file.get();
			state = state::STRING;
			break;

		case state::DELIM:
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
		throw Error("Error in token: invalid combination or characters", step_type::LEX_ANALYSIS, line);
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
		int number = std::stoi(token);
		if (number > max_num || number < min_num)
			throw Error("Error in token: number out of range", step_type::LEX_ANALYSIS, line);
		return Token(TOKEN_NUM, std::to_string(number));
	}
	catch (std::invalid_argument&) { throw Error("Error in token: invalid number", step_type::LEX_ANALYSIS, line); }
	catch (std::out_of_range&) { throw Error("Error in token: number out of range", step_type::LEX_ANALYSIS, line); }
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
		{"string", TOKEN_STRING},
		{"boolean", TOKEN_BOOL},
		{"true", TOKEN_TRUE},
		{"false", TOKEN_FALSE}
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
