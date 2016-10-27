/*DAVID NUNO
COSC 3360
FUNDAMENTALS OF OPERATING SYSTEMS
*---------------------------------------------------------------------*
You are to simulate the execution of a stream of interactive
processes by a single-user system with a very large
memory, a multi-core processor and one hard drive.
*/

#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <iomanip>

using namespace std;

int numCores = 0;
int processIndex = -1;

struct processes {
	float startTime;										//STORE START TIME
	float coretime = 0;										//STORE TOTAL CORE TIME
	int processnumber;										//PROCESS NUMBER
	queue <string> processType;								//STORE PROCESS TYPE IN QUEUE
	queue <float> processTime;								//STORE PROCESS TIME IN QUEUE
	string status;											//STORE PROCESS STATUS
	bool started;											//CHECK IF PROCESS STARTED
	int processSize;										//CHECK PROCESS SIZE
};

struct coreDevice {
	int processnumber = 0;									//PROCESS NUMBER
	bool isCoreBusy = false;								//CHECK IF CORE BUSY
	float scheduledTime = 0;								//SCHEDULED TIME TO RELEASE CORE
};

struct deviceGeneral {
	int processnumber = 0;									//PROCESS NUMBER
	bool isDeviceBusy = false;								//CHECK IF DEVICE BUSY
	float deviceSchedule = 0;								//SCHEDULED TIME TO START DEVICE
	bool isNonBlock;										//CHECK IF BLOCKED
};
//simulate other devices

struct deviceQueue {			
	queue <int> processNum;									//STORE PROCESS NUMBER				
	queue <float> timeUsed;									//STORE TIME TO BE USED						
	queue <bool> isBlocking;								//CHECK IF BLOCKING						
};

void systemState(double clock, int busyCores, struct deviceQueue ready, struct deviceQueue disk);		//PRINT SYSTEM STATE


