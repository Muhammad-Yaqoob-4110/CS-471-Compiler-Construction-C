#include<vector>
#include "../assembly/acg.h"
#include "../intermediate/icg.h"
#include "../symboltable/symbTable.h"
#include "../lexer/lexer.h"
#include "./parser.h"
#include<unordered_set>
#include<iostream>
#include<string>
using namespace std;


Parser::Parser(const vector<Token> &tokens, SymbolTable &symTable, IntermediateCodeGnerator &icg)
        : tokens(tokens), pos(0), symTable(symTable), icg(icg) {}

void Parser::parseProgram()
{
    while (tokens[pos].type != T_EOF)
    {
        parseStatement();
    }
}

void Parser::parseStatement()
{
    if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || 
        tokens[pos].type == T_DOUBLE || tokens[pos].type == T_STRING || 
        tokens[pos].type == T_CHAR || tokens[pos].type == T_BOOL)
    {
        parseDeclarationOrDeclarationAssignment();
    }
    else if (tokens[pos].type == T_ID)
    {
        parseAssignment();
    }
    else if (tokens[pos].type == T_VOID)
    {
        parseVoidFunction();
    }
    else if (tokens[pos].type == T_IF)
    {
        parseIfStatement();
    }
    else if (tokens[pos].type == T_SWITCH)
    {
        parseSwitchStatement();
    }
    else if (tokens[pos].type == T_RETURN)
    {
        parseReturnStatement();
    }
    else if (tokens[pos].type == T_LBRACE)
    {
        parseBlock();
    }
    else if (tokens[pos].type == T_AGAR)
    {
        parseAgarStatement();
    }
    else if (tokens[pos].type == T_WHILE)
    {
        parseWhileStatement();
    }
    else if (tokens[pos].type == T_FOR)
    {
        parseForStatement();
    }
    else if(tokens[pos].type == T_BREAK){
        parseBreakStatement();
    }
    else if (tokens[pos].type == T_DO)
    {
        parseDoWhileStatement();
    }
    else if (tokens[pos].type == T_STANDARD_OUTPUT_STREAM)
    {
        parsePrintStatement();
    }
    else if (tokens[pos].type == T_STARNDARD_INPUT_STREAM)
    {
        parseInputStatement();
    }
    else
    {
        cout << "Syntax error: unexpected token '" << tokens[pos].value << "' at line " << tokens[pos].lineNumber << endl;
        exit(1);
    }
}

void Parser::parseVoidFunction(){
    expect(T_VOID);
    expect(T_ID);
    expect(T_LPAREN);
    expect(T_RPAREN);
    parseBlock();
}
void Parser::parseInputStatement(){
    expect(T_STARNDARD_INPUT_STREAM);
    expect(T_EXTRACTION_OPERATOR);
    expect(T_ID);
    expect(T_SEMICOLON);
}
void Parser::parsePrintStatement(){
    expect(T_STANDARD_OUTPUT_STREAM);
    expect(T_STREAM_INSERTION_OPERATOR);
    expect(T_STRING);
    expect(T_SEMICOLON);
}
void Parser::parseDoWhileStatement()
{
    // Parse 'do' keyword
    expect(T_DO);

    // Generate start label for the do-while loop
    string startLabel = icg.newTemp() + "_do_while_start";
    icg.addInstruction(startLabel + ":");

    // Parse the body of the do-while loop
    parseBlock();

    // Expect 'while' keyword
    expect(T_WHILE);
    expect(T_LPAREN);

    // Generate label for condition check
    string conditionLabel = icg.newTemp() + "_do_while_condition";
    icg.addInstruction(conditionLabel + ":");

    // Parse the condition expression
    string conditionExpr = parseExpression();
    expect(T_RPAREN);
    expect(T_SEMICOLON);

    // Generate condition check instruction
    string conditionTemp = icg.newTemp();
    icg.addInstruction(conditionTemp + " = " + conditionExpr);

    // Generate conditional jump back to start of loop
    string endLabel = icg.newTemp() + "_do_while_end";
    icg.addInstruction("if !" + conditionTemp + " goto " + endLabel);
    icg.addInstruction("goto " + startLabel);
    icg.addInstruction(endLabel + ":");
}

