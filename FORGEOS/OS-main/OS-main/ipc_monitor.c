#define _POSIX_C_SOURCE 200809L

#include "ipc_monitor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#define FIFO_PATH "/tmp/kernel_monitor_fifo"

static volatile sig_atomic_t signal_received = 0;


/* ==========================================
   SIGNAL HANDLER
   ========================================== */

static void handle_sigint(int signal_number)
{
    (void)signal_number;

    signal_received = 1;

    write(STDOUT_FILENO,
          "\nSIGINT received. Signal handler executed.\n",
          43);
}


/* ==========================================
   CO-3 OPTION 1
   ANONYMOUS PIPE
   ========================================== */

static void anonymous_pipe_demo(void)
{
    printf("\n============================================\n");
    printf("          ANONYMOUS PIPE DEMO\n");
    printf("============================================\n");

    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        return;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");

        close(pipe_fd[0]);
        close(pipe_fd[1]);

        return;
    }


    /*
     * CHILD
     */

    if (pid == 0)
    {
        close(pipe_fd[1]);

        char buffer[256];

        ssize_t bytes =
            read(pipe_fd[0],
                 buffer,
                 sizeof(buffer) - 1);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';

            printf("\n[CHILD]\n");
            printf("Message received: %s\n",
                   buffer);
        }

        close(pipe_fd[0]);

        _exit(0);
    }


    /*
     * PARENT
     */

    close(pipe_fd[0]);

    const char message[] =
        "Hello Child! This message was sent using an anonymous pipe.";

    printf("\n[PARENT]\n");

    printf("Sending message to child...\n");

    write(pipe_fd[1],
          message,
          strlen(message));

    close(pipe_fd[1]);

    waitpid(pid,
            NULL,
            0);

    printf("Anonymous pipe communication completed.\n");
}


/* ==========================================
   CO-3 OPTION 2
   NAMED PIPE / FIFO
   ========================================== */

static void fifo_demo(void)
{
    printf("\n============================================\n");
    printf("          NAMED PIPE / FIFO DEMO\n");
    printf("============================================\n");

    unlink(FIFO_PATH);

    if (mkfifo(FIFO_PATH, 0600) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mkfifo");
            return;
        }
    }

    printf("FIFO created: %s\n",
           FIFO_PATH);

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");

        unlink(FIFO_PATH);

        return;
    }


    /*
     * CHILD - READER
     */

    if (pid == 0)
    {
        int read_fd =
            open(FIFO_PATH,
                 O_RDONLY);

        if (read_fd == -1)
        {
            perror("open FIFO");
            _exit(1);
        }

        char buffer[256];

        ssize_t bytes =
            read(read_fd,
                 buffer,
                 sizeof(buffer) - 1);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';

            printf("\n[CHILD / FIFO READER]\n");

            printf("Received: %s\n",
                   buffer);
        }

        close(read_fd);

        _exit(0);
    }


    /*
     * PARENT - WRITER
     */

    sleep(1);

    int write_fd =
        open(FIFO_PATH,
             O_WRONLY);

    if (write_fd == -1)
    {
        perror("open FIFO");

        unlink(FIFO_PATH);

        return;
    }

    const char message[] =
        "Hello Child! This message was sent through a named FIFO.";

    printf("\n[PARENT / FIFO WRITER]\n");

    printf("Sending message...\n");

    write(write_fd,
          message,
          strlen(message));

    close(write_fd);

    waitpid(pid,
            NULL,
            0);

    unlink(FIFO_PATH);

    printf("Named FIFO communication completed.\n");
}


/* ==========================================
   CO-3 OPTION 3
   SIGNAL HANDLING
   ========================================== */

static void signal_demo(void)
{
    printf("\n============================================\n");
    printf("             SIGNAL DEMO\n");
    printf("============================================\n");

    struct sigaction action;

    memset(&action,
           0,
           sizeof(action));

    action.sa_handler =
        handle_sigint;

    sigemptyset(&action.sa_mask);

    action.sa_flags = 0;

    if (sigaction(SIGINT,
                  &action,
                  NULL) == -1)
    {
        perror("sigaction");
        return;
    }

    signal_received = 0;

    printf("\nSignal handler installed.\n");

    printf("Press Ctrl+C to send SIGINT.\n");

    printf("Waiting for signal...\n\n");


    int counter = 1;

    while (!signal_received)
    {
        printf("Monitoring... %d\n",
               counter++);

        sleep(1);
    }


    printf("\nSignal handling completed.\n");

    printf("The program received SIGINT safely.\n");
}


/* ==========================================
   MAIN CO-3 MENU
   ========================================== */

void show_ipc_monitor(void)
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("============================================\n");
        printf("            CO-3: IPC MONITOR\n");
        printf("============================================\n");

        printf("1. Anonymous Pipe\n");
        printf("2. Named Pipe (FIFO)\n");
        printf("3. Signal Handling\n");
        printf("4. Back to Main Menu\n");

        printf("============================================\n");

        printf("Enter your choice: ");

        if (scanf("%d",
                  &choice) != 1)
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

                anonymous_pipe_demo();

                break;


            case 2:

                fifo_demo();

                break;


            case 3:

                signal_demo();

                break;


            case 4:

                return;


            default:

                printf("\nPlease enter 1, 2, 3 or 4.\n");
        }
    }
}
