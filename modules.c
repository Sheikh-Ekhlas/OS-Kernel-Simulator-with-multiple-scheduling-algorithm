/*
 * PROJECT: OS KERNEL SIMULATION (STRICT FLOW EDITION)
 * FILE: modules.c
 */

#include "os_sim.h"

Process processTable[MAX_PROCESSES];
Process tempTable[MAX_PROCESSES];
int processCount = 0;
int mainMemory[MEMORY_SIZE]; 
GanttSegment ganttHistory[100];
int ganttIndex = 0;

// --- VISUALIZATION HELPERS ---

void printHeader(char* title) {
    printf("\n" CYAN "============================================\n");
    printf("   %s\n", title);
    printf("============================================" RESET "\n");
}

const char* getStateName(ProcessState s) {
    switch(s) {
        case NEW: return "NEW";
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}

//  - Log State Transitions
void logStateChange(Process *p, ProcessState newState) {
    printf(YELLOW "[STATE] PID %d: %s -> %s\n" RESET, 
           p->pid, getStateName(p->state), getStateName(newState));
    p->state = newState;
}

// --- MEMORY MODULE [cite: 65, 86] ---

void initializeMemory() {
    for(int i = 0; i < MEMORY_SIZE; i++) mainMemory[i] = 0;
}

void printVisualMemory() {
// 1. Calculate Actual RAM Usage (Real-time)
    int used = 0;
    for(int i=0; i<MEMORY_SIZE; i++) if(mainMemory[i] != 0) used++;
    
    // 2. Calculate Projected Requirement (Forecast)
    int totalNeeded = 0;
    for(int i=0; i<processCount; i++) {
        totalNeeded += processTable[i].memoryReq;
    }

    float percent = (float)used / MEMORY_SIZE * 100;
    float projectedPercent = (float)totalNeeded / MEMORY_SIZE * 100;

    printf("\n" CYAN "--- MEMORY DIAGNOSTICS ---" RESET "\n");
    printf("[REAL-TIME RAM] Currently Occupied:  %d MB (%.1f%%)\n", used, percent);
    printf("[PROCESS LOAD]  Total Memory Needed: %d MB (%.1f%%)\n", totalNeeded, projectedPercent);
    
    if(totalNeeded > MEMORY_SIZE) {
        printf(RED "[WARNING] System Overload! Processes require more RAM than available.\n" RESET);
    } else {
        printf(GREEN "[STATUS] Safe. Sufficient Memory Available.\n" RESET);
    }
    printf("--------------------------\n");    
}

bool allocateMemory(Process *p) {
    int freeCount = 0, start = -1;
    printf(BLUE "[ALLOCATION] Checking memory for PID %d (%d MB)...\n" RESET, p->pid, p->memoryReq);

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
            printf(GREEN "[SUCCESS] Allocated Address %d-%d to PID %d\n" RESET, start, start+freeCount-1, p->pid);
            return true;
        }
    }
    printf(RED "[FAILED] Not enough contiguous memory for PID %d\n" RESET, p->pid);
    return false;
}

void deallocateMemory(int pid) { // [cite: 87]
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (mainMemory[i] == pid) mainMemory[i] = 0;
    }
    printf(BLUE "[DEALLOCATION] Memory freed for PID %d\n" RESET, pid);
}

// --- PROCESS MANAGEMENT (CRUD) ---

void createProcess(int pid, int arrival, int burst, int priority, int memory) {
    if (processCount >= MAX_PROCESSES) { printf(RED "Max Limit Reached!\n" RESET); return; }
    // Check Duplicate
    for(int i=0; i<processCount; i++) if(processTable[i].pid == pid) { printf(RED "PID Exists!\n" RESET); return; }

    Process p = {pid, arrival, burst, priority, memory, NEW, burst, 0, 0};
    processTable[processCount] = p;
    processCount++;
    printf(GREEN "Process %d Created.\n" RESET, pid);
}

void deleteProcess(int pid) {
    int idx = -1;
    for(int i=0; i<processCount; i++) if(processTable[i].pid == pid) idx = i;
    
    if(idx == -1) { printf(RED "PID Not Found!\n" RESET); return; }
    
    // Shift remaining
    for(int i=idx; i<processCount-1; i++) processTable[i] = processTable[i+1];
    processCount--;
    printf(RED "Process %d Deleted.\n" RESET, pid);
}

