#include "tests_source.h"

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
