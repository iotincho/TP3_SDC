#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#define SIG_TEST 44
#define BUF_LEN 10
/*
 *  handler de la señal enviada por el timer SIG_TEST
 */
void signalFunction(int n, siginfo_t *info, void *unused) {
    printf("timer  expired %d\n", info->si_int);
    exit(0);
}

int main (int argc , char *argv[] ) {
    FILE * fp;
    char buffer[10];
    struct sigaction sig;
    
    sig.sa_sigaction = signalFunction; // Callback function
    sig.sa_flags = SA_SIGINFO;
    sigaction(SIG_TEST, &sig, NULL);

    /* try open driver file */
    fp = fopen ("/proc/myModuleFile", "r+");
    if(fp == NULL){
        perror("Driver file not found");
        exit(1);
    }

    /*prompt*/
    printf("Time for timer [ms]: ");
    
    /* clear buffer and await input */
    memset(buffer,'\0',BUF_LEN);
    fgets(buffer,BUF_LEN,stdin);
    
    /* use driver */
    fwrite(buffer, sizeof(char), BUF_LEN , fp);
    
    fclose(fp);
    printf("await for timer expires\n");
    while(1);
    return(0);
}
