DAQ.h
___________
#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>

dataOneSample.h
___________
#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>

dynamixel.h
___________


expParadigm.h
___________
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <ctime>
#include <dataLogger.h>
#include "FPGAControl.h"
#include <servoControl.h>

expParadigmManualPerturbation.h
___________
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <ctime>
#include "FPGAControl.h"

expParadigmMuscleLengthCalibration.h
___________
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
//#include <ctime>
//#include <dataLogger.h>
//#include "FPGAControl.h"
#include <servoControl.h>
#include <motorControl.h>

expParadigmServoPerturbation.h
___________
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <ctime>
#include <dataLogger.h>
#include "FPGAControl.h"
#include <servoControl.h>

expParadigmVoluntaryMovement.h
___________
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <ctime>
#include "FPGAControl.h"

FPGAControl.h
___________
//#include <pthread.h>
#include "motorControl.h"
#include "SomeFpga.h"

fpgaIO.hpp
___________
#include <iostream>
#include <stdbool.h>
#include "./include/utilities.h"
#ifdef _WIN64
	#include "./lib/FrontPanel/latest/nt64/okFrontPanelDLL.h"
#elif __linux__
	#include "./lib/FrontPanel/latest/ubuntu64/API/okFrontPanelDLL.h"
#endif

listman.h
___________
#include <stdio.h>
#include <stdlib.h>
#include <utilities.h>

logger.h
___________
#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include "listman.h"

motorControl.h
___________
#include <NIDAQmx.h>
#include <utilities.h>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <ctime>

okFrontPanelDLL.h
___________
#include <string>

parseConf.h
___________
#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <file.h>
#include "listman.h"

pipeIO.h
___________
#include "okFrontPanelDLL.h"
#include "utilities.h"

servoControl.h
___________
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include "dynamixel\import\dynamixel.h"

SomeFpga.h
___________
#include <iostream>
#include "utilities.h"
//#include <pthread.h>
#include <malloc.h>
#include "okFrontPanelDLL.h"

utilities.h
___________
#include <math.h>
#include <NIDAQmx.h>
#include <stdio.h>

dataOneSample.cpp
___________
#include "dataOneSample.h"

experimentUserInterface.cpp
___________
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <utilities.h>
#include <okFrontPanelDLL.h>
#include <servoControl.h>

expParadigm.cpp
___________
#include "expParadigm.h"
#include "FPGAControl.h"
#include <servoControl.h>

expParadigmManualPerturbation.cpp
___________
#include "expParadigmManualPerturbation.h"
#include "FPGAControl.h"
#include <conio.h>
#include <stdio.h>

expParadigmMuscleLengthCalibration.cpp
___________
#include "expParadigmMuscleLengthCalibration.h"
#include "motorControl.h"
#include <servoControl.h>
#include <utilities.h>
#include <NIDAQmx.h>

expParadigmServoPerturbation.cpp
___________
#include "expParadigmServoPerturbation.h"
#include "FPGAControl.h"
#include <servoControl.h>

expParadigmVoluntaryMovement.cpp
___________
#include "expParadigmVoluntaryMovement.h"
#include "FPGAControl.h"
#include <conio.h>
#include <stdio.h>
#include <utilities.h>

FPGAControl.cp
___________
#include "FPGAControl.h"
#include "SomeFpga.h"
//#include <pthread.h>
#include <malloc.h>
#include "utilities.h"
#include "NIDAQmx.h"
#include "motorControl.h"
#include <windows.h>
#include <process.h>
#include <math.h>

fpgaIO.cpp
___________
#include <iostream>
#include <stdbool.h>
#include "./include/utilities.h"
#include "./include/fpgaIO.hpp"
#ifdef _WIN64
        #include "./lib/FrontPanel/latest/nt64/okFrontPanelDLL.h"
#elif __linux__
        #include "./lib/FrontPanel/latest/ubuntu64/API/okFrontPanelDLL.h"
#endif

listman.cpp
___________
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include "listman.h"

main.cpp
___________
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <utilities.h>
#include <okFrontPanelDLL.h>
#include <servoControl.h>

motorControl.cpp
___________
#include "motorControl.h"
#include <utilities.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>

okFrontPanelDLL.cpp
___________
#include <stdio.h>
#include <stdlib.h>


#include "okFrontPanelDLL.h"

parseConf.cpp
___________
#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>
#include <file.h>
#include "listman.h"
#include <string.h>

pipeIO.cpp
___________
#include <iostream>
#include "pipeIO.h"
#include "okFrontPanelDLL.h"
#include "utilities.h"
#include <malloc.h>
#include <string.h>

servoControl.cpp
___________
#include "servoControl.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include "dynamixel\import\dynamixel.h"

SomeFpga.cpp
___________
#include "SomeFpga.h"
#include "utilities.h"
#include <assert.h>
#include <Windows.h>
#include "okFrontPanelDLL.h"

test.cpp
___________
#include <iostream>
#inlude "../lib/FrontPanel/latest/ubuntu64/API/okFrontPanelDLL.h"

utilities.cpp
___________
#include <utilities.h>
#include <stdio.h>
#include <conio.h>
#include <dataOneSample.h>
#include <motorControl.h>
#include <expParadigmMuscleLengthCalibration.h>
#include <expParadigmServoPerturbation.h>
#include <expParadigmManualPerturbation.h>
#include <expParadigmVoluntaryMovement.h>
#include <FPGAControl.h>