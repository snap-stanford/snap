#include <stdlib.h>

// only defined in stdlib.h
double drand48()
{
    unsigned int randomValue;
    rand_s(&randomValue);
    return ((double)randomValue) / UINT_MAX;
}
