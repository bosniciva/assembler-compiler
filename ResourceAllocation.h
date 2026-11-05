#ifndef RESOURCE_ALLOCATION_H
#define RESOURCE_ALLOCATION_H

#include <vector>
#include <unordered_map>
#include <stack>
#include <iostream>

#include "IR.h"
#include "Types.h"


class ResourceAllocation {
public:
    ResourceAllocation(Variables variables, int numRegisters);
    const std::vector<std::vector<bool>>& getOriginalMatrix() const;
    void addInterference(Variable* var1, Variable* var2);
    void printGraph();
    std::vector<std::vector<bool>>& getGraph();

    void createInterferenceGraph(Instructions instructions);
    bool simplify();
    void printStack() const;
    std::stack<int>& getStack();

    bool colorGraph(std::unordered_map<int, Variable*>& indexToVar);
    void printColors();
    void printVariableAssignments(const std::unordered_map<int, Variable*>& indexToVar);

private:
    int countEdges(int node);
    void removeEdges(int node);
    bool allRemoved(const std::vector<bool>& removed);
    int findAvailableColor(const std::vector<bool>& availableColors);

    int numVariables;
    int numRegisters;
    std::vector<std::vector<bool>> matrix;
    std::vector<std::vector<bool>> originalMatrix;
    std::unordered_map<std::string, int> variableIndex;
    std::stack<int> simpStack;
    std::vector<Regs> colors;
};

#endif // RESOURCE_ALLOCATION_H