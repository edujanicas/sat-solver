#include "parser.h"
#include "debugPrinter.h"

void printOutput(V output) {
    int i, j;
    printDebugInt("Number of parsed clauses: ", VECTORtotal(output));
    for (i = 0; i < VECTORtotal(output); i++) {
        printDebugInt("Clause", i);
        V clause = VECTORget(output, i);
        for (j = 0; j < VECTORtotal(clause); j++) {
            Var var = VECTORget(clause, j);
            if (!var->sign)
                printDebugInt("-", var->id);
            else
                printDebugInt("", var->id);
        }
        printDebug("\n");
    }
}

void badFormatted(char *message, char *details) {
    printf("ERROR: the file is badly formatted. %s%s<\n", message, details);
    exit(EXIT_FAILURE);
}

int isSeparator(int character) {
    if (isspace(character))
        return 1;
    if (character == '\n')
        return 1;
    if (character == '\t')
        return 1;
    if (character == EOF)
        return 1;
    return 0;
}

void skipLine(FILE *inputFile) {
    int readChar;
    do {
        readChar = fgetc(inputFile);
    } while (readChar != '\n' && readChar != EOF);
    printDebug("Skipped a comment\n");
}

void checkStartSequence(FILE *inputFile) {
    char *fileStart = "p cnf ";
    int readChar;

    do {
        readChar = fgetc(inputFile);
        if (readChar == (int) *fileStart) {
            fileStart++;
        } else {
            if (readChar == 'c') {
                readChar = fgetc(inputFile);
                if (readChar == ' ') {
                    skipLine(inputFile);
                    fileStart = "p cnf ";
                    continue;
                } else {
                    badFormatted("The file must start with", "p cnf ");
                }
            } else {
                badFormatted("The file must start with", "p cnf ");
            }
        }
    } while (*fileStart);

}

void readUntilSpace(FILE *inputFile, int *lastReadChar, char *readNumberChars) {
    int i = 0;
    int readChar;
    do {
        readChar = fgetc(inputFile);
        if (readChar == 'c')
            skipLine(inputFile);
        else
            *(readNumberChars + i) = (char) readChar;
        i++;
    } while (i < MAX_NUMBER_LENGTH && !isSeparator(readChar));

    if (i >= MAX_NUMBER_LENGTH)
        badFormatted("This parser only accepts numbers of length in characters up to: ", MAX_NUMBER_LENGTH_C);

    *lastReadChar = readChar;
}

int readNumberUntilSpace(FILE *inputFile, int *lastReadChar) {
    int readNumber;
    char *readNumberChars = malloc(MAX_NUMBER_LENGTH * sizeof(char));

    readUntilSpace(inputFile, lastReadChar, readNumberChars);

    readNumber = strtol(readNumberChars, NULL, 0);

    /* Will fail at this point if
     *    strtol failed
     *    strtol did not attempt (this would not trigger a strtol fail)
     * */
    if (readNumber == 0 && (errno == EINVAL || errno == ERANGE || !isdigit(readNumberChars[0])))
        badFormatted("Expected a number, read: ", readNumberChars);

    return readNumber;
}

void makeOutput(FILE *inputFile, V output, int numberOfClauses, int numberOfLits) {
    int i;
    V tempClause;
    Var tempVar;
    int lastReadChar = 0;
    int readInt;

    for (i = 0; i < numberOfClauses; i++) {
        tempClause = VECTORinit();
        do {
            readInt = readNumberUntilSpace(inputFile, &lastReadChar);
            printDebugInt("read  ", readInt);
            if (abs(readInt) < 0 || abs(readInt) > numberOfLits) {
                badFormatted("not a valid literal", "");
            }

            if (readInt != 0) {
                tempVar = VARinit((unsigned int) abs(readInt), readInt > 0);
                VECTORadd(tempClause, tempVar);
                printDebug("added\n");
            }

            if (!isSeparator(lastReadChar)) {
                badFormatted("", "");
            } else if (lastReadChar == EOF) {
                if (i < numberOfClauses - 1) {
                    badFormatted("Could not find enough clauses", "");
                } else {
                    break;
                }
            }

        } while (readInt != 0);
        printDebug("adding clause\n");
        VECTORadd(output, tempClause);

    }

    if (lastReadChar != EOF)
        printDebugInt("WARNING: Heading indicated %d clauses, they have been parsed correctly. EOF not reached.\n",
                      numberOfClauses);
}

int readHeaderParameter(FILE *inputFile) {
    int lastReadChar;
    int parameter = readNumberUntilSpace(inputFile, &lastReadChar);
    if (!isSeparator(lastReadChar))
        badFormatted("", "");
    else if (lastReadChar == EOF)
        badFormatted("Could not find number of clauses parameter", "");
    return parameter;
}

V parse(char *path) {
    V output = VECTORinit();

    FILE *inputFile;

    int numberOfLits, numberOfClauses;

    inputFile = fopen(path, "r");

    checkStartSequence(inputFile);

    numberOfLits = readHeaderParameter(inputFile);

    numberOfClauses = readHeaderParameter(inputFile);

    makeOutput(inputFile, output, numberOfClauses, numberOfLits);

    printOutput(output);

    return output;
}

//uncomment for testing purposes
//
//int main(int argc, char *argv[]) {
//    if (argc != 2) {
//        printf("Please run with exactly one argument\n");
//        exit(EXIT_FAILURE);
//    }
//
//    printf("parsing file%s\n", argv[1]);
//    parse(argv[1]);
//}
