#include "udpcommon.h"

int udp_create_sender()
{
	struct sockaddr_in addr;
	int fd;
	//struct ip_mreq mreq;
	int yes = 1;
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		return -1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(0);

	if(bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("bind");
		return -1;
	}
	return fd;
}

int udp_create_receiver(char *mgroup, int port)
{
	struct sockaddr_in addr;
	struct ip_mreq mreq;
	int fd;
	int yes = 1;
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		return -1;
	}

	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
	{
		perror("setsockopt (SO_REUSEADDR)");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if(bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		return -1;
	}

	if(mgroup != NULL)
	{
		mreq.imr_multiaddr.s_addr = inet_addr(mgroup);
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
		{
			perror("setsockopt (IP_ADD_MEMBERSHIP)");
			return -1;
		}
	}
	return fd;
}
