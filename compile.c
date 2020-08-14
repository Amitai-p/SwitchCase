#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHAR 1000
#define MAX_LINE 500
#define RDI %rdi
#define RDX %rdx
#define RAX %rax
#define RDX %rdx
#define RCX %rcx

void readFromFile();

int numFromCase(char *str);

void checkMinMax(int *minCase, int *maxCase, int *flagFirstTime, int number);

typedef struct {
    int val;
    char *actionsInCase[100];
    char *actionsInAssembly[100];
    int numActions;
    int numAssemblyActions;
    int haveBreak;
    int placeInJumpTable;
} Case;

void checkActions(Case *caseArray, int sizeArray, char **, char **, char **, int, int);

void equal(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***, int *);

void plus(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***, int *);

void minus(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***, int *);

void mult(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***, int *);

void shiftLeft(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***, int *);

void shiftRight(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***, int *);

int main() {
    readFromFile();
    return 0;
}

void readFromFile() {
    int minCase = 0, maxCase = 0, number, flagFirstTime = 0, numCases = 0, sizeArray = 0, isCase = 0, index = -1,
            flagDefult = 0, i=0, j=0;
    char *str = (char *) malloc(MAXCHAR);
    char *tempStr;
    Case *caseArray = NULL;
    FILE *fileRead = fopen("switch.c", "r");
    if (fileRead == NULL) {
        printf("ERROR \n");
    }
    while (fgets(str, MAXCHAR, fileRead) != NULL) {
        if (strstr(str, "case") != NULL) {
            number = numFromCase(str);
            //numCases++;
            checkMinMax(&minCase, &maxCase, &flagFirstTime, number);
        }
    }
    fclose(fileRead);
    sizeArray = maxCase - minCase + 2;
    caseArray = (Case *) malloc(sizeof(Case) * sizeArray);
    for (i = 0; i < sizeArray; i++) {
        caseArray[i].numActions = 0;
        caseArray[i].numAssemblyActions = 0;
        caseArray[i].val = maxCase + 1;
    }
    fileRead = fopen("switch.c", "r");
    while (fgets(str, MAXCHAR, fileRead) != NULL) {
        //printf("%s", str);
        if (strstr(str, "case") != NULL) {
            tempStr = strtok(str, " ");
            tempStr = strtok(NULL, " ");
            tempStr = strtok(tempStr, ":");
            number = atoi(tempStr);
            number -= minCase;
            isCase = 1;
            index++;
        }
        if (isCase) {
            caseArray[index].val = atoi(tempStr) - minCase;
            caseArray[index].haveBreak = 0;
            numCases++;
            isCase = 0;
        } else if (strstr(str, "}") != NULL) {
            break;
        } else if (strstr(str, "break") != NULL) {
            // have a break.
            caseArray[index].haveBreak = 1;
        } else if (strstr(str, "{") != NULL) {
            continue;
        } else if (strstr(str, "default") != NULL) {
            // for defult case.
            flagDefult = 1;
            caseArray[index].val = sizeArray - 1;
            //caseArray[index].haveBreak = 0;
        } else {
            if (!flagDefult) {
                // get the lines of actions.
                if(index<0){
                    continue;
                }
                caseArray[index].actionsInCase[caseArray[index].numActions] = (char *) malloc(strlen(str) + 1);
                strcpy(caseArray[index].actionsInCase[caseArray[index].numActions], str);
                caseArray[index].numActions++;
                // caseArray[index].numAssemblyActions++;
            } else {
                // get the lines of actions.
                caseArray[numCases].actionsInCase[caseArray[numCases].numActions] =
                        (char *) malloc(strlen(str) + 1);
                strcpy(caseArray[numCases].actionsInCase[caseArray[numCases].numActions], str);
                caseArray[numCases].numActions++;
                caseArray[numCases].haveBreak=0;
                //flagDefult = 0;

                //   caseArray[number].numAssemblyActions++;
            }
        }
    }
    for (i = 0; i < sizeArray; i++) {
        for (j = 0; j < caseArray[i].numActions; j++) {
            // printf("%d ,%d ,%s",i,j, caseArray[i].actionsInCase[j]);
        }
    }
    char *var1 = NULL, *var2 = NULL, *op = NULL, *var1A, *var2A;
    fclose(fileRead);
    checkActions(caseArray, numCases+1, &var1, &var2, &op, minCase, maxCase);
    for (i = 0; i < sizeArray; i++) {
        for (j = 0; j < caseArray[i].numActions; j++) {
            // printf("%d ,%d ,%s",i,j, caseArray[i].actionsInAssembly[j]);
        }
    }
}

