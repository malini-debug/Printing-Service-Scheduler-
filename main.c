/*
 * Printing Service Scheduler - FCFS (First Come First Served)
 * ------------------------------------------------------------
 * Each print job has: user, document name, pages, arrival time.
 * Burst time (print time) = pages * SECS_PER_PAGE.
 * Jobs are served strictly in order of arrival (FCFS).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_JOBS      100
#define NAME_LEN      50
#define SECS_PER_PAGE 2

typedef struct {
    int  id;
    char user[NAME_LEN];
    char document[NAME_LEN];
    int  pages;
    int  arrival;
    int  burst;
    int  start;
    int  finish;
    int  waiting;
    int  turnaround;
} PrintJob;

static PrintJob jobs[MAX_JOBS];
static int job_count = 0;
static int next_id = 1;

/* ---------- input helpers ---------- */
static void read_line(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\n")] = '\0';
}

static int read_int(const char *prompt, int min) {
    char line[64];
    int value;
    while (1) {
        read_line(prompt, line, sizeof(line));
        if (sscanf(line, "%d", &value) == 1 && value >= min) return value;
        printf("  Invalid input. Enter a number >= %d.\n", min);
    }
}

/* ---------- job management ---------- */
static void add_job_data(const char *user, const char *doc, int pages, int arrival) {
    if (job_count >= MAX_JOBS) { printf("Queue full!\n"); return; }
    PrintJob *j = &jobs[job_count++];
    memset(j, 0, sizeof(*j));
    j->id = next_id++;
    strncpy(j->user, user, NAME_LEN - 1);
    strncpy(j->document, doc, NAME_LEN - 1);
    j->pages = pages;
    j->arrival = arrival;
    j->burst = pages * SECS_PER_PAGE;
}

static void add_job(void) {
    char user[NAME_LEN], doc[NAME_LEN];
    read_line("User name       : ", user, sizeof(user));
    read_line("Document name   : ", doc, sizeof(doc));
    int pages   = read_int("Number of pages : ", 1);
    int arrival = read_int("Arrival time (s): ", 0);
    add_job_data(user, doc, pages, arrival);
    printf("Job added successfully.\n");
}

static void load_sample(void) {
    job_count = 0; next_id = 1;
    add_job_data("Arun",   "Report.pdf",     10, 0);
    add_job_data("Priya",  "Resume.docx",     2, 1);
    add_job_data("Karthik","Assignment.pdf", 15, 3);
    add_job_data("Divya",  "Photo.jpg",       4, 6);
    add_job_data("Suresh", "Notes.txt",       6, 20);
    printf("Sample data loaded (5 jobs).\n");
}

static void clear_jobs(void) {
    job_count = 0; next_id = 1;
    printf("All jobs cleared.\n");
}

static void view_jobs(void) {
    if (job_count == 0) { printf("No jobs in queue.\n"); return; }
    printf("\n%-4s %-12s %-18s %-6s %-8s %-6s\n",
           "ID", "User", "Document", "Pages", "Arrival", "Burst");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < job_count; i++)
        printf("%-4d %-12s %-18s %-6d %-8d %-6d\n",
               jobs[i].id, jobs[i].user, jobs[i].document,
               jobs[i].pages, jobs[i].arrival, jobs[i].burst);
}

/* ---------- FCFS scheduling ---------- */
static void run_fcfs(void) {
    if (job_count == 0) { printf("No jobs to schedule.\n"); return; }

    PrintJob q[MAX_JOBS];
    memcpy(q, jobs, sizeof(PrintJob) * job_count);

    /* stable insertion sort by arrival time (ties keep submission order) */
    for (int i = 1; i < job_count; i++) {
        PrintJob key = q[i];
        int j = i - 1;
        while (j >= 0 && q[j].arrival > key.arrival) { q[j + 1] = q[j]; j--; }
        q[j + 1] = key;
    }

    int time = 0, idle = 0;
    double total_wait = 0, total_tat = 0;

    for (int i = 0; i < job_count; i++) {
        if (time < q[i].arrival) { idle += q[i].arrival - time; time = q[i].arrival; }
        q[i].start      = time;
        q[i].finish     = time + q[i].burst;
        q[i].waiting    = q[i].start - q[i].arrival;
        q[i].turnaround = q[i].finish - q[i].arrival;
        time = q[i].finish;
        total_wait += q[i].waiting;
        total_tat  += q[i].turnaround;
    }

    printf("\n=========== FCFS PRINT SCHEDULE ===========\n");
    printf("%-4s %-12s %-18s %-6s %-8s %-6s %-6s %-6s %-5s %-5s\n",
           "ID","User","Document","Pages","Arrival","Burst","Start","Finish","Wait","TAT");
    printf("-----------------------------------------------------------------------------------\n");
    for (int i = 0; i < job_count; i++)
        printf("%-4d %-12s %-18s %-6d %-8d %-6d %-6d %-6d %-5d %-5d\n",
               q[i].id, q[i].user, q[i].document, q[i].pages, q[i].arrival,
               q[i].burst, q[i].start, q[i].finish, q[i].waiting, q[i].turnaround);

    /* Gantt chart */
    printf("\nGantt Chart:\n|");
    for (int i = 0; i < job_count; i++) {
        if (i > 0 && q[i].start > q[i-1].finish) printf(" idle |");
        printf(" J%d (%s) |", q[i].id, q[i].user);
    }
    printf("\n%d", q[0].start);
    for (int i = 0; i < job_count; i++) {
        if (i > 0 && q[i].start > q[i-1].finish) printf("  ...  %d", q[i].start);
        printf("  ...  %d", q[i].finish);
    }
    printf("\n");

    printf("\nAverage Waiting Time    : %.2f s\n", total_wait / job_count);
    printf("Average Turnaround Time : %.2f s\n", total_tat / job_count);
    printf("Total Printer Idle Time : %d s\n", idle);
    printf("All jobs finished at    : %d s\n", time);
}

/* ---------- main menu ---------- */
int main(void) {
    int choice;
    do {
        printf("\n===== PRINTING SERVICE SCHEDULER (FCFS) =====\n");
        printf("1. Add print job\n");
        printf("2. View job queue\n");
        printf("3. Run FCFS scheduler\n");
        printf("4. Load sample data\n");
        printf("5. Clear all jobs\n");
        printf("0. Exit\n");
        choice = read_int("Enter choice: ", 0);
        switch (choice) {
            case 1: add_job();    break;
            case 2: view_jobs();  break;
            case 3: run_fcfs();   break;
            case 4: load_sample();break;
            case 5: clear_jobs(); break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
    return 0;
}
