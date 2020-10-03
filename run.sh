echo "lamport clock is running."
g++ -c -m64 -Wall -no-pie -o main.o main.cpp 
g++  -m64 -no-pie -o main.exe main.o
./main.exe
echo "lamport clock has finished running."