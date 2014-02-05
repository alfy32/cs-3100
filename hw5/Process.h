#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

using namespace std;

#ifndef _MY_CONSTS_
#define _MY_CONSTS_
const int SHARED_LOCATION = 2246;
const int SEM_KEY = 2246;
const int ADD_KEY = 1;
const int WAIT_KEY = -1;
#endif

class Process
{
private:
	int* _counter;
	
public:
	Process();
	
	void runProducer();
	void runConsumer();
};

#endif