/*
* Filename: cli.c
* Author:  Zaid Omar & Shreyansh Tiwari
* Date:    4/6/2017
* Description: This holds the server for communicating against the client
* and creating connections for the client to connect to
*/
//LIBRARY DECLARATIONS
#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"string.h"
#include"netinet/in.h"
#include"pthread.h"
#include"arpa/inet.h"

//CONSTANT DEFINITIONS
#define PORT 4693
#define BUF_SIZE 2000
#define CLADDR_LEN 100

//global variable
int i = 0;
//Struct to store information of client
struct clientInfo
{
	//variable declarations within struct
	int sockfd;
	struct sockaddr_in cliAddr;
	char name[6];
	char ip[1000];
}clients[10];

void * receiveMessage(void * socket);

void main()
{
	//structure for handling internet address
	struct sockaddr_in addr;
	struct sockaddr_in cl_addr;

	//integer declarations for length and sockets
	int sockfd = 0;
	int len = 0;
	int ret = 0;
	int newsockfd = 0;

	//char declaration of buffer
	char buffer[BUF_SIZE];

	//char variable declaration of client address
	char clientAddr[CLADDR_LEN];

	//process thread declaration
	pthread_t rThread;

	//socket descriptor that is returned by socket()
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//if less than 0, there is an error creating the socket
	if (sockfd < 0)
	{
		printf("Error creating socket!\n");
		exit(1);
	}

	//if no error, print socket is created
	printf("Socket created...\n");

	//fill block of memory for address
	memset(&addr, 0, sizeof(addr));

	//Using internet communication
	addr.sin_family = AF_INET;
	//Ip of current machine
	addr.sin_addr.s_addr = INADDR_ANY;
	//port being used (declared as a constant)
	addr.sin_port = PORT;

	//integer ret is equal to the bind which is attaching the struct to the socket
	ret = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
	//error checking to check for an error binding
	if (ret < 0)
	{
		printf("Error binding!\n");
		exit(1);
	}
	printf("Binding done...\n");

	printf("Waiting for a connection...\n");
	//listening to the clients
	listen(sockfd, 10);

	//int len is equal to the size of the client address
	//len = sizeof(cl_addr);

	len = sizeof(clients[i].cliAddr);
	//infinite loop
	while (1)
	{
		//accepting the connection from the client
		newsockfd = 0;
		newsockfd = accept(sockfd, (struct sockaddr *) &(clients[i].cliAddr), (socklen_t *) &len);
		

		struct sockaddr_in addrr;
socklen_t socklen = sizeof(addrr); // important! Must initialize length, garbage produced otherwise
if (getsockname(newsockfd, (struct sockaddr*) &addrr, &socklen) < 0) {
    //log_message(LOG_ERR, "Failed to get socket address");
}
printf("Connection accepted from Address: %s, port: %d\n", inet_ntoa(addrr.sin_addr), ntohs(addrr.sin_port));
		strcpy(&clients[i].ip[0],inet_ntoa(addrr.sin_addr));

		//error checking
		if (newsockfd < 0)
		{
			printf("Error accepting connection!\n");
			exit(1);
		}
		//array of client
		clients[i].sockfd = newsockfd;

		memset(buffer, 0, BUF_SIZE);


		//creating a new thread for receiving messages from the client
		ret = pthread_create(&rThread, NULL, receiveMessage, (void *)newsockfd);
		if (ret)
		{
			printf("ERROR: Return Code from pthread_create() is %d\n", ret);
			exit(1);
		}
	}
	//close the sockets
	//close(newsockfd);
	close(sockfd);

	pthread_exit(NULL);
	return;
}

//Function Name: receiveMessage
//Paramenters: void * socket
//Description: This function allows the server to receive messages that are being sent
//from the clients
//Return: VOID N/A
void * receiveMessage(void * socket)
{
	//integer declarations
	int sockfd = 0;
	int ret = 0;
	int ctr = 0;
	int cur = i;
	//increment i
	i++;

	//char declarations
	char buffer[BUF_SIZE];
	char *name;

	sockfd = (int)socket;
	//memory allocate buffer
	memset(buffer, 0, BUF_SIZE);
	//ret is equal to the functon of receive message from the socket
	ret = recvfrom(sockfd, &clients[cur].name[0], 6, 0, NULL, NULL);
	//error checking
	if (ret < 0)
	{
		printf("Error receiving data!\n");
	}
	//for loop containing
	for (;;)
	{
		//ret is equal to the functon of receive message from the socket
		ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
		if(!strcmp("bye",buffer)){
			close(sockfd);
			return;
		}
		if (ret < 0)
		{
			printf("Error receiving data!\n");
		}
		else
		{
			for (ctr = 0;ctr<i;ctr++)
			{
				//send Ip address
				sendto(clients[ctr].sockfd, clients[cur].ip, 100, 0, (struct sockaddr *) &(clients[ctr].cliAddr), sizeof(clients[ctr].cliAddr));
				//send Name
				sendto(clients[ctr].sockfd, clients[cur].name, 6, 0, (struct sockaddr *) &(clients[ctr].cliAddr), sizeof(clients[ctr].cliAddr));
				//send Ip Message
				sendto(clients[ctr].sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &(clients[ctr].cliAddr), sizeof(clients[ctr].cliAddr));

			}
		}
	}
}