int numFromCase(char *str) {
    char *tempStr;
    int number;
    tempStr = strtok(str, " ");
    tempStr = strtok(NULL, " ");
    tempStr = strtok(tempStr, ":");
    number = atoi(tempStr);
    return number;
}

void checkMinMax(int *minCase, int *maxCase, int *flagFirstTime, int number) {
    if (!*flagFirstTime) {
        *minCase = number;
        *maxCase = number;
        *flagFirstTime = 1;
    }
    if (number > *maxCase) {
        *maxCase = number;
    }
    if (number < *minCase) {
        *minCase = number;
    }
}

void swapOfCases(Case *case1, Case *case2) {
    Case tempCase = *case1;
    *case1 = *case2;
    *case2 = tempCase;
}

Case *bubbleSortOfCases(Case *cases, int size) {
    int i, j;
    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - i - 1; j += 1) {
            if (cases[j].val > cases[j + 1].val)
                swapOfCases(&(cases[j]), &(cases[j + 1]));
        }
    }
    return cases;
}

void
checkActions(Case *caseArray, int sizeArray, char **var1ptr, char **var2ptr, char **opPtr, int minCase, int maxCase) {
    int numOp = 0, i=0, j=0, k=0;
    int numAssembly = 0;
    FILE *output = fopen("switch.s", "w+");
    if (output == NULL) {
        printf("ERROR \n");
    }
    long int tempNum1 = minCase;
    long int tempNum2 = maxCase - minCase;
    fprintf(output, ".section .text\n");
    fprintf(output, ".globl switch2\n");
    fprintf(output, "switch2:\n");
    fprintf(output, "movq $0,%%rax\n");
    fprintf(output, "subq $%ld,%%rdx\n", tempNum1);
    fprintf(output, "cmpq $%ld,%%rdx\n", tempNum2);
    fprintf(output, "ja .L%d \n", sizeArray);
    fprintf(output, "jmp *.L%d(,%%rdx,8)\n", maxCase-minCase + 5);
    char **tempAssembly = NULL;
    for (i = 0; i < sizeArray; i++) {
        char *tempForFile = ".L";
        fprintf(output, "%s", tempForFile);
        fprintf(output, "%d", i + 1);
        fprintf(output, "%s", ":\n");
        caseArray[i].placeInJumpTable = i+1;
        for (j = 0; j < caseArray[i].numActions; j++) {
            char *op = NULL;
            char *temp = "";
            char ch;
            char *str = caseArray[i].actionsInCase[j];
            char *var1 = strtok(str, "=");
            char *var2 = strtok(NULL, "=");
            var2 = strtok(var2, ";");
            op = "=";
            char* str1 = strtok(str, "=");
            //printf("str: %s   \n", str1);
            char* str2 = strtok(NULL, " ");
            // printf("%s  %s \n", str1, str2);
            if (strlen(var1) > 5) {
                if (strstr(var1, "result")) {
                    ch = var1[8];
                } else {
                    ch = var1[5];
                }
                numOp = 0;
                switch (ch) {
                    case '+':
                        op = "+=";
                        numOp = 1;
                        break;
                    case '-':
                        op = "-=";
                        numOp = 2;
                        break;
                    case '*':
                        op = "*=";
                        numOp = 3;
                        break;
                    case '<':
                        op = "<<=";
                        numOp = 4;
                        break;
                    case '>':
                        op = ">>=";
                        numOp = 5;
                        break;
                }
            }
            var1 = strtok(var1, " ");
            var2 = strtok(var2, " ");
            switch (numOp) {
                case 0:
                    equal(caseArray[i], var1, op, var2, numOp, &tempAssembly, &numAssembly);
                    break;
                case 1:
                    plus(caseArray[i], var1, op, var2, numOp, &tempAssembly, &numAssembly);
                    break;
                case 2:
                    minus(caseArray[i], var1, op, var2, numOp, &tempAssembly, &numAssembly);
                    break;
                case 3:
                    mult(caseArray[i], var1, op, var2, numOp, &tempAssembly, &numAssembly);
                    break;
                case 4:
                    shiftLeft(caseArray[i], var1, op, var2, numOp, &tempAssembly, &numAssembly);
                    break;
                case 5:
                    shiftRight(caseArray[i], var1, op, var2, numOp, &tempAssembly, &numAssembly);
                    break;
            }
            for (k = 0; k < numAssembly; k++) {
                fprintf(output, "%s", tempAssembly[k]);
            }
        }
        if(caseArray[i].haveBreak){
            fprintf(output, "%s", "jmp .L");
            fprintf(output, "%d\n", sizeArray+1);
        }
    }
    caseArray = bubbleSortOfCases(caseArray, sizeArray);
    fprintf(output, "%s", ".L");
    fprintf(output, "%d", sizeArray+1);
    fprintf(output, "%s\n", ":");
    fprintf(output, "%s\n", "ret");
    fprintf(output, ".section .rodata\n" ".align 8\n");
    fprintf(output, ".L%d:\n", maxCase-minCase + 5);
    int flag = -1;
    int numJump = 0;
    for (i = 0; i < sizeArray; i++) {
        if (caseArray[i].val != caseArray[sizeArray - 1].val) {
            if (flag != -1) {
                for (j = 1; j < caseArray[i].val - flag; ++j) {
                    fprintf(output, "\t.quad .L%d\n", sizeArray);
                }
            }
            fprintf(output, "\t.quad .L%d\n", caseArray[i].placeInJumpTable);
            numJump++;
            flag = caseArray[i].val;
        }
    }
    fclose(output);
    return;
}

