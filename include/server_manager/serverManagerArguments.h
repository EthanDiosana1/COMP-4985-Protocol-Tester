#ifndef SERVER_MANAGER_ARGUMENTS
#define SERVER_MANAGER_ARGUMENTS

#define SERVER_MANAGER_NUM_ARGS 4

#include <stdint.h>

struct server_manager_arguments
{
    char    *ip;
    uint16_t port;
    char    *passcode;
};

/**
 * @brief Prints the contents of the given server_arguments struct.
 * @param arguments The struct to print.
 */
void print_server_manager_arguments(struct server_manager_arguments arguments);

/**
 * @brief Frees the contents of the given struct.
 * @param arguments The struct which needs freeing.
 */
void free_server_manager_arguments(struct server_manager_arguments *arguments);

/**
 * @brief Returns a arguments struct if the arguments are valid.
 *
 * @param argc The number of args.
 * @param argv The arguments.
 */
struct server_manager_arguments *parse_args(int argc, const char *argv[]);

#endif
