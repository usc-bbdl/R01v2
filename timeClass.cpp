#include "timeClass.h"
timeClass::timeClass()	
{
    actualTime = 0;
}
timeClass::~timeClass()	
{
}

int timeClass::resetTimer()
{
    actualTime = 0;
    return 0;
}
