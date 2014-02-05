/*
Alan Christensen
CS 3100
sysinfo.cpp
8/4/2012
*/

#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

//function prototypes
void displayCPUinfo();
void displayMemInfo();
void displayTimeInfo();
void displayThreadsInfo();
void displayOSverInfo();
void displayHelp();

//main function 
//checks if there is an argument given: if not it runs the help page
// if so it runs the argument and show the desired sys info
int main(int argc, char* argv[])
{
	string argument;

	if(argc > 1)
		argument = argv[1];
	else
		argument = "-help";

	if(argument == "-cpu")
		displayCPUinfo();
	else if(argument == "-mem")
		displayMemInfo();
	else if(argument == "-time")
		displayTimeInfo();
	else if(argument == "-threads")
		displayThreadsInfo();
	else if(argument == "-osver")
		displayOSverInfo();
	else
		displayHelp();

	return 0;
}

// Displays the CPU vendor, model name, speed and cache size
void displayCPUinfo()
{
	cout << "Here is the CPU info available:\n\n";
	system("cat /proc/cpuinfo | grep 'vendor_id\\|model name\\|cache size'");
	cout << endl;
}

// Displays the memory size, free memory, and swap space (all in KB)
void displayMemInfo()
{
	cout << "Here is the Memory info Available:\n\n";
	system("cat /proc/meminfo | grep 'MemTotal\\|MemFree\\|SwapTotal'");
	cout << endl;
}

// Displays the number of days, hours, and minutes since the last kernel reboot
void displayTimeInfo()
{
	ifstream file("/proc/uptime");
	int seconds, min, hours, days;
	file >> seconds;

	days = seconds/(60*60*24);	
	hours = (seconds/3600)%24;
	min = (seconds/60)%60;
	seconds = seconds%60;

	cout << "Here is how long it has been since the last kernal reboot:\n\n";
	
	cout << days << " days, " << hours << " hours, " 
		<< min << " minutes, " << seconds << " seconds." << endl
		<< endl;
}

// Displays the max number of threads possible
void displayThreadsInfo()
{
	cout << "Here is the max number of threads:\n\n";
        system("cat /proc/sys/kernel/threads-max");
	cout << endl;
}

// Displays the current Linux kernel version
void displayOSverInfo()
{
	cout << "This is the current linux kernal version:\n\n";
        system("cat /proc/version_signature");
	cout << endl;
}

// Displays the man page that hs the options available
void displayHelp()
{
	cout << endl
	<< "sysinfo(1)			CS 3100 Programmer's Manual" << endl
	<< endl
	<< "NAME: sysinfo - report Linux operating system information" << endl
	<< "SYNOPSIS: sysinfo [-option]" << endl
	<< "DESCRIPTION: Displays the requested information on the current Linux OS information.\n"
	<< endl
	<< "-cpu Display the CPU vendor, model name, speed and cache size" << endl
	<< endl
	<< "-mem Display the memory size, free memory, and swap space (all in KB)" << endl
	<< endl
	<< "-time Display the number of days, hours, and minutes since the last kernel reboot" << endl
	<< endl
	<< "-threads Display the max number of threads possible" << endl
	<< endl
	<< "-osver Display the current Linux kernel version" << endl
	<< endl
	<< "-help Displays this man page" << endl
	<< endl;
}
