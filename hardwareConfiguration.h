#ifndef HARD_CONF_H
#define HARD_CONF_H

class hardwareConfiguration
{
    bool isConfigured;
    int numberOfActiveMuscles;
    int *activeMuscleNumbers;
public:
    hardwareConfiguration(void);	
    ~hardwareConfiguration(void);
    int getMuscleNumbers(int *);
    int getNumberOfMuscles(int);
};
#endif