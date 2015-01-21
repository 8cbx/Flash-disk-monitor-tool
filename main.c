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
    //printf("%s\n",cwd);
}

void notify(char *pw,int fd)
{
    int wd;
    wd=inotify_add_watch(fd,pw,IN_ALL_EVENTS);
    if(wd==-1)
    {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
}

void changewd(char *twd,int fd)
{
    struct dirent *entry;
    DIR *dir;
    //printf("%s\n",twd);
    //printf("%s\n",cwd);
    errno=0;
    dir=opendir(twd);
    {
        if(errno!=0)
        {
            perror("opendir");
            exit(EXIT_FAILURE);
        }
    }
    notify(twd,fd);
    errno=0;
    while((entry=readdir(dir))!=NULL)
    {
        if(strcmp(".",entry->d_name)==0||strcmp("..",entry->d_name)==0)
            continue;
        else if(entry->d_type==4)
        {
            //struct dirent tmp = *entry;
            //printf("%s %s %d\n",twd,entry->d_name,(int)entry->d_type);
            char nextwd[1024];
            strcpy(nextwd,twd);
            strcat(nextwd,"/\0");
            strcat(nextwd,entry->d_name);
            changewd(nextwd,fd);
            //closedir(dir);
        }
        //if(entry->d_type==8)printf("%s/%s\n",twd,entry->d_name);
    }
    closedir(dir);
}

void readnoti(int fd)
{
    char buf[1024]__attribute__((aligned(4)));
    ssize_t len,i=0,j;
    len=read(fd,buf,1024);
    while(i<len)
    {
        struct inotify_event *event = (struct inotify_event *) &buf[i];
        if(event->len)
        {
            printf("%s %d\n",event->name,event->mask);
        }
        i+=sizeof(struct inotify_event)+event->len;
    }
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
    changewd(path,fd);
    while(1)
        readnoti(fd);
    return 0;
}
