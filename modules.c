/*
 * PROJECT: OS KERNEL SIMULATION (FINAL PRO MAX ULTRA)
 * FILE: modules.c
 * DESCRIPTION: Added CPU Time Metric
 */

#include "os_sim.h"

// Definitions
Process processTable[MAX_PROCESSES];
Process tempTable[MAX_PROCESSES];
int processCount = 0;
int mainMemory[MEMORY_SIZE]; 
GanttSegment ganttHistory[100];
int ganttIndex = 0;

// ==========================================
// MODULE 1: VISUALIZATION TOOLS
// ==========================================

void printHeader(char* title) {
    printf("\n" ANSI_COLOR_CYAN "============================================\n");
    printf("   %s\n", title);
    printf("============================================" ANSI_COLOR_RESET "\n");
}

void recordGantt(int pid, int start, int end) {
    if(ganttIndex < 100) {
        ganttHistory[ganttIndex].pid = pid;
        ganttHistory[ganttIndex].startTime = start;
        ganttHistory[ganttIndex].endTime = end;
        ganttIndex++;
    }
}

void printGanttChart() {
    printf("\n" ANSI_COLOR_YELLOW "--- GANTT CHART (Timeline) ---" ANSI_COLOR_RESET "\n");
    if(ganttIndex == 0) { printf("No execution history.\n"); return; }

    // Top Line
    printf(" ");
    for(int i=0; i<ganttIndex; i++) {
        printf("-------");
    }
    printf("\n|");

    // Process IDs
    for(int i=0; i<ganttIndex; i++) {
        printf("  P%d  |", ganttHistory[i].pid);
    }

    // Bottom Line
    printf("\n ");
    for(int i=0; i<ganttIndex; i++) {
        printf("-------");
    }
    printf("\n");

    // Time Stamps
    printf("%d", ganttHistory[0].startTime);
    for(int i=0; i<ganttIndex; i++) {
        if(ganttHistory[i].endTime < 10) printf("      %d", ganttHistory[i].endTime);
        else printf("     %d", ganttHistory[i].endTime);
    }
    printf("\n");
}

void printFinalStats(int totalTime, int busyTime) {
    float cpuUtil = 0;
    if(totalTime > 0) cpuUtil = ((float)busyTime / totalTime) * 100;
    
    float avgWait = 0, avgTurn = 0;
    int executed = 0;

    printf("\n" ANSI_COLOR_GREEN "--- PERFORMANCE METRICS ---" ANSI_COLOR_RESET "\n");
    printf("| PID | Waiting | Turnaround | State |\n");
    printf("|-----|---------|------------|-------|\n");

    for(int i=0; i<processCount; i++) {
        if(processTable[i].state == TERMINATED) {
            printf("| %3d | %7d | %10d | DONE  |\n", 
                processTable[i].pid, processTable[i].waitingTime, processTable[i].turnAroundTime);
            avgWait += processTable[i].waitingTime;
            avgTurn += processTable[i].turnAroundTime;
            executed++;
        }
    }
    
    if(executed > 0) {
        printf("--------------------------------------\n");
        printf("Total Simulation Time:   %d sec\n", totalTime);
        printf("Total CPU Busy Time:     %d sec\n", busyTime);  // <-- NEW LINE ADDED HERE
        printf("Average Waiting Time:    %.2f sec\n", avgWait/executed);
        printf("Average Turnaround Time: %.2f sec\n", avgTurn/executed);
        printf("CPU Utilization:         %.2f%%\n", cpuUtil);
    }
}

// ==========================================
// MODULE 2: MEMORY MANAGEMENT
// ==========================================

void initializeMemory() {
    for(int i = 0; i < MEMORY_SIZE; i++) mainMemory[i] = 0;
}

void printVisualMemory() {
    int used = 0;
    for(int i=0; i<MEMORY_SIZE; i++) if(mainMemory[i] != 0) used++;
    float percent = (float)used / MEMORY_SIZE * 100;

    printf("\n[RAM] Usage: %.1f%% [", percent);
    int bars = (int)(percent / 5); 
    for(int i=0; i<20; i++) {
        if(i < bars) printf("#");
        else printf(".");
    }
    printf("] (%d/%d MB)\n", used, MEMORY_SIZE);
}