void Parser::parseBreakStatement(){
    expect(T_BREAK);
    expect(T_SEMICOLON);
}
void Parser::parseSwitchStatement()
{
    // Parse 'switch' keyword
    expect(T_SWITCH);
    expect(T_LPAREN);
    
    // Parse the switch expression (what we're switching on)
    string switchExpr = parseExpression();
    expect(T_RPAREN);
    
    // Expect opening brace of switch block
    expect(T_LBRACE);
    
    // Flag to track if default case has been seen
    bool hasDefaultCase = false;
    
    // Generate labels for switch statement
    string endSwitchLabel = icg.newTemp() + "_switch_end";
    
    // Parse cases
    while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF)
    {
        if (tokens[pos].type == T_CASE)
        {
            // Parse case
            expect(T_CASE);
            
            // Parse case expression (can be a literal or constant expression)
            string caseExpr = parseExpression();
            
            // Expect colon after case
            expect(T_COLON);
            
            // Generate a unique label for this case
            string caseLabel = icg.newTemp() + "_case";
            
            // Add comparison instruction
            string compareTemp = icg.newTemp();
            icg.addInstruction(compareTemp + " = " + switchExpr + " == " + caseExpr);
            
            // Add conditional jump
            string nextCaseLabel = icg.newTemp() + "_next_case";
            icg.addInstruction("if !" + compareTemp + " goto " + nextCaseLabel);
            
            // Parse statements in this case block
            while (tokens[pos].type != T_CASE && 
                   tokens[pos].type != T_DEFAULT && 
                   tokens[pos].type != T_RBRACE)
            {
                parseStatement();
            }
            
            // Add unconditional jump to end of switch
            icg.addInstruction("goto " + endSwitchLabel);
            
            // Label for next case
            icg.addInstruction(nextCaseLabel + ":");
        }
        else if (tokens[pos].type == T_DEFAULT)
        {
            // Ensure only one default case
            if (hasDefaultCase) {
                cout << "Syntax error: Multiple default cases in switch statement" << endl;
                exit(1);
            }
            
            expect(T_DEFAULT);
            expect(T_COLON);
            
            hasDefaultCase = true;
            
            // Parse statements in default case block
            while (tokens[pos].type != T_RBRACE)
            {
                parseStatement();
            }
        }
        else
        {
            break;
        }
    }
    
    // Add end switch label
    icg.addInstruction(endSwitchLabel + ":");
    
    // Close switch block
    expect(T_RBRACE);
}


string Parser::parseIncrementDecrement(){
    if (tokens[pos].type == T_ID) {
        string var = tokens[pos].value;
        expect(T_ID);
        if (tokens[pos].type == T_PLUS && tokens[pos + 1].type == T_PLUS) {
            string incrementCode = var + " = " + var + " + 1"; // TAC for increment
            icg.addInstruction(incrementCode);
            pos += 2;
            return incrementCode;
        } else if (tokens[pos].type == T_MINUS && tokens[pos + 1].type == T_MINUS) {
            string decrementCode = var + " = " + var + " - 1"; // icg for decrement
            icg.addInstruction(decrementCode);
            pos += 2;
            return decrementCode;
        } else if (tokens[pos].type == T_ASSIGN) {
            pos++;
            string expr = parseExpression();
            string assignmentCode = var + " = " + expr; // icg for assignment
            icg.addInstruction(assignmentCode);
            return assignmentCode; // Return the icg for assignment
        } else {
            cout << "Syntax error: invalid increment/decrement in 'for' loop at line " << tokens[pos].lineNumber << endl;
            exit(1);
        }
    } else {
        cout << "Syntax error: expected increment/decrement expression in 'for' loop at lineNumber " << tokens[pos].lineNumber << endl;
        exit(1);
    }
}

