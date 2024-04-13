#ifndef PROTOCOL_SERVER_SM_TESTS_FUNCTIONS_C
#define PROTOCOL_SERVER_SM_TESTS_FUNCTIONS_C

/**
 * @brief Connects to the server with the correct passcode.
 */
int connect_correct_passcode(int argc, char *argv[]);

/**
 * @brief Connects to the server with the incorrect passcode a given number of times.
 * @param argc The number of arguments.
 * @param argv The arguments.
 */
int connect_incorrect_passcode(int argc, char *argv[]);

/*
 * @brief Spawns a client that connects to the server while the server manager is connected.
 * @param argc The number of arguments.
 * @param argv The arguments.
 */
int client_connect_normal(int argc, char *argv[]);

/**
 * @brief Spawns a client that connects to the server while the server manager is connected AFTER sending the shutdown command.
 * @param argc The number of arguments.
 * @param argv The arguments.
 *
 */
int client_connect_after_sending_q(int argc, char *argv[]);
#endif
