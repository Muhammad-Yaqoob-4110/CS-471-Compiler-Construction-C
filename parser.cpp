#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>
#include <unordered_map>
#include <variant>

using namespace std;

enum TokenType {
    T_INT, T_FLOAT, T_DOUBLE, T_STRING, T_BOOL, T_CHAR, // datatypes 
    T_ID, T_NUM, T_IF, T_ELSE, T_RETURN, 
    T_AGAR, T_MAGAR,//  our own keywords
    T_AND, T_OR, T_EQ, T_NEQ, 
    T_WHILE, T_FOR, T_DO, T_TRUE, T_FALSE, // keywords
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,  
    T_SEMICOLON, T_GT, T_LT, T_GTE, T_LTE, T_NOT, T_EOF, 
};

struct Symbol {
    string type;
    variant<int, float, double, bool, char, string> value;
};

class SymbolTable {
private:
    unordered_map<string, Symbol> table;

public:
    void insert(const string &name, const string &type, const variant<int, float, double, bool, char, string> &value) {
        table[name] = {type, value};
    }

    Symbol* lookup(const string &name) {
        auto it = table.find(name);
        if (it != table.end()) {
            return &it->second;
        }
        return nullptr;
    }
};


struct Token {
    TokenType type;
    string value;
    int line;
};

class Lexer {
    private:
        string src;
        size_t pos;
        int line;

    public:
        Lexer(const string &src) {
            this->src = src;  
            this->pos = 0;   
            this->line = 1; 
        }

        vector<Token> tokenize() {
            vector<Token> tokens;
            while (pos < src.size()) {
                char current = src[pos];
                
                if (isspace(current)) {
                    if (current == '\n') line++;
                    pos++;
                    continue;
                }
                if (isdigit(current)) {
                    tokens.push_back(Token{T_NUM, consumeNumber(), line});
                    continue;
                }
                if (isalpha(current)) {
                    string word = consumeWord();
                    if (word == "int") tokens.push_back(Token{T_INT, word, line});
                    else if (word == "float") tokens.push_back(Token{T_FLOAT, word, line});
                    else if (word == "double") tokens.push_back(Token{T_DOUBLE, word, line});
                    else if (word == "string") tokens.push_back(Token{T_STRING, word, line});
                    else if (word == "bool") tokens.push_back(Token{T_BOOL, word, line});
                    else if (word == "char") tokens.push_back(Token{T_CHAR, word, line});
                    else if (word == "if") tokens.push_back(Token{T_IF, word, line});
                    else if (word == "agar") tokens.push_back(Token{T_AGAR, word, line});  // agar magar🎈
                    else if (word == "magar") tokens.push_back(Token{T_MAGAR, word, line});  // agar magar🎈
                    else if (word == "else") tokens.push_back(Token{T_ELSE, word, line});
                    else if (word == "return") tokens.push_back(Token{T_RETURN, word, line});
                    else if (word == "while") tokens.push_back(Token{T_WHILE, word, line});
                    else if (word == "for") tokens.push_back(Token{T_FOR, word, line});
                    else if (word == "do") tokens.push_back(Token{T_DO, word, line});
                    else if (word == "true") tokens.push_back(Token{T_TRUE, word,line});
                    else if (word == "false") tokens.push_back(Token{T_FALSE, word, line});
                    else tokens.push_back(Token{T_ID, word, line});
                    continue;
                }
                 if (current == '&' && src[pos + 1] == '&') {
                    tokens.push_back(Token{T_AND, "&&", line});
                    pos += 2; // Consume both '&' characters
                    continue;
                }
                if (current == '|' && src[pos + 1] == '|') {
                    tokens.push_back(Token{T_OR, "||", line});
                    pos += 2; // Consume both '|' characters
                    continue;
                }
                if (current == '=') {
                    if (src[pos + 1] == '=') {
                        tokens.push_back(Token{T_EQ, "==", line});
                        pos += 2; // Consume both '=' characters
                        continue;
                    }
                }
                if (current == '!') {
                    if (src[pos + 1] == '=') {
                        tokens.push_back(Token{T_NEQ, "!=", line});
                        pos += 2; // Consume both '!' and '=' characters
                        continue;
                    }
                }

                if (current == '/' && src[pos + 1] == '/') {
                    // Skip until the end of the line
                    pos += 2;
                    while (pos < src.size() && src[pos] != '\n') pos++;
                    continue;
                }

                if (current == '<' && peek() == '=') {
                    tokens.push_back(Token{T_LTE, "<=", line});
                    advance(2);
                    continue;
                }

                if (current == '=' && peek() == '=') {
                    tokens.push_back(Token{T_EQ, "==", line});
                    advance(2);
                    continue;
                }

                if (current == '!' && peek() == '=') {
                    tokens.push_back(Token{T_NEQ, "!=", line});
                    advance(2);
                    continue;
                }

                switch (current) {
                        case '=': tokens.push_back(Token{T_ASSIGN, "=", line}); break;
                        case '+': tokens.push_back(Token{T_PLUS, "+", line}); break;
                        case '-': tokens.push_back(Token{T_MINUS, "-", line}); break;
                        case '*': tokens.push_back(Token{T_MUL, "*", line}); break;
                        case '/': tokens.push_back(Token{T_DIV, "/", line}); break;
                        case '(': tokens.push_back(Token{T_LPAREN, "(", line}); break;
                        case ')': tokens.push_back(Token{T_RPAREN, ")", line}); break;
                        case '{': tokens.push_back(Token{T_LBRACE, "{", line}); break;  
                        case '}': tokens.push_back(Token{T_RBRACE, "}", line}); break;  
                        case ';': tokens.push_back(Token{T_SEMICOLON, ";", line}); break;
                        case '>': tokens.push_back(Token{T_GT, ">", line}); break;
                        case '<': tokens.push_back(Token{T_LT, "<", line}); break;
                        case '!': tokens.push_back(Token{T_NOT, "!", line}); break;
                        
                        break;
                        default: cout << "Unexpected character: " << current << " at line " << line << endl; exit(1);
                }
                pos++;
            }
            tokens.push_back(Token{T_EOF, "", line});
            return tokens;
        }
        char peek() {
        return (pos + 1 < src.size()) ? src[pos + 1] : '\0';
        }

