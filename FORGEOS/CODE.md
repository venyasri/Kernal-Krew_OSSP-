#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>

void cpu_info() {
    FILE *file = fopen("/proc/cpuinfo", "r");

    if (file == NULL) {
        perror("Error opening /proc/cpuinfo");
        return;
    }

    char line[256];

    printf("\n========== CPU INFORMATION ==========\n");

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "model name", 10) == 0 ||
            strncmp(line, "cpu cores", 9) == 0) {
            printf("%s", line);
        }
    }

    fclose(file);
}

void memory_info() {
    FILE *file = fopen("/proc/meminfo", "r");

    if (file == NULL) {
        perror("Error opening /proc/meminfo");
        return;
    }

    char line[256];
    long total = 0;
    long available = 0;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "MemTotal: %ld kB", &total) == 1) {
            continue;
        }

        if (sscanf(line, "MemAvailable: %ld kB", &available) == 1) {
            continue;
        }
    }

    fclose(file);

    long used = total - available;

    printf("\n========== MEMORY INFORMATION ==========\n");
    printf("Total Memory     : %.2f GB\n", total / 1024.0 / 1024.0);
    printf("Available Memory : %.2f GB\n", available / 1024.0 / 1024.0);
    printf("Used Memory      : %.2f GB\n", used / 1024.0 / 1024.0);

    if (total > 0) {
        printf("Memory Usage     : %.2f%%\n",
               (used * 100.0) / total);
    }
}

void uptime_info() {
    FILE *file = fopen("/proc/uptime", "r");

    if (file == NULL) {
        perror("Error opening /proc/uptime");
        return;
    }

    double uptime;
    fscanf(file, "%lf", &uptime);

    fclose(file);

    int days = uptime / 86400;
    uptime = (int)uptime % 86400;

    int hours = uptime / 3600;
    uptime = (int)uptime % 3600;

    int minutes = uptime / 60;
    int seconds = (int)uptime % 60;

    printf("\n========== SYSTEM UPTIME ==========\n");
    printf("Uptime: %d Days %d Hours %d Minutes %d Seconds\n",
           days, hours, minutes, seconds);
}

void load_average() {
    FILE *file = fopen("/proc/loadavg", "r");

    if (file == NULL) {
        perror("Error opening /proc/loadavg");
        return;
    }

    double load1, load5, load15;

    fscanf(file, "%lf %lf %lf",
           &load1, &load5, &load15);

    fclose(file);

    printf("\n========== LOAD AVERAGE ==========\n");
    printf("1 Minute  : %.2f\n", load1);
    printf("5 Minutes : %.2f\n", load5);
    printf("15 Minutes: %.2f\n", load15);
}

void cpu_usage() {
    FILE *file = fopen("/proc/stat", "r");

    if (file == NULL) {
        perror("Error opening /proc/stat");
        return;
    }

    unsigned long long user, nice, system, idle;
    unsigned long long iowait, irq, softirq, steal;

    fscanf(file, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle,
           &iowait, &irq, &softirq, &steal);

    fclose(file);

    unsigned long long idle_time = idle + iowait;

    unsigned long long total =
        user + nice + system + idle +
        iowait + irq + softirq + steal;

    unsigned long long busy =
        total - idle_time;

    double usage = 0;

    if (total > 0) {
        usage = (busy * 100.0) / total;
    }

    printf("\n========== CPU USAGE ==========\n");
    printf("CPU Usage: %.2f%%\n", usage);
}

void process_info() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir("/proc");

    if (dir == NULL) {
        perror("Error opening /proc");
        return;
    }

    printf("\n========== RUNNING PROCESSES ==========\n");
    printf("%-10s %-30s %-10s\n",
           "PID", "PROCESS NAME", "STATE");

    int count = 0;

    while ((entry = readdir(dir)) != NULL) {

        if (!isdigit(entry->d_name[0])) {
            continue;
        }

        char path[256];

        snprintf(path,
                 sizeof(path),
                 "/proc/%s/status",
                 entry->d_name);

        FILE *file = fopen(path, "r");

        if (file == NULL) {
            continue;
        }

        char line[256];
        char name[100] = "Unknown";
        char state[100] = "Unknown";

        while (fgets(line, sizeof(line), file)) {

            if (strncmp(line, "Name:", 5) == 0) {
                sscanf(line, "Name:\t%99s", name);
            }

            if (strncmp(line, "State:", 6) == 0) {
                sscanf(line, "State:\t%c", &state[0]);
            }
        }

        fclose(file);

        printf("%-10s %-30s %-10s\n",
               entry->d_name,
               name,
               state);

        count++;

        if (count >= 20) {
            break;
        }
    }

    closedir(dir);
}

void kernel_info() {
    FILE *file = fopen("/proc/version", "r");

    if (file == NULL) {
        perror("Error opening /proc/version");
        return;
    }

    char line[512];

    printf("\n========== KERNEL INFORMATION ==========\n");

    if (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

void show_all() {
    printf("\n");
    printf("============================================\n");
    printf("          LINUX KERNEL MONITOR\n");
    printf("============================================\n");

    kernel_info();
    cpu_info();
    cpu_usage();
    memory_info();
    uptime_info();
    load_average();
    process_info();

    printf("\n============================================\n");
}

void live_monitor() {

    while (1) {

        system("clear");

        show_all();

        printf("\nLive monitoring... Press Ctrl+C to stop.\n");

        sleep(3);
    }
}

int main() {

    int choice;

    while (1) {

        printf("\n============================================\n");
        printf("          LINUX KERNEL MONITOR\n");
        printf("============================================\n");

        printf("1. CPU Information\n");
        printf("2. CPU Usage\n");
        printf("3. Memory Information\n");
        printf("4. System Uptime\n");
        printf("5. Load Average\n");
        printf("6. Running Processes\n");
        printf("7. Kernel Information\n");
        printf("8. Display All Information\n");
        printf("9. Live Monitoring\n");
        printf("0. Exit\n");

        printf("--------------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                cpu_info();
                break;

            case 2:
                cpu_usage();
                break;

            case 3:
                memory_info();
                break;

            case 4:
                uptime_info();
                break;

            case 5:
                load_average();
                break;

            case 6:
                process_info();
                break;

            case 7:
                kernel_info();
                break;

            case 8:
                show_all();
                break;

            case 9:
                live_monitor();
                break;

            case 0:
                printf("\nExiting Kernel Monitor...\n");
                return 0;

            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    }

    return 0;
}
