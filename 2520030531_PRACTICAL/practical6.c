#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
int main()
{
int fd1,fd2;
char buf[100];
char response[100];

printf("Enter message: ");
fgets(buf,sizeof(buf),stdin);

fd1=open("client_fifo",O_WRONLY);
write(fd1,buf,sizeof(buf));
close(fd1);

fd2=open("server_fifo",O_RDONLY);
read(fd2,response,sizeof(response));
close(fd2);

printf("server: %s\n",response);
return 0;
}

//server
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
int main()
{
int fd1, fd2;
char buf[100];
mkfifo("client_fifo", 0666);
mkfifo("server_fifo", 0666);
printf("Server started...\n");
fd1=open("client_fifo",O_RDONLY);
read(fd1,buf,sizeof(buf));
close(fd1);
printf("Client: %s\n",buf);
fd2=open("server_fifo",O_WRONLY);
write(fd2,"Message received by server", sizeof("Message received by server"));
close(fd2);
