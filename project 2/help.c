#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

/*kai stis duo sunarthseis xrhsimopoiw tis duo teleutaies parametrous
 * wste se periptwsh sfalmatos na tupwthoun sthn kathierwmenh eksodo
 * to onoma tou arxeiou kai h seira sthn opoia egine klhsh ths sunarthshs*/

ssize_t my_read(int fd, void *buf, size_t count,char *file, int line){
    size_t check_read, total_read;
    
    check_read = read(fd, buf, count);
    if(check_read < 0){
        perror("read");
        printf("file: %s and line: %d\n", file, line);
        exit(1);
    }
    total_read = check_read;
    
	/*h read epanalambanetai wste na diabastoun osa bytes epithumoume, se periptwsh
	 * pou de diabastoun apo thn prwth read*/
    while(check_read != 0){
        
        check_read = read(fd, buf + total_read, count-total_read);
        total_read = total_read + check_read;
        if(check_read < 0){
            perror("read");
            printf("file: %s and line: %d\n", file, line);
            exit(1);
        }
    }
    return(total_read);
}

ssize_t my_write(int fd, void *buf, size_t count, char *file, int line){
    size_t check_write,total_write;
    
    check_write = write(fd, buf, count);
    if(check_write < 0){
        perror("write");
        printf("file: %s and line: %d\n", file, line);
        exit(1);
    }
    if(check_write != count){
        total_write = check_write;
		
		/*h write epanalambanetai wste na graftoun osa bytes epithumoume, se periptwsh
		* pou de graftoun apo thn prwth write*/
        while(total_write != count){
            
            check_write = write(fd, buf + total_write, count-total_write);
            total_write = total_write + check_write;
            if(check_write < 0){
                perror("write");
                printf("file: %s and line: %d\n", file, line);
                exit(1);
            }
        }
    }
    return(count);
}
