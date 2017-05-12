#include "hardwareConfiguration.h"
#include <iostream>

hardwareConfiguration::hardwareConfiguration()
{
    FILE *configFile;
    char *header[200];
    configFile = fopen("hardwareConfigration.csv","r");
    if (configFile == NULL) {
        printf("Could not open the configuration file.\n");
        isConfigured = false;
    }
    else{
        numberOfActiveMuscles = 0;
        fscanf(configFile,"%s\n",&header);
        fscanf(configFile,"%d\n",&numberOfActiveMuscles);
        activeMuscleNumbers = new int[numberOfActiveMuscles];

        for(int i = 0; i<numberOfActiveMuscles;i++)
        {
            fscanf(configFile,"%d,",&activeMuscleNumbers[i]);
        }
        fscanf(configFile,"\n,");
        fclose(configFile);
        isConfigured = true;
        printf("Configuration is successfully loaded.\n");
    }
}
hardwareConfiguration::~hardwareConfiguration()
{
}
int hardwareConfiguration::getMuscleNumbers(int * activeMuscleNumbers)
{
    if (isConfigured)
    {
        for(int i = 0; i<numberOfActiveMuscles;i++)
        {
            activeMuscleNumbers[i] = this->activeMuscleNumbers[i];
        }
        
        return 1;
    }
    else
    {
        printf("Error: Hardware configuration is not set up properly");
        return 0;
    }
}

int hardwareConfiguration::getNumberOfMuscles(int number)
{
    if (isConfigured)
    {
        number = numberOfActiveMuscles;
        return 1;
    }
    else
    {
        printf("Error: Hardware configuration is not set up properly");
        return 0;
    }
}