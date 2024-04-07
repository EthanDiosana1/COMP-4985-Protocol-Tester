#include "stringTools.h"
#include <stdio.h>
#include <string.h>

#define ERR_DELIM_EMPTY "delimiter must be 1 or more characters in length.\n"
#define ERR_DELIM_NULL "delimiter cannot be null\n"
#define ERR_MALLOC_FAILED "memory allocation failed\n"

struct tokenized_string tokenize_string(const char *str, const char *delim, size_t expectedMaxTokens)
{
    struct tokenized_string tokenized_string;
    char                  **storedTokens;
    const char             *token;
    char                   *saveptr;
    char                   *str_copy;
    size_t                  numTokens = 1;

    if(delim == NULL)
    {
        fprintf(stderr, ERR_DELIM_NULL);
        exit(EXIT_FAILURE);
    }

    if(strlen(delim) < 1)
    {
        fprintf(stderr, ERR_DELIM_EMPTY);
        exit(EXIT_FAILURE);
    }

    // Initlialize token storage.
    storedTokens = malloc(sizeof(char *) * (expectedMaxTokens + 1));
    if(storedTokens == NULL)
    {
        fprintf(stderr, ERR_MALLOC_FAILED);
        exit(EXIT_FAILURE);
    }

    // Copy the string.
    str_copy = strdup(str);
    if(str_copy == NULL)
    {
        free(storedTokens);
        fprintf(stderr, ERR_MALLOC_FAILED);
        exit(EXIT_FAILURE);
    }

    // Tokenize the string.
    token = strtok_r(str_copy, delim, &saveptr);
    while(token != NULL)
    {
        printf("token: %s\n", token);
        if(numTokens >= expectedMaxTokens)
        {
            size_t newSize = (numTokens + expectedMaxTokens + 1) * sizeof(char *);
            if(reallocateStringArray(&storedTokens, newSize) == EXIT_FAILURE)
            {
                fprintf(stderr, ERR_MALLOC_FAILED);
                free(str_copy);
                exit(EXIT_FAILURE);
            }
        }
        storedTokens[numTokens] = strdup(token);
        printf("stored token: %s\n", storedTokens[numTokens]);
        if(storedTokens[numTokens] == NULL)
        {
            fprintf(stderr, ERR_MALLOC_FAILED);
            free(str_copy);
            exit(EXIT_FAILURE);
        }
        numTokens++;
        token = strtok_r(NULL, delim, &saveptr);
    }

    // Ensuring the array is properly null-terminated
    storedTokens[numTokens] = NULL;

    tokenized_string.numTokens = numTokens;
    tokenized_string.tokens    = storedTokens;

    free(str_copy);

    printf("[SUCCESS]: string tokenized.\n");
    return tokenized_string;
}

void free_tokenized_string(struct tokenized_string tokenized_string)
{
    // Free each token in the string array.
    printf("freeing tokenized string...\n");
    if(tokenized_string.tokens == NULL)
    {
        fprintf(stderr, "tokenized_string.tokens is NULL\n");
        exit(EXIT_FAILURE);
    }
    for(size_t i = 0; i < tokenized_string.numTokens; i++)
    {
        if(tokenized_string.tokens[i] != NULL)
        {
            printf("Freeing token at %zu: %s\n", i, tokenized_string.tokens[i]);
            printf("%s\n", tokenized_string.tokens[i]);

            free(tokenized_string.tokens[i]);
        }
    }
    free(tokenized_string.tokens);
}

void print_tokenized_string(struct tokenized_string tokenized_string)
{
    char *tokenString;

    tokenString = string_array_to_string(tokenized_string.tokens, ",", tokenized_string.numTokens);

    printf("struct tokenized_string {\n\tnumTokens: %zu\n\ttokens: [%s]\n}", tokenized_string.numTokens, tokenString);

    free(tokenString);
}

char *string_array_to_string(char **stringArray, const char *delim, size_t len)
{
    size_t totalLength = 0;
    char  *output;

    if(stringArray == NULL)
    {
        fprintf(stderr, "stringArray cannot be null\n");
        exit(EXIT_FAILURE);
    }

    if(delim == NULL)
    {
        fprintf(stderr, ERR_DELIM_NULL);
        exit(EXIT_FAILURE);
    }

    if(strlen(delim) < 1)
    {
        fprintf(stderr, ERR_DELIM_EMPTY);
        exit(EXIT_FAILURE);
    }

    // Calculate total length needed.
    for(size_t i = 0; i < len; i++)
    {
        totalLength += strlen(stringArray[i] + strlen(delim));
    }

    // Remove last delim length.
    if(len > 0)
    {
        totalLength -= strlen(delim);
    }

    // Allocate memory for output.
    output = malloc(totalLength + 1);
    if(output == NULL)
    {
        fprintf(stderr, ERR_MALLOC_FAILED);
        exit(EXIT_FAILURE);
    }

    // Build the string.
    output[0] = '\0';
    for(size_t i = 0; i < len; i++)
    {
        strlcat(output, stringArray[i], strlen(stringArray[i]));

        // Do not put delim after last element.
        if(i < (len - 1))
        {
            strlcat(output, delim, strlen(delim));
        }
    }

    return output;
}

void freeStringArray(char **stringArray)
{
    printf("freeing string array...\n");
    if(stringArray != NULL)
    {
        char **current = stringArray;
        while(*current != NULL)
        {
            printf("FREEING: %s\n", *current);
            free(*current);
            current++;
        }
        free(stringArray);
    }
    else
    {
        printf("string array is already NULL\n");
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
