#define _POSIX_C_SOURCE 200809L

#include "thread_monitor.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


/* ==========================================
   GLOBAL VARIABLES
   ========================================== */

static int shared_counter = 0;

static pthread_mutex_t counter_mutex =
    PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t condition_mutex =
    PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t condition =
    PTHREAD_COND_INITIALIZER;

static int data_ready = 0;

static sem_t counting_semaphore;


/* ==========================================
   1. POSIX THREAD DEMO
   ========================================== */

static void *basic_thread_function(void *argument)
{
    int thread_number =
        *(int *)argument;

    printf("Thread %d started.\n",
           thread_number);

    printf("Thread %d is executing concurrently.\n",
           thread_number);

    sleep(1);

    printf("Thread %d finished.\n",
           thread_number);

    return NULL;
}


static void basic_thread_demo(void)
{
    printf("\n============================================\n");
    printf("             POSIX THREAD DEMO\n");
    printf("============================================\n");

    pthread_t thread1;
    pthread_t thread2;

    int number1 = 1;
    int number2 = 2;

    printf("Creating two POSIX threads...\n\n");

    if (pthread_create(&thread1,
                       NULL,
                       basic_thread_function,
                       &number1) != 0)
    {
        perror("pthread_create");
        return;
    }

    if (pthread_create(&thread2,
                       NULL,
                       basic_thread_function,
                       &number2) != 0)
    {
        perror("pthread_create");
        return;
    }

    pthread_join(thread1, NULL);

    pthread_join(thread2, NULL);

    printf("\nBoth threads completed.\n");
}


/* ==========================================
   2. RACE CONDITION DEMO
   ========================================== */

static void *race_thread_function(void *argument)
{
    (void)argument;

    for (int i = 0; i < 100000; i++)
    {
        /*
         * Multiple threads access the same
         * variable without synchronization.
         */
        shared_counter++;
    }

    return NULL;
}


static void race_condition_demo(void)
{
    printf("\n============================================\n");
    printf("             RACE CONDITION DEMO\n");
    printf("============================================\n");

    shared_counter = 0;

    pthread_t thread1;
    pthread_t thread2;

    pthread_create(&thread1,
                   NULL,
                   race_thread_function,
                   NULL);

    pthread_create(&thread2,
                   NULL,
                   race_thread_function,
                   NULL);

    pthread_join(thread1, NULL);

    pthread_join(thread2, NULL);

    printf("Expected value : 200000\n");
    printf("Actual value   : %d\n",
           shared_counter);

    printf("\nBoth threads accessed the same shared\n");
    printf("variable without a mutex.\n");

    printf("This demonstrates a race condition.\n");
}


/* ==========================================
   3. MUTEX DEMO
   ========================================== */

static void *mutex_thread_function(void *argument)
{
    (void)argument;

    for (int i = 0; i < 100000; i++)
    {
        pthread_mutex_lock(&counter_mutex);

        shared_counter++;

        pthread_mutex_unlock(&counter_mutex);
    }

    return NULL;
}


static void mutex_demo(void)
{
    printf("\n============================================\n");
    printf("               MUTEX DEMO\n");
    printf("============================================\n");

    shared_counter = 0;

    pthread_t thread1;
    pthread_t thread2;

    pthread_create(&thread1,
                   NULL,
                   mutex_thread_function,
                   NULL);

    pthread_create(&thread2,
                   NULL,
                   mutex_thread_function,
                   NULL);

    pthread_join(thread1, NULL);

    pthread_join(thread2, NULL);

    printf("Expected value : 200000\n");
    printf("Actual value   : %d\n",
           shared_counter);

    printf("\nMutex protected the shared variable.\n");
    printf("Only one thread can enter the critical\n");
    printf("section at a time.\n");
}


/* ==========================================
   4. CONDITION VARIABLE DEMO
   ========================================== */

static void *consumer_thread(void *argument)
{
    (void)argument;

    pthread_mutex_lock(&condition_mutex);

    printf("\n[CONSUMER]\n");
    printf("Consumer is waiting for data...\n");

    while (!data_ready)
    {
        pthread_cond_wait(&condition,
                          &condition_mutex);
    }

    printf("Consumer received the notification.\n");
    printf("Consumer can now continue.\n");

    pthread_mutex_unlock(&condition_mutex);

    return NULL;
}


