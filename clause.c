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
        VECTORadd(watchers[((Var) VECTORget(literals, 0))->id], output);
        VECTORadd(watchers[((Var) VECTORget(literals, 1))->id], output);
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
    //TODO
}