void Parser::parseInitialization(){
    if (tokens[pos].type == T_ID) {
        parseAssignment();
    } else {
        cout << "Syntax error: expected initialization statement in 'for' loop at line " << tokens[pos].lineNumber << endl;
        exit(1);
    }
}

void Parser::parseForStatement() {
    string initLabel = "L" + to_string(icg.tempCount++);
    string startLabel = "L" + to_string(icg.tempCount++);
    string endLabel = "L" + to_string(icg.tempCount++);

    // for (i = 0; i < 5; i = i + 1){}
    expect(T_FOR);
    expect(T_LPAREN);

    // parseInitialization();
    parseDeclarationOrDeclarationAssignment();
    icg.addInstruction(initLabel + ":");

    string condition = parseExpression();
    icg.addInstruction("if " + condition + " goto " + endLabel);

    expect(T_SEMICOLON);
    string incrementCode = parseIncrementDecrement();

    expect(T_RPAREN);
    
    icg.addInstruction(startLabel + ":");
    parseBlock();  // Parse the body of the loop
    icg.addInstruction(incrementCode);
    icg.addInstruction("goto " + initLabel);
    icg.addInstruction(endLabel + ":");
}



void Parser::parseWhileStatement()
{
    string startLabel = "L" + to_string(icg.tempCount++);
    string endLabel = "L" + to_string(icg.tempCount++);

    icg.addInstruction("goto " + startLabel);
    icg.addInstruction(startLabel + ":");

    expect(T_WHILE);
    expect(T_LPAREN);
    string condition = parseExpression();
    expect(T_RPAREN);

    string temp = icg.newTemp();
    icg.addInstruction(temp + " = " + condition);
    
    icg.addInstruction("if " + temp + " goto " + endLabel); 
    icg.addInstruction("goto " + startLabel);
    
    parseBlock();
    
    icg.addInstruction("goto " + startLabel);
    icg.addInstruction(endLabel + ":");
}


void Parser::parseAgarStatement()
{
    expect(T_AGAR);
    expect(T_LPAREN); 
    string cond = parseExpression();
    expect(T_RPAREN);

    string temp = icg.newTemp();
    icg.addInstruction(temp + " = " + cond); 
    icg.addInstruction("agar " + temp + " goto L1");
    icg.addInstruction("goto L2");
    icg.addInstruction("L1:");

    parseStatement();

    if (tokens[pos].type == T_MAGAR)
    { // If an `magar` part exists, handle it.
        icg.addInstruction("goto L3");
        icg.addInstruction("L2:");
        expect(T_MAGAR);
        parseStatement();
        icg.addInstruction("L3:");
    }
    else
    {
        icg.addInstruction("L2:");
    }
}

