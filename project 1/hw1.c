/* Authors : Karampinis Ioannis (02529), Krystamtsis Nikolaos(02542)
 * Deadline : 11/03/2018
 * Programma pou apotelei mia vash dedomenwn opou o xrhsths
 * mporei na prosthetei dedomena apo arxeia,na kanei anazhthsh autwn,na 
 * ta eksagei apo th vash se alla arxeia kai na ta diagrafei.*/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#define SIZE 25

/*O tropos anazhthshs twn periexomenwn twn arxeiwn pou eixame prosthesei mesa sth bash dedomenwn einai 
 * idios stis sunarthseis call_find,call_export,call_delete kai filename_search. H akribh perigrafh
 * ths leitourgias auths ths anazhthshs paratithetai leptomerws sthn sunarthsh call_find*/

/*H sunarthsh auth xrhsimopoieitai gia na anixneuei onomata arxeiwn twn opoiwn ta 
 * perixomena exoume prosthesei mesa sto arxeio ths bashs. An ta entopisei epistrefei
 * thn timh 1, diaforetika epistrefei 0. Th xrhsimopoioume stis sunarthseis:
 * call_import,call_export,call_delete*/
int filename_search(int fd, long int database_size, char *filename){
	int check_read, name_len;
	char temporary[SIZE] = "";
	long int bytes_counter = 0, i, data_size, check_lseek;
    
    check_lseek = lseek(fd, 4, SEEK_SET);
    if(check_lseek < 0){
        perror("lseek");
        exit(1);
    }
	
	for(i = 0; i < database_size - 4; i = i + bytes_counter){
		
		bytes_counter=0;
		
		check_read = read(fd, &name_len, sizeof(int));
		if(check_read < 0){
			perror("read");
            exit(1);
		}
	
		bytes_counter = bytes_counter + check_read;
		
		check_read = read(fd, temporary, name_len);
		if(check_read < 0){
			perror("read");
            exit(1);
		}
		
		bytes_counter = bytes_counter + check_read;
		
		temporary[check_read] = '\0';
		
		if(strcmp(temporary,filename)==0){
			return(1);
		}
		
		check_read = read(fd, &data_size, sizeof(long int));
		if(check_read < 0){
			perror("read");
            exit(1);
		}
		
		bytes_counter = bytes_counter + check_read;
		
		
		check_lseek = lseek(fd, data_size, SEEK_CUR);
		if(check_lseek < 0){
			perror("lseek");
            exit(1);
		}
		
		bytes_counter = bytes_counter + data_size;
	}
	return(0);
}


