#include "logger.h"

#include <time.h>
#include <process.h>

//construct header and second line, open a file and print header&second line
logger::logger(motorData* Data){
    mData=Data;

    //construct data file header
    strcpy(header,"Time, Exp Prot, Len1, Len2, ForcMeas1, ForcMeas2,");
    if (dataAcquisitionFlag[0]){
        strcat (header, ", ForceDes1, ForceDes2");
    }
    if (dataAcquisitionFlag[1]){
        strcat (header, ", EMG1, EMG2");
    }
    if (dataAcquisitionFlag[2]){
        strcat (header, ", Ia1, Ia2");
    }
    if (dataAcquisitionFlag[3]){
        strcat (header, ", II1, II2");
    }
    if (dataAcquisitionFlag[4]){
        strcat (header, ", Spike Count1, Spike Count2");
    }
    if (dataAcquisitionFlag[5]){
        strcat (header, ", Raster 1-1,  Raster 2-1");
    }
    if (dataAcquisitionFlag[6]){
        strcat (header, ", Raster 1-2,  Raster 2-2");
    }
    if (dataAcquisitionFlag[7]){
        strcat (header, ", Raster 1-3,  Raster 2-3");
    }
    if (dataAcquisitionFlag[8]){
        strcat (header, ", Raster 1-4,  Raster 2-4");
    }
    if (dataAcquisitionFlag[9]){
        strcat (header, ", Raster 1-5,  Raster 2-5");
    }
    if (dataAcquisitionFlag[10]){
        strcat (header, ", Raster 1-6,  Raster 2-6");
    }

    //second line of data file
    char dataTemp[100]="";
    strcat(header,"\n");
    sprintf(dataTemp,"%d,%d,%d,%d,",dataAcquisitionFlag[0],dataAcquisitionFlag[1],dataAcquisitionFlag[2],dataAcquisitionFlag[3]);
    strcat(header,dataTemp);
    sprintf(dataTemp,"%d,%d,%d,%d,",dataAcquisitionFlag[4],dataAcquisitionFlag[5],dataAcquisitionFlag[6],dataAcquisitionFlag[7]);
    strcat(header,dataTemp);
    sprintf(dataTemp,"%d,%d,%d,%d\n",dataAcquisitionFlag[8],dataAcquisitionFlag[9],dataAcquisitionFlag[10],dataAcquisitionFlag[11]);
    strcat(header,dataTemp);

    //create data file
    time_t t = time(NULL);
    tm* timePtr = localtime(&t);
    char fileName[200];
    char dataSample[600]="";//1 line of data
    //char dataTemp[100]="";//additional data add to datasample
    sprintf_s(
            fileName,
            "C:\\data_logger_function_test\\realTimeData%4d_%02d_%02d_%02d_%02d_%02d.txt",
            timePtr->tm_year+1900, 
            timePtr->tm_mon+1, 
            timePtr->tm_mday, 
            timePtr->tm_hour, 
            timePtr->tm_min, 
            timePtr->tm_sec
            );
    dataFile = fopen(fileName,"w");
    //no longer write header here
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Motor Command1, Motor Command2, Length 1, Length2, Velocity1, Velocity2, EMG1, EMG2, is sample missed\n");
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Length 1, Length2, Velocity1, Velocity2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    //fprintf(dataFile,"Time, Load Cell1, Load Cell2, Length 1, Length2, motorRef1, motorRef2, spindleIa1, spindleIa2, spindleII1, spindleII2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    //fprintf(dataFile,"Time, Length 1, Length2, EMG1, EMG2, GammaStat, GammaDyn, is sample missed\n");
    //write existed header to datafile
    fprintf(dataFile,header);

}

