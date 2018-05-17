#ifndef TESTS_SOURCE
#define TESTS_SOURCE

class Derived;

class Base
{
public:
    Base(Derived *derived): m_pDerived(derived) {}
    ~Base();
    virtual void function(void) = 0;

    Derived * m_pDerived;
};

#pragma warning(disable:4355)
class Derived : public Base
{
public:
    Derived() : Base(this) {};   // C4355
    virtual void function(void) {}
};

void AccessViolation();
void InvalidParameter();
void RaiseSigill();
void RaiseSigsegv();
void RaiseSigterm();
void ThrowException();

#endif // TESTS_SOURCE
