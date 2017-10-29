#include "sat.h"

bool value(Var p) {
    if (p->id <= numberOfLiterals && assignments[p->id] != unassigned) {
        if (p->sign == true) {
            return assignments[p->id];
        } else if (p->sign == false)
            return !assignments[p->id];
    }
    return unassigned;
}

bool conflict(V formula) {
    // Check if there is at least one clause whose literals are all false
    for (int i = 0; i < VECTORtotal(formula); i++) {
        int flag = false;

        V currentClause = VECTORget(formula, i);

        for (int j = 0; j < VECTORtotal(currentClause); j++) {
            Var currentVar = VECTORget(currentClause, j);
            if (assignments[currentVar->id] == unassigned ||
                value(currentVar) == true) {
                flag = true;

                break;
            }
        }
        // Each literal of currentClause is false
        if (flag == false) return true;
    }

    // There are no clauses whose literals are all false
    return false;
}

bool allVarsAssigned() {
    for (unsigned int i = 1; i <= numberOfLiterals; i++) {
        if (assignments[i] == unassigned) return false;
    }
    return true;
}

unsigned int decide() {
    // TODO: Add heuristic to choose variable
    for (unsigned int id = 1; id <= numberOfLiterals; id++) {
        if (assignments[id] == unassigned) {
            
            trail_lim_size++;
            trail_lim = realloc(trail_lim, sizeof(int) * trail_lim_size);
            trail_lim[trail_lim_size - 1] = VECTORtotal(trail);

            // Dummy variable to store the id
            enqueue(VARinit(id, false), NULL);

            return id;
        }
    }
    return 0;
}

void change_decision(unsigned int assigned) {
    assignments[assigned] = false;
}

//returns false on conflict, true on succesfull enqueueing
bool enqueue(Var p, C from) {

    if (value(p) == false) {
        //conflict
        return false;
    } else if (value(p) == true) {
        //already assigned
        return true;
    } else {
        assignments[p->id] = neg(p)->sign;
        //TODO: decision levels, reasoning list, trail
        //level = sat.c :: decisionLevel();
        //level = sat.c :: reason[p->id] = from;
        VECTORadd(trail, p);

        QUEUEinsert(propagationQ, p);
        return true;
    }
}

/*
// TODO: KILL ME
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

void cancel() {

    // c is the difference between the total number of assignments and and first assignment of the 
    // current level, i.e., the number of assignments to cancel
    int c = VECTORtotal(trail) - trail_lim[trail_lim_size--];
    for (; c != 0; c--) {
        // Get the last element from the trail vector (last assignment)
        Var p = VECTORget(trail, VECTORtotal(trail) - 1);
        // p is null
        assignments[p->id] = unassigned;
        VECTORpop(trail);
    }

}

void printAssignments() {

    for (unsigned int i = 1; i <= numberOfLiterals; i++) {
        if (assignments[i]) printf("%d", i);
        else printf("-%d", i);
        if (i != numberOfLiterals) printf(" ");
    }
    printf("\n");
}

int solve(V formula) {

    // Current assigned variable
    unsigned int assigned = 0;
    bool conf = false;

    while(true) {

        // TODO: new_formula = propagate(formula)
        V new_formula = formula;

        if (conf && trail_lim_size == 0) {
            // FIXME: Workarround spot before having propagate
            // Correct place for this check will be next to the other ifs

            // Trail size = 0 means we are at the root level
            // Top level conflict!
            return false;
        }

        // Check for conflicts
        conf = conflict(new_formula);

        if (!conf && allVarsAssigned()) {
            
            return true;

        } 
        /* else if (conf && trail_lim_size == 0) {
            // Trail size = 0 means we are at the root level
            // Top level conflict!
            return false;

        } */ 
        else if (conf) {
            // Conflict in the middle of the tree
            // TODO: Analize the conflict and learn something
            // TODO: Non chronological backtracking
            if(assignments[assigned] == true) change_decision(assigned);
            else cancel();

        } else {
            // No conflicts

            // TODO: Simplify DB if on top level
            // TODO: Reduce the number of learnt clauses to avoid size issues
            assigned = decide();
        }    
    }
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }

    V cnf = parse(argv[1]);

    // Initialize the assignments vector. Each literal is indexed at its id (0 index unused)
    assignments = (bool*) malloc(sizeof(bool) * numberOfLiterals + sizeof(bool));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        assignments[i] = unassigned;
    }

    // Trail vector will keep track of each assignment, to backtrack
    trail = VECTORinit();    
    trail_lim_size = 0;

    watchers = (V*) malloc(sizeof(V) * numberOfLiterals);
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        watchers[i] = VECTORinit();
    }

    propagationQ = QUEUEinit();

    printDebug("Runing dpll...");
    int result = solve(cnf);

    if (result == true) {
        printf("SAT\n");
        printAssignments();
    } else {
        printf("UNSAT\n");
    }

    VECTORfree(cnf);

    exit(EXIT_SUCCESS);
}
