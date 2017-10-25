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

    //removing duplicates
    CLAUSEremoveDuplicates(literals);

    if (VECTORtotal(literals) == 0)
        return false;

    if (VECTORtotal(literals) == 1) {
        //TODO: sat::enqueue()
        //enqueue(((Var) VECTORget(literals, 0))->id);
    } else {
        //TODO: clauses of lenght 1 will not be added!
        output = malloc(sizeof(struct clause));
        output->learnt = learnt;
        output->activity = 0;

        if (learnt) {
            //TODO: see SAT.pseudo
        }

        //TODO: add clauses to watchers of lits[0, 1]
    }
}

bool CLAUSEclean(V literals) {
    int i = 0;
    for (i = 0; i < VECTORtotal(literals); i++) {
        Var currentVar = VECTORget(literals, i);
        if (assignments[currentVar->id] == true) {
            return true;
        } else if (assignments[currentVar->id] == false) {
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