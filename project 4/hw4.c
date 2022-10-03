/*programma tou karampini ioanni kai tou krystamtsi nikolaou*/
/*To programma trexei programmata, ta opoia diaxeirizetai me th bohtheia
 * enos ksupnhthriou. Ta topothetei se mia lista kai mporei na tous stelnei
 * to shma sigusr1, sigterm kai to sigkill otan prokeitai na termatisei 
 * to programma*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<poll.h>
#include<errno.h>

#define SIZE 250
#define NO_SIGNAL 0

struct list{
	int pid;
	char *onoma;
	char **orismata;
	struct list *next;
	struct list *prev;
};

struct list *head;
/*deikths pou deixnei to programma pou trexei*/
struct list *running_proc;

static volatile sig_atomic_t gotsig = NO_SIGNAL;

void init_list(){
	head = (struct list *)malloc(sizeof(struct list));
	if(head==NULL){
		perror("problem in initialiazation");
		exit(1);
	}
	
	head->next = head;
	head->prev = head;
	
	running_proc = head;
}

/*h synarthsh prosthetei neo kombo sto telos ths listas, mesa sto kombo
 * apothikeuw ta xarakthristika enos programmatos pou trexei to programma mas*/
void add_list(int pid, char *name, char **args, int counter){
	struct list *new;
	
	new = (struct list*)malloc(sizeof(struct list));
	if(new==NULL){
		perror("problem in add");
		exit(1);
	}
	
	new->pid = pid;
    
	new->onoma  = strdup(name);
	if(new->onoma == NULL){
		perror("strdup");
		exit(1);
	}
	
	new->orismata = args;
	
	new->next = head;
	new->prev = head->prev;
	new->next->prev = new;
    new->prev->next = new;
}


void print_list(){
	int i;
	struct list *curr;
	
	for(curr = head->next; curr != head ; curr = curr->next){
		printf("pid: %d, ", curr->pid);
		printf("name: (%s ", curr->onoma);
		for(i=1; curr->orismata[i] != NULL; i++){
			printf(", %s", curr->orismata[i]);
		}
		/*ektupwnei R an to curr einai to programma pou trexei*/ 
		if(running_proc == curr){
			printf(")(R)\n");
		}
		else{
			printf(")\n");
		}
	}
}


void delete_list(int pid){
	int i;
    struct list *curr;
    
    head->pid = pid;
    
    for(curr = head->next; curr->pid != pid ; curr = curr->next){}
    
    /*an uparxei enas kombos mesa sth lista kai pame na ton diagrapsoume
     * tote bazoume ton running_proc na deixnei sth head( curr->next )*/
    if((curr == running_proc) && (curr->next == head) && (curr->prev == head)){
		running_proc = curr->next;
	}
        
    if(curr != head){
            
        curr->next->prev = curr->prev;
        curr->prev->next = curr->next;
		
		for(i=0; curr->orismata[i] != NULL; i++){
			free(curr->orismata[i]);
		}
		free(curr->onoma);
		free(curr->orismata);
        free(curr);
    }
    head->pid = 0;
}


void destroy_list(){
	int i, check_kill;
    struct list *curr;
    
    /*stelnw SIGKILL se oles tis diergasies kai apeleutherwnw th 
     * dunamika desmeumenh mnhmh*/
    for(curr = head->next; curr != head; curr = curr->next){
		
		check_kill = kill(curr->pid, SIGKILL);
		if(check_kill < 0){
			perror("kill");
			exit(1);
		}
		
		for(i=0; curr->orismata[i] != NULL; i++){
			free(curr->orismata[i]);

		}
		free(curr->onoma);
		free(curr->orismata);
        free(curr);
    }
    
    free(head);
}

/*elegxei an h lista einai adeia*/
/*an einai adeia h synarthsh epistrefei 1, diaforetika 0*/
int empty_list(){
	
	if(head == head->next){
		return(1);
	}
	else{
		return(0);
	}
}

