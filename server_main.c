/*****************************************************************************
 * Author: Rishikesh Nagare
 * Project: Seat mapping application 
 * Course: Network Applications
 * Email: rishikeshnagare@gmail.com 
 * 
 *****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "server.h"


 
static int            msock           = -1;
static volatile bool  listenerRunning = true;
char remoteIP[INET6_ADDRSTRLEN];

void * serverthread(void * parm);       /* thread function prototype    */

pthread_mutex_t  mut;
pthread_mutex_t  seatBookMutex;

int visits =  0; 


/**
 **************************************************************************
 *
 * \brief Signal handler to stop the server with Ctrl-C.
 *
 **************************************************************************
 */
static void
SignalHandler(int signo)
{
    if (signo == SIGINT) {
        if (msock > 0) {
            shutdown(msock, SHUT_RDWR);
        }
        listenerRunning = false;
    }
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/**
 **************************************************************************
 *
 * \brief Create a TCP listen socket on the given port.
 *
 **************************************************************************
 */
static int
CreatePassiveTCP6(unsigned port)
{
    int msock;
    struct sockaddr_in6 svrAddr;

    msock = socket(AF_INET6, SOCK_STREAM, 0);
    if (msock < 0) {
        perror("Failed to allocate the listen socket");
        exit(EXIT_FAILURE);
    }

    memset(&svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin6_family = AF_INET6;
    svrAddr.sin6_port   = htons(port);
    memcpy(&svrAddr.sin6_addr, &in6addr_any, sizeof svrAddr.sin6_addr);

    if (bind(msock, (struct sockaddr *)&svrAddr, sizeof svrAddr) < 0) {
        perror("Failed to bind IP address and port to the listen socket");
        exit(EXIT_FAILURE);
    }

    if (listen(msock, 10) < 0) {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    return msock;
}


/**
 **************************************************************************
 *
 * \brief Validates the client socket returned by accept().
 *
 * Return true if the client socket (ssock) is valid, false otherwise.
 *
 **************************************************************************
 */
static bool
ValidateClientSocket(int ssock)   // IN
{
    struct sockaddr_storage localAddr;
    struct sockaddr_storage cliAddr;
    socklen_t localAddrLen;
    socklen_t cliAddrLen;
    char svrName[INET6_ADDRSTRLEN + PORT_STRLEN];
    char cliName[INET6_ADDRSTRLEN + PORT_STRLEN];

    if (ssock < 0) {
        if (listenerRunning && errno != EINTR) {
            perror("Failed to accept a connection");
            listenerRunning = false;
        }
        return false;
    }

    localAddrLen = sizeof localAddr;
    if (getsockname(ssock, (struct sockaddr *)&localAddr, &localAddrLen) < 0) {
        perror("Failed to get server address info for new connection");
        close(ssock);
        return false;
    }

    cliAddrLen = sizeof cliAddr;
    if (getpeername(ssock, (struct sockaddr *)&cliAddr, &cliAddrLen) < 0) {
        perror("Failed to get peer address info for new connection");
        close(ssock);
        return false;
    }

    SocketAddrToString6((const struct sockaddr *)&localAddr,
                        svrName, sizeof svrName);
    SocketAddrToString6((const struct sockaddr *)&cliAddr,
                        cliName, sizeof cliName);
    Log("Accepted client %s at server %s\n", cliName, svrName);

    return true;
}


/**
 **************************************************************************
 *
 * \brief The server loop to accept new client connections.
 *
 **************************************************************************
 */
static void
ServerListenerLoop(void)
{
    fd_set rfds;
    fd_set afds;
    int nfds;
    int fd;

    nfds = getdtablesize();  // Get descriptor table size
    FD_ZERO(&afds);
    FD_SET(msock, &afds);    // Watch msock

    while (listenerRunning) {
        memcpy(&rfds, &afds, sizeof(rfds));
        if (select(nfds, &rfds, NULL, NULL, NULL) < 0) {
            perror("Failed to select");
            return;
        }

        if (FD_ISSET(msock, &rfds)) {
            int ssock;
            struct sockaddr_storage cliAddr;
            socklen_t cliAddrLen = sizeof cliAddr;
            ssock = accept(msock, (struct sockaddr *)&cliAddr, &cliAddrLen);

            if (ValidateClientSocket(ssock)) {
                FD_SET(ssock, &afds);
            }
        }

        for (fd = 0; fd < nfds; fd++) {
            if (fd != msock && FD_ISSET(fd, &rfds)) {
                Server(fd);
                FD_CLR(fd, &afds);
            }
        }
    }
}


/**
 **************************************************************************
 *
 * \brief Main entry point.
 *
 **************************************************************************
 */
int main(int argc,      // IN
     char *argv[])  // IN
{
    int index;
    for(index =0 ; index<MAX_SEATS;index++)
    {
        SeatMatrixServer[index] = 'A';
    }
    ServerArgs svrArgs;
    pthread_mutex_init(&mut, NULL);
    pthread_mutex_init(&seatBookMutex, NULL);
    pthread_t  tid;             /* variable to hold thread ID */
    int ssock;		
    signal(SIGINT, SignalHandler);

    ParseArgs(argc, argv, &svrArgs);
    struct sockaddr_storage cliAddr;
    socklen_t cliAddrLen = sizeof cliAddr;
	
    msock = CreatePassiveTCP6(svrArgs.listenPort);

    Log("\nServer started listening at *:%u\n", svrArgs.listenPort);
    Log("Press Ctrl-C to stop the server.\n\n");

    //ServerListenerLoop();


	while ( listenerRunning ) {

         printf("SERVER: Waiting for contact ...\n");
	         if (  (ssock = accept(msock, (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0) {
	                      fprintf(stderr, "accept failed\n");
                              exit (1);
	 }
    ValidateClientSocket(ssock);

	printf("selectserver: new connection from %s on "
                            "socket %d\n",
                            inet_ntop(cliAddr.ss_family,
                                get_in_addr((struct sockaddr*)&cliAddr),
                                remoteIP, INET6_ADDRSTRLEN),
                            ssock);
	pthread_create(&tid, NULL, serverthread, (void *) ssock );
	
     }
			
    close(msock);
    Log("Server stopped listening at *:%u\n", svrArgs.listenPort);

    return 0;
}



void * serverthread(void * parm)
{
   int tsd, tvisits;
   char     buf[100];           /* buffer for string the server sends */

   tsd = (int) parm;
   Server(tsd);
  
   pthread_mutex_lock(&mut);
        tvisits = ++visits;
   pthread_mutex_unlock(&mut);

   sprintf(buf,"This server has been contacted %d time%s\n",
	   tvisits, tvisits==1?".":"s.");

   printf("SERVER thread: %s", buf);
   send(tsd,buf,strlen(buf),0);
   
   close(tsd);
   pthread_exit(0);
} 

