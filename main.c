/*
 * PROJECT: OS KERNEL SIMULATION
 * FILE: main.c (Driver)
 * AUTHOR: [Your Name]
 * DESCRIPTION: Main entry point and User Menu
 */

#include "os_sim.h"

// ==========================================
// SECTION 1: MAIN FUNCTION
// ==========================================
int main() {
    printf("--- OS KERNEL SIMULATOR (MODULAR) ---\n");
    
    // 1. Initialize Test Data
    createProcess(1, 0, 8, 2, 200);
    createProcess(2, 1, 4, 3, 200); 
    createProcess(3, 2, 9, 1, 300); 
    createProcess(4, 3, 5, 2, 100);

    saveState(); // Backup data for resetting
    
    int choice, quantum;
    
    // 2. Main Menu Loop
    while(1) {
        printf("\nSelect Scheduling Algorithm:\n");
        printf("1. FCFS\n2. SJF\n3. Priority\n4. Round Robin\n5. Exit\nChoice: ");
        scanf("%d", &choice);
        
        resetProcesses(); // Clean memory and reset states
        
        switch(choice) {
            case 1: 
                runFCFS(); 
                printMetrics(); 
                break;
            case 2: 
                runSJF(); 
                printMetrics(); 
                break;
            case 3: 
                runPriority(); 
                printMetrics(); 
                break;
            case 4: 
                printf("Enter Time Quantum: ");
                scanf("%d", &quantum);
                runRoundRobin(quantum); 
                printMetrics(); 
                break;
            case 5: 
                exit(0);
            default: 
                printf("Invalid Choice! Please Try Again.\n");
        }
    }
    return 0;
}