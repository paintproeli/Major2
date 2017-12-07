#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>

int serverfd = 0, connectionHandler = 0, client1sockfd = 0, client2sockfd = 0;//declarations of required variables
struct sockaddr_in serv_addr;
int client1 = 0;
int client2 = 0;
int cli1total = 0;
int cli2total = 0;
int numberOfClientsConnected = 0;
void *server(void *arg);
int main(int argc, char *argv[])
{
	char sending[1025];//sending buffer
    time_t ticks; //timer ticks
	int whichClient = 0;//which client is connected
	
    if(argc != 2)//invalid no. of arguments
    {
        printf("\n Usage: *exec* <port>\n");
        return 1;
    }
    serverfd = socket(AF_INET, SOCK_STREAM, 0);//
	
    memset(&serv_addr, '0', sizeof(serv_addr));//sets 0
    memset(sending, '0', sizeof(sending)); //sets 0
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
	
	//unlink(2);
    bind(serverfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    listen(serverfd, 10); //listen for connections
	pthread_t serverThread;// start server up

    while(1)
    {

        connectionHandler = accept(serverfd, (struct sockaddr*)NULL, NULL); //holds the fd for when a client connects

        if (client1 == 0){ //if client 1 doesn't exist atm
            whichClient = 1;//tell it client 1 has connected
            client1 = 1; //client 1 = TRUE
            numberOfClientsConnected += 1; 
            client1sockfd = connectionHandler;//set cli1fd
        } else {
             whichClient = 2;//client 2 has connected
            client2 = 1; //client 2 = TRUE
            numberOfClientsConnected += 1;//increase num of clients counter
            client2sockfd = connectionHandler;//store cli2fd
        }
  
        if(pthread_create( &serverThread , NULL ,  server , &whichClient) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        while (numberOfClientsConnected >2){//keep running
			

        }


     }
     pthread_join(serverThread, NULL);
}

void *server(void *arg)//server function in which server performs required functionality
{ 

    int clientNumber = *(int*)arg;//which client connected
	
    int sockfd = 0;//client 1
    int sockfd2 = 0;//client 2
    int total = 0; // highest number for port swapping
	char sending[1024];// sending buffer
    char recieving[1024];//receiving buffer
	char newClient[1024];//sending buffer
	int n = 0 ;
	
    while(1)
	{				
		bzero(recieving,1024);		
        if (clientNumber == 1){ //client1
		
			n = read(client1sockfd, recieving, sizeof(recieving)-1);
			if(n < 0)
			{
				printf("Error reading from socket\n");
			}
			cli1total += atoi(recieving);//update cli1total
			printf("\nClient%i has sent %s \n", clientNumber,recieving);
			printf("Client 1 total is %d\n",cli1total);
        }  
		
		else//client2
		{
			n = read(client2sockfd, recieving, sizeof(recieving)-1);
			if(n < 0)
			{
				printf("Error reading from socket\n");
			}
			cli2total += atoi(recieving);//update cli2 total
			printf("\nClient%i has sent %s \n", clientNumber,recieving);
			printf("Client 2 total is %d\n",cli2total);
		}
		
	


        if(n <= 0)//if a client dc's
        {
            if (clientNumber == 1)//client one disconnected
			{
                printf("\n Client 1 Disconnected \n");
				client1 = 0; //client1 = FALSE
                numberOfClientsConnected = numberOfClientsConnected - 1;
            } 
			else // client 2 disconnected
			{
				printf("\n Client 2 Disconnected \n");
                client2 = 0;//client 2 = FALSE
                numberOfClientsConnected = numberOfClientsConnected - 1;
            }
            break;

        }
		
        //store whichever total is the highest
		if (cli1total > cli2total)
		{
			total = cli1total;
		}
		else
		{
			total = cli2total;
		}

		
		
		
		
        if (total > 1023 && total < 49152)//telling a client to be the host
		{

            if (numberOfClientsConnected == 2)//if there's 1 or 2 clients 
			{
              
                printf("\n Sending portno to other client\n");
                bzero(newClient,1024);
                sprintf(newClient, "PORT %d",total);//puts "PORT ____" into newClient string
				
                if (clientNumber == 1)//whichever client was the last to send
				{
                    write(client2sockfd, newClient, strlen(newClient));
                }
				else 
				{
                     write(client1sockfd, newClient, strlen(newClient));
                }

            }
			else//resets the values
			{
                printf("Resetting Value \n");
                if(total == cli1total)//reset the counter for each client connected
				{
					total =0;
					cli1total = 0;
				}
				else
				{
					total = 0;
					cli2total = 0;
				}
				
            }

        }
		else//not in the portno values
		{
			if (clientNumber == 1)
			{
				 sprintf(sending, "%d",cli1total);//sends the total 
				 write(client1sockfd, sending, strlen(sending));//writes to client 1
			} 
			else 
			{
				 sprintf(sending, "%d",cli2total);//sends the total 
				 write(client2sockfd, sending, strlen(sending));// writes to client 2
			}
		}
    }

}


