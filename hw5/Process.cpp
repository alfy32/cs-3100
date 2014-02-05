#include "Process.h"

//The constructor sets up the shared memory and points to it with the counter pointer
Process::Process()
{
	unsigned long sharedID = shmget(SHARED_LOCATION, 4, 0666 | IPC_CREAT);
	
	_counter = static_cast<int*>(shmat(sharedID, 0, 0));
}

//This will start the producer process
void Process::runProducer()
{
	cout << "Process producer Starting..." << endl;
	
	//initailize counter
	*_counter = 0;
	
	//setup the semaphore
	int semID = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
	semctl(semID, 0, SETVAL, 1);
	
	bool interleave = false;
	
	int timer = 0;
	
	while(*_counter < 4000000000)
	{
		if(!interleave)
		{
			int temp = *_counter;
			
			(*_counter)++;
			
			if(*_counter != temp+1)
			{
				cout << "Process producer detected a problem.  Expected " << temp+1 << ", actual value was " << *_counter << endl;
				
				interleave = true;
				
				cout << "Process producer acting to protect against interleave." << endl;
			}
		}
		else
		{
			if(timer > 500000)
			{
				*_counter = -1;
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
			
			(*_counter)++;
	
			//return key
			buf.sem_op = ADD_KEY;
			semop(semID, &buf, 1);
		}
	}
	
	cout << "Process producer shutting down." << endl;
}

//The consumer will decrement the counter
void Process::runConsumer()
{
	cout << "Process consumer Starting..." << endl;
	
	//setup the semaphore
	int semID = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
	
	bool interleave = false;
	
	while(*_counter > 0)
	{
		//before interleave error is detected
		if(!interleave)
		{
			int temp = *_counter;
			
			(*_counter)++;
			
			if(*_counter != temp+1)
			{
				cout << "Process consumer detected a problem.  Expected " << temp+1 << ", actual value was " << *_counter << endl;
				
				interleave = true;
				
				cout << "Process consumer acting to protect against interleave." << endl;
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
			
			(*_counter)--;
	
			//return key
			buf.sem_op = ADD_KEY;
			semop(semID, &buf, 1);
		}
	}
		
	cout << "Process consumer shutting down." << endl;
}