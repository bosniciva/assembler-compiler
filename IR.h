#ifndef __IR__
#define __IR__

#include "Types.h"
#include <list>
#include <string>
#include <sstream>
#include <istream>
#include <ostream>
#include <iostream>

using namespace std;

/**
 * This class represents one variable from program code.
 */
class Variable
{
public:
	enum VariableType
	{
		MEM_VAR,
		REG_VAR,
		NO_TYPE
	};

	Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign), memValue("") {}
	Variable(std::string name, int pos) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign), memValue("") {}

	VariableType getType() const{
		return m_type;
	}


	std::string getName() {
		return m_name;
	}

	int getPosition() const{
		return m_position;
	}

	void setPosition(int p) {
		m_position = p;
	}

	void setType(VariableType vt) {
		this->m_type = vt;
	}
	
	Regs& getAssignment() { return m_assignment; }
	void setAssignment(Regs reg) { m_assignment = reg; }

	std::string getMemValue() {
		return memValue;
	}
	void setMemValue(std::string mv) {
		memValue = mv;
	}

private:
	VariableType m_type;
	std::string m_name;
	int m_position;
	Regs m_assignment;
	std::string memValue;
};
/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;


/**
 * This class represents one instruction in program code.
 */
class Instruction
{
public:
	Instruction() : m_position(0), m_type(I_NO_TYPE) {}
	Instruction(int pos, InstructionType type, Variables& dst, Variables& src) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src), target(nullptr), m_num() {}


	void setType(InstructionType it) {
		this->m_type = it;
	}

	InstructionType getType() const {
		return m_type;
	}

	int getPosition() const {
		return m_position;
	}

	std::string getTypeAsString() const {
		switch (m_type) {
		case I_ADD:
			return "add";
		case I_ADDI:
			return "addi";
		case I_SUB:
			return "sub";
		case I_LA:
			return "la";
		case I_LI:
			return "li";
		case I_LW:
			return "lw";
		case I_SW:
			return "sw";
		case I_B:
			return "b";
		case I_BLTZ:
			return "bltz";
		case I_NOP:
			return "nop";
		default:
			return "unknown";
		}
	}

	Variables& getDst() {
		return m_dst;
	}


	Variables& getSrc() {
		return m_src;
	}
	void setSrc(Variables src) {
		m_src = src;
	}

	Variables& getIn() {
		return m_in;
	}
	void setIn(Variables& in) {
		m_in = in;
	}

	Variables& getOut() {
		return m_out;
	}
	void setOut(Variables& out) {
		m_out = out;
	}

	Variables& getUse() {
		return m_use;
	}
	void setUse(Variables& use) {
		m_use = use;
	}

	Variables& getNum() {
		return m_num;
	}
	void setNum(Variables& num) {
		m_num = num;
	}

	Variables& getDef() {
		return m_def;
	}
	void setDef(Variables& def) {
		m_def = def;
	}

	std::list<Instruction*>& getSucc() {
		return m_succ;
	}

	std::list<Instruction*>& getPred() {
		return m_pred;
	}

	void setTarget(Instruction* tar) {
		target = tar;
	}

	Instruction* getTarget() const {
		return target;
	}

private:
	int m_position;
	Variables m_num;
	InstructionType m_type;
	
	Variables m_dst;
	Variables m_src;

	Variables m_use;
	Variables m_def;
	Variables m_in;
	Variables m_out;
	std::list<Instruction*> m_succ;
	std::list<Instruction*> m_pred;

	Instruction* target; // za b i bltz
};
/**
 * This type represents list of instructions from program code.
 */
typedef std::list<Instruction*> Instructions;


class Label {
public:
	Label(std::string name, int pos) : m_name(name), m_position(pos) {}

	int getPosition() const{
		return m_position;
	}

	std::string getName() {
		return m_name;
	}
private:
	int m_position;
	std::string m_name;
};
/**
 * This type represents list of labels from program code.
 */
typedef std::list<Label*> Labels;

class Function {
public:
	Function(std::string name, int pos) : m_name(name), m_position(pos) {}

	int getPosition() const{
		return m_position;
	}

	std::string getName() {
		return m_name;
	}
private:
	int m_position;
	std::string m_name;
};
/**
 * This type represents list of functions from program code.
 */
typedef std::list<Function*> Functions;


#endif
