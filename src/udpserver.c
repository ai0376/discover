#include "udpcommon.h"
#include <sys/ioctl.h>
#include <net/if.h>

CNW_Device_Type get_dev_type()
{
	char *s = getenv("DEV_TYPE");
	if(s)
	{
		if(strcmp(s , "MII_TX") == 0)
		{
			return Type_Host;
		}
		else if(strcmp(s , "MII_RX") == 0)
		{
			return Type_Client;
		}
		else
		{
			return Type_Unknown;
		}
	}
	else
	{
		return Type_Unknown;
	}
}

static int get_device_mac(char *device_mac, int len)
{
	struct ifreq ifreq;
	int sock;
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return -1;
	}
	strcpy(ifreq.ifr_name, "eth0");
	if(ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0)
	{
		perror("ioctl");
		return -1;
	}
	close(sock);
	return snprintf(device_mac, len, "%02X:%02X:%02X:%02X:%02X:%02X",(unsigned char)ifreq.ifr_hwaddr.sa_data[0], (unsigned char)ifreq.ifr_hwaddr.sa_data[1], (unsigned char)ifreq.ifr_hwaddr.sa_data[2], (unsigned char)ifreq.ifr_hwaddr.sa_data[3], (unsigned char)ifreq.ifr_hwaddr.sa_data[4], (unsigned char)ifreq.ifr_hwaddr.sa_data[5]);
}
static void udphand()
{
	int q_fd, r_fd;
	int ret = -1;
	struct sockaddr_in addr_c;
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
	addr.sin_family = AF_INET;
	//addr.sin_addr.s_addr = inet_addr(grp_addr);
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(CNW_NAME_SERVICE_SEND_PORT);
	r_fd = udp_create_receiver(CNW_NAME_SERVICE_GROUP_ADDR, CNW_NAME_SERVICE_RECEIVE_PORT);
	if(r_fd == -1)
	{
		close(q_fd);
		exit(EXIT_FAILURE);
	}
	memset(&addr_c, 0, sizeof(addr_c));
	int n = 0;
	char computer[BUFF_LEN]={0};
	while(1)
	{
		memset(&query, 0, sizeof(query));
		if((n=recvfrom(r_fd ,&query, sizeof(query) ,0,(struct   sockaddr*)&addr_c,&addr_len)) < 0)
		{
			perror( "recvfrom ");
			close(r_fd);
			close(q_fd);
			exit(EXIT_FAILURE);
			return;
		}
		else
		{
			memset(computer, 0, BUFF_LEN);
			gethostname(computer, BUFF_LEN);	//get device name
			strcpy(reply.device_name, computer);	//copy device name
			get_device_mac(reply.device_mac, MAC_LEN); // device_mac

			CNW_Device_Type type = get_dev_type();

			addr.sin_addr.s_addr = addr_c.sin_addr.s_addr;
			if(query.device_type == type )
			{
				reply.device_type = type;
				sendto(q_fd, &reply, sizeof(reply), 0, (struct sockaddr *)&addr, addr_len);
			}
			else if(query.device_type == Type_Any)
			{
				reply.device_type = type;
				sendto(q_fd, &reply, sizeof(reply), 0, (struct sockaddr *)&addr, addr_len);
			}
			else
			{
				//reply.device_type = Type_Unknown;
				//sendto(q_fd, &reply, sizeof(reply), 0, (struct sockaddr *)&addr, addr_len);
			}

		}
	}
	close(r_fd);
	close(q_fd);
	exit(EXIT_SUCCESS);
}

int main(int argc,char **argv)
{
	udphand();
	return 0;
}
