#ifndef SCOPE_H
#define SCOPE_H

#include <vector>
# include <iostream>
# include <cassert>
# include "symbol.h"
# include "type.h"


typedef std::vector<Symbol *> Symbols;

class Scope {
    typedef std::string string;
    Scope *_enclosing;
    Symbols _symbols;

public:
    Scope(Scope *enclosing = nullptr);

    void insert(Symbol *symbol);
    Symbol *find(const string &name) const;
    Symbol *lookup(const string &name) const;
    void remove(const string &name);

    Scope *enclosing() const { return _enclosing; };
    const Symbols &symbols() const { return _symbols; };
};


#endif /* SCOPE_H */