#ifndef INTERMEDIATE_CODE_GENERATOR_H
#define INTERMEDIATE_CODE_GENERATOR_H
#include<string>
#include<vector>
using namespace std;

class IntermediateCodeGnerator
{
public:
    vector<string> instructions;
    int tempCount = 0;

    string newTemp();

    void addInstruction(const string &instr);

    void printInstructions();

    void saveInstructionsToFile(const string &filename);
};

#endif