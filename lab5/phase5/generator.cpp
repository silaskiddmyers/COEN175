#include "generator.h"

using namespace std;

static ostream &operator <<(ostream &ostr, Expression *expr)
{
    expr->operand(ostr);
    return ostr;
}

void generateGlobals(const Scope *scope) {
    Symbols globals = scope->symbols();
    cout << "#Global Variables:" << endl;
    for (unsigned i = 0; i < globals.size(); i++) {
        if(!globals[i]->type().isFunction())
            cout << "\t.comm\t" << globals[i]->name() << ", " << globals[i]->type().size() << endl;
    }
}

void Procedure::generate(){
    cout << "#Procedure:" << endl;
    Symbols symbols = _body->declarations()->symbols();
    Parameters *params = _id->type().parameters();
    unsigned paramsOffset = 8;
    unsigned declsOffset = 0;
    unsigned i = 0;
    if(params != nullptr)
    {
        
        for(;i<params->size(); i++)
        {
            symbols[i]->_offset = paramsOffset;
            paramsOffset += symbols[i]->type().size();
        }
    }
    for(; i<symbols.size(); i++) {
            declsOffset -= symbols[i]->type().size();
            symbols[i]->_offset = declsOffset;
    }

    // cout << _id->name() << ":" << endl;
    // cout << "#Prepare Function:" << endl;
    // cout << "\tpush\t%ebp" << endl;
    // cout << "\tmovl\t%esp, %ebp" << endl;
    // cout << "\tsubl\t$" << -declsOffset << ", %esp" << endl;
    
    _body->generate();

    // cout << "#Close Function:" << endl;
    // cout << "\tmovl\t%ebp, %esp" << endl;
    // cout << "\tpopl\t%ebp" << endl;
    // cout << "\tret" << endl;
    // cout << "\t.globl\t" << _id->name() << endl;

}


void Block::generate(){
    cout << "#Block:" << endl;
    for (auto stmt : _stmts) {
        stmt->generate();
    }
}

void Simple::generate(){
    _expr->generate();
}

void Call::generate(){
    cout << "#Function Call:" << endl;
    for (int i = _args.size()-1; i >= 0; i--) { 
        _args[i]->generate();
        cout << "\tpushl\t" << _args[i] << endl;
    }
    cout << "\tcall\t" << _expr << endl;
}

void Assignment::generate(){
    cout << "#Assignment:" << endl;
    _right->generate();
    _left->generate();
    cout << "\tmovl\t" << _right << ", %eax" << endl;
    cout << "\tmovl\t%eax, " << _left << endl;
}


void Number::operand(ostream &ostr) const{
    ostr << "$" << _value;
}

void Identifier::operand(ostream &ostr) const{
    if (_symbol->_offset == 0) {//if global
        cout << _symbol->name();
    }
    else {
        cout << _symbol->_offset << "(%ebp)" << endl;
    }
}