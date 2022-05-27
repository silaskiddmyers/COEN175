/*
 * File:	generator.h
 *
 * Description:	This file contains the function declarations for the code
 *		generator for Simple C.  Most of the function declarations
 *		are actually member functions provided as part of Tree.h.
 */

# ifndef GENERATOR_H
# define GENERATOR_H

#include "Label.h"
#include "machine.h"
#include "string.h"
#include "Tree.h"

using namespace std;

void generateGlobals(Scope *scope);
void assign(Expression *expr, Register *reg);
void load(Expression *expr, Register *reg);
void compute(Expression *result, Expression *left, Expression *right, const string &opcode);
Register *getreg();

# endif /* GENERATOR_H */
