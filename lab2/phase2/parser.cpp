/*
Silas Kidd Myers

Description: Design a parser that dtermines order that tokens are executed.

*/


#include <map>
# include <iostream>
# include "lexer.h"
# include "tokens.h"

using namespace std;
//lookahead = la
string lexbuf;
int la;

void expression();
int specifier();
void pointers();
void statements();
void parameterList();

void error()
{
    cout << "Syntax error at " << lexbuf << endl;
    exit(1);
}

static void match(int t)
{
    //cout << "matching " << t << "la: " << la << endl;
    if(la == t)
    {
        la = lexan(lexbuf);
    }
    else  
        error();
}

void expressionList()
{
    //cout << "expList" << endl;
    expression();
    while(la == COMMA)
    {
        match(',');
        expression();
    }
}

void type(bool paren)
{
    //cout << "type" << endl;
    if(la == NUM)
    {
        match(NUM);
    }
    else if(la == ID)
    {
        match(ID);
    }
    else if(la == STRING)
    {
        match(STRING);
    }
    else if(la == CHARACTER)
    {
        match(CHARACTER);
    }
    else if(paren)
    {
        //match('(');
        expression();
        match(')');
    }

}

void topExpression(bool paren)
{
    //cout << "topExp" << endl;
    type(paren);
    while((la == ARROW) || (la == LBRACK ) || (la == DOT) || la == LPAREN)
    {   
        if(la == ARROW)
        {
            match(ARROW);
            match(ID);
            cout << "arrow" << endl;
        }
        else if(la == LBRACK)
        {
            match('[');
            expression();
            match(']');
            cout << "index" << endl;
        }
        else if(la == DOT)
        {
            match(DOT);
            match(ID);
            cout << "dot" << endl;
        }
        else if(la == LPAREN)
        {
            match('(');
            if(la != RPAREN)
                expressionList();
            match(')');
            cout << "call" << endl;
        }

    }
}

void unaryExpression()
{
    //cout << "unaryExp" << endl;
    if (la == ADDR) {
        match('&');
        unaryExpression();
        cout << "addr" << endl;
    }
    else if (la == STAR) {
        match('*');
        unaryExpression();
        cout << "deref" << endl;
    }
    else if (la == NOT) {
        match(NOT);
        unaryExpression();
        cout << "not" << endl;
    }
    else if (la == MINUS) {
        match('-');
        unaryExpression();
        cout << "neg" << endl;
    }
    else if (la == SIZEOF){ 
        //cout << "sizeof top" << endl;
        match(SIZEOF);
        if(la == '(')
        {
            match('(');
            if(la == CHAR || la == INT || la == STRUCT){
                specifier();
                pointers();
                match(')');
            }
            else
                topExpression(true);
        }
        else
            unaryExpression();
        cout << "sizeof" << endl;
    }
    else if (la == LPAREN){
        match('(');

        if((la == CHAR) || la == INT || la == STRUCT){
            specifier();
            pointers();
            match(')');
            unaryExpression();
            cout << "cast"  << endl;
        }
        else
            topExpression(true);
    }
    else{
        topExpression(false);
    }
    return;
    
}

void mdrExpression()
{
    //cout << "mdrExpr" << endl;
    unaryExpression();
    while(la == '*' || la == '/' || la == '%')
    {
        int temp = la;
        match(la);        
        unaryExpression();       
        if (temp == STAR) {
            cout << "mul" << endl;
        }
        else if (temp == DIV) {
            cout << "div" << endl;
        }
        else {
            cout << "rem" << endl;
        }

    }
}

void asExpression()
{
    //cout << "asExpr" << endl;
    mdrExpression();
    while(la == '+' || la == '-')
    {
        int temp = la;
        match(la);        

        mdrExpression();        
        if(temp == '+')
        {
            cout << "add" << endl;
        }
        else if(temp == '-')
        {
            cout << "sub" << endl;
        }

    }
}

void cmpExpression()
{
    //cout << "cmpExpr" << endl;
    asExpression();
    while((la == LTN) || (la == GTN) || (la == LEQ) || (la == GEQ)){
        int temp = la;
        match(la);
        asExpression();
        if(temp == LTN)
        {
            cout << "ltn" << endl;
        }
        else if(temp == GTN)
        {
            cout << "gtn" << endl;
        }
        else if(temp == LEQ)
        {
            cout << "leq" << endl;
        }
        else
        {
            cout << "geq" << endl;
        }
        

    }
}

void eqExpression()
{
    //cout << "eqExpr" << endl;
    cmpExpression();
    while((la == EQL) || (la == NEQ))
    {
        int temp = la;
        match(la);
        cmpExpression();
        if(temp == EQL)
        {
            cout << "eql" << endl;
        }
        else{
            cout << "neq" << endl;
        }
    }
}

