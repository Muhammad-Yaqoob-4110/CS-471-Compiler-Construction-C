
#ifndef ASSEMBLY_CODE_GENERATOR_H
#define ASSEMBLY_CODE_GENERATOR_H
#include<unordered_set>
#include <regex>
#include<string>
#include<iostream>

using namespace std;

class AssemblyCodeGenerator {
    public:
        vector<string> assemblyCode;
        unordered_set<string> definedVariables;

        void generateAssembly(const vector<string>& tacInstructions);

        void printAssembly() const;

        void saveInstructionsToFile(const string &filename);

    private:
        void collectVariables(const vector<string>& instructions);

        void extractVariablesFromInstruction(const string& instr);

        bool isNumeric(const string& str);

        void processAssignment(const string& instr);

        void translateBinaryOp(const string& lhs, const string& rhs, const string& op);

        void processConditional(const string& instr);

        void processLabel(const string& instr);

        void processGoto(const string& instr);


        void addProgramExit();
    };

#endif