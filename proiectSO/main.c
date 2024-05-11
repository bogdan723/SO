#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/wait.h>
#define SIZE 1024

int nrcorupte=0;
char dif[10][100]={""};
int n=0;
struct stat stat_buffer;

int comparare_snapshot(int fd1, int fd2)
{
    fstat(fd1 , &stat_buffer);
    int size1 = stat_buffer.st_size;
    fstat(fd2 , &stat_buffer);
    int size2 = stat_buffer.st_size;

    if (size1 != size2)
    {
        return 1;
    }
    else
    {
        char buf1[SIZE] = "\0";
        char buf2[SIZE] = "\0";
        int c1;
        int c2;
        lseek(fd1 , 0 , SEEK_SET);
        lseek(fd2 , 0 , SEEK_SET);
        while ((c1 = read(fd1 , buf1 , SIZE)) > 0 && (c2 = read(fd2 , buf2 , SIZE)) > 0)
        {
            if(strcmp(buf1, buf2) != 0)
                return 1;
        }
    }

    return 0;
}

void salvare_snapshot(int fd, char *dirPath)
{
    char aux[SIZE] = "";

    char *filename = dirPath;

    while (*filename) filename++;

    while (filename > dirPath && *filename != '/') filename--;

    if (*filename == '/') filename++;

    int length_buf = snprintf(aux, SIZE, "Ultima modificare: %s\nIntrare: %s\nDimensiune: %ld octeți\n Numar Inode: %ld\n", ctime(&stat_buffer.st_mtime), filename, stat_buffer.st_size, stat_buffer.st_ino);
    
    write(fd , aux , length_buf);
}

void clonare_snapshot(int fd1, int fd2)
{
    lseek(fd1, 0, SEEK_SET);
    lseek(fd2, 0, SEEK_SET);

    char buf2[SIZE];
    int c1 = 0;

    while ((c1 = read(fd2 , buf2 , SIZE)) > 0)
    {
        write(fd1, buf2, c1);
    }
}

void creare_snapshot(int fd, char *dirPath , char izolare[])
{
    int pid, wstatus;
    pid_t w;
    DIR *director = opendir(dirPath);

    if (director == NULL)
    {
        perror("Eroare folder");
        return;
    }
    struct dirent *curr;

    while ((curr = readdir(director)) != NULL)
    {
        if (strcmp(curr->d_name, ".") != 0 && strcmp(curr->d_name, "..") != 0)
        {
            char array_cale[SIZE];

            snprintf(array_cale, sizeof(array_cale), "%s/%s", dirPath, curr->d_name);
            if (lstat(array_cale, &stat_buffer) == 0)
            {
                if (S_ISREG(stat_buffer.st_mode))
                {
                    if ((stat_buffer.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH)) == 0)
                    {
                        int pipefd[2];

                        if(pipe(pipefd)<0)
                        {
                            perror("[ERROR] Pipe");
                            exit(0);
                        }

                        if ((pid=fork())<0)
                        {
                            perror("[ERROR] Proces");
                            exit(0);
                        }

                        if (pid==0)
                        {
                            close(pipefd[0]);
                            dup2(pipefd[1] , 1);
                            execlp("./script.sh", "script.sh", array_cale , NULL);
                        }
                        else{
                            close(pipefd[1]);
                            char message[100]="";
                            int count=read(pipefd[0] , message , sizeof(message));
                            close(pipefd[0]);
                            if(count<0)
                            {
                                perror("[ERROR] Pipe");
                                exit(0);
                            }

                            message[strcspn(message , "\n")]='\0';

                            if(strcmp(message , "SAFE")!=0)
                            {
                                nrcorupte++;
                                char nume_fis[150]="";
                                char cale_noua[300]="";
                                char *p=strrchr(array_cale , '/');
                                if(p)                                    
                                {
                                    strcpy(nume_fis , p+1);
                                }

                                snprintf(cale_noua , sizeof(cale_noua) , "%s/%s" , izolare , nume_fis);
                                rename(array_cale , cale_noua);
                            }
                            w=wstatus = wait(&wstatus);
                            if(w==-1)
                            {
                                perror("[ERROR] Wait");
                                exit(0);
                            }
                        }
                    }
                    else salvare_snapshot(fd, array_cale);
                }
                else if(S_ISDIR(stat_buffer.st_mode) && S_ISLNK(stat_buffer.st_mode)==0)
                {
                    creare_snapshot(fd, array_cale , izolare);
                }
            }
        }
    }
    closedir(director);
}

