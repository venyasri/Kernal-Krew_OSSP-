#include "system_info.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/utsname.h>


/*
 * Calculate CPU usage using /proc/stat
 */
static double get_cpu_usage(void)
{
    unsigned long long user1;
    unsigned long long nice1;
    unsigned long long system1;
    unsigned long long idle1;
    unsigned long long iowait1;
    unsigned long long irq1;
    unsigned long long softirq1;
    unsigned long long steal1;

    unsigned long long user2;
    unsigned long long nice2;
    unsigned long long system2;
    unsigned long long idle2;
    unsigned long long iowait2;
    unsigned long long irq2;
    unsigned long long softirq2;
    unsigned long long steal2;

    FILE *file = fopen("/proc/stat", "r");

    if (file == NULL)
    {
        perror("Unable to open /proc/stat");
        return -1.0;
    }

    /*
     * Read first CPU measurement.
     * There are exactly 8 values.
     */
    if (fscanf(file,
               "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user1,
               &nice1,
               &system1,
               &idle1,
               &iowait1,
               &irq1,
               &softirq1,
               &steal1) != 8)
    {
        fclose(file);
        return -1.0;
    }

    fclose(file);

    /*
     * Wait for a short interval.
     */
    sleep(1);

    file = fopen("/proc/stat", "r");

    if (file == NULL)
    {
        perror("Unable to open /proc/stat");
        return -1.0;
    }

    /*
     * Read second CPU measurement.
     */
    if (fscanf(file,
               "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user2,
               &nice2,
               &system2,
               &idle2,
               &iowait2,
               &irq2,
               &softirq2,
               &steal2) != 8)
    {
        fclose(file);
        return -1.0;
    }

    fclose(file);

    /*
     * Calculate idle and total CPU time.
     */
    unsigned long long idle_time1 =
        idle1 + iowait1;

    unsigned long long idle_time2 =
        idle2 + iowait2;

    unsigned long long total1 =
        user1 +
        nice1 +
        system1 +
        idle1 +
        iowait1 +
        irq1 +
        softirq1 +
        steal1;

    unsigned long long total2 =
        user2 +
        nice2 +
        system2 +
        idle2 +
        iowait2 +
        irq2 +
        softirq2 +
        steal2;

    unsigned long long total_difference =
        total2 - total1;

    unsigned long long idle_difference =
        idle_time2 - idle_time1;

    if (total_difference == 0)
    {
        return 0.0;
    }

    return 100.0 *
           (double)(total_difference - idle_difference) /
           (double)total_difference;
}


/*
 * Read system uptime using:
 * open() -> read() -> close()
 */
static double get_system_uptime(void)
{
    int fd = open("/proc/uptime", O_RDONLY);

    if (fd == -1)
    {
        perror("Unable to open /proc/uptime");
        return -1.0;
    }

    char buffer[128];

    ssize_t bytes_read =
        read(fd, buffer, sizeof(buffer) - 1);

    close(fd);

    if (bytes_read <= 0)
    {
        return -1.0;
    }

    buffer[bytes_read] = '\0';

    double uptime = 0.0;

    sscanf(buffer, "%lf", &uptime);

    return uptime;
}


/*
 * Read Linux load average
 */
static void get_load_average(void)
{
    int fd = open("/proc/loadavg", O_RDONLY);

    if (fd == -1)
    {
        perror("Unable to open /proc/loadavg");
        return;
    }

    char buffer[128];

    ssize_t bytes_read =
        read(fd, buffer, sizeof(buffer) - 1);

    close(fd);

    if (bytes_read <= 0)
    {
        return;
    }

    buffer[bytes_read] = '\0';

    printf("Load Average     : %s", buffer);
}


/*
 * Main CO-1 system information function
 */
void show_system_information(void)
{
    struct utsname system_info;

    printf("\n");
    printf("============================================\n");
    printf("          CO-1: SYSTEM INFORMATION\n");
    printf("============================================\n");

    /*
     * Linux system information
     */
    if (uname(&system_info) == -1)
    {
        perror("uname");
        return;
    }

    printf("Operating System : %s\n",
           system_info.sysname);

    printf("Kernel Version   : %s\n",
           system_info.release);

    printf("Architecture     : %s\n",
           system_info.machine);

    printf("Computer Name    : %s\n",
           system_info.nodename);

    /*
     * Process information
     */
    printf("Process ID       : %d\n",
           getpid());

    printf("Parent Process ID: %d\n",
           getppid());

    /*
     * CPU usage
     */
    double cpu =
        get_cpu_usage();

    if (cpu >= 0.0)
    {
        printf("CPU Usage        : %.2f%%\n",
               cpu);
    }
    else
    {
        printf("CPU Usage        : Unable to calculate\n");
    }

    /*
     * System uptime
     */
    double uptime =
        get_system_uptime();

    if (uptime >= 0.0)
    {
        printf("System Uptime    : %.0f seconds\n",
               uptime);
    }
    else
    {
        printf("System Uptime    : Unable to read\n");
    }

    /*
     * Load average
     */
    get_load_average();

    printf("\n");

    printf("CO-1 Concepts Demonstrated:\n");
    printf("--------------------------------------------\n");
    printf("1. User Space and Kernel Space\n");
    printf("2. Linux /proc Virtual Filesystem\n");
    printf("3. System Calls\n");
    printf("4. System Information Retrieval\n");
    printf("5. CPU Utilization\n");
    printf("6. Linux Systems Programming\n");

    printf("============================================\n");
}
