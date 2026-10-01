#define _DEFAULT_SOURCE

#include "live_monitor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>


/* ==========================================
   CPU INFORMATION
   ========================================== */

static int read_cpu(
    unsigned long long *idle,
    unsigned long long *total)
{
    FILE *file = fopen("/proc/stat", "r");

    if (file == NULL)
    {
        return -1;
    }

    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle_time;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;

    int result = fscanf(
        file,
        "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
        &user,
        &nice,
        &system,
        &idle_time,
        &iowait,
        &irq,
        &softirq,
        &steal
    );

    fclose(file);

    if (result != 8)
    {
        return -1;
    }

    *idle = idle_time + iowait;

    *total =
        user +
        nice +
        system +
        idle_time +
        iowait +
        irq +
        softirq +
        steal;

    return 0;
}


/* ==========================================
   MEMORY INFORMATION
   ========================================== */

static void read_memory(
    unsigned long long *total_memory,
    unsigned long long *available_memory)
{
    FILE *file = fopen("/proc/meminfo", "r");

    if (file == NULL)
    {
        *total_memory = 0;
        *available_memory = 0;
        return;
    }

    char line[256];

    *total_memory = 0;
    *available_memory = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line,
                    "MemTotal:",
                    9) == 0)
        {
            sscanf(line + 9,
                   "%llu",
                   total_memory);
        }

        if (strncmp(line,
                    "MemAvailable:",
                    13) == 0)
        {
            sscanf(line + 13,
                   "%llu",
                   available_memory);
        }
    }

    fclose(file);
}


/* ==========================================
   PROCESS COUNT
   ========================================== */

static int count_processes(void)
{
    DIR *directory =
        opendir("/proc");

    if (directory == NULL)
    {
        return 0;
    }

    struct dirent *entry;

    int count = 0;

    while ((entry = readdir(directory)) != NULL)
    {
        int is_number = 1;

        for (size_t i = 0;
             i < strlen(entry->d_name);
             i++)
        {
            if (!isdigit(
                    (unsigned char)
                    entry->d_name[i]))
            {
                is_number = 0;
                break;
            }
        }

        if (is_number &&
            strlen(entry->d_name) > 0)
        {
            count++;
        }
    }

    closedir(directory);

    return count;
}


/* ==========================================
   UPTIME
   ========================================== */

static double read_uptime(void)
{
    FILE *file =
        fopen("/proc/uptime", "r");

    if (file == NULL)
    {
        return 0.0;
    }

    double uptime = 0.0;

    fscanf(file,
           "%lf",
           &uptime);

    fclose(file);

    return uptime;
}


/* ==========================================
   LOAD AVERAGE
   ========================================== */

static void read_load_average(
    double *load1,
    double *load5,
    double *load15)
{
    FILE *file =
        fopen("/proc/loadavg", "r");

    if (file == NULL)
    {
        *load1 = 0;
        *load5 = 0;
        *load15 = 0;
        return;
    }

    fscanf(file,
           "%lf %lf %lf",
           load1,
           load5,
           load15);

    fclose(file);
}


/* ==========================================
   DISPLAY LIVE INFORMATION
   ========================================== */

static void display_live_information(
    double cpu_usage)
{
    unsigned long long total_memory;
    unsigned long long available_memory;

    read_memory(
        &total_memory,
        &available_memory);

    double used_memory =
        (double)total_memory -
        (double)available_memory;

    double memory_percentage = 0.0;

    if (total_memory > 0)
    {
        memory_percentage =
            (used_memory /
             (double)total_memory) *
            100.0;
    }

    double uptime =
        read_uptime();

    double load1;
    double load5;
    double load15;

    read_load_average(
        &load1,
        &load5,
        &load15);

    int processes =
        count_processes();

    time_t current_time =
        time(NULL);

    struct tm *time_info =
        localtime(&current_time);

    char time_string[64];

    if (time_info != NULL)
    {
        strftime(
            time_string,
            sizeof(time_string),
            "%Y-%m-%d %H:%M:%S",
            time_info);
    }
    else
    {
        strcpy(
            time_string,
            "Unknown");
    }


    printf("\033[2J");
    printf("\033[H");

    printf("============================================================\n");
    printf("                 LINUX KERNEL MONITOR\n");
    printf("                    LIVE MONITORING\n");
    printf("============================================================\n");

    printf("Time              : %s\n",
           time_string);

    printf("------------------------------------------------------------\n");

    printf("CPU Usage         : %6.2f %%\n",
           cpu_usage);

    printf("Memory Usage      : %6.2f %%\n",
           memory_percentage);

    printf("Total Memory      : %llu MB\n",
           total_memory / 1024);

    printf("Available Memory  : %llu MB\n",
           available_memory / 1024);

    printf("------------------------------------------------------------\n");

    printf("System Uptime     : %.0f seconds\n",
           uptime);

    printf("Process Count     : %d\n",
           processes);

    printf("Load Average      : %.2f %.2f %.2f\n",
           load1,
           load5,
           load15);

    printf("------------------------------------------------------------\n");

    printf("Monitoring Sources:\n");
    printf("  /proc/stat       -> CPU information\n");
    printf("  /proc/meminfo    -> Memory information\n");
    printf("  /proc/uptime     -> System uptime\n");
    printf("  /proc/loadavg    -> Load average\n");
    printf("  /proc            -> Process information\n");

    printf("------------------------------------------------------------\n");

    printf("Refreshing every 2 seconds...\n");
    printf("Press Q + Enter to exit live monitoring.\n");

    printf("============================================================\n");

    fflush(stdout);
}


/* ==========================================
   LIVE MONITORING
   ========================================== */

void start_live_monitoring(void)
{
    unsigned long long idle1;
    unsigned long long total1;

    unsigned long long idle2;
    unsigned long long total2;

    if (read_cpu(&idle1,
                 &total1) != 0)
    {
        printf("Unable to read CPU information.\n");
        return;
    }

    printf("\nStarting live monitoring...\n");

    sleep(1);

    while (1)
    {
        if (read_cpu(&idle2,
                     &total2) != 0)
        {
            printf("Unable to read CPU information.\n");
            return;
        }

        unsigned long long idle_difference =
            idle2 - idle1;

        unsigned long long total_difference =
            total2 - total1;

        double cpu_usage = 0.0;

        if (total_difference > 0)
        {
            cpu_usage =
                100.0 *
                (double)(
                    total_difference -
                    idle_difference
                ) /
                (double)total_difference;
        }

        display_live_information(
            cpu_usage);

        idle1 = idle2;
        total1 = total2;

        /*
         * Check whether user entered Q.
         */
        printf("\n");

        sleep(2);

        /*
         * Non-blocking keyboard input is avoided
         * for portability. Press Ctrl+C to stop
         * immediately if needed.
         */
    }
}
