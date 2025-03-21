#include <stdio.h>
#include <stdlib.h>

// Structure to store process details
struct Process {
    int PID;           
    int Arrival_Time;  
    int Burst_Time;    
    int Priority;     
};

// Function prototypes
void fcfs(struct Process *P, int process_count);
void sjf(struct Process *P, int process_count);

// Function to compare processes based on Arrival Time (used in sorting)
int compareArrivalTime(const void *a, const void *b) {
    struct Process *p1 = (struct Process *)a;
    struct Process *p2 = (struct Process *)b;
    return p1->Arrival_Time - p2->Arrival_Time;
}

// Function to display Gantt chart
void printGanttChart(int process_order[], int process_times[], int count, int start_time) {
    printf("\nGantt Chart:\n|");
    for (int i = 0; i < count; i++) {
        printf(" P%-2d |", process_order[i]);
    }
    printf("\n%-2d", start_time);
    for (int i = 0; i < count; i++) {
        printf("    %-2d", process_times[i]); 
    }
    printf("\n");
}

// Function to display the process table before scheduling
void displayProcessTable(struct Process *P, int process_count) {
    printf("\nInitial Process Table:\n");
    printf("PID  Arrival  Burst  Priority\n");
    printf("---  -------  -----  --------\n");
    for (int i = 0; i < process_count; i++) {
        printf("%-4d %-8d %-6d %-8d\n", P[i].PID, P[i].Arrival_Time, P[i].Burst_Time, P[i].Priority);
    }
}

// First Come, First Served (FCFS) Scheduling Algorithm
void fcfs(struct Process *P, int process_count) {
    int t = 0; // Current time tracker
    float total_wt = 0, total_tat = 0;

    // Sort processes by arrival time
    qsort(P, process_count, sizeof(struct Process), compareArrivalTime);

    int process_order[process_count]; // Execution order of processes
    int process_times[process_count]; // Completion times of processes

    printf("\nFCFS Scheduling Results:\n");
    printf("PID  Arrival  Burst  Completion  Waiting  Turnaround\n");
    printf("---  -------  -----  ----------  -------  ----------\n");

    for (int i = 0; i < process_count; i++) {
        if (t < P[i].Arrival_Time) {
            t = P[i].Arrival_Time;  // If CPU is idle, move time forward
        }

        
        // Calculate completion, turnaround, and waiting times
        int completion_time = t + P[i].Burst_Time;
        int turnaround_time = completion_time - P[i].Arrival_Time;
        int waiting_time = turnaround_time - P[i].Burst_Time;

        total_wt += waiting_time;
        total_tat += turnaround_time;

        process_order[i] = P[i].PID;          // Store execution order
        process_times[i] = completion_time;   // Store completion times

        printf("%-4d %-8d %-6d %-10d %-8d %-10d\n",
               P[i].PID, P[i].Arrival_Time, P[i].Burst_Time,
               completion_time, waiting_time, turnaround_time);

        t = completion_time;  // Move current time to completion time
    }

    printf("\nAverage Waiting Time: %.2f", total_wt / process_count);
    printf("\nAverage Turnaround Time: %.2f\n", total_tat / process_count);

    printGanttChart(process_order, process_times, process_count, P[0].Arrival_Time);
}

// Shortest Job First (SJF) Scheduling Algorithm
void sjf(struct Process *P, int process_count) {
    int current_time = 0, completed = 0;
    int visited[process_count];  // Tracks whether a process has been scheduled
    float total_wt = 0, total_tat = 0;

    int process_order[process_count]; // Stores scheduled order of processes
    int process_times[process_count]; // Stores completion times of processes

    // Initialize visited array
    for (int i = 0; i < process_count; i++) {
        visited[i] = 0;
    }

    // Sort processes by arrival time
    qsort(P, process_count, sizeof(struct Process), compareArrivalTime);

    printf("\nSJF Scheduling Results:\n");
    printf("PID  Arrival  Burst  Completion  Waiting  Turnaround\n");
    printf("---  -------  -----  ----------  -------  ----------\n");

    while (completed < process_count) {
        int shortest_index = -1;
        int shortest_burst = __INT_MAX__;

        // Find the shortest job available at the current time
        for (int i = 0; i < process_count; i++) {
            if (!visited[i] && P[i].Arrival_Time <= current_time) {
                if (P[i].Burst_Time < shortest_burst) {
                    shortest_burst = P[i].Burst_Time;
                    shortest_index = i;
                }
            }
        }

        if (shortest_index == -1) {
            current_time++;  // If no process is available, move time forward
            continue;
        }

        // Calculate completion, turnaround, and waiting times
        int completion_time = current_time + P[shortest_index].Burst_Time;
        int turnaround_time = completion_time - P[shortest_index].Arrival_Time;
        int waiting_time = turnaround_time - P[shortest_index].Burst_Time;

        total_wt += waiting_time;
        total_tat += turnaround_time;

        // Store execution order and times
        process_order[completed] = P[shortest_index].PID;
        process_times[completed] = completion_time;

        printf("%-4d %-8d %-6d %-10d %-8d %-10d\n",
               P[shortest_index].PID, P[shortest_index].Arrival_Time, P[shortest_index].Burst_Time,
               completion_time, waiting_time, turnaround_time);

        current_time = completion_time; // Update current time
        visited[shortest_index] = 1; // Mark process as scheduled
        completed++;
    }

    printf("\nAverage Waiting Time: %.2f", total_wt / process_count);
    printf("\nAverage Turnaround Time: %.2f\n", total_tat / process_count);

    printGanttChart(process_order, process_times, process_count, P[0].Arrival_Time);
}

int main() {
    FILE *file;
    int process_count = 0;
    char file_line[256];
    unsigned int first_line_read = 0;

    // Open and count the number of processes (excluding the header)
    file = fopen("processes.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    while (fgets(file_line, sizeof(file_line), file) != NULL) {
        if (first_line_read == 0) {
            first_line_read = 1;  // Skip header line
        } else {
            process_count++;
        }
    }
    fclose(file);

    if (process_count == 0) {
        printf("Error: No process data found in file.\n");
        return 1;
    }

    struct Process P[process_count];

    // Reopen file to read and store process details
    file = fopen("processes.txt", "r");
    if (file == NULL) {
        printf("Error reopening file\n");
        return 1;
    }

    first_line_read = 0;
    int index = 0;

    // Read and parse process data using strtol()
    while (fgets(file_line, sizeof(file_line), file) != NULL) {
        if (first_line_read == 0) {
            first_line_read = 1;  // Skip header line
        } else {
            char *ptr = file_line;
            int values[4] = {0};

            // Extract PID, Arrival Time, Burst Time, and Priority
            for (int i = 0; i < 4; i++) {
                values[i] = strtol(ptr, &ptr, 10);
            }

            if (values[0] > 0) {  // Ensure valid process ID
                P[index].PID = values[0];
                P[index].Arrival_Time = values[1];
                P[index].Burst_Time = values[2];
                P[index].Priority = values[3];
                index++;
            }
        }
    }

    fclose(file);
    process_count = index; // Adjust process count if necessary

    displayProcessTable(P, process_count);

    int choice;
    printf("\nChoose Scheduling Algorithm:\n1. FCFS\n2. SJF\nEnter your choice (1 or 2): ");
    scanf("%d", &choice);

    if (choice == 2) {
        sjf(P, process_count);
    } else {
        fcfs(P, process_count);
    }

    return 0;
}
