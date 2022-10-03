/*programma twn karampini ioanni kai krystamtsi nikolaou*/
/*To programma dexetai aithseis apo agents(ta pid tous) kai epilegei an tha 
 * tis dextei analoga me ton arithmo twn agents pou exoun sundethei. Dexetai 
 * krathseis gia aeroporika eishthria kai tis ektypwnei sthn othonh, mexri na 
 *dwthei apo to xrhsth sthn kathierwmenh eisodo to Q. Tote ektupwnei ta process id
 *kai tis sunolikes krathseis twn agents pou einai akomh energoi*/ 
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include<poll.h>
#include<sys/sem.h>
#include"help.h"

#define SIZE 100

struct information{
	char etairia[4];
	char anaxwrhsh[5];
	char afiksh[5];
	int staseis;
	int theseis;
};

struct agent_info{
	int id;
	int tickets;
};

int main(int argc, char *argv[]){
	int check_mkfifo, fd_listen, pid, num, dromologia=0, key, k, check_poll, i;
	int check_shmdt, check_shmctl,shmid;
	int check_entrance,fd_answer,fd[atoi(argv[1])], j,check_pid;
	int sem_key, semid,check_semctl;
	ssize_t check_read;
	struct information *info;
	struct pollfd poll_info[atoi(argv[1])+2];
	struct agent_info agent[atoi(argv[1])];
	FILE *fp;
    char to_agent[SIZE],str[SIZE],elegxos_eksodou[SIZE],to_server[SIZE],format[30];
    
	/*o server briskei posa dromologia einai kataxwrhmena mesa sto arxeio
	 * to opoio tou upodeiksame*/
	fp = fopen(argv[2], "r");
	if(fp == NULL){
		perror("fopen");
		exit(1);
	}
	
	/*gia kathe seira pou diabazei apo to arxeio tha auksanontai kata 1 kai
	 * ta dromologia mexri na epistrepsei h fgets NULL pou tha upodeiksei 
	 * telos arxeiou*/
	while(1){
		if((fgets(str,100,fp))==NULL){
			break;
		}
		
		dromologia++;
	}
	
	/*dhmiourgei ena eponumo pipe apo to opoio tha dexetai ta pid twn agents*/
	check_mkfifo = mkfifo(argv[3], S_IRWXU);
	if(check_mkfifo < 0){
		perror("mkfifo");
		exit(1);
	}
	
	/*dhmiourgei mia omada shmatoforwn*/
	sem_key = ftok(argv[2], 'b');
	if(sem_key < 0){
		perror("ftok");
		exit(1);
	}
	
	semid = semget(sem_key,1,IPC_CREAT|IPC_EXCL|S_IRWXU);
	if(semid < 0){
		perror("semget");
		exit(1);
	}
	
	check_semctl = semctl(semid,0,SETVAL,1);
	if(check_semctl < 0){
		perror("semctl");
		exit(1);
	}
	
	/*dhmiourgia kai desmeush koinoxrhsths mnhmhs*/
	key = ftok(argv[2], 'a');
	if(key < 0){
		perror("ftok");
		exit(1);
	}
	
	shmid = shmget(key, dromologia * sizeof(struct information),IPC_CREAT|IPC_EXCL|S_IRWXU);
	if(shmid < 0){
		perror("shmget");
		exit(1);
	}
	
	info = (struct information *) shmat(shmid, NULL, 0);
	if(info == (struct information*)-1){
		perror("shmat");
		exit(1);
	}
	
	fp = fopen(argv[2], "r");
	if(fp == NULL){
		perror("fopen");
		exit(1);
	}
	
	/*pernaei sthn koinoxrhsth mnhmh tis plhrofories pou mas parexei to arxeio*/
	for(k=0; k < dromologia; k++){
		fscanf(fp,"%s", info[k].etairia);
		fscanf(fp,"%s", info[k].anaxwrhsh);
		fscanf(fp,"%s", info[k].afiksh);
		fscanf(fp,"%d", &info[k].staseis);
		fscanf(fp,"%d", &info[k].theseis);
		
	}
	
	fclose(fp);
	
	
	for(k=0; k < atoi(argv[1]) ; k++){
        agent[k].id = 0;
        agent[k].tickets = 0;
	}
	
	fd_listen = open(argv[3], O_RDONLY);
	if(fd_listen < 0){
		perror("open");
		exit(1);
	}
	
	/*ston pinaka tou poll apothikeuw sth thesh 0 to STDIN_FILENO kai sth thesh
	 * 1 ton file descriptor tou akrou anagnwshs tou pipe pou dexetai tis aithseis
	 * apo ton agent*/
	poll_info[0].fd = STDIN_FILENO;
	poll_info[0].events = POLLIN;
	poll_info[1].fd = fd_listen;
	poll_info[1].events = POLLIN;
	
	
	while(1){
		
		/*o pinakas tou poll exei 2+atoi(arv[1]) theseis, einai autes pou proanaferame
		 * kai oi upoloipes apo thn 2 mexri thn 1+atoi(argv[1]) einai oi file descriptors
		 * gia ta akra anagnwshs twn pipes pou stelnoun plhroforia apo tous agents ston server*/
		check_poll = poll(poll_info, 2 + atoi(argv[1]), 5*1000);
		if(check_poll < 0){
			perror("poll");
			exit(1);
		}
		if(check_poll == 0){
			continue;
		}
		
		for(i=0; i < 2+atoi(argv[1]) ; i++){
			if(poll_info[i].revents & POLLIN){
				printf("trying pipe: %d\n", i);
				
				/*an diabastei kati apo thn prwth thesh tou pinaka, dhladh apo to
				 * STDIN_FILENO*/
				if(i==0){
					sprintf(format,"%%%ds",SIZE-1);
					scanf(format, elegxos_eksodou);
					
					/*elegxei an diabasthke to mhnuma Q gia termatismo tou server*/
					if((strcmp(elegxos_eksodou,"Q"))==0){
						
						close(fd_listen);
						
						/*ektupwnei tis krathseis twn energwn praktorwn*/
                        for(k = 0; k < atoi(argv[1]); k++){
                            if(agent[k].id != 0){
                                printf("\nagent id: %d\n", agent[k].id);
                                printf("arithmos krathsewn: %d\n", agent[k].tickets);
                            }
                        }
						
						/*ananewnei to arxeio me ta dromologia meta tis krathseis pou pragmatopoihthhkan*/
						fp = fopen(argv[2] , "w");
						if(fp == NULL){
							perror("fopen");
							exit(1);
						}
	
						for(k=0; k < dromologia; k++){
		
							fprintf(fp,"%s %s %s %d %d\n", info[k].etairia, info[k].anaxwrhsh, info[k].afiksh, info[k].staseis, info[k].theseis);
						}
	
						fclose(fp);
						
						check_semctl = semctl(semid,0,IPC_RMID);
						if(check_semctl < 0){
							perror("semctl");
							exit(1);
						}
	
						check_shmdt = shmdt(info);
						if(check_shmdt < 0){
							perror("shmdt");
							exit(1);
						}
						check_shmctl = shmctl(shmid,IPC_RMID, NULL);
						if(check_shmctl < 0){
							perror("shmctl");
							exit(1);
						}
						unlink(argv[3]);
						return(0);
					}
				}
				
				/*an labei aithsh apo enan agent*/
				if(i==1){
					check_read = my_read(fd_listen, &pid, sizeof(int), __FILE__, __LINE__);
					
					sprintf(to_agent,"from_server_to_agent_%d", pid);
					sprintf(to_server,"from_agent_to_server_%d", pid);
					
					for(j=0; j< atoi(argv[1]); j++){
						
						/*elegxei an exei sundethei o megistos arithmos praktorwn*/
						/*an den exei sundethei o megistos arithmos tote o agent ginetai
						 * dektos kai tou apostelontai oi katallhles plhrofories kai
						 *h metablhth check_entrance pairnei thn timh 1*/
                        if(agent[j].id == 0){
                            agent[j].id = pid;
							check_entrance = 1;
							
							
							if(check_read > 0){
								printf("agent's pid: %d\n", pid);
							}
							
							fd_answer = open(to_agent, O_WRONLY);
							if(fd_answer < 0 ){
								perror("open");
								exit(1);
							}
							
							my_write(fd_answer, &key, sizeof(int), __FILE__, __LINE__);
							
							my_write(fd_answer, &dromologia, sizeof(int), __FILE__, __LINE__);
							
							my_write(fd_answer,&sem_key,sizeof(int), __FILE__, __LINE__);
							
							/*efoson apostaloun oi katallhles plhrofories pros ton agent, o
							 * server perimenei na anoiksei o agent to akro grapsimatos to 
							 * opoio to kanei amesws me to pou labei ta mhnumata afou prwta
							 * ektupwsei to menu epilogwn */
							fd[j] = open(to_server, O_RDONLY);
							if(fd[j] < 0 ){
								perror("open");
								exit(1);
							}
							
							/*ton file descriptor pou dinei to akro anagnwshs ton
							 * apothikeuoume ston pinaka tou poll*/
							poll_info[j+2].fd = fd[j];
							poll_info[j+2].events = POLLIN;
							
							break;
						}
						check_entrance = 0;
					}
					
					/*an o agent den ginei dektos tote stelnei o server katallhlo
					 * mhnuma gia ton termatismo tou agent*/
					/*katalabainoume oti o agent ginetai dektos an check_entrance==1 ,an 
					 * einai isos me 0 den ginetai dektos*/
					if(check_entrance == 0){
						fd_answer = open(to_agent, O_WRONLY);
						if(fd_answer < 0 ){
							perror("open");
							exit(1);
						}
						my_write(fd_answer,&check_entrance,sizeof(int),__FILE__, __LINE__);
					}
					close(fd_answer);
				}
				
				/*an labei mhnumata gia tis krathseis apo tous agent*/
				if(i > 1){
                    
                    check_read = my_read(fd[i-2],&num,sizeof(int), __FILE__, __LINE__);
                    
                    my_read(fd[i-2],&check_pid,sizeof(int), __FILE__, __LINE__);
                    
                    for(k = 0; k < atoi(argv[1]); k++){
                        if(agent[k].id == check_pid){
                            agent[k].tickets = agent[k].tickets + num;
                        }
                    }
                    
                    if(num != 0){
                        printf("agent %d - krathseis: %d\n", check_pid, num);
                    }
					
					/*an o agent kanei exit tote epistrefei num==0 kai pauei pleon na einai energos
					 * etsi mhdenizoume th thesh tou ston pinaka apo structs pou eixame apothhkeusei
					 * ta stoixeia twn agents, wste na mporei na sundethei kapoios allos agent sth
					 * thesh tou*/
					if(num == 0){
						close(fd[i-2]);
						agent[i-2].id = 0;
                        agent[i-2].tickets = 0;
					}
				}
			}
		}
	}
}
