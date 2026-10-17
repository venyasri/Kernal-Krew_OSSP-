#include<stdio.h>
#include<unistd.h> //for fork() system call
#include<sys/wait.h> //for wait() and waitpid() system call
#include<stdlib.h> // for exit() system call
int main()
{
int pid[3];//array to create 3 childs
for(int i=0;i<3;i++)//for loop to navigate through the childs
{
pid[i]=fork();//creating the childs
if(pid[i]==0)//checking for the child processes
{
printf("Child %d pid=%d\n",i+1,getpid());
sleep(i+1);//making every child sleep for 1s
exit(0);//exiting the standard
}
}
wait(NULL);//taking the status of one child
printf("Parent:Any one child completes with wait()\n");//one parent printing its status
waitpid(pid[2],NULL,0);//taking the status of 3rd child
printf("Parent: Child 3 completes with waitpid()\n");//one parent printing 3rd child status
wait(NULL);//last child status
printf("Parent:All the childs completed");//another parent taking the status of the last
}
