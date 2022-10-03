#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include"help.h"
#include<signal.h>

int main(int argc, char *argv[]){
	int fd, fd_p2p3[2], fd_p3p4[2], pipe_check,fd_p4p5[2], check_dup2, check_execlp;
	int pid_dirlist, pid_archive, pid_crypt, pid_unarchive, check_kill;
	ssize_t check_read;
	char str[9]="";
	
	if(strcmp(argv[1],"-E")==0){
		
		/*dhmiourgei neo arxeio me onoma pou dinei o xrhsths kai grafei sthn
		 arxh tou to magic number P2CRYPTAR*/
		
		fd = open(argv[4],O_WRONLY | O_CREAT | O_EXCL, 0700);
		if(fd < 0){
			perror("file already exists");
			exit(1);
		}
		
		my_write(fd,"P2CRYPTAR", 9, __FILE__, __LINE__);
		
		/*dhmiourgei to pipe pou sundeei thn diergasia gia to dirlist 
		 *me auth gia to p2archive*/
		pipe_check = pipe(fd_p2p3);
		if(pipe_check < 0){
			perror("pipe");
			exit(1);
		}
		
		/*dhmiourgei to pipe pou sundeei thn diergasia gia to p2archive 
		 *me auth gia to p2crypt*/
		pipe_check = pipe(fd_p3p4);
		if(pipe_check < 0){
			perror("pipe");
			exit(1);
		}
		
		
		/*diergasia pou ektelei to programma dirlist kai anakateuthunei to akro grapsimatos
		 tou pipe anamesa sth dirlist kai sthn archive*/
		pid_dirlist = fork();
		if(pid_dirlist < 0){
			perror("dirlist fork");
			
			/*an apotuxei h fork tote to programma skotwnei
			 * ola ta paidia kai sth sunexeia termatizei*/
			check_kill = kill(-1, SIGKILL);
			if(check_kill < 0){
				perror("kill");
				exit(1);
			}
			
			exit(1);
		}
		
		if(pid_dirlist == 0){
			check_dup2 = dup2(fd_p2p3[1],STDOUT_FILENO);
			if(check_dup2 < 0){
				perror("dup2");
				exit(1);
			}
			
			close(fd_p2p3[0]);
			close(fd_p3p4[0]);
			close(fd_p3p4[1]);
			close(fd_p2p3[1]);
			
			check_execlp = execlp("./dirlist", "dirlist", argv[2] , NULL);
			if(check_execlp < 0){
				perror("execlp");
				exit(1);
			}
			
			return(0);
		}
		
		/*diergasia pou ektelei to programma p2archive kai anakateuthunei to akro anagnwshs
		 * tou pipe anamesa sthn dirlist kai sthn archive kai to akro grapsimatos tou pipe
		 anamesa sthn archive kai sthn crypt*/
		pid_archive = fork();
		if(pid_archive < 0){
			perror("archive fork");
			
			check_kill = kill(-1, SIGKILL);
			if(check_kill < 0){
				perror("kill");
				exit(1);
			}
			
			exit(1);
		}
		
		if(pid_archive == 0){
			
			check_dup2=dup2(fd_p2p3[0],STDIN_FILENO);
			if(check_dup2 < 0){
				perror("dup2");
				exit(1);
			}
			check_dup2=dup2(fd_p3p4[1],STDOUT_FILENO);
						if(check_dup2 < 0){
				perror("dup2");
				exit(1);
			}
			
			close(fd_p2p3[1]);
			close(fd_p3p4[0]);
			close(fd_p2p3[0]);
			close(fd_p3p4[1]);
			
			check_execlp=execlp("./p2archive", "p2archive", NULL);
			if(check_execlp < 0){
				perror("execlp");
				exit(1);
			}
			
			return(0);
		}
		
		/*diergasia pou ektelei to programma p2crypt kai anakateuthunei to akro anagnwshs
		 * tou pipe anamesa sthn archive kai sthn crypt*/
		pid_crypt = fork();
		if(pid_crypt < 0){
			perror("crypt fork");
			
			check_kill = kill(-1, SIGKILL);
			if(check_kill < 0){
				perror("kill");
				exit(1);
			}
			
			exit(1);
		}
		
		if(pid_crypt == 0){
			
			check_dup2=dup2(fd_p3p4[0],STDIN_FILENO);
			if(check_dup2 < 0){
				perror("dup2");
				exit(1);
			}
			check_dup2=dup2(fd,STDOUT_FILENO);
			if(check_dup2 < 0){
				perror("dup2");
				exit(1);
			}
			
			close(fd_p3p4[1]);
			close(fd_p3p4[0]);
			close(fd_p2p3[0]);
			close(fd_p2p3[1]);
			
			check_execlp=execlp("./p2crypt", "p2crypt", argv[3], NULL);
			if(check_execlp < 0){
				perror("execlp");
				exit(1);
			}
			
			close(fd);
			return(0);
		}
		
		close(fd_p2p3[0]);
		close(fd_p2p3[1]);
		close(fd_p3p4[0]);
		close(fd_p3p4[1]);
		
		waitpid(-1, NULL, 0);
		waitpid(-1, NULL, 0);
		waitpid(-1, NULL, 0);
		
	}
	
	if(strcmp(argv[1],"-D")==0){
		
		/*anoigei ena arxeio pou tou dinoume ws orisma*/
		fd = open(argv[4], O_RDONLY, 0700);
		if(fd < 0){
			perror("hw2 open");
			exit(1);
		}
		
		/*diabazei ta 9 prwta bytes tou arxeiou pou anoijame kai elegxei an to magic
		 * number einai to P2CRYPTAR*/
		check_read = my_read(fd, str, 9, __FILE__, __LINE__);
		
		str[check_read]='\0';
		
		if(strcmp(str,"P2CRYPTAR") != 0){
			fprintf(stderr,"the magic number of the chosen file is not P2CRYPTAR\n");
			exit(1);
		}
		
		/*dhmiourgia pipe pou sundeei th diergasia gia th p2crypt me auth gia th p2unarchive*/
		pipe_check = pipe(fd_p4p5);
		if(pipe_check < 0){
			perror("pipe");
			exit(1);
		}
		
		/*diergasia pou ektelei to programma p2crypt kai anakateuthunei to akro grapsimatos
		 * tou pipe anamesa sthn unarchive kai sthn crypt*/
		pid_crypt = fork();
		if(pid_crypt < 0){
			perror("crypt fork");
			
			check_kill = kill(-1, SIGKILL);
			if(check_kill < 0){
				perror("kill");
				exit(1);
			}
			
			exit(1);
		}
        
        if(pid_crypt == 0){
			
            check_dup2=dup2(fd,STDIN_FILENO);
			if(check_dup2 < 0){
				perror("dup2");
				exit(1);
			}
            check_dup2=dup2(fd_p4p5[1],STDOUT_FILENO);
			if(check_dup2 < 0){
				perror("dup2");
				exit(1);
			}
			
            close(fd_p4p5[0]);
            close(fd_p4p5[1]);
			
            check_execlp=execlp("./p2crypt","p2crypt", argv[3], NULL);
			if(check_execlp < 0){
				perror("execlp");
				exit(1);
			}
			
            close(fd);
            return(0);
        }
        
		/*diergasia pou ektelei to programma p2unarchive kai anakateuthunei to akro anagnwshs
		 * tou pipe anamesa sthn unarchive kai sthn crypt*/
        pid_unarchive = fork();
		if(pid_unarchive < 0){
			perror("unarchive fork");
			
			check_kill = kill(-1, SIGKILL);
			if(check_kill < 0){
				perror("kill");
				exit(1);
			}
			
			exit(1);
		}
        
        if(pid_unarchive == 0){
			
            check_dup2=dup2(fd_p4p5[0], STDIN_FILENO);
			if(check_dup2 < 0){
				perror("dup2");
				exit(1);
			}
			
            close(fd_p4p5[0]);
			close(fd_p4p5[1]);
			
            check_execlp=execlp("./p2unarchive","p2unarchive", argv[2], NULL);
			if(check_execlp < 0){
				perror("execlp");
				exit(1);
			}
            
            return(0);
        }
		
        close(fd_p4p5[1]);
        close(fd_p4p5[0]);
        
        waitpid(-1,NULL,0);
        waitpid(-1,NULL,0);
		
    }
    
	return(0);
}
