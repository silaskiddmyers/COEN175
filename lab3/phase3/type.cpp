#include "type.h"
#include <cassert>
#include "iostream"

Type::Type(int kind, const string &specifier, unsigned indirection)
    : _kind(kind), _specifier(specifier), _indirection(indirection)
{
    _length = 0;
    _parameters = nullptr;
}

Type::Type(int kind, const string &specifier, unsigned indirection, Parameters* parameters)
    : _kind(kind), _specifier(specifier), _indirection(indirection), _parameters(parameters)
{
    _length = 0;
}

Type::Type(int kind, const string &specifier, unsigned indirection, unsigned length)
    : _kind(kind), _specifier(specifier), _indirection(indirection), _length(length)
{
    _parameters = nullptr;
}

Type::Type()
    : _kind(ERROR), _specifier("error"), _indirection(0)
{
    _length = 0;
    _parameters = nullptr;
}

bool Type::isStruct() const
{
    return _kind != ERROR && _specifier != "CHAR" && _specifier != "INT";
}

bool Type::operator!=(const Type &that) const {
    return !operator==(that);
}

bool Type::operator==(const Type &rhs) const {
    if (_kind != rhs._kind) { 
        return false;
    }
    if (_kind == ERROR) {
        return true;
    }
    if (_specifier != rhs._specifier) {
        return false;
    }
    if (_indirection != rhs._indirection) {
        return false;
    }
    if (_kind == SCALAR) {
        return true;
    }
    if (_kind == ARRAY) {
        return _length == rhs._length;
    }
    if (!_parameters || !rhs._parameters) {
        return true;
    }
    return *_parameters == *rhs._parameters;
}

std::ostream &operator <<(std::ostream &ostr, const Type &type) {
    ostr << "(" << type.specifier() << "," << type.indirection() << ",";

    if(type.isArray())
        ostr << "ARRAY";
    else if(type.isError())
        ostr << "ERROR";
    else if(type.isCallback())
        ostr << "CALLBACK";
    else if(type.isFunction())
        ostr << "FUNCTION";
    else {
        assert(type.isScalar());
        ostr << "SCALAR";
    }
    ostr << ")";
    return ostr;
}