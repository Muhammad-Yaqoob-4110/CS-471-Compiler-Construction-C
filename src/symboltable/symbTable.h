// symbTable.h
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <variant>
#include <stdexcept>
#include <iomanip>

using namespace std;

enum class Scope {
    GLOBAL,
    LOCAL,
    PARAMETER,
    TEMPORARY
};

class SymbolTableEntry {
public:
    string name;
    string type;
    Scope scope;
    variant<int, float, double, string, char, bool> value;
    bool isInitialized;

    SymbolTableEntry() : isInitialized(false) {}
    
    template<typename T>
    void setValue(T val) {
        value = val;
        isInitialized = true;
    }

    template<typename T>
    T getValue() const {
        if (!isInitialized) {
            throw runtime_error("Variable not initialized");
        }
        return get<T>(value);
    }
};

class SymbolTable {
public:
    void declareVariable(const string &name, const string &type, Scope scope = Scope::LOCAL);
    string getVariableType(const string &name) const;
    Scope getVariableScope(const string &name) const;
    
    template<typename T>
    void setVariableValue(const string &name, T value) {
        if (symbolTable.find(name) == symbolTable.end()) {
            throw runtime_error("Semantic error: Variable '" + name + "' is not declared.");
        }
        symbolTable[name].setValue(value);
    }

    template<typename T>
    T getVariableValue(const string &name) const {
        if (symbolTable.find(name) == symbolTable.end()) {
            throw runtime_error("Semantic error: Variable '" + name + "' is not declared.");
        }
        return symbolTable.at(name).getValue<T>();
    }

    bool isDeclared(const string &name) const;
    bool isInitialized(const string &name) const;
    void printSymbolTable() const;

private:
    map<string, SymbolTableEntry> symbolTable;
};

#endif // SYMBOL_TABLE_H