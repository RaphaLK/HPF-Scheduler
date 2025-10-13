/*****
 * Highest Priority First with Preemptive Scheduling
 *
 * Project Steps:
 * - Run algorithm for 100 quanta, labeled 0 - 99. Before each run, we want to
 * generate a set of simulated processes. Each process will have a randomly generated:
 *    - Arrival time: A float value from 0 - 99
 *    - Expected total run time: float from 0.1 - 10 quanta
 *    - A priority from 1 - 4, where 1 is the highest
 * 
 * Notes: Use RR with a time slice of 1 quantum.
 * 
 * Written by: Raphael Kusuma -- 10/11/2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hpf_pre.h"

process processList[100];

int main()
{
  srand(time(NULL));
  generate_proc();
}

int hpf_sched() 
{

}

void generate_proc()
{
  for (int i = 0; i < 100; i++)
  {
    process simProcess;
    simProcess.arrivalTime = (float)rand() / (float)(RAND_MAX) * 99.0f;
    simProcess.arrivalTime = 0.1f + (float)rand() / (float)(RAND_MAX) * 9.9f;
    simProcess.priority = (rand() % 4) + 1;
    processList[i];
  }
}