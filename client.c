#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

int sock = 0, number = 0;//socket
char Buff[1024];//buffer
struct sockaddr_in serv_addr;
int total_amount = 0;//total count
char * ip_cl; //ip
pthread_mutex_t mLock;

void *client(void *arg){

    while(1){
		
		bzero(Buff,1024);
        number = read(sock, Buff, sizeof(Buff)-1);//reads from socket
        if(number < 0)
		{
			printf ("READ FAILED\n");
		}
		
        printf( "The new total for this client is %d\n",atoi(Buff));
		
        int port_info;//port number
            if(strstr(Buff,"PORT")!=NULL){
                    sleep(2);
                    printf( "\nConnecting with Other Client\n");
                    char data_message[1024];//buffer string for messages
                    strcpy(data_message,Buff);//copies message into message string
                    char dlim[] = " ";//delimeter assignments
                    char * port_string= strtok(Buff,dlim);//string conversion
                    port_string = strtok(NULL,dlim);//port channel assignment

                    port_info = atoi(port_string);//ascii to int port number

                    close(sock);//closes socket feed

                    int sock1 = 0; //new socket open
                    char Buff1[1024];//recieves message in this buffer
                    struct sockaddr_in serv_addr1;

                    memset(Buff1, '0',sizeof(Buff1));//sets 0 in buffer

                    if((sock1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)//error checking condition
                    {
                        printf("\n Error : Could not create socket \n");
                        return 0;
                    }

                    memset(&serv_addr1, '0', sizeof(serv_addr1)); //sets 0

                    serv_addr1.sin_family = AF_INET;//sets type
                    serv_addr1.sin_port = htons(port_info); //big endian to little endian

                    if(inet_pton(AF_INET, ip_cl, &serv_addr1.sin_addr)<=0)//error checking condition
                    {
                        printf("\n inet_pton error occured\n");
                        return 0;
                    }

                    if( connect(sock1, (struct sockaddr *)&serv_addr1, sizeof(serv_addr1)) < 0)//error checking condition
                    {
                       printf("\n Error : Connect Failed \n");
                       return 0;
                    }

                    char transBuff[1024];//sending buffer to server

                    sprintf(transBuff, "%i",total_amount);
                    write(sock1, transBuff, strlen(transBuff));//sends to server
                    close(sock1);//closes socket connection

                    printf("\n Sending socket and disconnecting \n");
                    exit(0);



                } else {
                    total_amount = atoi(Buff);

                    if (total_amount > 1023 && total_amount < 49152){// threshold condition required in question
                        printf( "\nStarting Own Server");

                        int listen_to = 0, connect_to = 0;// listening variables.
                        struct sockaddr_in serv_addr2;

                        char Buff2[1025]; //recieves in this buffer

                        listen_to = socket(AF_INET, SOCK_STREAM, 0);//listen function
                        memset(&serv_addr2, '0', sizeof(serv_addr2));//sets 0
                        memset(Buff2, '0', sizeof(Buff2)); //sets 0

                        serv_addr2.sin_family = AF_INET;
                        serv_addr2.sin_addr.s_addr = htonl(INADDR_ANY);
                        serv_addr2.sin_port = htons(total_amount);

                        bind(listen_to, (struct sockaddr*)&serv_addr2, sizeof(serv_addr2)); //binding function

                        listen(listen_to, 10); //listens

                        connect_to = accept(listen_to, (struct sockaddr*)NULL, NULL);//accepts connections

                       read(connect_to, Buff2, sizeof(Buff2)-1);//reads from server
                            Buff2[number] = 0;

                        printf("\nReceived Value from other Client \n");

                    }
                }
        }
}

int main(int argc, char *argv[])// main
{
	int update;

    if(argc != 4) //tells the connected client
    {
        printf("\n Usage: %s <ip of server> <port> <ip of other client>\n",argv[0]);
        return 0;
    }
/*	update = pthread_mutex_init(&mLock, NULL);
	if(update != 0)
	{
		printf("Error: %s\n", strerror(update));
	}
*/	
    ip_cl = argv[3];
    memset(Buff, '0',sizeof(Buff));//sets 0
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)//makes connection with other client
    {
        printf("\n Error : Could not create socket \n");
        return 0;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); //sets 0

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)//error checking
    {
        printf("\n inet_pton error occured\n");
        return 0;
    }

    if( connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)//error checking
    {
       printf("\n Error : Connect Failed \n");
       return 0;
    }

    pthread_t client_thread;

    if( pthread_create( &client_thread , NULL ,  client , (void*) 1) < 0)//error checking
    {
        perror("could not create thread");
        return 0;
    }

	
//	pthread_mutex_lock(&mLock);
    while (1){ //sends value from user input in this loop
        int stuff = 0;
		printf( "\nEnter a value :");
        scanf("%i", &stuff);
        if (stuff == 0 ){
             close(sock);
            exit(0);
        } else {
           char transBuff[1024];
            sprintf(transBuff, "%i",stuff);
            int number = write(sock, transBuff, strlen(transBuff));

        }
    }
//	pthread_mutex_unlock(&mLock);
	
    pthread_join(client_thread, NULL);// creates thread
	
/*	update = pthread_mutex_destroy(&mLock);
	if(update != 0)
	{
		printf("Error: %s\n", strerror(update));
	}
*/	
    return 0;
}
