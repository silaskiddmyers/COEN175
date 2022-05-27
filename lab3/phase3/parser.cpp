/*
 * File:	parser.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

#include <cstdlib>
#include <iostream>
#include "tokens.h"
#include "lexer.h"
#include "checker.h"

using namespace std;

static int lookahead;
static string lexbuf;

static void expression();
static void statement();

/*
string tokenToType(int token)
{
	if (token == INT)
	{
		return "INT";
	}

	if (token == CHAR)
	{

		return "CHAR";
	}
	else
		return "STRUCT";
}
*/
/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

static void error()
{
	if (lookahead == DONE)
		report("syntax error at end of file", "done");
	else
		report("syntax error at '%s'", lexbuf);

	exit(EXIT_FAILURE);
}

/*
 * Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
	// cout << "matching " << t << " with " << lookahead << endl;
	if (lookahead != t)
	{
		error();
	}

	lookahead = lexan(lexbuf);
}

static string identifier()
{
	string val = lexbuf;
	match(ID);
	return val;
}

static unsigned number()
{
	string val = lexbuf;
	match(NUM);
	return stoi(val);
}

/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.
 */

static bool isSpecifier(int token)
{
	return token == INT || token == CHAR || token == STRUCT;
}

/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.  Simple C has int, char, and
 *		structure types.
 *
 *		specifier:
 *		  int
 *		  char
 *		  struct identifier
 */

static string specifier()
{
	if (lookahead == INT)
	{
		match(INT);
		return "INT";
	}

	if (lookahead == CHAR)
	{
		match(CHAR);
		return "CHAR";
	}

	match(STRUCT);
	// identifier();
	return identifier();
}

/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

static unsigned pointers()
{
	unsigned indirection = 0;
	while (lookahead == '*')
	{
		match('*');
		indirection++;
	}
	return indirection;
}

/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, both with optional pointer
 *		declarators, or a callback (i.e., a simple function
 *		pointer).
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ num ]
 *		  pointers ( * identifier ) ( )
 */

static void declarator(string spec_type)
{
	unsigned indirection = pointers();

	if (lookahead == '(')
	{
		match('(');
		match('*');
		string name = identifier();
		match(')');
		match('(');
		match(')');
		cout << "1" << endl;
		declareVariable(name, Type(CALLBACK, spec_type, indirection));
	}
	else
	{
		string name = identifier();

		if (lookahead == '[')
		{
			match('[');
			unsigned num = number();
			match(']');
			declareVariable(name, Type(ARRAY, spec_type, indirection, num));
		}
		else
			declareVariable(name, Type(SCALAR, spec_type, indirection));
	}
}

/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ';'
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
	string spec_type = specifier();
	declarator(spec_type);

	while (lookahead == ',')
	{
		match(',');
		declarator(spec_type);
	}

	match(';');
}

/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
	while (isSpecifier(lookahead))
		declaration();
}

/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *
 *		primary-expression:
 *		  ( expression )
 *		  identifier
 *		  character
 *		  string
 *		  num
 */

static void primaryExpression(bool lparen)
{
	if (lparen)
	{
		expression();
		match(')');
	}
	else if (lookahead == CHARACTER)
		match(CHARACTER);

	else if (lookahead == STRING)
		match(STRING);

	else if (lookahead == NUM)
		number();

	else if (lookahead == ID)
	{
		string name = identifier();
		checkID(name);
	}
	else
		error();
}

/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 *		  postfix-expression ( expression-list )
 *		  postfix-expression ( )
 *		  postfix-expression . identifier
 *		  postfix-expression -> identifier
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static void postfixExpression(bool lparen)
{
	primaryExpression(lparen);

	while (1)
	{
		if (lookahead == '[')
		{
			match('[');
			expression();
			match(']');
			// cout << "index" << endl;
		}
		else if (lookahead == '(')
		{
			match('(');

			if (lookahead != ')')
			{
				expression();

				while (lookahead == ',')
				{
					match(',');
					expression();
				}
			}

			match(')');
			// cout << "call" << endl;
		}
		else if (lookahead == '.')
		{
			match('.');
			identifier();
			// cout << "dot" << endl;
		}
		else if (lookahead == ARROW)
		{
			match(ARROW);
			identifier();
			// cout << "arrow" << endl;
		}
		else
			break;
	}
}

/*
 * Function:	prefixExpression
 *
 * Description:	Parse a prefix expression.
 *
 *		prefix-expression:
 *		  postfix-expression
 *		  ! prefix-expression
 *		  - prefix-expression
 *		  * prefix-expression
 *		  & prefix-expression
 *		  sizeof prefix-expression
 *		  sizeof ( specifier pointers )
 *		  ( specifier pointers ) prefix-expression
 *
 *		This grammar is still ambiguous since "sizeof(type) * n"
 *		could be interpreted as a multiplicative expression or as a
 *		cast of a dereference.  The correct interpretation is the
 *		former, as the latter makes little sense semantically.  We
 *		resolve the ambiguity here by always consuming the "(type)"
 *		as part of the sizeof expression.
 */

