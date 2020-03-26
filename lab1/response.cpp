#define _GNU_SOURCE 1
#include <sched.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>
#include <chrono>
#include <cmath>
#include <unistd.h>
#include <fstream>

sig_atomic_t t = 0;
int out[2000] = {0};
void collectData (int param)
{
    std::ofstream outFile;
    outFile.open ("responseTime_t1rm.out");
    for (int i = 0; i < 2000; i++)
    {
        outFile << ::out[i] << std::endl;
    }
    outFile.close();
}

void setSchedulingPolicy (int policy, int priority)
{
    sched_param sched;
    sched_getparam(0, &sched);
    sched.sched_priority = priority;
    if (sched_setscheduler(0, policy, &sched)) {
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }
}

void workload_1ms (void)
{
        int repeat = 100000; // tune this for the right amount of workload
        int tmp = 10000;
	for (int i = 0; i <= repeat; i++)
        {
                tmp = sqrt(tmp)*100;
        }
}

void pinCPU (int cpu_number)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);

    CPU_SET(cpu_number, &mask);

    if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1)
    {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
}

int main (void)
{
    void (*prev_handler)(int);
    prev_handler = signal (SIGINT, collectData);

    pinCPU(0);
    int period = 5000; // unit: microsecond
    int delta;
    setSchedulingPolicy (SCHED_FIFO, 99);
    while (1)
    {
        std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
        for (int j = 0; j < 2; j++)
        {
        	workload_1ms ();
        }
        std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
        delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
	if (::t < 2000)
        {
            ::out[::t++] = delta;
        }
	if (delta > period)
        {
    	    continue;        
	}
        else
        {
            usleep (period-delta);
        }
    }
    return 0;
}
