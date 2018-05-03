//John Harvey
//Message Queue

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MSGSZ 128

//prototypes
void ChildProcess(int clientID);
void ParentProcess();
void ClientSends(int cID, int ARQbit, const char *command, double value, int cID2);
void ServerReceives();
void ServerSends(int cID, int ARQbit, double balance);

//struct for message buffer
typedef struct msgbuf {
  long mtype;
  char mtext[MSGSZ];
} message_buf;

//struct for client info
struct Clients {
  int cID;
  double balance;
};


const char *commands[5] = {"Open", "Deposit", "Withdraw", "Close", "Transfer"};
double balance[4] = {300.0, 1345.0, 432.0, 588.3};
double values[5] = {23.0, 163.7, 74.2, 92.4, 102.3};



int main(int argc, char const *argv[]){

  //open file for writing
  FILE *fp;
  fp = fopen("server.txt", "w+");

  pid_t pid[4];

  int i;

  //declare client structs
  struct Clients Client2, Client3, Client4, Client5;

  //setup client id's and balances
  Client2.cID = 2;
  Client2.balance = balance[0];

  Client3.cID = 3;
  Client3.balance = balance[1];

  Client4.cID = 4;
  Client4.balance = balance[2];

  Client5.cID = 5;
  Client5.balance = balance[3];

  int msqid;
  int msgflg = IPC_CREAT | 0666;
  key_t key;
  message_buf sbuf;
  message_buf rbuf;
  size_t buf_length;

  key = 1234;

  //****************fork client 2**************************//
  pid[0] = fork();
  if (pid[0] == 0){
    //in client 2 process
    fputs("Client 2 created\n", fp);
    printf("Client %d starting balance: %f\n\n",Client2.cID, Client2.balance );
    //client 2 deposits 163.7 into client 2
    ClientSends(Client2.cID, 1, commands[1], values[1], 2);
    fputs("Client 2 deposited 163.7 into client 2 account\n", fp);
    ServerReceives();
    Client2.balance = Client2.balance + values[1];
    ServerSends(Client2.cID, 1, Client2.balance );
  }


  //****************fork client 3**************************//
  pid[1] = fork();
  if (pid[1] == 0){
    //in client 3 process
    fputs("Client 3 created\n", fp);
    printf("Client %d starting balance: %f\n\n",Client3.cID, Client3.balance );
    //client 3 Withdraws 74.2 from client 3
    ClientSends(Client3.cID, 1, commands[2], values[2], 3);
    fputs("Client 3 withdraws 74.2 from client 3 account\n", fp);
    ServerReceives();
    Client3.balance = Client3.balance - values[2];
    ServerSends(Client3.cID, 1, Client3.balance );
  }


  //****************fork client 4**************************//
  pid[2] = fork();
  if (pid[2] == 0){
    //in client 4 process
    fputs("Client 4 created\n", fp);
    printf("Client %d starting balance: %f\n\n",Client4.cID, Client4.balance );
    //client 4 transfers 92.4 to client 5
    ClientSends(Client4.cID, 1, commands[4], values[3], 5);
    fputs("Client 4 transfers 92.4 to client 5\n", fp);
    ServerReceives();
    Client4.balance = Client4.balance - values[3];
    Client5.balance = Client5.balance + values[3];
    ServerSends(Client4.cID, 1, Client4.balance );
  }
  //****************fork client 5**************************//
  pid[3] = fork();
  if (pid[3] == 0){
    //in client 5 process
    fputs("Client 5 created\n", fp);
    printf("Client %d starting balance: %f\n\n",Client5.cID, Client5.balance );
    //client 5 deposits 102.3 to client 5
    ClientSends(Client5.cID, 1, commands[1], values[4], 5);
    fputs("Client 5 deposits 102.3 to client 5\n", fp);
    ServerReceives();
    Client5.balance = Client5.balance + values[4];
    ServerSends(Client5.cID, 1, Client5.balance );
  }

  fclose(fp);
  return 0;
}



//***********************function to handle client sends*********************//
void ClientSends(int cID, int ARQbit, const char *command, double value, int cID2){
  int msqid;
  int msgflg = IPC_CREAT | 0666;
  key_t key;
  message_buf sbuf;
  message_buf rbuf;
  size_t buf_length;

  key = 1234;

  char str[128];
  sprintf(str, "%d, %d, %s, %f, %d", cID, ARQbit, command, value, cID2);


  if ((msqid = msgget(key, msgflg)) < 0){
    perror("msgget");
    exit(1);
  }


  sbuf.mtype = 1;

  (void)strcpy(sbuf.mtext, str);

  buf_length = strlen(sbuf.mtext) + 1;

  if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0){
    //printf("%d, %d, %s\n", msqid, sbuf.mtype, sbuf.mtext );
    perror("msgsnd");
    exit(1);
  }
  else
    printf("Message '%s' sent\n", sbuf.mtext );

}



//*******************************function to handle the server receiving***************//
void ServerReceives(){
  int msqid;
  int msgflg = IPC_CREAT | 0666;
  key_t key;
  message_buf sbuf;
  message_buf rbuf;
  size_t buf_length;

  key = 1234;


  if ((msqid = msgget(key, msgflg)) < 0){
    perror("msgget");
    exit(1);
  }

  //receive message type 1
  rbuf.mtype = 1;
  if (msgrcv(msqid, &rbuf, MSGSZ, 1, 0) < 0){
    perror("msgrcv");
    exit(1);
  }
  printf("Received message '%s'\n", rbuf.mtext );
}


//*****************************function to handle the server sending************************//
void ServerSends(int cID, int ARQbit, double balance){
  int msqid;
  int msgflg = IPC_CREAT | 0666;
  key_t key;
  message_buf sbuf;
  message_buf rbuf;
  size_t buf_length;

  key = 1234;

  char str[128];
  sprintf(str, "Client %d, ARQ bit %d, new balance = %f", cID, ARQbit, balance);

  //Send message
  sbuf.mtype = cID;
  (void)strcpy(sbuf.mtext, str);
  buf_length = strlen(sbuf.mtext) + 1;


  if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0){

    perror("msgsnd");
    exit(1);
  }
  else
    printf("Message '%s' sent\n\n", sbuf.mtext );

}
