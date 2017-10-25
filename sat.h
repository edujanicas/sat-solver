#ifndef SAT_SAT_H
#define SAT_SAT_H

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "var.h"
#include "parser.h"
#include "debugPrinter.h"

int numberOfLiterals;
bool *assignments;

bool value(Var p);

int conflict(V formula);

int allVarsAssigned(V formula);

unsigned int decide(V formula);

void change_decision(V formula, unsigned int assigned);

void simplifyClause(V clause, V unitVars);

V propagate(V formula);

unsigned int getNumberOfVars(V formula);

void printAssignments(V formula);

int solve(V formula);

#endif //SAT_SAT_H
