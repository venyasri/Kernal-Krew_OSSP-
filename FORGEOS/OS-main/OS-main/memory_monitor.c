#include "memory_monitor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>


/* ==========================================
   1. SYSTEM MEMORY
   ========================================== */

static void show_system_memory(void)
{
    FILE *file = fopen("/proc/meminfo", "r");

    if (file == NULL)
    {
        perror("Unable to open /proc/meminfo");
        return;
    }

    char line[256];

    unsigned long long total = 0;
    unsigned long long free_memory = 0;
    unsigned long long available = 0;
    unsigned long long buffers = 0;
    unsigned long long cached = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        sscanf(line, "MemTotal: %llu kB", &total);
        sscanf(line, "MemFree: %llu kB", &free_memory);
        sscanf(line, "MemAvailable: %llu kB", &available);
        sscanf(line, "Buffers: %llu kB", &buffers);
        sscanf(line, "Cached: %llu kB", &cached);
    }

    fclose(file);

    unsigned long long used = 0;

    if (total > available)
    {
        used = total - available;
    }

    printf("\n============================================\n");
    printf("              SYSTEM MEMORY\n");
    printf("============================================\n");

    printf("Total Memory     : %.2f GB\n",
           total / 1024.0 / 1024.0);

    printf("Free Memory      : %.2f GB\n",
           free_memory / 1024.0 / 1024.0);

    printf("Available Memory : %.2f GB\n",
           available / 1024.0 / 1024.0);

    printf("Used Memory      : %.2f GB\n",
           used / 1024.0 / 1024.0);

    printf("Buffers          : %.2f MB\n",
           buffers / 1024.0);

    printf("Cached           : %.2f MB\n",
           cached / 1024.0);

    if (total > 0)
    {
        printf("Memory Usage     : %.2f%%\n",
               used * 100.0 / total);
    }
}


/* ==========================================
   2. PROCESS MEMORY
   ========================================== */

static void show_process_memory(void)
{
    FILE *file = fopen("/proc/self/status", "r");

    if (file == NULL)
    {
        perror("Unable to open /proc/self/status");
        return;
    }

    char line[256];

    printf("\n============================================\n");
    printf("              PROCESS MEMORY\n");
    printf("============================================\n");

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strncmp(line, "VmSize:", 7) == 0 ||
            strncmp(line, "VmRSS:", 6) == 0 ||
            strncmp(line, "VmData:", 7) == 0 ||
            strncmp(line, "VmStk:", 6) == 0 ||
            strncmp(line, "VmExe:", 6) == 0 ||
            strncmp(line, "VmLib:", 6) == 0)
        {
            printf("%s", line);
        }
    }

    fclose(file);
}


/* ==========================================
   3. PAGE FAULTS
   ========================================== */

static void show_page_faults(void)
{
    struct rusage usage;

    if (getrusage(RUSAGE_SELF, &usage) != 0)
    {
        perror("getrusage");
        return;
    }

    printf("\n============================================\n");
    printf("              PAGE FAULTS\n");
    printf("============================================\n");

    printf("Minor Page Faults : %ld\n",
           usage.ru_minflt);

    printf("Major Page Faults : %ld\n",
           usage.ru_majflt);

    printf("\nMinor page faults : %ld\n",
           usage.ru_minflt);

    printf("Major page faults : %ld\n",
           usage.ru_majflt);
}


/* ==========================================
   4. DYNAMIC MEMORY ALLOCATION
   ========================================== */

static void memory_allocation_demo(void)
{
    printf("\n============================================\n");
    printf("         DYNAMIC MEMORY ALLOCATION\n");
    printf("============================================\n");

    size_t size = 10 * 1024 * 1024;

    printf("Requesting 10 MB using malloc()...\n");

    char *memory = malloc(size);

    if (memory == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("Memory allocation successful.\n");

    /*
     * Access one byte every 4096 bytes.
     * This causes the allocated pages to be used.
     */
    for (size_t i = 0; i < size; i += 4096)
    {
        memory[i] = 1;
    }

    printf("Allocated memory has been accessed.\n");

    printf("Releasing memory using free()...\n");

    free(memory);

    printf("Memory successfully released.\n");
}


/* ==========================================
   5. COPY-ON-WRITE
   ========================================== */

static void copy_on_write_demo(void)
{
    printf("\n============================================\n");
    printf("            COPY-ON-WRITE DEMO\n");
    printf("============================================\n");

    int *value = malloc(sizeof(int));

    if (value == NULL)
    {
        perror("malloc");
        return;
    }

    *value = 100;

    printf("Before fork():\n");
    printf("Parent value = %d\n", *value);

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");

        free(value);
        return;
    }

    if (pid == 0)
    {
        printf("\n[CHILD PROCESS]\n");

        printf("Child initially sees value = %d\n",
               *value);

        *value = 200;

        printf("Child changes its value to = %d\n",
               *value);

        printf("\nChild modified its memory.\n");
        printf("Linux can create a private copy of the page.\n");

        free(value);

        _exit(0);
    }
    else
    {
        waitpid(pid, NULL, 0);

        printf("\n[PARENT PROCESS]\n");

        printf("Parent still sees value = %d\n",
               *value);

        printf("\nThis demonstrates the idea of\n");
        printf("Copy-on-Write after fork().\n");

        free(value);
    }
}


/* ==========================================
   CO-4 MENU
   ========================================== */

void show_memory_monitor(void)
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("============================================\n");
        printf("           CO-4: MEMORY MONITOR\n");
        printf("============================================\n");

        printf("1. System Memory\n");
        printf("2. Process Memory\n");
        printf("3. Page Fault Information\n");
        printf("4. Dynamic Memory Allocation\n");
        printf("5. Copy-on-Write Demonstration\n");
        printf("6. Back to Main Menu\n");

        printf("============================================\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input.\n");

            while (getchar() != '\n')
            {
                /* Clear input */
            }

            continue;
        }

        while (getchar() != '\n')
        {
            /* Clear input */
        }

        switch (choice)
        {
            case 1:
                show_system_memory();
                break;

            case 2:
                show_process_memory();
                break;

            case 3:
                show_page_faults();
                break;

            case 4:
                memory_allocation_demo();
                break;

            case 5:
                copy_on_write_demo();
                break;

            case 6:
                return;

            default:
                printf("\nPlease enter a number between 1 and 6.\n");
        }
    }
}
