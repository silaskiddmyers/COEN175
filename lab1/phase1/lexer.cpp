/*
Silas Kidd Myers
COEN 175
Project 1: Lexical analysis
Description: performs a lexical analysis on Simple C files, reading the standard input and outputting a standard output
*/

#include <cctype>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
using namespace std;

set<string> keywords = {	"auto", "break", "case", "char", "const", "continue", "default", "do", 
						"double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return",
						"short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};

set<string> operators = {"=", "|", "||", "&&", "==", "!=", "<", ">", "<=", ">=", "+", "-", "*", "/", 
						"%", "&", "!", "++", "--", ".", "->", "(", ")", 
						"[", "]", "{", "}", ";", ":", ","};
/*
int isKeyword(string test){
	int i;
	for(i = 0; i < 32; i++){
		if( test == keywords[i])
			return 1;
	}
	return 0;		
}


int isOperator(string op_test){
	int i;
	for(i=0; i<30; i++){
		if(op_test == operators[i])
			return 1;
	}
	return 0;
}
*/

int isComment(char comm_test){
	if((comm_test == '/') && (cin.peek() == '*'))
		return 1;
	return 0;
}

int isSpace(char space_test){
	if((space_test == '\\') && ((cin.peek() == 't') || (cin.peek() == 'n') || (cin.peek() == 'f') || (cin.peek() == 'v') || (cin.peek() == 'r')))
		return 1;
	return 0;
}

int main(int argc, char* argv[] ){

	char first_char, temp_char;
	string temp_string;
	int i;
	

	while(!cin.eof())
	{
		first_char = cin.get();//read first character of file

		if(cin.eof())
			break;

		if(isSpace(first_char))
		{
			cin.get();
		}
		//token is digit
		else if( isdigit(first_char)) {
			temp_string = "";//start with empty string, append first character to string
			i = 1;
			temp_string += first_char;

			while(isdigit(cin.peek()))
			{
				temp_string += cin.get();
			}
			cout << "number:" << temp_string << "\n";
			//first_char = cin.get();//first_char is next char to be tested
		}

		//token is string
		else if(first_char == '"'){
			temp_string = "";//start with empty string, append first character to string
			i = 1;
			temp_string += first_char;

			while(cin.peek() != '"')
			{
				if(cin.peek() == '\\') {
					temp_string += cin.get();
					//temp_string += cin.get();
				}

				temp_string += cin.get();
			}
			temp_string += cin.get();//for final " mark

			cout << "string:" << temp_string << "\n";
			//first_char = cin.get();
		}

		//token is char
		else if(first_char == '\''){
			temp_string = "";
			temp_string += first_char;
			temp_string += cin.get();//place cursor after first char of token
			
			while(cin.peek() != '\'')
			{
				if(cin.peek() == '\\') {
					temp_string += cin.get();
				}
				temp_string += cin.get();
			}
			temp_string += cin.get();//for final " mark
			/*if(temp_char = '\\')//if char has a backslash 
				temp_string = '\'' + '\\' + cin.get() + '\'';//need extra get to get next character
			else
				temp_string = '\'' + temp_char + '\'';*/
			cout << "character:" << temp_string << "\n";
		}

		//token is a comment
		else if(isComment(first_char))
		{
			temp_char = cin.get();
			temp_char = cin.get();//put cursor after asterisk from comment

			while(true)
			{
				if((temp_char == '*') && (cin.peek() == '/'))
					break;
				else
					temp_char = cin.get();
			}
			cin.get();
			//first_char = cin.get(); //next get will be after last slash
		}

		//if token is operator
		else if( (!isalnum(first_char)) && (!isspace(first_char)) && (!isComment(first_char))) {
			temp_string = "";
			temp_string += first_char;
			temp_char = cin.peek();

			string operator_string = "";
			operator_string += first_char; 
			operator_string += temp_char;
			/*!isalnum(temp_char) && (!isspace(temp_char)) && (isComment(first_char) == 0)*/
			if(	(operators.count(operator_string) > 0)																	)
			//because code will be valid, there are no cases where a non-alphanumeric non-space character will not be a part of operator
			{
				temp_string += cin.get();
			}
			cout << "operator:" << temp_string << "\n";

			//first_char = cin.get();
		}

		//if token is either a key word or an identifier
		else if( isalpha(first_char) || (first_char == '_')){
			temp_string = "";
			temp_string += first_char;

			temp_char = cin.peek();
			while(isalnum(temp_char) || (temp_char == '_')){
				temp_string += cin.get();
				temp_char = cin.peek();
			}
			if(keywords.count(temp_string) > 0)
			{
				cout << "keyword:" << temp_string << "\n";
			}
			else
			{
				cout << "identifier:" << temp_string << "\n";
			}
			//first_char = cin.get();
		}
	}		
	return 0;
}