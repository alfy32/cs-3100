# make relationsship file for MyIostream program
Main: MainCode.o MyIostream.o
	g++ -o main MainCode.o MyIostream.o

MainCode.o: MainCode.cpp
	g++ -c MainCode.cpp

MyIostream.o: MyIostream.cpp
	g++ -c MyIostream.cpp

clean: 
	rm *.o
	rm main
