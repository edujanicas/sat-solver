#include "clause.h"
#include "sat.h"

V CLAUSEdeepCopy(V clause) {
    //TODO
}

bool CLAUSEnew(V literals, bool learnt, C output) {
    if (!learnt) {
        if (CLAUSEclean(literals)) {
            return true;
        }
    }

    CLAUSEremoveDuplicates(literals);

    if (VECTORtotal(literals) == 0)
        return false;

    if (VECTORtotal(literals) == 1) {
        enqueue((Var) VECTORget(literals, 0), NULL);
    } else {
        output = malloc(sizeof(struct clause));
        output->learnt = learnt;
        output->activity = 0;

        if (learnt) {
            //TODO: see SAT.pseudo
        }

        //adding to watchers of literals[0, 1]
        addToWatchersOf(output, ((Var) VECTORget(literals, 0)));
        addToWatchersOf(output, ((Var) VECTORget(literals, 1)));
    }
}

bool CLAUSEclean(V literals) {
    int i = 0;
    for (i = 0; i < VECTORtotal(literals); i++) {
        Var currentVar = VECTORget(literals, i);
        if (value(currentVar) == true) {
            return true;
        } else if (value(currentVar) == false) {
            VECTORdelete(literals, i);
        }
    }

    return false;
}

void CLAUSEremoveDuplicates(V literals) {
    int i = 0, j = 0;
    for (i = 0; i < VECTORtotal(literals); i++) {
        Var currentVar = VECTORget(literals, i);
        for (j = i + 1; j < VECTORtotal(literals); j++) {
            Var comparisonVar = VECTORget(literals, j);
            if (currentVar->id == comparisonVar->id && currentVar->sign == comparisonVar->sign) {
                VECTORdelete(literals, j);
                j--;
            }
        }
    }
}

bool CLAUSEpropagate(C clause, Var p) {
    //invariant: at the moment of the call the clause is not in watchers[p->id] anymore

    Var negP = neg(p);

    //make sure -p is in literals[1]
    if (((Var) VECTORget(clause->literals, 0))->id == p->id &&
        ((Var) VECTORget(clause->literals, 0))->sign == negP->sign) {
        VECTORset(clause->literals, 0, VECTORget(clause->literals, 1));
        VECTORset(clause->literals, 1, neg(p));
    }

    free(negP);

    if (value((Var) VECTORget(clause->literals, 0)) == true) {
        //already satisfied, reinsert into watchers of p
        addToWatchersOf(clause, p);
        return true;
    }

    //else look for another variable to watch
    for (int i = 2; i < VECTORtotal(clause->literals); i++) {
        Var currentVar = VECTORget(clause->literals, i);
        if (value(currentVar) != false) {
            VECTORset(clause->literals, i, currentVar);
            VECTORset(clause->literals, i, neg(p));
            addToWatchersOf(clause, currentVar);
        }
    }

    //if no watchable variable is found, clause is unit
    addToWatchersOf(clause, p);
    return enqueue(VECTORget(clause->literals, 0), clause);
}

void addToWatchersOf(C clause, Var p) {
    VECTORadd(watchers[p->id], clause);
}