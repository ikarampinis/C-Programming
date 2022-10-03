/*h p2archive diabazei arxeia kai grafei to onoma tous, stoixeia tous kai ta 
 * periexomena tous se ena arxeio*/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>
#include"help.h"

#define SIZE 200


int main(int argc, char *argv[]){
	char filename[SIZE],format[20], temporary[512];
	char *res;
	int name_length, check_stat, i, new_fd,check_scanf;
    ssize_t check_read; 
    
	struct stat buffer;
	
	while(1){
		/*diabazei to onoma enos arxeiou se morfh filepath*/
		sprintf(format, "%%%ds", SIZE-1);
		check_scanf=scanf(format, filename);
		
		if(check_scanf == EOF){
			break;
		}
		
		/*sugkratei sth sumboloseira res to onoma tou arxeiou xwris to filepath
		 * kai ena '/' prin apo auto*/
		res=strrchr(filename,'/');
		
		check_stat=stat(filename, &buffer );
		if(check_stat<0){
			perror("stat");
			exit(1);
		}
		
		/*to mhkos tou onomatos tou arxeiou xwris to '/' */
		name_length = strlen(res) - 1;
		
		my_write(STDOUT_FILENO, &name_length, sizeof(int), __FILE__, __LINE__);
        
		my_write(STDOUT_FILENO, res + 1, name_length, __FILE__, __LINE__);
		
		my_write(STDOUT_FILENO, &buffer.st_atime, sizeof(time_t), __FILE__, __LINE__);
		
		my_write(STDOUT_FILENO, &buffer.st_mtime, sizeof(time_t), __FILE__, __LINE__);
		
		my_write(STDOUT_FILENO, &buffer.st_mode, sizeof(mode_t), __FILE__, __LINE__);
		
		my_write(STDOUT_FILENO, &buffer.st_size, sizeof(off_t), __FILE__, __LINE__);
		
		/*anoigei to arxeio wste na diabastoun ta periexomena tou*/
		new_fd = open(filename, O_RDONLY, 0700);
		if(new_fd < 0){
			perror("open");
			exit(1);
		}
		
		if(buffer.st_size < 512){
			my_read(new_fd, temporary, 512, __FILE__, __LINE__);
			
			my_write(STDOUT_FILENO, temporary, buffer.st_size, __FILE__, __LINE__);
            
		}
		else{
			for(i=0; i < buffer.st_size ; i = i + check_read){
				check_read = my_read(new_fd, temporary, 512, __FILE__, __LINE__);
				
				my_write(STDOUT_FILENO, temporary, check_read, __FILE__, __LINE__);

			}
		}
		close(new_fd);
	}
	return(0);
}
