#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void handler_user(int sig);

int main (int argc , char *argv[] ) {
   FILE * fp;
	char buff[10];
	
	if (signal(SIGUSR1, handler_user) == SIG_ERR){
       perror("signal");
       exit(1);	
	}
	printf("The process id is %d\n", getpid());
   fp = fopen ("/proc/myModuleFile", "r+");
   
   fwrite("10000" , 1 , sizeof("10000") , fp );
   fclose(fp);
   fgets(buff, 10, stdin);
   return(0);
}

void handler_user(int sig){
    printf("Timer expired \n");
}