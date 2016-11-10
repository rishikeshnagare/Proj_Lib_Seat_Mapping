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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "common.h"
#include "client.h"
#include "math.h"
typedef bool (*CmdFunc)(int sd, char *data, int dataSize);
              
typedef struct CmdHandler {
    const char *cmd;
    CmdFunc     func;
} CmdHandler;

static bool ProcessCmdHelp(int sd, char *data, int dataSize);
static bool ProcessCmdShow(int sd, char *data, int dataSize);
static bool ProcessCmdClear(int sd, char *data, int dataSize);
static bool ProcessCmdPost(int sd, char *data, int dataSize);
static bool ProcessCmdMap(int sd, char *data, int dataSize);
static bool ProcessCmdBook(int sd, char *data, int dataSize);
static bool ProcessCmdCancel(int sd, char *data, int dataSize);

CmdHandler cmdHandlers[] = {
    { "help",  ProcessCmdHelp  },
    { "show",  ProcessCmdShow  },
    { "clear", ProcessCmdClear },
    { "post",  ProcessCmdPost  },
    { "map",  ProcessCmdMap  },
    { "book",  ProcessCmdBook  },
    {"cancel", ProcessCmdCancel},
};


/**
 **************************************************************************
 *
 * \brief Print the usage message and exit the program.
 *
 **************************************************************************
 */
static void
Usage(const char *prog) // IN
{
    Log("Usage:\n");
    Log("    %s <server_ip> <server_port>\n", prog);
    exit(EXIT_FAILURE);
}


/**
 **************************************************************************
 *
 * \brief Parse the client command line arguments.
 *
 **************************************************************************
 */
void
ParseArgs(int argc,             // IN
          char *argv[],         // IN
          ClientArgs *cliArgs)  // OUT
{
    if (argc < 3) {
        Usage(argv[0]);
    }

    memset(cliArgs, 0, sizeof *cliArgs);

    cliArgs->svrHost = argv[1];
    cliArgs->svrPort = atoi(argv[2]);
    if (cliArgs->svrPort == 0) {
        Usage(argv[0]);
    }
}


/**
 **************************************************************************
 *
 * \brief Process the "help" command.
 *
 **************************************************************************
 */
static bool
ProcessCmdHelp(int sd,        // IN
               char *data,    // IN
               int dataSize)  // IN
{
    printf("Commands:\n");
    printf("   book          : Displays a map of seats and allows you to proceed for booking.\n");
    printf("   map           : Get the current seat map.\n");
    printf("   cancel        : Cancels any reservations made on your name.\n");
    printf("                 : WARNING! requires PW (DONT ACT SMART!! (-_-) )\n");
    printf("\n");
    return true;
}


/**
 **************************************************************************
 *
 * \brief Process the "show" command.
 *
 **************************************************************************
 */
static bool
ProcessCmdShow(int sd,        // IN
               char *data,    // IN
               int dataSize)  // IN
{
    MsgHdr req, reply;

    memset(&req, 0, sizeof req);
    req.type = MSG_SHOW;
    
    if (WriteFully(sd, &req, sizeof req) <= 0) {
        return false;
    }

    if (ReadFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }
    if (reply.type != MSG_BOARD) {
        Error("Unexpected reply message type %d\n", reply.type);
        return false;
    }

    while (reply.dataSize > 0) {
        char ch;
        if (ReadFully(sd, &ch, sizeof ch) <= 0) {
            return false;
        }
        printf("%c", ch);
        reply.dataSize--;
    }
    return true;
}


/**
 **************************************************************************
 *
 * \brief Process the "MAP" command.
 *
 **************************************************************************
 */
static bool
ProcessCmdMap(int sd,        // IN
               char *data,    // IN
               int dataSize)  // IN
{
    MsgHdr req, reply;
    
    memset(&req, 0, sizeof req);
    req.type = MSG_MAP;

 

    if (WriteFully(sd, &req, sizeof req) <= 0) {
        return false;
    }

    if (ReadFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }
    if (reply.type != MSG_MAP) {
        Error("Unexpected reply message type %d\n", reply.type);
        return false;
    }

        char ch[100];
            if (ReadFully(sd, &ch, sizeof ch) <= 0) {
            return false;
            }
            int i,j;

        for(i=0;i<MAX_SEATS;i++)
         {
            if(!(i%(int)sqrt(MAX_SEATS)) && i)
                printf("\n");
            printf("    %c%2i    ",ch[i],i+1);
                       
         }
 printf("\n");
    
        //reply.dataSize--;

 return false;
}

