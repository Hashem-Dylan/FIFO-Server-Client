/*
* Dylan Hashem
* C Implementation of a FIFO-Server for multiple clients
*/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

//Define struct for System Requests
struct SystemRequest {
    char priv_fifo[50], client_num[14], sys_call[14], param_list[100];
    int param_num;
}systemRequest;
int sys_call_1(struct SystemRequest systemRequest);
int sys_call_2(struct SystemRequest systemRequest);
int sys_call_3(struct SystemRequest systemRequest);
int sys_call_4(struct SystemRequest systemRequest);
int sys_call_5(struct SystemRequest systemRequest);
void promptUser(struct SystemRequest systemRequest);
int handleSysCall(struct SystemRequest systemRequest, int sysCallNum);
void initialUserPrompt(void);
void copy_string(char*, char*);
int wait_for_signal(struct SystemRequest systemRequest);
int main()
{

    initialUserPrompt();
    return 0;
}

//handle system call 1 to establish p_id from server
void initialUserPrompt(){
    int sysCallNum;
    char servMesg[80];
    char p_id[14];
    struct SystemRequest systemRequest_1;
    printf("\nEnter System Call 1 to connect to server: ");
    scanf("%s", systemRequest_1.sys_call);
    sscanf(systemRequest_1.sys_call, "%d", &sysCallNum);
    printf("\nsysCallNum: %d", sysCallNum);
    strcpy(p_id, "0");
    systemRequest_1.param_num = 0;
    strcpy(systemRequest_1.client_num, p_id);
    sprintf(systemRequest_1.priv_fifo, "FIFO_%s", systemRequest_1.client_num);
    // Open FIFO for write only
    int fd = open("Common_FIFO", O_WRONLY);
    printf("\ncommon fifo opened \n");
    //write request to server
    write(fd, &systemRequest_1, sizeof(systemRequest_1));
    printf("request sent\n");
    close(fd);
    
    //make private fifo
    mkfifo(systemRequest_1.priv_fifo, 0666); //temporary fifo_0
    fd = open(systemRequest_1.priv_fifo, O_RDONLY);
    read(fd, servMesg, 80);
    printf("%s", servMesg);
    strcpy(p_id, &servMesg[strlen(servMesg)-2]);
    printf("\nNew p_id:%s", p_id);
    strcpy(systemRequest_1.client_num, p_id);
    sprintf(systemRequest_1.priv_fifo, "FIFO_%s", systemRequest_1.client_num);
    printf("\nSystem Request details: sys_call:%s client_num:%s param_num:%d priv_fifo:%s", systemRequest_1.sys_call, systemRequest_1.client_num, systemRequest_1.param_num, systemRequest_1.priv_fifo);
    mkfifo(systemRequest_1.priv_fifo, 0666);
    close(fd);
    promptUser(systemRequest_1);
    
}
//loop to initiate all system calls after 1
void promptUser(struct SystemRequest systemRequest_1){
    int endLoop = 1;
    while(endLoop != 0){
    int sysCallNum;
    struct SystemRequest systemRequest;
    strcpy(systemRequest.priv_fifo, systemRequest_1.priv_fifo);
    strcpy(systemRequest.client_num, systemRequest_1.client_num);
    strcpy(&systemRequest.param_list[0], " ");
    systemRequest.param_num = 1;
    printf("\nEnter System Call [2 - end, 3 - create message queue, 4 - write message queue, 5 - read from message queue]: ");
    scanf("%s", systemRequest.sys_call);
    sscanf(systemRequest.sys_call, "%d", &sysCallNum);
    printf("\nsysCallNum: %d", sysCallNum);
    endLoop = handleSysCall(systemRequest, sysCallNum);
    }
}
//handle system calls from 2-5
int handleSysCall(struct SystemRequest systemRequest, int sysCallNum){
    switch(sysCallNum){
        case 2:
            if(sys_call_2(systemRequest)!=1){
                return 0;
            }else{
                return 1;
            }
            break;
        case 3:
            if(sys_call_3(systemRequest)!=1){
                return 0;
            }else{
                return 1;
            }
            break;
        case 4:
            if(sys_call_4(systemRequest)!=1){
                return 0;
            }else{
                return 1;
            }
            break;
        case 5:
            if(sys_call_5(systemRequest)!=1){
                return 0;
            }else{
                return 1;
            }
            break;
            
        default :
           printf("\nInvalid System Call");
            return 1;
            
     }
}

