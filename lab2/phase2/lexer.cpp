/*
 * File:	lexer.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the lexical analyzer for Simple C.
 *
 *		Extra functionality:
 *		- checking for out of range integer literals
 *		- checking for invalid string constants
 *		- checking for invalid character constants
 */

# include <set>
# include <cstdio>
# include <cerrno>
# include <cctype>
# include <cstdlib>
# include <iostream>
# include "tokens.h"
# include "string.h"
# include "lexer.h"

using namespace std;
int numerrors, lineno = 1;
char temp_c;

/* Later, we will associate token values with each keyword */

static set<string> keywords = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while",
};


/*
 * Function:	report
 *
 * Description:	Report an error to the standard error prefixed with the
 *		line number.  We'll be using this a lot later with an
 *		optional string argument, but C++'s stupid streams don't do
 *		positional arguments, so we actually resort to snprintf.
 *		You just can't beat C for doing things down and dirty.
 */

void report(const string &str, const string &arg)
{
    char buf[1000];

    snprintf(buf, sizeof(buf), str.c_str(), arg.c_str());
    cerr << "line " << lineno << ": " << buf << endl;
    numerrors ++;
}


/*
 * Function:	lexan
 *
 * Description:	Read and tokenize the standard input stream.  The lexeme is
 *		stored in a buffer.
 */

int lexan(string &lexbuf)
{
    static int c = cin.get();
    bool invalid, overflow;
    long val;
    int p;


    /* The invariant here is that the next character has already been read
       and is ready to be classified.  In this way, we eliminate having to
       push back characters onto the stream, merely to read them again. */

    while (!cin.eof()) {
	lexbuf.clear();


	/* Ignore white space */

	while (isspace(c)) {
	    if (c == '\n')
		lineno ++;

	    c = cin.get();
	}


	/* Check for an identifier or a keyword */

	if (isalpha(c) || c == '_') {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isalnum(c) || c == '_');

	    if (keywords.count(lexbuf) > 0)
		{
			//cout << "keyword:" << lexbuf << endl;

			int index = (int)distance(keywords.begin(), keywords.find(lexbuf)) + 256;
			return index;
		}
		
	    else
		{
			//cout << "identifier:" << lexbuf << endl;
			return ID;
		}
		

	    


	/* Check for a number */

	} else if (isdigit(c)) {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isdigit(c));

	    errno = 0;
	    val = strtol(lexbuf.c_str(), NULL, 0);

	    if (errno != 0 || val != (int) val)
		report("integer constant too large");

	    //cout << "number:" << lexbuf << endl;
	    return NUM;


	/* There must be an easier way to do this.  It might seem stupid at
	   this point to recognize each token separately, but eventually
	   we'll be returning separate token values to the parser, so we
	   might as well do it now. */

	} else {
	    lexbuf += c;

	    switch(c) {


	    /* Check for '|' and '||' */

	    case '|':
		c = cin.get();

		if (c == '|') {
		    lexbuf += c;
		    c = cin.get();
		}

		//cout << "operator:" << lexbuf << endl;
		return 288;


	    /* Check for '=' and '==' */

	    case '=':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return EQL;
		}

		//cout << "operator:" << lexbuf << endl;
		return ASSIGN;


	    /* Check for '&' and '&&' */

	    case '&':
		c = cin.get();

		if (c == '&') {
		    lexbuf += c;
		    c = cin.get();
			return 289;
		}

		//cout << "operator:" << lexbuf << endl;
		return 38;


	    /* Check for '!' and '!=' */

	    case '!':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return 291;
		}

		//cout << "operator:" << lexbuf << endl;
		return 33;


	    /* Check for '<' and '<=' */

	    case '<':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return LEQ;
		}

		//cout << "operator:" << lexbuf << endl;
		return 60;


	    /* Check for '>' and '>=' */

	    case '>':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return GEQ;
		}

		//cout << "operator:" << lexbuf << endl;
		return GTN;


	    /* Check for '-', '--', and '->' */

	    case '-':
		c = cin.get();

		if (c == '-') {
		    lexbuf += c;
		    c = cin.get();
			return 295;
		} else if (c == '>') {
		    lexbuf += c;
		    c = cin.get();
			return 296;
		}

		//cout << "operator:" << lexbuf << endl;
		return 45;


	    /* Check for '+' and '++' */

	    case '+':
		c = cin.get();

		if (c == '+') {
		    lexbuf += c;
		    c = cin.get();
			return 294;
		}

		//cout << "operator:" << lexbuf << endl;
		return 43;


	    /* Check for simple, single character tokens */

	    case '*': case '%': case ':': case ';':
	    case '(': case ')': case '[': case ']':
	    case '{': case '}': case '.': case ',':
		temp_c = c;
		c = cin.get();
		//cout << "operator:" << lexbuf << endl;
		return (int)temp_c;


	    /* Check for '/' or a comment */

	    case '/':
		c = cin.get();

		if (c == '*') {
		    do {
			while (c != '*' && !cin.eof()) {
			    if (c == '\n')
				lineno ++;

			    c = cin.get();
			}

			c = cin.get();
		    } while (c != '/' && !cin.eof());

		    c = cin.get();
		    break;

		} else {
		    //cout << "operator:" << lexbuf << endl;
		    return 47;
		}


	    /* Check for a string literal */

	    case '"':
		do {
		    p = c;
		    c = cin.get();
		    lexbuf += c;

		    if (c == '\n')
			lineno ++;

		} while (p == '\\' || (c != '"' && c != '\n' && !cin.eof()));

		if (c == '\n' || cin.eof())
		    report("premature end of string constant");
		else {
		    string s = lexbuf.substr(1, lexbuf.size() - 2);
		    s = parseString(s, invalid, overflow);

		    if (invalid)
			report("unknown escape sequence");
		    else if (overflow)
			report("escape sequence out of range");
		}

		c = cin.get();
		//cout << "string:" << lexbuf << endl;
		return 299;


	    /* Check for a character literal */

	    case '\'':
		do {
		    p = c;
		    c = cin.get();
		    lexbuf += c;

		    if (c == '\n')
			lineno ++;

		} while (p == '\\' || (c != '\'' && c != '\n' && !cin.eof()));

		if (c == '\n' || cin.eof())
		    report("premature end of character constant");
		else {
		    string s = lexbuf.substr(1, lexbuf.size() - 2);
		    s = parseString(s, invalid, overflow);

		    if (invalid)
			report("unknown escape sequence");
		    else if (overflow)
			report("escape sequence out of range");
		    else if (s.size() == 0)
			report("empty character constant");
		    else if (s.size() != 1)
			report("multi-character character constant");
		}
		c = cin.get();
		//cout << "character:" << lexbuf << endl;
		return 300;


	    /* Handle EOF here as well */

	    case EOF:
		return 302;


	    /* Everything else is illegal */

	    default:
		c = cin.get();
		break;
	    }
	}
    }

    return 0;
}


/*
 * Function:	main
 *
 * Description:	Read and tokenize and standard input stream.
 */
/*
int main()
{
    string lexbuf;

    while (lexan(lexbuf) != 0)
		continue;

    return 0;
}
*/