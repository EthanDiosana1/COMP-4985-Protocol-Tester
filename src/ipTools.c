#include "ipTools.h"
#include "stringTools.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define BASE_10 10

#define IPV4_TOKENS_MAX 4

/**
 * @brief Converts a string to a uint16_t. Returns NULL on fail.
 * @param str The string to convert.
 * @returns uint16_t or NULL
 */
uint16_t *str_to_uint16_t(const char *str)
{
    long int  result;    // The result of strtol
    uint16_t *output;    // The number to be outputted by the function
    char     *endptr;
    int       base;

    base   = BASE_10;
    output = malloc(sizeof(uint16_t));

    errno  = 0;
    result = strtol(str, &endptr, base);

    if(errno == EINVAL)
    {
        perror("strtol");
        free(output);
        return NULL;
    }

    if(errno == ERANGE)
    {
        perror("strtol");
        free(output);
        return NULL;
    }

    if(endptr == str)
    {
        fprintf(stderr, "no digits were found\n");
        free(output);
        return NULL;
    }

    *output = (uint16_t)result;
    return output;
}

/**
 * @brief Returns 1 if the ip is valid and 0 if not.
 * @param ip The ip to check.
 * @returns 1 if ip is valid, 0 if not.
 */
bool is_valid_ipv4(const char *ip)
{
    struct tokenized_string tokenized_string;
    const char             *delim = ".";

    tokenized_string = tokenize_string(ip, delim, IPV4_TOKENS_MAX);
    // print_tokenized_string(tokenized_string);
    if(tokenized_string.numTokens != IPV4_TOKENS_MAX)
    {
        free_tokenized_string(tokenized_string);
        return false;
    }

    free_tokenized_string(tokenized_string);

    return true;
}