int sys_call_1(struct SystemRequest systemRequest){
    char p_id[14];
    strcpy(p_id, "0");
    char servMesg[80];
    systemRequest.param_num = 0;
    strcpy(systemRequest.client_num, p_id);
    sprintf(systemRequest.priv_fifo, "FIFO_%s", systemRequest.client_num);
    // Open FIFO for write only
    int fd = open("Common_FIFO", O_WRONLY);
    printf("\ncommon fifo opened \n");
    //write request to server
    write(fd, &systemRequest, sizeof(systemRequest));
    printf("request sent\n");
    close(fd);
    
    //make private fifo
    mkfifo(systemRequest.priv_fifo, 0666); //temporary fifo_0
    fd = open(systemRequest.priv_fifo, O_RDONLY);
    read(fd, servMesg, 80);
    printf("%s", servMesg);
    strcpy(p_id, &servMesg[strlen(servMesg)-2]);
    printf("\nNew p_id:%s", p_id);
    strcpy(systemRequest.client_num, p_id);
    sprintf(systemRequest.priv_fifo, "FIFO_%s", systemRequest.client_num);
    printf("\nSystem Request details: sys_call:%s client_num:%s param_num:%d priv_fifo:%s", systemRequest.sys_call, systemRequest.client_num, systemRequest.param_num, systemRequest.priv_fifo);
    mkfifo(systemRequest.priv_fifo, 0666);
    close(fd);
    return 1;
    
}
int sys_call_2(struct SystemRequest systemRequest){
    //Specify ending parameters
    char servMesg[80];
    //define new request message
    struct SystemRequest endRequest;
    strcpy(endRequest.priv_fifo, systemRequest.priv_fifo);
    strcpy(endRequest.client_num, systemRequest.client_num);
    strcpy(endRequest.sys_call, "2");
    strcpy(&endRequest.param_list[0], "Ending connection");
    endRequest.param_num = 1;
    
    
    
    
    //open common fifo in write mode
    int fd = open("Common_FIFO", O_WRONLY);
    write(fd, &endRequest, sizeof(endRequest));
    close(fd);
    //read from FIFO for response
    fd = open(systemRequest.priv_fifo, O_RDONLY);
    read(fd, servMesg, 80);
    printf("\n%s",servMesg);
    unlink(systemRequest.priv_fifo);
    close(fd);
    //i++; //i used for limiting number of calls
    return 0;
    
    
}
int sys_call_3(struct SystemRequest systemRequest){
    char servMesg[80];
    //define new request message
    struct SystemRequest createRequest;
    strcpy(createRequest.priv_fifo, systemRequest.priv_fifo);
    strcpy(createRequest.client_num, systemRequest.client_num);
    strcpy(createRequest.sys_call, "3");
    strcpy(&createRequest.param_list[0], "Create queue");
    createRequest.param_num = 1;
    
    //open common fifo in write mode
    int fd = open("Common_FIFO", O_WRONLY);
    write(fd, &createRequest, sizeof(createRequest));
    close(fd);
    //read from FIFO for response
    fd = open(systemRequest.priv_fifo, O_RDONLY);
    read(fd, servMesg, sizeof(servMesg));
    printf("\n%s",servMesg);
    //unlink(systemRequest.priv_fifo);
    close(fd);
    return 1;
}
int sys_call_4(struct SystemRequest systemRequest){
    char servMesg[80];
    char mesgText [50];
    char recipient [14];
    printf("\nWhich queue would you like to send a message to: ");
    scanf("%s", recipient);
   printf("\nType a message to server: ");
   scanf("%s", mesgText);
    
   
   //define new request message
   struct SystemRequest writeRequest;
   strcpy(writeRequest.priv_fifo, systemRequest.priv_fifo);
   strcpy(writeRequest.client_num, systemRequest.client_num);
   strcpy(writeRequest.sys_call, "4");
   strcpy(&writeRequest.param_list[0], recipient);
   strcpy(&writeRequest.param_list[15], mesgText);
   writeRequest.param_num = 2;
   
   //print System request details
   printf("\nSystem Request details: sys_call:%s client_num:%s param_num:%d priv_fifo:%s", writeRequest.sys_call, writeRequest.client_num, writeRequest.param_num, writeRequest.priv_fifo);
   
   //open common fifo in write mode
    int fd = open("Common_FIFO", O_WRONLY);
    write(fd, &writeRequest, sizeof(writeRequest));
    close(fd);
    //read from FIFO for response
    fd = open(systemRequest.priv_fifo, O_RDONLY);
    read(fd, servMesg, 80);
    printf("%s",servMesg);
    //unlink(systemRequest.priv_fifo);
    close(fd);
    printf("\n");
    wait_for_signal(writeRequest);
    printf("\nMessage read");
    return 1;
}
int sys_call_5(struct SystemRequest systemRequest){
    //read from server queue
    char servMesg[80];
    //define new request message
    struct SystemRequest readRequest;
    strcpy(readRequest.priv_fifo, systemRequest.priv_fifo);
    strcpy(readRequest.client_num, systemRequest.client_num);
    strcpy(readRequest.sys_call, "5");
    readRequest.param_num = 1;
    
    //print system request details
    printf("\nSystem Request details: sys_call:%s client_num:%s param_num:%d priv_fifo:%s", readRequest.sys_call, readRequest.client_num, readRequest.param_num, readRequest.priv_fifo);
    
    //open common fifo in write mode
    int fd = open("Common_FIFO", O_WRONLY);
    write(fd, &readRequest, sizeof(readRequest));
    close(fd);
    //read from FIFO for response
    fd = open(systemRequest.priv_fifo, O_RDONLY);
    read(fd, &servMesg, sizeof(servMesg));
    printf("\n%s",servMesg);
    //unlink(systemRequest.priv_fifo);
    close(fd);
    return 1;
    
}

void copy_string(char *target, char *source)
{
   while(*source)
   {
      *target = *source;
      source++;
      target++;
   }
   *target = '\0';
}
int wait_for_signal(struct SystemRequest systemRequest){
    int i_signal = 0;
    char servMesg[80];
    while (i_signal<1){
        int fd = open(systemRequest.priv_fifo, O_RDONLY);
        read(fd, servMesg, 80);
        i_signal = atoi(&servMesg[strlen(servMesg)-1]);
        close(fd);
    }
    return 0;
}
