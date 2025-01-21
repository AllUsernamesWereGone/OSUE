#include "fork.h"

#define INITIAL_CAPACITY 10

void merge(FILE* output, char** arr1, char** arr2, u_int counter1, u_int counter2) {

    int i=0, j=0;

    while (i < counter1 && j < counter2) {
        if (strcmp(arr1[i], arr2[j])<0) {
            fprintf(output, "%s", arr1[i]);
            i++;
        }
        else {
            fprintf(output, "%s", arr2[j]);
            j++;
        }
    }

    // Copy the remaining elements of arr1[], if any
    while (i < counter1) {
        fprintf(output, "%s", arr1[i]);
        i++;
    }

    // Copy the remaining elements of arr2[], if any
    while (j < counter2) {
        fprintf(output, "%s", arr2[j]);
        j++;
    }
}


char **readInput(FILE *dataInput, unsigned int *count) {
    char **lines = NULL;  // Array of strings (lines)
    size_t capacity = INITIAL_CAPACITY;  // Initial capacity of the array
    size_t size = 0;      // Number of lines stored
    char *buffer = NULL;  // Temporary buffer for each line
    size_t bufferSize = 0;

    // Allocate memory for the initial array
    lines = malloc(capacity * sizeof(char *));
    if (!lines) {
        perror("malloc\n");
        return NULL;  // Return NULL to indicate failure
    }

    // Read lines from the provided input
    while (getline(&buffer, &bufferSize, dataInput) != -1) {
    //for (int a = 0; a <5 ; a++){

        // Resize the array if needed
        if (size == capacity) {
            capacity *= 2;  // Double the capacity
            char **newLines = realloc(lines, capacity * sizeof(char *));
            if (!newLines) {
                perror("realloc\n");
                free(buffer);
                for (size_t i = 0; i < size; i++) {
                    free(lines[i]);
                }
                free(lines);
                return NULL;
            }
            lines = newLines;
        }

        // Store the line in the array
        lines[size] = strdup(buffer);  // Duplicate the buffer
        if (!lines[size]) {
            perror("strdup\n");
            free(buffer);
            for (size_t i = 0; i < size; i++) {
                free(lines[i]);
            }
            free(lines);
            return NULL;
        }
        size++;
    }

    // Free the buffer used by getline
    free(buffer);

    // Set the count to the number of lines read
    *count = size;

    return lines;  // Return the dynamically allocated array
}