#include <unistd.h>
#include <stdio.h>
int main()
{
int pid;
printf("Before fork()\n");
printf("Process ID = %d\n", getpid());
printf("Parent Process ID = %d\n", getppid());
printf("State = Running\n\n");
pid = fork();
if(pid < 0)
{
printf("Fork not successful\n");
}
else if(pid == 0)
{
printf("Child Process\n");
printf("Process ID = %d\n", getpid());
printf("Parent Process ID = %d\n", getppid());
printf("State = Running\n");
}
 else
{
printf("Parent Process\n");
printf("Process ID = %d\n", getpid());
printf("Parent Process ID = %d\n", getppid());
printf("Child Process ID = %d\n", pid);
printf("State = Running\n");
}
}
