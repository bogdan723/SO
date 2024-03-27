#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

void printNume(char nume[50][50],int n)
{
  for(int i=0;i<n;i++)
    {
      printf("%s\n",nume[i]);
    }
}

int main(int argc, char* argv[])
{
  DIR* dir=NULL;
  if((dir = opendir(argv[1]))==NULL)
    {
      perror("deschidere director");
      exit(-1);
    }
  char nume[50][50];
  int k=0;
  struct dirent* date;
  while((date = readdir(dir))!=NULL)
    {
      strcpy(nume[k],date->d_name);
      k++;
    }
  printNume(nume,k);
  closedir(dir);
}
