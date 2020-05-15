#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <iostream>
#include <chrono>
#include <cmath>
#include <unistd.h>


void *tau_1(void *);
void *tau_2(void *);
void *tau_3(void *);
void setSchedulingPolicy(int, int);
void pinCPU(int);
void do_wrap_up(int, int);

int r1 = 0, r2 = 0, r3 = 0;
extern int
main(void)
{
  pinCPU(0);

  pthread_t thread1, thread2, thread3;
  if (pthread_create(&thread1, 
		 NULL,
		 tau_1,
		 (void *) &r1) != 0)
	perror("pthread_create"), exit(1); 

  if (pthread_create(&thread2, 
		 NULL, 
		 tau_2,
		 (void *) &r2) != 0)
	perror("pthread_create"), exit(1);

 if (pthread_create(&thread3,
                 NULL,
                 tau_3,
                 (void *) &r3) != 0)
        perror("pthread_create"), exit(1);

  if (pthread_join(thread1, NULL) != 0)
	perror("pthread_join"),exit(1);

  if (pthread_join(thread2, NULL) != 0)
	perror("pthread_join"),exit(1);

  if (pthread_join(thread3, NULL) != 0)
        perror("pthread_join"),exit(1);

  //do_wrap_up(r1, r2);

  return 0; 
}

void *tau_1(void *pnum_times)
{
  int period = 50000;
  int repeat = 100000, tmp = 10000;
  int delta;
  setSchedulingPolicy (SCHED_FIFO, 99);

  while(1){
  	std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
        
	for (int i = 0; i <= repeat; i++)
        {
                tmp = sqrt(tmp)*100;
        }
	        
	std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
        delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        if (delta > period)
        {
            continue;
        }
        else
        {
            usleep (period-delta);
        }
      printf("tau_1 delta = %d\n",delta);	
  }

  return(NULL);
}

void *tau_2(void *pnum_times)
{
  int period = 100000;
  int repeat = 100000, tmp = 10000;
  int delta;
  setSchedulingPolicy (SCHED_FIFO, 98);

  while(1){
        std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

	for(int j = 0; j < 50; j++){
        	for (int i = 0; i <= repeat; i++)
        	{
                	tmp = sqrt(tmp)*100;
        	}
	}

        std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
        delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        if (delta > period)
        {
            continue;
        }
        else
        {
            usleep (period-delta);
        }
	printf("tau_2 delta = %d\n",delta);
  }

  return(NULL);
}

void *tau_3(void *pnum_times)
{
  int period = 200000;
  int repeat = 100000, tmp = 10000;
  int delta;
  setSchedulingPolicy (SCHED_FIFO, 97);

  while(1){
        std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

	for (int j = 0; j < 10; j++){
        	for (int i = 0; i <= repeat; i++)
        	{
                	tmp = sqrt(tmp)*100;
        	}
	}
	
        std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
        delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        if (delta > period)
        {
            continue;
        }
        else
        {
            usleep (period-delta);
        }
	printf("tau_3 delta = %d\n",delta);
  }

  return(NULL);
}

void do_wrap_up(int one_times, int another_times)
{
  int total;
  total = one_times + another_times;
  printf("All done, one thing %d, another %d for a total of %d\n",
	one_times, another_times, total);
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

void setSchedulingPolicy (int newPolicy, int priority)
{
    sched_param sched;
    int oldPolicy;
    if (pthread_getschedparam(pthread_self(), &oldPolicy, &sched)) {
        perror("pthread_setschedparam");
        exit(EXIT_FAILURE);
    }
    sched.sched_priority = priority;
    if (pthread_setschedparam(pthread_self(), newPolicy, &sched)) {
        perror("pthread_setschedparam");
        exit(EXIT_FAILURE);
    }
}
