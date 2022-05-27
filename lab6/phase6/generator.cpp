/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */
#include "generator.h"
#include <cassert>
#include <iostream>
#include <map>
using namespace std;

static int offset;
static string funcname;
static ostream &operator<<(ostream &ostr, Expression *expr);

static map<string, Label> strings;

static Register *eax = new Register("%eax", "%al");
static Register *ecx = new Register("%ecx", "%cl");
static Register *edx = new Register("%edx", "%dl");

static vector<Register *> registers = {eax, ecx, edx};

/*
 * Function:	align (private)
 *
 * Description:	Return the number of bytes necessary to align the given
 *		offset on the stack.
 */

static int align(int offset)
{
    if (offset % STACK_ALIGNMENT == 0)
        return 0;

    return STACK_ALIGNMENT - (abs(offset) % STACK_ALIGNMENT);
}

/*
 * Function:	operator << (private)
 *
 * Description:	Convenience function for writing the operand of an
 *		expression using the output stream operator.
 */

static ostream &operator<<(ostream &ostr, Expression *expr)
{
    if (expr->_register != nullptr)
        return ostr << expr->_register;

    expr->operand(ostr);
    return ostr;
}

/*
 * Function:	Expression::operand
 *
 * Description:	Write an expression as an operand to the specified stream.
 */

void Expression::operand(ostream &ostr) const
{
    //assert(_offset != 0);
    ostr << _offset << "(%ebp)";
}

/*
 * Function:	Identifier::operand
 *
 * Description:	Write an identifier as an operand to the specified stream.
 */

void Identifier::operand(ostream &ostr) const
{
    if (_symbol->_offset == 0)
        ostr << global_prefix << _symbol->name();
    else
        ostr << _symbol->_offset << "(%ebp)";
}

/*
 * Function:	Number::operand
 *
 * Description:	Write a number as an operand to the specified stream.
 */

void Number::operand(ostream &ostr) const
{
    ostr << "$" << _value;
}

void String::operand(ostream &ostr) const {
    Label str;
    if (strings.count(_value) == 0) {
        strings.insert(std::pair<string, Label> (_value, str));
    }
    else{
        str = strings.find(_value)->second;
    }
    ostr << str;
}

/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call expression.
 *
 * 		On a 32-bit Linux platform, the stack needs to be aligned
 * 		on a 4-byte boundary.  (Online documentation seems to
 * 		suggest that 16 bytes are required, but 4 bytes seems to
 * 		work and is much easier.)  Since all arguments are 4-bytes
 *		wide, the stack will always be aligned.
 *
 *		On a 32-bit OS X platform, the stack needs to aligned on a
 *		16-byte boundary.  So, if the stack will not be aligned
 *		after pushing the arguments, we first adjust the stack
 *		pointer.  However, this trick only works if none of the
 *		arguments are themselves function calls.
 *
 *		To handle nested function calls, we need to generate code
 *		for the nested calls first, which requires us to save their
 *		results and then push them on the stack later.  For
 *		efficiency, we only first generate code for the nested
 *		calls, but generate code for ordinary arguments in place.
 */

void Call::generate()
{
    unsigned numBytes;

    /* Generate code for any nested function calls first. */

    numBytes = 0;

    for (int i = _args.size() - 1; i >= 0; i--)
    {
        numBytes += _args[i]->type().size();

        if (STACK_ALIGNMENT != SIZEOF_REG && _args[i]->_hasCall)
            _args[i]->generate();
    }

    /* Align the stack if necessary. */

    if (align(numBytes) != 0)
    {
        cout << "\tsubl\t$" << align(numBytes) << ", %esp" << endl;
        numBytes += align(numBytes);
    }

    /* Generate code for any remaining arguments and push them on the stack. */

    for (int i = _args.size() - 1; i >= 0; i--)
    {
        if (STACK_ALIGNMENT == SIZEOF_REG || !_args[i]->_hasCall)
            _args[i]->generate();

        cout << "\tpushl\t" << _args[i] << endl;
        assign(_args[i], nullptr);
    }

    /* Call the function and then reclaim the stack space. */

    load(nullptr, eax);
    load(nullptr, ecx);
    load(nullptr, edx);

    if (_expr->type().isCallback())
    {
        _expr->generate();

        if (_expr->_register == nullptr)
            load(_expr, getreg());

        cout << "\tcall\t*" << _expr << endl;
        assign(_expr, nullptr);
    }
    else
        cout << "\tcall\t" << _expr << endl;

    if (numBytes > 0)
        cout << "\taddl\t$" << numBytes << ", %esp" << endl;

    assign(this, eax);
}

