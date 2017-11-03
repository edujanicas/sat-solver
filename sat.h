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

// --- BECAUSE C
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

// --- Propagation
V *watchers;
Q propagationQ;

// --- Assignments
unsigned int numberOfLiterals;
unsigned int numberOfClauses;
bool *assignments;
V trail;
int *trail_lim;
int trail_lim_size;

// --- Backtrack and learning
C *reason;
int rootLevel;
V learnts;
Var lastDecisionUndone;

// --- Decision level
int *level;

// --- Solver Functions
bool value(Var p);

bool conflict(V formula);

bool allVarsAssigned();

unsigned int selectVar();

bool decide(unsigned int id);

void change_decision(unsigned int assigned);

void simplifyClause(V clause, V unitVars);

void undoOne();

C propagate();

void printAssignments();

int solve(V formula);

bool enqueue(Var p, C from);

int currentDecisionLevel();

void initializeAssigments();

#endif //SAT_SAT_H
