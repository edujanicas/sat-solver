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

int currentDecisionLevel() {
    return trail_lim_size;
}

//TODO KILL ME?
bool conflict(V formula) {
    // Check if there is at least one clause whose literals are all false
    for (unsigned int i = 0; i < VECTORtotal(formula); i++) {
        bool flag = false;

        C currentClause = VECTORget(formula, i);
        V currentLiterals = currentClause->literals;

        for (unsigned int j = 0; j < VECTORtotal(currentLiterals); j++) {
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
    trail_lim = realloc(trail_lim, sizeof(unsigned int) * trail_lim_size);
    trail_lim[trail_lim_size - 1] = VECTORtotal(trail);

    //TODO why false?
    // Dummy variable to store the id
    Var decidingVar = VARinit(id, false);

    printDebugVar("Assigned true to ", decidingVar);
    return enqueue(decidingVar, NULL);
}

void change_decision(unsigned int assigned) {
    if (assignments[assigned] == true) {
        level[assigned] = currentDecisionLevel();
        assignments[assigned] = false;
    } else if (assignments[assigned] == false) {
        level[assigned] = currentDecisionLevel();
        assignments[assigned] = true;
    }
}

//returns false on conflict, true on succesfull enqueueing
bool enqueue(Var p, C from) {
    printDebugVar("Enqueueing for propagation: ", p);
    if (value(p) == false) {
        //conflict
        return false;
    } else if (value(p) == true) {
        //already assigned
        return true;
    } else {
        assignments[p->id] = p->sign;

        level[p->id] = currentDecisionLevel();
        reason[p->id] = from;
        VECTORadd(trail, p);

        QUEUEinsert(propagationQ, p);
        return true;
    }
}

C propagate() {
    printDebugInt("Propagation starting, queue size: ", propagationQ->size);
    while (propagationQ->size > 0) {
        Var propagatingVar = QUEUEdequeue(propagationQ);
        unsigned int numberOfWatchers = watchers[propagatingVar->id]->total;

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

void undoOne() {
    // Get the last element from the trail vector (last assignment)
    Var p = VECTORget(trail, VECTORtotal(trail) - 1);
    int id = p->id;

    assignments[id] = unassigned;
    reason[id] = NULL;
    level[id] = -1;

    //reset p in orders of variables
    //unbind(p);

    VECTORpop(trail);
}

void cancel() {

    // c is the difference between the total number of assignments and and first assignment of the 
    // current level, i.e., the number of assignments to cancel
    unsigned int c = VECTORtotal(trail) - trail_lim[--trail_lim_size];
    for (; c != 0; c--) {
        undoOne();
    }

}

void cancelUntil(int level) {
    while (currentDecisionLevel() > level)
        cancel();
}

void printAssignments() {

    for (unsigned int i = 1; i <= numberOfLiterals; i++) {
        if (assignments[i]) printf("%d", i);
        else printf("-%d", i);
        if (i != numberOfLiterals) printf(" ");
    }
    printf("\n");
}

void learn(V learntClauseVars) {
    C clause = NULL;
    CLAUSEnew(learntClauseVars, true, &clause);
    enqueue(VECTORget(clause->literals, 0), clause);
    if (clause != NULL)
        VECTORadd(learnts, clause);
}

int analyze(C conflictClause, V learntClauseLits) {
    Var p = NULL;
    V reasonForP = VECTORinit();
    int backtrackLevel = 0;
    int count = 0;
    bool seen[numberOfLiterals];

    for (int i = 0; i < numberOfLiterals; i++) {
        seen[i] = false;
    }

    do {
        VECTORfree(reasonForP);
        if (conflictClause != NULL) {
            reasonForP = CLAUSEreasonFor(conflictClause, p);

            //expand reasoning for p
            for (int i = 0; i < VECTORtotal(reasonForP); i++) {
                Var q = VECTORget(reasonForP, i);
                if (!seen[q->id]) {
                    seen[q->id] = true;

                    if (level[q->id] == currentDecisionLevel()) {
                        count++;
                    } else if (level[q->id > 0]) {
                        VECTORadd(learntClauseLits, q);
                        backtrackLevel = max(backtrackLevel, level[q->id]);
                    }
                }
            }

            //select next p
            do {
                p = VECTORget(trail, VECTORtotal(trail) - 1);
                VECTORpop(trail);
                conflictClause = reason[p->id];
                undoOne();
            } while (!seen[p->id]);

            count--;
        }
    } while (count > 0);

    return backtrackLevel;
}

int solve(V formula) {

    // Currently last assigned variable
    unsigned int lastAssigned = 0;

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

            V learntClauseVars = VECTORinit();

            int backtrackTo = analyze(conflictingClause, learntClauseVars);

            cancelUntil(max(backtrackTo, rootLevel));

            learn(learntClauseVars);


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

void initializeAssigments() {
    assignments = (bool *) malloc(sizeof(bool) * numberOfLiterals + sizeof(bool));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        assignments[i] = unassigned;
    }
}

void initializeLevel() {
    level = (int *) malloc(sizeof(int) * numberOfLiterals + sizeof(int));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        level[i] = -1;
    }
}

void initializeReason() {
    reason = (C *) malloc(sizeof(C) * numberOfLiterals + sizeof(C));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        reason[i] = NULL;
    }
}

void initializeTrail() {
    // Trail vector will keep track of each assignment, to backtrack
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
    initializeLevel();
    initializeReason();

    learnts = VECTORinit();

    propagationQ = QUEUEinit();

    initializeWatchers();

    rootLevel = currentDecisionLevel();

    return PARSEformula(inputFile);
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }

    V cnf = initialize(argv[1]);
    if (cnf == NULL) {
        printf("UNSAT\n");
        exit(EXIT_SUCCESS);
    }

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
