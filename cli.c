#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"string.h"
#include"netinet/in.h"
#include"netdb.h"
#include"pthread.h"
#include<ncurses.h>
#include<time.h>

#define PORT 4693
#define BUF_SIZE 20080


//store all the user information here
char message[10][41];
char namelist[10][6];
time_t times[10];
char ips[10][100];
char myname[6];

int i=0,temp=0,k=0,first=0,j=0,n,len;


//Multithreaded function to recieve message from server
void * receiveMessage(void * socket)
{
 int sockfd, ret;
 char buffer[BUF_SIZE], name[6], ip[100];
 char temptime[20];

 // remap the socket id to int from void
 sockfd = (int) socket;
 memset(buffer, 0, BUF_SIZE);

 // infinite loop to receive the message until user enters "bye"
 for (;;)
 {

  ret = recvfrom(sockfd, ip, 100, 0, NULL, NULL); // recieve IP address
  ret = recvfrom(sockfd, name, 6, 0, NULL, NULL);  //recieve Name
  ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL); //recieve Message


  if (ret < 0)
  {
   printw("Error receiving data!\n");
  }
  else
  {
    strcpy(message[i],buffer);
    strcpy(&namelist[i][0],name);

  // get the current time
    times[i] = time(0);
    strcpy(&ips[i][0],ip);
    temp = k = i;
    j=0;

  // clear the screen for new data
    clear();
    refresh();

  // goto the start of the terminal
    move(0,0);

  // print the promt
    printw(">\n");
  // horizontal line
  hline('-',80);

  //  Print the last 10 messages and other informations in reverse order
    do{
    if (k == -1)
    {
      k = 9;
    }

    // copy the time to temptime variable
      strcpy(temptime,ctime(&times[k]));
      strcpy(temptime,&temptime[11]);

    // add null termination to temptime
      temptime[8]='\0';

    if (strcmp(myname, &namelist[k][0]))
    {
      mvprintw((j + 2), 0, "%-15s [%-5s] << %-40s (%s)\n", &ips[k][0], &namelist[k][0], &message[k][0], temptime);
    }

    else
    {
      // print the information and ip for machine
      mvprintw((j + 2), 0, "%-15s [%-5s] >> %-40s (%s)\n", &ips[k][0], &namelist[k][0], &message[k][0], temptime);

      // if message is bye, return to previous function
      if (!strcmp("bye\0", &message[k][0]))
      {
        exit(1);
        return;
        // close(socket);
      }
    }

      if(first==j)
        break;

    // if
      j++;
      k= (k-1);

    }while(k != i && j!=10);

  // ncurses functions
  move(0,1);
  clrtoeol();
  refresh();

    i = (i + 1) % 10;
    first++;
 }
}

}

int main(int argc, char**argv)
{

 struct sockaddr_in addr, cl_addr;
 int sockfd, ret;
 char * serverAddr;
 pthread_t rThread;
 char tempbuf[41], bye[12]="bye",buffer[BUF_SIZE];

 // first refresh operation to clear the screen
 initscr();
 start_color();
 clear();
 refresh();

 // if the number of arguments are not valid then display error and exit
 if (argc < 2)
 {
  printw("usage: client < ip address >\n");
  exit(1);
 }

 // storing the serverAddr
 serverAddr = argv[1];

 // Checking if the socket exists
 sockfd = socket(AF_INET, SOCK_STREAM, 0);

 if (sockfd < 0)
 {
  printw("Error creating socket!\n");
  endwin();
  exit(1);
 }

 // Socket created
 printw("Socket created...\n");

 // initializing all the bits to 0
 memset(&addr, 0, sizeof(addr));

 // Using internet communication
 addr.sin_family = AF_INET;
 // ip address of machine
 addr.sin_addr.s_addr = inet_addr(serverAddr);
 // port that we're using
 addr.sin_port = PORT;


 // connect to the server
 ret = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));

 // check if successfully connected, exit if dont
 if (ret < 0)
 {
  printw("Error connecting to the server!\n");
  endwin();
  exit(1);
 }

 // connected
 printw("Connected to the server...\n");

 // initialize all bits to 0
 memset(buffer, 0, BUF_SIZE);


 printw("Enter your messages one by one and press return key!\n");

 //creating a new thread for receiving messages from the server
 ret = pthread_create(&rThread, NULL, receiveMessage, (void *) sockfd);

 //
 if (ret)
 {
  printw("ERROR: Return Code from pthread_create() is %d\n", ret);
  endwin();
  exit(1);
 }

 // ncurses functions
 refresh();
 printw("Enter the name:\n");
 refresh();
 getstr(myname);
 clear();
 move(0,0);
myname[5] = '\0';
 printw("> ");
 refresh();

 // send the name of the user to server
 ret = sendto(sockfd, myname, 6, 0, (struct sockaddr *) &addr, sizeof(addr));

 // error sending the name
  if (ret < 0)
  {
   printw("Error sending data!\n\t-%s", buffer);
   endwin();
  }

  // put new line character to buffer
  buffer[0]='\n';

  // run infinite loop
 while (1)
 {
   // send the message to server
  mvgetstr(0,1, buffer);

  // length of message
  len = strlen(buffer);
  n=0;

  // run loop until the whole message is sent
  do
  {
    // make sure the message is 40 characters long
    strncpy(tempbuf,&buffer[n*40],40);

  // send the message to server
    ret = sendto(sockfd, tempbuf, 41, 0, (struct sockaddr *) &addr, sizeof(addr));

  // couldn't send data
    if (ret < 0)
  {
     printw("Error sending data!\n\t-%s", buffer);
     endwin();
    }

    n++;

  }while((len=len-40)>0);

  // get out if user input bye
  if(!strcmp(tempbuf,"bye\0"))
    break;
 }
pthread_cancel(rThread);
 // exiting the thread
 pthread_exit(rThread);

 // closing the socket
 close(sockfd);
 endwin();
 exit(1);
 return 0;
}
