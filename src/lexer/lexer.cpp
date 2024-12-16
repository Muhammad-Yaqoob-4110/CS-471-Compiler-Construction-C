#include "./lexer.h"
#include<vector>
#include <iomanip>

using namespace std;

Lexer::Lexer(const string &src) : src(src), pos(0), lineNumber(1) {}

vector<Token> Lexer::tokenize()
{
    vector<Token> tokens;
    while (pos < src.size())
    {
        char current = src[pos];
        // Handle new line
        if (current == '\n')
        {
            lineNumber++;
            pos++;
            continue;
        }
        // Handle spaces
        if (isspace(current))
        {
            pos++;
            continue;
        }
        // Hanlde Preprocessor Directives
        if (current == '#')
        {
            size_t start = pos;
            while (pos < src.size() && src[pos] != '\n') pos++;
            string directive = src.substr(start, pos - start);
            tokens.push_back(Token{T_PREPROCESSOR, directive, lineNumber});
            continue;
        }
        
        // Handle ==
        if (current == '=' && pos + 1 < src.size() && src[pos + 1] == '=')
        {
            tokens.push_back(Token{T_EQ, "==", lineNumber});
            pos += 2;
            continue;
        }
        // Handle !=
        if (current == '!' && pos + 1 < src.size() && src[pos + 1] == '=')
        {
            tokens.push_back(Token{T_NE, "!=", lineNumber});
            pos += 2;
            continue;
        }
        // Handle <<
        if (current == '<' && pos + 1 < src.size() && src[pos + 1] == '<')
        {
            tokens.push_back(Token{T_STREAM_INSERTION_OPERATOR, "<<", lineNumber});
            pos += 2;
            continue;
        }
        // Handle >>
        if (current == '>' && pos + 1 < src.size() && src[pos + 1] == '>')
        {
            tokens.push_back(Token{T_EXTRACTION_OPERATOR, ">>", lineNumber});
            pos += 2;
            continue;
        }
        // Handle &&
        if (current == '&' && pos + 1 < src.size() && src[pos + 1] == '&')
        {
            tokens.push_back(Token{T_LOGICAL_AND, "&&", lineNumber});
            pos += 2;
            continue;
        }
        // Handle ||
        if (current == '|' && pos + 1 < src.size() && src[pos + 1] == '|')
        {
            tokens.push_back(Token{T_LOGICAL_OR, "||", lineNumber});
            pos += 2;
            continue;
        }

        // Handle <=
        if (current == '<' && pos + 1 < src.size() && src[pos + 1] == '=')
        {
            tokens.push_back(Token{T_LE, "<=", lineNumber});
            pos += 2;
            continue;
        }

        // Handle >=
        if (current == '>' && pos + 1 < src.size() && src[pos + 1] == '=')
        {
            tokens.push_back(Token{T_GE, ">=", lineNumber});
            pos += 2;
            continue;
        }

        // Check for comments, Single Line and MultiLine
        if (current == '/' && pos + 1 < src.size()) 
        {
            if (src[pos + 1] == '/' || src[pos + 1] == '*')
            {
                skipComments();
                continue;
            }
        }
        // Hanlde Int, Float, Double
        if (isdigit(current))
        {
            bool isFloat = false;
            string number = consumeNumber(isFloat);
            TokenType type = isFloat ? T_FLOAT : T_NUM;
            tokens.push_back(Token{type, number, lineNumber});
            continue;
        }

        // Handle String
        if (current == '"') // Start of a string
        {
            string strValue = consumeString();
            tokens.push_back(Token{T_STRING, strValue, lineNumber});
            continue;
        }
        // Handle Colon
        if (current == ':') 
        {
            tokens.push_back(Token{T_COLON, ":", lineNumber});
            pos ++;
            continue;
        }

        if (isalpha(current))
        {
            string word = consumeWord();
            TokenType type = T_ID;

            if (word == "int") type = T_INT;
            else if (word == "float") type = T_FLOAT;
            else if (word == "double") type = T_DOUBLE;
            else if (word == "string") type = T_STRING;
            else if (word == "char") type = T_CHAR;
            else if (word == "bool") type = T_BOOL;
            else if (word == "true") type = T_TRUE;
            else if (word == "false") type = T_FALSE;
            else if (word == "else") type = T_ELSE;
            else if (word == "return") type = T_RETURN;
            else if (word == "agar") type = T_AGAR;
            else if (word == "if") type = T_IF;
            else if (word == "magar") type = T_MAGAR;
            else if (word == "while") type = T_WHILE;
            else if (word == "for") type = T_FOR;
            else if (word == "switch") type = T_SWITCH;
            else if (word == "case") type = T_CASE;
            else if (word == "break") type = T_BREAK;
            else if (word == "continue") type = T_CONTINUE;
            else if (word == "default") type = T_DEFAULT;
            else if (word == "do") type = T_DO;
            else if (word == "cout") type = T_STANDARD_OUTPUT_STREAM;
            else if (word == "cin") type = T_STARNDARD_INPUT_STREAM;
            else if (word == "void") type = T_VOID;

            tokens.push_back(Token{type, word, lineNumber});
            continue;
        }

        switch (current)
        {
        case '=': tokens.push_back(Token{T_ASSIGN, "=", lineNumber}); break;
        case '+': tokens.push_back(Token{T_PLUS, "+", lineNumber}); break;
        case '-': tokens.push_back(Token{T_MINUS, "-", lineNumber}); break;
        case '*': tokens.push_back(Token{T_MUL, "*", lineNumber}); break;
        case '/': tokens.push_back(Token{T_DIV, "/", lineNumber}); break;
        case '(': tokens.push_back(Token{T_LPAREN, "(", lineNumber}); break;
        case ')': tokens.push_back(Token{T_RPAREN, ")", lineNumber}); break;
        case '{': tokens.push_back(Token{T_LBRACE, "{", lineNumber}); break;
        case '}': tokens.push_back(Token{T_RBRACE, "}", lineNumber}); break;
        case ';': tokens.push_back(Token{T_SEMICOLON, ";", lineNumber}); break;
        case '>': tokens.push_back(Token{T_GT, ">", lineNumber}); break;
        case '<': tokens.push_back(Token{T_LT, "<", lineNumber}); break;
        default: cout << "Unexpected character: " << current << " at line " << lineNumber << endl; exit(1);
        }
        pos++;
    }
    tokens.push_back(Token{T_EOF, "", lineNumber});
    return tokens;
}

