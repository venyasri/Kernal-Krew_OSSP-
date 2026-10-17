#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_var = 10;
static int static_var = 20;

void code_function()
{
    printf("This is a code segment function.\n");
}

int main()
{
    int stack_var = 30;
    int *heap_var = (int *)malloc(sizeof(int));

    *heap_var = 40;

    printf("\n===== Process Memory Layout =====\n");

    printf("Code   address  : %p\n", (void *)code_function);
    printf("Global address  : %p\n", (void *)&global_var);
    printf("Static address  : %p\n", (void *)&static_var);
    printf("Heap   address  : %p\n", (void *)heap_var);
    printf("Stack  address  : %p\n", (void *)&stack_var);

    printf("\nProcess ID (PID): %d\n", getpid());

    printf("\nPress Enter to exit...\n");
    getchar();

    free(heap_var);

    return 0;
}
