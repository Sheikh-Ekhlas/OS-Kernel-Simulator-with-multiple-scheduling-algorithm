/*
 * PROJECT: OS KERNEL SIMULATION
 * FILE: os_sim.h (Header File)
 * DESCRIPTION: Definitions, Structs, and Function Prototypes
 */

#ifndef OS_SIM_H
#define OS_SIM_H

// ==========================================
// SECTION 1: LIBRARIES & CONSTANTS
// ==========================================
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_PROCESSES 10
#define MEMORY_SIZE 1024 

// Process States Enum
typedef enum { 
    NEW, 
    READY, 
    RUNNING, 
    WAITING, 
    TERMINATED 
} ProcessState;

// ==========================================
// SECTION 2: PROCESS CONTROL BLOCK (PCB)
// ==========================================
typedef struct {
    int pid;            // Process ID
    int arrivalTime;    // Time of Arrival
    int burstTime;      // CPU Time Needed
    int priority;       // Higher Value = Higher Priority
    int memoryReq;      // RAM Required (MB)
    ProcessState state; // Current Status
    
    // Scheduling Metrics
    int remainingTime;  // For Round Robin
    int completionTime;
    int waitingTime;
    int turnAroundTime;
} Process;

// ==========================================
// SECTION 3: GLOBAL VARIABLES (EXTERN)
// ==========================================
// These are defined in modules.c, but accessible everywhere
extern Process processTable[MAX_PROCESSES];
extern Process tempTable[MAX_PROCESSES];
extern int processCount;
extern int mainMemory[MEMORY_SIZE];

// ==========================================
// SECTION 4: FUNCTION PROTOTYPES
// ==========================================

// Memory Management
void initializeMemory();
bool allocateMemory(Process *p);
void deallocateMemory(int pid);
void printMemoryMap();

// Process Helpers
void saveState();
void resetProcesses();
void createProcess(int pid, int arrival, int burst, int priority, int memory);
void printMetrics();

// Scheduling Algorithms
void runFCFS();
void runSJF();
void runPriority();
void runRoundRobin(int quantum);

#endif