#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "var.h"
#include "parser.h"


int conflict(V formula) {
    // VERY DUMB SOLVER. IT MAY HAVE DIFERENT VARIABLES WITH THE SAME ID AND DIFERENT VALUES
    for (int i = 0; i < VECTORtotal(formula); i++) {
        for (int j = 0; j < VECTORtotal(VECTORget(formula, i)); j++) {
            Var current = ((Var)VECTORget(VECTORget(formula, i), j));
            if(current->value != unassigned) {
                for (int k = i; k < VECTORtotal(formula); k++) {
                    for (int l = j; l < VECTORtotal(VECTORget(formula, k)); l++){
                        Var cmp = ((Var)VECTORget(VECTORget(formula, k), l));
                        if(cmp->value != unassigned &&
                            current->id == cmp->id &&
                            current->value != cmp->value) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    // CHECK IF THERE ARE NO ALL FALSE OR clauses
    for (int i = 0; i < VECTORtotal(formula); i++) {
        int value = false;
        for (int j = 0; j < VECTORtotal(VECTORget(formula, i)); j++) {
            Var current = ((Var)VECTORget(VECTORget(formula, i), j));
            if(current -> value == unassigned || VARgetValue(current)==true) {
              value = true;
            }
        }
        if(value==false) return true;
    }
    return false;
}

int allVarsAssigned(V formula) {
    for (int i = 0; i < VECTORtotal(formula); i++) {
        for (int j = 0; j < VECTORtotal(VECTORget(formula, i)); j++) {
            if (((Var)VECTORget(VECTORget(formula, i), j))->value == unassigned) return false;
        }
    }
    return true;
}

Var decide(V formula) {
    for (int i = 0; i < VECTORtotal(formula); i++) {
        for (int j = 0; j < VECTORtotal(VECTORget(formula, i)); j++) {
            Var var = (Var)VECTORget(VECTORget(formula, i), j);
            if (var->value == unassigned){
                var->value = true;
                return var;
            }
        }
    }
    return NULL;
}
void change_decision(Var assigned) {
    if(assigned->value == true)
        assigned->value = false;
    else
        assigned->value = true;
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

V propagate(V formula) {
    printf("Propagation starting...\n");

    // Create simplified formula
    V result = VECTORinit();

    // Store variables appearing in unit clauses
    V unit_vars = VECTORinit();

    for (int i = 0; i < VECTORtotal(formula); ++i) {
        V clause = VECTORget(formula, i);

        if (VECTORtotal(clause) == 1) {
            printf("Clause %d: size 1\n", i);

            Var unit_var = VECTORget(clause, 0);
            VECTORadd(unit_vars, unit_var);
        }
        else {
            printf("Clause %d: size %d\n", i, VECTORtotal(clause));

            VECTORadd(result, VECTORcopy(clause));
        }
    }

    // Propagate units
    for (int i = 0; i < VECTORtotal(result); ++i) {
        V clause = VECTORget(result, i);

        for (int j = 0; j < VECTORtotal(clause); ++j) {
            check_vars_in_clause(clause, unit_vars);
        }
    }
    printf("Propagation ending...\n");

    return result;
}

int solve(V formula) {
    V new_formula = propagate(formula);
    if(!conflict(new_formula) && allVarsAssigned(new_formula)) {
        return true;
    } else if (conflict(new_formula)){
        return false;
    }
    Var assigned = decide(new_formula);
    if(solve(new_formula)) {
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
                result = ((Var)VECTORget(VECTORget(formula, i), j)) -> id; 
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
                
                Var var = ((Var)VECTORget(VECTORget(formula, i), j)); 
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

    printf("Runing dpll...\n");
    int result = solve(cnf);

    if(result == true) {
        printf("SAT\n");
        printAssignments(cnf);
    } else {
      printf("UNSAT\n");
    }

    exit(EXIT_SUCCESS);
}
