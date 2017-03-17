#ifndef EXPPARADIGM_CDMRP_H
#define EXPPARADIGM_CDMRP_H
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include "FPGAControl.h"

class expParadigmCDMRPimplant
{
    int rep[100];
    int numTrials;
    double amp[100];
    double freq[100];
public:
    expParadigmCDMRPimplant(void);
    void readData(void);
    int startParadigm(motorControl *motorObj);
    ~expParadigmCDMRPimplant(void);
};
#endif