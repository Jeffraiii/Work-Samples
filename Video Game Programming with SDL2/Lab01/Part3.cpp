#include "Parts.h"
#include <iostream>

struct MyStruct
{
	bool mBool = false;
	float mValue = 0.0f;
};

void Part3()
{
	MyStruct s;
	
	if (s.mValue)
	{
		std::cout << "TRUE: " << s.mValue << std::endl;
	}
	else
	{
		std::cout << "FALSE: " << s.mValue << std::endl;
	}
}
