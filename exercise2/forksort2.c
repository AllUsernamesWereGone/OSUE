#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void merge(FILE *left, FILE *right, FILE *output) {
    char leftLine[BUFFER_SIZE], rightLine[BUFFER_SIZE];
    char *leftStatus = fgets(leftLine, BUFFER_SIZE, left);
    char *rightStatus = fgets(rightLine, BUFFER_SIZE, right);

    while (leftStatus && rightStatus) {
        if (strcmp(leftLine, rightLine) <= 0) {
            fputs(leftLine, output);
            leftStatus = fgets(leftLine, BUFFER_SIZE, left);
        } else {
            fputs(rightLine, output);
            rightStatus = fgets(rightLine, BUFFER_SIZE, right);
        }
    }

    while (leftStatus) {
        fputs(leftLine, output);
        leftStatus = fgets(leftLine, BUFFER_SIZE, left);
    }
    while (rightStatus) {
        fputs(rightLine, output);
        rightStatus = fgets(rightLine, BUFFER_SIZE, right);
    }
}

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void sort_recursive(char *lines[], int num_lines) {
    if (num_lines == 1) {
        printf("%s", lines[0]);
        exit(EXIT_SUCCESS);
    }

    int mid = num_lines / 2;
    int pipeLeft[2], pipeRight[2];

    if (pipe(pipeLeft) == -1 || pipe(pipeRight) == -1)
        handle_error("pipe");

    pid_t leftPid = fork();
    if (leftPid == -1)
        handle_error("fork");

    if (leftPid == 0) {
        close(pipeLeft[0]);
        dup2(pipeLeft[1], STDOUT_FILENO);
        close(pipeLeft[1]);

        sort_recursive(lines, mid);
        exit(EXIT_SUCCESS);
    }

    pid_t rightPid = fork();
    if (rightPid == -1)
        handle_error("fork");

    if (rightPid == 0) {
        close(pipeRight[0]);
        dup2(pipeRight[1], STDOUT_FILENO);
        close(pipeRight[1]);

        sort_recursive(lines + mid, num_lines - mid);
        exit(EXIT_SUCCESS);
    }

    close(pipeLeft[1]);
    close(pipeRight[1]);

    FILE *leftStream = fdopen(pipeLeft[0], "r");
    FILE *rightStream = fdopen(pipeRight[0], "r");

    if (!leftStream || !rightStream)
        handle_error("fdopen");

    merge(leftStream, rightStream, stdout);

    fclose(leftStream);
    fclose(rightStream);

    int leftStatus, rightStatus;
    waitpid(leftPid, &leftStatus, 0);
    waitpid(rightPid, &rightStatus, 0);

    if (!WIFEXITED(leftStatus) || WEXITSTATUS(leftStatus) != EXIT_SUCCESS ||
        !WIFEXITED(rightStatus) || WEXITSTATUS(rightStatus) != EXIT_SUCCESS) {
        fprintf(stderr, "Child process error\n");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    char *lines[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int count = 0;

    while (fgets(buffer, BUFFER_SIZE, stdin)) {
        lines[count] = strdup(buffer);
        if (!lines[count])
            handle_error("strdup");
        count++;
    }

    if (count == 0)
        return EXIT_SUCCESS;

    sort_recursive(lines, count);

    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }

    return EXIT_SUCCESS;
}