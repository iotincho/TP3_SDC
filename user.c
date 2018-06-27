//
// Created by sergio on 27/06/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define SIG_TEST 44

/*
 *  handler de la señal enviada por el timer SIG_TEST
 */
void signalFunction(int n, siginfo_t *info, void *unused) {
    printf("timer  expired %d\n", info->si_int);
}

int main (int argc , char *argv[] ) {
    FILE * fp;
    char buff[10];
    struct sigaction sig;
	
    sig.sa_sigaction = signalFunction; // Callback function
    sig.sa_flags = SA_SIGINFO;
    sigaction(SIG_TEST, &sig, NULL);

    printf("The process id is %d\n", getpid());
    
    fp = fopen ("/proc/myModuleFile", "r+");
    
    fwrite("10000" , 1 , sizeof("10000") , fp );
    
    fclose(fp);
    fgets(buff, 10, stdin);
    return(0);
}

