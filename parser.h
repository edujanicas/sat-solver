#include <ctype.h>
#include <errno.h>

#include "vector.h"
#include "var.h"

#define MAX_NUMBER_LENGTH 10
#define MAX_NUMBER_LENGTH_C "10"

void printDebugOutput(V output);
void badFormatted(char *message, char *details);
int isSeparator(int character);
void checkStartSequence(FILE *inputFile);
int readNumberUntilSpace(FILE *inputFile, int *lastReadChar);
V parse(char *path);