static void prefixExpression()
{
	if (lookahead == '!')
	{
		match('!');
		prefixExpression();
		// cout << "not" << endl;
	}
	else if (lookahead == '-')
	{
		match('-');
		prefixExpression();
		// cout << "neg" << endl;
	}
	else if (lookahead == '*')
	{
		match('*');
		prefixExpression();
		// cout << "deref" << endl;
	}
	else if (lookahead == '&')
	{
		match('&');
		prefixExpression();
		// cout << "addr" << endl;
	}
	else if (lookahead == SIZEOF)
	{
		match(SIZEOF);

		if (lookahead == '(')
		{
			match('(');

			if (isSpecifier(lookahead))
			{
				specifier();
				pointers();
				match(')');
			}
			else
				postfixExpression(true);
		}
		else
			prefixExpression();

		// cout << "sizeof" << endl;
	}
	else if (lookahead == '(')
	{
		match('(');

		if (isSpecifier(lookahead))
		{
			specifier();
			pointers();
			match(')');
			prefixExpression();
			// cout << "cast" << endl;
		}
		else
			postfixExpression(true);
	}
	else
		postfixExpression(false);
}

/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.
 *
 *		multiplicative-expression:
 *		  prefix-expression
 *		  multiplicative-expression * prefix-expression
 *		  multiplicative-expression / prefix-expression
 *		  multiplicative-expression % prefix-expression
 */

static void multiplicativeExpression()
{
	prefixExpression();

	while (1)
	{
		if (lookahead == '*')
		{
			match('*');
			prefixExpression();
			// cout << "mul" << endl;
		}
		else if (lookahead == '/')
		{
			match('/');
			prefixExpression();
			// cout << "div" << endl;
		}
		else if (lookahead == '%')
		{
			match('%');
			prefixExpression();
			// cout << "rem" << endl;
		}
		else
			break;
	}
}

/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */

static void additiveExpression()
{
	multiplicativeExpression();

	while (1)
	{
		if (lookahead == '+')
		{
			match('+');
			multiplicativeExpression();
			// cout << "add" << endl;
		}
		else if (lookahead == '-')
		{
			match('-');
			multiplicativeExpression();
			// cout << "sub" << endl;
		}
		else
			break;
	}
}

/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static void relationalExpression()
{
	additiveExpression();

	while (1)
	{
		if (lookahead == '<')
		{
			match('<');
			additiveExpression();
			// cout << "ltn" << endl;
		}
		else if (lookahead == '>')
		{
			match('>');
			additiveExpression();
			// cout << "gtn" << endl;
		}
		else if (lookahead == LEQ)
		{
			match(LEQ);
			additiveExpression();
			// cout << "leq" << endl;
		}
		else if (lookahead == GEQ)
		{
			match(GEQ);
			additiveExpression();
			// cout << "geq" << endl;
		}
		else
			break;
	}
}

/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static void equalityExpression()
{
	relationalExpression();

	while (1)
	{
		if (lookahead == EQL)
		{
			match(EQL);
			relationalExpression();
			// cout << "eql" << endl;
		}
		else if (lookahead == NEQ)
		{
			match(NEQ);
			relationalExpression();
			// cout << "neq" << endl;
		}
		else
			break;
	}
}

/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static void logicalAndExpression()
{
	equalityExpression();

	while (lookahead == AND)
	{
		match(AND);
		equalityExpression();
		// cout << "and" << endl;
	}
}

/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static void expression()
{
	logicalAndExpression();

	while (lookahead == OR)
	{
		match(OR);
		logicalAndExpression();
		// cout << "or" << endl;
	}
}

/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static void statements()
{
	while (lookahead != '}')
		statement();
}

/*
 * Function:	assignment
 *
 * Description:	Parse an assignment statement.
 *
 *		assignment:
 *		  expression = expression
 *		  expression
 */

static void assignment()
{
	expression();

	if (lookahead == '=')
	{
		match('=');
		expression();
	}
}

