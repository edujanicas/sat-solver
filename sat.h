#ifndef SAT_SAT_H
#define SAT_SAT_H

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "var.h"
#include "parser.h"
#include "debugPrinter.h"

unsigned int numberOfLiterals;
bool *assignments;

bool value(Var p);

int conflict(V formula);

int allVarsAssigned();

unsigned int decide();

void change_decision(unsigned int assigned);

void simplifyClause(V clause, V unitVars);

V propagate();

void printAssignments();

int solve(V formula);

#endif //SAT_SAT_H
