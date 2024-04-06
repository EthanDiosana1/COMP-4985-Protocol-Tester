#include "stringTools.h"
#include <stdio.h>
#include <string.h>

struct tokenized_string tokenize_string(const char *str, const char *delim, size_t expectedMaxTokens)
{
    struct tokenized_string tokenized_string;
    char                  **storedTokens;    // Stores the token.
    const char             *token;           // The current token.
    char                   *saveptr;
    char                   *str_copy;    // A copy of the string to tokenize.
    size_t                  numTokens = 1;

    // Throw error if delim is NULL.
    if(delim == NULL)
    {
        fprintf(stderr, "delim cannot be null.\n");
        exit(EXIT_FAILURE);
    }

    // Throw error if delim length is < 1.
    if(strlen(delim) < 1)
    {
        fprintf(stderr, "delim must be 1 or more chars in length.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize storedTokens.
    storedTokens = malloc(sizeof(char *) * expectedMaxTokens);

    // Copy the string.
    str_copy = strdup(str);

    // Tokenize the copy.
    token = strtok_r(str_copy, delim, &saveptr);
    while(token != NULL)
    {
        size_t index = numTokens - 1;

        // Duplicate the token and store it.
        storedTokens[index] = strdup(token);

        // Handle memory allocation failure.
        if(storedTokens[index] == NULL)
        {
            freeStringArray(storedTokens);
            free(str_copy);
            exit(EXIT_FAILURE);
        }

        // Reallocate storedTokens with more space as needed.
        if(numTokens % expectedMaxTokens == 0)
        {
            size_t newSize = (numTokens + expectedMaxTokens) * sizeof(char *);
            if(reallocateStringArray(&storedTokens, newSize) == EXIT_FAILURE)
            {
                free(str_copy);
                exit(EXIT_FAILURE);
            }
        }

        numTokens++;
        token = strtok_r(NULL, delim, &saveptr);
    }

    // Set the struct values.
    tokenized_string.numTokens = numTokens;
    tokenized_string.tokens    = storedTokens;

    // Free the malloced memory.
    if(str_copy != NULL)
    {
        free(str_copy);
    }

    return tokenized_string;
}

void free_tokenized_string(struct tokenized_string tokenized_string)
{
    // Free each token in the stirng.
    for(size_t i = 0; i < tokenized_string.numTokens; i++)
    {
        free(tokenized_string.tokens[i]);
    }
    free(tokenized_string.tokens);
}

void freeStringArray(char **stringArray)
{
    if(stringArray != NULL)
    {
        char **current = stringArray;
        while(*current != NULL)
        {
            free(*current);
            current++;
        }
        free(stringArray);
    }
}

int reallocateStringArray(char ***stringArray, size_t length)
{
    char **temp = realloc(*stringArray, (length * sizeof(char *)));

    // Handle memory allocation failure.
    if(temp == NULL)
    {
        fprintf(stderr, "failed to reallocate memory for string array\n");
        return EXIT_FAILURE;
    }
    *stringArray = temp;
    return EXIT_SUCCESS;
}
