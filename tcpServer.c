#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>


#define SERVER_PORT 5420
#define MAX_LINE 256
#define MAX_PENDING 5
#define MAXNAME 256
struct packet {
	short type;
	char uName[MAXNAME];
	char mName[MAXNAME];
	char data[MAXNAME];
};

struct registrationTable {
	int port;
	int sockid;
	char mName[MAXNAME];
	char uName[MAXNAME];
};
int main(int argc, char* argv[])
{
	struct packet packet_reg, packet_chat, packet_conf, packet_resp;
	struct registrationTable table[10];
	char clientname[MAXNAME];


	struct sockaddr_in sin;
	struct sockaddr_in clientAddr;
	char buf[MAX_LINE];
	int s, new_s;
	int len;

	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("tcpserver: socket");
		exit(1);
	}

	/* build address data structure */
	bzero((char*)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);


	if (bind(s, (struct sockaddr*) & sin, sizeof(sin)) < 0) {
		perror("tcpclient: bind");
		exit(1);
	}
	listen(s, MAX_PENDING);



	/* wait for connection, then receive and print text */
	while (1) {
		if ((new_s = accept(s, (struct sockaddr*) & clientAddr, &len)) < 0) {
			perror("tcpserver: accept");
			exit(1);
		}

		printf("\n Server: Client Connection Established \n");


		/*receive registration packet*/
		if (recv(new_s, &packet_reg, sizeof(packet_reg), 0) < 0) {
			printf("\n Could not receive first registration packet \n");
			exit(1);
		}
		else {
			printf("\n Server: Registration packet received \n");
		}

		/*create conf packet*/
		packet_conf.type = htons(221);
		strcpy(packet_conf.mName, clientname);

		/*send response*/
		if (send(new_s, &packet_conf, sizeof(packet_conf), 0) < 0) {
			printf("\n Send failed\n");
			exit(1);
		}
		else {
			printf("\n Server: Sent confirmation of received chat data \n");
		}

		/*construct chat response*/
		packet_resp.type = htons(231);
		strcpy(packet_chat.data, buf);

		while (1) {
			fputs(buf, stdout);

			/*receive chat packet*/
			if (recv(new_s, &packet_chat, sizeof(packet_chat), 0) < 0) {
				printf("\n received failed\n");
				exit(1);
			}
			else {
				printf("\n Server: Chat packet received \n");
				printf("\n Message: %s\n", packet_chat.data);
			}


			/*send response*/
			if (send(new_s, &packet_resp, sizeof(packet_resp), 0) < 0) {
				printf("\n Send failed\n");
				exit(1);
			}
			else {
				printf("\n Server: Response sent \n");
			}
		}
		close(new_s);

	}
}