bool allocateMemory(Process *p) {
    int freeCount = 0, start = -1;
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (mainMemory[i] == 0) {
            if (start == -1) start = i;
            freeCount++;
        } else {
            start = -1;
            freeCount = 0;
        }
        if (freeCount == p->memoryReq) {
            for (int j = start; j < start + freeCount; j++) mainMemory[j] = p->pid;
            return true;
        }
    }
    return false;
}

void deallocateMemory(int pid) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (mainMemory[i] == pid) mainMemory[i] = 0;
    }
}

// ==========================================
// MODULE 3: PROCESS HELPERS
// ==========================================

void saveState() {
    for(int i=0; i<processCount; i++) tempTable[i] = processTable[i];
}

void resetProcesses() {
    initializeMemory();
    ganttIndex = 0; 
    for(int i=0; i<processCount; i++) {
        processTable[i] = tempTable[i];
        processTable[i].state = NEW;
        processTable[i].remainingTime = processTable[i].burstTime;
        processTable[i].waitingTime = 0;
        processTable[i].turnAroundTime = 0;
    }
}

void createProcess(int pid, int arrival, int burst, int priority, int memory) {
    if (processCount >= MAX_PROCESSES) return;
    Process p = {pid, arrival, burst, priority, memory, NEW, burst, 0, 0, 0};
    processTable[processCount] = p;
    processCount++;
    printf(ANSI_COLOR_GREEN "[LOG] Process %d Created.\n" ANSI_COLOR_RESET, pid);
}

// ==========================================
// MODULE 4: SCHEDULING ALGORITHMS
// ==========================================

void runFCFS() {
    printHeader("RUNNING FCFS SCHEDULER");
    for (int i = 0; i < processCount - 1; i++) {
        for (int j = 0; j < processCount - i - 1; j++) {
            if (processTable[j].arrivalTime > processTable[j+1].arrivalTime) {
                Process temp = processTable[j]; processTable[j] = processTable[j+1]; processTable[j+1] = temp;
            }
        }
    }

    int currentTime = 0;
    int busyTime = 0;

    for(int i = 0; i < processCount; i++) {
        Process *p = &processTable[i];
        if (currentTime < p->arrivalTime) currentTime = p->arrivalTime;

        if (allocateMemory(p)) {
            printVisualMemory();
            printf("Time %d: Process %d Started (Burst %d)\n", currentTime, p->pid, p->burstTime);
            
            recordGantt(p->pid, currentTime, currentTime + p->burstTime); 
            
            p->waitingTime = currentTime - p->arrivalTime;
            currentTime += p->burstTime;
            busyTime += p->burstTime;
            p->turnAroundTime = currentTime - p->arrivalTime;
            p->state = TERMINATED;
            deallocateMemory(p->pid);
        } else {
            printf(ANSI_COLOR_RED "Time %d: Process %d FAILED (Memory Full)\n" ANSI_COLOR_RESET, currentTime, p->pid);
        }
    }
    printGanttChart();
    printFinalStats(currentTime, busyTime);
}

void runSJF() {
    printHeader("RUNNING SJF SCHEDULER");
    int currentTime = 0, completed = 0, busyTime = 0;
    
    while(completed < processCount) {
        int idx = -1;
        int minBurst = INT_MAX;
        
        for(int i=0; i<processCount; i++) {
            if(processTable[i].arrivalTime <= currentTime && processTable[i].state == NEW) {
                if(processTable[i].burstTime < minBurst) {
                     if (allocateMemory(&processTable[i])) {
                         deallocateMemory(processTable[i].pid);
                         minBurst = processTable[i].burstTime;
                         idx = i;
                     }
                }
            }
        }

        if(idx != -1) {
            Process *p = &processTable[idx];
            allocateMemory(p);
            printVisualMemory();
            
            recordGantt(p->pid, currentTime, currentTime + p->burstTime);
            
            p->waitingTime = currentTime - p->arrivalTime;
            printf("Time %d: Process %d Started (Burst %d)\n", currentTime, p->pid, p->burstTime);
            currentTime += p->burstTime;
            busyTime += p->burstTime;
            p->turnAroundTime = currentTime - p->arrivalTime;
            p->state = TERMINATED;
            deallocateMemory(p->pid);
            completed++;
        } else {
             bool pending = false;
             for(int i=0; i<processCount; i++) if(processTable[i].state == NEW) pending = true;
             if(!pending) break;
             currentTime++;
        }
    }
    printGanttChart();
    printFinalStats(currentTime, busyTime);
}

