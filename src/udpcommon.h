#ifndef __UDP_COMMON_H
#define __UDP_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#define CNW_NAME_SERVICE_GROUP_ADDR "225.1.0.0"		//multicast
#define CNW_NAME_SERVICE_RECEIVE_PORT 3333		//	server receive port  :server
#define CNW_NAME_SERVICE_SEND_PORT 3334			// server send port:    server

#define CNW_NAME_SERVICE_QUERY_PORT 3333	// client query port  : client
#define CNW_NAME_SERVICE_REPLY_PORT 3334    // server reply port  : client


#define WAIT_REPLY_TIMEOUT 3

#define BUFF_LEN 512
#define MAC_LEN 18

typedef enum _CNW_Device_Type_{
	Type_Any = 0,
	Type_Host,
	Type_Client,
	Type_Unknown,
}CNW_Device_Type;

typedef struct _query_struct_{
	CNW_Device_Type device_type;
}query_struct;

#define MAX_NAME_LENGTH 256

typedef struct _reply_struct_{
	CNW_Device_Type device_type;
	char device_name[MAX_NAME_LENGTH];
	char device_mac[MAC_LEN];
}reply_struct;


int udp_create_sender();
int udp_create_receiver(char *mgroup, int port);

#endif
