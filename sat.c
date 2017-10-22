#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "queue.h"
#include "vector.h"
#include "var.h"

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

    printf("UNSAT\n");

    exit(EXIT_SUCCESS);
}
