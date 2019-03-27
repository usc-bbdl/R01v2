#include "matrixFunctions.h"

#include <utilities.h>
#include <stdio.h>

//matrixMultiply
//only works for square matrices of len 6. Used specifically for the JR3 calibration matrix application to the raw JR3 data.

//@param MatrixInput a square 6x6 matrix
//@param vectorOfVolts the output volts that are observed
//@return vectorOfNewtons the resultant Newtons mapping
int matrixFunctions::matrixMultiply(const double matrixInput[6][6], double vectorOfVolts[], double vectorOfNewtons[])
{
        for(int i=0; i<6; i++) {
                vectorOfNewtons[i]=0;
                for(int k=0; k<6; k++) {
                        vectorOfNewtons[i]=vectorOfNewtons[i]+(matrixInput[i][k]*vectorOfVolts[k]);
                }
        }
        return(0);
}
