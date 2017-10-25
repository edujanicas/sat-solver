#include "sat.h"

bool value(Var p) {
    if (p->id <= numberOfLiterals)
        if (p->sign == true) {
            return assignments[p->id];
        } else if ((p->sign == false))
            return !assignments[p->id];
}

int conflict(V formula) {
    // Check if there are no all false or clauses
    for (int i = 0; i < VECTORtotal(formula); i++) {
        int flag = false;
        
        V currentClause = VECTORget(formula, i);
        
        for (int j = 0; j < VECTORtotal(currentClause); j++) {
            Var currentVar = VECTORget(currentClause, j);
            if (assignments[currentVar->id] == unassigned || 
                value(currentVar) == true) {
                flag = true;
            }
        }
        
        if (flag == false) return true;
    }
    return false;
}

unsigned int getNumberOfIds(V formula) {

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

int allVarsAssigned(V formula) {
    for (unsigned int i = 1; i <= numberOfLiterals; i++) {
        if (assignments[i] == unassigned) return false;
    }
    return true;
}

unsigned int decide(V formula) {
    
    for (unsigned int id = 1; id <= numberOfLiterals; id++) {
        if (assignments[id] == unassigned) {
            assignments[id] = true;
            return id;
        } 
    }
    return 0;
}

void change_decision(unsigned int assigned) {
    assignments[assigned] = false;
}

/*
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
    V unitVars = VECTORinit();

    // Splitting in unit clauses and non unit clauses
    for (int i = 0; i < VECTORtotal(formula); ++i) {
        V clause = VECTORget(formula, i);

        if (VECTORtotal(clause) == 1) {
            printDebugInt("Size 1 in clause number: ", i);

            Var unit_var = VECTORget(clause, 0);
            VECTORadd(unitVars, unit_var);
        } else {
            printDebugIntInt("Clause n : size", i, VECTORtotal(clause));

            VECTORadd(result, VECTORshallowCopy(clause)); //TODO change to deep
        }
    }

    printOutput(result);

    // Propagate units
    for (int i = 0; i < VECTORtotal(result); ++i) {
        V clause = VECTORget(result, i);

        for (int j = 0; j < VECTORtotal(clause); ++j) {
            simplifyClause(clause, unitVars);
        }
    }
    printDebug("Propagation ending...");

    printOutput(result);

    return result;
}
*/

void printAssignments(V formula) {

    for(unsigned int i = 1; i <= numberOfLiterals; i++)           
        if(assignments[i]) printf("%d ", i);
        else printf("-%d ", i);
    printf("\n");
}

int solve(V formula) {
    V new_formula = formula;
    if (!conflict(new_formula) && allVarsAssigned(new_formula)) {
        return true;
    } else if (conflict(new_formula)) {
        return false;
    }
    unsigned int assigned = decide(new_formula);
    if (solve(new_formula)) {
        return true;
    } else {
        change_decision(assigned);
        return solve(new_formula);
    }
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }
    
    V cnf = parse(argv[1]);

    numberOfLiterals = getNumberOfIds(cnf);
    assignments = (bool*)malloc(sizeof(bool)*numberOfLiterals + sizeof(bool));

    for(unsigned int i = 0; i <= numberOfLiterals; i++) {
        assignments[i] = unassigned;
    }

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
