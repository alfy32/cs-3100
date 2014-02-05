/*	Alan Christensen
	10/24/2012
	CS3100
*/
#include <vector>
#include <string>
#include <iostream>

#include "Process.h"
#include "Threaded.h"

using namespace std;

//This demos the failure of interleaving. It will have two process or threads produce and consume by incrementing and decrementing a counter.
int main(int argc, char* argv[])
{
	vector<string> args;
	
	for(int i = 0; i < argc; i++)
		args.push_back(argv[i]);
	
	//see if there are enough arguments given for a process command.
	// requires the following input:
	//	'./interleave process producer' This will start it as the producer.  Can safely assume the producer will always start first.
	//	'./interleave process consumer' This will start it as the consumer.  Can safely assume the consumer will always start second.		
	if(args.size() > 2 && args[1] == "process")
	{	
		Process process;
				
		if(args[2] == "producer")
		{
			process.runProducer();
		}	
		else if(args[2] == "consumer")
		{
			process.runConsumer();
		}
		else
			cout << "Incorrect arguments given." << endl;
	}
	//For the threaded demo
	//	'./interleave threaded' will start it for the threaded demonstration.
	else if(args.size() > 1 && args[1] == "threaded")
	{
		Threaded *threaded = new Threaded;
		
		threaded->runProducer();
		
		sleep(1);
		
		threaded->runConsumer();
		
		threaded->wait();
	}
	else
		cout << "The arguments given are not handled by this program." << endl;
		
	return 0;
}