#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

struct client_info {
    int ID;
    char username[100];
    char ip[INET_ADDRSTRLEN];
	int sockn;
};

struct client_info clients[100];
int n = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void share(char *message) {
    int a;
    for(a = 0; a < n; a++) {
        if(send(clients[a].sockn,message,strlen(message),0) < 0) {
            perror("sending failure");
            continue;
        }
    }   
}

void killConnectionWith(char surnom[]) {
    pthread_mutex_lock(&mutex);
    char res[600];
    int i,j,find = -1;
    memset(res, 0x00, sizeof(res));
    for(i = 0; i < n; i++) {
        if(strcmp(clients[i].username, surnom) == 0) {
            printf("client founded: %s\n", clients[i].username);
            find = i;
            strcpy(res, surnom);
            strcat(res," quit the group\n");
            res[strlen(res)] = '\0';
        }
    }
    if (find > -1) {
        close(clients[find].sockn);
        for(i = 0; i < n; i++) {
            if(clients[i].sockn == clients[find].sockn) {
                j = i;
                while(j < n-1) {
                    clients[j] = clients[j+1];
                    j++;
                }
            }
        }
        n--;
        share(res);
        memset(res, 0x00, sizeof(res));
    }
    else {
        printf("This username doesn't exist\n");
    }
    pthread_mutex_unlock(&mutex);
}

char shdn[] = "_shdn";

void *keyb() {
    int length;
    char kill[500],space,surnom[100],message[500];
    while(fgets(message,500,stdin) > 0) {
        if(length < 0) {
            perror("no message was sent");
            exit(1);
        }
        if (strlen(message) == strlen(shdn) + 1 && message[0] == '_' && message[1] == 's' && message[2] == 'h' && message[3] == 'd' && message[4] == 'n') {
            exit(1);
        }
        else {
            if (message[0] == '_' && message[1] == 'k' && message[2] == 'i' && message[3] == 'l' && message[4] == 'l') {
                sscanf(message, "%s%c%s", kill, &space, surnom);
                killConnectionWith(surnom);
            }
        }
        memset(message,'\0',sizeof(message));
    }
}

void sendToAllClient(char *message,int curr) {
    int i;

    for(i = 0; i < n; i++) {
        if(clients[i].sockn != curr) {
            if(send(clients[i].sockn,message,strlen(message),0) < 0) {
                perror("failure sending message");
                continue;
            }
        }
    }
}


char quit[] = "_quit";
char who[] = "_who";

void *receiveMessage(void *sock) {
	struct client_info client = *((struct client_info *)sock);
	char message[500],res[600];
	int length,i,j;
    memset(message, 0x00, sizeof(message));
    memset(res, 0x00, sizeof(res));
	while((length = recv(client.sockn,message,500,0)) > 0) {
        if (strlen(message) == strlen(who) + 1 && message[0] == '_' && message[1] == 'w' && message[2] == 'h'
                 && message[3] == 'o') {
            memset(res, 0x00, sizeof(res));
            for(i = 0; i < n; i++) {
                if (i == n - 1) {
                    strcat(res, clients[i].username);
                }
                else {
                    strcat(res, clients[i].username);
                    strcat(res, ", ");
                }
            }
            strcat(res, "\n");
            res[strlen(res)] = '\0';
            if(send(client.sockn,res,strlen(res),0) < 0) {
                perror("sending failure");
            }
            memset(message, 0x00, sizeof(message));
            memset(res, 0x00, sizeof(res));
        }
        else if (strlen(message) == strlen(quit) + 1 && message[0] == '_' && message[1] == 'q' && message[2] == 'u' && message[3] == 'i' && message[4] == 't') {
            
        }
        else {
            strcpy(res, client.username);
            strcat(res,": ");
            strcat(res,message);
            message[length] = '\0';
            res[strlen(res)] = '\0';
            sendToAllClient(res,client.sockn);
            memset(message, 0x00, sizeof(message));
            memset(res, 0x00, sizeof(res));
        }
	}
	pthread_mutex_lock(&mutex);
    strcpy(res, client.username);
    strcat(res, " quit the group\n");
    res[strlen(res)] = '\0';
	printf("%s disconnected\n",client.username);
	for(i = 0; i < n; i++) {
		if(clients[i].sockn == client.sockn) {
			j = i;
			while(j < n-1) {
				clients[j] = clients[j+1];
				j++;
			}
		}
	}
	n--;
    share(res);
    memset(message,'\0',sizeof(message));
    memset(res,'\0',sizeof(res));
	pthread_mutex_unlock(&mutex);
}

int main(int argc,char *argv[]) {
    printf("Server start ...)\n");
    printf("Wait for client to connect...if you want to close server just type :' _shdn ')\n");

	struct sockaddr_in my_addresse,t_addr;
	int my_socket,t_sock,portno,length;
	socklen_t taddresse_size;
	pthread_t recvt, keybThread;
    char res[600],ip[INET_ADDRSTRLEN];
    struct client_info client;
	if(argc > 2) {
		printf("too many arguments ...");
		exit(1);
	}

	portno = atoi(argv[1]);

	my_socket = socket(AF_INET,SOCK_STREAM,0);
	memset(my_addresse.sin_zero,'\0',sizeof(my_addresse.sin_zero));

	my_addresse.sin_family = AF_INET;
	my_addresse.sin_port = htons(portno);
	my_addresse.sin_addr.s_addr = inet_addr("127.0.0.1");
	taddresse_size = sizeof(t_addr);

	if(bind(my_socket,(struct sockaddr *)&my_addresse,sizeof(my_addresse)) != 0) {
		perror("binding unsuccessful !!");
		exit(1);
	}

	if(listen(my_socket,5) != 0) {
		perror("listening unsuccessful !!");
		exit(1);
	}

    pthread_create(&keybThread, NULL, keyb, NULL);
    
    memset(res, 0x00, sizeof(res));
	while(1) {
		if((t_sock = accept(my_socket,(struct sockaddr *)&t_addr,&taddresse_size)) < 0) {
			perror("accept unsuccessful !!");
			exit(1);
		}
		pthread_mutex_lock(&mutex);
		inet_ntop(AF_INET, (struct sockaddr *)&t_addr, ip, INET_ADDRSTRLEN);
		printf("%s connected\n",ip);
		client.sockn = t_sock;
		strcpy(client.ip,ip);
		clients[n].sockn = t_sock;
        clients[n].ID = n+1;
        strcpy(clients[n].ip,ip);
        
        memset(clients[n].username, 0x00, sizeof(clients[n].username));
        length = recv(clients[n].sockn, clients[n].username, 100, 0);
        while (length < 0) {
            length = recv(clients[n].sockn, clients[n].username, 100, 0);
        }
        printf("User: %s\n", clients[n].username);
        clients[n].username[strlen(clients[n].username)] = '\0';
        strcpy(res, clients[n].username);
        strcat(res," has been added to the group\n");
        res[strlen(res)] = '\0';
        sendToAllClient(res, clients[n].sockn);
        memset(res, 0x00, sizeof(res));
		pthread_create(&recvt,NULL,receiveMessage,&clients[n]);
        n++;
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}