/*Prosthetei periexomena arxeiwn (pou epilegoume) sth vash dedomenwn*/
void call_import(int fd, char *filename){
	int new_fd, length, check_read, check_write, i,check_if_exist,ftruncate_check;
	char temporary[512] = "";
	long int numOfbytes, check_lseek,base_size;
	
	/*anoigei to arxeio pou epilejame wste sth sunexeia na diabastoun
	 * ta periexomena tou kai na prostethoun sth bash dedomenwn*/
	new_fd = open(filename, O_RDONLY, S_IRWXU);
	if(new_fd < 0){
		/*sthn periptwsh pou to onoma pou dwsame den antistoixei
		 * se kapoio arxeio, tote ektupwnei to parakatw mhnuma*/
		printf("no such file or directory\n");
	}
	else{
	
		length = strlen(filename);
	
		/*upologizetai kai ektupwnetai to sunoliko megethos twn periexomenwn tou arxeiou
		 *(pou theloume na prosthesoume) se bytes*/
		numOfbytes = lseek(new_fd, 0, SEEK_END);
		printf("number of bytes = %ld\n", numOfbytes);
		
		check_lseek = lseek(new_fd, 0, SEEK_SET);
		if(check_lseek < 0){
			perror("lseek");
			exit(1);
		}
			
		/*grafei to mhkos tou onomatos tou arxeiou pou theloume na prosthesoume
		*(me arithmo) mesa sth bash dedomenwn*/
    
		check_write = write(fd, &length,sizeof(int));
		if(check_write < 0){
			perror("write");
			exit(1);
		}
	
		/*grafei to onoma tou arxeiou, tou opoiou ta periexomena theloume na prosthesoume
		*mesa sth bash dedomenwn*/	
		check_write = write(fd, filename, length);
		if(check_write < 0){
			perror("write");
			exit(1);
		}
	
		/*grafei ta sunolika bytes twn periexomenwn tou arxeiou pou theloume 
		*na prosthesoume (me arithmo) mesa sth bash dedomenwn*/
		check_write = write(fd, &numOfbytes, sizeof(long int));
		if(check_write < 0){
			perror("write");
			exit(1);
		}

		/* Prostithontai tmhmatika ta priexomena tou arxeiou sth vash dedomenwn efoson exei megethos
		* megalutero apo 512 Bytes*/
		if(numOfbytes > 512){
			for(i = 0; i < numOfbytes ; i = i + 512){
	
				check_read = read(new_fd, temporary, 512);
				if(check_read < 0){
					perror("read");
					exit(1);
				}
		
				check_write = write(fd, temporary, check_read);
				if(check_write < 0){
					perror("write");
					exit(1);
				}
			}
		}
		/* Prostithontai ta periexomena tou arxeiou sth vash dedomenwn pou exei megethos mikrotero
		* apo 512 Bytes*/
		else{
			
			check_read = read(new_fd, temporary, 512);
			if(check_read <= 0){
				perror("read");
				exit(1);
			}
		
			check_write = write(fd, temporary, check_read);
			if(check_write < 0){
				perror("write");
				exit(1);
			}
		}
		close(new_fd);
		
		/*Ginetai elegxos gia na diapistwthei an to arxeio pou prosthesame uparxei eidh 
		 * sth bash. An uparxei afaireitai auto pou molis prosthesame*/
		
		/*upologizei posa bytes eixe h bash dedomenwn prin prosthesoume
		 * to teleutaio arxeio*/
		base_size=lseek(fd,-(sizeof(int)+length+sizeof(long int)+numOfbytes),SEEK_END);
		if(base_size < 0){
			perror("lseek");
			exit(1);
		}
	
		check_lseek=lseek(fd, 4, SEEK_SET);
		if(check_lseek < 0){
			perror("lseek");
			exit(1);
		}
		
		/*kaloume thn sunarthsh filename_search gia na doume an uparxei eidh to
		 * sugkekrimeno arxeio sth bash (an uparxei epistrefei 1, alliws 0)*/
		check_if_exist = filename_search(fd,base_size,filename);
		
		check_lseek=lseek(fd, 0, SEEK_END);
		if(check_lseek < 0){
			perror("lseek");
			exit(1);
		}
		
		if(check_if_exist == 1){
			/*an uparxei to arxeio sth bash jana tote ektupwnei to parakatw mhnuma
			 * kai sth sunexeia afairei to arxeio pou molis prosthesame*/
			printf("You can't put a file for second time in the base\n");
				
			ftruncate_check = ftruncate(fd, base_size);
			if(ftruncate_check < 0){
				perror("ftruncate");
				exit(1);
			}
			/*xrhsimopoioume edw thn lseek gia na metaferoume ton file descriptor sto telos
			 * ths bashs dedomenwn kai to arxeio ths bashs na meiwthei katallhla, xwris na 
			 * afethoun kena sto eswteriko tou logw ths ftruncate*/
			check_lseek=lseek(fd, 0, SEEK_END);
			if(check_lseek < 0){
				perror("lseek");
				exit(1);
			}
		}
	}
}

/* Anazhtaei sth vash dedomenwn to arxeio pou thelei o xrhsths
 * kai prosdiorizei to arxeio pou moiazei leksikografika.*/