        void advance(size_t steps = 1) {
            pos += steps;
        }


        string consumeNumber() {
            size_t start = pos;
            while (pos < src.size() && isdigit(src[pos])) pos++;
            return src.substr(start, pos - start);
        }

        string consumeWord() {
            size_t start = pos;
            while (pos < src.size() && isalnum(src[pos])) pos++;
            return src.substr(start, pos - start);
        }
};


class Parser {
 

public:
    Parser(const vector<Token> &tokens) {
        this->tokens = tokens;  
        this->pos = 0;          
    }

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        // cout << "Parsing completed successfully! No Syntax Error" << endl;
        // Print the generated TAC
        cout << "Generated Three-Address Code:" << endl;
        for (const string &instruction : tac) {
            cout << instruction << endl;
        }
    }

private:
    vector<Token> tokens;
    size_t pos;
    SymbolTable symbolTable;
    vector<string> tac;  // Store three-address code
    int tempCounter;     // Counter for temporary variables

    string newTemp() {
        return "t" + to_string(tempCounter++);
    }


    void parseStatement() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_DOUBLE || 
        tokens[pos].type == T_STRING || tokens[pos].type == T_BOOL || tokens[pos].type == T_CHAR) {
            parseDeclaration();
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_IF) {
            parseIfStatement();
        } else if (tokens[pos].type == T_AGAR) {  // agar magar
            parseAgarStatement();
        } else if (tokens[pos].type == T_RETURN) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_FOR) { // let's handle for loop
            parseForStatement();
        }
        else if (tokens[pos].type == T_WHILE) {
            parseWhileStatement();
        }  else if (tokens[pos].type == T_DO) { 
            parseDoWhileStatement();
        } else if (tokens[pos].type == T_LBRACE) {  
                parseBlock();
        } else {
            cout << "Syntax error: unexpected token '" << tokens[pos].value << "' at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void parseBlock() {
        expect(T_LBRACE);  
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);  
    }
    void parseDeclaration() {
        string type = tokens[pos].value;
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_DOUBLE || 
            tokens[pos].type == T_STRING || tokens[pos].type == T_BOOL || tokens[pos].type == T_CHAR) {
            pos++;
        } else {
            cout << "Syntax error: expected a type but found" << tokens[pos].type << "' at line " << tokens[pos].line << endl;
            // cout << "Syntax error: expected a type but found " << tokens[pos].value << endl;
            exit(1);
        }

        // expect(T_ID);

        if (tokens[pos].type == T_ID) {
            string varName = tokens[pos].value;
            pos++;

            expect(T_SEMICOLON);
            variant<int, float, double, bool, char, string> initialValue;
            if (type == "int") initialValue = 0;
            else if (type == "float") initialValue = 0.0f;
            else if (type == "double") initialValue = 0.0;
            else if (type == "bool") initialValue = false;
            else if (type == "char") initialValue = '\0';
            else if (type == "string") initialValue = "";

            symbolTable.insert(varName, type, initialValue);
        } else {
            cerr << "Syntax error: expected an identifier after type at line " << tokens[pos].line << endl;
            exit(1);
        }
        // expect(T_SEMICOLON);
    }

    void parseAssignment() {
        string varName = tokens[pos].value;
        expect(T_ID);
        expect(T_ASSIGN);
        string value = parseExpression();
        
        expect(T_SEMICOLON);

        tac.push_back(varName + " = " + value);

    }

    void parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();  
        if (tokens[pos].type == T_ELSE) {
            expect(T_ELSE);
            parseStatement();  
        }
    }

    void parseAgarStatement() {
        expect(T_AGAR);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();  
        if (tokens[pos].type == T_MAGAR) {
            expect(T_MAGAR);
            parseStatement();  
        }
    }

    void parseReturnStatement() {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }
