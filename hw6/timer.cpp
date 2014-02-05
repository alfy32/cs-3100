/*	Alan Christensen
	A01072246
	Nov-02-2012
	CS 3100
*/

#include <iostream>
#include <sys/time.h>
#include <cmath>
#include <deque>
#include <cstdlib>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

using namespace std;

const int ARRAY_SIZE = 10000000; //for testing quick things do it this many times and divide
const int TD_ARRAY_SIZE = 1000000;
const int NUM_TESTS = 100;	//number of times to test so I can throw out bad results

//semaphore consts
const int SEM_KEY = 2246;
const short ADD_KEY = 1;
const short WAIT_KEY = -1;
const int NUM_KEYS = 10000;

//prototypes
double findMean(deque<long> &values);
double stdDeviation(deque<long> &values);
void memoryTest();
void timeOfDayTest();
void forkTest();
void *threadStart(void* arg);
void pthread_startTest();
void semaphoreWaitTest();
void semaphoreSignalTest();

//Tests the time it takes to do OS tasks
int main()
{
	memoryTest();
	
	timeOfDayTest();
	
	forkTest();
	
	pthread_startTest();
	
	semaphoreWaitTest();
	
	semaphoreSignalTest();

	return 0;
}

//used to find the mean of a collection of long values stored in a deque
double findMean(deque<long> &values)
{
	//get the total
	long total = 0;
	
	for(int i = 0; i < values.size(); i++)
	{
		total += values[i];
	}
	
	//calculate mean
	return total/values.size();
}

//calculates the standard deviation and throws out values more than one 
// deviation from the mean
double stdDeviation(deque<long> &values)
{	
	//calculate mean
	double mean = findMean(values);
	
	//total (number-mean)^2
	double meanTotal = 0;
	for(int i = 0; i < values.size(); i++)
	{
		meanTotal += (values[i]-mean) * (values[i]-mean);
	}
	
	//calculate the average of the means squared and take the square root
	double stdDev = sqrt(meanTotal/values.size());
	
	for(int i = 0; i < values.size(); i++)
	{
		if(abs(values[i] - mean) > stdDev)
		{
			values.erase(values.begin()+i);
			//output a start for each of results thrown out
			cout << "*";
		}
	}
}

//tests the memory access time by initailizing a large array to 10
void memoryTest()
{
	timeval startTime;
	timeval endTime;
	
	char A[ARRAY_SIZE];
	
	cout << "Memory access test..................." << endl;

	//
	// Warm up the cache first
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		A[i] = 10;
	}

	//
	//Do full test by unrolling to 20
	deque<long> results;
	
	for(int test = 0; test < NUM_TESTS; test++)
	{
		gettimeofday(&startTime, NULL);

		for (int i = 0; i < ARRAY_SIZE; i+=20)
		{
			A[i + 0] = 1;
			A[i + 1] = 1;
			A[i + 2] = 1;
			A[i + 3] = 1;
			A[i + 4] = 1;
			A[i + 5] = 1;
			A[i + 6] = 1;
			A[i + 7] = 1;
			A[i + 8] = 1;
			A[i + 9] = 1;
			A[i + 10] = 1;
			A[i + 11] = 1;
			A[i + 12] = 1;
			A[i + 13] = 1;
			A[i + 14] = 1;
			A[i + 15] = 1;
			A[i + 16] = 1;
			A[i + 17] = 1;
			A[i + 18] = 1;
			A[i + 19] = 1;
		}

		gettimeofday(&endTime,NULL);
		
		results.push_back(endTime.tv_usec - startTime.tv_usec + 
						  (endTime.tv_sec - startTime.tv_sec) * 1000000);
	}
	
	//find the standard deviation and drop result outside of that
	stdDeviation(results);
	
	//calculate the time in microseconds for one memory access
	double time = findMean(results)/ARRAY_SIZE;
	
	cout << "Results: " << time*1000 << " nano-seconds" << endl;
}

//tests the time it takes to find the time of day
void timeOfDayTest()
{
	cout << "Testing timeofday..........." << endl;
	
	timeval startTime,
			endTime, 
			tempTime;
	
	deque<long> results;
	
	//run the test many times
	for(int test = 0; test < NUM_TESTS; test++)
	{
		gettimeofday(&startTime, NULL);
		
		//use gettimeofday a lot of times
		for(int i = 0; i < TD_ARRAY_SIZE; i++)
		{
			gettimeofday(&tempTime, NULL);
		}

		gettimeofday(&endTime,NULL);
		
		results.push_back(endTime.tv_usec - startTime.tv_usec + 
						  (endTime.tv_sec - startTime.tv_sec) * 1000000);
	}
	
	//find the standard deviation and drop result outside of that
	stdDeviation(results);
	
	//time in microseconds
	double microseconds = findMean(results)/TD_ARRAY_SIZE;
	
	cout << "Time: " << microseconds*1000 << " nano-seconds" << endl;
}

