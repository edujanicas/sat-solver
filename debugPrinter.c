#include <stdio.h>
#include "debugPrinter.h"

void printDebugInt(char* message, int value) {
    if(IS_DEBUG)
        printf("%s%d\n",message, value);
}

void printDebugIntInt(char *message, int value1, int value2) {
    if (IS_DEBUG)
        printf("%s%d : %d\n", message, value1, value2);
}
void printDebugChar(char* message, int value) {
    if(IS_DEBUG)
        printf("%s%c\n",message, value);
}
void printDebug(char* message) {
    if(IS_DEBUG)
        printf("%s\n", message);

}