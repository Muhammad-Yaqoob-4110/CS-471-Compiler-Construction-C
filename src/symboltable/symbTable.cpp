// symbTable.cpp
#include "symbTable.h"

void SymbolTable::declareVariable(const string &name, const string &type, Scope scope)
{
    if (symbolTable.find(name) != symbolTable.end())
    {
        throw runtime_error("Semantic error: Variable '" + name + "' is already declared.");
    }
    
    SymbolTableEntry entry;
    entry.name = name;
    entry.type = type;
    entry.scope = scope;
    
    symbolTable[name] = entry;
}

string SymbolTable::getVariableType(const string &name) const
{
    if (symbolTable.find(name) == symbolTable.end())
    {
        throw runtime_error("Semantic error: Variable '" + name + "' is not declared.");
    }
    return symbolTable.at(name).type;
}

Scope SymbolTable::getVariableScope(const string &name) const
{
    if (symbolTable.find(name) == symbolTable.end())
    {
        throw runtime_error("Semantic error: Variable '" + name + "' is not declared.");
    }
    return symbolTable.at(name).scope;
}

bool SymbolTable::isDeclared(const string &name) const
{
    return symbolTable.find(name) != symbolTable.end();
}

bool SymbolTable::isInitialized(const string &name) const
{
    if (symbolTable.find(name) == symbolTable.end())
    {
        throw runtime_error("Semantic error: Variable '" + name + "' is not declared.");
    }
    return symbolTable.at(name).isInitialized;
}

void SymbolTable::printSymbolTable() const
{
    // Define column widths
    const int nameWidth = 20;
    const int typeWidth = 15;
    const int scopeWidth = 15;
    const int valueWidth = 20;

    // Print top border
    cout << "+" << string(nameWidth + 2, '-') 
         << "+" << string(typeWidth + 2, '-')
         << "+" << string(scopeWidth + 2, '-')
         << "+" << string(valueWidth + 2, '-') << "+" << endl;

    // Print header row
    cout << "| " << left << setw(nameWidth) << "Variable Name"
         << " | " << left << setw(typeWidth) << "Type"
         << " | " << left << setw(scopeWidth) << "Scope"
         << " | " << left << setw(valueWidth) << "Value" 
         << " |" << endl;

    // Print header separator
    cout << "+" << string(nameWidth + 2, '-') 
         << "+" << string(typeWidth + 2, '-')
         << "+" << string(scopeWidth + 2, '-')
         << "+" << string(valueWidth + 2, '-') << "+" << endl;

    // Check if the symbol table is empty
    if (symbolTable.empty())
    {
        cout << "| " << setw(nameWidth + typeWidth + scopeWidth + valueWidth + 7) << "No symbols declared." << " |" << endl;
    }
    else
    {
        // Print each symbol
        for (const auto &entry : symbolTable)
        {
            string scopeStr;
            switch(entry.second.scope) {
                case Scope::GLOBAL: scopeStr = "GLOBAL"; break;
                case Scope::LOCAL: scopeStr = "LOCAL"; break;
                case Scope::PARAMETER: scopeStr = "PARAMETER"; break;
                case Scope::TEMPORARY: scopeStr = "TEMPORARY"; break;
            }

            string valueStr = "Uninitialized";
            if (entry.second.isInitialized) {
                try {
                    // Attempt to convert value to string based on type
                    if (holds_alternative<int>(entry.second.value))
                        valueStr = to_string(get<int>(entry.second.value));
                    else if (holds_alternative<float>(entry.second.value))
                        valueStr = to_string(get<float>(entry.second.value));
                    else if (holds_alternative<double>(entry.second.value))
                        valueStr = to_string(get<double>(entry.second.value));
                    else if (holds_alternative<string>(entry.second.value))
                        valueStr = get<string>(entry.second.value);
                    else if (holds_alternative<char>(entry.second.value))
                        valueStr = string(1, get<char>(entry.second.value));
                    else if (holds_alternative<bool>(entry.second.value))
                        valueStr = get<bool>(entry.second.value) ? "true" : "false";
                } catch (...) {
                    valueStr = "Error";
                }
            }

            cout << "| " << left << setw(nameWidth) << entry.first
                 << " | " << left << setw(typeWidth) << entry.second.type
                 << " | " << left << setw(scopeWidth) << scopeStr
                 << " | " << left << setw(valueWidth) << valueStr
                 << " |" << endl;
        }
    }

    // Print bottom border
    cout << "+" << string(nameWidth + 2, '-') 
         << "+" << string(typeWidth + 2, '-')
         << "+" << string(scopeWidth + 2, '-')
         << "+" << string(valueWidth + 2, '-') << "+" << endl;
}