/*allazei thesh ston running_proc kai deixnei sthn epomenh diergasia
 * pou exei apothikeutei sth lista, parallhla stamataei kai ksekinaei diergasies*/
void next_process(){
	int check_kill;
	
    /*se periptwsh pou h lista htan adeia kai prosthesoume enan kombo, tote 
     * o running_proc deixnei se auto to kombo kai stelnei SIGCONT sthn antistoixh
     * diergasia gia na ksekinhsei*/ 
	if(head == running_proc){
		running_proc = head->next;
		
		check_kill = kill(running_proc->pid, SIGCONT);
		if(check_kill < 0){
			perror("kill1");
			exit(1);
		}
		
		return;
	}
	
	/*se periptwsh pou h lista den einai adeia kai periexei 
     * toulaxiston enan kombo*/
    /*o running_proc deixnei sthn epomenh diergasia, enw parallhla sthn prohgoumenh
     * stelnetai SIGSTOP kai sthn epomenh SIGCONT*/
	if((running_proc != head)){
		
        /*stelnontas to shma 0 elegxo an uparxei auth h diergasia akoma kai 
         den exei termatisei*/
        check_kill = kill(running_proc->pid, 0);
		if(check_kill == 0){
			check_kill = kill(running_proc->pid, SIGSTOP);
            if(check_kill < 0){
                perror("kill STOP");
                exit(1);
            }
        }
		
		running_proc = running_proc->next;
        if(running_proc == head){
            running_proc = head->next;
        }
		
		check_kill = kill(running_proc->pid, 0);
		if(check_kill == 0){
			check_kill = kill(running_proc->pid, SIGCONT);
			if(check_kill < 0){
				perror("kill CONTINUOUS");
				exit(1);
			}
		}
            
		return;
	}
}

void alarm_fuction(){
	int check_empty_list;
	
	check_empty_list = empty_list();

	/*an h lista einai adeia*/
	if(check_empty_list == 1){
		return;
	}
	
	/*an den einai adeia kai exw prosthesei toulaxiston enan kombo*/
	next_process();
    
	return;
}

/*handler gia to shma SIGUSR1*/
/*stelnw se kathe diergasia pou periexei h lista to SIGUSR1*/
static void handler_sigusr1(int sig_sigusr1){
    int check_kill;
    struct list *curr;
    
    for(curr = head->next; curr != head; curr = curr->next){
        
        check_kill = kill(curr->pid, SIGUSR1);
        if(check_kill < 0){
            perror("kill handler_sigusr1");
            exit(1);
        }
    }
}
    
/*handler gia to shma SIGALRM*/
static void handler_alarm(int sig_alarm){
	printf("BEEP!\n");
	alarm_fuction();
}

/*handler gia to shma SIGCHLD*/
static void handler_wait(int sig_wait){
	
	gotsig = sig_wait;
	
}

