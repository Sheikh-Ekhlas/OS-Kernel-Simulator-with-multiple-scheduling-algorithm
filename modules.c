/*
 * PROJECT: OS KERNEL SIMULATION
 * FILE: modules.c (Implementation)
 * DESCRIPTION: Contains Logic for Memory, Process Mgmt, and Scheduling
 */

#include "os_sim.h" // Header include karna zaroori hai

// ==========================================
// SECTION 1: VARIABLE DEFINITIONS
// ==========================================
Process processTable[MAX_PROCESSES];
Process tempTable[MAX_PROCESSES];
int processCount = 0;
int mainMemory[MEMORY_SIZE]; 

// ==========================================
// SECTION 2: MEMORY MANAGEMENT MODULE
// ==========================================

// Initialize RAM with 0 (Empty)
void initializeMemory() {
    for(int i = 0; i < MEMORY_SIZE; i++) mainMemory[i] = 0;
}

// First-Fit Algorithm: Finds the first continuous block of free memory
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
        
        // If block found
        if (freeCount == p->memoryReq) {
            for (int j = start; j < start + freeCount; j++) mainMemory[j] = p->pid;
            return true;
        }
    }
    return false; // Not enough memory
}

// Releases memory occupied by a process
void deallocateMemory(int pid) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (mainMemory[i] == pid) mainMemory[i] = 0;
    }
}

void printMemoryMap() {
    printf("\n--- VISUAL MEMORY MAP (Simulated RAM) ---\n");
    int currentPid = mainMemory[0];
    int start = 0;

    for (int i = 1; i < MEMORY_SIZE; i++) {
        if (mainMemory[i] != currentPid) {
            // Jab Process ID change ho, pichla block print karo
            if (currentPid == 0) 
                printf("[ Addr %4d - %4d ] : [ FREE SPACE ]\n", start, i-1);
            else 
                printf("[ Addr %4d - %4d ] : [ PROCESS %d  ]\n", start, i-1, currentPid);
            
            currentPid = mainMemory[i];
            start = i;
        }
    }
    if (currentPid == 0) 
        printf("[ Addr %4d - %4d ] : [ FREE SPACE ]\n", start, MEMORY_SIZE-1);
    else 
        printf("[ Addr %4d - %4d ] : [ PROCESS %d  ]\n", start, MEMORY_SIZE-1, currentPid);
    printf("---------------------------------------------\n");
}

// ==========================================
// SECTION 3: PROCESS HELPER MODULE
// ==========================================

// Saves initial state to support resetting for multiple algorithms
void saveState() {
    for(int i=0; i<processCount; i++) tempTable[i] = processTable[i];
}

// Resets processes to 'NEW' state for the next run
void resetProcesses() {
    initializeMemory();
    for(int i=0; i<processCount; i++) {
        processTable[i] = tempTable[i];
        processTable[i].state = NEW;
        processTable[i].remainingTime = processTable[i].burstTime;
        processTable[i].waitingTime = 0;
        processTable[i].turnAroundTime = 0;
    }
}

// Creates a new process and adds it to the table
void createProcess(int pid, int arrival, int burst, int priority, int memory) {
    if (processCount >= MAX_PROCESSES) return;
    Process p = {pid, arrival, burst, priority, memory, NEW, burst, 0, 0, 0};
    processTable[processCount] = p;
    processCount++;
}

// Prints final table and average times
void printMetrics() {
    printf("\n-------------------------------------------------\n");
    printf("| PID | Priority | Waiting | Turnaround | State |\n");
    printf("-------------------------------------------------\n");
    float totalWait = 0, totalTurn = 0;
    int executed = 0;
    for (int i = 0; i < processCount; i++) {
        if (processTable[i].state == TERMINATED) {
            printf("| %3d | %8d | %7d | %10d | DONE  |\n", 
                processTable[i].pid, processTable[i].priority, 
                processTable[i].waitingTime, processTable[i].turnAroundTime);
            totalWait += processTable[i].waitingTime;
            totalTurn += processTable[i].turnAroundTime;
            executed++;
        } else {
            printf("| %3d | %8d | FAILED  |   FAILED   | NO MEM|\n", processTable[i].pid, processTable[i].priority);
        }
    }
    if(executed > 0)
        printf("\nAverage Waiting Time: %.2f\nAverage Turnaround Time: %.2f\n", totalWait/executed, totalTurn/executed);
}

// ==========================================
// SECTION 4: SCHEDULING ALGORITHMS
// ==========================================