int main() {

	ifstream input("input10.txt");

	processes processArray[256];							//INITIALIZE PROCESS ARRAY

	string proc;											//STORE PROCEDURE FROM INPUT FILE
	float arg;												//STORE ARGUMENT FROM INPUT FILE
	int	integer;											//STORE CORE NUMBER


	if (input.fail())										//IF INPUT FILE FAILS
		cout << "ERROR: Input file failure!" << endl;

	
	while (input >> proc) {																				//WHILE LOOP TO READ INPUT FILE
		if (proc == "NCORES") {																			//IF NCORES
			input >> integer;											
			numCores = integer;																			//SET NUMBER OF CORES
		}//END IF NCORES
		//set process number and index for process array

		if (proc == "START") {																			//IF START
			processIndex++;																				//INCREMENT PROCESS INDEX BY ONE
			input >> arg;
			processArray[processIndex].startTime = arg;													//INITIALIZE START TIME
			processArray[processIndex].started = false;													//INITIALIZE STARTED TO FALSE
			processArray[processIndex].coretime = 0;													//INITIALIZE CORE TIME TO 0
			processArray[processIndex].processnumber = processIndex;									//SET PROCESS NUMBER
		}//END IF START

		if (proc == "CORE") {																			//IF CORE
			input >> arg;	
			processArray[processIndex].processType.push("CORE");										//PUSH INTO PROCESS TYPE QUEUE
			processArray[processIndex].processTime.push(arg);											//PUSH INTO PROCESS TIME QUEUE
			processArray[processIndex].processSize = processArray[processIndex].processSize + 1;		
		}//END IF CORE
		if (proc == "BLOCK") {																			//IF BLOCK
			input >> arg;
			processArray[processIndex].processType.push("BLOCK");										//PUSH INTO PROCESS TYPE QUEUE
			processArray[processIndex].processTime.push(arg);											//PUSH INTO PROCESS TIME QUEUE
			processArray[processIndex].processSize = processArray[processIndex].processSize + 1;
		}//END IF BLOCK
		if (proc == "NOBLOCK") {																		//IF NOBLOCK
			input >> arg;
			processArray[processIndex].processType.push("NOBLOCK");										//PUSH INTO PROCESS TYPE QUEUE
			processArray[processIndex].processTime.push(arg);											//PUSH INTO PROCESS TIME QUEUE
			processArray[processIndex].processSize = processArray[processIndex].processSize + 1;

		}//END IF NOBLOCK
		if (proc == "USER") {																			//IF USER
			input >> arg;
			processArray[processIndex].processType.push("USER");										//PUSH INTO PROCESS TYPE QUEUE
			processArray[processIndex].processTime.push(arg);											//PUSH IN PROCESS TIME QUEUE
			processArray[processIndex].processSize++;													//INCREMEMENT PROCESS SIZE BY 1
		}//END IF USER
	}//END WHILE LOOP

	int processTerminated = 0;																			//INITIALIZE PROCESSES TERMINATED TO 0
	float globalClock = 0.0;																			//INITIATE GLOBAL CLOCK TO 0

	deviceGeneral diskDevice;
	deviceGeneral userDevice;

	coreDevice core[265];																				//INITIALIZE CORE ARRAYS

	deviceQueue readyQueue;																				//READY QUEUE
	deviceQueue diskQueue;																				//DISK QUEUE
	deviceQueue userQueue;																				//USER QUEUE

	while (processTerminated < processIndex + 1) {														//WHILE LOOP TO INCREMENT GLOBAL CLOCK

		for (int i = 0; i < processIndex + 1; i++) {													//FOR LOOP TO INITIALIZE PROCESSES
			if (processArray[i].startTime <= globalClock && processArray[i].started != true) {
				processArray[i].started = true;

				if (processArray[i].processType.front() == "CORE") {									//IF CORE
					readyQueue.timeUsed.push(processArray[i].processTime.front());						//PUSH TIME USED INTO READY QUEUE
					readyQueue.processNum.push(i);														//PUSH PROCESS NUMBER INTO READY QUEUE
					processArray[i].status = "READY";													//CHANGE PROCESS STATUS TO READY
					processArray[i].processTime.pop();													//POP CORE PROCESS TIME
					processArray[i].processType.pop();													//POP CORE PROCESS TYPE
				}//END IF
			}//END IF
		}//END FOR

		 //START DEVICE

		 //CORE DEVICE

		for (int coreinx = 0; coreinx < numCores; coreinx++) {											//LOOP THROUGH

			if (core[coreinx].isCoreBusy != true && !readyQueue.processNum.empty()) {					//IF CORE NOT BUSY & READY QUEUE NONEMPTY
				core[coreinx].scheduledTime = readyQueue.timeUsed.front() + globalClock;				//SCHEDULE CORE RELEASE
				core[coreinx].processnumber = readyQueue.processNum.front();							//ENTER PROCESS NUMBER INTO CORE
				core[coreinx].isCoreBusy = true;														//CHANGE CORE STATUS TO TRUE
				processArray[readyQueue.processNum.front()].status = "RUNNING";							//CHANGE PROCESS STATUS TO RUNNING

				processArray[core[coreinx].processnumber].coretime = processArray[core[coreinx].processnumber].coretime + readyQueue.timeUsed.front();			//INCREMENT CORE TIME USED FOR PROCESS

				readyQueue.timeUsed.pop();																//POP PROCESS TIME USED FROM READY QUEUE
				readyQueue.processNum.pop();															//POP PROCESS TYPE FROM READY QUEUE
			}//END IF

			else if (core[coreinx].scheduledTime <= globalClock && core[coreinx].isCoreBusy == true) {	//IF
				int procnum = core[coreinx].processnumber;

				if (processArray[procnum].processType.empty() && processArray[procnum].status != "OFFLINE") {
					core[coreinx].isCoreBusy = false;
					processArray[procnum].status = "TERMINATED";										//CHANGE PROCESS STATUS 
					cout << "process " << procnum << " terminated" << endl;

				}//END IF

				else {
					if (processArray[procnum].processType.front() == "BLOCK") {							//IF BLOCK
						diskQueue.timeUsed.push(processArray[procnum].processTime.front());
						diskQueue.processNum.push(procnum);												//PUSH PROCESS INTO DISK QUEUE
						diskQueue.isBlocking.push(true);												//PUSH PROCESS BLOCK INTO DISK QUEUE

						processArray[procnum].processTime.pop();										//POP PROCESS TIME
						processArray[procnum].processType.pop();										//POP PROCESS TYPE

						processArray[procnum].status = "WAITING";

						core[coreinx].isCoreBusy = false;
						cout << "-Process number " << procnum << " at time:  " << globalClock << " from core " << coreinx << " from core to disk queue" << endl;

					}//END IF
					else if (processArray[procnum].processType.front() == "NOBLOCK") {					//ELSE IF NONBLOCK
						diskQueue.timeUsed.push(processArray[procnum].processTime.front());
						diskQueue.isBlocking.push(false);
						diskQueue.processNum.push(procnum);
						processArray[procnum].status = "WAITING";
						processArray[procnum].processTime.pop();										//POP PROCESS TIME
						processArray[procnum].processType.pop();										//POP PROCESS TYPE

						core[coreinx].isCoreBusy = false;
						cout << "-Process number " << procnum << " at time:  " << globalClock << " from core " << coreinx << " from core to disk queue" << endl;

					}//END ELSE IF
					else if (processArray[procnum].processType.front() == "USER") {						//ELSE IF USER

						userQueue.timeUsed.push(processArray[procnum].processTime.front());				//PUSH PROCESS TIME INTO USER QUEUE
						userQueue.processNum.push(procnum);												//PUSH PROCESS INTO USER QUEUE

						processArray[procnum].status = "WAITING";										//CHANGE PROCESS STATUS TO WAITING

						processArray[procnum].processTime.pop();										//POP PROCESS TIME
						processArray[procnum].processType.pop();										//POP PROCESS TYPE

						core[coreinx].isCoreBusy = false;

						cout << "-Process number " << procnum << " at time:  " << globalClock << " from core " << coreinx << " from core to user queue" << endl;

					}//END INNER ELSE IF
				}//END ELSE
			}//END OUTER ELSE IF
		}//END FOR LOOP

		if (diskDevice.isDeviceBusy != true && !diskQueue.timeUsed.empty()) {
			if (diskQueue.isBlocking.front() == true) {

				diskDevice.deviceSchedule = diskQueue.timeUsed.front() + globalClock;
				diskDevice.processnumber = diskQueue.processNum.front();
				diskDevice.isDeviceBusy = true;
				diskDevice.isNonBlock = false;
				processArray[diskQueue.processNum.front()].status = "WAITING";

				cout << "-Process number " << diskDevice.processnumber << " at time:  " << globalClock << " from disk queue to disk as blocking " << endl;
				//cout << core[coreinx].scheduledTime << endl;
				diskQueue.isBlocking.pop();
				diskQueue.timeUsed.pop();
				diskQueue.processNum.pop();
			}//END IF
			else if (diskQueue.isBlocking.front() == false) {
				diskDevice.deviceSchedule = diskQueue.timeUsed.front() + globalClock;
				diskDevice.processnumber = diskQueue.processNum.front();
				diskDevice.isDeviceBusy = true;
				diskDevice.isNonBlock = true;
				processArray[diskQueue.processNum.front()].status = "READY";

				cout << "-Process number " << diskDevice.processnumber << " at time:  " << globalClock << " from disk queue to disk as nonblocking " << endl;
				diskQueue.isBlocking.pop();
				diskQueue.timeUsed.pop();
				diskQueue.processNum.pop();

				readyQueue.timeUsed.push(processArray[diskDevice.processnumber].processTime.front());
				readyQueue.processNum.push(diskDevice.processnumber);

				processArray[diskDevice.processnumber].processTime.pop();
				processArray[diskDevice.processnumber].processType.pop();
			}//END ELSE IF
		}//END IF

		if (diskDevice.deviceSchedule <= globalClock && diskDevice.isDeviceBusy == true) {
			if (diskDevice.isNonBlock == true) {
				cout << "-Process number " << diskDevice.processnumber << " at time:  " << globalClock << " release non-blocking process " << endl;
				diskDevice.isDeviceBusy = false;
			}//END IF
			else if (diskDevice.isNonBlock == false) {
				diskDevice.isDeviceBusy = false;

				cout << "-Process number " << diskDevice.processnumber << " at time:  " << globalClock << " release blocking disk process from disk to core " << endl;
				readyQueue.timeUsed.push(processArray[diskDevice.processnumber].processTime.front());
				readyQueue.processNum.push(diskDevice.processnumber);

				processArray[diskDevice.processnumber].processTime.pop();									//POP PROCESS TIME USED
				processArray[diskDevice.processnumber].processType.pop();									//POP PROCESS TYPE USED
			}//END ELSE IF
		}//END IF

		if (userDevice.isDeviceBusy != true && !userQueue.timeUsed.empty()) {								//IF
			userDevice.deviceSchedule = userQueue.timeUsed.front() + globalClock;
			userDevice.processnumber = userQueue.processNum.front();
			userDevice.isDeviceBusy = true;

			processArray[userQueue.processNum.front()].status = "WAITING";									//CHANGE PROCESS STATUS TO WAITING

			cout << "-Process number " << userDevice.processnumber << " at time:  " << globalClock << " from user queue to user device " << endl;

			userQueue.timeUsed.pop();
			userQueue.processNum.pop();
		}//END FOR

		if (userDevice.deviceSchedule <= globalClock && userDevice.isDeviceBusy == true) {				
			userDevice.isDeviceBusy = false;

			cout << "-Process " << userDevice.processnumber << " at time:  " << globalClock << " release user device to core " << endl;
			readyQueue.timeUsed.push(processArray[userDevice.processnumber].processTime.front());
			readyQueue.processNum.push(userDevice.processnumber);

			processArray[userDevice.processnumber].processTime.pop();
			processArray[userDevice.processnumber].processType.pop();
		}//END IF

		for (int j = 0; j < processIndex + 1; j++) {														//FOR LOOP TO CHECK PROCESSES TERMINATED
			if (processArray[j].status == "TERMINATED") {													//IF PROCESS TERMINATED
				processTerminated++;																		//INCREMENT NUMBER OF PROCESSES TERMINATED

				int usedCores = 0;																			//INITIALIZE NUMBER OF USED CORES TO 0

				for (int corenum = 0; corenum < numCores; corenum++)										//FOR LOOP TO CHECK BUSY CORES
					if (core[corenum].isCoreBusy == true)													//IF CORE BUSY
						usedCores++;																		//INCREMENT USED CORES BY 1

				cout << "PROCESS " << j << " terminates at time " << globalClock << endl << endl;
				systemState(globalClock, usedCores, readyQueue, diskQueue);

				//-----------------------START PROCESS STATE TABLE-------------------------------//
				cout << "ProcessID" << setw(15) << "start time" << setw(10) << "core time" << setw(15) << "status" << endl;

				for (int k = 0; k < processIndex + 1; k++) {
					if (processArray[k].status != "OFFLINE") {
						cout << k << setw(20) << processArray[k].startTime << setw(10) << processArray[k].coretime;
						if (processArray[k].status == "READY")  cout << " " << setw(18) << "ready" << endl; 
						if (processArray[k].status == "WAITING")  cout << " " << setw(18) << "waiting" << endl; 
						if (processArray[k].status == "RUNNING")  cout << " " << setw(18) << "running" << endl; 
						if (processArray[k].status == "TERMINATED")  cout << " " << setw(18) << "terminated" << endl; 
					}
				}//END FOR LOOP
				 //-----------------------END PROCESS STATE TABLE-------------------------------//

				//------------------------START PROCESS TABLE-------------------------------//

				cout << "PROCESS TABLE:\n";
				cout << setfill('-') << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << endl;
				//PRINTING HEADER
				cout << setfill(' ') << setw(1) << "|" << setw(15) << left << " PROCESS ID" << setw(1) << "|" << setw(15) << left << " START TIME" << setw(1) << "|" << setw(15) << left << " CORE TIME" << setfill(' ') << setw(1) << "|" << setw(15) << left << " STATUS" << setw(1) << "|" << endl;
				//PRINTING BORDER
				cout << setfill('-') << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << endl;

				for (int m = 0; m < processIndex; m++) {
					//PRINTING BORDER
					cout << setfill(' ') << setw(1) << "| " << setw(14) << m << setw(1) << "| " << setw(14) << left << processArray[m].startTime << setw(1) << "| " << setw(14) << left << processArray[m].coretime << setw(1) << "| " << setw(14) << left << processArray[m].status << setw(1) << "|" << endl;
					//PRINTING BORDER
					cout << setfill('-') << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << endl;
				}//end for loop

				 //-------------------------END PROCESS TABLE------------------------------//
				processArray[j].status = "OFFLINE";															//CHANGE STATUS OF PROCESS TO OFFLINE
			}//END IF
		}//END FOR LOOP
		globalClock += 0.1;																					//INCREMEMENT THE GLOBAL CLOCK BY .1 FOR EVERY LOOP
	}//END WHILE LOOP

	system("pause");																						//PAUSE SYSTEM

	return 0;
}//end main


void systemState(double clock, int busyCores, struct deviceQueue ready, struct deviceQueue disk) {						//PRINT SYSTEM STATE
	cout << "CURRENT STATE OF THE SYSTEM AT TIME " << clock << ":" << endl;
	cout << "Current number of busy cores: " << busyCores << endl;

	cout << "READY QUEUE:\n";

	if (ready.processNum.empty())
		cout << "empty\n";

	else while (!ready.processNum.empty()) {
		cout << "Process " << ready.processNum.front();
		ready.processNum.front();
		ready.processNum.pop();
		cout << endl;
	}

	cout << "DISK QUEUE:\n";

	if (disk.processNum.empty())
		cout << "empty\n";
	else while (!disk.processNum.empty()) {
		cout << "Process " << ready.processNum.front();
		disk.processNum.front();
		disk.processNum.pop();
		cout << endl;
	}
}//end systemState
