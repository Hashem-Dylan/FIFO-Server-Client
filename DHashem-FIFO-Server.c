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
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

//Define struct for System Requests
struct SystemRequest {
    char priv_fifo[50], client_num[14], sys_call[14], param_list[100];
    int param_num;
};//systemRequest;
struct node {
   int data;
   int key;
   struct node *next;
};

struct message {
    char sender[14]; //assigned from requests[i].client_num
    char recipient[14]; //assigned from &requests[i].param_list[0]
    char mesgText[50]; //assigned from &requests[i].param_list[15]
};
//will point to a segment in queueList
struct mesgQueue {
    char queueOwner[14]; //assigned from requests[i].client_num
    struct message *m; //created in system call #3
    
};
struct node *head = NULL;
struct node *current = NULL;
bool start = true;
int i = 0;
void sys_call_1(struct SystemRequest requests[i], int available_client_num);
void sys_call_2(struct SystemRequest requests[i]);
void sys_call_3(struct SystemRequest requests[i], struct mesgQueue **queueList);
void sys_call_4(struct SystemRequest requests[i], struct mesgQueue **queueList);
void sys_call_5(struct SystemRequest requests[i], struct mesgQueue **queueList);
int readContainer(struct SystemRequest requests[i]);
struct mesgQueue *createMesgQueue(struct SystemRequest requests[i]);
void signalClient(struct message retrievedMesg);
bool isEmpty(void);
void printList(void);
void insertFirst(int key, int data);
struct node* delete(int key);
int length(void);
size_t container = 0;



int main()

{
    //declare variables and initilize struct
    int fd1;
    struct SystemRequest  systemRequest;
    struct SystemRequest requests[40];
    int available_client_num =1;
    struct mesgQueue **queueList = malloc(10 * sizeof(struct mesgQueue *)); //initialize queueList pointers
    for (int j = 0; j < 10; j++) {
            queueList[j] = malloc(sizeof(struct mesgQueue)); //create 10 mesgQueues to choose from
        }
    //Make common FIFO
    mkfifo("Common_FIFO", 0666); //well known fifo
    int i = 0;
    
    while (i < 50) //limit on how many commands will be received
    {
        fd1 = open("Common_FIFO",O_RDONLY); //open common fifo
        //check for data from container
        container = read(fd1, &requests[i], sizeof(systemRequest));
        if(container > 0){
            close(fd1);
            int sysCallNum;
            sysCallNum = readContainer(&requests[i]);
            switch(sysCallNum) {
                case 1 :
                    sys_call_1(&requests[i], available_client_num);
                    available_client_num++;
                    i++;
                    break;
                case 2 :
                    sys_call_2(&requests[i]);
                    if (isEmpty() == 1){
                        //end server if client list is empty
                        printf("\n Client List Empty. Ending Server");
                        return 0;
                    }
                    i++;
                    break;
                case 3 :
                    sys_call_3(&requests[i], queueList);
                    i++;
                    break;
                case 4 :
                    sys_call_4(&requests[i], queueList);
                    i++;
                  break;
                case 5 :
                    sys_call_5(&requests[i], queueList);
                    i++;
                  break;
                default :
                    printf("Invalid system call\n" );
                    i++;
            }
        }
        
    }
    return 0;
}
int readContainer(struct SystemRequest requests[i]){
//print system request data
printf("\nSystem call from client:%s ", requests[i].client_num);
printf("Client%s: %s", requests[i].client_num, requests[i].param_list);
printf("\nSystem Request details: sys_call:%s client_num:%s param_num:%d ", requests[i].sys_call, requests[i].client_num, requests[i].param_num);
sprintf(requests[i].priv_fifo, "FIFO_%s", requests[i].client_num);
int sysCallNum = 0;
sscanf(requests[i].sys_call, "%d", &sysCallNum);
printf("\nsysCallNum: %s", requests[i].sys_call);
    return sysCallNum;
}
void sys_call_1(struct SystemRequest requests[i], int available_client_num){
    char servMesg[80];
    sprintf(servMesg, "Server - Established Private FIFO for client:%d\n", available_client_num);
    printf("\nSystem Request details: sys_call:%s client_num:%s param_num:%d ", requests[i].sys_call, requests[i].client_num, requests[i].param_num);
    insertFirst(available_client_num, available_client_num);
    printf("\nClient Number: %d ", available_client_num);
    //open to write to private fifo
    int fd1 = open(requests[i].priv_fifo,O_WRONLY);
    write(fd1, servMesg, sizeof(servMesg)); //assign data to priv_fifo
    close(fd1);
    printf("\n");
    available_client_num++;
    printf("\nCurrent Client List:");
    printList();
    printf("\n");
    
}
void sys_call_2(struct SystemRequest requests[i]){
    char servMesg[80];
    strcpy(servMesg, "Server - Ending connection");
    int fd1 = open(requests[i].priv_fifo,O_WRONLY);
    int removeClientNum = atoi(requests[i].client_num);
    //removing client number from client list
    delete(removeClientNum);
    printf("\nCurrent Client List:");
    printList();
    write(fd1, servMesg, sizeof(servMesg));
    unlink(requests[i].priv_fifo);
    close(fd1);
    //print system request details
    printf("\nSystem Request details: sys_call:%s client_num:%s param_num:%d ", requests[i].sys_call, requests[i].client_num, requests[i].param_num);
    
}
void sys_call_3(struct SystemRequest requests[i], struct mesgQueue **queueList){
    char servMesg[80];
    struct mesgQueue ***p = &queueList;
    int queueOwner = atoi(requests[i].client_num);
    strcpy(queueList[queueOwner]->queueOwner, requests[i].client_num);
    strcpy(servMesg, &requests[i].param_list[requests[i].param_num]);
    //create queue from request info
    struct mesgQueue *newQueue = createMesgQueue(&requests[i]);
    strcpy(newQueue->queueOwner, requests[i].client_num);
    memcpy(queueList[queueOwner], newQueue, 100); //copy new queue to queueList
    sprintf(servMesg, "Established Message Queue for client %s",(*p)[queueOwner]->queueOwner); //Assign serMesg
    int fd1 = open(requests[i].priv_fifo,O_WRONLY);
    write(fd1, servMesg, sizeof(servMesg));
    close(fd1);
    //print system request details
    printf("\nSystem Request details: sys_call:%s client_num:%s param_num:%d ", requests[i].sys_call, requests[i].client_num, requests[i].param_num);
    

    
}
void sys_call_4(struct SystemRequest requests[i], struct mesgQueue **queueList){
    char servMesg[80];
    struct message newMesg;
    strcpy(newMesg.recipient,&requests[i].param_list[0]);
    strcpy(newMesg.mesgText,&requests[i].param_list[15]);
    strcpy(newMesg.sender,requests[i].client_num);
    int queueOwner = atoi(newMesg.recipient);
    queueList[queueOwner]->m = &newMesg;
    
    sprintf(servMesg, "Message to Client:%s sent. Waiting for message to be read",newMesg.recipient);
    int fd1 = open(requests[i].priv_fifo,O_WRONLY);
    write(fd1, &servMesg, sizeof(servMesg));
    close(fd1);
    //print system request details
    printf("\nSystem Request details: sys_call:%s client_num:%s param_num:%d ", requests[i].sys_call, requests[i].client_num, requests[i].param_num);
    
}
void sys_call_5(struct SystemRequest requests[i], struct mesgQueue **queueList){
    char servMesg[80];
    struct message retrievedMesg;
    int queueOwner = atoi(requests[i].client_num);
    retrievedMesg = *queueList[queueOwner]->m;
    
    sprintf(servMesg, "Message from Client%s%s",retrievedMesg.sender, retrievedMesg.mesgText);
    int fd1 = open(requests[i].priv_fifo,O_WRONLY);
    write(fd1, &servMesg, sizeof(servMesg));
    //unlink(requests[i].priv_fifo);
    close(fd1);
    signalClient(retrievedMesg);
    //print system request details
    printf("\nSystem Request details: sys_call:%s client_num:%s param_num:%d ", requests[i].sys_call, requests[i].client_num, requests[i].param_num);
}


