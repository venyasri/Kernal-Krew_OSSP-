#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i;

    // malloc()
    int *a = (int *)malloc(3 * sizeof(int));

    printf("Memory allocated using malloc()\n");

    for (i = 0; i < 3; i++)
    {
        a[i] = i + 1;
        printf("a[%d] = %d\n", i, a[i]);
    }

    // calloc()
    int *b = (int *)calloc(3, sizeof(int));

    printf("\nMemory allocated using calloc()\n");

    for (i = 0; i < 3; i++)
    {
        printf("b[%d] = %d\n", i, b[i]);
    }

    // realloc()
    a = (int *)realloc(a, 5 * sizeof(int));

    printf("\nMemory increased using realloc()\n");

    for (i = 3; i < 5; i++)
    {
        a[i] = i + 1;
    }

    for (i = 0; i < 5; i++)
    {
        printf("a[%d] = %d\n", i, a[i]);
    }

    // free()
    free(a);
    free(b);

    printf("\nMemory released using free()\n");

    return 0;
}
