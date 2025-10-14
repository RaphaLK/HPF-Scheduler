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

#define MAX_PROCESSES 167 // "It's ok to create more processes than you use"
#define MAX_QUANTA 100

 typedef struct process {
  float arrivalTime;
  float expectedRunTime;
  int priority;
} process;

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
  process* processes[MAX_PROCESSES];
  int front;
  int rear;
  int count;
} queue;

// Statistics structure
typedef struct stats {
  float avgTurnaroundTime;
  float avgWaitingTime;
  float cpuUtilization;
  int totalProcesses;
  int totalPreemptions;
} stats;


process processList[MAX_PROCESSES];
int numProcesses = 0;

// Queue management functions
void initQueue(queue* q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

void enqueue(queue* q, process* p) {
    if (q->count < MAX_PROCESSES) {
        q->rear = (q->rear + 1) % MAX_PROCESSES;
        q->processes[q->rear] = p;
        q->count++;
    }
}

process* dequeue(queue* q) {
    if (q->count > 0) {
        process* p = q->processes[q->front];
        q->front = (q->front + 1) % MAX_PROCESSES;
        q->count--;
        return p;
    }
    return NULL;
}

int isQueueEmpty(queue* q) {
    return q->count == 0;
}


// HPF Non-Preemptive Scheduling
void hpf_non_preemptive() {
    queue priorityQueues[4];
    for (int i = 0; i < 4; i++) {
        initQueue(&priorityQueues[i]);
    }
    
    float currentTime = 0;
    int processIndex = 0;
    process* currentProcess = NULL;
    stats schedulerStats = {0};
    int idleTime = 0;
    
    printf("\n=== HPF Non-Preemptive Scheduling ===\n");
    printf("Time\tProcess\tPriority\tRemaining\tAction\n");
    
    while (currentTime < MAX_QUANTA * 2) {
        // Add arriving processes to appropriate priority queues
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
        
        // Select next process if no current process (non-preemptive)
        if (currentProcess == NULL) {
            for (int i = 0; i < 4; i++) {
                if (!isQueueEmpty(&priorityQueues[i])) {
                    currentProcess = dequeue(&priorityQueues[i]);
                    if (currentProcess->startTime < 0) {
                        currentProcess->startTime = currentTime;
                    }
                    printf("%.1f\t%c\tP%d\t\t%.1f\t\tStarted\n", 
                           currentTime, currentProcess->processName, 
                           currentProcess->priority, currentProcess->remainingTime);
                    break;
                }
            }
        }
        
        if (currentProcess != NULL) {
            // Execute process for 1 quantum
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
                idleTime = 0;
            }
        } else {
            // CPU is idle
            idleTime++;
            if (idleTime <= 2) {
                printf("%.1f\t-\t-\t\t-\t\tIdle\n", currentTime);
            }
            
            if (idleTime > 10 && processIndex >= numProcesses) {
                break;
            }
        }
        
        currentTime += 1.0f;
    }
    
    calculateStats(&schedulerStats);
    schedulerStats.cpuUtilization = ((currentTime - idleTime) / currentTime) * 100.0f;
    printStats(&schedulerStats, "HPF Non-Preemptive");
}
