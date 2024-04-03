#include <check.h>
#include <stdlib.h>

// Testing suites
#include "../include/protocolServerManagerTests.h"
#include "protocolCommonTests.h"

// Main function to run the tests
int main(void)
{
    // Run base protocol tests
    runCommonProtocolTests();
    testProtocolServerManager();
    return EXIT_SUCCESS;
}