/**
 **************************************************************************
 *
 * \brief Process the "BOOK" command.
 *
 **************************************************************************
 */
static bool
ProcessCmdBook(int sd,        // IN
               char *data,    // IN
               int dataSize)  // IN
{
    MsgHdr req, reply;
    
    memset(&req, 0, sizeof req);
    req.type = MSG_BOOK;
    
 

    if (WriteFully(sd, &req, sizeof req) <= 0) {
        return false;
    }

    if (ReadFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }
    if (reply.type != MSG_BOOK) {
        Error("Unexpected reply message type %d\n", reply.type);
        return false;
    }

    SeatInfo tempSeatInfo;

    printf("Choose a seat from the below available map \nA = available\nR = reserved\n");
        char ch[100];
            if (ReadFully(sd, &ch, sizeof ch) <= 0) {
            return false;
            }
            int i;

        for(i=0;i<MAX_SEATS;i++)
         {
            if(!(i% (int)sqrt(MAX_SEATS)) && i)
                printf("\n");
            printf("    %c%2i    ",ch[i],i+1);
                       
         }
     /********************************SENDING SEAT NUMBER ******************************************/    
     printf("\nEnter the seat number to be booked!\n\n");
     scanf("%i", &tempSeatInfo.seatNumber);
     //printf("\nSEAT %i\n\n", tempSeatInfo.seatNumber);
     bool do_while = true;
     do {
    if (ch[tempSeatInfo.seatNumber-1] == 'R' || tempSeatInfo.seatNumber>MAX_SEATS)
        {printf("\nSeat Already Reserved or Invalid Entry!! Max Seats %i :: Enter Different seat Number\n", MAX_SEATS);
         scanf("%i", &tempSeatInfo.seatNumber);}
    else if(ch[tempSeatInfo.seatNumber-1] == 'A' && tempSeatInfo.seatNumber<=MAX_SEATS)
    { do_while = false;}    
     }while (do_while);    
    char choice[2];    
    sprintf(choice,"%i",tempSeatInfo.seatNumber);
    
    if (WriteFully(sd, &choice, sizeof choice) <= 0) {
        return false;
    }
    /************************************************************************************************/

    printf("\n***********************Enter the seat Reservation Details!!************************\n");
    printf("\n******************************Enter your First name!!******************************\n");
    scanf("%s", &tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentFName);
    if (WriteFully(sd, &tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentFName, sizeof tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentFName) <= 0) {
        return false;
    }
    printf("\n*****************************Enter your Last name!!********************************\n");
    scanf("%s", &tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentLName);
    if (WriteFully(sd, &tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentLName, sizeof tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentLName) <= 0) {
        return false;
    }

    printf("\n*****************************Enter your Student ID!!*******************************\n");
    scanf("%s", &tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentID);
    if (WriteFully(sd, &tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentID, sizeof tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentID) <= 0) {
        return false;
    }

    printf("\n***************************Enter your Student password!!***************************\n");
    scanf("%s", &tempSeatInfo.s[tempSeatInfo.seatNumber-1].password);
    if (WriteFully(sd, &tempSeatInfo.s[tempSeatInfo.seatNumber-1].password, sizeof tempSeatInfo.s[tempSeatInfo.seatNumber-1].password) <= 0) {
        return false;
    }    
    char cnfBooking[2];
    printf("\nConfirm booking seat %i? Enter (y/n)\n",tempSeatInfo.seatNumber);
    scanf("%s", &cnfBooking);
    if (WriteFully(sd, &cnfBooking, sizeof cnfBooking) <= 0) {
        return false;
    }





 return false;
}


/**
 **************************************************************************
 *
 * \brief Process the "cancel" command.
 *
 **************************************************************************
 */
