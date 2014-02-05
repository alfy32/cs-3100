#ifndef THREADED_H
#define THREADED_H

#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#ifndef _MY_CONSTS_
#define _MY_CONSTS_
const int SEM_KEY = 2246;
const int ADD_KEY = 1;
const int WAIT_KEY = -1;
#endif

class Threaded
{
public:
	Threaded();

	void runProducer();
	void runConsumer();
	void wait()		{ pthread_join(m_tid, NULL); }

protected:
	pthread_t m_tid;
	int counter;
	int semID;

	static void* threadStartProducer(void* arg);
	static void* threadStartConsumer(void* arg);
	void cleanup();
	virtual void internalRunProducer();
	virtual void internalRunConsumer();
};

#endif
