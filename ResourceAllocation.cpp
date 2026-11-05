// Iva Bosnic 6.6.2024

#include "ResourceAllocation.h"

using namespace std;

ResourceAllocation::ResourceAllocation(Variables variables, int registers)
    : numRegisters(registers) {
    numVariables = variables.size();
    matrix.resize(numVariables, vector<bool>(numVariables, false));
    originalMatrix.resize(numVariables, vector<bool>(numVariables, false));

    int index = 0;
    for (Variable* var : variables) {
        variableIndex[var->getName()] = index++;
    }
    colors.resize(numVariables, no_assign);
}

const std::vector<std::vector<bool>>& ResourceAllocation::getOriginalMatrix() const {
    for (const auto& row : matrix) {
        for (bool cell : row) {
            cout << (cell ? "1 " : "0 ");
        }
        cout << endl;
    }
    return originalMatrix;
}

void ResourceAllocation::addInterference(Variable* var1, Variable* var2) {
    int i = variableIndex[var1->getName()];
    int j = variableIndex[var2->getName()];
    if (i != j) {
        matrix[i][j] = true;
        matrix[j][i] = true;
        originalMatrix[i][j] = true;
        originalMatrix[j][i] = true;
    }
}

void ResourceAllocation::printGraph() {
    for (const auto& row : matrix) {
        for (bool cell : row) {
            cout << (cell ? "1 " : "0 ");
        }
        cout << endl;
    }
}

vector<vector<bool>>& ResourceAllocation::getGraph() {
    return matrix;
}

void ResourceAllocation::createInterferenceGraph(Instructions instructions) {
    for (Instruction* instr : instructions) {
        for (Variable* defVar : instr->getDef()) {
            for (Variable* outVar : instr->getOut()) {
                if (defVar != outVar) {
                    addInterference(defVar, outVar);
                }
            }
        }

        for (Variable* useVar : instr->getUse()) {
            for (Variable* outVar : instr->getOut()) {
                if (useVar != outVar) {
                    addInterference(useVar, outVar);
                }
            }
        }
    }
    printGraph(); 
}

bool ResourceAllocation::simplify() {
    int n = matrix.size();
    vector<bool> removed(n, false);

    bool progress = true;
    while (progress) {
        progress = false;
        for (int i = 0; i < n; ++i) {
            if (!removed[i] && countEdges(i) < numRegisters) {
                removed[i] = true;
                cout << "Removing node " << i << endl;
                simpStack.push(i);
                removeEdges(i);
                progress = true;
            }
        }
    }

    if (!allRemoved(removed)) {
        while (!allRemoved(removed)) {
            int maxDegree = -1;
            int spillNode = -1;
            for (int i = 0; i < n; ++i) {
                if (!removed[i] && countEdges(i) > maxDegree) {
                    maxDegree = countEdges(i);
                    spillNode = i;
                }
            }
            if (spillNode != -1) {
                cout << "Spilling node " << spillNode << " due to high degree" << endl;
                removed[spillNode] = true;
                simpStack.push(spillNode);
                removeEdges(spillNode);
            }
        }
    }
    return allRemoved(removed);
}

void ResourceAllocation::printStack() const {
    std::stack<int> tempStack = simpStack;
    std::cout << "Simplification stack: ";
    while (!tempStack.empty()) {
        std::cout << tempStack.top() << " ";
        tempStack.pop();
    }
    std::cout << std::endl;
}

std::stack<int>& ResourceAllocation::getStack() {
    return simpStack;
}

int ResourceAllocation::countEdges(int node) {
    int count = 0;
    for (bool edge : matrix[node]) {
        if (edge) count++;
    }
    return count;
}

void ResourceAllocation::removeEdges(int node) {
    for (int i = 0; i < matrix.size(); ++i) {
        matrix[node][i] = false;
        matrix[i][node] = false;
    }
}

bool ResourceAllocation::allRemoved(const vector<bool>& removed) {
    for (bool r : removed) {
        if (!r) return false;
    }
    return true;
}

bool ResourceAllocation::colorGraph(std::unordered_map<int, Variable*>& indexToVar) {
    while (!simpStack.empty()) {
        int node = simpStack.top();
        simpStack.pop();

        std::vector<bool> availableColors(numRegisters, true);

        for (int i = 0; i < originalMatrix.size(); ++i) {
            if ((originalMatrix[node][i]) && colors[i] != no_assign) {
                availableColors[colors[i] -1] = false;
            }
        }

        int color = findAvailableColor(availableColors);
        if (color == -1) {
            return false;
        }

        colors[node] = static_cast<Regs>(color + 1);

        if (indexToVar.find(node) != indexToVar.end()) {
            indexToVar[node]->setAssignment(colors[node]);
        }
    }

    std::vector<bool> usedColors(numRegisters, false);
    for (Regs color : colors) {
        if (color != no_assign) {
            usedColors[color - 1] = true;
        }
    }

    for (int i = 1; i < numRegisters; ++i) {
        if (!usedColors[i]) {
            cout << "Not all registers were used." << endl;
        }
    }

    return true;
}

void ResourceAllocation::printColors() {
    for (Regs color : colors) {
        cout << color << " ";
    }
    cout << endl;
}

int ResourceAllocation::findAvailableColor(const vector<bool>& availableColors) {
    for (int i = 0; i < availableColors.size(); ++i) {
        if (availableColors[i]) {
            return i;
        }
    }
    return -1;
}

void ResourceAllocation::printVariableAssignments(const std::unordered_map<int, Variable*>& indexToVar) {
    for (const auto& entry : indexToVar) {
        int index = entry.first;
        Variable* var = entry.second;
        cout << "Variable " << var->getName() << " is assigned to register " << colors[index] << endl;
    }
}