/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate()
{
    for (auto stmt : _stmts)
    {
        stmt->generate();

        //for (auto reg : registers)
            //assert(reg->_node == nullptr);
    }
}

/*
 * Function:	Simple::generate
 *
 * Description:	Generate code for a simple (expression) statement, which
 *		means simply generating code for the expression.
 */

void Simple::generate()
{
    _expr->generate();
    assign(_expr, nullptr);
}

/*
 * Function:	Procedure::generate
 *
 * Description:	Generate code for this function, which entails allocating
 *		space for local variables, then emitting our prologue, the
 *		body of the function, and the epilogue.
 */

void Procedure::generate()
{
    int param_offset;

    /* Assign offsets to the parameters and local variables. */

    param_offset = 2 * SIZEOF_REG;
    offset = param_offset;
    allocate(offset);

    /* Generate our prologue. */

    funcname = _id->name();
    cout << global_prefix << funcname << ":" << endl;
    cout << "\tpushl\t%ebp" << endl;
    cout << "\tmovl\t%esp, %ebp" << endl;
    cout << "\tsubl\t$" << funcname << ".size, %esp" << endl;

    /* Generate the body of this function. */

    _body->generate();

    /* Generate our epilogue. */

    cout << endl
         << global_prefix << funcname << ".exit:" << endl;
    cout << "\tmovl\t%ebp, %esp" << endl;
    cout << "\tpopl\t%ebp" << endl;
    cout << "\tret" << endl
         << endl;

    offset -= align(offset - param_offset);
    cout << "\t.set\t" << funcname << ".size, " << -offset << endl;
    cout << "\t.globl\t" << global_prefix << funcname << endl
         << endl;
}

static void findBaseAndOffset(Expression *expr, Expression *&base, int &offset)
{
    int field;
    base = expr;
    offset = 0;
    while (base->isField(base, field))
        offset += field;
    //cout << offset << endl;
}

/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(Scope *scope)
{
    const Symbols &symbols = scope->symbols();

    for (auto symbol : symbols)
        if (!symbol->type().isFunction())
        {
            cout << "\t.comm\t" << global_prefix << symbol->name() << ", ";
            cout << symbol->type().size() << endl;
        }

    
    cout << "\t.data\t" << endl;
    for (auto str : strings) {
        string temp = escapeString(str.first);
        cout << str.second << ":\t.asciz\t\"" << temp << "\"" << endl;
    }

}

/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for an assignment statement.
 *
 *		NOT FINISHED: Only works if the right-hand side is an
 *		integer literal and the left-hand side is an integer
 *		scalar.
 */

void Assignment::generate()
{
    _right->generate();
    int field_offset;
    Expression *base;
    findBaseAndOffset(_left, base, field_offset);
    if(_right->_register == nullptr)
        load(_right, getreg());

    if (base->isDereference(base))
    {
        base->generate();
        if(base->_register == nullptr)
            load(base, getreg());
        if (_right->type().size() == 1)
        {
            cout << "\tmovzbl\t" << _right << ", ";
            if(field_offset != 0)
                cout << field_offset; 
            cout << "(" << base << ")" << endl;
        }
        else
        {
            cout << "\tmovl\t" << _right << ", ";
            if(field_offset != 0)
                cout << field_offset;
            cout <<  "(" << base << ")" << endl;
        }
        assign(base, nullptr);
    }
    else
    {
        base->generate();
        if (_right->type().size() == 1)
        {
            cout << "\tmovzbl\t" << _right << ", " << field_offset << "+" << base << endl;
        }
        else
        {
            cout << "\tmovl\t" << _right << ", " << field_offset << "+" << base << endl;
        }
    }
    assign(_left, nullptr);
    assign(base, nullptr);
    assign(_right, nullptr);
}

void assign(Expression *expr, Register *reg)
{
    if (expr != nullptr)
    {
        if (expr->_register != nullptr)
            expr->_register->_node = nullptr;
        expr->_register = reg;
    }
    if (reg != nullptr)
    {
        if (reg->_node != nullptr)
            reg->_node->_register = nullptr;
        reg->_node = expr;
    }
}

Register *getreg()
{
    for (auto reg : registers)
        if (reg->_node == nullptr)
            return reg;
    load(nullptr, registers[0]);
    return registers[0];
}