void updateProcess(int pid) {
    int idx = -1;
    for(int i=0; i<processCount; i++) if(processTable[i].pid == pid) idx = i;
    
    if(idx == -1) { printf(RED "PID Not Found!\n" RESET); return; }
    
    printf("Updating PID %d (Enter -1 to keep current):\n", pid);
    int input;
    
    printf("Burst Time (Current: %d): ", processTable[idx].burstTime);
    scanf("%d", &input); if(input != -1) processTable[idx].burstTime = input;

    printf("Priority (Current: %d): ", processTable[idx].priority);
    scanf("%d", &input); if(input != -1) processTable[idx].priority = input;

    printf("Memory (Current: %d): ", processTable[idx].memoryReq);
    scanf("%d", &input); if(input != -1) processTable[idx].memoryReq = input;
    
    printf(GREEN "Process Updated.\n" RESET);
}

void viewProcessTable() {
    printf("\n--- CURRENT PROCESS QUEUE ---\n");
    printf("PID\tBurst\tPrio\tMem\tState\n");
    for(int i=0; i<processCount; i++) {
        printf("%d\t%d\t%d\t%d\t%s\n", 
            processTable[i].pid, processTable[i].burstTime, processTable[i].priority, 
            processTable[i].memoryReq, getStateName(processTable[i].state));
    }
    printf("-----------------------------\n");
}

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

// --- GANTT & STATS [cite: 31, 268] ---

void recordGantt(int pid, int start, int end) {
    if(ganttIndex < 100) {
        ganttHistory[ganttIndex] = (GanttSegment){pid, start, end};
        ganttIndex++;
    }
}

void printGanttChart() {
    printf("\n" YELLOW "--- GANTT CHART ---\n" RESET);
    if(ganttIndex==0) return;
    
    printf("|");
    for(int i=0; i<ganttIndex; i++) printf("  P%d  |", ganttHistory[i].pid);
    printf("\n");
    
    printf("%d", ganttHistory[0].startTime);
    for(int i=0; i<ganttIndex; i++) printf("      %d", ganttHistory[i].endTime);
    printf("\n");
}

void printFinalStats(int totalTime, int busyTime) {
    float cpuUtil = totalTime > 0 ? ((float)busyTime/totalTime)*100 : 0;
    float avgWait = 0, avgTurn = 0;
    int executed = 0;

    printf("\n" GREEN "--- FINAL STATISTICS ---\n" RESET);
    // Added 'Burst' column here for verification
    printf("PID\tBurst\tWait\tTurnaround\n");
    
    for(int i=0; i<processCount; i++) {
        if(processTable[i].state == TERMINATED) {
            printf("%d\t%d\t%d\t%d\n", 
                processTable[i].pid, 
                processTable[i].burstTime, // Shows individual CPU time
                processTable[i].waitingTime, 
                processTable[i].turnAroundTime);
            
            avgWait += processTable[i].waitingTime;
            avgTurn += processTable[i].turnAroundTime;
            executed++;
        }
    }

    if(executed > 0) {
        printf("--------------------------------------\n");
        // Shows Global CPU Stats
        printf("Total Simulation Time:   %d sec\n", totalTime);
        printf("Total CPU Busy Time:     %d sec\n", busyTime);
        printf("CPU Utilization:         %.2f%%\n", cpuUtil);
        printf("Average Waiting Time:    %.2f sec\n", avgWait/executed);
        printf("Average Turnaround Time: %.2f sec\n", avgTurn/executed);
    }
}

// --- SCHEDULERS (Example FCFS & RR - Others similar) ---

void runFCFS() {
    printHeader("RUNNING FCFS");
    // Sort by Arrival Time
    for (int i = 0; i < processCount - 1; i++) 
        for (int j = 0; j < processCount - i - 1; j++) 
            if (processTable[j].arrivalTime > processTable[j+1].arrivalTime) {
                Process temp = processTable[j]; processTable[j] = processTable[j+1]; processTable[j+1] = temp;
            }

    int currentTime = 0, busyTime = 0;
    for(int i = 0; i < processCount; i++) {
        Process *p = &processTable[i];
        if (currentTime < p->arrivalTime) currentTime = p->arrivalTime;

        logStateChange(p, READY); // New -> Ready
        
        if (allocateMemory(p)) {
            logStateChange(p, RUNNING); // Ready -> Running
            
            printf("Time %d: Executing PID %d\n", currentTime, p->pid);
            recordGantt(p->pid, currentTime, currentTime + p->burstTime);
            
            p->waitingTime = currentTime - p->arrivalTime;
            currentTime += p->burstTime;
            busyTime += p->burstTime;
            p->turnAroundTime = currentTime - p->arrivalTime;
            
            logStateChange(p, TERMINATED); // Running -> Terminated
            deallocateMemory(p->pid);
        }
    }
    printGanttChart();
    printFinalStats(currentTime, busyTime);
}