void Parser::parseDeclarationOrDeclarationAssignment()
{
    // Determine the type of the variable
    string varType;
    switch(tokens[pos].type) {
        case T_INT:    varType = "int"; break;
        case T_FLOAT:  varType = "float"; break;
        case T_DOUBLE: varType = "double"; break;
        case T_STRING: varType = "string"; break;
        case T_CHAR:   varType = "char"; break;
        case T_BOOL:   varType = "bool"; break;
        default: 
            cout << "Unexpected type in declaration" << endl;
            exit(1);
    }
    
    // Consume the type token
    expect(tokens[pos].type);
    
    // Get the variable name
    string varName = expectAndReturnValue(T_ID);
    
    // Declare the variable in the symbol table
    symTable.declareVariable(varName, varType, Scope::GLOBAL);
    
    // Check if this is a declaration with assignment
    if (tokens[pos].type == T_ASSIGN)
    {
        // Consume the assignment token
        expect(T_ASSIGN);
        
        // Handle different types of assignments
        if(tokens[pos].type == T_STRING){
            string strValue = expectAndReturnValue(T_STRING);
            
            // Validate type compatibility
            if (varType != "string") {
                throw runtime_error("Type mismatch: Cannot assign string to " + varType);
            }
            
            // Set value in symbol table
            symTable.setVariableValue<string>(varName, strValue);
            
            // Generate intermediate code
            icg.addInstruction(varName + " = " + strValue);
        }
        else if(tokens[pos].type == T_TRUE || tokens[pos].type == T_FALSE) {
            // Handle boolean literals
            string boolValue = expectAndReturnValue(tokens[pos].type);
            
            // Validate type compatibility
            if (varType != "bool") {
                throw runtime_error("Type mismatch: Cannot assign boolean to " + varType);
            }
            
            // Convert string to bool
            bool boolVal = (boolValue == "true");
            
            // Set value in symbol table
            symTable.setVariableValue<bool>(varName, boolVal);
            
            // Generate intermediate code
            icg.addInstruction(varName + " = " + boolValue);
        }
        else {
            // For numeric and other types
            string expr = parseExpression();
            
            // Validate type compatibility would ideally happen in parseExpression()
            // For now, we'll just generate the assignment
            
            // Set value based on type
            if (varType == "int") {
                int intVal = stoi(expr);
                symTable.setVariableValue<int>(varName, intVal);
            }
            else if (varType == "float") {
                float floatVal = stof(expr);
                symTable.setVariableValue<float>(varName, floatVal);
            }
            else if (varType == "double") {
                double doubleVal = stod(expr);
                symTable.setVariableValue<double>(varName, doubleVal);
            }
            
            // Generate intermediate code
            icg.addInstruction(varName + " = " + expr);
        }
    }
    
    // Expect semicolon to end the statement
    expect(T_SEMICOLON);
}

void Parser::parseAssignment()
{
    string varName = expectAndReturnValue(T_ID);
    
    // Verify variable is declared
    if (!symTable.isDeclared(varName)) {
        throw runtime_error("Semantic error: Variable '" + varName + "' not declared before assignment");
    }
    
    // Get the variable's type for type checking
    string varType = symTable.getVariableType(varName);
    
    // Consume assignment token
    expect(T_ASSIGN);

    if(tokens[pos].type == T_TRUE || tokens[pos].type == T_FALSE) {
        // Handling of boolean literals
        string boolValue = expectAndReturnValue(tokens[pos].type);
        
        // Validate type compatibility
        if (varType != "bool") {
            throw runtime_error("Type mismatch: Cannot assign boolean to " + varType);
        }
        
        // Convert string to bool
        bool boolVal = (boolValue == "true");
        
        // Set value in symbol table
        symTable.setVariableValue<bool>(varName, boolVal);
        
        // Generate intermediate code
        icg.addInstruction(varName + " = " + boolValue);
    }
    else if(tokens[pos].type == T_STRING){
        string strValue = expectAndReturnValue(T_STRING);
        
        // Validate type compatibility
        if (varType != "string") {
            throw runtime_error("Type mismatch: Cannot assign string to " + varType);
        }
        
        // Set value in symbol table
        symTable.setVariableValue<string>(varName, strValue);
        
        // Generate intermediate code
        icg.addInstruction(varName + " = " + strValue);
    }
    else {
        // For numeric and other types
        string expr = parseExpression();
        
        // Set value based on type
        if (varType == "int") {
            // int intVal = stoi(expr);
            symTable.setVariableValue<string>(varName, expr);
        }
        else if (varType == "float") {
            // float floatVal = stof(expr);
            symTable.setVariableValue<string>(varName, expr);
        }
        else if (varType == "double") {
            // double doubleVal = stod(expr);
            symTable.setVariableValue<string>(varName, expr);
        }
        
        // Generate intermediate code
        icg.addInstruction(varName + " = " + expr);
    }
    
    // Expect semicolon to end the statement
    expect(T_SEMICOLON);
}

void Parser::parseIfStatement()
{
    expect(T_IF);
    expect(T_LPAREN);
    string cond = parseExpression();
    expect(T_RPAREN);

    string temp = icg.newTemp();
    icg.addInstruction(temp + " = " + cond);

    icg.addInstruction("if " + temp + " goto L1");
    icg.addInstruction("goto L2");
    icg.addInstruction("L1:");

    parseStatement();

    if (tokens[pos].type == T_ELSE)
    { // If an `else` part exists, handle it.
        icg.addInstruction("goto L3");
        icg.addInstruction("L2:");
        expect(T_ELSE);
        parseStatement();
        icg.addInstruction("L3:");
    }
    else
    {
        icg.addInstruction("L2:");
    }
}