static bool
ProcessCmdCancel(int sd,        // IN
                char *data,    // IN
                int dataSize)  // IN
{
    MsgHdr req, reply;

    memset(&req, 0, sizeof req);
    req.type = MSG_CANCEL;
    
    if (WriteFully(sd, &req, sizeof req) <= 0) {
        return false;
    }
    if (ReadFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }
    if (reply.type != MSG_CANCEL) {
        Error("Unexpected reply message type %d\n", reply.type);
        return false;
    }

    SeatInfo tempSeatInfo;
    printf("\n*****************************Enter your Seat Number!!********************************\n");
    scanf("%i", &tempSeatInfo.seatNumber);
    char cSeatNum[2];
    sprintf(cSeatNum,"%i",tempSeatInfo.seatNumber);
    if (WriteFully(sd, &cSeatNum, sizeof cSeatNum) <= 0) {
        return false;
    }


    printf("\n*****************************Enter your Last name!!********************************\n");
    scanf("%s", &tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentLName);
    if (WriteFully(sd, &tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentLName, sizeof tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentLName) <= 0) {
        return false;
    }

    printf("\n*****************************Enter your Student ID!!*******************************\n");
    scanf("%s", &tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentID);
    if (WriteFully(sd, &tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentID, sizeof tempSeatInfo.s[tempSeatInfo.seatNumber-1].studentID) <= 0) {
        return false;
    }

    printf("\n***************************Enter your Student password!!***************************\n");
    scanf("%s", &tempSeatInfo.s[tempSeatInfo.seatNumber-1].password);
    if (WriteFully(sd, &tempSeatInfo.s[tempSeatInfo.seatNumber-1].password, sizeof tempSeatInfo.s[tempSeatInfo.seatNumber-1].password) <= 0) {
        return false;
    }    
   char Ack[2];
    if (ReadFully(sd, &Ack, sizeof Ack) <= 0) {
            return false;
        }
    printf("Ack status %c\n",Ack[0]);


    char cnfBooking[2];
    printf("\nConfirm canellation of seat Number: %i ? Enter (y/n)\n",tempSeatInfo.seatNumber);
    scanf("%s", &cnfBooking);
    if (WriteFully(sd, &cnfBooking, sizeof cnfBooking) <= 0) {
        return false;
        }

    if((cnfBooking[0]=='Y' || cnfBooking[0]=='y') && Ack[0] == 'A')     
    {
        printf("\n***************************Succesfully Cancelled the Booking!!***************************\n");
    }
    else {
        printf("\n***************************Incorrect Data Entered Cancellation Failed!!***************************\n");
    }








    return false;
}



/**
 **************************************************************************
 *
 * \brief Process the "clear" command.
 *
 **************************************************************************
 */
static bool
ProcessCmdClear(int sd,        // IN
                char *data,    // IN
                int dataSize)  // IN
{
    MsgHdr req, reply;

    memset(&req, 0, sizeof req);
    req.type = MSG_CLEAR;
    
    if (WriteFully(sd, &req, sizeof req) <= 0) {
        return false;
    }
    if (ReadFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }
    if (reply.type != MSG_STATUS) {
        Error("Unexpected reply message type %d\n", reply.type);
        return false;
    }
    return false;
}


/**
 **************************************************************************
 *
 * \brief Process the "post" command.
 *
 **************************************************************************
 */
static bool
ProcessCmdPost(int sd,        // IN
               char *data,    // IN
               int dataSize)  // IN
{
    MsgHdr req, reply;

    memset(&req, 0, sizeof req);
    req.type     = MSG_POST;
    req.dataSize = dataSize;
    
    if (WriteFully(sd, &req, sizeof req) <= 0) {
        return false;
    }
    if (WriteFully(sd, data, dataSize) <= 0) {
        return false;
    }

    if (ReadFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }
    if (reply.type != MSG_STATUS) {
        Error("Unexpected reply message type %d\n", reply.type);
        return false;
    }
    return false;
}


/**
 **************************************************************************
 *
 * \brief Dispatch client commands.
 *
 **************************************************************************
 */
void
Client(int sock,                   // IN
       const ClientArgs *cliArgs)  // IN
{
    bool running = true;

    Log("\n*** Welcome to Library seat reservation Client. *** \n\n"); 
    Log("Enter a command or 'help' to see a list of available commands.\n\n");

    while (running) {
        char *cmdBuf;
        char *cmd, *saveptr;
        char *data;
        int cmdBufSize, dataSize;
        int i;

        cmdBuf = readline("cmdLine> ");
        if (cmdBuf == NULL) {
            return;
        }
       
        cmdBufSize = strlen(cmdBuf);

        cmd = strtok_r(cmdBuf, " ", &saveptr);

        data     = cmdBuf + strlen(cmd) + 1;
        dataSize = cmdBufSize - strlen(cmd);

        for (i = 0; i < ARRAYSIZE(cmdHandlers); i++) {
            CmdHandler *handler = &cmdHandlers[i];
            if (strcasecmp(cmd, handler->cmd) == 0) {
                running = handler->func(sock, data, dataSize);
                break;
            }
        }
        if (i == ARRAYSIZE(cmdHandlers)) {
            Error("Unknown command %s type help to get a list of usable commands\n", cmd);
            running = true;
        }
        free(cmdBuf);
    }
}

