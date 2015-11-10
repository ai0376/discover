#include "udpcommon.h"

static void do_query(CNW_Device_Type device_type)
{
	int q_fd, r_fd;
	struct timeval timeout;
	int ret = -1;
	fd_set fds;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	query_struct query;
	reply_struct reply;
	char grp_addr[] = CNW_NAME_SERVICE_GROUP_ADDR;

	q_fd = udp_create_sender();
	if(q_fd == -1)
	{
		exit(EXIT_FAILURE);
	}
	r_fd = udp_create_receiver(NULL, CNW_NAME_SERVICE_REPLY_PORT);
	if(r_fd == -1)
	{
		close(q_fd);
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(grp_addr);
	addr.sin_port = htons(CNW_NAME_SERVICE_QUERY_PORT);

	query.device_type = device_type;
	sendto(q_fd, &query, sizeof(query), 0, (struct sockaddr *)&addr, addr_len);
	FD_ZERO(&fds);
	FD_SET(r_fd, &fds);

	timeout.tv_usec = 0;
	timeout.tv_sec = WAIT_REPLY_TIMEOUT;
	printf("IP\tHostname\tMac\tType\n");
	printf(">>>>>>>>\n");

	while(select(r_fd + 1, &fds, NULL, NULL, &timeout) > 0)
	{
		ret = recvfrom(r_fd, &reply, sizeof(reply), 0, (struct sockaddr *)&addr, &addr_len);
		if(ret == -1)
		{
			fprintf(stderr, "recvfrom error (%d)\n", errno);
			close(r_fd);
			close(q_fd);
			exit(EXIT_FAILURE);
		}else if(ret == 0)
		{
			fprintf(stderr, "peer shutdowned");
			break;
		}else
		{
			printf("%s\t", inet_ntoa(addr.sin_addr));
			printf("%s\t", reply.device_name);
			printf("%s\t", reply.device_mac);
			if(reply.device_type == Type_Host)
			{
				printf("Type_Host\t");
			}
			else if(reply.device_type == Type_Client)
			{
				printf("Type_Client\t");
			}
			printf("\n");
		}
	}
	printf("<<<<<<<\n");
	close(r_fd);
	close(q_fd);
	exit(EXIT_SUCCESS);
}

int main(int argc,char **argv)
{
	CNW_Device_Type device_type = Type_Any;
	char *device_bane;
	struct option longopts[] = {
			{"type",	required_argument,	NULL, 't'},
			{NULL,		0,		NULL,  0}
		};
	enum{
		cmd_query,
		cmd_help
	}cmd = cmd_query;
	while(1)
	{
		int c;
		int index = 0;
		c = getopt_long(argc, argv, "t:f:", longopts, &index);

		if(c == -1)
			break;
		switch(c)
		{
		case 't':
			if (strncmp(optarg, "host", 4) == 0)
			{
				device_type = Type_Host;
			}
			else if (strncmp(optarg, "client", 6) == 0)
			{
				device_type = Type_Client;
			}
			break;
		case '?':
			cmd = cmd_help;
			break;
		default:
			fprintf(stderr, "getopt error (%d)", c);
		}
	}
	if(cmd == cmd_query)
	{
		printf("device_type = %d\n", device_type);
	}
	switch(cmd)
	{
	case cmd_query:
		do_query(device_type);
	case cmd_help:
		break;
	default:
		fprintf(stderr, "unknown cmd\n");
	}
	return 0;
}