void Parser::parseReturnStatement()
{
    expect(T_RETURN);
    string expr = parseExpression();
    icg.addInstruction("return " + expr);
    expect(T_SEMICOLON);
}

void Parser::parseBlock()
{
    expect(T_LBRACE);
    while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF)
    {
        parseStatement();
    }
    expect(T_RBRACE);
}

string Parser::parseExpression()
{
    string term = parseTerm();
    while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS)
    {
        TokenType op = tokens[pos++].type;
        string nextTerm = parseTerm();
        string temp = icg.newTemp();
        icg.addInstruction(temp + " = " + term + (op == T_PLUS ? " + " : " - ") + nextTerm);
        term = temp;
    }
        while (tokens[pos].type == T_GT || tokens[pos].type == T_LT || tokens[pos].type == T_EQ || tokens[pos].type == T_NE || tokens[pos].type == T_LE || tokens[pos].type == T_GE || tokens[pos].type == T_LOGICAL_AND || tokens[pos].type == T_LOGICAL_OR)
    {
        // pos++;
        TokenType op = tokens[pos++].type;
        string nextExpr = parseExpression();
        string temp = icg.newTemp();
        if (op == T_GT) {
        icg.addInstruction(temp + " = " + term + " > " + nextExpr);
        } else if (op == T_EQ) {
            icg.addInstruction(temp + " = " + term + " == " + nextExpr);
        } else if (op == T_LT) {
            icg.addInstruction(temp + " = " + term + " < " + nextExpr);
        } else if (op == T_NE) {
            icg.addInstruction(temp + " = " + term + " != " + nextExpr);
        } else if (op == T_LE) {
            icg.addInstruction(temp + " = " + term + " <= " + nextExpr);
        } else if (op == T_GE) {
            icg.addInstruction(temp + " = " + term + " >= " + nextExpr);
        } else if (op == T_LOGICAL_AND) {
            icg.addInstruction(temp + " = " + term + " && " + nextExpr);
        } else if (op == T_LOGICAL_OR) {
            icg.addInstruction(temp + " = " + term + " || " + nextExpr);
        }

        term = temp;
    }
    return term;
}

string Parser::parseTerm()
{
    string factor = parseFactor();
    while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV)
    {
        TokenType op = tokens[pos++].type;
        string nextFactor = parseFactor();
        string temp = icg.newTemp();
        icg.addInstruction(temp + " = " + factor + (op == T_MUL ? " * " : " / ") + nextFactor);
        factor = temp;
    }
    return factor;
}

string Parser::parseFactor()
{
    if (tokens[pos].type == T_NUM)
    {
        return tokens[pos++].value;
    }
    // Handle float literals, e.g., "20.09774"
    else if (tokens[pos].type == T_FLOAT) {
        return tokens[pos++].value;
    }
    else if (tokens[pos].type == T_ID)
    {
        return tokens[pos++].value;
    }
    else if (tokens[pos].type == T_TRUE || tokens[pos].type == T_FALSE)
    {
        return tokens[pos++].value;
    }
    else if (tokens[pos].type == T_LPAREN)
    {
        expect(T_LPAREN);
        string expr = parseExpression();
        expect(T_RPAREN);
        return expr;
    }
    else
    {
        cout << "Syntax error: unexpected token '" << tokens[pos].value << "' at line " << tokens[pos].lineNumber << endl;
        exit(1);
    }
}
void Parser::expect(TokenType type)
{
    if (tokens[pos].type != type)
    {
        cout << "Syntax error: expected '" << type << "' at line " << tokens[pos].lineNumber << endl;
        exit(1);
    }
    pos++;
}
string Parser::expectAndReturnValue(TokenType type)
{
    string value = tokens[pos].value;
    expect(type);
    return value;
}