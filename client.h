/*****************************************************************************
 * Author: Rishikesh Nagare
 * Project: Seat mapping application 
 * Course: Network Applications
 * Email: rishikeshnagare@gmail.com 
 * 
 *****************************************************************************
 */

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"

char seatMapClient[MAX_SEATS];
/**
 * The client command line arguments.
 */
typedef struct ClientArgs {
    const char     *svrHost;
    unsigned short  svrPort;
} ClientArgs;

void ParseArgs(int argc, char *argv[], ClientArgs *cliArgs);
void Client(int sock, const ClientArgs *cliArgs);

#endif

