/*diabazei ta periexomena apo ena arxeio, ta kruptografei kai ta grafei sthn 
 * kathierwmenh eksodo*/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include"help.h"

int main(int argc, char *argv[]){
	char temporary[512], keyword[strlen(argv[1])], buffer[512];
	int i, j, counter;
	ssize_t check_read;
	
	while(1){
        
		/*diabazei apo thn STDIN_FILENO ta periexomena ana 512 bytes*/
		check_read = my_read(STDIN_FILENO, temporary, 512, __FILE__, __LINE__);
		
		if(check_read == 0){
			break;
		}
		
		counter=0;
		
		/*kruptografei ta periexomena ana strlen(argv[1]) bytes th fora*/
		for(i = 0; i < check_read; i = i + strlen(argv[1]) ){
			
			if(check_read - i < strlen(argv[1]) ){
				for(j = 0; j < check_read - i; j++){
					buffer[counter] = temporary[j+i] ^ keyword[j];
					counter++;
				}
			}
			else{
				for(j = 0; j < strlen(argv[1]); j++ ){
					buffer[counter] = temporary[j+i] ^ keyword[j];
					counter++;
				}
			}
		}
		
		my_write(STDOUT_FILENO, buffer, counter, __FILE__, __LINE__);
        
	}
	return(0);
}
