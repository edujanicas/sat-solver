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

void staticVarOrder() {

    // Clear activity
    for(unsigned int i = 1; i <= numberOfLiterals; i++) activity[i] = 0;

    // Simple variable activity heuristic
    for(unsigned int i = 0; i < VECTORtotal(cnf); i++) {
        
        C c = VECTORget(cnf, i);
        double add = 2^(-VECTORtotal(c->literals));
        for(unsigned int j = 0; j < VECTORtotal(c->literals); j++) {
            Var v = VECTORget(c->literals, j);
            activity[v->id] += add;
        }

    }

}

unsigned int selectVar() {
    
    double maxActivity = 0;
    unsigned int maxId = 0;

    for (unsigned int id = 1; id <= numberOfLiterals; id++) {
        if (assignments[id] == unassigned) {
            if(maxActivity < activity[id]) {
                maxActivity = activity[id];
                maxId = id;
            }
        }
    }
    return maxId;
}

bool decide(unsigned int id) {
    trail_lim_size++;
    trail_lim = realloc(trail_lim, sizeof(unsigned int) * trail_lim_size);
    trail_lim[trail_lim_size - 1] = VECTORtotal(trail);

    // Dummy variable to store the id
    Var decidingVar = VARinit(id, true);

    printDebugVar("Assigned true to ", decidingVar);
    return enqueue(decidingVar, NULL);
}

void change_decision(unsigned int assigned) {
    assignments[assigned] = false;
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

void cancel() {

    // c is the difference between the total number of assignments and and first assignment of the 
    // current level, i.e., the number of assignments to cancel
    unsigned int c = VECTORtotal(trail) - trail_lim[--trail_lim_size];
    for (; c != 0; c--) {
        // Get the last element from the trail vector (last assignment)
        Var p = VECTORget(trail, VECTORtotal(trail) - 1);
        // p is null
        assignments[p->id] = unassigned;
        VECTORpop(trail);
    }

}

void varBumpActivity(Var v) {
    
    if(activity[v->id] += var_inc >= 1e100) varRescaleActivity();
    activity[v->id] *= var_inc;

}

void varRescaleActivity() {
    
    for(unsigned int i = 1; i<=numberOfLiterals; i++) activity[i] *= VARINC;
    var_inc *= VARINC;

}

void varDecayActivity() {

    var_inc *= var_decay;

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
    var_decay = VARDECAY;

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
            varDecayActivity();
        } else {
            //no conflict
            if (allVarsAssigned()) {
                return true;
            }

            // TODO: Simplify DB if on top level
            // TODO: Reduce the number of learnt clauses to avoid size issues
            
            staticVarOrder();
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

void initializeTrail() {
    // Trail vector will keep track of each assignment, to backtrack
    trail = VECTORinit();
    trail_lim_size = 0;
}

void initializeWatchers() {
    watchers = (V*) malloc(sizeof(V) * numberOfLiterals + sizeof(V));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        watchers[i] = VECTORinit();
    }
}

void initializeActivities() {
    activity = (double *) malloc(sizeof(double) * numberOfLiterals + sizeof(double));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        activity[i] = 0;
    }
}

V initialize(char *path) {

    FILE *inputFile = PARSERinit(path);
    PARSEheader(inputFile);

    initializeAssigments();
    initializeTrail();

    propagationQ = QUEUEinit();

    initializeWatchers();
    initializeActivities();

    return PARSEformula(inputFile);
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }

    cnf = initialize(argv[1]);
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
