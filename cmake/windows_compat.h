#pragma once

// msvc does not support token for logical operators
// https://stackoverflow.com/questions/24414124/why-does-vs-not-define-the-alternative-tokens-for-logical-operators
#include <ciso646>

// sometimes assumed
#include <string>

// only defined in stdlib.h
double drand48();
