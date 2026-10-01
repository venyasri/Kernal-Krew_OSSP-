#include "process_monitor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>


/*
 * Check whether a string contains only digits.
 * Linux process directories inside /proc have numeric names.
 */
static int is_number(const char *text)
{
    if (text == NULL || *text == '\0')
    {
        return 0;
    }

    for (int i = 0; text[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)text[i]))
        {
            return 0;
        }
    }

    return 1;
}


/*
 * Display information about one process.
 */
static void display_process_info(const char *pid_string)
{
    char path[256];

    snprintf(path,
             sizeof(path),
             "/proc/%s/status",
             pid_string);

    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        return;
    }

    char line[512];

    char process_name[256] = "Unknown";
    char process_state[256] = "Unknown";
    char parent_pid[64] = "Unknown";

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strncmp(line, "Name:", 5) == 0)
        {
            sscanf(line + 5,
                   "%255s",
                   process_name);
        }
        else if (strncmp(line, "State:", 6) == 0)
        {
            char state_value[128];

            if (sscanf(line + 6,
                       " %127[^\n]",
                       state_value) == 1)
            {
                strncpy(process_state,
                        state_value,
                        sizeof(process_state) - 1);

                process_state[
                    sizeof(process_state) - 1
                ] = '\0';
            }
        }
        else if (strncmp(line, "PPid:", 5) == 0)
        {
            sscanf(line + 5,
                   "%63s",
                   parent_pid);
        }
    }

    fclose(file);

    printf("%-8s %-25s %-20s %-10s\n",
           pid_string,
           process_name,
           process_state,
           parent_pid);
}


/*
 * Display currently running Linux processes.
 */
static void display_running_processes(void)
{
    DIR *proc_directory = opendir("/proc");

    if (proc_directory == NULL)
    {
        perror("Unable to open /proc");
        return;
    }

    printf("\n");
    printf("%-8s %-25s %-20s %-10s\n",
           "PID",
           "PROCESS",
           "STATE",
           "PPID");

    printf("-----------------------------------------------------------------------\n");

    struct dirent *entry;

    int displayed = 0;

    while ((entry = readdir(proc_directory)) != NULL)
    {
        /*
         * Process directories have numeric names.
         */
        if (!is_number(entry->d_name))
        {
            continue;
        }

        display_process_info(entry->d_name);

        displayed++;

        /*
         * Display a reasonable number of processes
         * so the terminal does not become too large.
         */
        if (displayed >= 20)
        {
            break;
        }
    }

    closedir(proc_directory);

    printf("\nDisplayed %d processes.\n",
           displayed);
}


/*
 * Demonstrate fork().
 *
 * fork() creates a new child process.
 */
static void demonstrate_fork(void)
{
    printf("\n============================================\n");
    printf("          PROCESS CREATION DEMO\n");
    printf("============================================\n");

    printf("Parent PID before fork: %d\n",
           getpid());

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return;
    }

    /*
     * Child process
     */
    if (pid == 0)
    {
        printf("\n[CHILD PROCESS]\n");

        printf("Child PID       : %d\n",
               getpid());

        printf("Child Parent PID: %d\n",
               getppid());

        printf("Child process is executing...\n");

        sleep(2);

        printf("Child process is terminating.\n");

        /*
         * Exit only the child.
         */
        _exit(0);
    }

    /*
     * Parent process
     */
    else
    {
        printf("\n[PARENT PROCESS]\n");

        printf("Parent PID : %d\n",
               getpid());

        printf("Child PID  : %d\n",
               pid);

        printf("Parent is waiting for child...\n");

        int status;

        pid_t result =
            waitpid(pid,
                    &status,
                    0);

        if (result == pid)
        {
            if (WIFEXITED(status))
            {
                printf("Child terminated normally.\n");

                printf("Child exit status: %d\n",
                       WEXITSTATUS(status));
            }
            else
            {
                printf("Child did not terminate normally.\n");
            }
        }
    }
}


/*
 * Explain the process lifecycle.
 */
static void display_process_lifecycle(void)
{
    printf("\n============================================\n");
    printf("            PROCESS LIFECYCLE\n");
    printf("============================================\n");

    printf("\n");
    printf("        CREATE\n");
    printf("           |\n");
    printf("           v\n");
    printf("         READY\n");
    printf("           |\n");
    printf("           v\n");
    printf("        RUNNING\n");
    printf("        /     \\\n");
    printf("       /       \\\n");
    printf("      v         v\n");
    printf(" WAITING      TERMINATED\n");
    printf("    |\n");
    printf("    v\n");
    printf(" RUNNING\n");

    printf("\nProcess states are maintained by the Linux operating system.\n");
}


/*
 * Main CO-2 function.
 */
void show_process_monitor(void)
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("============================================\n");
        printf("          CO-2: PROCESS MONITOR\n");
        printf("============================================\n");

        printf("1. Show Running Processes\n");
        printf("2. Demonstrate fork() and wait()\n");
        printf("3. Show Process Lifecycle\n");
        printf("4. Back to Main Menu\n");

        printf("============================================\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("\nInvalid input.\n");

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

                display_running_processes();

                break;

            case 2:

                demonstrate_fork();

                break;

            case 3:

                display_process_lifecycle();

                break;

            case 4:

                return;

            default:

                printf("\nPlease enter a number between 1 and 4.\n");
        }
    }
}
