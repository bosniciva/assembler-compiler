/* Iva Bosnic RA222/2022
* Datum: 6.6.2024.
*/
#include <iostream>
#include <exception>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <stack>

#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "IR.h"
#include "Types.h"
#include "LivenessAnalysis.h"
#include "ResourceAllocation.h"

using namespace std;



static void printInstructionTable(SyntaxAnalysis& syntax) {
	auto instrucionsList = syntax.getInstructionsList();
	if (!instrucionsList.empty()) {
		cout << "Instruction Table:" << endl;
		cout << "--------------------------------------------------------------" << endl;
		cout << "| Position | Type      | Destination     | Source           |" << endl;
		cout << "--------------------------------------------------------------" << endl;

		for (Instruction* instr : instrucionsList) {
			cout << "| " << instr->getPosition() << "        ";
			cout << "| " << instr->getTypeAsString() << "  ";

			cout << "| ";
			for (Variable* var : instr->getDst()) {
				cout << var->getName() << " ";
			}
			cout << " ";
			for (int i = 0; i < 16 - instr->getDst().size(); i++) {
				cout << " ";
			}
			cout << "| ";

			for (Variable* var : instr->getSrc()) {
				cout << var->getName() << " ";
			}
			cout << " ";
			for (int i = 0; i < 16 - instr->getSrc().size(); i++) {
				cout << " ";
			}
			cout << "|" << endl;
		}

		cout << "--------------------------------------------------------------" << endl;
	}
}

static void printVariablesTable(SyntaxAnalysis& syntax) {
	auto varList = syntax.getVariablesList();
	if (!varList.empty()) {
		std::cout << "Variables Table:" << std::endl;
		std::cout << "-------------------------------------------" << std::endl;
		std::cout << "| Position | Name       | Type            |" << std::endl;
		std::cout << "-------------------------------------------" << std::endl;

		for (Variable* var : varList) {
			std::cout << "| " << var->getPosition() << "        ";
			std::cout << "| " << var->getName() << "      ";

			std::string typeStr;
			switch (var->getType()) {
			case Variable::MEM_VAR:
				typeStr = "MEM_VAR";
				break;
			case Variable::REG_VAR:
				typeStr = "REG_VAR";
				break;
			default:
				typeStr = "NO_TYPE";
				break;
			}

			std::cout << "| " << typeStr << "      ";
			std::cout << "|" << std::endl;
		}

		std::cout << "-------------------------------------------" << std::endl;
	}
}

static void printFunctionsTable(SyntaxAnalysis& syntax) {
	auto functions = syntax.getFunctionsList();
	if (!functions.empty()) {
		std::cout << "Functions Table:" << std::endl;
		std::cout << "----------------------------" << std::endl;
		std::cout << "| Position | Name          |" << std::endl;
		std::cout << "----------------------------" << std::endl;

		for (Function* func : functions) {
			std::cout << "| " << func->getPosition() << "        ";
			std::cout << "| " << func->getName() << "    ";
			std::cout << "|" << std::endl;
		}

		std::cout << "----------------------------" << std::endl;
	}
}

static void printLabelsTable(SyntaxAnalysis& syntax) {
	auto labels = syntax.getLabelsList();
	if (!labels.empty()) {
		std::cout << "Labels Table:" << std::endl;
		std::cout << "----------------------------" << std::endl;
		std::cout << "| Position | Name          |" << std::endl;
		std::cout << "----------------------------" << std::endl;

		for (Label* label : labels) {
			std::cout << "| " << label->getPosition() << "        ";
			std::cout << "| " << label->getName() << "    ";
			std::cout << "|" << std::endl;
		}

		std::cout << "----------------------------" << std::endl;
	}
}

static void printInstr(const Instructions& instructions) {
	if (!instructions.empty()) {
		cout << "Instruction Details:" << endl;
		cout << "--------------------------------------------------------------------------------" << endl;
		cout << "| Position | Succ     | Use           | Def           | In            | Out           |" << endl;
		cout << "--------------------------------------------------------------------------------" << endl;

		for (Instruction* instr : instructions) {
			cout << "| " << instr->getPosition() << "        | ";

			for (Instruction* succ : instr->getSucc()) {
				cout << succ->getPosition() << " ";
			}
			cout << "       | ";

			for (Variable* var : instr->getUse()) {
				cout << var->getName() << " ";
			}
			cout << "         | ";

			for (Variable* var : instr->getDef()) {
				cout << var->getName() << " ";
			}
			cout << "         | ";

			for (Variable* var : instr->getIn()) {
				cout << var->getName() << " ";
			}
			cout << "          | ";

			for (Variable* var : instr->getOut()) {
				cout << var->getName() << " ";
			}
			cout << "          |" << endl;
		}

		cout << "--------------------------------------------------------------------------------" << endl;
	}
}

static std::string getVariableLocation(Variable* var, SyntaxAnalysis sy) {
	Variables regs = sy.getRegistersList();
	if (var->getType() == Variable::REG_VAR) {
		for (Variable* reg : regs) {
			if (reg->getName() == var->getName()) {
				switch (reg->getAssignment()) {
				case 1: return "$t0";
				case 2: return "$t1";
				case 3: return "$t2";
				case 4: return "$t3";
				default: return "Nije nesto dobro";
				}
			}
		}
	}
	else {
		return var->getName();
	}
}

