#include <check.h>
#include <stdlib.h>

// Testing suites
#include "../include/protocolCommonTests.h"

// Main function to run the tests
int main(void)
{
    // Run base protocol tests
    runCommonProtocolTests();

    return EXIT_SUCCESS;
}