// ./a.out -o output -s isolated dir1 dir2 dir3 ... dir10
int main(int argc, char **argv)
{
    if (argc < 6 || argc > 15 || strcmp(argv[1], "-o") != 0 || strcmp(argv[3], "-s") != 0) {
        printf("Opțiuni invalide. Utilizare: %s -o director_ieșire -s director_spațiu_izolare dir1 [dir2 dir3 ...]\n", argv[0]);
        exit(0);
    }

    // if (argc > 15)
    // {
    //     perror("Eroare");
    //     exit(0);
    // }

    for(int i=5 ; i<argc ; i++)
    {
        if (lstat(argv[i], &stat_buffer)!=0 || S_ISDIR(stat_buffer.st_mode) == 0)
        {
            perror("Nu exista folderul");
            continue;
        }

        if (S_ISDIR(stat_buffer.st_mode) && S_ISLNK(stat_buffer.st_mode)==0)
        {
            int ok=0;
            for(int j=0 ; j<n ; j++)
            {
                if(strcmp(dif[j] , argv[i])==0)
                {
                    ok=1;
                    break;
                }
            }
            if(ok==0)
            {
                strcpy(dif[n++] , argv[i]);
            }
        }
    }

    char dir_out[SIZE];
    strcpy(dir_out, argv[2]);   
    
    pid_t w;
    int wstatus , pid;

    for (int i = 0; i < n; i++)
    {
        if (lstat(dif[i], &stat_buffer) != 0)
        {
            perror("Eroare folder");
            continue;
        }
        if ((pid = fork()) < 0)
        {
            perror("eroare creare proces");
            exit(0);
        }
        if (pid == 0)
        {
            nrcorupte=0;

            char d1[SIZE] = "";
            strcat(d1, dir_out);
            strcat(d1, "/snapshot_");
            strcat(d1, dif[i]);
            strcat(d1, ".txt");

            int fd1 = open(d1, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
            if (fd1 == -1)
            {
                perror("Eroare fisier");
                exit(-1);
            }
            if (lstat(d1, &stat_buffer) == 0)
            {
                if (stat_buffer.st_size == 0)
                {
                    creare_snapshot(fd1, dif[i] , argv[4]);
                }

                char d2[SIZE] = "";
                strcat(d2, dir_out);
                strcat(d2, "/_snapshot_");
                strcat(d2, dif[i]);
                strcat(d2, ".txt");

                int fd2 = open(d2, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
                if (fd2 == -1)
                {
                    perror("Eroare fisier :(");
                    exit(-1);
                }

                creare_snapshot(fd2, dif[i] , argv[4]);

                if (comparare_snapshot(fd1, fd2) == 1)
                {
                    clonare_snapshot(fd1, fd2);
                    unlink(d2);
                    close(fd1);
                    close(fd2);
                }
                else
                {
                    close(fd1);
                    close(fd2);
                    unlink(d2);
                }
            }

            exit(nrcorupte);
        }
    }

    for(int i=0 ; i<n ; i++)   
    {
        w = wait(&wstatus);
        if(w==-1)
        {
            perror("[ERROR] Wait");
            exit(1);
        }
        if(WIFEXITED(wstatus)) 
        {
            printf("Procesul Copil %d s-a incheiat cu PID=%d si cu %d fisiere cu potential periculos.\n", i , w , WEXITSTATUS(wstatus));
        }
    }
    return 0;
}