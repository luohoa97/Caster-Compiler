#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

char* stringVar = NULL;
int numVar = 0;
int* vectorVar = NULL;
char* globalMemory = NULL;

void stripComments(char* line) {
    char* commentIndex = strchr(line, '[');
    if (commentIndex != NULL) {
        *commentIndex = '\0';
    }
}

void replaceComments(char* line) {
    char* commentIndex = strstr(line, "[");
    if (commentIndex != NULL) {
        char* endCommentIndex = strstr(commentIndex, "]");
        if (endCommentIndex != NULL) {
            strncpy(commentIndex, "/*", 2);
            strncpy(endCommentIndex, "*/", 2);
        }
    }
}

char* readline() {
    char* line = NULL;
    size_t len = 0;
    if (getline(&line, &len, stdin) == -1) {
        printf("Error: Failed to read input\n");
        exit(EXIT_FAILURE);
    }
    line[strcspn(line, "\n")] = '\0';
    return line;
}

char* screenReadline(int number) {
    static char inputBuffer[64];
    snprintf(inputBuffer, sizeof(inputBuffer), "Placeholder input for line %d", number);
    return inputBuffer;
}

char* readArg(const char* argName) {
    printf("Enter value for argument '%s': ", argName);
    return readline();
}

void executeFunction(const char* functionName, char* args) {
    if (strcmp(functionName, "print") == 0) {
        printf("%s\n", args);
    } else if (strcmp(functionName, "sum") == 0) {
        int sum = 0;
        char* token = strtok(args, ",");
        while (token != NULL) {
            sum += atoi(token);
            token = strtok(NULL, ",");
        }
        printf("Sum: %d\n", sum);
    } else {
        printf("Function '%s' not recognized\n", functionName);
    }
}

void compile(const char* inputFilename, const char* outputFilename, int stripCommentsFlag) {
    FILE* inputFile = fopen(inputFilename, "r");
    if (!inputFile) {
        printf("Error: Unable to open file '%s'\n", inputFilename);
        exit(EXIT_FAILURE);
    }

    FILE* outputFile = fopen(outputFilename, "w");
    if (!outputFile) {
        printf("Error: Unable to open file '%s' for writing\n", outputFilename);
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fprintf(outputFile, "#include <stdio.h>\n");
    fprintf(outputFile, "#include <stdlib.h>\n\n");

    if (globalMemory != NULL) {
        fprintf(outputFile, "%s;\n", globalMemory);
    }

    fprintf(outputFile, "int main() {\n");
    int lineNumber = 1;
    while ((read = getline(&line, &len, inputFile)) != -1) {
        if (stripCommentsFlag) {
            stripComments(line);
        } else {
            replaceComments(line);
        }
        if (strlen(line) > 0 && strstr(line, "var") != line) {
            if (strstr(line, "screen.readline(") != NULL) {
                int lineNumber;
                sscanf(line, "screen.readline(%d)", &lineNumber);
                char* input = screenReadline(lineNumber);
                fprintf(outputFile, "    %s\n", input);
            } else if (strstr(line, "function ") != NULL) {
                char* functionName = strtok(line, " ");
                char* args = strtok(NULL, "\n");
                executeFunction(functionName, args);
            } else {
                fprintf(outputFile, "    %s\n", line);
            }
        }
        lineNumber++;
    }

    fclose(inputFile);
    fclose(outputFile);

    free(line);

    printf("Compilation completed successfully. C++ code written to '%s'\n", outputFilename);
}

int main() {
    int stripCommentsFlag;
    printf("Do you want to strip comments? (Enter 1 for Yes, 0 for No): ");
    if (scanf("%d", &stripCommentsFlag) != 1) {
        printf("Error: Invalid input\n");
        exit(EXIT_FAILURE);
    }
    getchar();

    int defineGlobalFlag;
    printf("Do you want to define global variables? (Enter 1 for Yes, 0 for No): ");
    if (scanf("%d", &defineGlobalFlag) != 1) {
        printf("Error: Invalid input\n");
        exit(EXIT_FAILURE);
    }
    getchar();

    if (defineGlobalFlag) {
        printf("Enter global variable definition: ");
        globalMemory = readline();
    }

    printf("Enter the path to the .co file: ");
    char* inputFilename = readline();

    printf("Enter the output filename: ");
    char* outputFilename = readline();

    compile(inputFilename, outputFilename, stripCommentsFlag);

    free(inputFilename);
    free(outputFilename);

    if (globalMemory != NULL) {
        free(globalMemory);
    }

    return 0;
}