void load(Expression *expr, Register *reg)
{
    //cout << "#load" << endl;
    if (reg->_node != expr)
    {
        if (reg->_node != nullptr)
        {
            unsigned n = reg->_node->type().size();
            offset -= n;
            reg->_node->_offset = offset;
            cout << (n == 1 ? "\tmovb\t" : "\tmovl\t");
            cout << reg << ", " << offset << "(%ebp)" << endl;
        }
        if(expr != nullptr) {
            unsigned n = expr->type().size();
            cout << (n==1 ? "\tmovb\t" : "\tmovl\t");
            cout << expr << ", " << reg->name(n) << endl;
        }
        assign(expr, reg);
    }
    //cout << "#endl load" << endl;
}

void Add::generate()
{
    _left->generate();
    _right->generate();
    if(_left->_register == nullptr)
        load(_left, getreg());
    cout << "\taddl\t" << _right << ", " << _left << endl;
    assign(_right, nullptr);
    assign(this, _left->_register);
}

void compute(Expression *result, Expression *left, Expression *right, const string &opcode)
{
    //cout << "#compute " << opcode << endl;
    left->generate();
    right->generate();
    if(left->_register == nullptr)
        load(left, getreg());
    cout << "\t" << opcode << "\t" << right << ", " << left << endl;

    assign(right, nullptr);
    assign(result, left->_register);
    //cout << "#end compute " << opcode << endl;
}

static void divide(Expression *result, Expression *left, Expression *right, Register *reg)
{

    left->generate();
    right->generate();
    unsigned number;

    if(left->_register == nullptr)
        load(left, getreg());
    load(nullptr, edx);

    if (right->isNumber(number))
    {
        load(right, ecx);
    }

    cout << "\tcltd\t" << endl;
    cout << "\tidivl\t" << right << endl;

    assign(right, nullptr);
    assign(left, nullptr);

    assign(result, reg);
}

static void compare(Expression *result, Expression *left, Expression *right, const string &opcode)
{
    //cout << "#compare: " << opcode << endl;
    left->generate();
    right->generate();
    if(left->_register == nullptr)
        load(left, getreg());

    cout << "\tcmpl\t" << right << ", " << left << endl;
    cout << "\t" << opcode << "\t" << left->_register->byte() << endl;
    cout << "\tmovzbl\t" << left->_register->byte() << ", " << left->_register << endl;

    assign(result, left->_register);
    //cout << "#end compare: " << opcode << endl;
}

void Cast::generate()
{
    //cout << "#cast" << endl;
    _expr->generate();
    if(_expr->_register == nullptr)
        load(_expr, getreg());
    if(_expr->type().size() == 1)
        cout << "\tmovsbl\t" << _expr->_register->byte() << "," << _expr << endl; 
    assign(this, _expr->_register);
    //cout << "#end cast" << endl;
}

void Not::generate()
{
    //cout << "#not" << endl;
    _expr->generate();
    if(_expr->_register == nullptr)
        load(_expr, getreg());
    cout << "\tcmpl\t"
         << "$0, " << _expr << endl;
    cout << "\tsete\t" << _expr->_register->byte() << endl;
    cout << "\tmovzbl\t" << _expr->_register->byte() << ", " << _expr << endl;

    assign(this, _expr->_register);
    //cout << "#end not" << endl;
}

void Negate::generate()
{
    //cout << "#negate" << endl;
    _expr->generate();
    if(_expr->_register == nullptr)
        load(_expr, getreg());
    cout << "\tnegl\t" << _expr << endl;

    assign(this, _expr->_register);
    //cout << "#end negate" << endl;
}

void Dereference::generate()
{
    cout << "#dereference" << endl;
    int field_offset;
    Expression *base;
    findBaseAndOffset(_expr, base, field_offset);
    base->generate();
    if(base->_register == nullptr)
        load(base, getreg());
    if (_type.size() == 1)
    {
        cout << "\tmovb\t" << field_offset << "(" << base << "), " << base << endl;
    }
    else
    {
        cout << "\tmovl\t" << field_offset << "(" << base << "), " << base << endl;
    }
    assign(this, base->_register);
    cout << "#end dereference" << endl;
}

void Address::generate()
{
    //cout << "#address" << endl;
    int field_offset;
    Expression *base, *temp;
    findBaseAndOffset(_expr, base, field_offset);

    if (_expr->isDereference(temp))
    {
        temp->generate();
        if(temp->_register == nullptr)
            load(temp, getreg());
        assign(this, temp->_register);
    }
    else
    {
        assign(this, getreg());

        cout << "\tleal\t";
        if(field_offset !=0)
            cout << field_offset << "+";
        cout << base << ", " << this << endl;
    }
    //cout << "#end address" << endl;
}

void Expression::test(const Label &label, bool ifTrue)
{
    //cout << "#test " << label << endl;
    generate();
    if (_register == nullptr)
        load(this, getreg());
    cout << "\tcmpl\t$0, " << this << endl;
    cout << (ifTrue ? "\tjne\t" : "\tje\t") << label << endl;
    assign(this, nullptr);
}

