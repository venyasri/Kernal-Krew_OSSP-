#include<stdio.h>
#include<unistd.h>
int main()
{
int pid;
pid = fork();
if(pid < 0)
{
printf("Fork failed\n");
}
else if(pid == 0)
{
printf("Child process\n");
printf("Child PID = %d\n", getpid());
printf("Parent PID = %d\n", getppid());
execlp("ls", "ls", NULL);
}
else
{
printf("Parent process\n");
printf("Parent PID = %d\n", getpid());
printf("Child PID = %d\n", pid);
}
return 0;
}
