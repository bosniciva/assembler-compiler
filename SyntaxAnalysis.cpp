/* Iva Bosnic RA222/2022
* Datum: 3.6.2024.
*/
#include <iostream>
#include <iomanip>
#include <regex>

#include "SyntaxAnalysis.h"
#include "Token.h"
#include "IR.h"

using namespace std;

Variables regVariables;
Variables memVariables;
Labels labelsList;
Functions functionsList;
Variables variablesList;
Instructions instrucionsList;

SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex)
	: lexicalAnalysis(lex), errorFound(false), tokenIterator(lexicalAnalysis.getTokenList().begin())
{
}


bool SyntaxAnalysis::Do()
{
	if (tokenIterator == lexicalAnalysis.getTokenList().end()) {
		return true;
	}

	currentToken = getNextToken();
	position = 0;

	Q();

	return !errorFound;
}


void SyntaxAnalysis::printSyntaxError(Token token)
{
	cout << "Syntax error! Token: " << token.getValue() << " unexpected" << endl;
}


void SyntaxAnalysis::printTokenInfo(Token token)
{
	cout << setw(20) << left << token.tokenTypeToString(token.getType());
	cout << setw(25) << right << token.getValue() << endl;
}


void SyntaxAnalysis::eat(TokenType t)
{
	if (errorFound == false)
	{
		if (currentToken.getType() == t)
		{
			cout << currentToken.getValue() << endl;
			currentToken = getNextToken();
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
}


Token SyntaxAnalysis::getNextToken()
{
	if (tokenIterator == lexicalAnalysis.getTokenList().end()) {
		Do();
		Token *token = new Token(T_END_OF_FILE, "Kraj");
		return *token;
	}
	position++;
	return *tokenIterator++;
}


void SyntaxAnalysis::storeVariable() {
	string name = currentToken.getValue();
	int pos = position;
	Variable* newVariable = new Variable(name, pos);
	regex memorijska("^m[0-9]*$");
	regex registarska("^r[0-9]*$");
	for (Variable* var : variablesList) {
		if (name == var->getName()) {
			std::cerr << "Error! Variable already exists" + name << endl;
		}
	}
	if (currentToken.getType() == T_R_ID) {
		if (regex_match(name, registarska)) {
			newVariable->setType(Variable::REG_VAR);
		}
		else {
			std::cerr << "Error! Registarska varijabla mora poceti malim slovom r i bilo kojim brojem!" << endl;
		}
	}
	else if (currentToken.getType() == T_M_ID) {
		if (regex_match(name, memorijska)) {
			newVariable->setType(Variable::MEM_VAR);
			eat(T_M_ID);
			newVariable->setMemValue(currentToken.getValue());
		}
		else {
			std::cerr << "Error! Memorijska varijabla mora poceti malim slovom m i bilo kojim brojem!"<<endl;
		}
	}
	variablesList.push_back(newVariable);
}

void SyntaxAnalysis::storeLabel() {
	string name = currentToken.getValue();
	int pos = position;
	Label* newLabel = new Label(name, pos);
	for (Label* lab : labelsList) {
		if (name == lab->getName()) {
			std::cerr << "Error! Label already exists: " + name << endl;
		}
	}
	labelsList.push_back(newLabel);
}

void SyntaxAnalysis::storeFunction() {
	string name = currentToken.getValue();
	int pos = position;
	Function* newFunction = new Function(name, pos);
	regex funkcija("^[a-zA-Z][a-zA-Z0-9]*$");

	if (!regex_match(name, funkcija)) {
		std::cerr << "Error! Funkcija mora poceti slovom!" << endl;
	}
	functionsList.push_back(newFunction);

	if (functionsList.size() > 1) {
		std::cerr <<"Error! Function already exists" <<endl;
	}
}



Labels SyntaxAnalysis::getLabelsList() const {
	return labelsList;
}

Variables SyntaxAnalysis::getVariablesList() const {
	return variablesList;
}

Instructions SyntaxAnalysis::getInstructionsList() const {
	return instrucionsList;
}

Functions SyntaxAnalysis::getFunctionsList() const {
	return functionsList;
}
Variables SyntaxAnalysis::getRegistersList() const {
	for (Variable* var : variablesList) {
		if (var->getType() == Variable::REG_VAR) {
			regVariables.push_back(var);
		}
	}
	return regVariables;
}

Variables SyntaxAnalysis::getMemList() const {
	for (Variable* var : variablesList) {
		if (var->getType() == Variable::MEM_VAR) {
			regVariables.push_back(var);
		}
	}
	return memVariables;
}

void SyntaxAnalysis::Q()
{
	S();
	eat(T_SEMI_COL);
	L();
}

void SyntaxAnalysis::S()
{

	if (errorFound == false) {
		switch (currentToken.getType()) {
		case(T_MEM):
			eat(T_MEM);
			storeVariable();
			eat(T_NUM);
			break;
		case(T_REG):
			eat(T_REG);
			storeVariable();
			eat(T_R_ID);
			break;
		case(T_FUNC):
			eat(T_FUNC);
			storeFunction();
			eat(T_ID);
			break;
		case(T_ID):
			storeLabel();
			eat(T_ID);
			eat(T_COL);
			E();
			break;
		default:
			E();
			break;
		}
	}
}

void SyntaxAnalysis::L()
{
	if (errorFound == false) {
		switch (currentToken.getType()) {
		case(T_END_OF_FILE):
			eat(T_END_OF_FILE);
			break;
		default:
			Q();
			break;
		}
	}
}


void SyntaxAnalysis::E()
{
	int pos = position;
	Variables dst;
	Variables src;
	Variables num;
	static Instruction* lastInstruction = nullptr; 
		
	if (errorFound == false) {
		Instruction* newInstruction = nullptr;
		switch (currentToken.getType()) {
		case T_ADD: {
			eat(T_ADD);
			Variable* rd = new Variable(currentToken.getValue(), pos);
			rd->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			Variable* rs = new Variable(currentToken.getValue(), pos);
			rs->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			Variable* rt = new Variable(currentToken.getValue(), pos);
			rt->setType(Variable::REG_VAR);
			eat(T_R_ID);

			dst.push_back(rd);
			src.push_back(rs);
			src.push_back(rt);


			newInstruction = new Instruction(pos, I_ADD, dst, src);
			newInstruction->setDef(dst);
			newInstruction->setUse(src);
			instrucionsList.push_back(newInstruction);
			break;
		}
		case T_ADDI: {
			eat(T_ADDI);
			Variable* rt = new Variable(currentToken.getValue(), pos);
			rt->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			Variable* rs = new Variable(currentToken.getValue(), pos);
			rs->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			Variable* rd = new Variable(currentToken.getValue(), pos);
			eat(T_NUM);

			dst.push_back(rt);
			src.push_back(rs);
			num.push_back(rd);

			newInstruction = new Instruction(pos, I_ADDI, dst, src);
			newInstruction->setNum(num);
			newInstruction->setDef(dst);
			newInstruction->setUse(src);
			instrucionsList.push_back(newInstruction);
			break;
		}
		case(T_SUB): {
			eat(T_SUB);
			Variable* rd = new Variable(currentToken.getValue(), pos);
			rd->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			Variable* rs = new Variable(currentToken.getValue(), pos);
			rs->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			Variable* rt = new Variable(currentToken.getValue(), pos);
			rt->setType(Variable::REG_VAR);
			eat(T_R_ID);

			dst.push_back(rd);
			src.push_back(rs);
			src.push_back(rt);

			newInstruction = new Instruction(pos, I_SUB, dst, src);
			newInstruction->setDef(dst);
			newInstruction->setUse(src);
			instrucionsList.push_back(newInstruction);
			break;
		}
		case(T_LA): {
			eat(T_LA);
			Variable* rd = new Variable(currentToken.getValue(), pos);
			rd->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			Variable* ms = new Variable(currentToken.getValue(), pos);
			eat(T_M_ID);

			dst.push_back(rd);
			src.push_back(ms);

			newInstruction = new Instruction(pos, I_LA, dst, src);
			newInstruction->setDef(dst);
			instrucionsList.push_back(newInstruction);
			break;
		}
		case(T_LW): {
			eat(T_LW);
			Variable* rd = new Variable(currentToken.getValue(), pos);
			rd->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			Variable* rs = new Variable(currentToken.getValue(), pos);
			eat(T_NUM);
			eat(T_L_PARENT);
			Variable* rt = new Variable(currentToken.getValue(), pos);
			rt->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_R_PARENT);

			dst.push_back(rd);
			src.push_back(rt);
			num.push_back(rs);

			newInstruction = new Instruction(pos, I_LW, dst, src);
			newInstruction->setNum(num);
			newInstruction->setDef(dst);
			newInstruction->setUse(src);
			instrucionsList.push_back(newInstruction);

			break;
		}
		case(T_LI): {
			eat(T_LI);
			Variable* rd = new Variable(currentToken.getValue(), pos);
			rd->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			Variable* rs = new Variable(currentToken.getValue(), pos);
			eat(T_NUM);

			dst.push_back(rd);
			num.push_back(rs);

			newInstruction = new Instruction(pos, I_LI, dst, src);
			newInstruction->setNum(num);
			newInstruction->setDef(dst);
			newInstruction->setUse(src);
			instrucionsList.push_back(newInstruction);

			break;
		}
		case(T_SW): {
			eat(T_SW);
			Variable* rd = new Variable(currentToken.getValue(), pos);
			rd->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			Variable* rs = new Variable(currentToken.getValue(), pos);
			eat(T_NUM);
			eat(T_L_PARENT);
			Variable* rt = new Variable(currentToken.getValue(), pos);
			rt->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_R_PARENT);

			dst.push_back(rd);
			src.push_back(rt);
			num.push_back(rs);

			newInstruction = new Instruction(pos, I_LW, dst, src);
			newInstruction->setNum(num);
			newInstruction->setDef(dst);
			newInstruction->setUse(src);
			instrucionsList.push_back(newInstruction);
			break;
		}
		case(T_B): {
			eat(T_B);
			string labelName = currentToken.getValue();
			eat(T_ID);

			newInstruction = new Instruction(pos, I_B, dst, src);
			instrucionsList.push_back(newInstruction);

			Instruction* targetInstruction = nullptr;
			for (Label* label : labelsList) {
				if (label->getName() == labelName) {
					int targetPos = label->getPosition();
					for (Instruction* instr : instrucionsList) {
						if (instr->getPosition() == targetPos) {
							targetInstruction = instr;
							break;
						}
					}
					break;
				}
			}

			if (targetInstruction) {
				newInstruction->setTarget(targetInstruction);
				targetInstruction->getPred().push_back(newInstruction);
			}

			break;
		}
		case(T_BLTZ): {
			eat(T_BLTZ);
			Variable* rs = new Variable(currentToken.getValue(), pos);
			rs->setType(Variable::REG_VAR);
			eat(T_R_ID);
			eat(T_COMMA);
			string labelName = currentToken.getValue();
			eat(T_ID);

			src.push_back(rs);
			newInstruction = new Instruction(pos, I_BLTZ, dst, src);
			newInstruction->setDef(dst);
			newInstruction->setUse(src);
			instrucionsList.push_back(newInstruction);

			Instruction* targetInstruction = nullptr;
			for (Label* label : labelsList) {
				if (label->getName() == labelName) {
					int targetPos = label->getPosition();
					for (Instruction* instr : instrucionsList) {
						if (instr->getPosition() == targetPos) {
							targetInstruction = instr;
							break;
						}
					}
					break;
				}
			}

			if (targetInstruction) {
				newInstruction->setTarget(targetInstruction);
				targetInstruction->getPred().push_back(newInstruction);
			}
			break;
		}
		case(T_NOP): {
			eat(T_NOP);

			newInstruction = new Instruction(pos, I_NOP, dst, src);
			instrucionsList.push_back(newInstruction);

			break;
		}
		default:
			printSyntaxError(currentToken);
			errorFound = true;
			break;
		}
		if (newInstruction) {
			if (lastInstruction) {
				lastInstruction->getSucc().push_back(newInstruction);
				newInstruction->getPred().push_back(lastInstruction);
			}

			lastInstruction = newInstruction;
		}
	}
}



