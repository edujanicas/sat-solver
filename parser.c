#include "parser.h"

void printDebugOutput(V output) {
    int i, j;
    printf("Parsed %d clauses\n", VECTORtotal(output));
    for (i = 0; i < VECTORtotal(output); i++) {
        printf("Clause %d:\n\t", i);
        V clause = VECTORget(output, i);
        for (j = 0; j < VECTORtotal(clause); j++) {
            Var var = VECTORget(clause, j);
            if (!var->sign)
                printf("-");
            printf("%d ", var->id);
        }
        printf("\n");
    }
}

void badFormatted(char *message, char *details) {
    printf("ERROR: the file is badly formatted. %s %s\n", message, details);
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

void checkStartSequence(FILE *inputFile) {
    char *FILE_START = "p cnf ";

    int readChar;

    for (; *FILE_START; FILE_START++) {
        readChar = fgetc(inputFile);
        if (readChar != (int) *FILE_START)
            badFormatted("File must start with\n", FILE_START);
    }
}

int readNumberUntilSpace(FILE *inputFile, int *lastReadChar) {
    int readNumber;
    int readChar;
    int i = 0;
    char *readNumberChars = malloc(MAX_NUMBER_LENGTH * sizeof(char));

    i = 0;
    do {
        readChar = fgetc(inputFile);
        *(readNumberChars + i) = (char) readChar;
        i++;
    } while (i < MAX_NUMBER_LENGTH && !isSeparator(readChar));

    if (i >= MAX_NUMBER_LENGTH)
        badFormatted("This parser only accepts numbers of length in characters up to: ", MAX_NUMBER_LENGTH_C);

    *lastReadChar = readChar;

    readNumber = strtol(readNumberChars, NULL, 0);

    /* Will fail at this point if
     *    strtol failed
     *    strtol did not attempt (this would not trigger a strtol fail)
     * */
    if (readNumber == 0 && (errno == EINVAL || errno == ERANGE || !isdigit(readNumberChars[0])))
        badFormatted("", "");

    return readNumber;
}


V parse(char *path) {
    int i;

    V output = VECTORinit();

    V tempClause;

    Var tempVar;

    FILE *inputFile;

    int numberOfLits, numberOfClauses;
    int lastReadChar;

    int readInt;

    inputFile = fopen(path, "r");

    checkStartSequence(inputFile);

    numberOfLits = readNumberUntilSpace(inputFile, &lastReadChar);
    if (!isSeparator(lastReadChar))
        badFormatted("", "");
    if (lastReadChar == EOF)
        badFormatted("Could not find number of clauses parameter", "");

    numberOfClauses = readNumberUntilSpace(inputFile, &lastReadChar);
    if (!isSeparator(lastReadChar))
        badFormatted("", "");
    if (lastReadChar == EOF)
        badFormatted("Could not find number of clauses parameter", "");

    for (i = 0; i < numberOfClauses; i++) {
        tempClause = VECTORinit();
        do {
            readInt = readNumberUntilSpace(inputFile, &lastReadChar);
            printf("read %d, ", readInt);
            if (abs(readInt) < 0 || abs(readInt) > numberOfLits) {
                badFormatted("not a valid literal", "");
            }
            if (readInt != 0) {
                tempVar = VARinit((unsigned int) abs(readInt), readInt > 0);
                VECTORadd(tempClause, tempVar);
                printf("added\n");
            }

            if (!isSeparator(lastReadChar))
                badFormatted("", "");
            if (lastReadChar == EOF) {
                if (i < numberOfClauses - 1) {
                    badFormatted("Could not find enough clauses", "");
                } else {
                    break;
                }
            }

        } while (readInt != 0);

        printf("adding clause\n");
        VECTORadd(output, tempClause);
    }
    if (lastReadChar != EOF)
        printf("WARNING: Heading indicated %d clauses, they have been parsed correctly. EOF not reached.\n",
               numberOfClauses);
    printDebugOutput(output);

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
