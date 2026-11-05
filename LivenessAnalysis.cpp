// Iva Bosnic 2.6.2024.

#include "LivenessAnalysis.h"
#include "IR.h"

using namespace std;

void livenessAnalysis(Instructions instructions)
{
	//iterira kroz instrukcije u obrnutom smeru
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto it = instructions.rbegin(); it != instructions.rend(); ++it) {
            Variables oldIn = (*it)->getIn();
            Variables oldOut = (*it)->getOut();

            Variables newOut;
            for (auto succ : (*it)->getSucc()) {
                Variables succIn = succ->getIn();
                newOut.insert(newOut.end(), succIn.begin(), succIn.end());
            }
            newOut.sort();
            newOut.unique();
            (*it)->setOut(newOut);

            Variables newIn = (*it)->getUse();
            for (auto var : newOut) {
                if (std::find((*it)->getDef().begin(), (*it)->getDef().end(), var) == (*it)->getDef().end()) {
                    newIn.push_back(var);
                }
            }
            newIn.sort();
            newIn.unique();
            (*it)->setIn(newIn);

            if (oldIn != newIn || oldOut != newOut) {
                changed = true;
            }
        }
    }
}
