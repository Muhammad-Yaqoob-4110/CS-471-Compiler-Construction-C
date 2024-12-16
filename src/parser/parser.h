#ifndef PARSER_H
#define PARSER_H
#include<vector>
#include "../assembly/acg.h"
#include "../intermediate/icg.h"
#include "../symboltable/symbTable.h"
#include "../lexer/lexer.h"
#include<string>
using namespace std;

class Parser
{
public:
    // Constructor
    Parser(const vector<Token> &tokens, SymbolTable &symTable, IntermediateCodeGnerator &icg);
    void parseProgram();

private:
    vector<Token> tokens;
    size_t pos;
    SymbolTable &symTable;
    IntermediateCodeGnerator &icg;

    void parseStatement();
    string parseIncrementDecrement();
    void parseInitialization();
    void parseForStatement();
    void parseWhileStatement();
    void parseAgarStatement();
    void parseDeclaration();
    void parseAssignment();
    void parseIfStatement();
    void parseReturnStatement();
    void parseBlock();
    string parseExpression();
    string parseTerm();
    string parseFactor();
    void expect(TokenType type);
    string expectAndReturnValue(TokenType type);
    void parseDeclarationOrDeclarationAssignment();
    void parseSwitchStatement();
    void parseBreakStatement();
    void parseDoWhileStatement();
    void parsePrintStatement();
    void parseInputStatement();
    void parseVoidFunction();

    // do while 
    // switch
    // single line declaration and assignment
    // Bool
    // User defined functions. 

    // FURTURE IMPROVEMETNS
    // Void function - without parameters
    // Void function - with parameter
    // Return type function - Withouot parameters
    // Return type function - With parameters
};

#endif
