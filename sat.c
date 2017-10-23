#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "queue.h"
#include "vector.h"
#include "var.h"

int conflict(V formula) {
    for (int i = 0; i < VECTORtotal(formula); i++) {
        for (int j = 0; j < VECTORtotal(VECTORget(formula, i)); j++) {
            Var current = ((Var)VECTORget(VECTORget(formula, i), j));
            if(current->value != 2) {
                for (int k = i; k < VECTORtotal(formula); k++) {
                    for (int l = j; l < VECTORtotal(VECTORget(formula, k)); l++){ 
                        Var cmp = ((Var)VECTORget(VECTORget(formula, k), l));
                        if(current->value == cmp->value) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

int allVarsAssigned(V formula) {
    return false;
}
int topLevelConflict(V formula) {
    return 0;
}
Var decide(V formula) {
    return NULL;
}
void change_decision(Var assigned) {

}

// TODO: Move me somewhere appropriate
// TODO: Remove variable when appropriate
void check_vars_in_clause(V clause, V unit_vars) {
    for (int i = 0; i < VECTORtotal(clause); ++i) {
        Var current_var = VECTORget(clause, i);

        for (int j = 0; j < VECTORtotal(unit_vars); ++j) {
            Var unit_var = VECTORget(clause, j);

            if (current_var->id == unit_var->id) {
                current_var->value = unit_var->sign;
            }
        }
    }
}

// TODO: Copy clauses to result
V propagate(V formula) {
    // Create simplified formula
    V result = VECTORinit();

    // Store variables appearing in unit clauses
    V unit_vars = VECTORinit();

    for (int i = 0; i < VECTORtotal(formula); ++i) {
        V clause = VECTORget(formula, i);

        if (VECTORtotal(clause) == 1) {
            printf("%d: size 1\n", i);

            Var unit_var = VECTORget(clause, 0);
            VECTORadd(unit_vars, unit_var);
        }
        else {
            printf("%d: size %d\n", i, VECTORtotal(clause));

            VECTORadd(result, clause);
        }
    }

    // Propagate units
    for (int i = 0; i < VECTORtotal(result); ++i) {
        V clause = VECTORget(result, i);

        for (int j = 0; j < VECTORtotal(clause); ++j) {
            check_vars_in_clause(clause, unit_vars);
        }
    }

    return result;
}

int dpll(V formula) {
    V new_formula = propagate(formula);
    if(!conflict(new_formula) && allVarsAssigned(new_formula)) {
        return true;
    } else if (conflict(new_formula)){
        return false;
    }
    Var assigned = decide(new_formula);
    if(dpll(new_formula)) {
        return true;
    } else {
        change_decision(assigned);
        return dpll(new_formula);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }
    V cnf = parse(argv[1]);

    printf("%d ",((Var)VECTORget(VECTORget(cnf, 0), 0))->id);
    printf("%d\n",((Var)VECTORget(VECTORget(cnf, 0), 0))->value);
    printf("%d ",((Var)VECTORget(VECTORget(cnf, 0), 1))->id);
    printf("%d\n",((Var)VECTORget(VECTORget(cnf, 0), 1))->value);
    printf("%d ",((Var)VECTORget(VECTORget(cnf, 1), 0))->id);
    printf("%d\n",((Var)VECTORget(VECTORget(cnf, 1), 0))->value);
    printf("%d ",((Var)VECTORget(VECTORget(cnf, 1), 1))->id);
    printf("%d\n",((Var)VECTORget(VECTORget(cnf, 1), 1))->value);

    // while(true) {
    //     propagate(); // propagate unit clauses
    //     if(!conflict()) {
    //         if(allVarsAssigned()) {
    //             printf("SAT\n");
    //             exit(EXIT_SUCCESS);
    //         } else {
    //             decide(); // pick a new variable and assign it
    //         }
    //     } else {
    //         analize(); // analize conflict and add a conflict cause
    //         if(topLevelConflict()) {
    //             printf("UNSAT\n");
    //             exit(EXIT_SUCCESS);
    //         } else {
    //             backtrack() // undo assignments until conflict clause is unit
    //         }
    //     }
    // }
    printf("Conflict? %d\n", conflict(cnf));
    printf("UNSAT\n");

    exit(EXIT_SUCCESS);
}
