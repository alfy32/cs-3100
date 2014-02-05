#include <iostream>
#include "Threaded.h"

using namespace std;

// The constructor initializes the shared memory
Threaded::Threaded()
{
	semID = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
}

//Creates the thread and calls the threadStart function
void Threaded::runProducer()
{
	pthread_create(&m_tid, NULL, threadStartProducer, static_cast<void*>(this));
}

//Creates the thread and calls the threadStart function
void Threaded::runConsumer()
{
	pthread_create(&m_tid, NULL, threadStartConsumer, static_cast<void*>(this));
}

//Trick to make the class nice
void* Threaded::threadStartProducer(void* arg)
{
	Threaded* myClass = static_cast<Threaded*>(arg);
	myClass->internalRunProducer();
	myClass->cleanup();
}

//Trick to make the class nice
void* Threaded::threadStartConsumer(void* arg)
{
	Threaded* myClass = static_cast<Threaded*>(arg);
	myClass->internalRunConsumer();
	myClass->cleanup();
}

//Close the threads
void Threaded::cleanup()
{
	pthread_exit(0);
}

//The producer increments the counter
void Threaded::internalRunProducer()
{
	cout << "Threaded producer Starting..." << endl;
	
	//initailize counter
	counter = 0;
	
	//setup the semaphore
	semctl(semID, 0, SETVAL, 1);
	
	bool interleave = false;
	
	int timer = 0;
	
	while(counter < 4000000000)
	{
		if(!interleave)
		{
			int temp = counter;
			
			(counter)++;
			
			if(counter != temp+1)
			{
				cout << "Threaded producer detected a problem.  Expected " << temp+1 << ", actual value was " << counter << endl;
				
				interleave = true;
				
				cout << "Threaded producer acting to protect against interleave." << endl;
			}
		}
		else
		{
			if(timer > 500000)
			{
				counter = -1;
				break;
			}
			timer++;
			
			//initialize semaphore buffer
			sembuf buf;
			buf.sem_num = 0;
			buf.sem_flg = 0;
			
			//wait for a key
			buf.sem_op = WAIT_KEY;
			semop(semID, &buf, 1);
			
			counter++;
	
			//return key
			buf.sem_op = ADD_KEY;
			semop(semID, &buf, 1);
		}
	}
	
	cout << "Threaded producer shutting down." << endl;
}

//The consumer decrements the counter
void Threaded::internalRunConsumer()
{
	cout << "Threaded consumer Starting..." << endl;
	
	bool interleave = false;
	
	while(counter > 0)
	{
		//before interleave error is detected
		if(!interleave)
		{
			int temp = counter;
			
			counter++;
			
			if(counter != temp+1)
			{
				cout << "Threaded consumer detected a problem.  Expected " << temp+1 << ", actual value was " << counter << endl;
				
				interleave = true;
				
				cout << "Threaded consumer acting to protect against interleave." << endl;
			}
		}
		//after interleave error is detected
		else
		{
			//initialize semaphore buffer
			sembuf buf;
			buf.sem_num = 0;
			buf.sem_flg = 0;
			
			//wait for a key
			buf.sem_op = WAIT_KEY;
			semop(semID, &buf, 1);
			
			counter--;
	
			//return key
			buf.sem_op = ADD_KEY;
			semop(semID, &buf, 1);
		}
	}
		
	cout << "Threaded consumer shutting down." << endl;
}
