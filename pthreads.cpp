/*
 DAVID NUNO
 COSC 3360
 You are to simulate the behavior of customers in a post office. Each of your customers will be
 simulated by a separate thread created by your main program. Arriving customers will do a P()
 operation on the semaphore representing the number of clerks working in the post office and
 release that semaphore by doing a V() operation on that semaphore when they are done.
 Your post office should have a single FIFO queue that you will represent by a single semaphore.
 */

#include <queue>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

static sem_t postOffice;													//INITIALIZE POST OFFICE SEMAPHORE.

int waitingCustomers = 0;													//INITIALIZE WAITING CUSTOMERS.

struct Customer {															//CUSTOMER STRUCT.
int customerNum;															//STORE CUSTOMER NUMBER.
int arrivalTime;															//STORE ARRIVAL TIME.
int serviceTime;															//STORE SERVICE TIME.
};

void *getService(void *arg) {

	int testNum;
	sem_getvalue(&postOffice, &testNum);
	if(testNum == 0)														//IF POST OFFICE CLERKS 0
		waitingCustomers++;													//INCREMENT WAITING CUSTOMERS.

	struct Customer *argptr;												//CREATE STRUCT POINTER.
	argptr = (struct Customer*) arg;										//SET EQUAL TO PARAMETER.

	sleep(argptr->arrivalTime);												//SLEEP UNTIL SCHEDULED ARRIVAL TIME.

	cout << "Customer " << argptr->customerNum << " arrives at post office!" << endl;
	sem_wait(&postOffice);													//DECREMENT CLERK.

	cout << "Customer " << argptr->customerNum << " gets service!" << endl;

	sleep(argptr->serviceTime);												//BEGIN SERVICE.
	cout << "Customer " << argptr->customerNum << " leaves the post office!\n";
	sem_post(&postOffice);													//INCREMENT AVAILABLE CLERK.

	pthread_exit((void*) 0);

}//END GET SERVICE FUNCTION

int main (int argc, char *argv[]) {
	unsigned int numberOfClerks = 0;										//INITIALIZE NUMBER OF CLERKS TO 0
	int tempNumber;															//DECLARE MISCELLANEOUS TEMP NUMBER 
	int numberOfCustomers = 0;
	int dummy;																//DUMMY INTEGER
	queue <Customer> customers;

	/*----------------------
	BEGIN STORING FROM INPUT
	-----------------------*/

	string firstline;

	getline(cin,firstline);

	istringstream postOfficeInfo(firstline);

	postOfficeInfo >> numberOfClerks;										//GET THE FIRST LINE OF THE INPUT (NUMBER OF CLERKS)

	string input;

	while (getline(cin, input)) {											//WHILE LOOP FOR INPUT TEXT FILE.

		istringstream  lineInput(input);									//STORE LINE.
		
		int tempid;															//TEMPORARY VARIABLES.
		int tempArrival;
		int tempService;

		lineInput >> tempid >> tempArrival >> tempService;					//INPUT FROM TEXT FILE.

		dummy += tempArrival;												//USE DUMMY INTEGER TO ASSIGN SCHEDULED ARRIVAL TIMES.

		Customer tempcust;													//DECLARE TEMP CUSTOMER.
		tempcust.arrivalTime = dummy;										//ASSIGN ARRIVAL TIME.
		tempcust.customerNum = tempid;										//ASSIGN CUSTOMER NUMBER.
		tempcust.serviceTime = tempService;									//ASSIGN SERVICE TIME.

		customers.push(tempcust);											//PUSH CUSTOMERS INTO QUEUE.

		numberOfCustomers++;												//INCREMENT NUMBER OF CUSTOMERS BY 1.

	}//END WHILE LOOP

	/*-------------------
	 INITIALIZE SEMAPHORES
	 -------------------*/
	cout << "Create postOffice semaphore to initialize at: " << numberOfClerks << endl;

	   if (sem_init(&postOffice, 0, numberOfClerks) == -1) {				//IF ERROR INITIALIZING POST OFFICE SEMAPHORE.
       perror("Unable to create post office semaphore\n");					//PROMPT USER ERROR.
       exit(1);																//EXIT PROGRAM.
       }//END IF

	sem_getvalue(&postOffice, &tempNumber);									//GET SEMAPHORE VALUE.
	cout << "The initial value of post office is:  " << tempNumber << endl;


 	cout << "Creating thread for each customer.\n";
	int index = 0;															//INITIALIZE INDEX TO LOOP THROUGH THREADS.
	pthread_t threads[100];													//DECLARE ARRAY OF THREADS.

	/*------------------------------------
	 WHILE LOOP THROUGH QUEUE OF CUSTOMERS
	 -----------------------------------*/

	while (!customers.empty()) {											//WHILE LOOP THROUGH QUEUE.

		pthread_create(&threads[index], NULL, getService, (void *) &customers.front());	//CREATE NEW THREAD FOR NEW CUSTOMER.

 		index++;															//INCREMENT INDEX BY 1.

 		customers.pop();													//POP CUSTOMER FROM QUEUE.
}//END WHILE LOOP

	for(int i = 0;i < index; i++)
		pthread_join(threads[i], NULL);										//WAIT FOR THREADS TO FINISH EXECUTING.

	/*-----------------
	 OUTPUT INFORMATION
	 ----------------*/
	cout << "\nTotal customers helped: " << numberOfCustomers << endl;
	cout << "Customers that did not wait: " << numberOfCustomers - waitingCustomers << endl;
	cout << "Customers that waited: " << waitingCustomers << endl;

}//END MAIN