#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

char quitCommand[] = "_quit";

void *recMsg(void *sock) {
	int length;
	int t_sock = *((int *)sock);
	char message[500];	
	while((length = recv(t_sock,message,500,0)) > 0) {
		message[length] = '\0';
		fputs(message,stdout);
		memset(message,'\0',sizeof(message));
	}
    exit(1);
}

int connectToServer(char nom[], char machine[], char port[]) {
	struct sockaddr_in their_addr;
	int my_sock;
	int their_addr_size;
	int portno;
	pthread_t sendt,recvt;
	char message[500];
	char username[100];
	char res[600];
	char ip[INET_ADDRSTRLEN];
	int length;

	portno = atoi(port);
	strcpy(username, nom);
	my_sock = socket(AF_INET,SOCK_STREAM,0);
	memset(their_addr.sin_zero,'\0',sizeof(their_addr.sin_zero));

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(portno);
	their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(my_sock,(struct sockaddr *)&their_addr,sizeof(their_addr)) < 0) {
		perror("connection not esatablished");
		exit(1);
	}
	inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
    
    length = write(my_sock, username, strlen(username));
    if (length < 0) {
        printf("Username is not sent to the server\n");
    }
    else {
    }
	pthread_create(&recvt,NULL,recMsg,&my_sock);
    
    memset(message, 0x00, sizeof(message));
    memset(res, 0x00, sizeof(res));
	while(fgets(message,500,stdin) > 0) {
		strcat(res,message);
		length = write(my_sock,res,strlen(res));
		if(length < 0) {
			perror("message not sent");
			exit(1);
		}
        if (strlen(message) == strlen(quitCommand) + 1 && message[0] == '_' && message[1] == 'q' && message[2] == 'u' && message[3] == 'i' && message[4] == 't') {
            exit(1);
        }
        memset(message, 0x00, sizeof(message));
        memset(res, 0x00, sizeof(res));
	}
	pthread_join(recvt,NULL);
	close(my_sock);
}

int main(int argc, char**argv) {
    char command[2000];
    char connect[2000];
    char nom[2000];
    char machine[2000];
    char port[2000];
    char space;
        
    printf("Welcome guys to ChatRoom :)\n");
    printf("Type '_connect <surnom> <machine> <port>' and connect to your server now.\n");
    printf("Type '_quit' to disoconnect from your server.\n");
    fputs("", stdout);
    fgets(command, sizeof (command), stdin);
    
    if (command[0] == '_' && command[1] == 'q' && command[2] == 'u' && command[3] == 'i' && command[4] == 't') {
        return 0;
    }
    else {
        if (command[0] == '_' && command[1] == 'c' && command[2] == 'o' && command[3] == 'n' && command[4] == 'n' && command[5] == 'e' && command[6] == 'c' && command[7] == 't') {
            
            sscanf(command, "%s%c%s%c%s%c%s", connect, &space, nom, &space, machine, &space, port);          
            connectToServer(nom, machine, port);
        }
        else {
            printf("GoodBye.\n");
            return 0;
        }
    }
}