void call_find(int fd, char *search){
	int check_read, name_len;
	long int data_size, bytes_counter = 0, i, check_lseek, full_size;
	char temporary[SIZE];
	
	/*upologizei ta bytes pou perixei h bash dedomenwn*/
	full_size = lseek(fd, 0, SEEK_END);
	if(full_size < 0){
		perror("lseek");
        exit(1);
	}
	/*metakinoume ton file descriptor kata 4 theseis mprosta apo thn arxh ths bashs dedomenwn wste na
	 * mhn xreiazetai na upologizetai kai to magic number sthn epanalhpsh pou akolouthei*/
	check_lseek = lseek(fd, 4, SEEK_SET);
	if(check_lseek < 0){
		perror("lseek");
        exit(1);
	}
	
	/* Ean o xrhsths dwsei ws epilogh "*", ektupwnontai ta onomata twn arxeiwn
	 * ta periexomena twn opoiwn periexontai sth vash dedomenwn*/ 
	if(strcmp(search,"*") == 0){
		/*afairoume 4 apo to full_size, afou den theloume na metrame kai to magic
		 * number pou periexetai mia fora sthn arxh ths bashs dedomenwn*/
		for(i = 0; i < full_size - 4; i = i + bytes_counter){
			
			bytes_counter=0;
			/*diabazei ton arithmo apo thn bash dedomenwn pou antistoixei sto 
			 * mhkos tou onomatos pou eixame prosthesei ki apothikeuei thn timh auth
			 * sth metablhth name_len*/
			check_read = read(fd, &name_len, sizeof(int));
			if(check_read < 0){
				perror("read");
                exit(1);
			}
			
			/*prosthetei 4 bytes (afou diabase enan akeraio) ston counter*/
			bytes_counter = bytes_counter + check_read;
			
			/*diabazei "name_len" bytes kai ta apothikeuei proswrina se enan pinaka
			 * megethous SIZE, auto einai to onoma tou arxeiou pou prosthesame prin sthn bash*/
			check_read = read(fd, temporary, name_len);
			if(check_read < 0){
				perror("read");
                exit(1);
			}
			
			/*prosthetei ston counter ta bytes pou diabase h read*/
			bytes_counter = bytes_counter + check_read;
			/*ftiaxnoume ena string bazontas sth check_read thesi tou pinaka temporary
			 * to '\0' */
			temporary[check_read] = '\0';
	
			printf(" %s\n", temporary);
			
			/*diabazei ton arithmo apo thn bash dedomenwn pou antistoixei sta
			 * bytes pou periexontai sto arxeio pou eixame prosthesei sth bash
			 kai ta apothikeuei sth metablhth data_size*/
			check_read = read(fd, &data_size, sizeof(long int));
			if(check_read < 0){
				perror("read");
                exit(1);
			}
			/*prosthetei 8 bytes (afou diabase ena long int) ston counter*/
			bytes_counter = bytes_counter + check_read;
		
			/*o fd proxwra kata data_size theseis*/
			check_lseek = lseek(fd, data_size, SEEK_CUR);
			if(check_lseek < 0){
				perror("lseek");
                exit(1);
			}
			
			bytes_counter = bytes_counter + data_size;
		}
	}
	/* Entopizei kai ektupwnei ta arxeia (twn opoiwn ta periexomena eixame prosthesei sth bash) 
	 pou periexoun sto onoma tous th symboloseira pou edwse o xrhsths*/
	else{
		for(i = 0; i < full_size - 4; i = i + bytes_counter){
 		
			bytes_counter=0;
			
			check_read = read(fd, &name_len, sizeof(int));
			if(check_read < 0){
				perror("read");
                exit(1);
			}
	
			bytes_counter = bytes_counter + check_read;
		
			check_read = read(fd, temporary, name_len);
			if(check_read < 0){
				perror("read");
                exit(1);
			}
		
			bytes_counter = bytes_counter + check_read;
		
			temporary[check_read] = '\0';
			
			/*elegxei an h sumboloseira pou edwse o xrhsths periexetai se arxeio, tou opoiou
			 * ta periexomena eixame prosthesei sth bash*/
			if(strstr(temporary,search) != NULL){
				printf(" %s\n", temporary);
			}
		
			check_read = read(fd, &data_size, sizeof(long int));
			if(check_read < 0){
				perror("read");
                exit(1);
			}
		
			bytes_counter = bytes_counter + check_read;
		
		
			check_lseek = lseek(fd, data_size, SEEK_CUR);
			if(check_lseek < 0){
				perror("lseek");
                exit(1);
			}
		
			bytes_counter = bytes_counter + data_size;
		}
	}
}

