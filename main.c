#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<errno.h>
#include<sys/inotify.h>

void currentwd()
{
    char cwd[1024];
    if(!getcwd(cwd,1024))
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
    printf("%s\n",cwd);
}
void changewd(char *twd)
{
    struct dirent *entry;
    DIR *dir;
    //printf("%s\n",twd);
    //printf("%s\n",cwd);
    errno=0;
    dir=opendir(twd);
    {
        if(errno != 0)
        {
            perror("opendir");
            exit(EXIT_FAILURE);
        }
    }
    errno=0;
    while((entry=readdir(dir))!=NULL)
    {
        if(strcmp(".",entry->d_name) == 0 ||strcmp("..",entry->d_name) == 0)
            continue;
        else if(entry->d_type==4)
        {
            //struct dirent tmp = *entry;
            //printf("%s %s %d\n",twd,entry->d_name,(int)entry->d_type);
            char nextwd[1024];
            strcpy(nextwd,twd);
            strcat(nextwd,"/\0");
            strcat(nextwd,entry->d_name);
            changewd(nextwd);
            //closedir(dir);
        }
        if(entry->d_type==8)printf("%s/%s\n",twd,entry->d_name);
    }
    closedir(dir);
}
int main()
{
    char path[1024]= {"/home/chu/local"};
    int fd;
    fd=inotify_init1(0);
    if(fd==-1)
    {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }
    currentwd();
    changewd(path);
    return 0;
}
