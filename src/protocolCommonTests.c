#include "../include/protocolCommonTests.h"
#include "../include/protocolLimitsTests.h"
#include <stdlib.h>

int runCommonProtocolTests(void)
{
    // Test the limits of the protocol fields
    testProtocolLimits();

    // Other tests

    return EXIT_SUCCESS;
}