void call_export(int fd, char *source, char *destination){
	int new_fd, check_read, check_write, name_len, dest_len;
	int check_if_exist;
	long int bytes_counter = 0 , i, full_size, data_size, j, check_lseek;
	char temporary[512], filename[SIZE] = "";
	
	/*upologizei to mhkos tou onomatos pou tha exei to arxeio sto opoio tha eksagoume
	 * periexomena apo th bash dedomenwn*/
	dest_len = strlen(destination);
	
	full_size = lseek(fd, 0, SEEK_END);
	if(full_size < 0){
		perror("lseek");
        exit(1);
	}
	
	check_lseek = lseek(fd, 4, SEEK_SET);
	if(check_lseek < 0){
		perror("lseek");
        exit(1);
	}
	
	/*elegxei an uparxoun ta periexomena ,tou arxeiou pou epileksame ,mesa sth bash
	 * (an uparxoun h sunarthsh filename_search tha brei to onoma mesa sth bash kai tha epistrepsei 1)*/
	check_if_exist = filename_search(fd, full_size, source);
	if(check_if_exist == 0){
		printf("The file doesn't exist into the base\n");
	}
	
	check_lseek = lseek(fd, 4, SEEK_SET);
	if(check_lseek < 0){
		perror("lseek");
        exit(1);
	}
	
	for(i = 0; i < full_size - 4; i = i + bytes_counter){
		
		bytes_counter=0;
		
		check_read = read(fd, &name_len, sizeof(int));
		if(check_read < 0){
			perror("read");
            exit(1);
		}
		
		bytes_counter = bytes_counter + check_read;
		
		check_read = read(fd, filename, name_len);
		if(check_read < 0){
			perror("read");
            exit(1);
		}
		
		bytes_counter = bytes_counter + check_read;
		
		filename[check_read] = '\0';
		
		check_read = read(fd, &data_size, sizeof(long int));
		if(check_read < 0){
			perror("read");
            exit(1);
		}
		
		bytes_counter = bytes_counter + check_read;

		if(strcmp(filename, source) == 0){
			/*an brei ta epithumhta periexomena mesa sth bash, tote anoigei ena neo arxeio
			 * me onoma pou exei epileksei o xrhsths kai ta antigrafei ekei*/
			new_fd = open(destination, O_WRONLY|O_CREAT|O_EXCL, S_IRWXU);
			if(new_fd < 0){
				/*an to arxeio eidh uparxei tote emfanizei to parakatw mhnuma*/
				printf("you can't export a file from the base to a file that already exist\n");
				close(new_fd);
				break;
			}
			
			check_write = write (new_fd, &dest_len, sizeof(int));
			if(check_write < 0){
				perror("write");
                exit(1);
			}
			
			check_write = write(new_fd, destination, dest_len);
			if(check_write < 0){
				perror("write");
                exit(1);
			}
			
			check_write = write(new_fd, &data_size, sizeof(long int));
			if(check_write < 0){
				perror("write");
                exit(1);
			}
			
			if(data_size < 512){
				check_read = read(fd, temporary, 512);
				if(check_read < 0){
					perror("read");
                    exit(1);
				}
				
				check_write = write(new_fd, temporary, data_size);
				if(check_write < 0){
					perror("write");
                    exit(1);
				}
			}
			else{
				for(j = 0; j < data_size; j = j + 512){
				
					check_read = read(fd, temporary, 512);
					if(check_read < 0){
						perror("read");
                        exit(1);
					}
					
					if((data_size-j)>512){
						check_write = write(new_fd, temporary, check_read);
						if(check_write < 0){
							perror("write");
                            exit(1);
						}
					}
					else{
						check_write = write(new_fd, temporary, data_size-j);
						if(check_write < 0){
							perror("write");
                            exit(1);
						}
					}
				}
			}
			close(new_fd);
		}
		else{
			check_lseek = lseek(fd, data_size, SEEK_CUR);
			if(check_lseek<0){
				perror("lseek");
                exit(1);
			}
		}
		bytes_counter = bytes_counter + data_size;
	}
}