/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  return expression ;
 *		  while ( expression ) statement
 *		  for ( assignment ; expression ; assignment ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  assignment ;
 */

static void statement()
{
	if (lookahead == '{')
	{
		openScope();
		match('{');
		declarations();
		statements();
		match('}');
		closeScope();
	}
	else if (lookahead == RETURN)
	{
		match(RETURN);
		expression();
		match(';');
	}
	else if (lookahead == WHILE)
	{
		match(WHILE);
		match('(');
		expression();
		match(')');
		statement();
	}
	else if (lookahead == FOR)
	{
		match(FOR);
		match('(');
		assignment();
		match(';');
		expression();
		match(';');
		assignment();
		match(')');
		statement();
	}
	else if (lookahead == IF)
	{
		match(IF);
		match('(');
		expression();
		match(')');
		statement();

		if (lookahead == ELSE)
		{
			match(ELSE);
			statement();
		}
	}
	else
	{
		assignment();
		match(';');
	}
}

/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always either a
 *		simple variable with optional pointer declarators, or a
 *		callback (i.e., a simple function pointer)
 *
 *		parameter:
 *		  specifier pointers identifier
 *		  specifier pointers ( * identifier ) ( )
 */

static Type parameter()
{
	Type t;
	string spec_type = specifier();
	unsigned indirection = pointers();

	if (lookahead == '(')
	{
		match('(');
		match('*');
		string name = identifier();
		match(')');
		match('(');
		match(')');
		t = Type(CALLBACK, spec_type, indirection);
		declareVariable(name, t);
	}
	else if (spec_type != "INT" && spec_type != "CHAR")
	{
		cout << "2" << endl;
		declareVariable(identifier(), Type(CALLBACK, spec_type, indirection));
	}
	else
	{
		t = Type(SCALAR, spec_type, indirection);
		string name = identifier();
		declareVariable(name, t);
	}
	return t;
}

/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  parameter-list
 *
 *		parameter-list:
 *		  parameter
 *		  parameter , parameter-list
 */

static Parameters *parameters()
{
	Parameters *param_list = new Parameters;
	if (lookahead == VOID)
		match(VOID);

	else
	{
		param_list->push_back(parameter());

		while (lookahead == ',')
		{
			match(',');
			param_list->push_back(parameter());
		}
	}
	return param_list;
}

/*
 * Function:	globalDeclarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable, an array, or a function, all with optional
 *		pointer	declarators, or a callback (i.e., a simple function
 *		pointer).
 *
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier ( )
 *		  pointers identifier [ num ]
 *		  pointers ( * identifier ) ( )
 */

static void globalDeclarator(string spec_type)
{
	unsigned indirection = pointers();

	if (lookahead == '(')
	{
		match('(');
		match('*');
		string name = identifier();
		match(')');
		match('(');
		match(')');
		cout << "3" << endl;
		declareVariable(name, Type(CALLBACK, spec_type, indirection));
	}
	else
	{
		string name = identifier();

		if (lookahead == '(')
		{
			match('(');
			match(')');
			declareFunc(name, Type(FUNCTION, spec_type, indirection));
		}
		else if (lookahead == '[')
		{
			match('[');
			unsigned num = number();

			match(']');
			declareVariable(name, Type(ARRAY, spec_type, indirection, num));
		}
		else
			declareVariable(name, Type(SCALAR, spec_type, indirection));
	}
}

/*
 * Function:	remainingDeclarators
 *
 * Description:	Parse any remaining global declarators after the first.
 *
 * 		remaining-declarators:
 * 		  ;
 * 		  , global-declarator remaining-declarators
 */

static void remainingDeclarators(string spec_type)
{
	while (lookahead == ',')
	{
		match(',');
		globalDeclarator(spec_type);
	}

	match(';');
}

/*
 * Function:	globalOrFunction
 *
 * Description:	Parse a global declaration or function definition.
 *
 * 		global-or-function:
 * 		  struct identifier { declaration declarations } ;
 * 		  specifier pointers identifier remaining-decls
 * 		  specifier pointers identifier ( ) remaining-decls
 * 		  specifier pointers identifier [ num ] remaining-decls
 * 		  specifier pointers identifier ( parameters ) { ... }
 */

static void globalOrFunction()
{
	string spec = specifier();
	if ((spec != "INT") && (spec != "CHAR") && (lookahead == '{'))
	{
		openStruct(spec);
		match('{');
		declaration();
		declarations();
		match('}');
		closeStruct(spec);
		match(';');
	}
	else
	{
		unsigned indirection = pointers();

		if (lookahead == '(')
		{
			match('(');
			match('*');
			string name = identifier();
			match(')');
			match('(');
			match(')');
			declareVariable(name, Type(CALLBACK, spec, indirection));
			remainingDeclarators(spec);
		}
		else
		{
			string name = identifier();

			if (lookahead == '[')
			{
				match('[');
				unsigned num = number();
				match(']');
				declareVariable(name, Type(ARRAY, spec, indirection, num));
				remainingDeclarators(spec);
			}
			else if (lookahead == '(')
			{

				match('(');

				if (lookahead == ')')
				{
					if (spec != "INT" && spec != "CHAR")
					{
						cout << "5" << endl;
						declareVariable(name, Type(CALLBACK, spec, indirection));
					}
					else
						declareFunc(name, Type(FUNCTION, spec, indirection));
					match(')');
					remainingDeclarators(spec);
					
				}
				else
				{

					Parameters *params = new Parameters;
					defineFunc(name, Type(FUNCTION, spec, indirection, params));
					openScope();
					params = parameters();
					match(')');

					match('{');
					declarations();
					statements();
					match('}');

					closeScope();
				}
			}
			else
			{
				declareVariable(name, Type(SCALAR, spec, indirection));
				remainingDeclarators(spec);
			}
		}
	}
}

/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main()
{
	openScope();
	lookahead = lexan(lexbuf);

	while (lookahead != DONE)
		globalOrFunction();
	closeScope();
	exit(EXIT_SUCCESS);
}
