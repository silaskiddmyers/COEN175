#ifndef CHECKER_H
#define CHECKER_H

#include <set>
#include <iostream>
#include "symbol.h"
#include "scope.h"
#include "lexer.h"
#include "tokens.h"

typedef std::string string;


void openScope();
void closeScope();

//declarations and definitions
void declareVariable(const string &name, const Type &type);
void declareFunc(string name,Type type);
void defineFunc(string name, Type type);

// for IDs and structs
void checkID(string &id);
void checkIfStruct(string name,Type type);
void openStruct(const string &name);
void closeStruct(const string &name);

#endif /* CHECKER_H */