void call_delete(int fd, char *filename_del, char *database_name){
	long int full_size,i, check_lseek, bytes_counter=0, data_size, j;
	long int position_1, position_2, check_ftruncate;
	int check_if_exist, name_len, check_read, check_write, new_fd;
	char temporary[512] = "", filename[SIZE] = "" ;
	
	full_size = lseek(fd, 0, SEEK_END);
	if(full_size < 0){
		perror("lseek");
        exit(1);
	}
	
	check_lseek = lseek(fd, 4, SEEK_SET);
	if(check_lseek < 0){
		perror("lseek");
        exit(1);
	}
	/*elegxei an exoume prosthesei kai uparxoun sth bash ta periexomena tou arxeiou
	 * pou anazhtoume na diagrapsoume*/
	check_if_exist = filename_search(fd, full_size, filename_del);
	if(check_if_exist == 0){
		printf("The file doesn't exist into the base\n");
	}
	
	check_lseek = lseek(fd, 4, SEEK_SET);
	if(check_lseek < 0){
		perror("lseek");
        exit(1);
	}
	
	for(i=0 ; i < full_size - 4; i = i + bytes_counter){
		
		bytes_counter=0;
		
		check_read = read(fd, &name_len, sizeof(int));
		if(check_read < 0){
			perror("read");
            exit(1);
		}
		
		bytes_counter = bytes_counter + check_read;
		
		check_read = read(fd, filename, name_len);
		if(check_read < 0){
			perror("read");
            exit(1);
		}
		
		filename[check_read] = '\0';
		
		bytes_counter = bytes_counter + check_read;
		
		check_read = read(fd, &data_size, sizeof(long int));
		if(check_read < 0){
			perror("read");
            exit(1);
		}
		
		bytes_counter = bytes_counter + check_read;
		
		if(strcmp(filename, filename_del)==0){
			/*otan brei ta periexomena pou anazhtoume kai theloume na diagrapsoume apo th bash
			 * kratame th thesh pou arxizoun (ta periexomena pou tha afairesoume) mesa sth 
			 * bash, auth einai h position_1*/
			position_1 = lseek(fd, -(sizeof(int)+name_len+sizeof(long int)), SEEK_CUR);
			if(position_1 < 0){
				perror("lseek");
                exit(1);
			}
			
			/*bazoume kai deutero file descriptor mesa sto arxeio ths bashs*/
			new_fd = open(database_name, O_RDONLY, 0700);
			if(new_fd < 0){
				perror("open");
    
			}
			
			/*to position_2 einai h thesi mesa sto arxeio ths bashs pou stamatoun
			 * ta periexomena pou theloume na diagrapsoume*/
			position_2 = lseek(new_fd, position_1 + sizeof(int) + name_len + sizeof(long int) + data_size, SEEK_SET);
			if(position_2 < 0){
				perror("lseek");
                exit(1);
			}
			
			/*Edw metaferei otidhpote exoume grapsei meta th thesi position_2 sto arxeio ths 
			 * kai to grafei sthn thesi position_1. Etsi kaluptontai ousiastika ta periexomena pou
			 * theloume na diagrapsoume me ta periexomena pou briskontai parakatw sto arxeio ths bashs*/
			if((full_size - position_2) < 512){
				check_read = read(new_fd, temporary, 512);
				if(check_read < 0){
					perror("read");
                    exit(1);
				}
				
				check_write = write(fd, temporary, check_read);
				if(check_write < 0){
					perror("write");
                    exit(1);
				}
			}
			else{
				for(j = 0; j < (full_size-position_2); j = j + 512){
					
					check_read = read(new_fd, temporary, 512);
					if(check_read < 0){
						perror("read");
                        exit(1);
					}
				
					check_write = write(fd, temporary, check_read);
					if(check_write < 0){
						perror("write");
                        exit(1);
					}
				}
			}
			close(new_fd);
			/*afairoume apo to telos ths bashs tosa bytes osa ta periexomena pou thelame
			 * na diagrapsoume apo th bash*/
			check_ftruncate = ftruncate(fd,full_size - (position_2 - position_1));
			if(check_ftruncate < 0){
				perror("ftruncate");
                exit(1);
			}
		}
		else{
			check_lseek = lseek(fd, data_size, SEEK_CUR);
			if(check_lseek < 0){
				perror("lseek");
                exit(1);
			}
		}
		bytes_counter = bytes_counter + data_size;
	}
	/*Xrhsimopoioume thn lseek edw gia na phgainei o file descriptor sto
	 * telos tou arxeiou ths bashs kai gia na mhn afhsei keno mesa sto arxeio
	 * ths bashs(logw ftruncate) otan pame na kanoume import ena neo arxeio.
	 * Etsi, meiwnetai katallhla to arxeio ths bashs kata th diagrafh. */
	check_lseek=lseek(fd, 0, SEEK_END);
	if(check_lseek<0){
		perror("lseek");
		exit(1);
	}
}
	
int main(int argc, char *argv[]){
	int fd, check_write;
	char destination[SIZE];
	char option, filename[SIZE] = "", format[30], format_str[30];
	
	fd = open(argv[1], O_RDWR|O_CREAT, S_IRWXU);
	if(fd < 0){
		perror("open");
		exit(1);
	}
    
    /*grafei to magic number sthn arxh tou arxeiou ths bashs*/
	check_write = write(fd, "beef", 4);
	if(check_write < 0){
		perror("write");
		exit(1);
	}
    
	/* Ektupwnei to menou epilogwn gia ton xrhsth*/
	do{
		printf("\ti(mport) <name>\n");
		printf("\tf(ind) <name>\n");
		printf("\te(xport) <src> <dest>\n");
		printf("\td(elete) <name>\n");
		printf("\tq(uit)\n");
		
		
		scanf(" %c", &option);
		
		if(option == 'q'){
			close(fd);
		}
		
		if(option != 'q'){
			sprintf(format, "%%%ds", SIZE-1);
			scanf(format, filename);
		}
		
		if(option == 'i'){
			call_import(fd, filename);
		}
		
		if(option == 'f'){
			call_find(fd, filename);
		}
		
		if(option == 'e'){
			sprintf(format_str,"%%%ds",SIZE-1);
			scanf(format_str ,destination);
			call_export(fd, filename, destination);
		}
		
		if(option == 'd'){
			call_delete(fd,filename,argv[1]);
		}
		
	} while(option != 'q');
	
	return(0);
}
	