// Algorithm 1: First Come First Serve (FCFS)
void runFCFS() {
    printf("\n[ RUNNING FCFS... ]\n");

    int currentTime = 0;
    for(int i = 0; i < processCount; i++) {
        Process *p = &processTable[i];
        
        if (currentTime < p->arrivalTime) currentTime = p->arrivalTime;
        
        if (allocateMemory(p)) {
            printf("\n--- Memory State at Time %d ---\n", currentTime);
            printMemoryMap(); 
            
            printf("Time %d: Process %d (Burst %d) Started.\n", currentTime, p->pid, p->burstTime);
            
            p->waitingTime = currentTime - p->arrivalTime;
            currentTime += p->burstTime;
            p->turnAroundTime = currentTime - p->arrivalTime;
            p->state = TERMINATED;
            deallocateMemory(p->pid);
            printf("Time %d: Process %d Finished.\n", currentTime, p->pid);
        } else {
            printf("Time %d: Process %d FAILED MEMORY.\n", currentTime, p->pid);
        }
    }
}

// Algorithm 2: Shortest Job First (SJF - Non Preemptive)
void runSJF() {
    printf("\n[ RUNNING SHORTEST JOB FIRST... ]\n");
    int currentTime = 0, completed = 0;
    
    while(completed < processCount) {
        int idx = -1;
        int minBurst = INT_MAX;

        // Select shortest job that has arrived
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
            printf("\n--- Memory State for Process %d ---\n", p->pid);
            printMemoryMap(); 
            p->waitingTime = currentTime - p->arrivalTime;
            printf("Time %d: Process %d (Burst %d) Started.\n", currentTime, p->pid, p->burstTime);
            currentTime += p->burstTime;
            p->turnAroundTime = currentTime - p->arrivalTime;
            p->state = TERMINATED;
            deallocateMemory(p->pid);
            completed++;
        } else {
             // Handle Idle or Finish
             bool pending = false;
             for(int i=0; i<processCount; i++) if(processTable[i].state == NEW) pending = true;
             if(!pending) break;
             currentTime++;
        }
    }
}

// Algorithm 3: Priority Scheduling (Higher # = Higher Priority)
void runPriority() {
    printf("\n[ RUNNING PRIORITY SCHEDULING... ]\n");
    int currentTime = 0, completed = 0;
    
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
            printf("\n--- Memory State for Process %d ---\n", p->pid);
            printMemoryMap(); 
            p->waitingTime = currentTime - p->arrivalTime;
            printf("Time %d: Process %d (Priority %d) Started.\n", currentTime, p->pid, p->priority);
            currentTime += p->burstTime;
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
}

// Algorithm 4: Round Robin (Preemptive)
void runRoundRobin(int quantum) {
    printf("\n[ RUNNING ROUND ROBIN (Quantum: %d)... ]\n", quantum);
    int currentTime = 0, completed = 0;
    
    // Initial Sort by Arrival
    for (int i = 0; i < processCount - 1; i++) {
        for (int j = 0; j < processCount - i - 1; j++) {
            if (processTable[j].arrivalTime > processTable[j+1].arrivalTime) {
                Process temp = processTable[j]; processTable[j] = processTable[j+1]; processTable[j+1] = temp;
            }
        }
    }
    
    // Load processes into memory (Simplified for RR)
    for(int i=0; i<processCount; i++) {
        if(allocateMemory(&processTable[i])) processTable[i].state = READY; 
    }
    printf("\n--- Initial Memory State (All Loaded) ---\n");
    printMemoryMap();
    while(completed < processCount) {
        bool workDone = false;
        for(int i=0; i<processCount; i++) {
            Process *p = &processTable[i];
            
            if(p->state == READY && p->arrivalTime <= currentTime && p->remainingTime > 0) {
                workDone = true;
                int executeTime = (p->remainingTime > quantum) ? quantum : p->remainingTime;
                
                printf("Time %d: Process %d runs for %d sec.\n", currentTime, p->pid, executeTime);
                currentTime += executeTime;
                p->remainingTime -= executeTime;
                
                if(p->remainingTime == 0) {
                    p->state = TERMINATED;
                    p->completionTime = currentTime;
                    p->turnAroundTime = p->completionTime - p->arrivalTime;
                    p->waitingTime = p->turnAroundTime - p->burstTime;
                    deallocateMemory(p->pid);
                    completed++;
                    printf("--- Process %d Finished at %d ---\n", p->pid, currentTime);
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
}