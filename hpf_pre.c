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
#include <string.h>

#define MAX_PROCESSES 167
#define MAX_QUANTA 100

// For the sake of not having multiple header files, I'm gonna have this large C file.
typedef struct process
{
  int processId;
  char processName;
  float arrivalTime;
  float expectedRunTime;
  float remainingTime;
  int priority;
  float startTime;
  float finishTime;
  float turnaroundTime;
  float waitingTime;
  int timesPreempted;
} process;

// Queue struct -> Will be used for the priority queue
typedef struct pqueue
{
  process *processes[MAX_PROCESSES];
  int front;
  int rear;
  int count;
} pqueue;

// Statistics structure
typedef struct stats
{
  float avgTurnaroundTime;
  float avgWaitingTime;
  float avgResponseTime;
  float throughput;
  int totalProcesses;
} stats;

process processList[MAX_PROCESSES];
int numProcesses = 0;

// Priority Queue util functions
void initQueue(pqueue *q)
{
  q->front = 0;
  q->rear = -1;
  q->count = 0;
}

void enqueue(pqueue *q, process *p)
{
  if (q->count < MAX_PROCESSES)
  {
    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->processes[q->rear] = p;
    q->count++;
  }
}

process *dequeue(pqueue *q)
{
  if (q->count > 0)
  {
    process *p = q->processes[q->front];
    q->front = (q->front + 1) % MAX_PROCESSES;
    q->count--;
    return p;
  }
  return NULL;
}

int isQueueEmpty(pqueue *q)
{
  return q->count == 0;
}

void generate_proc()
{
  numProcesses = 0;

  // "It's ok to create more processes than you use"
  for (int i = 0; i < MAX_PROCESSES; i++)
  {
    process *simProcess = &processList[numProcesses];
    simProcess->processId = numProcesses;
    simProcess->processName = 'A' + (rand() % 26);
    simProcess->arrivalTime = (float)rand() / (float)(RAND_MAX) * 99.0f;
    simProcess->expectedRunTime = 0.1f + (float)rand() / (float)(RAND_MAX) * 9.9f;
    simProcess->remainingTime = simProcess->expectedRunTime;
    simProcess->priority = (rand() % 4) + 1; // 1-4, where 1 is highest
    simProcess->startTime = -1;
    simProcess->finishTime = -1;
    simProcess->turnaroundTime = 0;
    simProcess->waitingTime = 0;
    simProcess->timesPreempted = 0;

    numProcesses++;
  }

  printf("Generated %d processes\n", numProcesses);
}

void calculateStats(stats *s)
{
  float totalTurnaround = 0;
  float totalWaiting = 0;
  float totalResponse = 0;
  int completedProcesses = 0;
  float maxFinishTime = 0;

  for (int i = 0; i < numProcesses; i++)
  {
    if (processList[i].finishTime >= 0)
    {
      totalTurnaround += processList[i].turnaroundTime;
      totalWaiting += processList[i].waitingTime;
      
      // Response time = start time - arrival time
      float responseTime = processList[i].startTime - processList[i].arrivalTime;
      totalResponse += responseTime;
      
      completedProcesses++;
      
      // Track maximum finish time for throughput calculation
      if (processList[i].finishTime > maxFinishTime)
      {
        maxFinishTime = processList[i].finishTime;
      }
    }
  }

  s->totalProcesses = completedProcesses;
  s->avgTurnaroundTime = completedProcesses > 0 ? totalTurnaround / completedProcesses : 0;
  s->avgWaitingTime = completedProcesses > 0 ? totalWaiting / completedProcesses : 0;
  s->avgResponseTime = completedProcesses > 0 ? totalResponse / completedProcesses : 0;
  
  // Throughput = completed processes / total simulation time
  s->throughput = maxFinishTime > 0 ? completedProcesses / maxFinishTime : 0;
}

void printStats(stats *s, const char *algorithmName)
{
  printf("\n=== %s Statistics ===\n", algorithmName);
  printf("Total Processes Completed: %d\n", s->totalProcesses);
  printf("Avg Turnaround Time: %.2f quanta\n", s->avgTurnaroundTime);
  printf("Avg Waiting Time: %.2f quanta\n", s->avgWaitingTime);
  printf("Avg Response Time: %.2f quanta\n", s->avgResponseTime);
  printf("Throughput: %.2f processes/quantum\n", s->throughput);
}

