/* Iva Bosnic RA222/2022
* Datum: 3.6.2024.
*/
#pragma once

#include "LexicalAnalysis.h"
#include "Token.h"
#include "IR.h"
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class SyntaxAnalysis
{
public:

	/**
	* Constructor
	*/
	SyntaxAnalysis(LexicalAnalysis& lex);

	/**
	* Method which performs lexical analysis
	*/
	bool Do();


	Instructions getInstructionsList() const ;
	Variables getVariablesList() const;
	Labels getLabelsList() const;
	Functions getFunctionsList() const;
	Variables getRegistersList() const;
	Variables getMemList() const;

	string getVariableLocation(Variable* var);
	void generateMIPSAssembly(const string& outputFileName);

private:

	int position;
	/**
	* Prints the error message, and token that caused the syntax error
	*/
	void printSyntaxError(Token token);

	/**
	* Prints the token info
	*/
	void printTokenInfo(Token token);

	/**
	* Eats the current token if its type is "t"
	* otherwise reports syntax error
	*
	* param[in] - t - the expected token type
	*/
	void eat(TokenType t);

	/**
	* Returns the next token from the token list
	*/
	Token getNextToken();

	/**
	 * Nonterminal Q
	 */
	void Q();

	void storeVariable();

	void storeFunction();

	
	void storeLabel();

	/**
	 * Nonterminal S
	 */
	void S();

	/**
	 * Nonterminal L
	 */
	void L();

	/**
	 * Nonterminal E
	 */
	void E();

	/**
	* Reference to lexical analysis module
	*/
	LexicalAnalysis& lexicalAnalysis;

	/**
	* Syntax error indicator
	*/
	bool errorFound;

	/**
	* Iterator to the token list which represents the output of the lexical analysis
	*/
	TokenList::iterator tokenIterator;

	/**
	* Current token that is being analyzed
	*/
	Token currentToken;
};
