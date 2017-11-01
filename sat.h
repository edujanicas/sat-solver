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

#define VARDECAY 1
#define VARINC 1e-100 

V cnf;

// --- Propagation
V *watchers;
Q propagationQ;

// --- Ordering

double *activity;
double var_inc;
double var_decay;

// --- Assignments
unsigned int numberOfLiterals;
unsigned int numberOfClauses;
bool *assignments;
V trail;
int *trail_lim;
int trail_lim_size;

// --- Solver Functions
bool value(Var p);

bool conflict(V formula);

bool allVarsAssigned();

unsigned int selectVar();

bool decide(unsigned int id);

void change_decision(unsigned int assigned);

void simplifyClause(V clause, V unitVars);

C propagate();

void printAssignments();

int solve(V formula);

bool enqueue(Var p, C from);

void initializeAssigments();

void varBumpActivity(Var v);

void varDecayActivity();

void varRescaleActivity();

#endif //SAT_SAT_H
