/**
 * @brief Contains useful tools for dealing with IP.
 * @author Ethan Diosana
 */

#ifndef IP_TOOLS
#define IP_TOOLS

#include <stdbool.h>
#include <stdint.h>

#define PORT_MIN 3000
#define PORT_MAX 10000

/**
 * @brief Converts a string to a uint16_t.
 * @param str The string to convert.
 * @returns uint16_t
 */
uint16_t str_to_uint16_t(const char *str);

/**
 * @brief Returns 1 if the ip is valid and 0 if not.
 * @param ip The ip to check.
 * @returns 1 if ip is valid, 0 if not.
 */
bool is_valid_ipv4(const char *ip);

#endif
