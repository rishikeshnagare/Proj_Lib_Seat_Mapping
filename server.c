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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "common.h"
#include "server.h"
 #include <pthread.h>

extern pthread_mutex_t  seatBookMutex;



typedef struct WhiteBoard {
    int  dataSize;
    char dataBuf[MAX_BOARD_DATA_SIZE];
} WhiteBoard;

static WhiteBoard board;

typedef struct CritStudentInfo {
    
    char studentFName[20];
    char studentLName[20];
    char studentID[10];
    int studentDOBM;
    int studentDOBD;
    int studentDOBY;
    char password[20];
} info;

typedef struct CritSeatInfo {
info s[MAX_SEATS];
int  seatNumber;

} CSeatInfo;

static CSeatInfo CritInfo;

typedef bool (*MsgFunc)(int sd, const MsgHdr *req, const char *cliName);
              
typedef struct MsgHandler {
    MsgType     type;
    MsgFunc     func;
} MsgHandler;

static bool ProcessMsgShow(int sd, const MsgHdr *req, const char *cliName);
static bool ProcessMsgClear(int sd, const MsgHdr *req, const char *cliName);
static bool ProcessMsgPost(int sd, const MsgHdr *req, const char *cliName);
static bool ProcessMsgMap(int sd, const MsgHdr *req, const char *cliName);
static bool ProcessMsgBook(int sd, const MsgHdr *req, const char *cliName);
static bool ProcessMsgCancel(int sd, const MsgHdr *req, const char *cliName);

MsgHandler msgHandlers[] = {
    { MSG_SHOW,  ProcessMsgShow  },
    { MSG_CLEAR, ProcessMsgClear },
    { MSG_POST,  ProcessMsgPost  },
    { MSG_MAP,  ProcessMsgMap  },
    { MSG_BOOK,  ProcessMsgBook  },
    { MSG_CANCEL,  ProcessMsgCancel  },
};


/**
 **************************************************************************
 *
 * \brief Show the usage message and exit the program.
 *
 **************************************************************************
 */
static void
Usage(const char *prog) // IN
{
    Log("Usage:\n");
    Log("    %s <port>\n", prog);
    exit(EXIT_FAILURE);
}


/**
 **************************************************************************
 *
 * \brief Parse command line arguments.
 *
 **************************************************************************
 */
void
ParseArgs(int argc,            // IN
          char *argv[],        // IN
          ServerArgs *svrArgs) // OUT
{
    if (argc != 2) {
        Usage(argv[0]);
    }
    svrArgs->listenPort = atoi(argv[1]);
    if (svrArgs->listenPort == 0) {
        Usage(argv[0]);
    }
}


/**
 **************************************************************************
 *
 * \brief Handler for MSG_SHOW.
 *
 **************************************************************************
 */
static bool
ProcessMsgShow(int sd,               // IN
               const MsgHdr *req,    // IN
               const char *cliName)  // IN
{
    MsgHdr reply;

    PrintMsg(req, cliName);

    memset(&reply, 0, sizeof reply);
    reply.type     = MSG_BOARD;
    reply.dataSize = board.dataSize;

    if (WriteFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }
    if (WriteFully(sd, board.dataBuf, board.dataSize) <= 0) {
        return false;
    }

    PrintMsg(&reply, cliName);
    return true;
}


/**
 **************************************************************************
 *
 * \brief Handler for MSG_CLEAR.
 *
 **************************************************************************
 */
static bool
ProcessMsgClear(int sd,               // IN
                const MsgHdr *req,    // IN
                const char *cliName)  // IN
{
    MsgHdr reply;

    PrintMsg(req, cliName);

    memset(&reply, 0, sizeof reply);
    reply.type     = MSG_STATUS;
    reply.status   = MSG_STATUS_SUCCESS;
    reply.dataSize = 0;

    board.dataSize = 0;

    if (WriteFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }

    PrintMsg(&reply, cliName);
    return true;
}

/**
 **************************************************************************
 *
 * \brief Handler for MSG_CANCEL.
 *
 **************************************************************************
 */