// 3AC
    string parseExpression() {
        string left = parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            string op = tokens[pos].value;
            pos++;
            string right = parseTerm();
            string temp = newTemp();
            tac.push_back(temp + " = " + left + " " + op + " " + right);
            left = temp;
        }
        // Handle relational operators (e.g., >)
        if (tokens[pos].type == T_GT || tokens[pos].type == T_LT || 
            tokens[pos].type == T_GTE || tokens[pos].type == T_LTE ||
            tokens[pos].type == T_EQ || tokens[pos].type == T_NEQ) {
            string op = tokens[pos].value;  // Capture the relational operator
            pos++;
            string right = parseExpression();  // Parse the right-hand side expression
            string temp = newTemp();  // Generate a temporary variable for TAC
            tac.push_back(temp + " = " + left + " " + op + " " + right);
            left = temp;  // Update the left operand to the result of the comparison
        }
            
        return left;
    }
// 3AC
    string parseTerm() {
        string left = parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            string op = tokens[pos].value;
            pos++;
            string right = parseFactor();
            string temp = newTemp();
            tac.push_back(temp + " = " + left + " " + op + " " + right);
            left = temp;
        }
        return left;
    }

// 3AC
    string parseFactor() {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_ID) {
            string value = tokens[pos].value;
            pos++;

            return value;
        } else if (tokens[pos].type == T_TRUE || tokens[pos].type == T_FALSE) {
            string value = tokens[pos].value; 
            pos++;
            return value;
            
        } else if (tokens[pos].type == T_LPAREN) {
            expect(T_LPAREN);
            string value = parseExpression();
            expect(T_RPAREN);
            return value;
        } else {
            cout << "Syntax error: unexpected token '" << tokens[pos].value << "' at line " << tokens[pos].line << endl;
            // cout << "Syntax error: unexpected token " << tokens[pos].value << endl;
            exit(1);
        }
    }

    string parseIncrementDecrement(){
        if (tokens[pos].type == T_ID) {
            string var = tokens[pos].value;
            expect(T_ID);
            if (tokens[pos].type == T_PLUS && tokens[pos + 1].type == T_PLUS) {
                string incrementCode = var + " = " + var + " + 1"; // TAC for increment
                tac.push_back(incrementCode);
                pos += 2;
                return incrementCode;
            } else if (tokens[pos].type == T_MINUS && tokens[pos + 1].type == T_MINUS) {
                string decrementCode = var + " = " + var + " - 1"; // TAC for decrement
                tac.push_back(decrementCode);
                pos += 2;
                return decrementCode;
            } else if (tokens[pos].type == T_ASSIGN) {
                pos++;
                string expr = parseExpression();
                string assignmentCode = var + " = " + expr; // TAC for assignment
                tac.push_back(assignmentCode);
                return assignmentCode; // Return the TAC for assignment
            } else {
                cout << "Syntax error: invalid increment/decrement in 'for' loop at line " << tokens[pos].line << endl;
                exit(1);
            }
        } else {
            cout << "Syntax error: expected increment/decrement expression in 'for' loop at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void parseInitialization(){
        if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else {
            cout << "Syntax error: expected initialization statement in 'for' loop at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void expect(TokenType type) {
        if (tokens[pos].type == type) {
            pos++;
        } else {
            cout << "Syntax error: unexpected token '" << tokens[pos].value << "' at line " << tokens[pos].line << endl;
            // cout << "Syntax error: expected " << type << " but found " << tokens[pos].value << endl;
            exit(1);
        }
    }

    void parseDoWhileStatement() {    
    string startLabel = "L" + to_string(tempCounter++);
    string endLabel = "L" + to_string(tempCounter++);
    
    tac.push_back(startLabel + ":");
        expect(T_DO);
        parseStatement();
        expect(T_WHILE);
        expect(T_LPAREN);
        string condition = parseExpression();
        expect(T_RPAREN);
        expect(T_SEMICOLON);
        tac.push_back("if " + condition + " goto " + startLabel);
        tac.push_back(endLabel + ":");
    }

    void parseWhileStatement() {
        string startLabel = "L" + to_string(tempCounter++);
        string endLabel = "L" + to_string(tempCounter++);
        
        tac.push_back(startLabel + ":");
        expect(T_WHILE);
        expect(T_LPAREN);
        string condition = parseExpression();
        expect(T_RPAREN);

        tac.push_back("if " + condition + " goto " + endLabel);
        parseBlock();
        tac.push_back("goto " + startLabel);
        tac.push_back(endLabel + ":");
    }

    void parseForStatement() {
        string initLabel = "L" + to_string(tempCounter++);
        string startLabel = "L" + to_string(tempCounter++);
        string endLabel = "L" + to_string(tempCounter++);

        expect(T_FOR);
        expect(T_LPAREN);
        parseInitialization();
        tac.push_back(initLabel + ":");

        string condition = parseExpression();
        tac.push_back("if " + condition + " goto " + endLabel);

        expect(T_SEMICOLON);
        string incrementCode = parseIncrementDecrement();

        expect(T_RPAREN);
        
        tac.push_back(startLabel + ":");
        parseBlock();  // Parse the body of the loop
        tac.push_back(incrementCode);
        tac.push_back("goto " + initLabel);
        tac.push_back(endLabel + ":");
    }
    
};

int main(int argc, char *argv[])
{
    // Task  1
    // Check if the correct number of arguments is provided
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    // Open the file
    ifstream inputFile(argv[1]);
    if (!inputFile.is_open())
    {
        cerr << "Error opening file: " << argv[1] << endl;
        return 1;
    }

    // // Read and print the file content
    string input = "", line;
    while (getline(inputFile, line))
    {
        input += line + "\n";
    }

    // cout << input;

    // Close the file
    inputFile.close();

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}
