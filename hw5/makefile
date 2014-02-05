#this makes my program
Main: Interleave.o Process.o Threaded.o
	g++ -o interleave Interleave.o Process.o Threaded.o -pthread

Interleave.o: Interleave.cpp
	g++ -c Interleave.cpp
	
Process.o: Process.cpp Process.h
	g++ -c Process.cpp
	
Threaded.o: Threaded.cpp Threaded.h
	g++ -c Threaded.cpp

clean:
	rm *.o
	rm interleave
	ipcrm -M 2246 
	ipcrm -S 2246