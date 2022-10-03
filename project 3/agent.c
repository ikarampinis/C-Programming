/*programma twn karampini ioanni kai krystamtsi nikolaou*/
/*To programma stelnei aithseis se enan server kai an ginoun dektes tote
 * ektupwnetai ena menu epilogwn apo to opoio o xrhsths epilegei
 * thn energeia pou epithumei na ektelestei*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<string.h>
#include<sys/sem.h>
#include<errno.h>
#include<signal.h>
#include"help.h"

#define SIZE 8

struct information{
	char etairia[4];
	char anaxwrhsh[5];
	char afiksh[5];
	int staseis;
	int theseis;
};

int main(int argc, char *argv[]){
	int fd,pid, check_mkfifo, shmid, k, check_shmdt,key,num,check_sigaction;
	int dromologia,counter=0,semid,check_semop,sem_key;
	ssize_t check_write;
	char to_agent[100],to_server[100], choice[SIZE], airline[SIZE], source[SIZE];
	char format[20], destination[SIZE];
	struct information *dedomena;
    struct sembuf op;
	struct sigaction act = {{0}};
	
	
	pid=getpid();
	
	/*ftiaxnei 2 epwnuma pipes, to ena stelnei plhrofories apo ton agent
	 * ston server kai to allo apo ton server ston agent*/
	sprintf(to_server,"from_agent_to_server_%d", pid);
	
	check_mkfifo = mkfifo(to_server,S_IRWXU);
    if(check_mkfifo < 0){
        perror("mkfifo");
        exit(1);
    }
    
	sprintf(to_agent,"from_server_to_agent_%d", pid);
    
    check_mkfifo = mkfifo(to_agent, S_IRWXU);
    if(check_mkfifo < 0){
        perror("mkfifo");
        exit(1);
    }
	
	/*o agent stelnei to process id tou ston server*/
	fd = open(argv[1], O_WRONLY);
	if(fd < 0){
		perror("open");
		exit(1);
	}
	
	my_write(fd, &pid, sizeof(int),__FILE__, __LINE__);
	
	close(fd);
    
	/*o agent perimenei apanthsh apo to server wste na apokthsei prosbash
	 * sthn koinoxrhsth mnhmh*/
	fd = open(to_agent, O_RDONLY);
	if(fd < 0){
		perror("open");
		exit(1);
	}
		
	my_read(fd, &key, sizeof(int), __FILE__, __LINE__);
	
	/*elegxos gia to ean egine dekth h aithsh tou agent apo ton server*/
	/*ean o server epistrepsei key iso me mhden tote o agent ektupwnei 
	 * katallhlo mhnuma kai termatizei, diaforetika apokta prosbash 
	 * sthn koinoxrhsth mnhmh*/
	if(key == 0){
        printf("Number of agents has reached the max number\n");
        unlink(to_agent);
        unlink(to_server);
        return(0);
    }
	
	my_read(fd, &dromologia, sizeof(int), __FILE__, __LINE__);
	
	my_read(fd, &sem_key, sizeof(int), __FILE__, __LINE__);
	
	semid = semget(sem_key,1,0);
    if(semid < 0){
        perror("semid");
        exit(1);
    }
		
	close(fd);
	
	shmid = shmget(key,0,0);
	if(shmid < 0){
		perror("shmget");
		exit(1);
	}
	
	dedomena = (struct information *) shmat(shmid, NULL, 0);
	if(dedomena == (struct information*)-1){
		perror("shmat");
		exit(1);
	}
	
	/*kanoume ton agent na agnoei to shma SIGPIPE an kai efoson tou stalei, wste
	 * na mhn kleinei apotoma se periptwsh pou mia write epistrepsei to error EPIPE*/
	act.sa_handler = SIG_IGN;
	check_sigaction = sigaction(SIGPIPE, &act, NULL);
	if(check_sigaction < 0){
		perror("sigaction");
		exit(1);
	}
	
	fd = open(to_server, O_WRONLY);
	if(fd < 0){
		perror("open");
		exit(1);
	}
    
	while(1){
		printf("\nFIND SRC DEST NUM\n");
		printf("RESERVE SRC DEST AIRLINE NUM\n");
		printf("EXIT\n");
		printf("==>");
		
		/*meiwnei to shmatoforo kata 1, etsi an ginei dektos kai allos agent
		 * prepei na perimenei prwta na ektelesei mia opoiadhpote energeia 
		 * tou o energos agent*/
		op.sem_num = 0;
		op.sem_op = -1;
		op.sem_flg = 0;
		check_semop = semop(semid,&op,1);
		/*se periptwsh pou kleisei o server kai o agent einai akomh energos*/
		if(check_semop < 0){
			printf("\nserver has been closed\n");
			close(fd);
	
			check_shmdt = shmdt(dedomena);
			if(check_shmdt < 0){
				perror("shmdt");
				exit(1);
			}
	
			unlink(to_agent);
			unlink(to_server);
	
			return(0);
		}
		
		/*diabazei thn epilogh apo to menu*/
		sprintf(format,"%%%ds", SIZE-1);
		scanf(format, choice);
		
		if((strcmp(choice,"FIND"))==0){
			
			/*diabazei aerodromio anaxwrhshs*/
			sprintf(format,"%%%ds", SIZE-1);
			scanf(format, source);
			
			/*diabazei aerodromio afikshs*/
			sprintf(format,"%%%ds", SIZE-1);
			scanf(format, destination);
			
			/*diabazei arithmo eishthriwn*/
			scanf("%d", &num);
			
			for(k=0; k < dromologia; k++){
				if(((strcmp(source,dedomena[k].anaxwrhsh))==0)&&((strcmp(destination,dedomena[k].afiksh))==0)&&(num<=dedomena[k].theseis)){
					printf("%s %s %s %d %d\n", dedomena[k].etairia, dedomena[k].anaxwrhsh, dedomena[k].afiksh, dedomena[k].staseis, dedomena[k].theseis);
				}
			}
			
			op.sem_num = 0;
			op.sem_op = 1;
			op.sem_flg = 0;
			check_semop = semop(semid,&op,1);
			/*se periptwsh pou kleisei o server kai o agent epileksei th FIND
			 * apo to menu epilogwn, wste o agent na kleinei me ton katallhlo tropo*/
			if(check_semop < 0){
				printf("\nserver has been closed\n");
				close(fd);
	
				check_shmdt = shmdt(dedomena);
				if(check_shmdt < 0){
					perror("shmdt");
					exit(1);
				}
	
				unlink(to_agent);
				unlink(to_server);
	
				return(0);
			}
		}
		
		
		if((strcmp(choice,"RESERVE"))==0){
			
			/*diabazei aerodromio anaxwrhshs*/
			sprintf(format,"%%%ds", SIZE-1);
			scanf(format, source);
			
			/*diabazei aerodromio afikshs*/
			sprintf(format,"%%%ds", SIZE-1);
			scanf(format, destination);
			
			/*diabazei thn epilogh ths etairias*/
			sprintf(format,"%%%ds", SIZE-1);
			scanf(format, airline);
			
			/*arithmos eishthriwn gia krathsh*/
			scanf("%d", &num);
			
			for(k=0; k < dromologia; k++){
				
				/*elegxos gia to an uparxoun arketa eishthria*/
				/*an den uparxoun tote exoume balei to counter na pairnei timh 1*/
				if(((strcmp(source,dedomena[k].anaxwrhsh))==0)&&((strcmp(destination,dedomena[k].afiksh))==0)&&((strcmp(airline,dedomena[k].etairia))==0)&&(dedomena[k].theseis-num<0)){
					
					printf("Not enough tickets!\n");
					counter = 1;
				}
				
				/*Psaxnei na brei an uparxei dromologio pou ikanopoiei
				 * tis apaithseis tou xrhsth. An uparxei stelnei ston
				 * server to process id kai ta eishthria pou kraththhkan,
				 * meta ektupwnei katallhlo munhma kai o counter pairnrei
				 * timh 1*/
				if(((strcmp(source,dedomena[k].anaxwrhsh))==0)&&((strcmp(destination,dedomena[k].afiksh))==0)&&((strcmp(airline,dedomena[k].etairia))==0)&&(num<=dedomena[k].theseis)){
					
					dedomena[k].theseis = dedomena[k].theseis - num;
					
					check_write = my_write(fd, &num, sizeof(int), __FILE__, __LINE__);
					/*an check_write == 0 shmainei oti exei epistrepsei to error
					 * EPIPE pou shmainei oti to akro anagnwshs exei kleisei to opoio 
					 * sumbainei otan termatistei o server*/
					if(check_write == 0){
						printf("server has been closed\n");
						
						close(fd);
	
						check_shmdt = shmdt(dedomena);
						if(check_shmdt < 0){
							perror("shmdt");
							exit(1);
						}
	
						unlink(to_agent);
						unlink(to_server);
	
						return(0);
					}
					
					my_write(fd, &pid, sizeof(int), __FILE__, __LINE__);
					
					printf("\nEPITYXHS KRATHSH EISHTHRION\n");
					
					counter = 1;
				}
			}
			
			/*an tipota apo ta duo parapanw den exei sumbei tote o counter = 0
			 * kai ektupwnei mhnuma oti h sugkekrimenh pthsh den uparxei*/
			if(counter == 0){
				printf("This flight doesn't exists!\n");
			}
			counter = 0;
			
			op.sem_num = 0;
			op.sem_op = 1;
			op.sem_flg = 0;
			check_semop = semop(semid,&op,1);
			if(check_semop < 0){
				printf("\nserver has been closed\n");
				break;
			}
		}
		
		
		if((strcmp(choice,"EXIT"))==0){
			/*an o xrhsths epileksei na kanei exit tote o agent stelenei ston 
			 * server arithmo krathsewn 0 kai to pid tou*/
			num = 0;
			
			check_write = my_write(fd, &num, sizeof(int), __FILE__, __LINE__);
			/*an check_write == 0 shmainei oti exei epistrepsei to error
			* EPIPE pou shmainei oti to akro anagnwshs exei kleisei to opoio 
			* sumbainei otan termatistei o server*/
			if(check_write == 0){
				printf("server has been closed\n");
				break;
			}
			
			my_write(fd, &pid, sizeof(int), __FILE__, __LINE__);
			
			op.sem_num = 0;
			op.sem_op = 1;
			op.sem_flg = 0;
			check_semop = semop(semid,&op,1);
			if(check_semop < 0){
				perror("semop");
				exit(1);
			}
			
			break;
		}
		
		/*se periptwsh pou o xrhsths den epileksei tipota apo to menu epilogwn
		 * tote gia na mhn kolhsei o agent auksanoume ton shmatoforo kata 1*/
		if((strcmp(choice,"FIND"))!=0 && (strcmp(choice,"RESERVE"))!=0 && (strcmp(choice,"EXIT"))!=0){
			op.sem_num = 0;
			op.sem_op = 1;
			op.sem_flg = 0;
			check_semop = semop(semid,&op,1);
			/*se periptwsh pou exei termatistei o server epistrefetai to error EIDRM*/
			if(check_semop < 0){
				printf("\nserver has been closed\n");
				break;
			}
		}
	}
	
	close(fd);
	
	check_shmdt = shmdt(dedomena);
	if(check_shmdt < 0){
		perror("shmdt");
		exit(1);
	}
	
	unlink(to_agent);
    unlink(to_server);
	
	return(0);
}
