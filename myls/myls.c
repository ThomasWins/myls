#include <fcntl.h>   //O_RDONLY, open
#include <unistd.h>  //read, close
#include <errno.h>   //errno
#include <stdio.h>   //perror, fprintf
#include <stdlib.h>  //exit
#include <dirent.h>  //opendir, readdir, DIR, struct dirent
#include <inttypes.h>// PRIdMAX
#include <sys/stat.h>//stat
#include <ctype.h>   // isprint
#include <limits.h>  // For PATH_MAX

#define BUFSIZE 16

// INPUT:
// gcc -0 myls myls.c; ./myls [DIR]

int main(int argc, char **argv) {

    // Get the path of the current directory
    char *path;
    if (argc < 1 || argc > 2) {
        fprintf(stderr, "USAGE: %s [DIR]\n", argv[0]);
        exit(1);
    } else if (argc == 2){
        path = argv[1];
    } else
        path = "./"; // If no argument, path = current directory


    // Init directory struct
    DIR *dirp = opendir(path);

    if (NULL == dirp){
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // Init stat struct
    struct stat statbuf;

//////////////////////////////////////////////////////////////////////////
/////////// LOOP THROUGH ALL THE FILES IN THE ACTIVE DIRECTORY ///////////
    struct dirent *curdir;
    while ((curdir = readdir(dirp)) != NULL)
    {
        //// FILE NAME ////
        printf("%16.16s\t", curdir->d_name);

        // Construct the full path to the file
        char filepath[PATH_MAX];
        snprintf(filepath, PATH_MAX, "%s/%s", path, curdir->d_name); // Use the provided path

        //// OPEN THE FILE ////
        int fd = open(filepath, O_RDONLY);
        if (fd == -1)
        {
            perror("open");
            continue; // Move to the next file if open fails
        }

        //// SET STAT FOR THE FILE ////
        if (stat(filepath, &statbuf) == -1)
        {
            perror("stat");
            close(fd);
            continue; // Skip this file if stat fails
        }

        //// NUMBER OF HARD LINKS ////
        printf("%" PRIdMAX "\t", (intmax_t)statbuf.st_nlink);

        char buf[BUFSIZE];
        // Reset buf to non-garbage values
        for (int i = 0; i < BUFSIZE; i++){
            buf[i] = '\0';
        }

        //// FILE TYPE ////
        switch(statbuf.st_mode & S_IFMT){
            case S_IFREG:
                printf("REG\t");
                break;
            case S_IFDIR:
                printf("DIR\t");
                break;
            case S_IFSOCK:
                printf("SOCK\t");
                break;
            case S_IFLNK:
                printf("LNK\t");
                break;
            case S_IFBLK:
                printf("BLK\t");
                break;
            case S_IFCHR:
                printf("CHR\t");
                break;
            case S_IFIFO:
                printf("FIFO\t");
                break;
            default: // Shouldnt happen
                printf("UNKNOWN");
                break;
        }

        //// FILE SIZE ////
        printf("%" PRIdMAX "\t", (intmax_t)statbuf.st_size);

        //// PREVIEW ////
        // If the file is REG, print the first 16 bytes
        if ((statbuf.st_mode & S_IFMT) == S_IFREG){
            // If REG, print preview
            

            // Set buffer to 16 bytes
            int bytes = read(fd, buf, BUFSIZE);
            if (bytes == -1){
                perror("read");
                close(fd);
                continue; 
            }

            // Replace non-printable characters with spaces
            for (int i = 0; i < bytes; i++)
            {
                if (!isprint(buf[i])) {
                    buf[i] = ' ';
                }  
            }

            close(fd);
            printf("%.*s", bytes, buf);
        }

        printf("\n");
    }
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
    closedir(dirp);
    return 0;
}