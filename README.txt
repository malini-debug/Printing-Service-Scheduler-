Printing Service Scheduler - FCFS (C)
=====================================
Compile : gcc -o printing_scheduler main.c      (or: make)
Run     : ./printing_scheduler      (Windows: printing_scheduler.exe)

Menu:
 1. Add print job (user, document, pages, arrival time)
 2. View job queue
 3. Run FCFS scheduler (table + Gantt chart + averages)
 4. Load sample data
 5. Clear all jobs

Burst time = pages x 2 seconds (change SECS_PER_PAGE in main.c).
FCFS rule: job that arrives first is printed first.
Waiting = Start - Arrival ; Turnaround = Finish - Arrival
