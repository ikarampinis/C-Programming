#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>
#include<sys/time.h>
#include <errno.h>
#include"help.h"

#define SIZE 250

int main(int argc, char *argv[]){
    int check_dir, name_length, check_utimes, check_chmod, i, new_fd;
    char filename[SIZE], filepath[SIZE], temporary[512];
    ssize_t check_read;
    struct timeval time[2];
    struct stat buffer;
    
	/*dhmiourgei neo katalogo kai an uparxei termatizetai to programma afou
	 * prwta ektupwthei katallhlo mhnuma*/
    check_dir = mkdir(argv[1], 0700);
    if(errno == EEXIST){
        fprintf(stderr,"Directory already exists!\n");
        exit(1);
    }
    if(check_dir < 0){
        perror("Creating directory");
        exit(1);
    }
    
    while(1){
        check_read = my_read(STDIN_FILENO, &name_length, sizeof(int), __FILE__, __LINE__);
        
		if(check_read == 0){
			break;
		}
    
        my_read(STDIN_FILENO, filename, name_length, __FILE__, __LINE__);
        
        filename[name_length]='\0';
        
        strcpy(filepath,argv[1]);
        strcat(filepath,"/");
        strcat(filepath,filename);
		
		/*anoigoume arxeio mesa ston neo katalogo pou orisame*/
        new_fd = open(filepath, O_WRONLY|O_CREAT, S_IRWXU);
        if(new_fd < 0){
            perror("Open");
            exit(1);
        }
    
        my_read(STDIN_FILENO, &buffer.st_atime, sizeof(time_t), __FILE__, __LINE__);
    
        my_read(STDIN_FILENO, &buffer.st_mtime, sizeof(time_t), __FILE__, __LINE__);
        
        my_read(STDIN_FILENO, &buffer.st_mode , sizeof(mode_t), __FILE__, __LINE__);
        
        my_read( STDIN_FILENO, &buffer.st_size, sizeof(off_t), __FILE__, __LINE__);
        
        if(buffer.st_size < 512){
			my_read(STDIN_FILENO, temporary, buffer.st_size, __FILE__, __LINE__);
			
			my_write(new_fd, temporary, buffer.st_size, __FILE__, __LINE__);

		}
		else{
			for(i = 0; i < buffer.st_size ; i = i + check_read){
                
				if(buffer.st_size - i >= 512){
                    
					check_read = my_read(STDIN_FILENO, temporary, 512, __FILE__, __LINE__);

				}
				else{
                    
					check_read = my_read(STDIN_FILENO, temporary, buffer.st_size - i, __FILE__, __LINE__);
                    
				}
				
				my_write(new_fd, temporary, check_read, __FILE__, __LINE__);
                
			}
		}
		
		/*kathws grafoume ta arxeia mesa ston neo katalogo apothikeuoume se auta
		 * tis palies adeies prosbashs kai xronous teleutaias prosbashs kai allaghs*/
		
		check_chmod = chmod( filepath, buffer.st_mode);
        if(check_chmod < 0){
            perror("chmod");
            exit(1);
        }
		
		time[0].tv_sec = buffer.st_atime;
        time[0].tv_usec = 0;
        time[1].tv_sec = buffer.st_mtime;
        time[1].tv_usec = 0;
    
        check_utimes = utimes(filepath,time);
        if(check_utimes<0){
            perror("utimes");
            exit(1);
        }
    
		close(new_fd);
    }
    return(0);
}
