#include "acg.h"
#include<vector>
#include<string>
#include<iostream>
#include <fstream>

using namespace std;

void AssemblyCodeGenerator::generateAssembly(const vector<string>& tacInstructions) {
    // Start with necessary assembly directives
    // assemblyCode.push_back("%include 'syscall.asm'  ; Include system call definitions");
    assemblyCode.push_back("section .data");
    // assemblyCode.push_back("    SYS_EXIT equ 1");
    // assemblyCode.push_back("    SYS_WRITE equ 4");
    // assemblyCode.push_back("    STDOUT equ 1");

    // Collect and declare variables
    collectVariables(tacInstructions);

    // Start text section
    assemblyCode.push_back("\nsection .text");
    assemblyCode.push_back("    global _start");
    assemblyCode.push_back("_start:");

    // Process each TAC instruction
    for (const auto& instr : tacInstructions) {
        if (instr.find(" = ") != string::npos) {
            processAssignment(instr);
        }
        else if (instr.find("if ") != string::npos || instr.find("agar ") != string::npos) {
            processConditional(instr);
        }
        else if (instr.find("goto") != string::npos) {
            processGoto(instr);
        }
        else if (instr.find(":") != string::npos) {
            processLabel(instr);
        }
        else if (!instr.empty()) {
            cerr << "Unsupported TAC instruction: " << instr << endl;
        }
    }

    // Add program exit
    // addProgramExit();
}

void AssemblyCodeGenerator::printAssembly() const {
    for (const auto& line : assemblyCode) {
        cout << line << endl;
    }
}

void AssemblyCodeGenerator::saveInstructionsToFile(const string &filename)
{
    // Open a .obj file in text mode
    ofstream outFile(filename);
    if (!outFile.is_open())
    {
        cerr << "Error: Unable to open file for writing!" << endl;
        return;
    }

    // Write each instruction to the file
    for (const auto &instr : assemblyCode)
    {
        outFile << instr << endl;
    }

    // Close the file
    outFile.close();
    cout << "Generated Assembly Code is saved to file: " << filename << endl;
}

void AssemblyCodeGenerator::collectVariables(const vector<string>& instructions) {
    for (const auto& instr : instructions) {
        // Extract variables from assignments and conditions
        extractVariablesFromInstruction(instr);
    }

    // Declare collected variables
    for (const auto& var : definedVariables) {
        if (isNumeric(var)) {
            // Skip numeric constants
            continue;
        }
        assemblyCode.push_back("    " + var + " dd 0");
    }
}

void AssemblyCodeGenerator::extractVariablesFromInstruction(const string& instr) {
    // Split instruction into tokens
    istringstream iss(instr);
    string token;
    while (iss >> token) {
        // Ignore keywords and operators
        if (token == "=" || token == "if" || token == "goto" || 
            token == "agar" || token == "+" || token == "-" || 
            token == "*" || token == "/" || 
            token.back() == ':') {
            continue;
        }

        // Skip if token is a numeric constant
        if (!isNumeric(token)) {
            definedVariables.insert(token);
        }
    }
}

bool AssemblyCodeGenerator::isNumeric(const string& str) {
    return str.find_first_not_of("0123456789") == string::npos;
}

void AssemblyCodeGenerator::processAssignment(const string& instr) {
    size_t eqPos = instr.find(" = ");
    string lhs = instr.substr(0, eqPos);
    string rhs = instr.substr(eqPos + 3);

    if (rhs.find("+") != string::npos)
        translateBinaryOp(lhs, rhs, "add");
    else if (rhs.find("-") != string::npos)
        translateBinaryOp(lhs, rhs, "sub");
    else if (rhs.find("*") != string::npos)
        translateBinaryOp(lhs, rhs, "imul");
    else if (rhs.find("/") != string::npos)
        translateBinaryOp(lhs, rhs, "idiv");
    else {
        // Simple assignment or constant
        assemblyCode.push_back("    mov dword [" + lhs + "], " + rhs);
    }
}

void AssemblyCodeGenerator::translateBinaryOp(const string& lhs, const string& rhs, const string& op) {
    size_t opPos = rhs.find_first_of("+-*/");
    string op1 = rhs.substr(0, opPos - 1);
    string op2 = rhs.substr(opPos + 2);

    assemblyCode.push_back("    mov eax, [" + op1 + "]");
    
    if (op == "idiv") {
        // Division requires special handling
        assemblyCode.push_back("    mov ebx, [" + op2 + "]");
        assemblyCode.push_back("    cdq  ; Sign extend for division");
        assemblyCode.push_back("    idiv ebx");
    } else {
        if (op == "imul") {
            assemblyCode.push_back("    imul eax, [" + op2 + "]");
        } else {
            // Addition or subtraction
            assemblyCode.push_back("    " + op + " eax, [" + op2 + "]");
        }
    }

    assemblyCode.push_back("    mov dword [" + lhs + "], eax");
}

void AssemblyCodeGenerator::processConditional(const string& instr) {
    // Support for both 'if' and 'agar' keywords
    size_t condStart = (instr.find("if ") != string::npos) ? instr.find("if ") : instr.find("agar ");
    size_t gotoPos = instr.find("goto");
    string condition = instr.substr(condStart + 3, gotoPos - condStart - 3);
    string label = instr.substr(gotoPos + 5);

    if (condition.find_first_of("<>=") == string::npos) {
        // Boolean condition
        assemblyCode.push_back("    cmp dword [" + condition + "], 1");
        assemblyCode.push_back("    je " + label);
    }
    else {
        // Relational condition
        size_t opPos = condition.find_first_of("<>=");
        string op1 = condition.substr(0, opPos - 1);
        string op2 = condition.substr(opPos + 2);
        string compOp = condition.substr(opPos, 1);

        assemblyCode.push_back("    mov eax, [" + op1 + "]");
        assemblyCode.push_back("    cmp eax, [" + op2 + "]");

        if (compOp == ">")
            assemblyCode.push_back("    jg " + label);
        else if (compOp == "<")
            assemblyCode.push_back("    jl " + label);
        else if (compOp == "=")
            assemblyCode.push_back("    je " + label);
    }
}

void AssemblyCodeGenerator::processLabel(const string& instr) {
    // Remove whitespace and ensure proper label format
    string trimmedLabel = regex_replace(instr, regex("^\\s+|\\s+$"), "");
    assemblyCode.push_back("\n" + trimmedLabel);
}

void AssemblyCodeGenerator::processGoto(const string& instr) {
    size_t gotoPos = instr.find("goto");
    string label = instr.substr(gotoPos + 5);
    assemblyCode.push_back("    jmp " + label);
}

void AssemblyCodeGenerator::addProgramExit() {
    // Add standard exit syscall
    assemblyCode.push_back("\n    ; Exit program");
    assemblyCode.push_back("    mov eax, SYS_EXIT");
    assemblyCode.push_back("    xor ebx, ebx  ; Exit code 0");
    assemblyCode.push_back("    int 0x80");
}