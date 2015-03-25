#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<map>
#include<string>
using namespace std;

map<string, string> MountData;
char OldMount[1024];

void AddNotify(char buf[], char buf2[])
{
    printf("Mount: %s %s\n", buf, buf2);
}

void MakeList(char buf[], int flag)
{
    int coun = 0;
    int pos = 0;
    const int len = strlen(buf);
    char MountPos[1024];
    char DiskName[1024];
    int Status = 0;
    while(pos < len)
    {
        while(buf[pos] == 32)
            pos++;
        if(coun == 0)
        {
            int i = 0;
            while(buf[pos] == 10)
                pos++;
            memset(MountPos, 0,sizeof(MountPos));
            while(buf[pos] != 32 && pos < 1024)
            {
                MountPos[i] = buf[pos];
                pos++;
                i++;
            }
        }
        if(coun == 1)
        {
            int i = 0;
            memset(DiskName, 0, sizeof(DiskName));
            while(buf[pos] != 32 && pos < 1024)
            {
                DiskName[i] = buf[pos];
                pos++;
                i++;
            }
            if(flag == 1)
                MountData[DiskName] = MountPos;
            if(flag == 2)
            {
                if(MountData[DiskName] != MountPos && strcmp(OldMount, DiskName) != 0)
                {
                    AddNotify(MountPos, DiskName);
                    memset(OldMount, 0, sizeof(OldMount));
                    strcpy(OldMount, DiskName);
                    Status = 1;
                }
                else if(MountData[DiskName] != MountPos)
                    Status = 1;
            }
        }
        if(coun == 2)
            while(buf[pos] != 32 && pos < 1024)
                pos++;
        if(coun == 3)
            while(buf[pos] != 10 && pos < 1024)
                pos++;
        coun++;
        coun %= 4;
    }
    if(Status == 0)
    {
        if(OldMount[0] != 0)
        {
            printf("Unmount: %s\n", OldMount);
            OldMount[0] = 0;
        }
    }
    return;
}
int main()
{
    FILE *stream;
    char buf[1024];
    MountData.clear();
    memset(buf, 0, sizeof(buf));
    stream = popen("findmnt -l | grep /dev/sd", "r");
    fread(buf, sizeof(char), sizeof(buf), stream);
    memset(OldMount, 0, sizeof(OldMount));
    MakeList(buf, 1);
    pclose(stream);
    while(1)
    {
        FILE *stream2;
        memset(buf, 0, sizeof(buf));
        stream2 = popen("findmnt -l | grep /dev/sd", "r");
        fread(buf, sizeof(char), sizeof(buf), stream2);
        MakeList(buf, 2);
        pclose(stream2);
    }
    return 0;
}