void equal(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***tempAssembly, int *numAssembly) {
    char line[MAX_LINE];
    if (strcmp(var1, "*p1") == 0) {
        if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "movq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "movq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "*p2") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "movq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "movq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "result") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "movq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "movq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    }
    *tempAssembly = thisCase.actionsInAssembly;
    *numAssembly = thisCase.numAssemblyActions;
}

void plus(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***tempAssembly, int *numAssembly) {
    char line[MAX_LINE];
    if (strcmp(var1, "*p1") == 0) {
        if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq(%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "*p2") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "result") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "addq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    }
    *tempAssembly = thisCase.actionsInAssembly;
    *numAssembly = thisCase.numAssemblyActions;
}

void minus(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***tempAssembly, int *numAssembly) {
    char line[MAX_LINE];
    if (strcmp(var1, "*p1") == 0) {
        if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq(%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "*p2") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "result") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "subq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    }
    *tempAssembly = thisCase.actionsInAssembly;
    *numAssembly = thisCase.numAssemblyActions;
}

void mult(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***tempAssembly, int *numAssembly) {
    char line[MAX_LINE];
    if (strcmp(var1, "*p1") == 0) {
        if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq(%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "*p2") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "result") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rbx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "imulq %rbx,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    }
    *tempAssembly = thisCase.actionsInAssembly;
    *numAssembly = thisCase.numAssemblyActions;
}

void shiftLeft(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***tempAssembly, int *numAssembly) {
    char line[MAX_LINE];
    if (strcmp(var1, "*p1") == 0) {
        if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq(%rdi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "*p2") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "result") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "shlq %cl,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    }
    *tempAssembly = thisCase.actionsInAssembly;
    *numAssembly = thisCase.numAssemblyActions;
}

void shiftRight(Case thisCase, char *var1, char *op, char *var2, int numOp, char ***tempAssembly, int *numAssembly) {
    char line[MAX_LINE];
    if (strcmp(var1, "*p1") == 0) {
        if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq(%rdi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,(%rdi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "*p2") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,(%rsi)\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    } else if (strcmp(var1, "result") == 0) {
        if (strcmp(var2, "*p1") == 0) {
            strcpy(line, "movq (%rdi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "*p2") == 0) {
            strcpy(line, "movq (%rsi),%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else if (strcmp(var2, "result") == 0) {
            strcpy(line, "movq %rax,%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        } else {
            strcpy(line, "movq $");
            strcat(line, var2);
            strcat(line, ",%rcx\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
            strcpy(line, "sarq %cl,%rax\n");
            thisCase.actionsInAssembly[thisCase.numAssemblyActions] =
                    (char *) malloc((strlen(line) + 1) * sizeof(char));
            strcpy(thisCase.actionsInAssembly[thisCase.numAssemblyActions], line);
            thisCase.numAssemblyActions += 1;
        }
    }
    *tempAssembly = thisCase.actionsInAssembly;
    *numAssembly = thisCase.numAssemblyActions;
}




