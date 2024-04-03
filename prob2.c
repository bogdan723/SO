#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 512

typedef struct {
  char nume[SIZE];
  time_t ultima_modificare;
} InfoFisier;

void ObtineInfoFisier(const char *cale, InfoFisier *info)
{
  struct stat statbuf;
  if (stat(cale, &statbuf) == -1)
    {
      perror("stat");
      return;
    }
  strcpy(info->nume, cale);
  info->ultima_modificare = statbuf.st_mtime;
}

void CreeazaInstantaneuDirector(const char *cale_director, FILE *fisier_instantaneu) {
  DIR *director = opendir(cale_director);
  if (director == NULL)
    {
      perror("opendir");
      return;
    }
  struct dirent *intrare;
  char cale_completa[SIZE];
  InfoFisier info;

  while ((intrare = readdir(director)) != NULL)
    {
      if (strcmp(intrare->d_name, ".") == 0 || strcmp(intrare->d_name, "..") == 0)
	{
	  continue;
	}
      printf(cale_completa, sizeof(cale_completa), "%s/%s", cale_director, intrare->d_name);
      ObtineInfoFisier(cale_completa, &info);
      fprintf(fisier_instantaneu, "%s, %ld\n", info.nume, info.ultima_modificare);
    } 
  closedir(director);
}

int main(int argc, char *argv[])
{
  if(argc != 2)
    {
      fprintf(stderr,"Utilizare: %s \n",argv[0]);
      exit(EXIT_FAILURE);
    }

  FILE *snapshot = fopen("Bogdi_out.txt", "w");
  if (snapshot == NULL)
    {
      perror("fopen");
      exit(EXIT_FAILURE);
    }
  
  CreeazaInstantaneuDirector(argv[1], snapshot);
  
  fclose(snapshot);
  return 0;
}
