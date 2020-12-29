# Lamport's Logical Clocks

Please visit my project report for more details! 

![lamportclock.gif](https://github.com/LeadProgramming/Lamport-Clock/blob/main/lamportclock.gif?raw=true)

## Summary

In a distributed system, multiple processes are transmitting or receiving messages at any time. This type of execution is known as asynchronous. Every process has there on clock but there is no global clock to know where these events are happening at any time. To keep track of these events we need to use a logical clock algorithm.

## Installation

### Linux/Window

I used Cygwin on windows for this project and it should work on both platform.

Simple way to run my program.

```
sh run.sh
```

**OR**

Here are the commands to compile and build an executable.

```c++
g++ -c -m64 -Wall -no-pie -o main.o main.cpp 
g++ -m64 -no-pie -o main.exe main.o 
```

**Note**: For all the window users please use Cygwin or WSL.

To execute the program.

```c++
./main.exe
```

## Reference

I used my professor lectures to aid me in building this algorithm