string Lexer::consumeNumber(bool &isFloat)
{
    size_t start = pos;
    bool hasDecimalPoint = false;
    while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.'))
    {
        if (src[pos] == '.')
        {
            if (hasDecimalPoint) break;
            hasDecimalPoint = true;
        }
        pos++;
    }
    if (pos < src.size() && (src[pos] == 'e' || src[pos] == 'E'))
    {
        isFloat = true;
        pos++;
        if (src[pos] == '+' || src[pos] == '-') pos++;
        while (pos < src.size() && isdigit(src[pos])) pos++;
    }
    isFloat = hasDecimalPoint || isFloat;
    return src.substr(start, pos - start);
}


string Lexer::consumeWord()
{
    size_t start = pos;
    while (pos < src.size() && isalnum(src[pos]))
        pos++;
    return src.substr(start, pos - start);
}

string Lexer::consumeString()
{
    size_t start = ++pos; // Skip the initial quote (")
    string result;

    while (pos < src.size())
    {
        char current = src[pos];

        if (current == '\\' && pos + 1 < src.size()) // Handle escape sequences
        {
            char next = src[pos + 1];
            if (next == '"' || next == '\\') // Escaped quote or backslash
            {
                result += next;
                pos += 2;
                continue;
            }
        }
        else if (current == '"') // End of string
        {
            pos++; // Move past the closing quote
            return result;
        }
        else
        {
            result += current;
            pos++;
        }
    }

    cout << "Unterminated string at line " << lineNumber << endl;
    exit(1); // Error: Unterminated string
}

void Lexer::skipComments()
{
    if (src[pos] == '/' && pos + 1 < src.size())
    {
        if (src[pos + 1] == '/')
        {
            pos += 2;
            while (pos < src.size() && src[pos] != '\n') pos++;
        }
        else if (src[pos + 1] == '*')
        {
            pos += 2;
            while (pos < src.size() && !(src[pos] == '*' && pos + 1 < src.size() && src[pos + 1] == '/'))
            {
                if (src[pos] == '\n') lineNumber++;
                pos++;
            }
            pos += 2; // Skip closing */
        }
    }
}