void runPriority() {
    printHeader("RUNNING PRIORITY SCHEDULER");
    int currentTime = 0, completed = 0, busyTime = 0;
    
    while(completed < processCount) {
        int idx = -1, maxPriority = -1;
        for(int i=0; i<processCount; i++) {
            if(processTable[i].arrivalTime <= currentTime && processTable[i].state == NEW) {
                if(processTable[i].priority > maxPriority) {
                     if (allocateMemory(&processTable[i])) {
                         deallocateMemory(processTable[i].pid);
                         maxPriority = processTable[i].priority;
                         idx = i;
                     }
                }
            }
        }
        if(idx != -1) {
            Process *p = &processTable[idx];
            allocateMemory(p);
            printVisualMemory();

            recordGantt(p->pid, currentTime, currentTime + p->burstTime); 

            p->waitingTime = currentTime - p->arrivalTime;
            printf("Time %d: Process %d Started (Priority %d)\n", currentTime, p->pid, p->priority);
            currentTime += p->burstTime;
            busyTime += p->burstTime;
            p->turnAroundTime = currentTime - p->arrivalTime;
            p->state = TERMINATED;
            deallocateMemory(p->pid);
            completed++;
        } else {
             bool pending = false;
             for(int i=0; i<processCount; i++) if(processTable[i].state == NEW) pending = true;
             if(!pending) break;
             currentTime++;
        }
    }
    printGanttChart();
    printFinalStats(currentTime, busyTime);
}

void runRoundRobin(int quantum) {
    printHeader("RUNNING ROUND ROBIN SCHEDULER");
    int currentTime = 0, completed = 0, busyTime = 0;
    
    // Sort
    for (int i = 0; i < processCount - 1; i++) {
        for (int j = 0; j < processCount - i - 1; j++) {
            if (processTable[j].arrivalTime > processTable[j+1].arrivalTime) {
                Process temp = processTable[j]; processTable[j] = processTable[j+1]; processTable[j+1] = temp;
            }
        }
    }
    
    // Load Memory
    for(int i=0; i<processCount; i++) {
        if(allocateMemory(&processTable[i])) processTable[i].state = READY; 
    }
    printVisualMemory();

    while(completed < processCount) {
        bool workDone = false;
        for(int i=0; i<processCount; i++) {
            Process *p = &processTable[i];
            if(p->state == READY && p->arrivalTime <= currentTime && p->remainingTime > 0) {
                workDone = true;
                int executeTime = (p->remainingTime > quantum) ? quantum : p->remainingTime;
                
                printf("Time %d: Process %d runs for %d sec\n", currentTime, p->pid, executeTime);
                recordGantt(p->pid, currentTime, currentTime + executeTime); 

                currentTime += executeTime;
                busyTime += executeTime;
                p->remainingTime -= executeTime;
                
                if(p->remainingTime == 0) {
                    p->state = TERMINATED;
                    p->completionTime = currentTime;
                    p->turnAroundTime = p->completionTime - p->arrivalTime;
                    p->waitingTime = p->turnAroundTime - p->burstTime;
                    deallocateMemory(p->pid);
                    completed++;
                }
            }
        }
        if(!workDone) {
             bool pending = false;
             for(int i=0; i<processCount; i++) if(processTable[i].state == READY || processTable[i].state == NEW) pending = true;
             if(!pending) break;
             currentTime++;
        }
    }
    printGanttChart();
    printFinalStats(currentTime, busyTime);
}