static bool
ProcessMsgCancel(int sd,               // IN
                const MsgHdr *req,    // IN
                const char *cliName)  // IN
{
    MsgHdr reply;

    PrintMsg(req, cliName);

    memset(&reply, 0, sizeof reply);
    reply.type     = MSG_CANCEL;
    reply.status   = MSG_STATUS_SUCCESS;
    reply.dataSize = 0;
    
    board.dataSize = 0;

    if (WriteFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }
  /********************************************************************
  CHECK ALL THE DETAILS BEFORE PROCEEDING!!
  *********************************************************************/
   char sNumb[2];
   if (ReadFully(sd, &sNumb, sizeof sNumb) <= 0) {
            return false;
        }
    printf("sNumb %s\n",sNumb);   


   char LastName[20];    
    if (ReadFully(sd, &LastName, sizeof LastName) <= 0) {
            return false;
        }
    printf("LastName %s\n",LastName);    

    char SID[10];    
    if (ReadFully(sd, &SID, sizeof SID) <= 0) {
            return false;
        }
    printf("SID %s\n",SID);    

    char pwd[20];    
    if (ReadFully(sd, &pwd, sizeof pwd) <= 0) {
            return false;
        }
    printf("pwd %s\n",pwd);   

    /************************************LOGIC TO GET THE INFO OF THE SEAT RESERVER*******************************************/
    int iSeat;

    if(strlen(sNumb) == 1){ iSeat = sNumb[0] - '0';}
    else if(strlen(sNumb) == 2) { iSeat = (sNumb[0] - '0')*10 + (sNumb[1] - '0');}
    bool LNameFlag,SIDFlag,PwdFlag;
    char Ack[2];
    LNameFlag = strcmp(CritInfo.s[iSeat-1].studentLName,LastName)==0?true:false;  
    SIDFlag = strcmp(CritInfo.s[iSeat-1].studentID,SID)==0?true:false;    
    PwdFlag = strcmp(CritInfo.s[iSeat-1].password,pwd)==0?true:false;            
    if (LNameFlag && SIDFlag && PwdFlag)
    {
        Ack[0] = 'A'; 
    if (WriteFully(sd, &Ack, sizeof Ack) <= 0) {
        return false;
        }
    }
    else
     {
        Ack[0] = 'N';
        if (WriteFully(sd, &Ack, sizeof Ack) <= 0) {
        return false;
        }
     }

    char confirmation[2];
    if (ReadFully(sd, &confirmation, sizeof confirmation) <= 0) {
            return false;
        }
    printf("confirmation %s\n",confirmation);
    if(Ack[0]=='A' && (confirmation[0] == 'y' || confirmation[0] == 'Y'))
    {
        pthread_mutex_lock(&seatBookMutex);
        printf("CANCEL MUTEX LOCK\n");

        SeatMatrixServer[iSeat-1] = 'A';
        memset(CritInfo.s[iSeat-1].studentLName,0,strlen(CritInfo.s[iSeat-1].studentLName));
        memset(CritInfo.s[iSeat-1].studentLName,0,strlen(CritInfo.s[iSeat-1].studentLName
        memset(CritInfo.s[iSeat-1].studentID,0,strlen(CritInfo.s[iSeat-1].studentID));
        memset(CritInfo.s[iSeat-1].password,0,strlen(CritInfo.s[iSeat-1].password));
        printf("%s",CritInfo.s[iSeat-1].studentFName);          
        
        pthread_mutex_unlock(&seatBookMutex);
        printf("CANCEL MUTEX UNLOCK\n");
    }




    PrintMsg(&reply, cliName);
    return true;
}


/**
 **************************************************************************
 *
 * \brief Handler for MSG_POST.
 *
 **************************************************************************
 */
static bool
ProcessMsgPost(int sd,               // IN
               const MsgHdr *req,    // IN
               const char *cliName)  // IN
{
    MsgHdr reply;
    int bytesToStore, bytesToSkip;

    PrintMsg(req, cliName);

    bytesToStore = MIN(req->dataSize,
                       MAX_BOARD_DATA_SIZE - board.dataSize - 1);
    bytesToSkip = req->dataSize - bytesToStore;
        
    if (bytesToStore > 0) {
        if (ReadFully(sd, board.dataBuf + board.dataSize, bytesToStore) <= 0) {
            return false;
        }
        board.dataSize += bytesToStore;

        /* Always append a newline. */
        board.dataBuf[board.dataSize] = '\n';
        board.dataSize++;
    }   

    while (bytesToSkip > 0) {
        char ch;
        if (ReadFully(sd, &ch, 1) <= 0) {
            return false;
        }
        bytesToSkip--;
    }

    memset(&reply, 0, sizeof reply);
    reply.type     = MSG_STATUS;
    reply.status   = MSG_STATUS_SUCCESS;
    reply.dataSize = 0;

    if (WriteFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }



    PrintMsg(&reply, cliName);
    return true;
}
/**
 **************************************************************************
 *
 * \brief Handler for MSG_MAP.
 *
 **************************************************************************
 */
static bool
ProcessMsgMap(int sd,               // IN
               const MsgHdr *req,    // IN
               const char *cliName)  // IN
{
    MsgHdr reply;

    PrintMsg(req, cliName);

    memset(&reply, 0, sizeof reply);
    reply.type     = MSG_MAP;
    reply.dataSize = board.dataSize;

    if (WriteFully(sd, &reply, sizeof reply) <= 0) {
        return false;
    }
    char msg[100];

    //SeatMatrixServer[8] = 'R';
    strcpy(msg, SeatMatrixServer);
    if (WriteFully(sd, &msg, sizeof msg) <= 0) {
        return false;
    }
    
    printf("SEND FROM HERE");
    PrintMsg(&reply, cliName);

    printf("Out of Map func! SERVER");
    return true;
}

/**
 **************************************************************************
 *
 * \brief Handler for MSG_BOOK.
 *
 **************************************************************************
 */
static bool
ProcessMsgBook(int sd,               // IN
               const MsgHdr *req,    // IN
               const char *cliName)  // IN
{
    MsgHdr reply;

    PrintMsg(req, cliName);

    memset(&reply, 0, sizeof reply);
    reply.type     = MSG_BOOK;
    reply.dataSize = board.dataSize;

    if (WriteFully(sd, &reply, sizeof reply) <= 0) {

        return false;
    }
    char msg[100];

    //SeatMatrixServer[8] = 'R';
    strcpy(msg, SeatMatrixServer);
    if (WriteFully(sd, &msg, sizeof msg) <= 0) {
        return false;
    }
    /*******************************************READING CLIENT DATA*******************************************************/
    char choice[2] ;
    int opt;
     if (ReadFully(sd, &choice, sizeof choice) <= 0) {
            return false;
        }

        printf("%s\n",choice);
       
        if(strlen(choice) == 1)
            { opt = choice[0] - '0';}
        else if(strlen(choice) == 2) { opt = (choice[0] - '0')*10 + (choice[1] - '0');}
        char seatStatus[2];   
    if(SeatMatrixServer[opt-1] == 'A')
         {
            seatStatus[0] = 'A';
         }
    else {seatStatus[0] = 'R';} 
     /*********************************************************************************************************************    
     *  Update the flags for all entries processed!! 
     *   
     **********************************************************************************************************************/
    

    
    char FirstName[20];    
    if (ReadFully(sd, &FirstName, sizeof FirstName) <= 0) {
            return false;
        }
      
    printf("FName %s\n",FirstName);  

    char LastName[20];    
    if (ReadFully(sd, &LastName, sizeof LastName) <= 0) {
            return false;
        }
    printf("LastName %s\n",LastName);    

    char SID[10];    
    if (ReadFully(sd, &SID, sizeof SID) <= 0) {
            return false;
        }
    printf("SID %s\n",SID);    

    char pwd[20];    
    if (ReadFully(sd, &pwd, sizeof pwd) <= 0) {
            return false;
        }
    printf("pwd %s\n",pwd);    
    char confirmation[2];
    if (ReadFully(sd, &confirmation, sizeof confirmation) <= 0) {
            return false;
        }
    printf("confirmation %s\n",confirmation);

    if(SeatMatrixServer[opt-1] == 'A' && (confirmation[0] == 'y' || confirmation[0] =='Y'))
    {
    pthread_mutex_lock(&seatBookMutex);
    printf("MUTEX LOCK\n");
    if(SeatMatrixServer[opt-1] == 'A')
        {
            SeatMatrixServer[opt-1] = 'R';
            CritInfo.seatNumber = opt;
            strcpy(CritInfo.s[opt-1].studentFName ,FirstName);
            strcpy(CritInfo.s[opt-1].studentLName ,LastName);
            strcpy(CritInfo.s[opt-1].studentID ,SID);
            strcpy(CritInfo.s[opt-1].password ,pwd);            
        }
    else
        {
         printf("UNFORTUNATELY SEAT WAS ALREADY BOOKED YOU ARE LATE!!!\n TRY BOOKING ANOTHER SEAT!!\n");
         pthread_mutex_unlock(&seatBookMutex);
        }
    printf("\nSeat %i Booked for the following student: %s ",CritInfo.seatNumber,CritInfo.s[opt-1].studentFName );    
    pthread_mutex_unlock(&seatBookMutex);
    printf("\nMUTEX UNLOCK\n");

    }
    else
        {printf("INVALID SEAT ENTRY OR CONFIRMATION CANCELLED!\n");
        printf("%i",opt);}
    PrintMsg(&reply, cliName);

    
    return false;
}



/**
 **************************************************************************
 *
 * \brief The server routine to handle requests from a particular client.
 *
 **************************************************************************
 */
void
Server(int sd)  // IN
{
    struct sockaddr_storage cliAddr;
    char cliName[INET6_ADDRSTRLEN + PORT_STRLEN];
    socklen_t cliAddrLen;
    MsgHdr req;
    int i;

    cliAddrLen = sizeof cliAddr;
    if (getpeername(sd, (struct sockaddr *)&cliAddr, &cliAddrLen) < 0) {
        perror("Failed to get peer address info for client socket");
        close(sd);
        return;
    }
    SocketAddrToString6((const struct sockaddr *)&cliAddr,
                        cliName, sizeof cliName);

    Log("\nClient %s (sock=%u) connected\n", cliName, sd);

    if (ReadFully(sd, &req, sizeof(req)) > 0) {
        for (i = 0; i < ARRAYSIZE(msgHandlers); i++) {
            MsgHandler *handler = &msgHandlers[i];
            if (handler->type == req.type) {
                handler->func(sd, &req, cliName);
                break;
            }
        }
        if (i == ARRAYSIZE(msgHandlers)) {
            Error("   [%s] Unknown message type %d\n", cliName, req.type);
        }
    }

    Log("Client %s (sock=%u) disconnected\n\n", cliName, sd);
    close(sd);
}

