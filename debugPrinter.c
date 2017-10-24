#include <stdio.h>
#include "debugPrinter.h"

void printDebugInt(char* message, int value) {
    if(IS_DEBUG)
        printf("%s%d\n",message, value);
}
void printDebugChar(char* message, int value) {
    if(IS_DEBUG)
        printf("%s%c\n",message, value);
}
void printDebug(char* message) {
    if(IS_DEBUG)
        printf("%s\n", message);

}