void andExpression()
{
    //cout << "andExpr" << endl;
    eqExpression();
    while(la == AND)
    {
        //int temp = la;
        match(AND);
        eqExpression();
        cout << "and" << endl;

    }
}

void orExpression()
{
    //cout << "orExp" << endl;
    andExpression();
    while(la == OR)
    {
        //int temp = la;
        match(OR);
        andExpression();
        cout << "or" << endl;

    }
}

void expression()
{
    orExpression();
}

void assignment()
{
    //cout << "assignment" << endl;
    expression();
    while(la == '=')
    {
        match('=');
        expression();
    }
    return;
}

void pointers()
{
    //cout << "pointers" << endl;
    while(la == STAR)
    { 
        match('*');
    }

}

int specifier()
{
    //cout << "specifier" << endl;
    if(la == CHAR)
    {
        match(CHAR);
        return CHAR;
    }
    else if(la == INT)
    {
        match(INT);
        return INT;
    }
    match(STRUCT);
    match(ID);
    return STRUCT;

}

void declarator()
{
    //cout << "declarator" << endl;
    pointers();
    if(la == ID)
    {
        match(ID);
        if(la == '[') {
            match('[');
            match(NUM);
            match(']');
        }
    }

    else if(la == '(')
    {
        match('(');
        match('*');
        match(ID);
        match(')');
        match('(');
        match(')');
    }
}

void declaratorList(){
    //cout << "declaratorList" << endl;
    declarator();
    while(la == COMMA)
    {
        match(',');
        declarator();
    }
    
}

void declaration()
{
    //cout << "declaration" << endl;
    specifier();

    declaratorList();
    match(';');
}

void declarations(){
    //cout << "declarations" << endl;
    while(la == INT || la == CHAR ||la == STRUCT){
        declaration();
    }
}

void statement()
{
    //cout << "statement" << endl;
    if(la == LBRACE){
        match('{');
        declarations();
        statements();
        match('}');
    }
    else if(la == WHILE){
        match(WHILE);
        match('(');
        expression();
        match(')');
        statement();
    }
    else if(la == RETURN){
        match(RETURN);
        expression();
        match(';');
    }
    else if(la == FOR){
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
    else if(la == IF){
        match(IF);
        match('(');
        expression();
        match(')');
        statement();

        if(la == ELSE){
            match(ELSE);
            statement();
        }
    }
    else{
        assignment();
        if(la == ';')
            match(';');
    }
}

void statements()
{
    //cout << "statements" << endl;
    while (la != RBRACE) {
        statement();
    }
}

void parameter()
{
    //cout << "parameter" << endl;
    specifier();
    pointers();
    if(la == LPAREN)
    {
        match(LPAREN);
        match('*');
        match(ID);
        match(')');
        match('(');
        match(')');
    }
    else
        match(ID);
}

void parameters()
{
    //cout << "paramters" << endl;
    if(la == VOID)
    {
        match(VOID);
    }
    else
        parameterList();
}

void parameterList()
{
    //cout << "parameterList" << endl;
    parameter();
    while(la == ',')
    {
        match(',');
        parameter();
    }
}

void globalDeclarator()
{
    //cout << "globalDeclarator" << endl;
    pointers();
    if(la == LPAREN)
    {
        match('(');
        match('*');
        match(ID);
        match(')');
        match('(');
        match(')');
    }
    else{
        match(ID);
        if (la == LPAREN) {
            match('(');
            match(')');
        }
        else if (la == LBRACK) {
            match('[');
            match(NUM);
            match(']');
        }
    }
}

void remainingDecls()
{
    //cout << "remainingDecls" << endl;
    while(la == COMMA) {
        match(',');
        globalDeclarator();
    }
    match(';');
}

void fog()
{
    //cout << "fog" << endl;
    if(specifier() == STRUCT && la == '{')
    {
        match('{');
        declaration();
        declarations();
        match('}');
        match(';');

    }
    else{
        pointers();
        if (la == '('){
            match('(');
            match('*');
            match(ID);
            match(')');
            match('(');
            match(')');
            remainingDecls();
		} else {
			match(ID);
			
			if (la == '['){
				match('[');
				match(NUM);
				match(']');
				remainingDecls();
			} else if (la == '('){
				match('(');
				
				if (la == ')'){
					match(')');
					remainingDecls();
				} else {
					parameters();
					match(')');
					match('{');
					declarations();
					statements();
					match('}');
				}
			} else {
				remainingDecls();
			}
        }
    }
}

int main()
{
    la = lexan(lexbuf);
    while (la != DONE)
        fog();

}