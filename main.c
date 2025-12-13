/*
 * PROJECT: OS KERNEL SIMULATION (STRICT FLOW EDITION)
 * FILE: main.c
 */

#include "os_sim.h"

int main() {
    printHeader("OS KERNEL SIMULATOR - COMPLEX ENGINEERING ACTIVITY");
    
    // Default Data
    createProcess(1, 0, 8, 2, 200);
    createProcess(2, 1, 4, 3, 200); 
    saveState();

    int choice, subChoice, pid, arr, burst, prio, mem, quantum;

    while(1) {
        // --- PHASE 1: PROCESS MANAGEMENT (Creation/Deletion/Checking) ---
        printf("\n" BLUE "=== STEP 1: KERNEL PROCESS MANAGER ===" RESET "\n");
        printf("1. Create New Process\n");
        printf("2. Delete Process\n");
        printf("3. Update Process Details\n");
        printf("4. View/Check Process Queue\n");
        printf("5. Check RAM Status (Allocation Check)\n");
        printf("6. " GREEN "PROCEED TO SCHEDULER >>" RESET "\n");
        printf("7. Exit\n");
        printf("Select: ");
        scanf("%d", &choice);

        if(choice == 7) exit(0);

        if(choice == 1) {
            printf("PID: "); scanf("%d", &pid);
            printf("Arrival (>=0): "); scanf("%d", &arr);
            printf("Burst (>0): "); scanf("%d", &burst);
            printf("Priority (1-10): "); scanf("%d", &prio);
            printf("Memory (1-%d MB): ", MEMORY_SIZE); scanf("%d", &mem);
    
          // Validation Check
            if(arr < 0 || burst <= 0 || mem <= 0 || mem > MEMORY_SIZE) {
            printf(RED "Error: Invalid Input Values! Process not created.\n" RESET);
             } else {
                createProcess(pid, arr, burst, prio, mem);
            saveState();
    }
        }
        else if(choice == 2) {
            printf("Enter PID to Delete: "); scanf("%d", &pid);
            deleteProcess(pid);
            saveState();
        }
        else if(choice == 3) {
            printf("Enter PID to Update: "); scanf("%d", &pid);
            updateProcess(pid);
            saveState();
        }
        else if(choice == 4) {
            viewProcessTable();
        }
        else if(choice == 5) {
            printVisualMemory(); // Shows Allocation Status
        }
        else if(choice == 6) {
            // --- PHASE 2: SCHEDULING ALGORITHMS ---
            while(1) {
                printf("\n" YELLOW "=== STEP 2: SCHEDULER EXECUTION ===" RESET "\n");
                printf("1. First Come First Serve (FCFS)\n");
                printf("2. Shortest Job First (SJF)\n");
                printf("3. Priority Scheduling\n");
                printf("4. Round Robin\n");
                printf("5. << Go Back to Process Manager\n");
                printf("Select: ");
                scanf("%d", &subChoice);

                if(subChoice == 5) break; // Go back to Phase 1

                resetProcesses(); // Always reset before run

                if(subChoice == 1) runFCFS();
                else if(subChoice == 2) runSJF();
                else if(subChoice == 3) runPriority();
                else if(subChoice == 4) {
                    printf("Enter Time Quantum: "); scanf("%d", &quantum);
                    runRoundRobin(quantum);
                }
                printf("\n" YELLOW "Simulation Complete. Press Enter to return to Menu..." RESET);
                    getchar(); 
                    getchar();
            }
        }
    }
    return 0;
}
