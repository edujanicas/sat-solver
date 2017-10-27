#ifndef SAT_SAT_H
#define SAT_SAT_H

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "var.h"
#include "parser.h"
#include "debugPrinter.h"
#include "queue.h"
#include "clause.h"

// --- Propagation
V *watchers;
Q propagationQ;

// --- Assignments
unsigned int numberOfLiterals;
bool *assignments;
V trail;
int *trail_lim;
int trail_lim_size;

// --- Solver Functions
bool value(Var p);

bool conflict(V formula);

bool allVarsAssigned();

unsigned int decide();

void change_decision(unsigned int assigned);

void simplifyClause(V clause, V unitVars);

V propagate();

void printAssignments();

int solve(V formula);

bool enqueue(Var p, C from);

#endif //SAT_SAT_H
