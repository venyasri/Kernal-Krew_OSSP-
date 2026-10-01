#include <stdio.h>

#include "system_info.h"
#include "process_monitor.h"
#include "ipc_monitor.h"
#include "memory_monitor.h"
#include "file_monitor.h"
#include "thread_monitor.h"
#include "live_monitor.h"

int main(void)
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("============================================\n");
        printf("          LINUX KERNEL MONITOR\n");
        printf("============================================\n");

        printf("1. System Information       [CO-1]\n");
        printf("2. Process Monitor          [CO-2]\n");
        printf("3. IPC Monitor              [CO-3]\n");
        printf("4. Memory Monitor           [CO-4]\n");
        printf("5. File System Monitor      [CO-5]\n");
        printf("6. Thread Monitor           [CO-6]\n");
        printf("7. Live Monitoring\n");
        printf("8. Exit\n");

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

        switch (choice)
        {
            case 1:

                show_system_information();

                break;
            case 2:

                show_process_monitor();

               break;

            case 3:

                show_ipc_monitor();

                break;

            case 4:

                show_memory_monitor();

                break;

            case 5:

                show_file_monitor();

                break;

            case 6:

                show_thread_monitor();

                break;

            case 7:

                start_live_monitoring();

                break;

            case 8:

                printf("\nExiting Kernel Monitor...\n");

                return 0;

            default:

                printf("\nPlease enter a number between 1 and 8.\n");
        }
    }

    return 0;
}
