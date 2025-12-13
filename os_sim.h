/*
 * PROJECT: OS KERNEL SIMULATION (STRICT FLOW EDITION)
 * FILE: os_sim.h
 */

#ifndef OS_SIM_H
#define OS_SIM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_PROCESSES 10
#define MEMORY_SIZE 1024 

// ANSI Colors for Professional Output
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

// Process States [cite: 29]
typedef enum { NEW, READY, RUNNING, TERMINATED } ProcessState;

typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;
    int memoryReq;
    ProcessState state;
    
    int remainingTime;  
    int waitingTime;
    int turnAroundTime;
} Process;

// Gantt Chart Helper
typedef struct { int pid; int startTime; int endTime; } GanttSegment;

extern Process processTable[MAX_PROCESSES];
extern Process tempTable[MAX_PROCESSES];
extern int processCount;
extern int mainMemory[MEMORY_SIZE];
extern GanttSegment ganttHistory[100];
extern int ganttIndex;

// --- FUNCTIONS ---

// Memory Management [cite: 50]
void initializeMemory();
bool allocateMemory(Process *p);
void deallocateMemory(int pid);
void printVisualMemory(); 

// Process Management (CRUD)
void createProcess(int pid, int arrival, int burst, int priority, int memory);
void deleteProcess(int pid);      // Missing Feature Added
void updateProcess(int pid);      // Missing Feature Added
void viewProcessTable();          // "Checking" Feature Added
void saveState();
void resetProcesses();

// Visualization & Logging
void printHeader(char* title);
void logStateChange(Process *p, ProcessState newState); // 
void recordGantt(int pid, int start, int end);
void printGanttChart(); // [cite: 31]
void printFinalStats(int totalTime, int busyTime);

// Schedulers
void runFCFS();
void runSJF();
void runPriority();
void runRoundRobin(int quantum);

#endif