//tests the time required to do the fork operation
void forkTest()
{
	cout << "Testing fork..........." << endl;
	
	timeval startTime,
			endTime, 
			tempTime;
	
	deque<long> results;
	
	//run the test many times
	for(int test = 0; test < NUM_TESTS; test++)
	{
		gettimeofday(&startTime, NULL);
		
		pid_t pid = fork();
		//parent process
		if(pid > 0)
		{
			//get the time when the process is back and running in the parent.
			gettimeofday(&endTime,NULL);
		}
		//child process
		else if(pid == 0)
		{
				//just quit
				exit(0);
				//we just want to start it so we can time the starting of it 
		}
		
		results.push_back(endTime.tv_usec - startTime.tv_usec + 
						  (endTime.tv_sec - startTime.tv_sec) * 1000000);
	}
	
	//find the standard deviation and drop results outside of that
	stdDeviation(results);
	
	//time in microseconds
	double microseconds = findMean(results);
	
	cout << "Time: " << microseconds << " micro-seconds" << endl;
}

//the function that runs when the thread is created
void *threadStart(void* arg)
{
	timeval* endTime = static_cast<timeval*>(arg);
	
	gettimeofday(endTime, NULL);
	
	pthread_exit(0);
}

//tests time to start a new pthread
void pthread_startTest()
{
	cout << "Testing pthread_start..........." << endl;
	
	timeval startTime,
			endTime;
	
	deque<long> results;
	
	//run the test many times
	for(int test = 0; test < NUM_TESTS; test++)
	{
		pthread_t tid;
		
		gettimeofday(&startTime, NULL);
		
		//create the thread and passs in the endTime structure as an agrument
		pthread_create(&tid, NULL, threadStart, static_cast<void*>(&endTime));
		
		//wait for the thread to finish
		pthread_join(tid, NULL);
		
		//gettimeofday(&endTime,NULL);
		
		
		results.push_back(endTime.tv_usec - startTime.tv_usec + 
						  (endTime.tv_sec - startTime.tv_sec) * 1000000);
	}
	
	//find the standard deviation and drop results outside of that
	stdDeviation(results);
	
	//time in microseconds
	double microseconds = findMean(results);
	
	cout << "Time: " << microseconds << " micro-seconds" << endl;
}

void semaphoreWaitTest()
{
	cout << "Testing semaphore wait..........." << endl;
	
	timeval startTime,
			endTime;
	
	deque<long> results;
	
	//set up the semaphore
	int semID = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
	
	sembuf buf;
	buf.sem_num = 0;
	buf.sem_flg = 0;
	buf.sem_op = WAIT_KEY;
	
	//run the test many times
	for(int test = 0; test < NUM_TESTS; test++)
	{	
		semctl(semID, 0, SETVAL, NUM_KEYS);	
		
		gettimeofday(&startTime, NULL);
		
		//loop it a lot of times because it take less than microseconds
		for(int i = 0; i < NUM_KEYS; i++)
		{
			semop(semID, &buf, 1);
		}
		
		gettimeofday(&endTime,NULL);
		
		results.push_back(endTime.tv_usec - startTime.tv_usec + 
						  (endTime.tv_sec - startTime.tv_sec) * 1000000);
	}
	
	//find the standard deviation and drop results outside of that
	stdDeviation(results);
	
	//time in microseconds
	double microseconds = findMean(results)/NUM_KEYS;
	
	//*1000 for micro to nano conversion
	cout << "Time: " << microseconds*1000 << " nano-seconds" << endl;
}

void semaphoreSignalTest()
{
	cout << "Testing semaphore signal..........." << endl;
	
	timeval startTime,
			endTime;
	
	deque<long> results;
	
	//set up the semaphore
	int semID = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
	semctl(semID, 0, SETVAL, 0);
	
	sembuf buf;
	buf.sem_num = 0;
	buf.sem_flg = 0;
	buf.sem_op = ADD_KEY;
	
	//run the test many times
	for(int test = 0; test < NUM_TESTS; test++)
	{
		gettimeofday(&startTime, NULL);
		
		//loop it a lot of times because it take less than microseconds
		for(int i = 0; i < NUM_KEYS; i++)
		{
			semop(semID, &buf, 1);
		}
		
		gettimeofday(&endTime,NULL);
		
		
		results.push_back(endTime.tv_usec - startTime.tv_usec + 
						  (endTime.tv_sec - startTime.tv_sec) * 1000000);
	}
	
	//find the standard deviation and drop results outside of that
	stdDeviation(results);

	//time in microseconds
	double microseconds = findMean(results)/NUM_KEYS;

	//*1000 for micro to nano conversion
	cout << "Time: " << microseconds*1000 << " nano-seconds" << endl;

}
