#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

using namespace std;

const string EXIT_STRING = "exit";
const string HISTORY_STRING = "history";
const string GET_HISTORY_STRING = "^";
const string PIPE_STRING = ">";
const string PTIME_STRING = "ptime";

const int PIPE_COUNT = 2;
const int PIPE_READ_END = 0;
const int PIPE_WRITE_END = 1;

const int STDIN = 0;
const int STDOUT = 1;

void readCommand(vector<string>& args);
int containsPipe(const vector<string>& args);
float runProgram(vector<string>& args);
float runProgramPipe(vector<string>& args1, vector<string>& args2);
void replaceFromHistory(vector<string>& args, vector<vector<string> >& history);
void showHistory(const vector<vector<string> >& history);

//my own command shell program
int main()
{
	vector<vector<string> > history;
	vector<string> args;
	float childRunTime = 0;

	//output "[cmd]:" to show we are in the program and accept a line as a command
	cout << "[cmd]:";
	readCommand(args);
	
	//loop until the exit string is entered
	while(args[0] != EXIT_STRING)
	{
		//replace the command with the given history if valid on "^ " and the number
		if(args[0] == GET_HISTORY_STRING)
			replaceFromHistory(args, history);
		
		//output the history vector contents when the history string is entered
		if(args[0] == HISTORY_STRING)
			showHistory(history);
			
		else if(args[0] == PTIME_STRING)
			cout << "Time spent executing child processes: " << childRunTime << " seconds.\n";

		//for pipes
		else if(containsPipe(args))
		{
			int pos = containsPipe(args);
			vector<string> args1;
			vector<string> args2;

			for(int i = 0; i < pos; i++)
				args1.push_back(args[i]);
			for(int i = pos+1; i < args.size(); i++)
				args2.push_back(args[i]);

			childRunTime += runProgramPipe(args1, args2);
		}
		//it is a regular command
		else
			childRunTime += runProgram(args);
		
		//add the current command to the history list and clear current args
		history.push_back(args);
		args.clear();
		
		//output the cmd and wait for a new one
		cout << "[cmd]:";
		readCommand(args);
	}
	
	return 0;
}

//reads in the command word by word into the args vector
//reads only one line
void readCommand(vector<string>& args)
{
	string command;

	while(cin.peek() != '\n')
	{
		cin >> command;
		args.push_back(command);
	}

	cin.ignore();

	if(args.size() == 0)
		args.push_back("");
}

//finds if there is a pipe and returns the location
int containsPipe(const vector<string>& args)
{
	for(int i = 0; i < args.size(); i++)
	{
		if(args[i] == ">")
			return i;
	}
	return 0;
}

//runs the command
float runProgram(vector<string>& args)
{
	float childRunTime = 0;
	struct timeval startTime;
	struct timeval endTime;
	
	//fork to create parent and child process to run from
	pid_t pid = fork();
	
	//get the child start time
	gettimeofday(&startTime, NULL);
	
	//pid 0 means child
	if(pid == 0)
	{
		//build the argv array
		char** argv = new char*[args.size()+1];

		for(int i = 0; i < args.size(); i++)
		{	
			argv[i] = new char[args[i].size()+1];
			strcpy(argv[i],args[i].c_str());
		}

		argv[args.size()] = NULL;
		
		//run program and check for validity
		if( execvp(argv[0], argv) == -1)
			cout << "Error invalid: The program you requested doesn't exist." << endl;
		
		//quit the child process on invalid program
		exit(0);
	}
	else
	{
		//wait on the child process
		wait(NULL);
		
		//get the time to count children runtime
		gettimeofday(&endTime, NULL);

		//combine the seconds and microseconds into the run time
		childRunTime = float(endTime.tv_sec - startTime.tv_sec) + 
					(float(endTime.tv_usec - startTime.tv_usec)/1000000);
	}
	
	return childRunTime;
}