string Lexer::tokenTypeToString(TokenType type) {
    switch (type) {
        case T_INT: return "INT";
        case T_FLOAT: return "FLOAT";
        case T_DOUBLE: return "DOUBLE";
        case T_STRING: return "STRING";
        case T_BOOL: return "BOOL";
        case T_TRUE: return "TRUE";
        case T_FALSE: return "FALSE";
        case T_CHAR: return "CHAR";
        case T_ID: return "IDENTIFIER";
        case T_NUM: return "NUMBER";
        case T_IF: return "IF";
        case T_ELSE: return "ELSE";
        case T_RETURN: return "RETURN";
        case T_ASSIGN: return "ASSIGN";
        case T_PLUS: return "PLUS";
        case T_MINUS: return "MINUS";
        case T_MUL: return "MULTIPLY";
        case T_DIV: return "DIVIDE";
        case T_LPAREN: return "LEFT_PAREN";
        case T_RPAREN: return "RIGHT_PAREN";
        case T_LBRACE: return "LEFT_BRACE";
        case T_RBRACE: return "RIGHT_BRACE";
        case T_SEMICOLON: return "SEMICOLON";
        case T_GT: return "GREATER_THAN";
        case T_LT: return "LESS_THAN";
        case T_EOF: return "EOF";
        case T_AGAR: return "AGAR";
        case T_MAGAR: return "MAGAR";
        case T_WHILE: return "WHILE";
        case T_EQ: return "EQUALS";
        case T_NE: return "NOT_EQUALS";
        case T_LE: return "LESS_OR_EQUAL";
        case T_GE: return "GREATER_OR_EQUAL";
        case T_FOR: return "FOR";
        case T_SWITCH: return "SWITCH";
        case T_CASE: return "CASE";
        case T_BREAK: return "BREAK";
        case T_CONTINUE: return "CONTINUE";
        case T_PREPROCESSOR: return "PREPROCESSOR";
        case T_COLON: return "COLON";
        case T_DEFAULT: return "DEFAULT";
        case T_DO: return "DO";
        case T_STREAM_INSERTION_OPERATOR: return "S_I_O";
        case T_STANDARD_OUTPUT_STREAM: return "COUT";
        case T_EXTRACTION_OPERATOR: return "E_O";
        case T_STARNDARD_INPUT_STREAM: return "CIN";
        case T_LOGICAL_AND: return "LOGICAL_AND";
        case T_LOGICAL_OR: return "LOGICAL_OR";
        case T_VOID: return "VOID";
        default: return "UNKNOWN";
    }
}



void Lexer::printTokenizer(const vector<Token> &tokens)
{
    // Clear the screen
    // system("cls"); 

    // Define column widths
    const int typeWidth = 20;
    const int valueWidth = 30;
    const int lineWidth = 10;

    // Print top border
    cout << "+" << string(typeWidth + 2, '-') 
        << "+" << string(valueWidth + 2, '-') 
        << "+" << string(lineWidth + 2, '-') 
        << "+" << endl;

    // Print header row
    cout << "| " << left << setw(typeWidth) << "Token Type"
        << " | " << left << setw(valueWidth) << "Token Value"
        << " | " << left << setw(lineWidth) << "Line No."
        << " |" << endl;

    // Print header separator
    cout << "+" << string(typeWidth + 2, '-') 
        << "+" << string(valueWidth + 2, '-') 
        << "+" << string(lineWidth + 2, '-') 
        << "+" << endl;

    // Check if tokens are empty
    if (tokens.empty())
    {
        cout << "| " << left << setw(typeWidth + valueWidth + lineWidth + 7)
            << "No tokens available." << " |" << endl;
    }
    else
    {
        // Print each token
        for (const auto &token : tokens)
        {
            cout << "| " << left << setw(typeWidth) << tokenTypeToString(token.type)
                << " | " << left << setw(valueWidth) << ("\"" + token.value + "\"")
                << " | " << left << setw(lineWidth) << token.lineNumber
                << " |" << endl;
        }
    }

    // Print bottom border
    cout << "+" << string(typeWidth + 2, '-') 
        << "+" << string(valueWidth + 2, '-') 
        << "+" << string(lineWidth + 2, '-') 
        << "+" << endl;
}