
#include "icg.h"
#include<iostream>
using namespace std;
#include <fstream>

string IntermediateCodeGnerator:: newTemp()
{
    return "t" + to_string(tempCount++);
}

void IntermediateCodeGnerator:: addInstruction(const string &instr)
{
    instructions.push_back(instr);
}

void IntermediateCodeGnerator::printInstructions()
{
    // file open
    for (const auto &instr : instructions)
    {
        // write in file
        cout << instr << endl;
    }
}

void IntermediateCodeGnerator::saveInstructionsToFile(const string &filename)
{
    // Open a .obj file in text mode
    ofstream outFile(filename);
    if (!outFile.is_open())
    {
        cerr << "Error: Unable to open file for writing!" << endl;
        return;
    }

    // Write each instruction to the file
    for (const auto &instr : instructions)
    {
        outFile << instr << endl;
    }

    // Close the file
    outFile.close();
    cout << "Generated Intermediate Code is saved to file: " << filename << endl;
}