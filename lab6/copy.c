#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void copy_mmap(int fd_from, int fd_to);
void copy_read_write(int fd_from, int fd_to);
int get_file_size(int fd);

int main(int argc, char **argv){
    if(argc == 1){
        fprintf(stderr, "copy: Missing arguments list\nTry 'copy -h' for help\n");
        return 1;
    }
    
    int c;
    int mflag = 0;
    
    opterr = 0;

    while((c = getopt(argc, argv, "hm")) != -1){        //if set "hm:" then -m requirs 1 argument and i have no idea how to force 2.
        switch (c){
            case 'h':
                printf("Usage:\n");
                printf(" copy <file_name> <new_file_name>\n");
                printf(" copy [-m] <file_name> <new_file_name>\n");
                return 0;
            case 'm':
                if(argc != 4){
                    fprintf(stderr, "copy: Option -m requires 2 arguments.\nTry 'copy -h' for help\n");
                    return 1;
                }
                mflag = 1;
                break;
            case '?':
                if(isprint (optopt))
                    fprintf(stderr, "copy: Unknown option '-%c'\nTry 'copy -h' for help\n", optopt);
                else
                    fprintf(stderr, "copy: Unknown option character `\\x%x`.\n", optopt);
                return 1;
            default:
                fprintf(stderr, "copy: abort()");
                abort();
        }
    }   //while
    //Possible cases: 
        //argc == 4 && mflag == 1
        //argc == 3 && mflag == 0
    if(argc != 3 && mflag == 0){
        fprintf(stderr, "copy: Wrong arguments list\nTry 'copy -h' for help\n");
        return 1;
    }
    
    int fd_from, fd_to;     //declaring file descriptors

    if((fd_from = open(argv[mflag+1], O_RDONLY)) == -1 || (fd_to = open(argv[mflag+2], O_RDWR | O_CREAT, 0666)) == -1){   //O_RDWD
        perror("open");
        return 1;
    }

    if(mflag == 0) //without -m option
        copy_read_write(fd_from, fd_to);
    else
        copy_mmap(fd_from, fd_to);

    close(fd_from);
    close(fd_to);
    return 0;
}


void copy_read_write(int fd_from, int fd_to){
        
    int file_size = get_file_size(fd_from);
    //printf("File size: %d\n", file_size);
    char buf[file_size];
    int ret_in, ret_out;
    while((ret_in = read(fd_from, buf, file_size)) > 0){
        ret_out = write(fd_to, buf, (ssize_t) ret_in);
    }
    //printf("exit copy_read_write\n");
}

void copy_mmap(fd_from, fd_to){
    struct stat sb;
    char *addr_from;
    char *addr_to;
    if(fstat(fd_from, &sb) == -1){
        perror("fstat");
        exit(1);
    }
    addr_from = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd_from, 0);
    if(addr_from == MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    //without truncate gives Bus error
    if(ftruncate(fd_to, sb.st_size) == -1){
        perror("ftruncate");
        exit(1);
    }

    addr_to = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_to, 0);
        
    if(addr_to == MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    memcpy(addr_to, addr_from, sb.st_size);
    if(msync(addr_to, sb.st_size, MS_SYNC) == -1){
        perror("msync");
        exit(1);
    }
    //printf("exit copy_mmap\n");
}

int get_file_size(int fd){
      off_t current = lseek(fd, (off_t)0, SEEK_CUR);
      off_t file_size = lseek(fd, (off_t)0, SEEK_END);
      lseek(fd, current, SEEK_SET); 
      return (int)file_size;     
}
