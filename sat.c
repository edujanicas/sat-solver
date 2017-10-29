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

        C currentClause = VECTORget(formula, i);
        V currentLiterals = currentClause->literals;

        for (int j = 0; j < VECTORtotal(currentLiterals); j++) {
            Var currentVar = VECTORget(currentLiterals, j);
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

unsigned int selectVar() {
    // TODO: Add heuristic to choose variable
    for (unsigned int id = 1; id <= numberOfLiterals; id++) {
        if (assignments[id] == unassigned) {
            return id;
        }
    }
    return 0;
}

bool decide(unsigned int id) {
    trail_lim_size++;
    trail_lim = realloc(trail_lim, sizeof(int) * trail_lim_size);
    trail_lim[trail_lim_size - 1] = VECTORtotal(trail);

    //TODO why false?
    // Dummy variable to store the id
    Var decidingVar = VARinit(id, false);

    printDebugVar("Assigned true to ", decidingVar);
    return enqueue(decidingVar, NULL);
}

void change_decision(unsigned int assigned) {
    assignments[assigned] = false;
}

//returns false on conflict, true on succesfull enqueueing
bool enqueue(Var p, C from) {
    printDebugVar("Enqueueing for propagation: " ,p);
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
*/
C propagate() {
    printDebugInt("Propagation starting, queue size: ", propagationQ->size);
    while (propagationQ->size > 0) {
        Var propagatingVar = QUEUEdequeue(propagationQ);
        int numberOfWatchers = watchers[propagatingVar->id]->total;

        printDebugVar("Propagating of ", propagatingVar);

        C propagationWatchers[numberOfWatchers];

        //moving the watching clauses to a temporary vector
        for (unsigned int i = 0; i < numberOfWatchers; i++) {
            propagationWatchers[i] = VECTORget(watchers[propagatingVar->id], i);
        }
        VECTORfree(watchers[propagatingVar->id]);
        watchers[propagatingVar->id] = VECTORinit();

        for (unsigned int i = 0; i < numberOfWatchers; i++) {
            if (!CLAUSEpropagate(propagationWatchers[i], propagatingVar)) {
                //clause was unit and conflicting, aborting

                //adding back the watchers
                for (unsigned int j = i + 1; j < numberOfWatchers; j++) {
                    VECTORadd(watchers[propagatingVar->id], propagationWatchers[j]);
                }

                //flushing propagationQ
                QUEUEclear(propagationQ);


                //and returning the conflicting clause
                return propagationWatchers[i];
            }
        }
    }

    return NULL;
}

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

    // Currently last assigned variable
    unsigned int lastAssigned = 0;
    bool conf = false;

    while (true) {
        printFormula(formula);

        C conflictingClause = propagate();

        if (conflictingClause != NULL) {
            printDebugInt("Conflict at level", trail_lim_size);
            //conflict
            if (trail_lim_size == 0) {
                //top level conflict
                return false;
            }
            // TODO: Analize the conflict and learn something
            // TODO: Non chronological backtracking
            if (assignments[lastAssigned] == true) {
                printDebugInt("Assigned false to ", lastAssigned);
                change_decision(lastAssigned);
            } else {
                cancel();
            }
        } else {
            //no conflict
            if (allVarsAssigned()) {
                return true;
            }

            // TODO: Simplify DB if on top level
            // TODO: Reduce the number of learnt clauses to avoid size issues
            lastAssigned = selectVar();
            if (lastAssigned > 0) {
                decide(lastAssigned);
            } else {
                printDebug("No more variables to assign");
                return false;
            }
        }
    }
}

//TODO: KILL ME {
//OLD SOLVE()
/*
    // TODO: new_formula = propagate(formula)
    V new_formula = formula;

    if(propagate() != NULL)


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
         else if (conf && trail_lim_size == 0) {
            // Trail size = 0 means we are at the root level
            // Top level conflict!
            return false;

        }
    else if (conf) {
        // Conflict in the middle of the tree
        // TODO: Analize the conflict and learn something
        // TODO: Non chronological backtracking
        if (assignments[assigned] == true) change_decision(assigned);
        else cancel();

    } else {
        // No conflicts

        // TODO: Simplify DB if on top level
        // TODO: Reduce the number of learnt clauses to avoid size issues
        assigned = decide();
    }
    //TODO: }
*/


void initializeAssigments() {
    assignments = (bool *) malloc(sizeof(bool) * numberOfLiterals + sizeof(bool));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        assignments[i] = unassigned;
    }
}

void initializeTrail() {
    trail = VECTORinit();
    trail_lim_size = 0;
}

void initializeWatchers() {
    watchers = (V *) malloc(sizeof(V) * numberOfLiterals + sizeof(V));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        watchers[i] = VECTORinit();
    }
}

V initialize(char *path) {

    FILE *inputFile = PARSERinit(path);
    PARSEheader(inputFile);

    initializeAssigments();
    initializeTrail();

    propagationQ = QUEUEinit();

    initializeWatchers();

    return PARSEformula(inputFile);
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }

    V cnf = initialize(argv[1]);

    printDebug("-----RUNNING DPLL-----");
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
