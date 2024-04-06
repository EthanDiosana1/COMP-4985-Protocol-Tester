#ifndef STRING_TOOLS_C
#define STRING_TOOLS_C

#include <stdlib.h>

/**
 * @brief Contains useful string tools.
 * @author Ethan D.
 */

struct tokenized_string
{
    size_t numTokens;
    char **tokens;
};

/**
 * @brief Returns a struct containing a string array of tokens and the number of tokens.
 * @param str The string to tokenize.
 * @param delim The delimiter to tokenize the string by.
 * @param expectedMaxTokens The expected maximum number of tokens. The function will reallocate memory for every additional token after this limit.
 * @return tokenized_string
 */
struct tokenized_string tokenize_string(const char *str, const char *delim, size_t expectedMaxTokens);

/**
 * @brief Frees a tokenized string struct.
 * @param tokenized_string The struct to free.
 */
void free_tokenized_string(struct tokenized_string tokenized_string);

/**
 * @brief Frees every string in the provided array and thefrees the array itself.
 * @param stringArray The array to be freed.
 */
void freeStringArray(char **stringArray);

/**
 * @brief Reallocates a string array with a new length.
 * @param stringArray The array to reallocate.
 * @param length The new length to resize the array to.
 * @returns int
 */
int reallocateStringArray(char ***stringArray, size_t length);

#endif