//runs the two programs and pipes the output from the first to the input of the second 
float runProgramPipe(vector<string>& args1, vector<string>& args2)
{
	float childRunTime1, 
		  childRunTime2;
	struct timeval startTime;
	struct timeval endTime;
	
	//set up pipe
	int pids[PIPE_COUNT];
	int savedIn = dup(STDIN), 
		savedOut = dup(STDOUT);
		
	pipe(pids);
	
	//get the child start time
	gettimeofday(&startTime, NULL);
	
	//fork for first proces
	pid_t pid = fork();
	
	//pid 0 means child
	if(pid == 0)
	{
		//build the argv array
		char** argv = new char*[args1.size()+1];

		for(int i = 0; i < args1.size(); i++)
		{
			argv[i] = new char[args1[i].size()+1];
			strcpy(argv[i],args1[i].c_str());
		}

		argv[args1.size()] = NULL;

		//set up the pipe ends
		savedOut = dup(STDOUT);
		dup2(pids[PIPE_WRITE_END], STDOUT);

		//run program and check for validity
		if( execvp(argv[0], argv) == -1)
			cout << "Error invalid: The program you requested doesn't exist." << endl;
		
		//for invalid programs kill the process
		exit(0);
	}
	else
	{
		wait(NULL);
		
		//close the write end of the pipe
		close(pids[PIPE_WRITE_END]);
		dup2(savedOut, STDOUT);

		//get the time to count children runtime
		gettimeofday(&endTime, NULL);

		//combine the seconds and microseconds into the run time
		childRunTime1 = float(endTime.tv_sec - startTime.tv_sec) + 
					(float(endTime.tv_usec - startTime.tv_usec)/1000000);
	}

	//get the child start time
    gettimeofday(&startTime, NULL);

	//fork for second proces
	pid = fork();

	//pid 0 means child
	if(pid == 0)
	{
		//build the argv array
		char** argv = new char*[args2.size()+1];

		for(int i = 0; i < args2.size(); i++)
		{
			argv[i] = new char[args2[i].size()+1];
			strcpy(argv[i],args2[i].c_str());
		}

		argv[args2.size()] = NULL;

		//set up the pipe ends
		savedIn = dup(STDIN);
		dup2(pids[PIPE_READ_END], STDIN);

		//run program and check for validity
		if( execvp(argv[0], argv) == -1)
			cout << "Error invalid: The program you requested doesn't exist." << endl;
		
		//for invalid programs kill the process
		exit(0);
	}
	else
	{
		wait(NULL);

		//close the read in of the pipe
		close(pids[PIPE_READ_END]); 
		dup2(savedIn, STDIN);

		//get the time to count children runtime
		gettimeofday(&endTime, NULL);

		//combine the seconds and microseconds into the run time
		childRunTime2 = float(endTime.tv_sec - startTime.tv_sec) + 
			(float(endTime.tv_usec - startTime.tv_usec)/1000000);
	}
	
	return childRunTime1 + childRunTime2;
}


//checks if a valid history index is given and if so it replaces the command with the one from history
void replaceFromHistory(vector<string>& args, vector<vector<string> >& history)
{
	//checks if there is a '^' and a number
	if(args.size() < 2)
	{
		cout << "Error index: no history index given.\n";
		cout << "Type 'history' to see the valid number options then type '^ ' and the desired number. \n\n";
	}
	else
	{
		// get the number using string stream
		stringstream myStream(args[1]);
		int number;
	
		myStream >> number;
	
		if(number < 1 || number > history.size())
		{
			cout << "Error index: max history size is " << history.size() << ". Invalid entry.\n";
			cout << "Type 'history' to see the valid number options then type '^ ' and the desired number. \n\n";
		}
		else
			args = history[number-1];
	}
}


//shows the history list
void showHistory(const vector<vector<string> >& history)
{
	for(int i = 0; i < history.size(); i++)
	{
		cout << "  " << i+1 << ": ";

		for(int j = 0; j < history[i].size(); j++)
		{
			cout << history[i][j] << " ";
		}
		cout << endl;
	}
}
