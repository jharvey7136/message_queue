Message Queue Simulation
========================

## John Harvey

### About 
Simple C program to demonstrate how a message queue works with an ARQ bit. The program simulates a basic banking system where clients have balances and can send and receive. 

### Install Notes - Ubuntu
1. Clone repo with git `git clone https://github.com/jharvey7136/message_queue.git`
2. Navigate to app's root directory through terminal
3. Compile program with command: `make`
4. Run program with command: `make run`
* Output is logged in server.txt
* Clean object files and executables with command: `make clean`

### Design
* Struct for message buffer, struct for client info
* Client commands: Open, Deposit, Withdraw, Close, Transfer
* Create and open file for logging:  
```c
FILE *fp;
  fp = fopen("server.txt", "w+");
```
* Fork 4 clients
* Function to handle client sends:  
```c
void ClientSends(int cID, int ARQbit, const char *command, double value, int cID2)
```
* Function to handle server receives:  
```c
void ServerReceives(){
//receive message type 1
  rbuf.mtype = 1;
  if (msgrcv(msqid, &rbuf, MSGSZ, 1, 0) < 0){
    perror("msgrcv");
    exit(1);
  }
  printf("Received message '%s'\n", rbuf.mtext );
```
* Function to handle server sending:  
```c
void ServerSends(int cID, int ARQbit, double balance)
```