static void *producer_thread(void *argument)
{
    (void)argument;

    sleep(2);

    pthread_mutex_lock(&condition_mutex);

    printf("\n[PRODUCER]\n");
    printf("Producer generated data.\n");

    data_ready = 1;

    printf("Producer sends condition signal.\n");

    pthread_cond_signal(&condition);

    pthread_mutex_unlock(&condition_mutex);

    return NULL;
}


static void condition_variable_demo(void)
{
    printf("\n============================================\n");
    printf("          CONDITION VARIABLE DEMO\n");
    printf("============================================\n");

    data_ready = 0;

    pthread_t producer;
    pthread_t consumer;

    pthread_create(&consumer,
                   NULL,
                   consumer_thread,
                   NULL);

    pthread_create(&producer,
                   NULL,
                   producer_thread,
                   NULL);

    pthread_join(consumer, NULL);

    pthread_join(producer, NULL);

    printf("\nCondition variable demonstration completed.\n");
}


/* ==========================================
   5. COUNTING SEMAPHORE DEMO
   ========================================== */

static void *semaphore_thread(void *argument)
{
    int thread_number =
        *(int *)argument;

    printf("Thread %d waiting for semaphore...\n",
           thread_number);

    sem_wait(&counting_semaphore);

    printf("Thread %d entered the limited resource.\n",
           thread_number);

    sleep(1);

    printf("Thread %d leaving the resource.\n",
           thread_number);

    sem_post(&counting_semaphore);

    return NULL;
}


static void semaphore_demo(void)
{
    printf("\n============================================\n");
    printf("           COUNTING SEMAPHORE DEMO\n");
    printf("============================================\n");

    /*
     * Allow only 2 threads to access the
     * simulated resource at the same time.
     */
    if (sem_init(&counting_semaphore,
                 0,
                 2) != 0)
    {
        perror("sem_init");
        return;
    }

    pthread_t threads[4];

    int numbers[4];

    for (int i = 0; i < 4; i++)
    {
        numbers[i] = i + 1;

        pthread_create(&threads[i],
                       NULL,
                       semaphore_thread,
                       &numbers[i]);
    }

    for (int i = 0; i < 4; i++)
    {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&counting_semaphore);

    printf("\nSemaphore demonstration completed.\n");
    printf("Only two threads were allowed to use\n");
    printf("the simulated resource at the same time.\n");
}


/* ==========================================
   6. DEADLOCK CONCEPT
   ========================================== */

static void deadlock_demo(void)
{
    printf("\n============================================\n");
    printf("             DEADLOCK CONCEPT\n");
    printf("============================================\n");

    printf("\nImagine two threads and two resources:\n\n");

    printf("Thread 1:\n");
    printf("  Locks Resource A\n");
    printf("       |\n");
    printf("       v\n");
    printf("  Waits for Resource B\n\n");

    printf("Thread 2:\n");
    printf("  Locks Resource B\n");
    printf("       |\n");
    printf("       v\n");
    printf("  Waits for Resource A\n\n");

    printf("Both threads keep waiting.\n");

    printf("\nThis situation is called DEADLOCK.\n");

    printf("\nCommon prevention technique:\n");
    printf("- Always acquire locks in the same order.\n");
    printf("- Avoid holding a lock unnecessarily.\n");
    printf("- Use timeout-based synchronization where appropriate.\n");

    printf("\nThis demonstration explains the deadlock\n");
    printf("concept without intentionally freezing the program.\n");
}


/* ==========================================
   CO-6 MENU
   ========================================== */

void show_thread_monitor(void)
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("============================================\n");
        printf("          CO-6: THREAD MONITOR\n");
        printf("============================================\n");

        printf("1. POSIX Thread Demonstration\n");
        printf("2. Race Condition Demonstration\n");
        printf("3. Mutex Demonstration\n");
        printf("4. Condition Variable Demonstration\n");
        printf("5. Counting Semaphore Demonstration\n");
        printf("6. Deadlock Concept\n");
        printf("7. Back to Main Menu\n");

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
                basic_thread_demo();
                break;

            case 2:
                race_condition_demo();
                break;

            case 3:
                mutex_demo();
                break;

            case 4:
                condition_variable_demo();
                break;

            case 5:
                semaphore_demo();
                break;

            case 6:
                deadlock_demo();
                break;

            case 7:
                return;

            default:
                printf("\nPlease enter a number between 1 and 7.\n");
        }
    }
}
