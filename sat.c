#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "var.h"
#include "parser.h"
#include "debugPrinter.h"


int conflict(V formula) {
    // Very dumb solver. It may have different variables with the same id and different values ??? WHAT ???
    for (int i = 0; i < VECTORtotal(formula); i++) {
        V currentClause = VECTORget(formula, i);
        for (int j = 0; j < VECTORtotal(currentClause); j++) {
            Var currentVar = VECTORget(currentClause, j);
            if (currentVar->value != unassigned) {
                for (int k = i; k < VECTORtotal(formula); k++) {
                    V currentCheckingClause = VECTORget(formula, k);
                    for (int l = j; l < VECTORtotal(currentCheckingClause); l++) {
                        Var currentCheckingVar = VECTORget(currentCheckingClause, l);
                        if (currentCheckingVar->value != unassigned &&
                            currentVar->id == currentCheckingVar->id &&
                            currentVar->value != currentCheckingVar->value) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    // Check if there are no all false or clauses
    for (int i = 0; i < VECTORtotal(formula); i++) {
        int value = false;
        V currentClause = VECTORget(formula, i);
        for (int j = 0; j < VECTORtotal(currentClause); j++) {
            Var currentVar = VECTORget(currentClause, j);
            if (currentVar->value == unassigned || VARgetValue(currentVar) == true) {
                value = true;
            }
        }
        if (value == false) return true;
    }
    return false;
}

int allVarsAssigned(V formula) {
    for (int i = 0; i < VECTORtotal(formula); i++) {
        V currentClause = VECTORget(formula, i);
        for (int j = 0; j < VECTORtotal(currentClause); j++) {
            if (((Var) VECTORget(currentClause, j))->value == unassigned) return false;
        }
    }
    return true;
}

Var decide(V formula) {
    for (int i = 0; i < VECTORtotal(formula); i++) {
        V currentClause = VECTORget(formula, i);
        for (int j = 0; j < VECTORtotal(currentClause); j++) {
            Var var = VECTORget(currentClause, j);
            if (var->value == unassigned) {
                var->value = true;
                return var;
            }
        }
    }
    return NULL;
}

void change_decision(Var assigned) {
    if (assigned->value == true)
        assigned->value = false;
    else if (assigned->value == false)
        assigned->value = true;
}

// TODO: Move me somewhere appropriate
// TODO: Remove variable when appropriate
void simplifyClause(V clause, V unitVars) {
    for (int i = 0; i < VECTORtotal(clause); ++i) {
        Var currentVar = VECTORget(clause, i);

        for (int j = 0; j < VECTORtotal(unitVars); ++j) {
            Var unitVar = VECTORget(unitVars, j);

            if (currentVar->id == unitVar->id) {
                currentVar->value = unitVar->sign;
            }
        }
    }
}

V propagate(V formula) {
    printDebug("Propagation starting...");

    // Create simplified formula
    V result = VECTORinit();

    // Store variables appearing in unit clauses
    V unit_vars = VECTORinit();

    for (int i = 0; i < VECTORtotal(formula); ++i) {
        V clause = VECTORget(formula, i);

        if (VECTORtotal(clause) == 1) {
            printDebugInt("Size 1 in clause number: ", i);

            Var unit_var = VECTORget(clause, 0);
            VECTORadd(unit_vars, unit_var);
        } else {
            printDebugIntInt("Clause n : size", i, VECTORtotal(clause));

            VECTORadd(result, VECTORcopy(clause));
        }
    }

    // Propagate units
    for (int i = 0; i < VECTORtotal(result); ++i) {
        V clause = VECTORget(result, i);

        for (int j = 0; j < VECTORtotal(clause); ++j) {
            simplifyClause(clause, unit_vars);
        }
    }
    printDebug("Propagation ending...");

    return result;
}

int solve(V formula) {
    V new_formula = propagate(formula);
    if (!conflict(new_formula) && allVarsAssigned(new_formula)) {
        return true;
    } else if (conflict(new_formula)) {
        return false;
    }
    Var assigned = decide(new_formula);
    if (solve(new_formula)) {
        return true;
    } else {
        change_decision(assigned);
        return solve(new_formula);
    }
}

unsigned int getNumberOfVars(V formula) {

    unsigned int result = 1;

    for(int i = 0; i < VECTORtotal(formula); i++) {
        for(int j = 0; j < VECTORtotal(VECTORget(formula, i)); j++ ) {

            if(((Var)VECTORget(VECTORget(formula, i), j)) -> id > result) {
                result = ((Var) VECTORget(VECTORget(formula, i), j))->id;
            }
        }
    }

    return result;
}

void printAssignments(V formula) {

    bool printed = false;

    for(unsigned int current = 1; current <= getNumberOfVars(formula); current++) {

        printed = false;

        for(int i = 0; i < VECTORtotal(formula); i++) {
            for(int j = 0; j < VECTORtotal(VECTORget(formula, i)); j++ ) {

                Var var = ((Var) VECTORget(VECTORget(formula, i), j));
                if(var->id == current && var->value != unassigned) {

                    if(var->value == true) printf("%d ", var->id);
                    else printf("-%d ", var->id);

                    printed = true;
                    break;
                }
            }
            if(printed == true) break;
        }
    }
    printf("\n");
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }

    V cnf = parse(argv[1]);

    printDebug("Runing dpll...");
    int result = solve(cnf);
    if (result == true) {
        printf("SAT\n");
        printAssignments(cnf);
    } else {
        printf("UNSAT\n");
    }

    VECTORfree(cnf);

    exit(EXIT_SUCCESS);
}
