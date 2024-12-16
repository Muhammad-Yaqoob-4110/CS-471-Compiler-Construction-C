
#include<iostream>
#include <fstream>
#include "./src/lexer/lexer.h"
#include "./src/parser/parser.h"
#include "./src/assembly/acg.h"

using namespace std;

int main(int argc, char *argv[])
{
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

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    lexer.printTokenizer(tokens);

    SymbolTable symTable;
    IntermediateCodeGnerator icg;
    Parser parser(tokens, symTable, icg);

    parser.parseProgram();
    symTable.printSymbolTable();

    // cout << "\nThree Address Code:" << endl;
    // icg.printInstructions();
    icg.saveInstructionsToFile("./target/icg.obj");

    AssemblyCodeGenerator acg;
    acg.generateAssembly(icg.instructions);

    // cout << "\nAssembly Code:" << endl;
    // acg.printAssembly();
    acg.saveInstructionsToFile("./target/assembly.asm");

    return 0;
}
