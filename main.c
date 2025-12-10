/*
 * PROJECT: OS KERNEL SIMULATION (FINAL VERSION)
 * FILE: main.c
 */

#include "os_sim.h"

int main() {
    printHeader("OS KERNEL SIMULATOR v3.0 (FINAL)");
    
    // Default Test Data
    createProcess(1, 0, 8, 2, 200);
    createProcess(2, 1, 4, 3, 200); 
    createProcess(3, 2, 9, 1, 300); 
    createProcess(4, 3, 5, 2, 100);
    
    saveState(); 

    int choice, quantum;
    int pid, arr, burst, prio, mem;

    while(1) {
        printf("\n" ANSI_COLOR_BLUE "=== MAIN MENU ===" ANSI_COLOR_RESET "\n");
        printf("1. FCFS Algorithm\n");
        printf("2. SJF Algorithm\n");
        printf("3. Priority Algorithm\n");
        printf("4. Round Robin Algorithm\n");
        printf("5. Add Custom Process\n");
        printf("6. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        resetProcesses(); 

        switch(choice) {
            case 1: runFCFS(); break;
            case 2: runSJF(); break;
            case 3: runPriority(); break;
            case 4: 
                printf("Enter Time Quantum: ");
                scanf("%d", &quantum);
                runRoundRobin(quantum); 
                break;
            case 5:
                printf("\n--- ADD NEW PROCESS ---\n");
                printf("Enter PID: "); scanf("%d", &pid);
                printf("Arrival Time: "); scanf("%d", &arr);
                printf("Burst Time: "); scanf("%d", &burst);
                printf("Priority: "); scanf("%d", &prio);
                printf("Memory (MB): "); scanf("%d", &mem);
                createProcess(pid, arr, burst, prio, mem);
                saveState(); // Update saved state
                break;
            case 6: exit(0);
            default: printf("Invalid Choice.\n");
        }
    }
    return 0;
}
