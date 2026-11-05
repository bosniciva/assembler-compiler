/* Iva Bosnic RA222/2022
* Datum: 3.6.2024.
*/
#pragma once
#ifndef __LIVENESS_ANALYSIS__
#define __LIVENESS_ANALYSIS__

#include "Types.h"
#include "SyntaxAnalysis.h"
#include "IR.h"

void livenessAnalysis(Instructions instructions);

#endif