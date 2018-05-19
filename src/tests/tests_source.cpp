#include "tests_source.h"
#include <cstdio>
#include <csignal>

Base::~Base()
{
    m_pDerived -> function();
}

void AccessViolation() {
    // Access violation
    int *p = 0;
#pragma warning(disable : 6011)   // warning C6011: Dereferencing NULL pointer 'p'
    *p = 0;
#pragma warning(default : 6011)
}

void InvalidParameter() {
    char* formatString;
    // Call printf_s with invalid parameters.
    formatString = NULL;
#pragma warning(disable : 6387)   // warning C6387: 'argument 1' might be '0': this does not adhere to the specification for the function 'printf'
    printf(formatString);
#pragma warning(default : 6387)  
}

void RaiseSigill() {
    raise(SIGILL);
}

void RaiseSigsegv() {
    raise(SIGSEGV);
}

void RaiseSigterm() {
    raise(SIGTERM);
}

void ThrowException() {
    throw 123;
}

#pragma warning(disable: 4717) // avoid C4717 warning
#define BIG_NUMBER 0x1fffffff

void StackOverflow() {
    int *pi = new int[BIG_NUMBER];
    pi[BIG_NUMBER/4] = 123;
    pi[BIG_NUMBER/2] = 456;
    pi[3*BIG_NUMBER/4] = 789;

    StackOverflow();
}
