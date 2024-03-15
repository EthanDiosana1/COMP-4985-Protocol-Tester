#include "baseProtocolTests.h"
#include "protocolLimitsTests.h"

int runBaseProtocolTests(void)
{
    // Test the limits of the protocol fields
    testProtocolLimits();

    // Other tests
    
    return EXIT_SUCCESS;
}

