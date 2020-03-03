#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define SERVER_PORT 5420
#define MAX_LINE 256
#define MAXNAME 256


int main(int argc, char* argv[])
{
	/*structure of the packet*/
	struct packet {
		short type;
		char uName[MAXNAME];
		char mName[MAXNAME];
		char data[MAXNAME];
	};

	/* variables */
	struct packet packet_reg;
	struct packet packet_conf;
	struct packet packet_chat;
	struct packet packet_resp;
	char clientname[MAXNAME];
	gethostname(clientname, sizeof(clientname));



	/* more variables */
	struct hostent* hp;
	struct sockaddr_in sin;
	char* host;
	char buf[MAX_LINE];
	int s, new_s;
	int len;


	if (argc == 2) {
		host = argv[1];
	}
	else {
		fprintf(stderr, "usage:newclient server\n");
		exit(1);
	}



	/* translate host name into peer's IP address */
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "unkown host: %s\n", host);
		exit(1);
	}

	/* active open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("tcpclient: socket");
		exit(1);
	}

	/* build address data structure */
	bzero((char*)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char*)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);


	//check to see if the client program has connected to the server. If it
	if (connect(s, (struct sockaddr*) & sin, sizeof(sin)) < 0)
	{
		perror("tcpclient: connect");
		close(s);
		exit(1);
	}
	else
	{
		printf("\nConenction established");
	}



	/*Constructing the registration packet at client*/
	packet_reg.type = htons(121);
	strcpy(packet_reg.mName, clientname);


	/*send the registration packet to server*/
	if (send(s, &packet_reg, sizeof(packet_reg), 0) < 0) {
		printf("\n Send failed\n");
		exit(1);
	}
	else {
		printf("\n Client: Registration packet sent \n");
	}


	/*receive confirmation packet*/
	if (recv(s, &packet_conf, sizeof(packet_conf), 0) < 0) {
		printf("\n Could not receive confirmation packet \n");
		exit(1);
	}
	else {
		printf("\n Client: Received confirmation packet \n");
		printf("\n Message: %s", packet_chat.data);
	}


	/*construct chat packet*/
	packet_chat.type = htons(131);
	strcpy(packet_chat.mName, clientname);


	/* main loop: get and send lines of text */
	while (fgets(buf, sizeof(buf), stdin)) {
		buf[MAX_LINE - 1] = '\0';
		len = strlen(buf) + 1;
		send(s, buf, len, 0);

		strcpy(packet_chat.data, buf);

		/*send chat*/
		if (send(s, &packet_chat, sizeof(packet_chat), 0) < 0) {
			printf("\n Send failed\n");
			exit(1);
		}


		/*receive chat response*/
		if (recv(s, &packet_resp, sizeof(packet_resp), 0) < 0) {
			printf("\n Could not receive confirmation packet \n");
			exit(1);
		}
	}
}
