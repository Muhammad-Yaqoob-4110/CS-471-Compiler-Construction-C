#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;


enum TokenType
{
    // T_LE <=
    // T_NE !=
    // T_EQ ==
    // T_GE >=
    // T_LT <
    // T_GT >
    T_INT, T_FLOAT, T_DOUBLE, T_STRING, T_BOOL, T_CHAR,
    T_ID,T_NUM,T_IF,T_ELSE,T_RETURN,T_ASSIGN,T_PLUS,
    T_MINUS,T_MUL,T_DIV,T_LPAREN,T_RPAREN,T_LBRACE,
    T_RBRACE,T_SEMICOLON,T_GT, T_LT ,T_EOF,
    T_AGAR,T_MAGAR,
    T_WHILE, T_EQ, T_NE, T_LE, T_GE,
    T_FOR, T_SWITCH, T_CASE, T_BREAK, T_CONTINUE, T_PREPROCESSOR,
    T_UNKNOWN, T_TRUE, T_FALSE, T_COLON, T_DEFAULT, T_DO,
    T_STREAM_INSERTION_OPERATOR, T_STANDARD_OUTPUT_STREAM, // << , cout
    T_EXTRACTION_OPERATOR, T_STARNDARD_INPUT_STREAM, // >>, cin
    T_LOGICAL_AND, T_LOGICAL_OR, T_VOID

};


struct Token
{
    TokenType type;
    string value;
    int lineNumber;
};

class Lexer
{
    private:
        string src;
        size_t pos;
        int lineNumber;
    public:
        Lexer(const string &src);
        vector<Token> tokenize();
        string consumeNumber(bool &isFloat);
        string consumeWord();
        string consumeString();
        void skipComments();
        string tokenTypeToString(TokenType type);
        void printTokenizer(const vector<Token> &tokens);
        

};


#endif
