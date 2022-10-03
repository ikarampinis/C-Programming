/* To programma dirlist dexetai ws orisma ena onoma katalogou (h kai to filepath)
 * kai ektupwnei ta arxeia pou periexontai sto sugkekrimeno katalogo,
 * sthn kathierwmenh eksodo */

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<string.h>

int main(int argc, char *argv[]){
	DIR *dir;
	struct dirent *filename;
	
	/*anoigei ton katalogo*/
	dir = opendir(argv[1]);
	if(dir == NULL){
		perror("opendir");
		exit(1);
	}
	else{
		while((filename = readdir(dir)) != NULL){ 
			
			/*elegxei ta periexomena tou katalogou, an einai arxeia ektypwnei 
			 * to onoma tous, an uparxoun upokatalogoi tous agnoei*/
			if(filename->d_type == DT_REG){
				printf("%s/%s\n", argv[1], filename -> d_name);
			}
		}
	}

	closedir(dir);
	return(0);
}
