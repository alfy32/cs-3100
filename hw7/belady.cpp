#include <iostream>
#include <vector>
#include <queue>
#include <ctime>
#include <cstdlib> 

using namespace std;

const int REF_SIZE = 1000;
const int MAX_FRAMES = 100;
const int NUM_SEQ = 100;

void generateSequence(vector<int>& sequence);

int main()
{
	//seed the random number
	srand(time(NULL));
	
	//output the greeting
	cout << "--- Belady's Anomaly Simulator ---" << endl
		<< endl
		<< endl
		<< "# of sequences tested: " << NUM_SEQ << endl
		<< "Length of memory reference string: " << MAX_FRAMES<< endl
		<< "Frames of physical memory: " << REF_SIZE << endl
		<< endl;
	
	//loop through 100 sequences
	for(int seqNum = 0; seqNum < NUM_SEQ; seqNum++)
	{
		//make a vector with a random sequence of 1000 numbers from 1 to 255
		vector<int> sequence;
		generateSequence(sequence);
		
		//initialize prev page faults to a big number
		int prevPageFaults = 10000;
	
		//run through 100 different frame sizes
		for(int frameSize = 1; frameSize <= MAX_FRAMES; frameSize++)
		{		
			//the bool array tells if the number is in the queue
			bool inQueue[251] = {};
			queue<int> memory;
			
			//initialize to no page faults
			int pageFaults = 0;
			
			//run the sequence through the queue
			for(int sequenceIndex = 0; sequenceIndex < REF_SIZE; sequenceIndex++)
			{
				if(!inQueue[sequence[sequenceIndex]])
				{
					pageFaults++;
					
					if(memory.size() >= frameSize)
					{						
						inQueue[memory.front()] = false;
						memory.pop();
						
						memory.push(sequence[sequenceIndex]);
						inQueue[sequence[sequenceIndex]] = true;
					
					}
					else
					{					
						memory.push(sequence[sequenceIndex]);
						inQueue[sequence[sequenceIndex]] = true;
					}
				}
			}
			
			if(pageFaults > prevPageFaults) 
			{	
				cout << "Anomoly Detected: " << endl;
				cout << "Sequence: " << seqNum+1 << endl;
				cout << "Page Faults: " << prevPageFaults << " @ Frame Size: " << frameSize-1 << endl;
				cout << "Page Faults: " << pageFaults << " @ Frame Size: " << frameSize << endl;
			}
			
			prevPageFaults = pageFaults;
		}
			
		
	}
				
	return 0;
}

//creates a random sequence of numbers 1 to 255
void generateSequence(vector<int>& sequence)
{
	for(int i = 0; i < REF_SIZE; i++)
		sequence.push_back(rand()%250 + 1);
}