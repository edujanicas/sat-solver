#ifndef SAT_DEBUGPRINTER_H
#define SAT_DEBUGPRINTER_H

#include "vector.h"
#include "var.h"
#include "clause.h"
#include "sat.h"

#define IS_DEBUG 1

void printDebugInt(char *message, int value);

void printFormula(V formula);

void printDebugIntInt(char *message, int value1, int value2);

void printDebugChar(char *message, int value);

void printDebugVar(char *message, Var var);

void printDebug(char *message);

#endif
