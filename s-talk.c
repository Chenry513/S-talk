#include "list.h"
#include "monRoutines.h"
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
char* myPort;
char* remoteHost;
char* remotePort;
static List* sendList;
static List* printList;

pthread_t readerThread;
pthread_t printerThread;
pthread_t senderThread;
pthread_t recieverThread;

#define BUFSIZE 256

void freer(void * v) {
    return;
}
static void* reader(void * nothing){
        // Declaring variables
        int numRead;
        char exitCode[] = "!\n";
        while(1) {
            //produce new item
            char msg[BUFSIZE];
            numRead = read(0, msg, BUFSIZE);
            if(numRead == -1){
                exit(4);
            }
            msg[numRead] = '\0';
            //critical section start
            monPrependSend(sendList, msg);
            //critical section end
            if(strcmp(msg, exitCode) == 0) {
                //end this thread and printing thread and recieving thread
                printf("exit key inputted\n");
                pthread_cancel(printerThread);
                pthread_cancel(recieverThread);
                pthread_exit(NULL);
            }
        }
        return NULL;
}

static void *UDPsender(void * nothing) {
    //prep socket
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; 
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(remoteHost, remotePort, &hints, &servinfo)) != 0) {
        exit(1);
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        break;
    }

    if (p == NULL) {
        exit(2);
    }

    freeaddrinfo(servinfo);
    char exitCode[] = "!\n";
    while(1) {
    
        char * msg = monTrimSend(sendList);
        if(msg == NULL){
            exit(3);
        }
        //send item
        numbytes = sendto(sockfd, msg, strlen(msg), 0, p->ai_addr, p->ai_addrlen);
        if(numbytes == -1) {
            printf("failed to send msg");
            exit(1);
        }
        //exit code sent
        if(strcmp(msg, exitCode) == 0) {
            //done with socket
            close(sockfd);
            pthread_exit(NULL);
        }
    }
    return NULL;
}


static void * printer(void * nothing) {
    char exitCode[] = "!\n";
    int numWrote;
    while(1) {
        char* buf;
        buf = monTrimRec(printList);
        if(buf == NULL){
            exit(3);
        }
        numWrote = write(1, buf, strlen(buf));
        if(numWrote == -1){
            exit(4);
        }
        //if just printed !
        if(strcmp(buf, exitCode) == 0)  {
            pthread_exit(NULL);
        }
    }
    return NULL;

}

static void * UDPreceiver(void * nothing) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; 

    if ((rv = getaddrinfo(NULL, myPort, &hints, &servinfo)) != 0) {
        exit(1);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        exit(2);
    }

    freeaddrinfo(servinfo);

    addr_len = sizeof their_addr;
    char exitCode[] = "!\n";
    while(1) {
        char buf[BUFSIZE];
        //recieve item
        numbytes = recvfrom(sockfd, buf, BUFSIZE-1 , 0,(struct sockaddr *)&their_addr, &addr_len);
        if(numbytes == -1) {
            printf("error recieving from other process");
            exit(1);
        }
        //critical section
        //prepend to list
        buf[numbytes] = '\0';
        monPrependRec(printList, buf);
        //critical section end
        if(strcmp(buf, exitCode) == 0){
            //end this thread and sending thread and reading thread
            printf("other user terminated communication, blocking input and printing last messages now\n");
            pthread_cancel(senderThread);
            pthread_cancel(readerThread);
            pthread_exit(NULL);
        }
    }
    return NULL;
}
int main (int argc, char * argv[])
{
    // Checks if arguments are correct
    if (argc!=4) {
        printf("Invalid arguments\n");
        return -1;
    }

    // Storing arguments
    myPort = argv[1];
    remoteHost = argv[2];
    remotePort = argv[3];

    // Create shared lists
    sendList = List_create();
    printList = List_create();
    pthread_create(&senderThread, NULL, UDPsender, NULL);
    pthread_create(&recieverThread, NULL, UDPreceiver, NULL);
    pthread_create(&printerThread, NULL, printer, NULL);
    pthread_create(&readerThread, NULL, reader, NULL);
    // Frees the lists
    List_free(printList, &freer);
    List_free(sendList, &freer);
    pthread_exit(NULL);
    return 0;
}