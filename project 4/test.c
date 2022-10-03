/*programma tou karampini ioanni kai tou krystamtsi nikolaou*/
/*to programma auksanei enan metrhth ana 5 sec mexri na ftasei to orio 
 * pou exoyme orisei*/
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/types.h>

#define NOT_SIGUSR1 0

static volatile sig_atomic_t gotsig = NOT_SIGUSR1;

static void handler(int sig){
	gotsig = sig;
}

int main(int argc, char *argv[]){
	int max, check_signal, counter=0, check_sigemptyset, check_sigprocmask;
	int check_sigaddset, check_sigaction;
	sigset_t set;
	struct sigaction act = {{0}};
	
	max = atoi(argv[2]);
	check_signal = atoi(argv[4]);
	
	check_sigemptyset = sigemptyset(&set);
	if(check_sigemptyset < 0){
		perror("sigemptyset");
		exit(1);
	}
		
	check_sigaddset = sigaddset(&set,SIGUSR1);
	if(check_sigaddset < 0){
		perror("sigaddset");
		exit(1);
	}
	
	/*an exoume orisma -b 1 tote to SIGUSR1 mplokaretai mexri o counter na ftasei
     * sto miso tou oriou*/
	if(check_signal == 1){
		check_sigprocmask = sigprocmask(SIG_BLOCK, &set, NULL);
		if(check_sigprocmask < 0){
			perror("sigprocmask");
			exit(1);
		}
	}
	
    act.sa_handler = handler;
    check_sigaction = sigaction(SIGUSR1, &act, NULL);
    if(check_sigaction < 0){
        perror("sigaction");
        exit(1);
    }
	
	while(counter < max){
		
		if((check_signal == 1) && (counter == max/2)){
			check_sigprocmask = sigprocmask(SIG_UNBLOCK, &set, NULL);
			if(check_sigprocmask < 0){
				perror("sigprocmask");
				exit(1);
			}
		}
		
		sleep(5);
		
        /*an lhfthei to SIGUSR1 epanaferei ton counter sto 0*/
		if(gotsig==SIGUSR1){
            counter=0;
            gotsig=NOT_SIGUSR1;
		}
		
		counter++;
		printf("pid: %d , counter: %d/%d\n", getpid(), counter, max);
	}
	
	return(0);
}
