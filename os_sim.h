/*
 * PROJECT: OS KERNEL SIMULATION (FINAL VERSION)
 * FILE: os_sim.h
 * DESCRIPTION: Header file with Gantt Chart, Colors, and Metrics support.
 */

#ifndef OS_SIM_H
#define OS_SIM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

// --- CONSTANTS ---
#define MAX_PROCESSES 10
#define MEMORY_SIZE 1024 

// --- COLORS FOR VISUALIZATION ---
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// --- ENUMS & STRUCTS ---
typedef enum { NEW, READY, RUNNING, WAITING, TERMINATED } ProcessState;

typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;
    int memoryReq;
    ProcessState state;
    
    // Metrics
    int remainingTime;  
    int completionTime;
    int waitingTime;
    int turnAroundTime;
} Process;

// Struct for Gantt Chart History
typedef struct {
    int pid;
    int startTime;
    int endTime;
} GanttSegment;

// --- GLOBAL VARIABLES ---
extern Process processTable[MAX_PROCESSES];
extern Process tempTable[MAX_PROCESSES];
extern int processCount;
extern int mainMemory[MEMORY_SIZE];
extern GanttSegment ganttHistory[100]; // Stores execution history
extern int ganttIndex;

// --- FUNCTION PROTOTYPES ---

// Memory
void initializeMemory();
bool allocateMemory(Process *p);
void deallocateMemory(int pid);
void printVisualMemory(); // New Feature

// Process Mgmt
void saveState();
void resetProcesses();
void createProcess(int pid, int arrival, int burst, int priority, int memory);

// Visualization & Stats
void printHeader(char* title);
void recordGantt(int pid, int start, int end); // New Feature
void printGanttChart(); // New Feature
void printFinalStats(int totalTime, int busyTime);

// Schedulers
void runFCFS();
void runSJF();
void runPriority();
void runRoundRobin(int quantum);

#endif