// HPF Preemptive Scheduling
void hpf_preemptive()
{
  pqueue priorityQueues[4];
  for (int i = 0; i < 4; i++)
  {
    initQueue(&priorityQueues[i]);
  }

  float currentTime = 0;
  int processIndex = 0;
  process *currentProcess = NULL;
  stats schedulerStats = {0};
  int idleTime = 0;
  int totalPreemptions = 0;

  printf("\n=== HPF Preemptive Scheduling ===\n");
  printf("Time\tProcess\tPriority\tRemaining\tStatus\n");

  while (currentTime < MAX_QUANTA * 2)
  { // Allow completion beyond 100 quanta
    // Add arriving processes to appropriate priority queues
    while (processIndex < numProcesses &&
           processList[processIndex].arrivalTime <= currentTime &&
           processList[processIndex].arrivalTime < MAX_QUANTA)
    {

      int priority = processList[processIndex].priority - 1; // Convert to 0-based index
      // Time (in Quanta) -> Process Name -> Proc Priority Level -> Remaining Quanta till Completion -> Current Status
      enqueue(&priorityQueues[priority], &processList[processIndex]);
      printf("%.1f\t%c\t%d\t\t%.1f\t\tArrived\n",
             currentTime, processList[processIndex].processName,
             processList[processIndex].priority, processList[processIndex].remainingTime);
      processIndex++;
    }

    // Check if current process should be preempted
    if (currentProcess != NULL)
    {
      // Check if a higher priority process has arrived
      for (int i = 0; i < currentProcess->priority - 1; i++)
      {
        if (!isQueueEmpty(&priorityQueues[i]))
        {
          // Preempt current process
          // Time (in Quanta) -> Process Name -> Proc Priority Level -> Remaining Quanta till Completion -> Current Status
          printf("%.1f\t%c\t%d\t\t%.1f\t\tPreempted\n",
                 currentTime, currentProcess->processName,
                 currentProcess->priority, currentProcess->remainingTime);

          int currentPriority = currentProcess->priority - 1;
          enqueue(&priorityQueues[currentPriority], currentProcess);
          currentProcess->timesPreempted++;
          totalPreemptions++;
          currentProcess = NULL;
          break;
        }
      }
    }

    // Select next process if no current process
    if (currentProcess == NULL)
    {
      for (int i = 0; i < 4; i++)
      {
        if (!isQueueEmpty(&priorityQueues[i]))
        {
          currentProcess = dequeue(&priorityQueues[i]);
          if (currentProcess->startTime < 0)
          {
            currentProcess->startTime = currentTime;
          }
        // Time (in Quanta) -> Process Name -> Proc Priority Level -> Remaining Quanta till Completion -> Current Status
          printf("%.1f\t%c\t%d\t\t%.1f\t\tStarted\n",
                 currentTime, currentProcess->processName,
                 currentProcess->priority, currentProcess->remainingTime);
          break;
        }
      }
    }

    if (currentProcess != NULL)
    {
      // Execute process for 1 quantum
      currentProcess->remainingTime -= 1.0f;

      if (currentProcess->remainingTime <= 0)
      {
        // Process completed
        currentProcess->finishTime = currentTime + 1.0f;
        currentProcess->turnaroundTime = currentProcess->finishTime - currentProcess->arrivalTime;
        currentProcess->waitingTime = currentProcess->turnaroundTime - currentProcess->expectedRunTime;
        
        // Time (in Quanta) -> Process Name -> Proc Priority Level -> Remaining Quanta till Completion -> Current Status
        printf("%.1f\t%c\t%d\t\t%.1f\t\tCompleted\n",
               currentTime + 1.0f, currentProcess->processName,
               currentProcess->priority, currentProcess->remainingTime);

        currentProcess = NULL;
        idleTime = 0;
      }
    }
    else
    {
      // CPU is idle
      idleTime++;
      if (idleTime <= 2)
      {
        printf("%.1f\t-\t-\t\t-\t\tIdle\n", currentTime);
      }

      // Break if idle for too long and no more processes can arrive
      if (idleTime > 10 && processIndex >= numProcesses)
      {
        break;
      }
    }

    currentTime += 1.0f;
  }

  calculateStats(&schedulerStats);
  printStats(&schedulerStats, "HPF Preemptive");
}

int main()
{
  srand(time(NULL));

  printf("HPF Scheduler Simulation\n");
  printf("========================\n");

  // Generate processes
  generate_proc();
  // Scheduler
  hpf_preemptive();

  return 0;
}