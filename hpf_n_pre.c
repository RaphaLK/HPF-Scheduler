/*****
 * Highest Priority First with Non-Preemptive Scheduling 
 * 
 * Project Steps:
 * - Run algorithm for 100 quanta, labeled 0 - 99. Before each run, we want to 
 * generate a set of simulated processes. Each process will have a randomly generated:
 *    - Arrival time: A float value from 0 - 99
 *    - Expected total run time: float from 0.1 - 10 quanta
 *    - A priority from 1 - 4, where 1 is the highest
 * 
 * Note: Use FCFS
 * 
 * Written by: Raphael Kusuma
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PROCESSES 26 // "It's ok to create more processes than you use"
#define MAX_QUANTA 100

// For the sake of not having multiple header files, I'm gonna have this large C file.
typedef struct process {
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

// Queue structure for priority queues
typedef struct queue {
  process* processes[NUM_PROCESSES];
  int front;
  int rear;
  int count;
} queue;

// Statistics structure
typedef struct stats {
  float avgTurnaroundTime;
  float avgWaitingTime;
  float avgResponseTime;
  float throughput;
  float cpuUtilization;
  int totalProcesses;
  int totalPreemptions;
} stats;


process processList[NUM_PROCESSES];
int numProcesses = 0;

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

  // Sort processes by arrival time for better scheduling
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

// Queue management functions
void initQueue(queue* q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

void enqueue(queue* q, process* p) {
    if (q->count < NUM_PROCESSES) {
        q->rear = (q->rear + 1) % NUM_PROCESSES;
        q->processes[q->rear] = p;
        q->count++;
    }
}

process* dequeue(queue* q) {
    if (q->count > 0) {
        process* p = q->processes[q->front];
        q->front = (q->front + 1) % NUM_PROCESSES;
        q->count--;
        return p;
    }
    return NULL;
}

int isQueueEmpty(queue* q) {
    return (q->count == 0);
}

void calculateStats(stats* s) {
    float totalTurnaround = 0;
    float totalWaiting = 0;
    float totalResponse = 0;
    int completedProcesses = 0;
    float maxFinishTime = 0;
    
    for (int i = 0; i < numProcesses; i++) {
        if (processList[i].finishTime >= 0) {
            totalTurnaround += processList[i].turnaroundTime;
            totalWaiting += processList[i].waitingTime;
            
            // Response time = start time - arrival time
            float responseTime = processList[i].startTime - processList[i].arrivalTime;
            totalResponse += responseTime;
            
            completedProcesses++;
            
            if (processList[i].finishTime > maxFinishTime) {
                maxFinishTime = processList[i].finishTime;
            }
        }
    }
    
    s->totalProcesses = completedProcesses;
    s->avgTurnaroundTime = completedProcesses > 0 ? totalTurnaround / completedProcesses : 0;
    s->avgWaitingTime = completedProcesses > 0 ? totalWaiting / completedProcesses : 0;
    s->avgResponseTime = completedProcesses > 0 ? totalResponse / completedProcesses : 0;
    s->throughput = maxFinishTime > 0 ? completedProcesses / maxFinishTime : 0;
    s->totalPreemptions = 0; // Non-preemptive has no preemptions
}

void printStats(stats* s, const char* algorithmName) {
    printf("\n=== %s Statistics ===\n", algorithmName);
    printf("Total Processes Completed: %d\n", s->totalProcesses);
    printf("Avg Turnaround Time: %.2f quanta\n", s->avgTurnaroundTime);
    printf("Avg Waiting Time: %.2f quanta\n", s->avgWaitingTime);
    printf("Avg Response Time: %.2f quanta\n", s->avgResponseTime);
    printf("Throughput: %.2f processes/quantum\n", s->throughput);
    printf("CPU Utilization: %.2f%%\n", s->cpuUtilization);
}

// HPF Non-Preemptive Scheduling with FCFS within each priority level
void hpf_non_preemptive() {
    queue priorityQueues[4];
    for (int i = 0; i < 4; i++) {
        initQueue(&priorityQueues[i]);
    }
    
    float currentTime = 0;
    int processIndex = 0;
    process* currentProcess = NULL;
    stats schedulerStats = {0};
    float totalIdleTime = 0;
    int consecutiveIdleTime = 0;
    
    printf("\n=== HPF Non-Preemptive Scheduling (with FCFS within priorities) ===\n");
    printf("Time\tProcess\tPriority\tRemaining\tAction\n");
    
    // Run until all processes are completed
    while (1) {
        // Add arriving processes to appropriate priority queues (only those arriving before quantum 100)
        while (processIndex < numProcesses && 
               processList[processIndex].arrivalTime <= currentTime && 
               processList[processIndex].arrivalTime < MAX_QUANTA) {
            
            int priority = processList[processIndex].priority - 1;
            enqueue(&priorityQueues[priority], &processList[processIndex]);
            printf("%.1f\t%c\tP%d\t\t%.1f\t\tArrived\n", 
                   currentTime, processList[processIndex].processName, 
                   processList[processIndex].priority, processList[processIndex].remainingTime);
            processIndex++;
        }
        
        // Select next process if no current process (non-preemptive FCFS within each priority)
        if (currentProcess == NULL) {
            // Check priority queues from highest to lowest priority
            for (int i = 0; i < 4; i++) {
                if (!isQueueEmpty(&priorityQueues[i])) {
                    currentProcess = dequeue(&priorityQueues[i]); // FCFS within this priority level
                    if (currentProcess->startTime < 0) {
                        currentProcess->startTime = currentTime;
                    }
                    printf("%.1f\t%c\tP%d\t\t%.1f\t\tStarted\n", 
                           currentTime, currentProcess->processName, 
                           currentProcess->priority, currentProcess->remainingTime);
                    consecutiveIdleTime = 0;
                    break;
                }
            }
        }
        
        if (currentProcess != NULL) {
            // Execute process for 1 quantum (non-preemptive)
            currentProcess->remainingTime -= 1.0f;
            
            if (currentProcess->remainingTime <= 0) {
                // Process completed
                currentProcess->finishTime = currentTime + 1.0f;
                currentProcess->turnaroundTime = currentProcess->finishTime - currentProcess->arrivalTime;
                currentProcess->waitingTime = currentProcess->turnaroundTime - currentProcess->expectedRunTime;
                
                printf("%.1f\t%c\tP%d\t\t%.1f\t\tCompleted\n", 
                       currentTime + 1.0f, currentProcess->processName, 
                       currentProcess->priority, currentProcess->remainingTime);
                
                currentProcess = NULL;
            }
        } else {
            // CPU is idle
            consecutiveIdleTime++;
            totalIdleTime += 1.0f;
            
            if (consecutiveIdleTime <= 2) {
                printf("%.1f\t-\t-\t\t-\t\tIdle\n", currentTime);
            }
            
            // Break if no more processes can arrive and all queues are empty
            if (processIndex >= numProcesses) {
                int allQueuesEmpty = 1;
                for (int i = 0; i < 4; i++) {
                    if (!isQueueEmpty(&priorityQueues[i])) {
                        allQueuesEmpty = 0;
                        break;
                    }
                }
                if (allQueuesEmpty) {
                    break;
                }
            }
        }
        
        currentTime += 1.0f;
    }
    
    calculateStats(&schedulerStats);
    schedulerStats.cpuUtilization = currentTime > 0 ? ((currentTime - totalIdleTime) / currentTime) * 100.0f : 0;
    printStats(&schedulerStats, "HPF Non-Preemptive (FCFS)");
}

int main() {
    srand(time(NULL));
    
    printf("HPF Non-Preemptive Scheduler Simulation\n");
    printf("=======================================\n");
    
    // Generate processes
    generate_proc();
    
    // Run HPF Non-Preemptive scheduler
    hpf_non_preemptive();
    
    return 0;
}
