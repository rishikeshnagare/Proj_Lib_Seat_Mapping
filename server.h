/*****************************************************************************
 * Author: Rishikesh Nagare
 * Project: Seat mapping application 
 * Course: Network Applications
 * Email: rishikeshnagare@gmail.com 
 * 
 *****************************************************************************
 */

#ifndef _SERVER_H_
#define _SERVER_H_
#include "common.h"



/**
 * The server command line arguments.
 */
typedef struct ServerArgs {
    unsigned short listenPort;
} ServerArgs;

char SeatMatrixServer[MAX_SEATS];


void ParseArgs(int argc, char *argv[], ServerArgs *svrArgs);
void Server(int sd);

#endif

