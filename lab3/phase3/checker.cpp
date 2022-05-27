#include "checker.h"

using namespace std;

static string redef = "redefinition of '%s'";
static string conflict = "conflicting types for '%s'";
static string redec = "redeclaration of '%s'";
static string undec = "'%s' undeclared";
static string pointer_type = "pointer type required for '%s'";
static string incomp_type = "'%s' has incomplete type";

static Scope *globalScope = nullptr;
static Scope *currentScope = nullptr;

static set<string> structures;

void openScope()
{
    // cout << "scope open\n";
    //  report("scope open");
    if (currentScope == nullptr)
    {
        currentScope = new Scope();
    }
    else
        currentScope = new Scope(currentScope);

    if (globalScope == nullptr)
    {
        globalScope = currentScope;
    }
}

void closeScope()
{
    // report("scope close");
    // cout << "scope close\n";

    Scope *temp = currentScope;
    currentScope = currentScope->enclosing();
    delete (temp);
}

void openStruct(const string &name)
{
    if (structures.count(name) > 0)
        report(redef, name);
    openScope();
}

void closeStruct(const string &name)
{
    closeScope();
    structures.insert(name);
}

void declareVariable(const string &name, const Type &type)
{ // given in class
    // cout << "declare " << name << " as (" << type.specifier() << ", " << type.indirection() << ')' << endl;

    Symbol *symbol = currentScope->find(name);
    if (symbol == nullptr)
    {
        checkIfStruct(name, type);
        symbol = new Symbol(name, type);
        currentScope->insert(symbol);
    }
    else if (currentScope->enclosing() != nullptr)
    {
        report(redec, name);
        return;
    }
    else if (type != symbol->type())
    {
        report(conflict, name);
        return;
    }
}

void declareFunc(string name, Type type)
{

    // cout << "declare " << name << " as (" << type.specifier() << ", " << type.indirection() << ", FUNCTION)" << endl;

    Symbol *symbol = globalScope->find(name);

    if (symbol == nullptr)
    {
        symbol = new Symbol(name, type);
        currentScope->insert(symbol);
    }
    else if (symbol->type() != type)
    {
        report(conflict, name);
    }
    else if (currentScope->enclosing() != nullptr)
    {
        report(redec, name);
    }
}

void defineFunc(string name, Type type)
{
    // cout << "define function " << name << endl;

    Symbol *symbol = currentScope->find(name);

    if (symbol != nullptr)
    {
        if (symbol->type().isFunction() && symbol->_defined)
        {
            report(redef, name);
        }
        else if (type != symbol->type())
        {
            // cout << type.specifier() << symbol->type().specifier();
            report(conflict, name);
        }
        currentScope->remove(name);
    }
    symbol = new Symbol(name, type, true);
    currentScope->insert(symbol);

    // Symbol *symbol = currentScope->find(name);

    // if (symbol == nullptr)
    // {
    //     symbol = new Symbol(name, type, true);
    //     currentScope->insert(symbol);
    // }
    // else if (symbol->type().parameters() == nullptr)
    // {
    //     currentScope->remove(name);
    //     symbol = new Symbol(name, type, true);
    //     currentScope->insert(symbol);
    // }
    // else
    // {
    //     //cout << type.specifier() << symbol->type().specifier();
    //     report(redef, name);
    // }
}

void checkIfStruct(string name, Type type)
{
    // cout << "check struct" << endl;
    if (type.isStruct() && type.indirection() == 0)
    {
        if (structures.count(type.specifier()) == 0)
            report(incomp_type, name);
        else if (type.isCallback())
            report(pointer_type, name);
    }
}

void checkID(string &id)
{
    // cout << "check id: " << id << endl;

    Symbol *symbol = currentScope->lookup(id);
    if (symbol == nullptr)
    {
        report(undec, id);
        // symbol = new Symbol(id, Type());
        // currentScope->insert(symbol);
    }
}