int main(int argc, char *argv[]){
	char **args, *token, *progname, choice[SIZE], *menu_choice, *prog_id;
    int pid,counter,check_execvp, check_kill, status;
    int check_setitimer, check_poll, check_sigaction, check_waitpid;
	size_t check_read;
	struct sigaction act_waitpid = {{0}};
	struct sigaction act_alarm = {{0}};
    struct sigaction act_sigusr1 = {{0}};
	struct itimerval time = {{0}};
    struct pollfd poll_buf[1];
    
    init_list();
    /*ektupwnw to pid tou patera se periptwsh pou thelw na tou steilw kapoio shma*/
    printf("parent pid: %d\n", getpid());
	
	act_alarm.sa_handler = handler_alarm;
	act_alarm.sa_flags = SA_RESTART;
	check_sigaction = sigaction(SIGALRM, &act_alarm, NULL);
	if(check_sigaction < 0){
		perror("sigaction for SIGALRM");
		exit(1);
	}
	
	time.it_value.tv_sec = 20;
	time.it_value.tv_usec = 0;
	time.it_interval.tv_sec = 20;
	time.it_interval.tv_usec = 0;
	
	check_setitimer = setitimer(ITIMER_REAL, &time, NULL);
	if(check_setitimer < 0){
		perror("setitimer");
		exit(1);
	}
	
	act_waitpid.sa_handler = handler_wait;
	act_waitpid.sa_flags = SA_RESTART;
	check_sigaction = sigaction(SIGCHLD, &act_waitpid, NULL);
	if(check_sigaction < 0){
		perror("sigaction for SIGCHLD");
		exit(1);
	}
	
	act_sigusr1.sa_handler = handler_sigusr1;
	act_sigusr1.sa_flags = SA_RESTART;
	check_sigaction = sigaction(SIGUSR1, &act_sigusr1, NULL);
	if(check_sigaction < 0){
		perror("sigaction for SIGUSR1");
		exit(1);
	}
	
	/*kanw poll kai thn bazw na elegxei thn kathierwmenh eisodo mono*/
    poll_buf[0].fd = STDIN_FILENO;
    poll_buf[0].events = POLLIN;
    
    while(1){
        
        /*se periptwsh pou kapoio paidi exei steilei shma to gotsig = SIGCHLD*/
		if(gotsig == SIGCHLD){
			
            /*elegxei poio paidi esteile shma*/
			while((check_waitpid=waitpid(-1,&status,WNOHANG)) > 0){
				
                /*elegxei tis periptwseis to paidi na termatise apo mono tou h apo kapoio shma*/
				if(WIFEXITED(status) || WIFSIGNALED(status)){
                    
                    /*otan stamatisei ena paidi xtupaei to ksupnhthri*/
                    /*sth sunexeia mesw ths alarm_fuction kai ths next_process
                     * o running_proc tha deixnei thn epomenh diergasia*/
					time.it_value.tv_sec = 0;
					time.it_value.tv_usec = 1;
					time.it_interval.tv_sec = 20;
					time.it_interval.tv_usec = 0;
                
					check_setitimer = setitimer(ITIMER_REAL, &time, NULL);
					if(check_setitimer < 0){
						perror("setitimer");
						exit(1);
					}
                    
                    /*to paidi pou termatise to diagrafw apo th lista*/
					delete_list(check_waitpid);
					printf("%d DELETED\n", check_waitpid);
				}
			}
			
			gotsig = NO_SIGNAL;
		}
        
        /*me th xrhsh ths poll petuxainw: an grapsw kati sthn kathierwmenh eisodo na elegxw ti
         * leitourgia na kanw(na prosthesw kati sth lista, na diagrapsw apo th lista k.l.p.), enw 
         an mou epistrepsei 0 (otan den exw grapsei kati) me thn entolh continue na arxizw thn 
         epanalhpsh apo thn arxh kai na elegxw ana ena deyterolepto an exei termatisei kapoio apo
         ta programmata pou trexw*/
        check_poll = poll(poll_buf, 1, 1000);
        if((errno != EINTR) && (check_poll < 0)){
            perror("poll");
            exit(1);
        }
        
        if(check_poll == 0){
            continue;
        }
        
        /*elegxos an exw grapsei kati sthn kathierwmenh eisodo*/
        if(poll_buf[0].revents && POLLIN){
            
            /*auto pou exw grapsei sthn kathierwmenh eisodo to pernaw sto choice*/
            check_read = read(STDIN_FILENO, choice, SIZE);
            if(check_read < 0){
                perror("read");
                exit(1);
            }
            
            /*ftiaxnw ena string to opoio apotelei ousiastika thn epilogh tou xrhsth*/
            /*bazw to \0 sthn thesh check_read-1 gia na mhn upologizetai to enter pou pataw
             * otan teleiwnw auta pou thelw na grapsw sthn kathierwmenh eisodo*/
            choice[check_read-1] = '\0';
            
            /*me thn strtok spaw to string choice se mikrotera strings pou ta ksexwrizw me ta
             * kena pou periexontai mesa sto choice*/
            /*menu_choice einai h epilogh apo to menu (exec, sig,term, list, quit)*/
            menu_choice = strtok(choice," ");
            
            if((strcmp(menu_choice,"exec"))==0){
                
                /*to onoma tou programmatos pou theloume na treksei*/
                progname = strtok(NULL," ");
                counter=0;
                
                /*desmeuoume tupika mnhmh gia mia seira tou pinaka args*/
                args = (char**) malloc(1 * sizeof(char*));
                    
                while(1){
                    /*gia kathe orisma desmeuoume kai mia nea seira ston pinaka args*/
                    if(counter != 0){
                        args = (char**) realloc(args, (counter+1) * sizeof(char*));
                        if(args == NULL){
                            printf("malloc problem\n");
                            exit(1);
                        }
                    }
                    
                    /*sthn prwth seira tou pinaka apo orismata topothetw to onoma tou programmatos*/
                    if(counter == 0){
						token = strrchr(progname, '/');
						if(token == NULL){
							args[counter] = (char*) malloc((strlen(progname) + 1) * sizeof(char));
							strcpy(args[counter],progname);
						}
						else{
							args[counter] = (char*) malloc(strlen(token) * sizeof(char));
							strcpy(args[counter], &token[1]);
						}
					}
					/*desmeuoume mnhmh gia kathe seira ksexwrista analoga me to mhkos tou string*/
					else{
						token = strtok(NULL," ");
						if(token == NULL){
                            /*apothikeuw sthn teleutaia seira to null ws teleutaio orisma*/
							args[counter] = NULL;
							break;
						}
                    
						args[counter] = (char*) malloc((strlen(token) + 1) * sizeof(char));
						if(args[counter] == NULL){
							perror("malloc problem");
							exit(1);
						}
						strcpy(args[counter],token);
					}
                    counter++;
                }
                
                pid = fork();
                if(pid<0){
                    perror("fork");
                    exit(1);
                }
                
                /*epanaferw ton default xeirismo tou SIGUSR1*/
                act_sigusr1.sa_handler = SIG_DFL;
				act_sigusr1.sa_flags = SA_RESTART;
				check_sigaction = sigaction(SIGUSR1, &act_sigusr1, NULL);
				if(check_sigaction < 0){
					perror("sigaction for SIGUSR1");
					exit(1);
				}
                
                if(pid == 0){
                    check_execvp = execvp(progname , args);
                    if(check_execvp == -1){
                        perror("execvp");
                        exit(1);
                    }
                    return(0);
                }
                /*kanw stop th diergasia*/
                /*oles oi diergasies einai stamatimenes sthn arxh*/
                check_kill = kill(pid, SIGSTOP);
                if(check_kill < 0){
                    perror("kill");
                    exit(1);
                }
                
                act_sigusr1.sa_handler = handler_sigusr1;
				act_sigusr1.sa_flags = SA_RESTART;
				check_sigaction = sigaction(SIGUSR1, &act_sigusr1, NULL);
				if(check_sigaction < 0){
					perror("sigaction for SIGUSR1");
					exit(1);
				}
                
                add_list(pid, progname, args, counter);
            }
            
            if((strcmp(menu_choice,"list"))==0){
                print_list();
            }
            
            if((strcmp(menu_choice,"term"))==0){
                prog_id = strtok(NULL," ");
                if(prog_id == NULL){
                    perror("strtok term");
                    exit(1);
                }
                
                check_kill = kill(atoi(prog_id), SIGTERM);
                if(check_kill<0){
                    perror("kill");
                    exit(1);
                }
            }
            
            if((strcmp(menu_choice,"sig"))==0){
                prog_id = strtok(NULL," ");
                if(prog_id == NULL){
                    perror("strtok sig");
                    exit(1);
                }
                
                check_kill = kill(atoi(prog_id), SIGUSR1);
                if(check_kill<0){
                    perror("kill");
                    exit(1);
                }
            }
            
            if((strcmp(menu_choice,"quit"))==0){
                
                destroy_list();
                
                return(0);
            }
        }
    }
}
            

