frequencyMin = 1;
frequencyMax = 1;
frequencyResolution = 1;
frequency = frequencyMin : frequencyResolution : frequencyMax;

cortexAmpMin = 1000;
cortexAmpMax = 1000;
cortexAmpResolution = 1;
cortexAmp = cortexAmpMin : cortexAmpResolution : cortexAmpMax;

gammaStaticAmpMin = 0;
gammaStaticAmpMax = 150;
gammaStaticAmpResolution = 50;
gammaStaticAmp = gammaStaticAmpMin : gammaStaticAmpResolution : gammaStaticAmpMax;

gammaDynamicAmpMin = 0;
gammaDynamicAmpMax = 150;
gammaDynamicAmpResolution = 50;
gammaDynamicAmp = gammaDynamicAmpMin : gammaDynamicAmpResolution : gammaDynamicAmpMax;

gammaStaticPhaseMin = -pi/2;
gammaStaticPhaseMax = pi/4;
gammaStaticPhaseResolution = pi/8;
gammaStaticPhase = gammaStaticPhaseMin : gammaStaticPhaseResolution : gammaStaticPhaseMax;

gammaDynamicPhaseMin = -pi/2;
gammaDynamicPhaseMax = pi/4;
gammaDynamicPhaseResolution = pi/8;
gammaDynamicPhase = gammaDynamicPhaseMin : gammaDynamicPhaseResolution : gammaDynamicPhaseMax;
experiment = [];
for frequencyIndex = 1 : length(frequency)
    frequencyThisTrial = frequency(frequencyIndex);
    for cortexAmpIndex = 1 : length(cortexAmp)
        cortexAmpThisTrial = cortexAmp(cortexAmpIndex);
        for gammaStaticAmpIndex = 1 : length(gammaStaticAmp)
            gammaStaticAmpThisTrial = gammaStaticAmp(gammaStaticAmpIndex);
            for gammaDynamicAmpIndex = 1 : length(gammaDynamicAmp)
                gammaDynamicAmpThisTrial = gammaDynamicAmp(gammaDynamicAmpIndex);
                for gammaStaticPhaseIndex = 1 : length(gammaStaticPhase)
                    gammaStaticPhaseThisTrial = gammaStaticPhase(gammaStaticPhaseIndex);
                    for gammaDynamicPhaseIndex = 1 : length(gammaDynamicPhase)
                        gammaDynamicPhaseThisTrial = gammaDynamicPhase(gammaDynamicPhaseIndex);
                        thisTrial = [frequencyThisTrial cortexAmpThisTrial gammaStaticAmpThisTrial gammaStaticPhaseThisTrial gammaDynamicAmpThisTrial gammaDynamicPhaseThisTrial 5];
                        experiment = [experiment; thisTrial];
                    end
                end
            end
        end
    end
end
%%
trialsPerFile = 100;
numTrials = size(experiment,1);
for i = 1 : floor(numTrials/trialsPerFile)
    thisExperiment = (experiment((i - 1) * trialsPerFile + 1: i * trialsPerFile,:));
    dlmwrite(['voluntaryTest',num2str(i),'.txt'],thisExperiment);
end
thisExperiment = (experiment(floor(numTrials/trialsPerFile) * trialsPerFile + 1 : end,:));
dlmwrite(['voluntaryTest',num2str(i),'.txt'],thisExperiment);
dlmwrite('voluntaryTestAll.txt',experiment);