//the print one line thread
void logger::logOneLine(){
    //timeNprotocol *args = (timeNprotocol*) parameters;
    //double tick=args->tick;
    //double tock=args->tock;
    //int expProtocol=args->expProtocol;

    char dataSample[600]="";//1 line of data
    char dataTemp[100]="";//additional data add to datasample

    //output to data file
    //fprintf(dataFile,"%.3f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d\n",tock,loadCellData[0],loadCellData[1],motorRef[0],motorRef[1], muscleLength[0], muscleLength[1], muscleVel[0],muscleVel[1], muscleEMG[0], muscleEMG[1], isLate);
    //fprintf(dataFile,"%.3f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d\n",tock,loadCellData[0],loadCellData[1], muscleLength[0], muscleLength[1], muscleVel[0],muscleVel[1], muscleEMG[0], muscleEMG[1], gammaStatic, gammaDynamic, isLate);
    //fprintf(dataFile,"%.3f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d\n",tock, muscleLength[0], muscleLength[1], muscleEMG[0], muscleEMG[1], gammaStatic, gammaDynamic, isLate);
    
    //construct 1 line of data(datasample)
    sprintf(dataSample,"%.3f,%d,%.6f,%.6f,%.6f,%.6f",tock,expProtocol,mData->muscleLength[0], mData->muscleLength[1], mData->loadCellData[0],mData->loadCellData[1]);
    //add additional data to the line
    if (dataAcquisitionFlag[0]){
        sprintf(dataTemp,",%.6f,%.6f",mData->motorRef[0],mData->motorRef[1]);
        strcat (dataSample, dataTemp);
    }
    if (dataAcquisitionFlag[1]){
        sprintf(dataTemp,",%.6f,%.6f",mData->muscleEMG[0], mData->muscleEMG[1]);
        strcat (dataSample, dataTemp);
    }
        if (dataAcquisitionFlag[2]){
        sprintf(dataTemp,",%.6f,%.6f",mData->spindleIa[0], mData->spindleIa[1]);
        strcat (dataSample, dataTemp);
    }
    if (dataAcquisitionFlag[3]){
        sprintf(dataTemp,",%.6f,%.6f",mData->spindleII[0], mData->spindleII[1]);
        strcat (dataSample, dataTemp);
    }
    if (dataAcquisitionFlag[4]){
        sprintf(dataTemp,",%d,%d",mData->muscleSpikeCount[0], mData->muscleSpikeCount[1]);
        strcat (dataSample, dataTemp);
    }
    if (dataAcquisitionFlag[5]){
        sprintf(dataTemp,",%u,%u",mData->raster_MN_1[0], mData->raster_MN_1[1]);
        strcat (dataSample, dataTemp);
    }
    if (dataAcquisitionFlag[6]){
        sprintf(dataTemp,",%u,%u",mData->raster_MN_2[0], mData->raster_MN_2[1]);
        strcat (dataSample, dataTemp);
    }
    if (dataAcquisitionFlag[7]){
        sprintf(dataTemp,",%u,%u",mData->raster_MN_3[0], mData->raster_MN_3[1]);
        strcat (dataSample, dataTemp);
    }
    if (dataAcquisitionFlag[8]){
        sprintf(dataTemp,",%u,%u",mData->raster_MN_4[0], mData->raster_MN_4[1]);
        strcat (dataSample, dataTemp);
    }
    if (dataAcquisitionFlag[9]){
        sprintf(dataTemp,",%u,%u",mData->raster_MN_5[0], mData->raster_MN_5[1]);
        strcat (dataSample, dataTemp);
    }
    if (dataAcquisitionFlag[10]){
        sprintf(dataTemp,",%u,%u",mData->raster_MN_6[0], mData->raster_MN_6[1]);
        strcat (dataSample, dataTemp);
    }
    if (dataAcquisitionFlag[11]){
        mData->cortexDrive[0] = max((mData->cortexVoluntaryAmp -0) * sin (2 * 3.1416 * mData->cortexVoluntaryFreq * tick), 0);
        mData->cortexDrive[1] = max((mData->cortexVoluntaryAmp -0) * sin (2 * 3.1416 * mData->cortexVoluntaryFreq * tick + 3.1416), 0);
    }
    //sprintf(dataTemp,",%d,%d,%d,%d,%.3f,%.3f,%d\n",gammaStatic1, gammaDynamic1, gammaStatic2, gammaDynamic2, cortexDrive[0], cortexDrive[1],newTrial);
    sprintf(dataTemp,"\n");
        
    //add "\n" to datasample
    strcat (dataSample, dataTemp);

    //add 1 line(datasample) to the file
    fprintf(dataFile,dataSample);

}

void logger::startthread(void * parameters){
    _beginthread(logger::writethread,0,this);
}

void logger::writethread(void * a){
    ((logger *)a) -> logOneLine(); 
}

/*
void motorControl::motorControlLoop(void* a)
{
	((motorControl*)a)->controlLoop();
}
*/

void logger::update(double tick, double tock, int expProtocol){
    this->tick=tick;
    this->tock=tock;
    this->expProtocol=expProtocol;
}

/*

        timeNprotocol *parameters;
        parameters = (timeNprotocol *)malloc(sizeof(timeNprotocol));
        parameters->tick=tick;
        parameters->tock=tock;
        parameters->expProtocol=expProtocol;

        //_beginthreadx(0,0,&logger::logOneLine,parameters,0,0);

        */