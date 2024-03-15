#include <check.h>
#include <stdlib.h>

// Testing suites
#include "protocolClientTests.h"
#include "protocolCommonTests.h"

// Main function to run the tests
int main(void)
{
    // Run base protocol tests
    runCommonProtocolTests();
    testProtocolClient();

    return EXIT_SUCCESS;
}