void While::generate()
{
    Label loop, exit;
    cout << loop << ":" << endl;
    _expr->test(exit, false);
    _stmt->generate();
    cout << "\tjmp\t" << loop << endl;
    cout << exit << ":" << endl;
}

void Field::generate() {
    //cout << "#field" << endl;
    int field_offset;
    Expression *base;
    findBaseAndOffset(this, base, field_offset);
    if(base->isDereference(base))
    {
        base->generate();
        if(base->_register == nullptr)
            load(base, getreg());

        if(_type.size() == 1)   cout << "\tmovb\t" << field_offset << " (" << base << "), ";
        else  cout << "\tmovl\t" << field_offset << " (" << base << "), ";
        assign(this, base->_register);
        cout << this << endl;
      
    }
    else {
        assign(this, getreg());
        //cout << this << endl;
        cout << "\tmovl\t" << field_offset << "+" << base << ", " << this << endl;
    }
    //cout << "#end field" << endl;
}

void LogicalOr::generate() {
  //cout << "#logical or" << endl;

    Label skip, shortCircuit;
    _left->test(skip, true);
    _right->test(skip, true);

    cout << "\tjmp\t" << shortCircuit << endl;

    assign(this, getreg());
    cout << skip << ":" << endl;
    cout << "\tmovl\t$1, " << this << endl;
    cout << shortCircuit << ":" << endl;
    //cout << "#end logical or" << endl;
}

void LogicalAnd::generate() {
    //cout << "#logical and" << endl;

    Label skip, shortCircuit;
    _left->test(skip, false);
    _right->test(skip, false);

    cout << "\tjmp\t" << shortCircuit << endl;

    assign(this, getreg());
    cout << skip << ":" << endl;
    cout << "\tmovl\t$0, " << this << endl;
    cout << shortCircuit << ":" << endl;
    //cout << "#end logical and" << endl;
}

void Return::generate() {
    ////cout << "#return statement" << endl;
    _expr->generate();
    if(_expr->_register != eax)
        load(_expr, eax);
    cout << "\tjmp\t" << funcname << ".exit" << endl;
    assign(_expr, nullptr);
    ////cout << "#end return statement" << endl;
}

void If::generate() {
    //cout << "#if statement" << endl;
    Label skip, exit;

    _expr->test(skip, false);
    _thenStmt->generate();

    if (_elseStmt != nullptr) {
        //cout << "#else statement" << endl;
        cout << "\tjmp\t" << exit << endl;
        cout << skip << ":" << endl;
        _elseStmt->generate();
        cout << exit << ":" << endl;
    }
    else {
        cout << skip << ":" << endl;
    }
    //cout << "#end if statement" << endl;
}

void For::generate() {
    //cout << "#for loop" << endl;
    Label loop, exit;
    _init->generate();
    cout << loop << ":" << endl;

    _expr->test(exit, false);
    _stmt->generate();
    _incr->generate();

    cout << "\tjmp\t" << loop << endl;
    cout << exit << ":" << endl;
    //cout << "#end for loop" << endl;
}

void Remainder::generate() {
    divide(this, _left, _right, edx);
}

void Divide::generate() {
    //cout << "#divide" << endl;
    divide(this, _left, _right, eax);
    //cout << "#end divide" << endl;
}

void Subtract::generate() {
    //cout << "#subtract" << endl;
    compute(this,_left, _right, "subl");
    //cout << "#end subtract" << endl;
}

void Multiply::generate() {
    //cout << "#multiply" << endl;
    compute(this, _left, _right, "imull");
    //cout << "#end multiply" << endl;
}

void LessThan::generate() {
    compare(this, _left, _right, "setl");
}

void GreaterThan::generate() {
    compare(this, _left, _right, "setg");
}

void LessOrEqual::generate() {
    compare(this, _left, _right, "setle");
}

void GreaterOrEqual::generate() {
    compare(this, _left, _right, "setge");
}

void Equal::generate() {
    compare(this, _left, _right, "sete");
}

void NotEqual::generate() {
    compare(this, _left, _right, "setne");
}

/*
void Number::generate() {

}

void Identifier::generate () {

}

void String::generate() {

}
*/
// Part A: front end middle end: regular expressions, NFA's/DFA's, parsers/grammars, scoping, types, optimization
// Part B: back end: storage allocation, code generation

// both worth equal points: 25 total points from first midterm, 75 that is new material

// Part A: 80 minutes
// break: 20 minutes
// Part B: 80 minutes
