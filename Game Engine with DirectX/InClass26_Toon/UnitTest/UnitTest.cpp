// UnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Physics.h"
#include <assert.h>

int main()
{
    bool ret = Physics::UnitTest();
    assert(ret);
    if (ret)
        return 0;
    return 1;
}

