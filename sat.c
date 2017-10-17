#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "vector.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }

    printf("UNSAT\n");

    // Heap and Queue testing ------------------------------------

    Q queue;
    int i;
    int* item;
    vector v;

    item = malloc(sizeof(int));

    printf("Queue test\n");

    queue = QUEUEinit();
    printf("Is queue empty? %d\n", QUEUEempty(queue));

    printf("Inserting element on queue...\n");
    *item = 5;
    QUEUEinsert(queue, item);
    printf("Is queue empty? %d\n", QUEUEempty(queue));

    printf("Removing element from queue...\n");
    item = QUEUEdequeue(queue);
    printf("Is queue empty? %d\n", QUEUEempty(queue));

    printf("Filling queue...\n");
    for (i = 0; i < 10; i++) {
    	QUEUEinsert(queue, item);
    }
    printf("Queue has size %d\n", QUEUEsize(queue));

    printf("Get all elements from the queue...\n");
    QUEUEclear(queue);

    printf("Get one extra element\n");
    if(QUEUEdequeue(queue) == NULL)
    	printf("Queue was empty\n");
    printf("Queue has size %d\n", QUEUEsize(queue));

    free(queue);
    free(item);

    printf("Done\n");
    printf("--------------------VECTORS---------------\n");

    VECTORinit(&v);

    VECTORadd(&v, "Ola");
    VECTORadd(&v, "a");
    VECTORadd(&v, "todos");

    for (i = 0; i < VECTORtotal(&v); i++)
    	printf("%s ", (char *) VECTORget(&v, i));
    printf("\n");

    VECTORdelete(&v, 3);
    VECTORdelete(&v, 2);
    VECTORdelete(&v, 1);

    VECTORset(&v, 0, "Hello");
    VECTORadd(&v, "World");

    for (i = 0; i < VECTORtotal(&v); i++)
    	printf("%s ", (char *) VECTORget(&v, i));
    printf("\n");

    VECTORfree(&v);

    // End of Heap and Queue testing -----------------------------

    exit(EXIT_SUCCESS);
}
