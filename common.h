/*****************************************************************************
 * Author: Rishikesh Nagare
 * Project: Seat mapping application 
 * Course: Network Applications
 * Email: rishikeshnagare@gmail.com 
 * 
 *****************************************************************************
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ARRAYSIZE(_x)    (sizeof(_x) / sizeof((_x)[0]))
#define MIN(x, y)        (((x) <= (y)) ? (x) : (y))

#define PORT_STRLEN      6
#define MAX_TITLE_LEN    32

#define MAX_BOARD_DATA_SIZE 8192
#define MAX_SEATS 36

/**
 *  Message type exchanged between client/server.
 */
typedef enum MsgType {
    MSG_UNKNOWN = 0,
    MSG_SHOW    = 1,
    MSG_CLEAR   = 2,
    MSG_POST    = 3,
    /* Server -> Client */
    MSG_BOARD   = 4,
    MSG_STATUS  = 5,
    MSG_MAP = 6,
    MSG_BOOK =7,
    MSG_CANCEL =8,
    MSG_AVAIL =9,
} MsgType;

typedef enum MsgStatus {
    MSG_STATUS_SUCCESS = 0,
} MsgStatus;

/**
 * Data type for messages exchanged between client/server.
 */
typedef struct MsgHdr {
    short type;
    short status;
    int   dataSize;
    char  data[0];
    
} MsgHdr;

typedef struct StudentInfo {
    
    char studentFName[20];
    char studentLName[20];
    char studentID[10];
    int studentDOBM;
    int studentDOBD;
    int studentDOBY;
    char password[20];
} StudentInfo;

typedef struct SeatInfo {
StudentInfo s[MAX_SEATS];
int seatNumber;
} SeatInfo;


void Log(const char *fmt, ...);
void Error(const char *fmt, ...);

int ReadFully(int sd, void *buf, int nbytes);
int WriteFully(int sd, void *buf, int nbytes);

void SocketAddrToString(const struct sockaddr_in *addr, char *addrStr,
                        int addrStrLen);
void SocketAddrToString6(const struct sockaddr *addr, char *addrStr,
                         int addrStrLen);
void PrintMsg(const MsgHdr *msg, const char *prefix); 

#endif

