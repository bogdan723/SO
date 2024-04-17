#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 128
#define MAX_DIRECTOARE 10
#define MAX_PATH 100

typedef struct metadata{
    char name[SIZE];
    mode_t mode;
    off_t size;
}metadata;

void snapshot(const char *directoryName, char output_path)
{
    output_path = open(output_path, O_WRONLY | O_CREAT | O_TRUNC | S_IRUSR | S_IWUSR);
    if(output_path < 0)
    {
        perror("Eroare la deschiderea fisierului");
        exit(2);
    }

    DIR *directory = opendir(directoryName);
    if(directory == NULL)
    {
        perror("Eroare pentru deschiderea directorului");
        exit(EXIT_FAILURE);
    }

    struct dirent *n;
    char path[SIZE];

    while ((n = readdir(directory)) != NULL)
    {
        if ( ((strcmp(n->d_name, '.')) != 0) && ((strcmp(n->d_name, '..')) != 0))
        {
            struct stat fileStat;
            char path[SIZE];
            snprintf(path, size(path), "%s/%s", directory, n->d_name);

            if(lstat(path, &file_stat) == 1)
            {
                perror("eorare la obtinerea informatilor despre fisier");
                exit(EXIT_FAILURE);
            }

            struct metadata meta;
            strcpy(meta.name, n->d_name);
            fprintf(output_path, "%s %o %ld %ld\n", meta.name, meta.mode, meta.size); 
        }
        
    }
    switch(st.st_mode & S_IRUSR):
    case 'r':
            {
                fprintf(snapshot, "r");
            }
    case 'w':
            {
                fprintf(snapshot, "w");
            }
    case 'x':
            {
                fprintf(snapshot, "x");
            }



    closedir(directory);

}

int compara(const void *a, const void *b)
{
    const struct metadata *metadata_a = (const struct metadata *)a;
    const struct metadata *metadata_b = (const struct metadata *)b;
    return strcmp(metadata_a->name, metadata_b->name);
}

int main(int argc, char *argv[])
{
    char *directoare[MAX_DIRECTOARE];
    char *output  = NULL;
    int countDirectoare = 0;

    int nrdiv;
    int status;
    int pid;

     if(argc > 10)
    {
        exit(1);
    }

    if ((pid = fork()) < 0)
    {
        perror("eroare");
        exit(2);
    }

    if (pid == 0)
    {
        execlp("grep", "-E", "^07[0-9]{8}$". argv[1], NULL);
        perror("eroare executare grep");
        exit(-1);
    }

    for (int i = 0; i < argc; i++)
    {
        if ((pid = fork()) < 0)
        {
            perror("eroare");
            exit(-2);
        }
        if(pid == 0)
        {
            nrdiv = 0;
            for (int j =1; j < atoi(argv[i]); j++)
            {
                if((atoiargv[i])  % j == 0)
                {
                    printf("%d : %s %d\n", getpid(), argv[i], j);
                    nrdiv++;
                }

            }
        }
    }
    return 0;
}