static void generateOutput(SyntaxAnalysis& syntax, const string& outputFileName) {
	ofstream outFile(outputFileName);

	//.globl sekcija
	auto functions = syntax.getFunctionsList();
	for (Function* func : functions) {
		outFile << ".globl " << func->getName() << endl;
	}
	outFile << endl;

	//.data sekcija
	outFile << ".data" << endl;
	for (Variable* var : syntax.getVariablesList()) {
		if (var->getType() == Variable::MEM_VAR) {
			outFile << var->getName() << ":\t.word " << var->getMemValue() << endl;
		}
	}
	outFile << endl;

	//.text sekcija
	auto labels = syntax.getLabelsList();
	unordered_map<int, string> labelPositions;
	for (Label* label : labels) {
		labelPositions[label->getPosition()] = label->getName();
	}

	outFile << ".text" << endl;

	for (Function* func : functions) {
		outFile << func->getName() << ":" << endl;
	}
	for (Instruction* instr : syntax.getInstructionsList()) {
		std::string instrStr = instr->getTypeAsString();
		Variables& dst = instr->getDst();
		Variables& src = instr->getSrc();
		Variables& num = instr->getNum();

		if (labelPositions.find(instr->getPosition()) != labelPositions.end()) {
			outFile << labelPositions[instr->getPosition()] << ":" << endl;
		}

		if (instrStr == "add" || instrStr == "sub") {
			outFile << instrStr << " "
				<< getVariableLocation(dst.front(), syntax) << ", "
				<< getVariableLocation(src.front(), syntax) << ", "
				<< getVariableLocation(src.back(), syntax) << std::endl;
		}
		else if (instrStr == "addi") {
			outFile << instrStr << " "
				<< getVariableLocation(dst.front(), syntax) << ", "
				<< getVariableLocation(src.front(), syntax) << ", "
				<< num.front()->getName() << std::endl;
		}
		else if (instrStr == "lw" || instrStr == "sw") {
			outFile << instrStr << " "
				<< getVariableLocation(dst.front(), syntax) << ", "
				<< num.front()->getName() << "(" << getVariableLocation(src.front(), syntax) << ")" << std::endl;
		}
		else if (instrStr == "li") {
			outFile << instrStr << " "
				<< getVariableLocation(dst.front(), syntax) << ", "
				<< num.front()->getName() << std::endl;
		}
		else if (instrStr == "la") {
			outFile << instrStr << " "
				<< getVariableLocation(dst.front(), syntax) << ", "
				<< getVariableLocation(src.front(), syntax) << std::endl;
		}
		else if (instrStr == "b" || instrStr == "bltz") {
			outFile << instrStr << " "
				<< getVariableLocation(src.front(), syntax) << ", "
				<< labelPositions[instr->getTarget()->getPosition()] << std::endl;
		}
		else if (instrStr == "nop") {
			outFile << instrStr << std::endl;
		}
	}
	outFile.close();
}



int main()
{
	try
	{
		std::string fileName = ".\\..\\examples\\simple.mavn";
		bool retVal = false;

		LexicalAnalysis lex;

		if (!lex.readInputFile(fileName))
			throw runtime_error("\nException! Failed to open input file!\n");

		
		lex.initialize();

		retVal = lex.Do();

		if (retVal)
		{
			cout << "Lexical analysis finished successfully!" << endl;
			lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}
		
		SyntaxAnalysis syntax(lex);

		bool syntaxSuccess = syntax.Do();

		if (syntaxSuccess)
		{
			cout << "Syntax analysis finished successfully!" << endl;
			printInstructionTable(syntax);
			printVariablesTable(syntax);
			printLabelsTable(syntax);
			printFunctionsTable(syntax);
		}
		else
		{
			cout << "Syntax analysis failed!" << endl;
		}
		Instructions& instructions = syntax.getInstructionsList();

		livenessAnalysis(instructions);
		printInstr(instructions);

		Variables regs = syntax.getRegistersList(); 

		ResourceAllocation resourceAllocation(regs, __REG_NUMBER__);

		// Create interference graph
		resourceAllocation.createInterferenceGraph(instructions);

		// Simplify the graph
		bool simplified = resourceAllocation.simplify();
		if (!simplified) {
			std::cout << "Simplification failed" << std::endl;
		}
		else {
			std::cout << "Simplification successful" << std::endl;
		}

		// Perform graph coloring
		std::unordered_map<int, Variable*> indexToVar;
		int index = 0;
		for (Variables::iterator it = regs.begin(); it != regs.end(); ++it, ++index) {
			indexToVar[index] = *it;
		}

		bool colored = resourceAllocation.colorGraph(indexToVar);
		if (!colored) {
			std::cout << "Graph coloring failed" << std::endl;
		}
		else {
			std::cout << "Graph coloring successful" << std::endl;
			resourceAllocation.printColors();
			resourceAllocation.printVariableAssignments(indexToVar);
		}
		generateOutput(syntax, "output.s");
		
	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
		return 1;
	}

	return 0;
}