void runRoundRobin(int quantum) {
    printHeader("RUNNING ROUND ROBIN");
    // Sort logic here...
    for(int i=0; i<processCount; i++) if(allocateMemory(&processTable[i])) logStateChange(&processTable[i], READY);
    
    int currentTime = 0, completed = 0, busyTime = 0;
    while(completed < processCount) {
        bool workDone = false;
        for(int i=0; i<processCount; i++) {
            Process *p = &processTable[i];
            if(p->state == READY && p->arrivalTime <= currentTime && p->remainingTime > 0) {
                workDone = true;
                logStateChange(p, RUNNING);

                int executeTime = (p->remainingTime > quantum) ? quantum : p->remainingTime;
                printf("Time %d: PID %d runs for %d sec (Remaining Burst: %d)\n", 
                currentTime, p->pid, executeTime, p->remainingTime - executeTime);
                recordGantt(p->pid, currentTime, currentTime + executeTime);
                
                currentTime += executeTime;
                busyTime += executeTime;
                p->remainingTime -= executeTime;

                if(p->remainingTime == 0) {
                    logStateChange(p, TERMINATED);
                    p->turnAroundTime = currentTime - p->arrivalTime;
                    p->waitingTime = p->turnAroundTime - p->burstTime;
                    deallocateMemory(p->pid);
                    completed++;
                } else {
                    logStateChange(p, READY);
                }
            }
        }
        if(!workDone) currentTime++;
    }
    printGanttChart();
    printFinalStats(currentTime, busyTime);
}

// Stubs for others
void runSJF() { 
    printHeader("RUNNING SHORTEST JOB FIRST (SJF)");
    
    // Sort by Burst Time (The Logic of SJF)
    // Note: We also check Arrival Time to keep it realistic
    for (int i = 0; i < processCount - 1; i++) {
        for (int j = 0; j < processCount - i - 1; j++) {
            // Primary Sort: Arrival Time
            if (processTable[j].arrivalTime > processTable[j+1].arrivalTime) {
                Process temp = processTable[j]; processTable[j] = processTable[j+1]; processTable[j+1] = temp;
            }
            // Secondary Sort: If Arrival is same, Sort by Burst
            else if (processTable[j].arrivalTime == processTable[j+1].arrivalTime && 
                     processTable[j].burstTime > processTable[j+1].burstTime) {
                Process temp = processTable[j]; processTable[j] = processTable[j+1]; processTable[j+1] = temp;
            }
        }
    }

    // Standard Execution Loop (Same as FCFS)
    int currentTime = 0, busyTime = 0;
    // (Copy the EXACT loop from runFCFS here, it works perfectly once sorted)
    for(int i = 0; i < processCount; i++) {
        Process *p = &processTable[i];
        if (currentTime < p->arrivalTime) currentTime = p->arrivalTime;

        logStateChange(p, READY); 
        
        if (allocateMemory(p)) {
            logStateChange(p, RUNNING);
            printf("Time %d: Executing PID %d (Burst %d)\n", currentTime, p->pid, p->burstTime);
            recordGantt(p->pid, currentTime, currentTime + p->burstTime);
            
            p->waitingTime = currentTime - p->arrivalTime;
            currentTime += p->burstTime;
            busyTime += p->burstTime;
            p->turnAroundTime = currentTime - p->arrivalTime;
            
            logStateChange(p, TERMINATED);
            deallocateMemory(p->pid);
        }
    }
    printGanttChart();
    printFinalStats(currentTime, busyTime);
}
 
void runPriority() { 
printHeader("RUNNING PRIORITY SCHEDULING");
    
    // Sort by Priority (Higher Number = Higher Priority)
    for (int i = 0; i < processCount - 1; i++) {
        for (int j = 0; j < processCount - i - 1; j++) {
            if (processTable[j].priority < processTable[j+1].priority) { // < for Descending Order
                Process temp = processTable[j]; processTable[j] = processTable[j+1]; processTable[j+1] = temp;
            }
        }
    }
    
    // Standard Execution Loop (Same as FCFS)
    int currentTime = 0, busyTime = 0;
    for(int i = 0; i < processCount; i++) {
        Process *p = &processTable[i];
        if (currentTime < p->arrivalTime) currentTime = p->arrivalTime;

        logStateChange(p, READY); 
        
        if (allocateMemory(p)) {
            logStateChange(p, RUNNING);
            printf("Time %d: Executing PID %d (Priority %d)\n", currentTime, p->pid, p->priority);
            recordGantt(p->pid, currentTime, currentTime + p->burstTime);
            
            p->waitingTime = currentTime - p->arrivalTime;
            currentTime += p->burstTime;
            busyTime += p->burstTime;
            p->turnAroundTime = currentTime - p->arrivalTime;
            
            logStateChange(p, TERMINATED);
            deallocateMemory(p->pid);
        }
    }
    printGanttChart();
    printFinalStats(currentTime, busyTime);    
}