//functions for linked list from tutorialspoint.com

//display the list
void printList() {
   struct node *ptr = head;
   printf("\n[ ");
    
   //start from the beginning
   while(ptr != NULL) {
      printf("(%d,%d) ",ptr->key,ptr->data);
      ptr = ptr->next;
   }
    
   printf(" ]");
}
//insert link at the first location
void insertFirst(int key, int data) {
   //create a link
   struct node *link = (struct node*) malloc(sizeof(struct node));
    
   link->key = key;
   link->data = data;
    
   //point it to old first node
   link->next = head;
    
   //point first to new first node
   head = link;
}

//is list empty
bool isEmpty() {
   return head == NULL;
}

int length() {
   int length = 0;
   struct node *current;
    
   for(current = head; current != NULL; current = current->next) {
      length++;
   }
    
   return length;
}

//delete a link with given key
struct node* delete(int key) {

   //start from the first link
   struct node* current = head;
   struct node* previous = NULL;
    
   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->key != key) {

      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //store reference to current link
         previous = current;
         //move to next link
         current = current->next;
      }
   }
   //found a match, update the link
   if(current == head) {
      //change first to point to next link
      head = head->next;
   } else {
      //bypass the current link
      previous->next = current->next;
   }
    
   return current;
}

struct mesgQueue *createMesgQueue(struct SystemRequest requests[i]){
    struct mesgQueue *queue = malloc(100);
    strcpy(queue->queueOwner,requests[i].client_num);
    return queue;
}

void signalClient(struct message retrievedMesg){
    char servMesg[80];
    char senderFifo[50];
    sprintf(senderFifo, "FIFO_%s", retrievedMesg.sender);
    strcpy(servMesg, "1");
    int fd1 = open(senderFifo,O_WRONLY);
    write(fd1, servMesg, sizeof(servMesg));
    close(fd1);
    
}
