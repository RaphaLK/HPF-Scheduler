/*****
 * Highest Priority First without Preemptive Scheduling
 *
 * Project Steps:
 * - Run algorithm for 100 quanta, labeled 0 - 99. Before each run, we want to
 * generate a set of simulated processes. Each process will have a randomly generated:
 *    - Arrival time: A float value from 0 - 99
 *    - Expected total run time: float from 0.1 - 10 quanta
 *    - A priority from 1 - 4, where 1 is the highest
 *
 * Notes: Use FCFS
 *
 * Written by: Raphael Kusuma -- 10/11/2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_PROCESSES 26
#define MAX_QUANTA 100

// For the sake of not having multiple header files, I'm gonna have this large C file.
typedef struct process
{
  // int processId;
  char processName;
  float arrivalTime;
  float expectedRunTime;
  float remainingTime;
  int priority;
  // When a process starts and finished
  float startTime;
  float finishTime;
  
  // It is equal to the sum total of Waiting time and Execution time.
  float turnaroundTime;
  float waitingTime;
  int timesPreempted;
} process;

// pqueue
typedef struct pqueue
{
  process *processes[NUM_PROCESSES];
  int front;
  int rear;
  int count;
} pqueue;

// Stats
typedef struct stats
{
  float avgTurnaroundTime;
  float avgWaitingTime;
  float avgResponseTime;
  float throughput;

  int totalProcesses;
} stats;

// Per-priority statistics structure
typedef struct priority_stats
{
  stats priorityStats[4];  // Statistics for each priority level (1-4)
  stats overallStats;      // Overall statistics across all priorities
} priority_stats;

process processList[NUM_PROCESSES];
int numProcesses = 0;

// Priority Queue util functions
void initQueue(pqueue *q)
{
  // Setting front and rear queue pointers.
  q->front = 0;
  q->rear = -1;
  q->count = 0;
}

void enqueue(pqueue *q, process *p)
{
  if (q->count < NUM_PROCESSES)
  {
    q->rear = (q->rear + 1) % NUM_PROCESSES;
    q->processes[q->rear] = p;
    q->count++;
  }
}

process *dequeue(pqueue *q)
{
  if (q->count > 0)
  {
    process *p = q->processes[q->front];
    q->front = (q->front + 1) % NUM_PROCESSES;
    q->count--;
    return p;
  }
  return NULL;
}

void generate_proc()
{
  numProcesses = 0;

  // "It's ok to create more processes than you use"
  for (int i = 0; i < NUM_PROCESSES; i++)
  {
    process *simProcess = &processList[numProcesses];
    simProcess->processName = 'A' + (rand() % 26);
    // arrival: 0 - 100f, expectedRunTime 0.1 - 10f 
    simProcess->arrivalTime = ((float)rand() / (float)(RAND_MAX)) * 99.0f;
    simProcess->expectedRunTime = 0.1f + ((float)rand() / (float)(RAND_MAX)) * 9.9f;
    simProcess->remainingTime = simProcess->expectedRunTime;
    simProcess->priority = (rand() % 4) + 1; // 1-4, where 1 is highest

    // Statistics
    simProcess->startTime = -1;
    simProcess->finishTime = -1;
    simProcess->turnaroundTime = 0;
    simProcess->waitingTime = 0;
    simProcess->timesPreempted = 0;

    numProcesses++;
  }

  // Sort processes by arrival time 
  for (int i = 0; i < numProcesses - 1; i++) {
    for (int j = 0; j < numProcesses - i - 1; j++) {
      if (processList[j].arrivalTime > processList[j + 1].arrivalTime) {
        process temp = processList[j];
        processList[j] = processList[j + 1];
        processList[j + 1] = temp;
      }
    }
  }

  printf("Generated %d processes\n", numProcesses);
}

// Process stats
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
  
  // the time-interval between submission of a request, and the first response to that request
  s->avgResponseTime = completedProcesses > 0 ? totalResponse / completedProcesses : 0;
  
  // Throughput = finished processes / total simulation time
  s->throughput = maxFinishTime > 0 ? completedProcesses / maxFinishTime : 0;
}

// Stats per priority queue
void calculatePriorityStats(priority_stats *ps)
{
  // Initialize stats of each priority queue
  for (int priority = 0; priority < 4; priority++)
  {
    ps->priorityStats[priority].avgTurnaroundTime = 0;
    ps->priorityStats[priority].avgWaitingTime = 0;
    ps->priorityStats[priority].avgResponseTime = 0;
    ps->priorityStats[priority].throughput = 0;
    ps->priorityStats[priority].totalProcesses = 0;
  }

  float totalTurnaround[4] = {0};
  float totalWaiting[4] = {0};
  float totalResponse[4] = {0};
  int completedProcesses[4] = {0};
  float maxFinishTime[4] = {0};
  
  float overallTotalTurnaround = 0;
  float overallTotalWaiting = 0;
  float overallTotalResponse = 0;
  int overallCompletedProcesses = 0;
  float overallMaxFinishTime = 0;

  // Calculate statistics for each priority and overall
  for (int i = 0; i < numProcesses; i++)
  {
    // Only count processes that actually started (startTime >= 0) and completed
    if (processList[i].startTime >= 0 && processList[i].finishTime >= 0)
    {
      int priority = processList[i].priority - 1; // Convert to 0-based index
      
      // Per-priority statistics
      totalTurnaround[priority] += processList[i].turnaroundTime;
      totalWaiting[priority] += processList[i].waitingTime;
      
      // resp time - time from arrival to start
      float responseTime = processList[i].startTime - processList[i].arrivalTime;
      totalResponse[priority] += responseTime;
      
      completedProcesses[priority]++;
      
      if (processList[i].finishTime > maxFinishTime[priority])
      {
        maxFinishTime[priority] = processList[i].finishTime;
      }
      
      // Overall statistics
      overallTotalTurnaround += processList[i].turnaroundTime;
      overallTotalWaiting += processList[i].waitingTime;
      overallTotalResponse += responseTime;
      overallCompletedProcesses++;
      
      if (processList[i].finishTime > overallMaxFinishTime)
      {
        overallMaxFinishTime = processList[i].finishTime;
      }
    }
  }

  // Calculate averages for each priority queue
  for (int priority = 0; priority < 4; priority++)
  {
    ps->priorityStats[priority].totalProcesses = completedProcesses[priority];
    
    if (completedProcesses[priority] > 0)
    {
      ps->priorityStats[priority].avgTurnaroundTime = totalTurnaround[priority] / completedProcesses[priority];
      ps->priorityStats[priority].avgWaitingTime = totalWaiting[priority] / completedProcesses[priority];
      ps->priorityStats[priority].avgResponseTime = totalResponse[priority] / completedProcesses[priority];
    }
    
    if (maxFinishTime[priority] > 0)
    {
      ps->priorityStats[priority].throughput = completedProcesses[priority] / maxFinishTime[priority];
    }
  }

  // Calculate overall averages
  ps->overallStats.totalProcesses = overallCompletedProcesses;
  
  if (overallCompletedProcesses > 0)
  {
    ps->overallStats.avgTurnaroundTime = overallTotalTurnaround / overallCompletedProcesses;
    ps->overallStats.avgWaitingTime = overallTotalWaiting / overallCompletedProcesses;
    ps->overallStats.avgResponseTime = overallTotalResponse / overallCompletedProcesses;
  }
  
  if (overallMaxFinishTime > 0)
  {
    ps->overallStats.throughput = overallCompletedProcesses / overallMaxFinishTime;
  }
}

void printStats(stats *s, const char *algorithmName)
{
  printf("\n=== %s Stats ===\n", algorithmName);
  printf("Total Processes Completed: %d\n", s->totalProcesses);
  printf("Avg. Turnaround Time: %.2f quanta\n", s->avgTurnaroundTime);
  printf("Avg. Waiting Time: %.2f quanta\n", s->avgWaitingTime);
  printf("Avg. Response Time: %.2f quanta\n", s->avgResponseTime);
  printf("Throughput: %.2f processes/quantum\n", s->throughput);
}

void printPriorityStats(priority_stats *ps, const char *algorithmName)
{
  printf("\n=== %s PQueue Statistics ===\n", algorithmName);
  
  // Print statistics for each priority queue
  for (int priority = 0; priority < 4; priority++)
  {
    printf("\n--- Priority %d Statistics ---\n", priority + 1);
    printf("Total Processes Completed: %d\n", ps->priorityStats[priority].totalProcesses);
    
    if (ps->priorityStats[priority].totalProcesses > 0)
    {
      printf("Avg. Turnaround Time: %.2f quanta\n", ps->priorityStats[priority].avgTurnaroundTime);
      printf("Avg. Waiting Time: %.2f quanta\n", ps->priorityStats[priority].avgWaitingTime);
      printf("Avg. Response Time: %.2f quanta\n", ps->priorityStats[priority].avgResponseTime);
      printf("Throughput: %.2f processes/quantum\n", ps->priorityStats[priority].throughput);
    }
    else
    {
      printf("N/A (starvation)\n");
      printf("Avg Turnaround Time: N/A\n");
      printf("Avg Waiting Time: N/A\n");
      printf("Avg Response Time: N/A\n");
      printf("Throughput: N/A\n");
    }
  }
  
  // Print overall statistics
  printf("\n--- Overall Statistics ---\n");
  printf("Total Processes Completed: %d\n", ps->overallStats.totalProcesses);
  printf("Avg Turnaround Time: %.2f quanta\n", ps->overallStats.avgTurnaroundTime);
  printf("Avg Waiting Time: %.2f quanta\n", ps->overallStats.avgWaitingTime);
  printf("Avg Response Time: %.2f quanta\n", ps->overallStats.avgResponseTime);
  printf("Throughput: %.2f processes/quantum\n", ps->overallStats.throughput);
}

// HPF Preemptive Scheduling
void hpf_preemptive()
{
  pqueue priorityQueues[4];
  for (int i = 0; i < 4; i++)
  {
    initQueue(&priorityQueues[i]);
  }

  int currentTime = 0;
  int processIndex = 0;
  process *currentProcess = NULL;
  priority_stats schedulerStats = {0};
  int idleTime = 0;
  int totalPreemptions = 0;

  printf("\n=== HPF Preemptive Scheduling ===\n");
  printf("Time\tProcess\tPriority\tRemaining\tStatus\n");

  while (currentTime < MAX_QUANTA * 2)
  { 
    // Allow completion beyond 100 quanta
    while (processIndex < numProcesses &&
           processList[processIndex].arrivalTime <= currentTime &&
           processList[processIndex].arrivalTime < MAX_QUANTA)
    {

      int priority = processList[processIndex].priority - 1; // Convert to 0-based index
      // Time (in Quanta) -> Process Name -> Proc Priority Level -> Remaining Quanta till Completion -> Current Status
      enqueue(&priorityQueues[priority], &processList[processIndex]);
      printf("%d\t%c\t%d\t\t%.1f\t\tArrived\n",
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
        if (priorityQueues[i].count > 0)
        {
          // Preempt current process
          // Time (in Quanta) -> Process Name -> Proc Priority Level -> Remaining Quanta till Completion -> Current Status
          printf("%d\t%c\t%d\t\t%.1f\t\tPre-empted\n",
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
        if (priorityQueues[i].count > 0)
        {
          // Check if it's the first time a process ran after quanta > 99
          // DO NOT dequeue yet
          process* tempProc = priorityQueues[i].processes[priorityQueues[i].front];

          if (tempProc->startTime < 0 && currentTime > MAX_QUANTA)
          {
            continue;
          }

          currentProcess = dequeue(&priorityQueues[i]);
          
          if (currentProcess->startTime < 0)
          {
            currentProcess->startTime = currentTime;
          }
        // Time (in Quanta) -> Process Name -> Proc Priority Level -> Remaining Quanta till Completion -> Current Status
          printf("%d\t%c\t%d\t\t%.1f\t\tStarted\n",
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
        currentProcess->finishTime = currentTime++;
        // turnaroundtime = finish - arrival
        currentProcess->turnaroundTime = currentProcess->finishTime - currentProcess->arrivalTime;
        // wait = turnaround - expectedruntime
        currentProcess->waitingTime = currentProcess->turnaroundTime - currentProcess->expectedRunTime;
        
        // Time (in Quanta) -> Process Name -> Proc Priority Level -> Remaining Quanta till Completion -> Current Status
        printf("%d\t%c\t%d\t\t%.1f\t\tCompleted\n",
               currentTime++, currentProcess->processName,
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
        printf("%d\t-\t-\t\t-\t\tIdle\n", currentTime);
      // Break if idle for too long and no more processes can arrive
      if (idleTime > 2 && processIndex >= numProcesses) 
        break;
    }

    currentTime++;
  }

  calculatePriorityStats(&schedulerStats);
  printPriorityStats(&schedulerStats, "HPF Preemptive");
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